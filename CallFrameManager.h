#ifndef CALL_FRAME_MANAGER_H
// FLET implementation for floating-point declarations
#define CALL_FRAME_MANAGER_H

#include "RegisterManager.h" // Required for the constant vectors

#include <string>
#include <vector>
#include <unordered_map>
#include <memory> // For std::unique_ptr
#include <map> // For std::map
#include "Encoder.h"
#include "DataTypes.h"

// Forward declaration of RegisterManager to avoid circular dependency issues
class RegisterManager;

class CallFrameManager {
public:
    // Constructor: Takes a reference to the RegisterManager
    CallFrameManager(RegisterManager& register_manager, const std::string& function_name = "", bool debug = false);

    // Informs the manager of a new local variable that needs space on the stack, with type.
    void add_local(const std::string& variable_name, VarType type);
    void add_parameter(const std::string& name, VarType type);
    
    // Get the number of parameters
    size_t get_parameter_count() const { return parameters_.size(); }

    // Retrieves the stack offset for a previously declared local variable.
    int get_offset(const std::string& variable_name) const;

    // Set the type of a variable (for dynamic temporaries)
    void set_variable_type(const std::string& variable_name, VarType type);

    // Get the type of a variable (returns VarType::UNKNOWN if not found)
    VarType get_variable_type(const std::string& variable_name) const;

    // Allocates a spill slot for a variable and returns its offset
    int get_spill_offset(const std::string& variable_name);

    // Returns true if the variable is a parameter
    bool is_parameter(const std::string& name) const;

    // Accessor for the function name
    const std::string& get_function_name() const { return function_name; }

    // Returns a vector of all local variable names
    std::vector<std::string> get_local_variable_names() const {
        std::vector<std::string> names;
        for (const auto& it : variable_offsets) {
            names.push_back(it.first);
        }
        return names;
    }
    
    // Returns the name of the parameter at the specified index
    std::string get_parameter_name(size_t index) const {
        if (index < parameters_.size()) {
            return parameters_[index].name;
        }
        throw std::runtime_error("Parameter index out of bounds: " + std::to_string(index));
    }

    // Explicitly forces a register to be saved/restored, regardless of locals.
    void force_save_register(const std::string& reg_name);

    // Set whether this function uses global pointer registers (X19/X28)
    void setUsesGlobalPointers(bool uses) { uses_global_pointers_ = uses; }

    // Predictively reserve callee-saved registers based on register pressure.
    void reserve_registers_based_on_pressure(int register_pressure);

    // Constructs the complete assembly instruction sequence for the function prologue.
    std::vector<Instruction> generate_prologue();

    // Constructs the complete assembly instruction sequence for the function epilogue.
    std::vector<Instruction> generate_epilogue();

    // Checks if a local variable with the given name exists.
    bool has_local(const std::string& variable_name) const;

    // Mark a variable as floating-point type (deprecated, use set_variable_type instead)
    [[deprecated("Use add_local/add_parameter with VarType instead")]]
    void mark_variable_as_float(const std::string& variable_name) { set_variable_type(variable_name, VarType::FLOAT); }

    // --- ADD THIS NEW METHOD ---
    void set_active_register_pool(bool use_extended_pool);
    
    // Check if a variable is a floating-point type (implemented in cf_is_float_variable.cpp)
    bool is_float_variable(const std::string& variable_name) const;

    // Generates a human-readable string representing the current state of the stack frame.
    std::string display_frame_layout() const;

    // New getter for the dedicated X29 spill slot offset.
    int get_x29_spill_slot_offset() const;

private:
    void debug_print(const std::string& message) const;
    int align_to_16(int size) const;

    RegisterManager& reg_manager; // Reference to the RegisterManager singleton
    bool debug_enabled;
    std::string function_name; // Stores the name of the function

    // **NEW:** Tracks the current accumulated size of locals for provisional offset assignment.
    int current_locals_offset_ = 16; // Start locals after FP/LR spill slots (+0, +8)

    // Local variable tracking
    struct LocalVar {
        std::string name;
        int size;

        // Constructor to initialize LocalVar with name and size
        LocalVar(const std::string& variable_name, int variable_size)
            : name(variable_name), size(variable_size) {}

    private:
        // Spill-related member variables moved to CallFrameManager class
        int spill_area_size_; // Total size of the spill area
        int next_spill_offset_; // Offset for the next spill slot
        std::map<std::string, int> spill_variable_offsets_; // Maps variable names to spill offsets
    };
    std::vector<LocalVar> local_declarations;
    int locals_total_size;

    // Final layout information
    std::unordered_map<std::string, int> variable_offsets; // Maps variable name to its final offset from FP
    std::vector<LocalVar> parameters_; // Stores parameter information including names in order
    int spill_area_size_; // Total size of the spill area
    int next_spill_offset_; // Offset for the next spill slot
    std::map<std::string, int> spill_variable_offsets_; // Maps variable names to spill offsets
    std::unordered_map<std::string, VarType> variable_types_; // Tracks the VarType for each variable

    // Helper to get the stack size for a given VarType
    size_t get_size_for_type(VarType type) const;

public:
    // Pre-allocate spill slots before prologue generation
    void preallocate_spill_slots(int count);

private:
    int final_frame_size;
    std::vector<std::string> callee_saved_registers_to_save;
    bool is_prologue_generated;

    // Track if this function uses global pointer registers (X19/X28)
    bool uses_global_pointers_ = false;

    // Pointer to the currently active pool
    const std::vector<std::string>* active_variable_regs_ = &RegisterManager::VARIABLE_REGS; // Default to standard pool

    // The final calculated offset for our dedicated slot.
    int x29_spill_slot_offset;

    // Stack Canary values
    static constexpr uint64_t UPPER_CANARY_VALUE = 0x1122334455667788ULL;
    static constexpr uint64_t LOWER_CANARY_VALUE = 0xAABBCCDDEEFF0011ULL;
    // Size of each canary in bytes
    static constexpr int CANARY_SIZE = 8; // Canaries are 64-bit (8 bytes)
    // Flag to enable/disable stack canaries (disabled by default)
    static bool enable_stack_canaries;

public:
    // Static method to enable or disable stack canaries
    static void setStackCanariesEnabled(bool enabled);
    
private:
    // Get canary size based on whether they're enabled or not
    int getCanarySize() const;
};

#endif // CALL_FRAME_MANAGER_H
