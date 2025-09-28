#include "VeneerManager.h"
#include "Encoder.h"
#include "LabelManager.h"
#include "RuntimeManager.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <algorithm>

void VeneerManager::initialize(uint64_t code_buffer_base) {
    code_buffer_base_ = code_buffer_base;
    // Clear any existing veneers
    veneer_labels_.clear();
    total_veneer_size_ = 0;
}

/**
 * @brief Creates the instruction sequence for a single veneer.
 *
 * This refactored version uses the standard Encoder to create instructions,
 * ensuring consistency with the rest of the codebase.
 *
 * @param function_name The name of the runtime function to call.
 * @return A VeneerEntry struct containing the generated instructions.
 */
VeneerManager::VeneerEntry VeneerManager::create_veneer(const std::string& function_name) {
    // Get the absolute address of the runtime function.
    void* func_ptr = get_runtime_function_pointer(function_name);
    if (!func_ptr) {
        throw std::runtime_error("Veneer creation failed: Runtime function not found: " + function_name);
    }
    uint64_t target_address = reinterpret_cast<uint64_t>(func_ptr);

    VeneerEntry veneer(function_name, 0, target_address); // Address will be set by the Linker.

    // 1. Generate the MOVZ/MOVK sequence to load the 64-bit absolute address
    //    of the runtime function into the veneer register (X16).
    //    This uses your existing, reliable Encoder function.
    std::vector<Instruction> mov_instructions = Encoder::create_movz_movk_abs64("X16", target_address, function_name);
    
    veneer.instructions.insert(veneer.instructions.end(), mov_instructions.begin(), mov_instructions.end());

    // 2. Generate the final indirect branch instruction.
    Instruction br_instr = Encoder::create_br_reg("X16");
    veneer.instructions.push_back(br_instr);
    
    // Tag all instructions within this veneer as JIT-specific.
    for (auto& instr : veneer.instructions) {
        instr.jit_attribute = JITAttribute::JitAddress;
    }
    
    return veneer;
}

/**
 * @brief Generates all necessary veneers for a set of external functions.
 *
 * This method iterates through all required external functions, creates a veneer
 * for each, and adds the veneer's instructions to the main instruction stream.
 * It also defines a unique label for each veneer.
 *
 * @param external_functions The set of function names needing veneers.
 * @param instruction_stream The main instruction stream to append veneers to.
 * @param label_manager The label manager to define veneer labels.
 */
void VeneerManager::generate_veneers(const std::set<std::string>& external_functions, 
                                     InstructionStream& instruction_stream, 
                                     LabelManager& label_manager) {
    // Always expand function families even if external_functions is empty
    // This ensures HeapManager functions get veneers even when not explicitly detected
    std::set<std::string> expanded_functions = expand_function_families(external_functions);
    
    if (expanded_functions.empty()) {
        if (RuntimeManager::instance().isTracingEnabled()) {
            std::cout << "[VeneerManager] No external functions found after expansion, skipping veneer generation." << std::endl;
        }
        return;
    }
    
    if (RuntimeManager::instance().isTracingEnabled()) {
        std::cout << "[VeneerManager] Generating " << expanded_functions.size() 
                  << " veneers (including function families) using standard pipeline..." << std::endl;
    }
    
    // Add a visual separator in the generated code for clarity.
    Instruction separator(0, "\n; --- Veneer Section ---");
    instruction_stream.add(separator);
    
    size_t veneer_count = 0;
    for (const std::string& func_name : expanded_functions) {
        // 1. Create a unique, predictable label for this veneer.
        std::string veneer_label = func_name + "_veneer";
        
        // 2. Define this label at the current position in the instruction stream.
        //    The Linker will calculate its final address.
        instruction_stream.define_label(veneer_label);
        
        // 3. Create the veneer's instruction sequence.
        VeneerEntry veneer = create_veneer(func_name);
        
        // 4. Add the veneer's instructions to the stream.
        for (const auto& instr : veneer.instructions) {
            instruction_stream.add(instr);
        }
        
        // 5. Store the veneer's label for the Linker to use.
        veneer_labels_[func_name] = veneer_label;
        veneer_count++;
        
        if (RuntimeManager::instance().isTracingEnabled()) {
            std::cout << "[VeneerManager] Created veneer: " << veneer_label 
                      << " for function: " << func_name << std::endl;
        }
    }
    
    Instruction end_separator(0, "; --- End Veneer Section ---\n");
    instruction_stream.add(end_separator);
    
    total_veneer_size_ = veneer_count * VENEER_SIZE; // Approximate size
    if (RuntimeManager::instance().isTracingEnabled()) {
        std::cout << "[VeneerManager] Generated " << veneer_count 
                  << " veneers, estimated total size: " << total_veneer_size_ << " bytes" << std::endl;
    }
}

std::string VeneerManager::get_veneer_label(const std::string& function_name) const {
    auto it = veneer_labels_.find(function_name);
    if (it != veneer_labels_.end()) {
        return it->second;
    }
    return ""; // Not found
}

bool VeneerManager::has_veneer(const std::string& function_name) const {
    return veneer_labels_.find(function_name) != veneer_labels_.end();
}

uint64_t VeneerManager::get_veneer_address(const std::string& function_name) const {
    // In the new system, this should be handled by the LabelManager
    // This method is kept for compatibility but should not be used
    std::cerr << "[VeneerManager] Warning: get_veneer_address() called. "
              << "Use LabelManager::get_label_address() instead." << std::endl;
    return 0;
}

uint64_t VeneerManager::get_main_code_start_address() const {
    // In the new system, this is handled by the normal code generation pipeline
    return code_buffer_base_ + total_veneer_size_;
}



std::vector<Instruction> VeneerManager::get_all_veneer_instructions() const {
    // In the new system, veneers are added directly to the instruction stream
    // This method is kept for compatibility but should not be used
    std::cerr << "[VeneerManager] Warning: get_all_veneer_instructions() called. "
              << "Veneers are now added directly to the instruction stream." << std::endl;
    return {};
}

void VeneerManager::print_debug_info() const {
    std::cout << "\n=== VeneerManager Debug Info ===" << std::endl;
    std::cout << "Code buffer base: 0x" << std::hex << code_buffer_base_ << std::dec << std::endl;
    std::cout << "Total veneer size: " << total_veneer_size_ << " bytes" << std::endl;
    std::cout << "Number of veneers: " << veneer_labels_.size() << std::endl;
    
    if (!veneer_labels_.empty()) {
        std::cout << "\nVeneer Labels:" << std::endl;
        for (const auto& pair : veneer_labels_) {
            std::cout << "  " << pair.first << " -> " << pair.second << std::endl;
        }
    }
    std::cout << "==============================\n" << std::endl;
}

void* VeneerManager::get_runtime_function_pointer(const std::string& function_name) const {
    try {
        const RuntimeFunction& func = RuntimeManager::instance().get_function(function_name);
        return func.address;
    } catch (const std::exception&) {
        return nullptr;
    }
}

/**
 * @brief Expands function families to include all variants.
 * 
 * This method detects function families and automatically adds related functions.
 * For example, if WRITEF is detected, it adds WRITEF1, WRITEF2, etc.
 * 
 * @param base_functions The original set of external functions
 * @return Expanded set including all function family variants
 */
std::set<std::string> VeneerManager::expand_function_families(const std::set<std::string>& base_functions) const {
    std::set<std::string> expanded = base_functions;
    
    // Always include essential HeapManager functions (injected by code generator)
    std::vector<std::string> essential_functions = {
        "HeapManager_enter_scope",
        "HeapManager_exit_scope"
    };
    
    for (const std::string& func : essential_functions) {
        if (RuntimeManager::instance().is_function_registered(func)) {
            expanded.insert(func);
            // std::cout << "[VeneerManager] CRITICAL: Adding essential HeapManager function: " << func << std::endl;
        } else {
            std::cout << "[VeneerManager] ERROR: HeapManager function NOT registered: " << func << std::endl;
        }
    }
    
    // Check for WRITEF family
    if (base_functions.find("WRITEF") != base_functions.end()) {
        if (RuntimeManager::instance().isTracingEnabled()) {
            std::cout << "[VeneerManager] WRITEF detected, adding WRITEF1-WRITEF7 variants..." << std::endl;
        }
        for (int i = 1; i <= 7; ++i) {
            std::string variant = "WRITEF" + std::to_string(i);
            // Only add if the variant actually exists in RuntimeManager
            if (RuntimeManager::instance().is_function_registered(variant)) {
                expanded.insert(variant);
            }
        }
    }
    
    // Future: Add other function families here
    // Example: FILE_* functions, SDL2_* functions, etc.
    
    return expanded;
}