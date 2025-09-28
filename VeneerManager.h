#ifndef VENEER_MANAGER_H
#define VENEER_MANAGER_H

#include <string>
#include <unordered_map>
#include <vector>
#include <set>
#include <cstdint>
#include "RuntimeManager.h"
#include "Encoder.h"
#include "InstructionStream.h"
#include "LabelManager.h"

/**
 * @brief Manages veneer generation for external function calls using the standard pipeline.
 * 
 * This class generates veneers using the standard Encoder, LabelManager, and InstructionStream
 * to ensure consistency with the rest of the codebase. Veneers are added directly to the
 * instruction stream with proper labels that can be resolved by the Linker.
 */
class VeneerManager {
public:
    /**
     * @brief Structure representing a single veneer entry.
     */
    struct VeneerEntry {
        std::string function_name;     // Name of the external function
        uint64_t veneer_address;       // Address where the veneer is located
        uint64_t function_pointer;     // Actual function pointer from runtime
        std::vector<Instruction> instructions; // The veneer instructions
        
        VeneerEntry() = default;
        VeneerEntry(const std::string& name, uint64_t addr, uint64_t func_ptr)
            : function_name(name), veneer_address(addr), function_pointer(func_ptr) {}
    };

    VeneerManager() = default;
    
    /**
     * @brief Initializes the veneer manager with the base address of the code buffer.
     * @param code_buffer_base The base address where the code buffer starts
     */
    void initialize(uint64_t code_buffer_base);
    
    /**
     * @brief Generates all necessary veneers for a set of external functions.
     * This method adds veneers directly to the instruction stream using standard
     * Encoder calls and defines proper labels via the LabelManager.
     * @param external_functions Set of unique external function names
     * @param instruction_stream The main instruction stream to append veneers to
     * @param label_manager The label manager to define veneer labels
     */
    void generate_veneers(const std::set<std::string>& external_functions, 
                         InstructionStream& instruction_stream, 
                         LabelManager& label_manager);
    
    /**
     * @brief Gets the address of a specific veneer.
     * @param function_name The name of the external function
     * @return The absolute address of the veneer, or 0 if not found
     */
    uint64_t get_veneer_address(const std::string& function_name) const;
    
    /**
     * @brief Gets the starting address for main function code.
     * This is the address immediately after all veneers.
     * @return The address where main function code should begin
     */
    uint64_t get_main_code_start_address() const;
    
    /**
     * @brief Gets the veneer label for a given function name.
     * @param function_name The name of the function
     * @return The label name for the veneer, or empty string if not found
     */
    std::string get_veneer_label(const std::string& function_name) const;
    
    /**
     * @brief Gets all generated veneer instructions in order (deprecated).
     * In the new system, veneers are added directly to the instruction stream.
     * @return Empty vector (compatibility method)
     */
    std::vector<Instruction> get_all_veneer_instructions() const;
    
    /**
     * @brief Gets the total size of the veneer section in bytes.
     * @return Size of all veneers combined
     */
    size_t get_total_veneer_size() const { return total_veneer_size_; }
    
    /**
     * @brief Checks if a veneer exists for the given function.
     * @param function_name The name of the function to check
     * @return True if a veneer exists, false otherwise
     */
    bool has_veneer(const std::string& function_name) const;
    
    /**
     * @brief Gets information about all generated veneer labels.
     * @return Map of function name to veneer label
     */
    const std::unordered_map<std::string, std::string>& get_veneer_labels() const { return veneer_labels_; }
    
    /**
     * @brief Prints debug information about all generated veneers.
     */
    void print_debug_info() const;

private:
    static constexpr size_t VENEER_SIZE = 20; // MOVZ/MOVK sequence (4 instructions) + BR = 5 * 4 = 20 bytes
    
    uint64_t code_buffer_base_ = 0;
    size_t total_veneer_size_ = 0;
    
    // Map of function name to veneer label
    std::unordered_map<std::string, std::string> veneer_labels_;
    
    /**
     * @brief Creates a single veneer for the specified function.
     * This refactored version uses the standard Encoder to create instructions.
     * @param function_name The name of the external function
     * @return The created veneer entry
     */
    VeneerEntry create_veneer(const std::string& function_name);
    
    /**
     * @brief Gets the runtime function pointer for the given function name.
     * @param function_name The name of the function
     * @return The function pointer, or nullptr if not found
     */
    void* get_runtime_function_pointer(const std::string& function_name) const;
    
    /**
     * @brief Expands function families to include all variants.
     * @param base_functions The original set of external functions
     * @return Expanded set including all function family variants
     */
    std::set<std::string> expand_function_families(const std::set<std::string>& base_functions) const;
};

#endif // VENEER_MANAGER_H