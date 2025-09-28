#ifndef LINKER_H
#define LINKER_H

#include <vector>
#include <cstdint>
#include <string>
#include <unordered_map>
#include "InstructionStream.h"
#include "LabelManager.h"
#include "RuntimeManager.h"
#include "Encoder.h" // For Instruction and RelocationType

// Structure to represent a veneer (trampoline) for out-of-range calls
struct Veneer {
    size_t address;           // Address where the veneer is located
    size_t target_address;    // Final target address the veneer jumps to
    std::string target_name;  // Name of the target function/label
    std::vector<Instruction> instructions; // The actual veneer instructions
};

class Linker {
public:
    Linker();

    // Processes the instruction stream, resolving labels and applying relocations.
    std::vector<Instruction> process(
        const InstructionStream& stream,
        LabelManager& manager,
        const RuntimeManager& runtime_manager,
        size_t code_base_address,
        void* rodata_base,
        void* data_base,
        bool enable_tracing = false // Flag to control debug output
    );

private:
    // --- Pass 1 Helper ---
    // Assigns addresses to all instructions and resolves label locations.
    std::vector<Instruction> assignAddressesAndResolveLabels(
        const InstructionStream& stream,
        LabelManager& manager,
        size_t code_base_address,
        void* data_base,
        bool enable_tracing
    );

    // --- Pass 2 Helper ---
    // Iterates through instructions to patch relocations.
    void performRelocations(
        std::vector<Instruction>& instructions,
        LabelManager& manager,
        const RuntimeManager& runtime_manager,
        bool enable_tracing
    );

    // Applies a PC-relative relocation to an instruction encoding.
    uint32_t apply_pc_relative_relocation(
        uint32_t instruction_encoding,
        size_t instruction_address,
        size_t target_address,
        RelocationType type,
        bool enable_tracing
    );

    // Applies a MOVZ/MOVK relocation to an instruction encoding.
    uint32_t apply_movz_movk_relocation(
        uint32_t instruction_encoding,
        size_t target_address,
        RelocationType type
    );

    // --- Smart Veneer System ---
    // Checks if a PC-relative branch is within range
    bool is_branch_in_range(size_t instruction_address, size_t target_address, RelocationType type);
    
    // Creates a veneer for an out-of-range call
    Veneer create_veneer(const std::string& target_name, size_t target_address, size_t veneer_address);
    
    // Finds or creates a veneer for a target, returns the veneer address
    size_t get_or_create_veneer(const std::string& target_name, size_t target_address, 
                               size_t caller_address, std::vector<Instruction>& instructions,
                               LabelManager& manager);

    // --- Veneer Management ---
    std::unordered_map<std::string, size_t> veneer_map_;  // target_name -> veneer_address
    std::vector<Veneer> veneers_;                         // All created veneers
    size_t next_veneer_address_;                          // Next available address for veneers
};

#endif // LINKER_H