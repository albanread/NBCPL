#include "graphics_resources.h"
#include "HeapManager.h"
#include <unordered_map>
#include <vector>
#include <mutex>
#include <atomic>
#include <cstring>
#include <cstdio>
#include <algorithm>

// =============================================================================
// INTERNAL DATA STRUCTURES
// =============================================================================

namespace {
    // Thread-safe handle generator
    std::atomic<uint64_t> g_next_handle_id{1};
    
    // Registry of available graphics backends
    std::unordered_map<std::string, GraphicsBackend> g_backends;
    std::mutex g_backends_mutex;
    
    // Tracked graphics resources by handle ID
    std::unordered_map<uint64_t, GraphicsSurface*> g_tracked_surfaces;
    std::unordered_map<uint64_t, GraphicsImage*> g_tracked_images;
    std::mutex g_resources_mutex;
    
    // Resources organized by SAMM scope depth
    std::unordered_map<int, std::vector<uint64_t>> g_surfaces_by_scope;
    std::unordered_map<int, std::vector<uint64_t>> g_images_by_scope;
    std::mutex g_scopes_mutex;
    
    // Configuration
    bool g_graphics_initialized = false;
    bool g_graphics_trace_enabled = false;
    std::mutex g_config_mutex;
    
    // Statistics
    struct GraphicsStats {
        int64_t total_surfaces = 0;
        int64_t total_images = 0;
        int64_t total_memory = 0;
        std::unordered_map<std::string, int64_t> backend_surfaces;
        std::unordered_map<std::string, int64_t> backend_images;
        std::unordered_map<std::string, int64_t> backend_memory;
    };
    GraphicsStats g_stats;
    std::mutex g_stats_mutex;
    
    void trace_log(const char* format, ...) {
        if (!g_graphics_trace_enabled) return;
        
        va_list args;
        va_start(args, format);
        printf("[SAMM Graphics] ");
        vprintf(format, args);
        va_end(args);
    }
    
    void update_surface_stats(const GraphicsSurface* surface, int delta) {
        std::lock_guard<std::mutex> lock(g_stats_mutex);
        g_stats.total_surfaces += delta;
        g_stats.backend_surfaces[surface->backend_name] += delta;
        
        int64_t memory_delta = static_cast<int64_t>(surface->estimated_memory) * delta;
        g_stats.total_memory += memory_delta;
        g_stats.backend_memory[surface->backend_name] += memory_delta;
    }
    
    void update_image_stats(const GraphicsImage* image, int delta) {
        std::lock_guard<std::mutex> lock(g_stats_mutex);
        g_stats.total_images += delta;
        g_stats.backend_images[image->backend_name] += delta;
        
        int64_t memory_delta = static_cast<int64_t>(image->estimated_memory) * delta;
        g_stats.total_memory += memory_delta;
        g_stats.backend_memory[image->backend_name] += memory_delta;
    }
    
    void cleanup_surface_internal(GraphicsSurface* surface) {
        if (!surface) return;
        
        trace_log("Cleaning up surface handle %llu (%s)\n", 
                  surface->handle_id, surface->backend_name);
        
        // Call backend-specific cleanup
        if (surface->cleanup_func && surface->native_handle) {
            surface->cleanup_func(surface->native_handle);
        }
        
        // Update statistics
        update_surface_stats(surface, -1);
        
        // Free the surface structure itself
        delete surface;
    }
    
    void cleanup_image_internal(GraphicsImage* image) {
        if (!image) return;
        
        trace_log("Cleaning up image handle %llu (%s)\n", 
                  image->handle_id, image->backend_name);
        
        // Call backend-specific cleanup
        if (image->cleanup_func && image->native_handle) {
            image->cleanup_func(image->native_handle);
        }
        
        // Update statistics
        update_image_stats(image, -1);
        
        // Free the image structure itself
        delete image;
    }
}

// =============================================================================
// BACKEND REGISTRATION API
// =============================================================================

extern "C" int SAMM_register_graphics_backend(const GraphicsBackend* backend) {
    if (!backend || !backend->backend_name) {
        return 0;
    }
    
    std::lock_guard<std::mutex> lock(g_backends_mutex);
    
    std::string name(backend->backend_name);
    g_backends[name] = *backend;
    
    trace_log("Registered graphics backend: %s\n", backend->backend_name);
    return 1;
}

extern "C" int SAMM_unregister_graphics_backend(const char* backend_name) {
    if (!backend_name) return 0;
    
    std::lock_guard<std::mutex> lock(g_backends_mutex);
    
    auto it = g_backends.find(backend_name);
    if (it != g_backends.end()) {
        g_backends.erase(it);
        trace_log("Unregistered graphics backend: %s\n", backend_name);
        return 1;
    }
    
    return 0;
}

extern "C" int SAMM_is_graphics_backend_available(const char* backend_name) {
    if (!backend_name) return 0;
    
    std::lock_guard<std::mutex> lock(g_backends_mutex);
    return g_backends.find(backend_name) != g_backends.end() ? 1 : 0;
}

extern "C" int SAMM_list_graphics_backends(const char** backends, int max_backends) {
    std::lock_guard<std::mutex> lock(g_backends_mutex);
    
    if (!backends) {
        return static_cast<int>(g_backends.size());
    }
    
    int count = 0;
    for (const auto& pair : g_backends) {
        if (count >= max_backends) break;
        backends[count++] = pair.first.c_str();
    }
    
    return count;
}

// =============================================================================
// GRAPHICS RESOURCE TRACKING API
// =============================================================================

extern "C" int SAMM_track_graphics_surface(GraphicsSurface* surface) {
    if (!surface || !g_graphics_initialized) {
        return 0;
    }
    
    // Get current SAMM scope depth
    int current_scope = HeapManager::getInstance().getCurrentScopeDepth();
    
    std::lock_guard<std::mutex> resources_lock(g_resources_mutex);
    std::lock_guard<std::mutex> scopes_lock(g_scopes_mutex);
    
    // Store the surface
    g_tracked_surfaces[surface->handle_id] = surface;
    
    // Add to current scope
    g_surfaces_by_scope[current_scope].push_back(surface->handle_id);
    
    // Update statistics
    update_surface_stats(surface, 1);
    
    trace_log("Tracked surface handle %llu in scope %d (%s, %dx%d, %zu bytes)\n",
              surface->handle_id, current_scope, surface->backend_name,
              surface->width, surface->height, surface->estimated_memory);
    
    return 1;
}

extern "C" int SAMM_track_graphics_image(GraphicsImage* image) {
    if (!image || !g_graphics_initialized) {
        return 0;
    }
    
    // Get current SAMM scope depth
    int current_scope = HeapManager::getInstance().getCurrentScopeDepth();
    
    std::lock_guard<std::mutex> resources_lock(g_resources_mutex);
    std::lock_guard<std::mutex> scopes_lock(g_scopes_mutex);
    
    // Store the image
    g_tracked_images[image->handle_id] = image;
    
    // Add to current scope
    g_images_by_scope[current_scope].push_back(image->handle_id);
    
    // Update statistics
    update_image_stats(image, 1);
    
    trace_log("Tracked image handle %llu in scope %d (%s, %dx%d, %zu bytes)\n",
              image->handle_id, current_scope, image->backend_name,
              image->width, image->height, image->estimated_memory);
    
    return 1;
}

extern "C" int SAMM_untrack_graphics_surface(uint64_t handle_id) {
    std::lock_guard<std::mutex> resources_lock(g_resources_mutex);
    std::lock_guard<std::mutex> scopes_lock(g_scopes_mutex);
    
    auto it = g_tracked_surfaces.find(handle_id);
    if (it == g_tracked_surfaces.end()) {
        return 0;
    }
    
    GraphicsSurface* surface = it->second;
    
    // Remove from all scopes
    for (auto& scope_pair : g_surfaces_by_scope) {
        auto& handles = scope_pair.second;
        handles.erase(std::remove(handles.begin(), handles.end(), handle_id), handles.end());
    }
    
    // Remove from tracking
    g_tracked_surfaces.erase(it);
    
    trace_log("Untracked surface handle %llu\n", handle_id);
    
    // Clean up the surface
    cleanup_surface_internal(surface);
    
    return 1;
}

extern "C" int SAMM_untrack_graphics_image(uint64_t handle_id) {
    std::lock_guard<std::mutex> resources_lock(g_resources_mutex);
    std::lock_guard<std::mutex> scopes_lock(g_scopes_mutex);
    
    auto it = g_tracked_images.find(handle_id);
    if (it == g_tracked_images.end()) {
        return 0;
    }
    
    GraphicsImage* image = it->second;
    
    // Remove from all scopes
    for (auto& scope_pair : g_images_by_scope) {
        auto& handles = scope_pair.second;
        handles.erase(std::remove(handles.begin(), handles.end(), handle_id), handles.end());
    }
    
    // Remove from tracking
    g_tracked_images.erase(it);
    
    trace_log("Untracked image handle %llu\n", handle_id);
    
    // Clean up the image
    cleanup_image_internal(image);
    
    return 1;
}

extern "C" int SAMM_move_graphics_surface_to_scope(uint64_t handle_id, int target_scope_offset) {
    if (target_scope_offset <= 0) return 0;
    
    int current_scope = HeapManager::getInstance().getCurrentScopeDepth();
    int target_scope = current_scope - target_scope_offset;
    
    if (target_scope < 0) return 0; // Cannot move to non-existent scope
    
    std::lock_guard<std::mutex> scopes_lock(g_scopes_mutex);
    
    // Remove from current scope
    auto& current_handles = g_surfaces_by_scope[current_scope];
    auto it = std::find(current_handles.begin(), current_handles.end(), handle_id);
    if (it == current_handles.end()) {
        return 0; // Handle not found in current scope
    }
    current_handles.erase(it);
    
    // Add to target scope
    g_surfaces_by_scope[target_scope].push_back(handle_id);
    
    trace_log("Moved surface handle %llu from scope %d to scope %d\n",
              handle_id, current_scope, target_scope);
    
    return 1;
}

extern "C" int SAMM_move_graphics_image_to_scope(uint64_t handle_id, int target_scope_offset) {
    if (target_scope_offset <= 0) return 0;
    
    int current_scope = HeapManager::getInstance().getCurrentScopeDepth();
    int target_scope = current_scope - target_scope_offset;
    
    if (target_scope < 0) return 0; // Cannot move to non-existent scope
    
    std::lock_guard<std::mutex> scopes_lock(g_scopes_mutex);
    
    // Remove from current scope
    auto& current_handles = g_images_by_scope[current_scope];
    auto it = std::find(current_handles.begin(), current_handles.end(), handle_id);
    if (it == current_handles.end()) {
        return 0; // Handle not found in current scope
    }
    current_handles.erase(it);
    
    // Add to target scope
    g_images_by_scope[target_scope].push_back(handle_id);
    
    trace_log("Moved image handle %llu from scope %d to scope %d\n",
              handle_id, current_scope, target_scope);
    
    return 1;
}

// =============================================================================
// RESOURCE LOOKUP AND ACCESS
// =============================================================================

extern "C" GraphicsSurface* SAMM_get_graphics_surface(uint64_t handle_id) {
    std::lock_guard<std::mutex> lock(g_resources_mutex);
    
    auto it = g_tracked_surfaces.find(handle_id);
    return (it != g_tracked_surfaces.end()) ? it->second : nullptr;
}

extern "C" GraphicsImage* SAMM_get_graphics_image(uint64_t handle_id) {
    std::lock_guard<std::mutex> lock(g_resources_mutex);
    
    auto it = g_tracked_images.find(handle_id);
    return (it != g_tracked_images.end()) ? it->second : nullptr;
}

extern "C" int SAMM_is_graphics_surface_valid(uint64_t handle_id) {
    std::lock_guard<std::mutex> lock(g_resources_mutex);
    return g_tracked_surfaces.find(handle_id) != g_tracked_surfaces.end() ? 1 : 0;
}

extern "C" int SAMM_is_graphics_image_valid(uint64_t handle_id) {
    std::lock_guard<std::mutex> lock(g_resources_mutex);
    return g_tracked_images.find(handle_id) != g_tracked_images.end() ? 1 : 0;
}

// =============================================================================
// SCOPE MANAGEMENT INTEGRATION
// =============================================================================

extern "C" void SAMM_cleanup_graphics_resources_for_scope(int scope_depth) {
    trace_log("Cleaning up graphics resources for scope %d\n", scope_depth);
    
    std::lock_guard<std::mutex> resources_lock(g_resources_mutex);
    std::lock_guard<std::mutex> scopes_lock(g_scopes_mutex);
    
    int cleaned_surfaces = 0;
    int cleaned_images = 0;
    
    // Clean up surfaces in this scope
    auto surfaces_it = g_surfaces_by_scope.find(scope_depth);
    if (surfaces_it != g_surfaces_by_scope.end()) {
        for (uint64_t handle_id : surfaces_it->second) {
            auto surface_it = g_tracked_surfaces.find(handle_id);
            if (surface_it != g_tracked_surfaces.end()) {
                cleanup_surface_internal(surface_it->second);
                g_tracked_surfaces.erase(surface_it);
                cleaned_surfaces++;
            }
        }
        g_surfaces_by_scope.erase(surfaces_it);
    }
    
    // Clean up images in this scope
    auto images_it = g_images_by_scope.find(scope_depth);
    if (images_it != g_images_by_scope.end()) {
        for (uint64_t handle_id : images_it->second) {
            auto image_it = g_tracked_images.find(handle_id);
            if (image_it != g_tracked_images.end()) {
                cleanup_image_internal(image_it->second);
                g_tracked_images.erase(image_it);
                cleaned_images++;
            }
        }
        g_images_by_scope.erase(images_it);
    }
    
    if (cleaned_surfaces > 0 || cleaned_images > 0) {
        trace_log("Cleaned up %d surfaces and %d images from scope %d\n",
                  cleaned_surfaces, cleaned_images, scope_depth);
    }
}

extern "C" void SAMM_force_graphics_cleanup(void) {
    trace_log("Forcing cleanup of all graphics resources\n");
    
    std::lock_guard<std::mutex> resources_lock(g_resources_mutex);
    std::lock_guard<std::mutex> scopes_lock(g_scopes_mutex);
    
    // Clean up all surfaces
    for (auto& pair : g_tracked_surfaces) {
        cleanup_surface_internal(pair.second);
    }
    g_tracked_surfaces.clear();
    
    // Clean up all images
    for (auto& pair : g_tracked_images) {
        cleanup_image_internal(pair.second);
    }
    g_tracked_images.clear();
    
    // Clear scope tracking
    g_surfaces_by_scope.clear();
    g_images_by_scope.clear();
}

extern "C" void SAMM_graphics_handle_memory_pressure(void) {
    trace_log("Handling graphics memory pressure\n");
    
    // For now, just force cleanup of completed scopes
    // In the future, we could implement more sophisticated policies
    int current_scope = HeapManager::getInstance().getCurrentScopeDepth();
    
    std::lock_guard<std::mutex> scopes_lock(g_scopes_mutex);
    
    std::vector<int> scopes_to_clean;
    for (const auto& pair : g_surfaces_by_scope) {
        if (pair.first > current_scope) {
            scopes_to_clean.push_back(pair.first);
        }
    }
    for (const auto& pair : g_images_by_scope) {
        if (pair.first > current_scope) {
            scopes_to_clean.push_back(pair.first);
        }
    }
    
    for (int scope : scopes_to_clean) {
        SAMM_cleanup_graphics_resources_for_scope(scope);
    }
}

// =============================================================================
// STATISTICS AND DEBUGGING
// =============================================================================

extern "C" void SAMM_get_graphics_stats(int64_t* active_surfaces, int64_t* active_images, int64_t* total_memory) {
    std::lock_guard<std::mutex> lock(g_stats_mutex);
    
    if (active_surfaces) *active_surfaces = g_stats.total_surfaces;
    if (active_images) *active_images = g_stats.total_images;
    if (total_memory) *total_memory = g_stats.total_memory;
}

extern "C" int SAMM_get_graphics_backend_stats(const char* backend_name, 
                                                int64_t* surfaces, int64_t* images, int64_t* memory) {
    if (!backend_name) return 0;
    
    std::lock_guard<std::mutex> lock(g_stats_mutex);
    
    std::string name(backend_name);
    
    if (surfaces) {
        auto it = g_stats.backend_surfaces.find(name);
        *surfaces = (it != g_stats.backend_surfaces.end()) ? it->second : 0;
    }
    
    if (images) {
        auto it = g_stats.backend_images.find(name);
        *images = (it != g_stats.backend_images.end()) ? it->second : 0;
    }
    
    if (memory) {
        auto it = g_stats.backend_memory.find(name);
        *memory = (it != g_stats.backend_memory.end()) ? it->second : 0;
    }
    
    return 1;
}

extern "C" void SAMM_dump_graphics_state(void) {
    printf("\n=== SAMM Graphics Resource State ===\n");
    
    {
        std::lock_guard<std::mutex> lock(g_stats_mutex);
        printf("Total Resources: %lld surfaces, %lld images, %lld bytes\n",
               g_stats.total_surfaces, g_stats.total_images, g_stats.total_memory);
        
        printf("By Backend:\n");
        std::unordered_set<std::string> all_backends;
        for (const auto& pair : g_stats.backend_surfaces) all_backends.insert(pair.first);
        for (const auto& pair : g_stats.backend_images) all_backends.insert(pair.first);
        
        for (const std::string& backend : all_backends) {
            int64_t surfaces = g_stats.backend_surfaces[backend];
            int64_t images = g_stats.backend_images[backend];
            int64_t memory = g_stats.backend_memory[backend];
            printf("  %s: %lld surfaces, %lld images, %lld bytes\n",
                   backend.c_str(), surfaces, images, memory);
        }
    }
    
    {
        std::lock_guard<std::mutex> lock(g_scopes_mutex);
        printf("By Scope:\n");
        for (const auto& pair : g_surfaces_by_scope) {
            if (!pair.second.empty()) {
                printf("  Scope %d: %zu surfaces\n", pair.first, pair.second.size());
            }
        }
        for (const auto& pair : g_images_by_scope) {
            if (!pair.second.empty()) {
                printf("  Scope %d: %zu images\n", pair.first, pair.second.size());
            }
        }
    }
    
    printf("=====================================\n");
}

extern "C" int SAMM_validate_graphics_state(void) {
    // Basic consistency checks
    std::lock_guard<std::mutex> resources_lock(g_resources_mutex);
    std::lock_guard<std::mutex> scopes_lock(g_scopes_mutex);
    
    // Check that all surfaces in scopes exist in tracking
    for (const auto& scope_pair : g_surfaces_by_scope) {
        for (uint64_t handle_id : scope_pair.second) {
            if (g_tracked_surfaces.find(handle_id) == g_tracked_surfaces.end()) {
                printf("ERROR: Surface handle %llu in scope %d but not in tracking\n",
                       handle_id, scope_pair.first);
                return 0;
            }
        }
    }
    
    // Check that all images in scopes exist in tracking
    for (const auto& scope_pair : g_images_by_scope) {
        for (uint64_t handle_id : scope_pair.second) {
            if (g_tracked_images.find(handle_id) == g_tracked_images.end()) {
                printf("ERROR: Image handle %llu in scope %d but not in tracking\n",
                       handle_id, scope_pair.first);
                return 0;
            }
        }
    }
    
    return 1;
}

extern "C" void SAMM_set_graphics_trace_enabled(int enabled) {
    std::lock_guard<std::mutex> lock(g_config_mutex);
    g_graphics_trace_enabled = (enabled != 0);
    trace_log("Graphics tracing %s\n", g_graphics_trace_enabled ? "enabled" : "disabled");
}

// =============================================================================
// INITIALIZATION AND SHUTDOWN
// =============================================================================

extern "C" int SAMM_initialize_graphics_resources(void) {
    std::lock_guard<std::mutex> lock(g_config_mutex);
    
    if (g_graphics_initialized) {
        return 1; // Already initialized
    }
    
    // Clear all data structures
    {
        std::lock_guard<std::mutex> backends_lock(g_backends_mutex);
        g_backends.clear();
    }
    
    {
        std::lock_guard<std::mutex> resources_lock(g_resources_mutex);
        g_tracked_surfaces.clear();
        g_tracked_images.clear();
    }
    
    {
        std::lock_guard<std::mutex> scopes_lock(g_scopes_mutex);
        g_surfaces_by_scope.clear();
        g_images_by_scope.clear();
    }
    
    {
        std::lock_guard<std::mutex> stats_lock(g_stats_mutex);
        g_stats = GraphicsStats{};
    }
    
    g_next_handle_id.store(1);
    g_graphics_initialized = true;
    
    trace_log("Graphics resource tracking initialized\n");
    return 1;
}

extern "C" void SAMM_shutdown_graphics_resources(void) {
    trace_log("Shutting down graphics resource tracking\n");
    
    // Force cleanup of all resources
    SAMM_force_graphics_cleanup();
    
    std::lock_guard<std::mutex> lock(g_config_mutex);
    g_graphics_initialized = false;
    
    trace_log("Graphics resource tracking shutdown complete\n");
}

extern "C" int SAMM_is_graphics_initialized(void) {
    std::lock_guard<std::mutex> lock(g_config_mutex);
    return g_graphics_initialized ? 1 : 0;
}

// =============================================================================
// HANDLE GENERATION
// =============================================================================

extern "C" uint64_t SAMM_generate_graphics_handle(void) {
    return g_next_handle_id.fetch_add(1);
}