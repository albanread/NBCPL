#ifndef REGISTER_MANAGER_H
#define REGISTER_MANAGER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <list>
#include <unordered_set>
#include <map>
#include "Encoder.h"

// Forward declarations
class NewCodeGenerator;
class CallFrameManager;
struct LiveInterval;

class RegisterManager {
public:
    // --- FP Register Pools ---
    static const std::vector<std::string> FP_VARIABLE_REGS;
    static const std::vector<std::string> FP_SCRATCH_REGS;

    // New pools for 128-bit Vector Registers
    static const std::vector<std::string> VEC_VARIABLE_REGS;  // Callee-saved (V8-V15)
    static const std::vector<std::string> VEC_SCRATCH_REGS;   // Caller-saved (V0-V7, V16-V31)

    // --- Singleton Access Method ---

    // Returns the name of the i-th callee-saved temp register (X19-X28)
    static std::string get_callee_saved_temp_reg_name(int index) {
        static const char* callee_saved[] = {"X19", "X20", "X21", "X22", "X23", "X24", "X25", "X26", "X27", "X28"};
        if (index < 0 || index >= 10) return "";
        return callee_saved[index];
    }
    static RegisterManager& getInstance();

    // Check if a register is a scratch register
    bool is_scratch_register(const std::string& register_name) const;




    // --- Variable & Scratch Management (Updated to use partitioned pools) ---
    std::pair<std::string, bool> acquire_reg_for_variable(const std::string& variable_name, NewCodeGenerator& code_gen);
    void release_reg_for_variable(const std::string& variable_name);
    std::string acquire_scratch_reg(NewCodeGenerator& code_gen);
    void release_scratch_reg(const std::string& reg_name);

    // Acquire/release a callee-saved temp register for preserving values across function calls
    std::string acquire_callee_saved_temp_reg(CallFrameManager& cfm);
    void release_callee_saved_temp_reg(const std::string& reg_name);

    // Spillable temporaries: for longer-lived temps (arguments, loop values, etc.)
    std::string acquire_spillable_temp_reg(NewCodeGenerator& code_gen);
    std::string acquire_spillable_fp_temp_reg(NewCodeGenerator& code_gen);

    /**
     * @brief Dumps the current state of all register pools and tracking maps to std::cout.
     * @param context A string to print as a header for context.
     */
    void dump_state(const std::string& context) const;

    /**
     * @brief Enables or disables debug/trace output for register state dumps.
     */
    void set_debug_enabled(bool enabled);

    /**
     * @brief Emergency cleanup of clean scratch registers to free up register space.
     * @return Number of registers freed
     */
    int emergency_cleanup_scratch_registers();

    /**
     * @brief Release all clean scratch registers at expression boundaries.
     * This helps prevent register pressure buildup during complex expressions.
     */
    void cleanup_expression_boundary();

    /**
     * @brief Force cleanup of stale variable-to-register mappings.
     * This resolves inconsistencies where registers are marked as scratch_from_vars
     * but no longer have valid variable mappings.
     */
    void force_cleanup_stale_variable_mappings();

    /**
     * @brief Sync RegisterManager with LinearScanAllocator decisions.
     * This method must be called after LinearScanAllocator runs to ensure
     * the RegisterManager knows which registers are reserved for variables.
     * @param allocations Map from function name to variable->register assignments
     * @param current_function The function currently being processed
     */
    void sync_with_allocator(const std::map<std::string, std::map<std::string, LiveInterval>>& allocations, 
                           const std::string& current_function);

    /**
     * @brief Reset allocations when starting a new function.
     * Clears any previous LinearScanAllocator reservations.
     */
    void reset_allocations();




    // Resets only caller-saved/scratch registers (used in routine call codegen)
    void reset_caller_saved_registers();
    void invalidate_caller_saved_registers();

    // --- Helper methods for code generation compatibility ---
    std::string get_free_register(NewCodeGenerator& code_gen);
    void release_register(const std::string& reg_name);
    std::string get_free_float_register();

    // --- Floating-point register allocation ---
    std::string acquire_fp_reg_for_variable(const std::string& variable_name, NewCodeGenerator& code_gen, CallFrameManager& cfm);
    std::string acquire_fp_scratch_reg();
    void release_fp_register(const std::string& reg_name);
    std::vector<std::string> get_in_use_fp_callee_saved_registers() const;
    std::vector<std::string> get_in_use_fp_caller_saved_registers() const;

    // --- Vector Register Management ---
    std::string acquire_vec_scratch_reg();
    void release_vec_scratch_reg(const std::string& reg_name);
    std::string acquire_vec_variable_reg(const std::string& variable_name, NewCodeGenerator& code_gen, CallFrameManager& cfm);
    
    // --- Q Register Management (128-bit NEON registers) ---
    std::string acquire_q_scratch_reg(NewCodeGenerator& code_gen);
    std::string acquire_q_temp_reg(NewCodeGenerator& code_gen);
    void release_q_register(const std::string& qreg);

    // --- Floating-point register management ---
    std::unordered_map<std::string, std::string> fp_variable_to_reg_map_;
    std::list<std::string> fp_variable_reg_lru_order_;

    // Counter for unique temporary names
    int temp_variable_counter_ = 0;

    // Helper to proactively clean up stale variable-to-register mappings
    void cleanup_stale_mappings_for_reg(const std::string& reg_name);

    // Debug/trace flag for conditional state dumps
    bool debug_enabled_ = false;

    // --- Vector Register Tracking ---
    std::unordered_map<std::string, std::string> vec_variable_to_reg_map_;
    std::list<std::string> vec_variable_reg_lru_order_;


    // --- Additional stubs for codegen compatibility ---
    // --- Helper methods for code generation compatibility ---
    std::string get_zero_register() const;

    void mark_register_as_used(const std::string& reg_name);

    // --- Register state tracking ---
    void set_initialized(const std::string& reg_name, bool value);
    bool is_initialized(const std::string& reg_name) const;

    // --- ABI & State Management ---
    void mark_dirty(const std::string& reg_name, bool is_dirty = true);
    bool is_dirty(const std::string& reg_name) const;
    std::vector<std::pair<std::string, std::string>> get_dirty_variable_registers() const;
    std::vector<std::string> get_in_use_callee_saved_registers() const;
    std::vector<std::string> get_in_use_caller_saved_registers() const;

    // --- Helper to check if a register is callee-saved (X19-X28, D8-D15) ---
    static bool is_callee_saved(const std::string& reg_name) {
        static const std::vector<std::string> callee_saved = {
            "X19", "X20", "X21", "X22", "X23", "X24", "X25", "X26", "X27", "X28",
            "D8", "D9", "D10", "D11", "D12", "D13", "D14", "D15"
        };
        return std::find(callee_saved.begin(), callee_saved.end(), reg_name) != callee_saved.end();
    }


    // Returns the next available pre-reserved callee-saved temp register (X19-X28)
    std::string get_next_available_temp_reg();

    void reset();

    // === NEON Control Methods ===
    
    /**
     * Enable or disable NEON instruction generation
     * When disabled, forces scalar fallback for all vector operations
     */
    void set_neon_enabled(bool enabled) { neon_enabled_ = enabled; }
    
    /**
     * Check if NEON instructions are enabled
     * @return true if NEON is enabled, false if --no-neon or disabled
     */
    bool is_neon_enabled() const { return neon_enabled_; }

    bool is_fp_register(const std::string& reg_name) const;
    bool is_variable_spilled(const std::string& variable_name) const;

    // --- Register allocation helpers for spill/restore ---
    // Returns true if the register is a caller-saved general-purpose register
    bool is_caller_saved_gp(const std::string& reg_name) const;
    // Returns true if the register is a caller-saved floating-point register
    bool is_caller_saved_fp(const std::string& reg_name) const;
    // Returns the register currently assigned to a variable (empty string if none)
    std::string get_register_for_variable(const std::string& variable_name) const;

private:
    // Private constructor to prevent direct instantiation
    RegisterManager();
    // Private destructor (optional, but good practice for singletons if specific cleanup is needed)
    ~RegisterManager() = default;

    // Delete copy constructor and assignment operator to prevent copying
    RegisterManager(const RegisterManager&) = delete;
    RegisterManager& operator=(const RegisterManager&) = delete;

    static RegisterManager* instance_;
    
    // NEON enable/disable state
    bool neon_enabled_ = true;  // Default: NEON enabled



public:
    // Callee-saved register pools for variables that live across function calls
    static const std::vector<std::string> CALLEE_SAVED_REGS;
    static const std::vector<std::string> FP_CALLEE_SAVED_REGS;
    
    // Track which temp register is next to be handed out
    int temp_reg_allocation_index_ = 0;
    enum RegisterStatus {
        FREE,
        IN_USE_VARIABLE,
        IN_USE_SCRATCH,
        IN_USE_ROUTINE_ADDR,
        IN_USE_DATA_BASE
    };

    struct RegisterInfo {
        RegisterStatus status;
        std::string bound_to;
        bool dirty;
        bool is_initialized = false; // Tracks if register holds valid value
    };


    std::unordered_map<std::string, RegisterInfo> registers;
    std::unordered_map<std::string, std::string> variable_to_reg_map;
    std::list<std::string> variable_reg_lru_order_;

    // Stores the list of caller-saved GP and FP registers spilled for restoration
    std::vector<std::string> caller_saved_spills_;
    std::vector<std::string> fp_caller_saved_spills_;

    // For saving/restoring caller-saved state



    const std::string DATA_BASE_REG = "X28";
    static const std::vector<std::string> VARIABLE_REGS;

    // --- Caller-saved registers for liveness analysis ---
    static const std::vector<std::string> CALLER_SAVED_REGS;
    
    // Split the SCRATCH_REGS based on caller/callee saved status
    static inline const std::vector<std::string> SCRATCH_REGS = {"X9", "X10", "X11", "X12", "X13", "X14", "X15"};
    static inline const std::vector<std::string> RESERVED_REGS = {"X19", "X28"};

    std::unordered_set<std::string> spilled_variables_;

    void initialize_registers();
    std::string find_free_register(const std::vector<std::string>& pool);
    Instruction generate_spill_code(const std::string& reg_name, const std::string& variable_name, CallFrameManager& cfm);
};

#endif // REGISTER_MANAGER_H
