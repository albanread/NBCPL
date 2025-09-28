#include "RuntimeManager.h"
#include "LabelManager.h"
#include <cstdio>
#include <algorithm>
#include <iostream>
#include <cstdint>



RuntimeManager::RuntimeManager() {}

std::string RuntimeManager::to_upper(const std::string& s) {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

void RuntimeManager::register_function(const std::string& name, int num_args, void* address, FunctionType type) {
    std::string upper_name = to_upper(name);
    if (functions_.count(upper_name)) {
        throw std::runtime_error("Runtime function '" + name + "' already registered.");
    }
    // --- START OF MODIFICATION ---
    size_t offset = next_table_offset_;
    next_table_offset_ += 8; // Each pointer is 8 bytes (64-bit)
    if ((offset / 8) >= 65536) {
        throw std::runtime_error("Exceeded pre-allocated runtime function table size (65536 entries).");
    }
    RuntimeFunction func(upper_name, num_args, address, type);
    func.table_offset = offset;
    functions_.emplace(upper_name, func);
    // --- END OF MODIFICATION ---
}

const RuntimeFunction& RuntimeManager::get_function(const std::string& name) const {
    std::string upper_name = to_upper(name);
    auto it = functions_.find(upper_name);
    if (it == functions_.end()) {
        throw std::runtime_error("Runtime function '" + name + "' not registered");
    }
    return it->second;
}

bool RuntimeManager::is_function_registered(const std::string& name) const {
    std::string upper_name = to_upper(name);
    return functions_.count(upper_name) > 0;
}



void RuntimeManager::set_function_address(const std::string& name, void* address) {
    std::string upper_name = to_upper(name);
    auto it = functions_.find(upper_name);
    if (it == functions_.end()) {
        throw std::runtime_error("Runtime function '" + name + "' not found.");
    }
    it->second.address = address;
}

void RuntimeManager::print_registered_functions() const {
    printf("=== Registered Runtime Functions ===\n");
    for (const auto& kv : functions_) {
        const auto& func = kv.second;
        printf("  %-16s | address: %p | args: %d | table_offset: %zu\n", func.name.c_str(), func.address, func.num_args, func.table_offset);
    }
    printf("====================================\n");
}

// Populate the X28-relative runtime function pointer table in the .data segment
void RuntimeManager::populate_function_pointer_table(void* data_segment_base) const {
    if (!data_segment_base) {
        throw std::runtime_error("Cannot populate runtime table with a null data segment base.");
    }

    // Get the map of all registered runtime functions
    const auto& functions = get_registered_functions();

    for (const auto& pair : functions) {
        const RuntimeFunction& func = pair.second;

        // Calculate the destination address inside the .data segment table
        uint64_t* destination_ptr = reinterpret_cast<uint64_t*>(
            static_cast<char*>(data_segment_base) + 524288 + func.table_offset
        );

        // Write the 64-bit absolute address of the function into the table
        *destination_ptr = reinterpret_cast<uint64_t>(func.address);
    }

    if (isTracingEnabled()) {
        std::cout << "JIT runtime table populated with " << functions.size() << " function pointers." << std::endl;
    }
}

void RuntimeManager::announce_runtime_capabilities() const {
    std::cout << std::endl;
    std::cout << "🚀 NewBCPL Runtime System Initialized" << std::endl;
    std::cout << "=====================================\n" << std::endl;
    
    // Count total functions
    size_t total_functions = functions_.size();
    std::cout << "📋 Runtime Functions: " << total_functions << " registered" << std::endl;
    
    // Check for specific capabilities
    bool has_sdl2 = is_function_registered("SDL2_INIT");
    bool has_heap = is_function_registered("GETVEC");
    bool has_io = is_function_registered("WRITES");
    bool has_math = is_function_registered("RND");
    
    std::cout << "🔧 Available Capabilities:" << std::endl;
    std::cout << "   • Core I/O Functions: " << (has_io ? "✅ Available" : "❌ Not Available") << std::endl;
    std::cout << "   • Heap Management: " << (has_heap ? "✅ Available" : "❌ Not Available") << std::endl;
    std::cout << "   • Math Functions: " << (has_math ? "✅ Available" : "❌ Not Available") << std::endl;
    
    if (has_sdl2) {
        // Count SDL2 functions
        size_t sdl2_count = 0;
        for (const auto& pair : functions_) {
            if (pair.first.substr(0, 4) == "SDL2") {
                sdl2_count++;
            }
        }
        std::cout << "   • SDL2 Graphics: ✅ Available (" << sdl2_count << " functions)" << std::endl;
        
        // Determine if this is static or dynamic SDL2
        std::cout << "     📦 SDL2 Type: ";
        // Static builds have 27 functions (26 SDL2 + 1 marker), dynamic builds have 26
        if (sdl2_count >= 27) {
            std::cout << "Statically Linked (Self-Contained)" << std::endl;
            std::cout << "     🎯 Deployment: No SDL2.dylib required on target systems" << std::endl;
        } else if (sdl2_count >= 26) {
            std::cout << "Dynamically Linked" << std::endl;
            std::cout << "     ⚠️  Deployment: Requires SDL2.dylib on target systems" << std::endl;
        } else {
            std::cout << "Unknown" << std::endl;
        }
    } else {
        std::cout << "   • SDL2 Graphics: ❌ Not Available" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "🎉 Runtime Ready for BCPL Program Execution!" << std::endl;
    std::cout << "=============================================" << std::endl;
    std::cout << std::endl;
}

// ADD THIS ENTIRE NEW METHOD
size_t RuntimeManager::get_function_offset(const std::string& name) const {
    std::string upper_name = to_upper(name);
    auto it = functions_.find(upper_name);
    if (it == functions_.end()) {
        throw std::runtime_error("Runtime function '" + name + "' not found when getting offset.");
    }
    return it->second.table_offset;
}


