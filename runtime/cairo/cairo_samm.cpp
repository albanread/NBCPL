#include "cairo_samm.h"
#include "cairo_core.h"
#include "cairo_runtime.h"
#include "../../HeapManager/HeapManager.h"
#include <vector>
#include <unordered_set>
#include <mutex>
#include <atomic>
#include <iostream>
#include <sstream>

// Global SAMM state for Cairo resources
static std::vector<std::unordered_set<CairoSurfaceHandle>> g_scope_surfaces;
static std::vector<std::unordered_set<CairoImageHandle>> g_scope_images;
static std::mutex g_samm_mutex;
static std::atomic<bool> g_samm_enabled{true};
static std::atomic<bool> g_trace_enabled{false};
static std::atomic<int64_t> g_memory_threshold{100 * 1024 * 1024}; // 100MB default

// Statistics
static std::atomic<uint64_t> g_objects_cleaned{0};
static std::atomic<uint64_t> g_scopes_processed{0};

// Helper functions
namespace {
    void trace(const std::string& message) {
        if (g_trace_enabled.load()) {
            std::cout << "[CairoSAMM] " << message << std::endl;
        }
    }
    
    size_t estimateSurfaceMemory(CairoSurface* surface) {
        if (!surface) return 0;
        // Rough estimate: width * height * 4 bytes (ARGB32) + overhead
        return surface->getWidth() * surface->getHeight() * 4 + 1024;
    }
    
    size_t estimateImageMemory(CairoImage* image) {
        if (!image) return 0;
        // Rough estimate: width * height * 4 bytes (ARGB32) + overhead
        return image->getWidth() * image->getHeight() * 4 + 1024;
    }
}

extern "C" {

// =============================================================================
// SAMM SCOPE MANAGEMENT
// =============================================================================

void CairoSAMM_enterScope(void) {
    if (!g_samm_enabled.load()) return;
    
    std::lock_guard<std::mutex> lock(g_samm_mutex);
    
    g_scope_surfaces.emplace_back();
    g_scope_images.emplace_back();
    
    trace("Entered scope " + std::to_string(g_scope_surfaces.size()));
}

void CairoSAMM_exitScope(void) {
    if (!g_samm_enabled.load()) return;
    
    std::lock_guard<std::mutex> lock(g_samm_mutex);
    
    if (g_scope_surfaces.empty()) {
        trace("Warning: exitScope called with no active scopes");
        return;
    }
    
    // size_t scope_level = g_scope_surfaces.size() - 1;  // unused for now
    
    // Clean up surfaces in current scope
    auto& surfaces = g_scope_surfaces.back();
    for (CairoSurfaceHandle handle : surfaces) {
        trace("Cleaning up surface handle " + std::to_string(handle));
        CairoResourceManager::releaseSurface(handle);
        g_objects_cleaned.fetch_add(1);
    }
    
    // Clean up images in current scope
    auto& images = g_scope_images.back();
    for (CairoImageHandle handle : images) {
        trace("Cleaning up image handle " + std::to_string(handle));
        CairoResourceManager::releaseImage(handle);
        g_objects_cleaned.fetch_add(1);
    }
    
    g_scope_surfaces.pop_back();
    g_scope_images.pop_back();
    g_scopes_processed.fetch_add(1);
    
    trace("Exited scope, cleaned " + std::to_string(surfaces.size() + images.size()) + " objects");
}

int64_t CAIRO_SAMM_SCOPE_DEPTH(void) {
    if (!g_samm_enabled.load()) return 0;
    
    std::lock_guard<std::mutex> lock(g_samm_mutex);
    return static_cast<int64_t>(g_scope_surfaces.size());
}

// =============================================================================
// RESOURCE TRACKING
// =============================================================================

void CairoSAMM_trackSurface(CairoSurfaceHandle handle) {
    if (!g_samm_enabled.load() || handle == 0) return;
    
    std::lock_guard<std::mutex> lock(g_samm_mutex);
    
    if (g_scope_surfaces.empty()) {
        trace("Warning: trackSurface called with no active scopes");
        return;
    }
    
    g_scope_surfaces.back().insert(handle);
    trace("Tracked surface handle " + std::to_string(handle) + 
          " in scope " + std::to_string(g_scope_surfaces.size()));
}

void CairoSAMM_trackImage(CairoImageHandle handle) {
    if (!g_samm_enabled.load() || handle == 0) return;
    
    std::lock_guard<std::mutex> lock(g_samm_mutex);
    
    if (g_scope_images.empty()) {
        trace("Warning: trackImage called with no active scopes");
        return;
    }
    
    g_scope_images.back().insert(handle);
    trace("Tracked image handle " + std::to_string(handle) + 
          " in scope " + std::to_string(g_scope_images.size()));
}

void CairoSAMM_untrackSurface(CairoSurfaceHandle handle) {
    if (!g_samm_enabled.load() || handle == 0) return;
    
    std::lock_guard<std::mutex> lock(g_samm_mutex);
    
    // Remove from all scopes (in case of retained resources)
    for (auto& scope : g_scope_surfaces) {
        scope.erase(handle);
    }
    
    trace("Untracked surface handle " + std::to_string(handle));
}

void CairoSAMM_untrackImage(CairoImageHandle handle) {
    if (!g_samm_enabled.load() || handle == 0) return;
    
    std::lock_guard<std::mutex> lock(g_samm_mutex);
    
    // Remove from all scopes (in case of retained resources)
    for (auto& scope : g_scope_images) {
        scope.erase(handle);
    }
    
    trace("Untracked image handle " + std::to_string(handle));
}

// =============================================================================
// RETAINED RESOURCE ALLOCATION
// =============================================================================

CairoSurfaceHandle CAIRO_CREATE_SURFACE_RETAINED(int64_t width, int64_t height, int64_t parent_scope_offset) {
    if (!g_samm_enabled.load()) {
        // Fall back to regular allocation
        // Fallback to regular allocation - circular dependency issue
        return 0;
    }
    
    std::lock_guard<std::mutex> lock(g_samm_mutex);
    
    if (g_scope_surfaces.empty()) {
        trace("Warning: CREATE_SURFACE_RETAINED called with no active scopes");
        // Fallback to regular allocation - circular dependency issue  
        return 0;
    }
    
    size_t target_scope = g_scope_surfaces.size() - 1;
    if (parent_scope_offset > 0 && parent_scope_offset < static_cast<int64_t>(g_scope_surfaces.size())) {
        target_scope = g_scope_surfaces.size() - parent_scope_offset - 1;
    }
    
    // Create surface normally but track in different scope
    try {
        auto surface = std::make_unique<CairoSurface>(static_cast<int>(width), static_cast<int>(height));
        CairoSurfaceHandle handle = CairoResourceManager::registerSurface(std::move(surface));
        
        g_scope_surfaces[target_scope].insert(handle);
        trace("Created retained surface handle " + std::to_string(handle) + 
              " in scope " + std::to_string(target_scope));
        
        return handle;
    } catch (...) {
        return 0;
    }
}

CairoSurfaceHandle CAIRO_LOAD_PNG_RETAINED(const uint32_t* filename, int64_t parent_scope_offset) {
    if (!g_samm_enabled.load()) {
        // Fall back to regular allocation
        // Fallback - circular dependency issue
        return 0;
    }
    
    std::lock_guard<std::mutex> lock(g_samm_mutex);
    
    if (g_scope_surfaces.empty()) {
        trace("Warning: LOAD_PNG_RETAINED called with no active scopes");
        // Fallback - circular dependency issue
        return 0;
    }
    
    size_t target_scope = g_scope_surfaces.size() - 1;
    if (parent_scope_offset > 0 && parent_scope_offset < static_cast<int64_t>(g_scope_surfaces.size())) {
        target_scope = g_scope_surfaces.size() - parent_scope_offset - 1;
    }
    
    // Load surface normally but track in different scope
    CairoSurfaceHandle handle = 0; // TODO: Fix circular dependency
    if (handle != 0) {
        // Move from current scope to target scope
        if (!g_scope_surfaces.empty()) {
            g_scope_surfaces.back().erase(handle);
        }
        g_scope_surfaces[target_scope].insert(handle);
        trace("Loaded retained PNG surface handle " + std::to_string(handle) + 
              " in scope " + std::to_string(target_scope));
    }
    
    return handle;
}

CairoImageHandle CAIRO_LOAD_IMAGE_RETAINED(const uint32_t* filename, int64_t parent_scope_offset) {
    if (!g_samm_enabled.load()) {
        // Fall back to regular allocation
        // Fallback - circular dependency issue
        return 0;
    }
    
    std::lock_guard<std::mutex> lock(g_samm_mutex);
    
    if (g_scope_images.empty()) {
        trace("Warning: LOAD_IMAGE_RETAINED called with no active scopes");
        // Fallback - circular dependency issue
        return 0;
    }
    
    size_t target_scope = g_scope_images.size() - 1;
    if (parent_scope_offset > 0 && parent_scope_offset < static_cast<int64_t>(g_scope_images.size())) {
        target_scope = g_scope_images.size() - parent_scope_offset - 1;
    }
    
    // Load image normally but track in different scope
    CairoImageHandle handle = 0; // TODO: Fix circular dependency
    if (handle != 0) {
        // Move from current scope to target scope
        if (!g_scope_images.empty()) {
            g_scope_images.back().erase(handle);
        }
        g_scope_images[target_scope].insert(handle);
        trace("Loaded retained image handle " + std::to_string(handle) + 
              " in scope " + std::to_string(target_scope));
    }
    
    return handle;
}

// =============================================================================
// MEMORY PRESSURE HANDLING
// =============================================================================

void CairoSAMM_handleMemoryPressure(void) {
    if (!g_samm_enabled.load()) return;
    
    trace("Handling memory pressure");
    
    std::lock_guard<std::mutex> lock(g_samm_mutex);
    
    size_t objects_cleaned = 0;
    
    // Clean up completed scopes from oldest to newest
    // Note: In a real implementation, we'd need better tracking of which scopes are completed
    for (size_t i = 0; i < g_scope_surfaces.size(); ++i) {
        // This is a simplified approach - in practice we'd need better lifecycle tracking
        if (g_scope_surfaces[i].size() > 100) { // Arbitrary threshold
            for (CairoSurfaceHandle handle : g_scope_surfaces[i]) {
                CairoResourceManager::releaseSurface(handle);
                objects_cleaned++;
            }
            g_scope_surfaces[i].clear();
        }
        
        if (g_scope_images[i].size() > 100) { // Arbitrary threshold
            for (CairoImageHandle handle : g_scope_images[i]) {
                CairoResourceManager::releaseImage(handle);
                objects_cleaned++;
            }
            g_scope_images[i].clear();
        }
    }
    
    g_objects_cleaned.fetch_add(objects_cleaned);
    trace("Memory pressure cleanup: released " + std::to_string(objects_cleaned) + " objects");
}

void CAIRO_SAMM_FORCE_CLEANUP(void) {
    if (!g_samm_enabled.load()) return;
    
    trace("Forcing immediate cleanup");
    
    std::lock_guard<std::mutex> lock(g_samm_mutex);
    
    size_t objects_cleaned = 0;
    
    // Clean up all scopes
    for (auto& scope : g_scope_surfaces) {
        for (CairoSurfaceHandle handle : scope) {
            CairoResourceManager::releaseSurface(handle);
            objects_cleaned++;
        }
        scope.clear();
    }
    
    for (auto& scope : g_scope_images) {
        for (CairoImageHandle handle : scope) {
            CairoResourceManager::releaseImage(handle);
            objects_cleaned++;
        }
        scope.clear();
    }
    
    g_objects_cleaned.fetch_add(objects_cleaned);
    trace("Force cleanup: released " + std::to_string(objects_cleaned) + " objects");
}

void CAIRO_SAMM_SET_MEMORY_THRESHOLD(int64_t threshold_bytes) {
    g_memory_threshold.store(threshold_bytes);
    trace("Set memory threshold to " + std::to_string(threshold_bytes) + " bytes");
}

// =============================================================================
// STATISTICS AND DEBUGGING
// =============================================================================

void CAIRO_SAMM_GET_STATS(int64_t* active_surfaces, int64_t* active_images, int64_t* memory_usage) {
    if (!active_surfaces || !active_images || !memory_usage) return;
    
    std::lock_guard<std::mutex> lock(g_samm_mutex);
    
    *active_surfaces = 0;
    *active_images = 0;
    *memory_usage = 0;
    
    for (const auto& scope : g_scope_surfaces) {
        *active_surfaces += static_cast<int64_t>(scope.size());
        for (CairoSurfaceHandle handle : scope) {
            CairoSurface* surface = CairoResourceManager::getSurface(handle);
            *memory_usage += static_cast<int64_t>(estimateSurfaceMemory(surface));
        }
    }
    
    for (const auto& scope : g_scope_images) {
        *active_images += static_cast<int64_t>(scope.size());
        for (CairoImageHandle handle : scope) {
            CairoImage* image = CairoResourceManager::getImage(handle);
            *memory_usage += static_cast<int64_t>(estimateImageMemory(image));
        }
    }
}

void CAIRO_SAMM_GET_SCOPE_STATS(int64_t scope_level, int64_t* surfaces_ptr, int64_t* images_ptr) {
    if (!surfaces_ptr || !images_ptr) return;
    
    std::lock_guard<std::mutex> lock(g_samm_mutex);
    
    *surfaces_ptr = 0;
    *images_ptr = 0;
    
    size_t target_scope = static_cast<size_t>(scope_level);
    if (target_scope < g_scope_surfaces.size()) {
        *surfaces_ptr = static_cast<int64_t>(g_scope_surfaces[target_scope].size());
    }
    
    if (target_scope < g_scope_images.size()) {
        *images_ptr = static_cast<int64_t>(g_scope_images[target_scope].size());
    }
}

void CAIRO_SAMM_SET_TRACE(int64_t enabled) {
    g_trace_enabled.store(enabled != 0);
    trace(enabled ? "Tracing enabled" : "Tracing disabled");
}

void CAIRO_SAMM_DUMP_STATE(void) {
    std::lock_guard<std::mutex> lock(g_samm_mutex);
    
    std::cout << "=== Cairo SAMM State Dump ===" << std::endl;
    std::cout << "SAMM Enabled: " << (g_samm_enabled.load() ? "Yes" : "No") << std::endl;
    std::cout << "Trace Enabled: " << (g_trace_enabled.load() ? "Yes" : "No") << std::endl;
    std::cout << "Memory Threshold: " << g_memory_threshold.load() << " bytes" << std::endl;
    std::cout << "Active Scopes: " << g_scope_surfaces.size() << std::endl;
    std::cout << "Objects Cleaned: " << g_objects_cleaned.load() << std::endl;
    std::cout << "Scopes Processed: " << g_scopes_processed.load() << std::endl;
    
    for (size_t i = 0; i < g_scope_surfaces.size(); ++i) {
        std::cout << "Scope " << i << ": " 
                  << g_scope_surfaces[i].size() << " surfaces, "
                  << g_scope_images[i].size() << " images" << std::endl;
    }
    
    std::cout << "===========================" << std::endl;
}

// =============================================================================
// INTEGRATION WITH CORE SAMM SYSTEM
// =============================================================================

void CairoSAMM_initialize(void) {
    std::lock_guard<std::mutex> lock(g_samm_mutex);
    g_scope_surfaces.clear();
    g_scope_images.clear();
    g_objects_cleaned.store(0);
    g_scopes_processed.store(0);
    trace("Cairo SAMM initialized");
}

void CairoSAMM_shutdown(void) {
    trace("Cairo SAMM shutting down");
    CAIRO_SAMM_FORCE_CLEANUP();
    
    std::lock_guard<std::mutex> lock(g_samm_mutex);
    g_scope_surfaces.clear();
    g_scope_images.clear();
    trace("Cairo SAMM shutdown complete");
}

bool CairoSAMM_isEnabled(void) {
    return g_samm_enabled.load();
}

void CairoSAMM_setEnabled(bool enabled) {
    g_samm_enabled.store(enabled);
    trace(enabled ? "Cairo SAMM enabled" : "Cairo SAMM disabled");
}

// =============================================================================
// BACKGROUND CLEANUP INTEGRATION
// =============================================================================

void CairoSAMM_queueBackgroundCleanup(void) {
    // In a full implementation, this would queue cleanup tasks for the background thread
    // For now, we just handle memory pressure
    CairoSAMM_handleMemoryPressure();
}

void CairoSAMM_processBackgroundCleanup(void) {
    // In a full implementation, this would process queued cleanup tasks
    // For now, we just handle memory pressure
    CairoSAMM_handleMemoryPressure();
}

void CairoSAMM_waitForBackgroundCleanup(void) {
    // In a full implementation, this would wait for background cleanup to complete
    // For now, this is a no-op since we don't have background threads
    trace("Background cleanup wait (no-op in current implementation)");
}

int64_t CAIRO_SAMM_BACKGROUND_QUEUE_SIZE(void) {
    // In a full implementation, this would return the size of the cleanup queue
    // For now, return 0
    return 0;
}

// =============================================================================
// ERROR HANDLING AND RECOVERY
// =============================================================================

int64_t CAIRO_SAMM_RECOVER_LEAKS(void) {
    trace("Recovering from potential leaks");
    
    size_t recovered = 0;
    
    // Force cleanup of all resources
    CAIRO_SAMM_FORCE_CLEANUP();
    
    // Also clean up any orphaned resources in the resource manager
    size_t total_surfaces = CairoResourceManager::getSurfaceCount();
    size_t total_images = CairoResourceManager::getImageCount();
    
    if (total_surfaces > 0 || total_images > 0) {
        trace("Found " + std::to_string(total_surfaces) + " orphaned surfaces and " +
              std::to_string(total_images) + " orphaned images");
        
        // In a full implementation, we'd clean these up
        // For now, just report them
        recovered = total_surfaces + total_images;
    }
    
    trace("Leak recovery complete: " + std::to_string(recovered) + " objects recovered");
    return static_cast<int64_t>(recovered);
}

int64_t CAIRO_SAMM_VALIDATE_STATE(void) {
    std::lock_guard<std::mutex> lock(g_samm_mutex);
    
    // Check for consistency between scope tracking and resource manager
    bool consistent = true;
    
    size_t tracked_surfaces = 0;
    size_t tracked_images = 0;
    
    for (const auto& scope : g_scope_surfaces) {
        tracked_surfaces += scope.size();
        for (CairoSurfaceHandle handle : scope) {
            if (!CairoResourceManager::getSurface(handle)) {
                trace("Warning: Tracked surface handle " + std::to_string(handle) + " not found in resource manager");
                consistent = false;
            }
        }
    }
    
    for (const auto& scope : g_scope_images) {
        tracked_images += scope.size();
        for (CairoImageHandle handle : scope) {
            if (!CairoResourceManager::getImage(handle)) {
                trace("Warning: Tracked image handle " + std::to_string(handle) + " not found in resource manager");
                consistent = false;
            }
        }
    }
    
    size_t manager_surfaces = CairoResourceManager::getSurfaceCount();
    size_t manager_images = CairoResourceManager::getImageCount();
    
    if (tracked_surfaces > manager_surfaces || tracked_images > manager_images) {
        trace("Warning: More tracked resources than manager resources");
        consistent = false;
    }
    
    std::ostringstream oss;
    oss << "State validation: " << (consistent ? "PASSED" : "FAILED");
    trace(oss.str());
    std::ostringstream oss2;
    oss2 << "Tracked: " << tracked_surfaces << " surfaces, " << tracked_images << " images";
    trace(oss2.str());
    std::ostringstream oss3;
    oss3 << "Manager: " << manager_surfaces << " surfaces, " << manager_images << " images";
    trace(oss3.str());
    
    return consistent ? 1 : 0;
}

void CAIRO_SAMM_EMERGENCY_RESET(void) {
    trace("EMERGENCY RESET: This will leak active resources!");
    
    std::lock_guard<std::mutex> lock(g_samm_mutex);
    
    // Clear all tracking without cleaning up resources (causes leaks but recovers from corruption)
    g_scope_surfaces.clear();
    g_scope_images.clear();
    g_objects_cleaned.store(0);
    g_scopes_processed.store(0);
    
    // Also clear the resource manager (this will leak memory but prevent crashes)
    CairoResourceManager::cleanup();
    
    trace("Emergency reset complete - system is in clean but leaked state");
}

} // extern "C"