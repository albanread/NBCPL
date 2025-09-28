#include "LinearScanAllocator.h"
#include <algorithm>
#include <iostream>
#include <cassert>

LinearScanAllocator::LinearScanAllocator(ASTAnalyzer& analyzer, bool debug) 
    : analyzer_(analyzer), debug_enabled_(debug) {}

std::map<std::string, LiveInterval> LinearScanAllocator::allocate(
    const std::vector<LiveInterval>& intervals,
    const std::vector<std::string>& int_regs,
    const std::vector<std::string>& float_regs,
    const std::string& current_function_name
) {
    if (debug_enabled_) {
        std::cout << "[Allocator] Starting partitioned linear scan for function: " << current_function_name << std::endl;
        std::cout << "[Allocator] Available integer registers: " << int_regs.size() 
                  << ", float registers: " << float_regs.size() << std::endl;
    }

    // Phase 2: Get call sites for this function
    const auto& call_sites = analyzer_.get_call_sites_for(current_function_name);
    
    if (debug_enabled_) {
        std::cout << "[Allocator] Function has " << call_sites.size() << " call sites at: ";
        for (int site : call_sites) {
            std::cout << site << " ";
        }
        std::cout << std::endl;
    }

    // Phase 2 & 3: Partition register pools and reserve scratch registers for code generation
    std::vector<std::string> callee_saved_int, caller_saved_int, scratch_reserved_int;
    std::vector<std::string> callee_saved_fp, caller_saved_fp;
    
    // Partition integer registers based on ARM64 ABI and reserve scratch registers
    // Reserve 3 scratch registers for code generation (from the 7 available)
    const std::vector<std::string> scratch_pool = {"X9", "X10", "X11", "X12", "X13", "X14", "X15"};
    const int RESERVED_SCRATCH_COUNT = 3;
    
    for (const auto& reg : int_regs) {
        if ((reg >= "X19" && reg <= "X28") || reg == "X29") {
            callee_saved_int.push_back(reg);
        } else {
            // Check if this is a scratch register we should reserve
            auto scratch_it = std::find(scratch_pool.begin(), scratch_pool.end(), reg);
            if (scratch_it != scratch_pool.end() && 
                scratch_reserved_int.size() < RESERVED_SCRATCH_COUNT) {
                scratch_reserved_int.push_back(reg);
            } else {
                caller_saved_int.push_back(reg);
            }
        }
    }
    
    // Partition floating-point registers based on ARM64 ABI  
    for (const auto& reg : float_regs) {
        // Extract the register number for proper numeric comparison
        if (reg.length() >= 2 && reg[0] == 'D') {
            int reg_num = std::stoi(reg.substr(1));
            if (reg_num >= 8 && reg_num <= 15) {
                callee_saved_fp.push_back(reg);
            } else {
                caller_saved_fp.push_back(reg);
            }
        } else {
            // Fallback for unexpected register format
            caller_saved_fp.push_back(reg);
        }
    }

    // Phase 2: Partition intervals based on call-crossing behavior
    std::vector<LiveInterval> call_crossing_intervals, local_only_intervals;
    for (const auto& interval : intervals) {
        if (does_interval_cross_call(interval, call_sites)) {
            call_crossing_intervals.push_back(interval);
        } else {
            local_only_intervals.push_back(interval);
        }
    }

    // Sort both lists by start point
    std::sort(call_crossing_intervals.begin(), call_crossing_intervals.end(),
              [](const auto& a, const auto& b) { return a.start_point < b.start_point; });
    std::sort(local_only_intervals.begin(), local_only_intervals.end(),
              [](const auto& a, const auto& b) { return a.start_point < b.start_point; });

    if (debug_enabled_) {
        std::cout << "[Allocator] Partitioned: " << call_crossing_intervals.size() 
                  << " call-crossing, " << local_only_intervals.size() << " local-only intervals" << std::endl;
        std::cout << "[Allocator] Register pools - Callee-saved INT: " << callee_saved_int.size() 
                  << ", Caller-saved INT: " << caller_saved_int.size() 
                  << ", Reserved scratch INT: " << scratch_reserved_int.size() << std::endl;
        std::cout << "[Allocator] Register pools - Callee-saved FP: " << callee_saved_fp.size() 
                  << ", Caller-saved FP: " << caller_saved_fp.size() << std::endl;
    }

    active_intervals_.clear();
    std::map<std::string, LiveInterval> allocations;

    // Stage 1: Allocate call-crossing intervals using callee-saved registers first
    if (debug_enabled_) {
        std::cout << "[Allocator] Stage 1: Allocating call-crossing intervals" << std::endl;
    }
    
    free_int_registers_ = callee_saved_int;
    free_float_registers_ = callee_saved_fp;
    
    for (auto interval : call_crossing_intervals) {
        if (debug_enabled_) {
            std::cout << "[Allocator] Allocating call-crossing " << interval.var_name 
                      << " [" << interval.start_point << "-" << interval.end_point << "]" << std::endl;
        }

        expire_old_intervals(interval.start_point, current_function_name);

        bool is_float = (interval.var_type == VarType::FLOAT);
        auto& free_pool = is_float ? free_float_registers_ : free_int_registers_;
        
        if (free_pool.empty()) {
            spill_at_interval(interval, current_function_name, allocations);
        } else {
            interval.assigned_register = free_pool.back();
            free_pool.pop_back();
            interval.is_spilled = false;
            
            if (debug_enabled_) {
                std::cout << "  Assigned callee-saved register " << interval.assigned_register 
                          << " to " << interval.var_name << std::endl;
            }
            
            active_intervals_.push_back(interval);
            active_intervals_.sort([](const auto& a, const auto& b) { 
                return a.end_point < b.end_point; 
            });
        }
        // CRITICAL FIX: Always update allocations map, but validate consistency
        allocations[interval.var_name] = interval;
        
        if (debug_enabled_) {
            std::cout << "[ALLOC] Updated allocations for " << interval.var_name 
                      << ": spilled=" << interval.is_spilled 
                      << ", register='" << interval.assigned_register << "'" << std::endl;
        }
    }

    // Stage 2: Allocate local-only intervals using caller-saved registers first
    if (debug_enabled_) {
        std::cout << "[Allocator] Stage 2: Allocating local-only intervals" << std::endl;
    }
    
    // Add caller-saved registers to the free pools
    free_int_registers_.insert(free_int_registers_.end(), caller_saved_int.begin(), caller_saved_int.end());
    free_float_registers_.insert(free_float_registers_.end(), caller_saved_fp.begin(), caller_saved_fp.end());
    
    for (auto interval : local_only_intervals) {
        if (debug_enabled_) {
            std::cout << "[Allocator] Allocating local-only " << interval.var_name 
                      << " [" << interval.start_point << "-" << interval.end_point << "]" << std::endl;
        }

        expire_old_intervals(interval.start_point, current_function_name);

        bool is_float = (interval.var_type == VarType::FLOAT);
        auto& free_pool = is_float ? free_float_registers_ : free_int_registers_;
        
        if (free_pool.empty()) {
            spill_at_interval(interval, current_function_name, allocations);
        } else {
            interval.assigned_register = free_pool.back();
            free_pool.pop_back();
            interval.is_spilled = false;
            
            if (debug_enabled_) {
                std::cout << "  Assigned register " << interval.assigned_register 
                          << " to " << interval.var_name << std::endl;
            }
            
            active_intervals_.push_back(interval);
            active_intervals_.sort([](const auto& a, const auto& b) { 
                return a.end_point < b.end_point; 
            });
        }
        // CRITICAL FIX: Always update allocations map, but validate consistency  
        allocations[interval.var_name] = interval;
        
        if (debug_enabled_) {
            std::cout << "[ALLOC] Updated allocations for " << interval.var_name 
                      << ": spilled=" << interval.is_spilled 
                      << ", register='" << interval.assigned_register << "'" << std::endl;
        }
    }
    
    if (debug_enabled_) {
        std::cout << "[Allocator] Partitioned allocation complete for " << current_function_name << std::endl;
        std::cout << "[Allocator] Reserved " << scratch_reserved_int.size() 
                  << " scratch registers for code generation: ";
        for (const auto& reg : scratch_reserved_int) {
            std::cout << reg << " ";
        }
        std::cout << std::endl;
        
        // Comprehensive validation of final state
        std::cout << "[Allocator] Final state validation:" << std::endl;
        std::cout << "  Active intervals remaining: " << active_intervals_.size() << std::endl;
        std::cout << "  Free integer registers: " << free_int_registers_.size() << std::endl;
        std::cout << "  Free float registers: " << free_float_registers_.size() << std::endl;
        
        // Validate no register conflicts for overlapping intervals
        int conflict_count = 0;
        for (const auto& pair1 : allocations) {
            if (pair1.second.is_spilled || pair1.second.assigned_register.empty()) continue;
            
            for (const auto& pair2 : allocations) {
                if (pair2.second.is_spilled || pair2.second.assigned_register.empty()) continue;
                if (pair1.first >= pair2.first) continue; // Avoid checking same pair twice
                
                // Check if same register assigned to variables with overlapping intervals
                if (pair1.second.assigned_register == pair2.second.assigned_register) {
                    // Check if intervals overlap
                    bool overlaps = !(pair1.second.end_point < pair2.second.start_point || 
                                    pair2.second.end_point < pair1.second.start_point);
                    if (overlaps) {
                        std::cout << "  ERROR: Register " << pair1.second.assigned_register 
                                  << " assigned to overlapping variables " << pair1.first 
                                  << " [" << pair1.second.start_point << "-" << pair1.second.end_point << "] and "
                                  << pair2.first << " [" << pair2.second.start_point << "-" << pair2.second.end_point << "]!" << std::endl;
                        conflict_count++;
                    }
                }
            }
        }
        
        if (conflict_count == 0) {
            std::cout << "  ✓ No register conflicts detected" << std::endl;
        } else {
            std::cout << "  ✗ " << conflict_count << " register conflicts detected!" << std::endl;
        }
        
        std::cout << "[Allocator] Results:" << std::endl;
        
        for (const auto& pair : allocations) {
            const auto& var_name = pair.first;
            const auto& interval = pair.second;
            bool crosses_call = does_interval_cross_call(interval, call_sites);
            std::cout << "  " << var_name << ": ";
            if (interval.is_spilled) {
                std::cout << "SPILLED";
            } else {
                std::cout << "reg " << interval.assigned_register;
                // Indicate register type
                bool is_callee_saved = ((interval.assigned_register >= "X19" && interval.assigned_register <= "X28") ||
                                       (interval.assigned_register >= "D8" && interval.assigned_register <= "D15"));
                std::cout << " (" << (is_callee_saved ? "callee-saved" : "caller-saved") << ")";
            }
            std::cout << " [" << (crosses_call ? "call-crossing" : "local-only") << "]" << std::endl;
        }
    }
    
    return allocations;
}

void LinearScanAllocator::expire_old_intervals(int current_point, const std::string& current_function_name) {
    auto it = active_intervals_.begin();
    while (it != active_intervals_.end()) {
        if (it->end_point >= current_point) {
            // This interval is still active
            ++it;
        } else {
            // This interval has expired, free its register
            if (debug_enabled_) {
                std::cout << "  Expiring interval for " << it->var_name 
                          << ", freeing register " << it->assigned_register << std::endl;
            }
            
            // Return register to appropriate pool based on STORED type (prevents corruption)
            bool is_float = (it->var_type == VarType::FLOAT);
            auto& free_pool = is_float ? free_float_registers_ : free_int_registers_;
            free_pool.push_back(it->assigned_register);
            
            // Remove from active list
            it = active_intervals_.erase(it);
        }
    }
}

void LinearScanAllocator::spill_at_interval(LiveInterval& interval, const std::string& current_function_name, std::map<std::string, LiveInterval>& allocations) {
    if (debug_enabled_) {
        std::cout << "[SPILL] RegisterManager State: Attempting to spill for " << interval.var_name << std::endl;
        std::cout << "[SPILL] Active intervals count: " << active_intervals_.size() << std::endl;
        std::cout << "[SPILL] Free integer registers: " << free_int_registers_.size() << std::endl;
        std::cout << "[SPILL] Free float registers: " << free_float_registers_.size() << std::endl;
    }

    // Check if we have any active intervals to work with
    if (active_intervals_.empty()) {
        // This is a critical bug - no active intervals but all registers are occupied
        // This indicates inconsistent state in the register allocator
        std::cerr << "[CRITICAL BUG] No active intervals to spill from but all registers are occupied!" << std::endl;
        std::cerr << "[CRITICAL BUG] Variable: " << interval.var_name << " [" << interval.start_point << "-" << interval.end_point << "]" << std::endl;
        std::cerr << "[CRITICAL BUG] Free integer registers: " << free_int_registers_.size() << std::endl;
        std::cerr << "[CRITICAL BUG] Free float registers: " << free_float_registers_.size() << std::endl;
        std::cerr << "[CRITICAL BUG] Active intervals count: " << active_intervals_.size() << std::endl;
        std::cerr << "[CRITICAL BUG] This indicates a bug in register state management." << std::endl;
        
        // Assert to catch this bug during development
        assert(false && "Register allocator state inconsistency: no active intervals but all registers occupied");
        
        // Fallback for release builds (though this should never happen)
        interval.is_spilled = true;
        return;
    }

    bool is_float = (interval.var_type == VarType::FLOAT);
    
    // Find the best candidate to spill among active intervals of the same type
    auto best_spill_candidate = active_intervals_.end();
    int latest_end_point = interval.end_point;
    
    for (auto it = active_intervals_.begin(); it != active_intervals_.end(); ++it) {
        bool candidate_is_float = (it->var_type == VarType::FLOAT);
        
        // Only consider intervals of the same type (int/float)
        if (is_float == candidate_is_float && it->end_point > latest_end_point) {
            best_spill_candidate = it;
            latest_end_point = it->end_point;
        }
    }
    
    if (best_spill_candidate != active_intervals_.end()) {
        // Spill the candidate since it lives longer than the current interval
        if (debug_enabled_) {
            std::cout << "[SPILL] Spilling " << best_spill_candidate->var_name 
                      << " (ends at " << best_spill_candidate->end_point 
                      << ") to make room for " << interval.var_name 
                      << " (ends at " << interval.end_point << ")" << std::endl;
        }
        
        // Take the register from the spilled interval
        interval.assigned_register = best_spill_candidate->assigned_register;
        interval.is_spilled = false;
        
        // Mark the candidate as spilled and clear its register assignment
        best_spill_candidate->is_spilled = true;
        best_spill_candidate->assigned_register = "";
        
        // CRITICAL FIX: Update the allocations map to reflect the spilled state
        // We must update the allocations map entry to match the spilled interval's state
        allocations[best_spill_candidate->var_name].is_spilled = true;
        allocations[best_spill_candidate->var_name].assigned_register = "";
        
        // Remove the spilled interval from active list and add the new one
        active_intervals_.erase(best_spill_candidate);
        active_intervals_.push_back(interval);
        
        // Keep active list sorted by end point for efficient expiration
        active_intervals_.sort([](const auto& a, const auto& b) { 
            return a.end_point < b.end_point; 
        });
    } else {
        // No suitable candidate found, spill the current interval
        if (debug_enabled_) {
            std::cout << "[SPILL] No suitable active interval found to spill. Spilling current interval " 
                      << interval.var_name << std::endl;
        }
        interval.is_spilled = true;
        // Ensure current interval's spilled state is reflected in allocations map
        // This prevents inconsistency where interval is spilled but allocations map shows it has a register
    }
    
    // VALIDATION: Check for allocation state consistency after spilling
    if (debug_enabled_) {
        for (const auto& pair : allocations) {
            const std::string& var_name = pair.first;
            const LiveInterval& alloc_interval = pair.second;
            
            // Check if variable is marked as spilled but still has a register
            if (alloc_interval.is_spilled && !alloc_interval.assigned_register.empty()) {
                std::cerr << "[ALLOCATION BUG] Variable '" << var_name 
                          << "' is marked as spilled but still has register '" 
                          << alloc_interval.assigned_register << "'" << std::endl;
            }
            
            // Check if variable has register but is marked as spilled  
            if (!alloc_interval.is_spilled && alloc_interval.assigned_register.empty()) {
                std::cerr << "[ALLOCATION BUG] Variable '" << var_name 
                          << "' is not spilled but has empty register assignment" << std::endl;
            }
        }
    }
}

bool LinearScanAllocator::is_float_variable(const std::string& name, const std::string& func_name) {
    return analyzer_.get_variable_type(func_name, name) == VarType::FLOAT;
}

// Phase 2: Helper function to detect if an interval crosses any function call
bool LinearScanAllocator::does_interval_cross_call(const LiveInterval& interval, const std::vector<int>& call_sites) const {
    for (int call_site : call_sites) {
        if (call_site >= interval.start_point && call_site <= interval.end_point) {
            return true;
        }
    }
    return false;
}