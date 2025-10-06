#include "RegisterManager.h"

// Define the list of caller-saved registers for liveness analysis and codegen safety
const std::vector<std::string> RegisterManager::CALLER_SAVED_REGS = {
    "X0", "X1", "X2", "X3", "X4", "X5", "X6", "X7", "X8", "X9",
    "X10", "X11", "X12", "X13", "X14", "X15", "X16", "X17", "X30",
    // Add any additional caller-saved registers as needed for your ABI
    // For ARM64, X0-X15, X16 (IP0), X17 (IP1), X30 (LR) are caller-saved
    // X19-X28 are callee-saved (not included here)
};

// Separate callee-saved and caller-saved register pools
// Callee-saved registers are preserved across function calls (X19-X28)
const std::vector<std::string> RegisterManager::CALLEE_SAVED_REGS = {
    "X19", "X20", "X21", "X22", "X23", "X24", "X25", "X26", "X27"
};

// This is the ONLY pool for spillable user variables.
// SCRATCH_REGS and RESERVED_REGS are now defined as static inline in the header.
const std::vector<std::string> RegisterManager::VARIABLE_REGS = {
    "X19", "X20", "X21", "X22", "X23", "X24", "X25", "X26", "X27"
};
// Callee-saved floating point registers (D8-D15)
const std::vector<std::string> RegisterManager::FP_CALLEE_SAVED_REGS = {
    "D8", "D9", "D10", "D11", "D12", "D13", "D14", "D15"
};

const std::vector<std::string> RegisterManager::FP_VARIABLE_REGS = {
    "D8", "D9", "D10", "D11", "D12", "D13", "D14", "D15"
};
const std::vector<std::string> RegisterManager::FP_SCRATCH_REGS = {
    "D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7",
    "D16", "D17", "D18", "D19", "D20",
    "D21", "D22", "D23", "D24", "D25", "D26", "D27", "D28", "D29", "D30", "D31"
};

// -----------------------------------------------------------------------------
// Invalidate all caller-saved registers after a function call (ABI correctness)
// -----------------------------------------------------------------------------
void RegisterManager::invalidate_caller_saved_registers() {
    // This function iterates through all caller-saved scratch registers
    // and resets their state to FREE, clearing any stale variable mappings.
    for(const auto& reg_name : SCRATCH_REGS) {
        if (registers.count(reg_name)) {
            // If a variable was mapped to this register, remove the mapping.
            if (registers[reg_name].status == IN_USE_VARIABLE) {
                const std::string& var_name = registers[reg_name].bound_to;
                variable_to_reg_map.erase(var_name);
                variable_reg_lru_order_.remove(var_name);
            }
            // Reset the register to its default free state.
            registers[reg_name] = {FREE, "", false};
        }
    }
    // Also invalidate floating-point scratch registers.
    for(const auto& reg_name : FP_SCRATCH_REGS) {
        if (registers.count(reg_name)) {
             if (registers[reg_name].status == IN_USE_VARIABLE) {
                const std::string& var_name = registers[reg_name].bound_to;
                fp_variable_to_reg_map_.erase(var_name);
                fp_variable_reg_lru_order_.remove(var_name);
            }
            registers[reg_name] = {FREE, "", false, false};
        }
    }
}

// --- Register state tracking: is_initialized flag ---
void RegisterManager::set_initialized(const std::string& reg_name, bool value) {
    if (registers.count(reg_name)) {
        registers[reg_name].is_initialized = value;
    }
}

bool RegisterManager::is_initialized(const std::string& reg_name) const {
    auto it = registers.find(reg_name);
    if (it != registers.end()) {
        return it->second.is_initialized;
    }
    return false;
}

// --- Vector Register Pools ---
const std::vector<std::string> RegisterManager::VEC_VARIABLE_REGS = {
    "V8", "V9", "V10", "V11", "V12", "V13", "V14", "V15"
};
const std::vector<std::string> RegisterManager::VEC_SCRATCH_REGS = {
    "V0", "V1", "V2", "V3", "V4", "V5", "V6", "V7",
    "V16", "V17", "V18", "V19", "V20", "V21", "V22", "V23",
    "V24", "V25", "V26", "V27", "V28", "V29", "V30", "V31"
};
#include "NewCodeGenerator.h" // Needed for spill/reload codegen access
#include "CallFrameManager.h" // Needed for spill/reload codegen access
#include "Encoder.h"          // Needed for spill/reload codegen access
#include <iostream>
#include <algorithm>
#include <stdexcept>

void RegisterManager::cleanup_stale_mappings_for_reg(const std::string& reg_name) {
   for (auto it = variable_to_reg_map.begin(); it != variable_to_reg_map.end(); ) {
       if (it->second == reg_name) {
           const std::string& var_to_remove = it->first;
           variable_reg_lru_order_.remove(var_to_remove);
           it = variable_to_reg_map.erase(it);
       } else {
           ++it;
       }
   }
}

// Debug state dump for RegisterManager
void RegisterManager::set_debug_enabled(bool enabled) {
    debug_enabled_ = enabled;
}

void RegisterManager::dump_state(const std::string& context) const {
    if (!debug_enabled_) {
        return;
    }

    std::cout << "\n--- RegisterManager State: " << context << " ---\n";

    // --- General Purpose Registers ---
    std::vector<std::string> used_gp, free_gp;
    std::vector<std::string> all_gp_regs = SCRATCH_REGS;
    all_gp_regs.insert(all_gp_regs.end(), VARIABLE_REGS.begin(), VARIABLE_REGS.end());

    for (const auto& reg_name : all_gp_regs) {
        const auto& info = registers.at(reg_name);
        if (info.status == FREE) {
            free_gp.push_back(reg_name);
        } else {
            std::string details = reg_name + " (bound to: '" + info.bound_to +
                                  "', dirty: " + (info.dirty ? "yes" : "no") + ")";
            used_gp.push_back(details);
        }
    }
    std::cout << "Used GP Registers (" << used_gp.size() << "):\n";
    for (const auto& s : used_gp) std::cout << "  " << s << "\n";
    std::cout << "Free GP Registers (" << free_gp.size() << "):\n";
    for (const auto& s : free_gp) std::cout << "  " << s;
    std::cout << "\n\n";

    // --- Floating-Point Registers ---
    std::vector<std::string> used_fp, free_fp;
    std::vector<std::string> all_fp_regs = FP_SCRATCH_REGS;
    all_fp_regs.insert(all_fp_regs.end(), FP_VARIABLE_REGS.begin(), FP_VARIABLE_REGS.end());

    for (const auto& reg_name : all_fp_regs) {
        const auto& info = registers.at(reg_name);
        if (info.status == FREE) {
            free_fp.push_back(reg_name);
        } else {
            std::string details = reg_name + " (bound to: '" + info.bound_to +
                                  "', dirty: " + (info.dirty ? "yes" : "no") + ")";
            used_fp.push_back(details);
        }
    }
    std::cout << "Used FP Registers (" << used_fp.size() << "):\n";
    for (const auto& s : used_fp) std::cout << "  " << s << "\n";
    std::cout << "Free FP Registers (" << free_fp.size() << "):\n";
    for (const auto& s : free_fp) std::cout << "  " << s;
    std::cout << "\n\n";

    // --- Tracking Maps ---
    std::cout << "Variable -> Register Map (`variable_to_reg_map`):\n";
    if (variable_to_reg_map.empty()) {
        std::cout << "  (empty)\n";
    } else {
        for (const auto& pair : variable_to_reg_map) {
            std::cout << "  '" << pair.first << "' -> " << pair.second << "\n";
        }
    }

    std::cout << "LRU Spill Order (`variable_reg_lru_order_` - back is victim):\n";
    if (variable_reg_lru_order_.empty()) {
        std::cout << "  (empty)\n";
    } else {
        std::cout << "  [ ";
        for (const auto& var : variable_reg_lru_order_) {
            std::cout << var << " ";
        }
        std::cout << "]\n";
    }
    std::cout << "--------------------------------------------------\n";
}
#include <list>
#include <unordered_set>
#include <unordered_map>

// Singleton instance
RegisterManager* RegisterManager::instance_ = nullptr;

// Returns the next available pre-reserved callee-saved temp register (X19-X28)
std::string RegisterManager::get_next_available_temp_reg() {
    // Callee-saved registers: X19-X28
    static const int kFirstCalleeSaved = 19;
    static const int kLastCalleeSaved = 28;

    for (int i = kFirstCalleeSaved; i <= kLastCalleeSaved; ++i) {
        std::string reg = "X" + std::to_string(i);

        // Skip reserved registers if needed (e.g., X28 for global base)
        if (reg == "X28") continue;

        // Only allocate if the register is free
        if (!registers.count(reg) || registers[reg].status == FREE) {
            registers[reg] = {IN_USE_SCRATCH, "temp", false};
            return reg;
        }
    }

    // Try cleanup strategies before giving up
    force_cleanup_stale_variable_mappings();
    
    // Try again after stale cleanup
    for (int i = kFirstCalleeSaved; i <= kLastCalleeSaved; ++i) {
        std::string reg = "X" + std::to_string(i);
        if (reg == "X28") continue;
        
        if (!registers.count(reg) || registers[reg].status == FREE) {
            registers[reg] = {IN_USE_SCRATCH, "temp", false};
            return reg;
        }
    }
    
    // Try expression boundary cleanup
    cleanup_expression_boundary();
    
    // Final attempt after all cleanup
    for (int i = kFirstCalleeSaved; i <= kLastCalleeSaved; ++i) {
        std::string reg = "X" + std::to_string(i);
        if (reg == "X28") continue;
        
        if (!registers.count(reg) || registers[reg].status == FREE) {
            registers[reg] = {IN_USE_SCRATCH, "temp", false};
            return reg;
        }
    }

    throw std::runtime_error("No more pre-reserved callee-saved temp registers available!");
}

RegisterManager::RegisterManager() {
    initialize_registers();
}

RegisterManager& RegisterManager::getInstance() {
    if (!instance_) {
        instance_ = new RegisterManager();
    }
    return *instance_;
}

void RegisterManager::reset() {
    initialize_registers();
}



bool RegisterManager::is_variable_spilled(const std::string& variable_name) const {
    return spilled_variables_.count(variable_name) > 0;
}

void RegisterManager::initialize_registers() {
    registers.clear();
    variable_to_reg_map.clear();
    variable_reg_lru_order_.clear();
    spilled_variables_.clear();
    
    // Initialize all registers as clean (not dirty)
    for (const auto& reg : VARIABLE_REGS) {
        registers[reg].dirty = false;
    }
    for (const auto& reg : SCRATCH_REGS) {
        registers[reg].dirty = false;
    }

    for (const auto& reg : FP_VARIABLE_REGS) {
        registers[reg].dirty = false;
    }
    for (const auto& reg : FP_SCRATCH_REGS) {
        registers[reg].dirty = false;
    }

    // Initialize all managed registers to FREE

    for (const auto& reg : VARIABLE_REGS) registers[reg] = {FREE, "", false};
    for (const auto& reg : SCRATCH_REGS) registers[reg] = {FREE, "", false};
    for (const auto& reg : RESERVED_REGS) registers[reg] = {IN_USE_DATA_BASE, "data_base", false};
    for (const auto& reg : FP_VARIABLE_REGS) registers[reg] = {FREE, "", false};
    for (const auto& reg : FP_SCRATCH_REGS) registers[reg] = {FREE, "", false};
    // Vector register initialization
    for (const auto& reg : VEC_VARIABLE_REGS) registers[reg] = {FREE, "", false};
    for (const auto& reg : VEC_SCRATCH_REGS) registers[reg] = {FREE, "", false};
    fp_variable_to_reg_map_.clear();
    fp_variable_reg_lru_order_.clear();
    vec_variable_to_reg_map_.clear();
    vec_variable_reg_lru_order_.clear();


}

// --- New Cache Management Logic ---

// --- Floating-point register allocation ---


std::string RegisterManager::get_free_float_register() {
    return acquire_fp_scratch_reg();
}





bool RegisterManager::is_scratch_register(const std::string& register_name) const {
    // This is a simplified check. A more robust implementation would use the SCRATCH_REGS vector.
    for (const auto& reg : SCRATCH_REGS) {
        if (reg == register_name) return true;
    }
    return false;
}







// --- Helper Functions ---

std::string RegisterManager::find_free_register(const std::vector<std::string>& pool) {
    for (const auto& reg : pool) {
        if (registers.at(reg).status == FREE) {
            return reg;
        }
    }
    return "";
}

Instruction RegisterManager::generate_spill_code(const std::string& reg_name, const std::string& variable_name, CallFrameManager& cfm) {
    // If the register is not dirty (hasn't been modified since loading), we can skip the store
    if (!is_dirty(reg_name)) {
        // Return an empty instruction (no-op) to indicate no spill needed
        return Instruction(0, "// Skipping spill for clean register " + reg_name + " (" + variable_name + ")");
    }
    
    // Otherwise generate the store instruction for the dirty register
    int offset = cfm.get_spill_offset(variable_name);
    
    // Use appropriate store instruction based on variable type
    if (cfm.is_float_variable(variable_name)) {
        return Encoder::create_str_fp_imm(reg_name, "X29", offset);
    } else {
        return Encoder::create_str_imm(reg_name, "X29", offset, variable_name);
    }
}


// --- Variable & Scratch Management (Using Partitioned Pools) ---










// --- ABI & State Management ---

std::string RegisterManager::acquire_scratch_reg(NewCodeGenerator& code_gen) {
    // Phase 3: LinearScanAllocator is now the single source of truth for allocation decisions.
    // This method only manages the pre-allocated scratch register pool.
    
    // 1. Try the dedicated scratch pool first.
    std::string reg = find_free_register(SCRATCH_REGS);
    if (!reg.empty()) {
        registers[reg] = {IN_USE_SCRATCH, "scratch", false};
        return reg;
    }

    // 2. Try cleanup of clean scratch registers before giving up
    int freed_count = emergency_cleanup_scratch_registers();
    if (freed_count > 0) {
        if (debug_enabled_) {
            std::cout << "[CLEANUP] Freed " << freed_count << " clean scratch registers" << std::endl;
        }
        reg = find_free_register(SCRATCH_REGS);
        if (!reg.empty()) {
            registers[reg] = {IN_USE_SCRATCH, "scratch", false};
            return reg;
        }
    }
    
    // 3. If no scratch registers are available, this indicates the expression is too complex
    // and should be broken down into simpler statements. This is now a compiler error.
    dump_state("FATAL: No scratch registers available for code generation.");
    throw std::runtime_error("No scratch registers available. Expression too complex - break into simpler statements.");
}

// --- Vector Register Management ---

// Acquire a vector scratch register (caller-saved)
std::string RegisterManager::acquire_vec_scratch_reg() {
    std::string reg = find_free_register(VEC_SCRATCH_REGS);
    if (!reg.empty()) {
        registers[reg] = {IN_USE_SCRATCH, "vec_scratch", false};
        return reg;
    }
    throw std::runtime_error("No available vector scratch registers.");
}

int RegisterManager::emergency_cleanup_scratch_registers() {
    int freed_count = 0;

    // Free any clean scratch registers (not dirty, so safe to release)
    std::vector<std::string> to_free;
    for (const auto& [reg_name, reg_info] : registers) {
        if (reg_info.status == IN_USE_SCRATCH && !reg_info.dirty && 
            (reg_info.bound_to == "scratch" || reg_info.bound_to == "scratch_from_vars")) {
            to_free.push_back(reg_name);
        }
    }

    for (const std::string& reg_name : to_free) {
        registers[reg_name] = {FREE, "", false};
        freed_count++;
        if (debug_enabled_) {
            std::cout << "[EMERGENCY] Released clean scratch register: " << reg_name << std::endl;
        }
    }

    return freed_count;
}

void RegisterManager::cleanup_expression_boundary() {
    // Release all clean scratch registers that aren't needed between expressions
    std::vector<std::string> to_release;
    
    for (const auto& [reg_name, reg_info] : registers) {
        if (reg_info.status == IN_USE_SCRATCH && !reg_info.dirty) {
            to_release.push_back(reg_name);
        }
    }
    
    for (const std::string& reg_name : to_release) {
        registers[reg_name] = {FREE, "", false};
        if (debug_enabled_) {
            std::cout << "[BOUNDARY] Released clean scratch register: " << reg_name << std::endl;
        }
    }
}

void RegisterManager::force_cleanup_stale_variable_mappings() {
    // Find registers marked as scratch_from_vars but with no corresponding variable mapping
    std::vector<std::string> stale_regs;
    
    for (const auto& [reg_name, reg_info] : registers) {
        if (reg_info.status == IN_USE_SCRATCH && reg_info.bound_to == "scratch_from_vars") {
            // This register should have a corresponding variable, but check if it's stale
            bool has_valid_mapping = false;
            for (const auto& [var_name, mapped_reg] : variable_to_reg_map) {
                if (mapped_reg == reg_name) {
                    has_valid_mapping = true;
                    break;
                }
            }
            
            if (!has_valid_mapping && !reg_info.dirty) {
                stale_regs.push_back(reg_name);
            }
        }
    }
    
    // Clean up stale mappings
    for (const std::string& reg_name : stale_regs) {
        registers[reg_name] = {FREE, "", false};
        if (debug_enabled_) {
            std::cout << "[CLEANUP] Freed stale scratch_from_vars register: " << reg_name << std::endl;
        }
    }
}

void RegisterManager::release_vec_scratch_reg(const std::string& reg_name) {
    if (registers.count(reg_name) && registers.at(reg_name).status == IN_USE_SCRATCH) {
        registers[reg_name] = {FREE, "", false};
    }
}

// Acquire a vector variable register (callee-saved, with LRU spill)
std::string RegisterManager::acquire_vec_variable_reg(const std::string& variable_name, NewCodeGenerator& code_gen, CallFrameManager& cfm) {
    // 1. Cache Hit: Variable is already in a register
    if (vec_variable_to_reg_map_.count(variable_name)) {
        std::string reg = vec_variable_to_reg_map_.at(variable_name);
        vec_variable_reg_lru_order_.remove(variable_name);
        vec_variable_reg_lru_order_.push_front(variable_name);
        return reg;
    }

    // 2. Cache Miss: Find a free register
    std::string reg = find_free_register(VEC_VARIABLE_REGS);
    if (!reg.empty()) {
        registers[reg] = {IN_USE_VARIABLE, variable_name, false};
        vec_variable_to_reg_map_[variable_name] = reg;
        vec_variable_reg_lru_order_.push_front(variable_name);
        return reg;
    }

    // 3. Spill: No free registers, spill the least recently used one.
    std::string victim_var = vec_variable_reg_lru_order_.back();
    vec_variable_reg_lru_order_.pop_back();
    std::string victim_reg = vec_variable_to_reg_map_[victim_var];
    
    // Generate spill code (uses 128-bit STR)
    int offset = cfm.get_offset(victim_var); // Assumes CFM handles 16-byte slots
    // NOTE: You will need to implement Encoder::create_str_vec_imm for actual codegen.
    // code_gen.emit(Encoder::create_str_vec_imm(victim_reg, "X29", offset));

    // Update state
    vec_variable_to_reg_map_.erase(victim_var);
    registers[victim_reg] = {IN_USE_VARIABLE, variable_name, false};
    vec_variable_to_reg_map_[variable_name] = victim_reg;
    vec_variable_reg_lru_order_.push_front(variable_name);

    return victim_reg;
}











// --- Codegen Helper Methods ---

std::string RegisterManager::get_free_register(NewCodeGenerator& code_gen) {
    return acquire_scratch_reg(code_gen);
}

// --- LinearScanAllocator Synchronization ---

void RegisterManager::sync_with_allocator(const std::map<std::string, std::map<std::string, LiveInterval>>& allocations, 
                                        const std::string& current_function) {
    // Clear any previous allocator reservations
    reset_allocations();
    
    // Cache allocations for live interval tracking
    cached_allocations_ = allocations;
    cached_function_name_ = current_function;
    current_instruction_point_ = 0;
    
    // Find allocations for the current function
    auto func_it = allocations.find(current_function);
    if (func_it == allocations.end()) {
        if (debug_enabled_) {
            std::cout << "[SYNC] No allocations found for function: " << current_function << std::endl;
        }
        return;
    }
    
    if (debug_enabled_) {
        std::cout << "[SYNC] Caching live intervals for function: " << current_function << std::endl;
        std::cout << "[SYNC] Found " << func_it->second.size() << " variable allocations" << std::endl;
    }
    
    // Initialize with variables that are live at instruction point 0
    update_live_intervals(allocations, current_function, 0);
}

void RegisterManager::update_live_intervals(const std::map<std::string, std::map<std::string, LiveInterval>>& allocations,
                                          const std::string& current_function,
                                          int instruction_point) {
    current_instruction_point_ = instruction_point;
    
    // Find allocations for the current function
    auto func_it = allocations.find(current_function);
    if (func_it == allocations.end()) {
        return;
    }
    
    const auto& func_allocations = func_it->second;
    
    // Clear previous variable-to-register mappings for reused registers
    std::set<std::string> active_registers;
    
    if (debug_enabled_) {
        std::cout << "[LIVE] Updating live intervals at instruction point " << instruction_point << std::endl;
    }
    
    // Update register assignments based on live intervals
    for (const auto& [variable, interval] : func_allocations) {
        const std::string& physical_reg = interval.assigned_register;
        
        if (!physical_reg.empty() && registers.count(physical_reg)) {
            // Check if this variable is live at the current instruction point
            bool is_live = (instruction_point >= interval.start_point && instruction_point <= interval.end_point);
            
            if (is_live) {
                active_registers.insert(physical_reg);
                
                // Update register state for the currently live variable
                registers[physical_reg] = {IN_USE_VARIABLE, variable, false};
                
                // Update variable mapping
                if (physical_reg.find("D") == 0) {
                    // Float register
                    fp_variable_to_reg_map_[variable] = physical_reg;
                    
                    // Remove from LRU if already present, then add to front
                    fp_variable_reg_lru_order_.remove(variable);
                    fp_variable_reg_lru_order_.push_front(variable);
                } else {
                    // Integer register
                    variable_to_reg_map[variable] = physical_reg;
                    
                    // Remove from LRU if already present, then add to front
                    variable_reg_lru_order_.remove(variable);
                    variable_reg_lru_order_.push_front(variable);
                }
                
                if (debug_enabled_) {
                    std::cout << "[LIVE] Register " << physical_reg << " active for variable " << variable 
                              << " [" << interval.start_point << "-" << interval.end_point << "]" << std::endl;
                }
            } else if (instruction_point > interval.end_point) {
                // Variable is no longer live, remove from mappings
                if (physical_reg.find("D") == 0) {
                    fp_variable_to_reg_map_.erase(variable);
                    fp_variable_reg_lru_order_.remove(variable);
                } else {
                    variable_to_reg_map.erase(variable);
                    variable_reg_lru_order_.remove(variable);
                }
                
                if (debug_enabled_) {
                    std::cout << "[LIVE] Variable " << variable << " expired from register " << physical_reg << std::endl;
                }
            }
        }
    }
    
    // Free registers that are no longer active
    for (auto& [reg_name, reg_info] : registers) {
        if (reg_info.status == IN_USE_VARIABLE && active_registers.find(reg_name) == active_registers.end()) {
            // This register was allocated by the LinearScanAllocator but no variable is currently live in it
            reg_info.status = FREE;
            reg_info.bound_to = "";
            reg_info.dirty = false;
            
            if (debug_enabled_) {
                std::cout << "[LIVE] Freed register " << reg_name << " (no active variables)" << std::endl;
            }
        }
    }
}

void RegisterManager::reset_allocations() {
    if (debug_enabled_) {
        std::cout << "[SYNC] Resetting all LinearScanAllocator reservations" << std::endl;
    }
    
    // Clear all variable mappings and reset registers to FREE
    // But preserve any registers that are currently being used as scratch
    
    for (auto& [reg_name, reg_info] : registers) {
        if (reg_info.status == IN_USE_VARIABLE) {
            reg_info = {FREE, "", false};
        }
    }
    
    // Clear variable mappings
    variable_to_reg_map.clear();
    variable_reg_lru_order_.clear();
    fp_variable_to_reg_map_.clear();
    fp_variable_reg_lru_order_.clear();
    vec_variable_to_reg_map_.clear();
    vec_variable_reg_lru_order_.clear();
    
    // Clear spilled variables set
    spilled_variables_.clear();
}












std::string RegisterManager::get_zero_register() const {
    return "XZR";
}

// Returns true if the register is a caller-saved general-purpose register
bool RegisterManager::is_caller_saved_gp(const std::string& reg_name) const {
    for (const auto& reg : CALLER_SAVED_REGS) {
        if (reg == reg_name) return true;
    }
    return false;
}

// Returns true if the register is a caller-saved floating-point register
bool RegisterManager::is_caller_saved_fp(const std::string& reg_name) const {
    for (const auto& reg : FP_SCRATCH_REGS) {
        if (reg == reg_name) return true;
    }
    return false;
}

// Returns the register currently assigned to a variable (empty string if none)
std::string RegisterManager::get_register_for_variable(const std::string& variable_name) const {
    auto it = variable_to_reg_map.find(variable_name);
    if (it != variable_to_reg_map.end()) {
        return it->second;
    }
    // Also check FP variable map
    auto fp_it = fp_variable_to_reg_map_.find(variable_name);
    if (fp_it != fp_variable_to_reg_map_.end()) {
        return fp_it->second;
    }
    return "";
}

void RegisterManager::mark_register_as_used(const std::string& reg_name) {
    // This could be used to track usage for more advanced register allocation.
}



// Implementation for invalidating caller-saved registers


bool RegisterManager::is_fp_register(const std::string& reg_name) const {
    // Check if the register is in the FP variable or scratch pools
    for (const auto& reg : FP_VARIABLE_REGS) {
        if (reg == reg_name) return true;
    }
    for (const auto& reg : FP_SCRATCH_REGS) {
        if (reg == reg_name) return true;
    }
    return false;
}

// --- Q Register Management (128-bit NEON registers) ---

std::string RegisterManager::acquire_q_scratch_reg(NewCodeGenerator& code_gen) {
    // Q registers are 128-bit NEON registers (Q0-Q31)
    // We map these to the VEC_SCRATCH_REGS but prefix with Q instead of V
    std::string vec_reg = acquire_vec_scratch_reg();
    if (!vec_reg.empty() && vec_reg[0] == 'V') {
        return "Q" + vec_reg.substr(1); // Convert V5 -> Q5
    }
    throw std::runtime_error("Failed to acquire Q scratch register");
}

std::string RegisterManager::acquire_q_temp_reg(NewCodeGenerator& code_gen) {
    // For temporary usage, use the same mechanism as scratch registers
    return acquire_q_scratch_reg(code_gen);
}

void RegisterManager::release_q_register(const std::string& qreg) {
    // Convert Q register back to V register for internal tracking
    if (qreg.length() >= 2 && qreg[0] == 'Q') {
        std::string vec_reg = "V" + qreg.substr(1); // Convert Q5 -> V5
        release_vec_scratch_reg(vec_reg);
    } else {
        throw std::runtime_error("Invalid Q register name: " + qreg);
    }
}
