#include "LivenessAnalysisPass.h"
#include <iostream>
#include <exception>

void LivenessAnalysisPass::run_data_flow_analysis() {
    if (trace_enabled_) {
        std::cout << "[LivenessAnalysisPass] Entering run_data_flow_analysis()" << std::endl;
    }
    bool changed = true;
    int iteration = 0;
    try {
        while (changed) {
            changed = false;
            iteration++;
            if (trace_enabled_) {
                std::cout << "[LivenessAnalysisPass] Data-flow iteration " << iteration << std::endl;
            }
            for (const auto& cfg_pair : cfgs_) {
                if (!cfg_pair.second) {
                    if (trace_enabled_) {
                        std::cout << "[LivenessAnalysisPass] Warning: CFG for function '" << cfg_pair.first << "' is null (data flow)." << std::endl;
                    }
                    continue;
                }
                // Use reverse post-order (RPO) for efficient convergence
                const auto& blocks_in_rpo = cfg_pair.second->get_blocks_in_rpo();
                for (auto it = blocks_in_rpo.rbegin(); it != blocks_in_rpo.rend(); ++it) {
                    BasicBlock* b = *it;
                    if (!b) {
                        if (trace_enabled_) {
                            std::cout << "[LivenessAnalysisPass] Warning: Null BasicBlock in function '" << cfg_pair.first << "' (data flow)." << std::endl;
                        }
                        continue;
                    }

                    if (trace_enabled_) {
                        std::cout << "[LivenessAnalysisPass] Processing block: " << b->id << " in function: " << cfg_pair.first << std::endl;
                    }

                    // 1. Calculate out[B] = U in[S] for all successors S
                    std::set<std::string> new_out_set;
                    for (BasicBlock* successor : b->successors) {
                        if (!successor) {
                            if (trace_enabled_) {
                                std::cout << "[LivenessAnalysisPass] Warning: Null successor in block " << b->id << std::endl;
                            }
                            continue;
                        }
                        new_out_set.insert(in_sets_[successor].begin(), in_sets_[successor].end());
                    }
                    out_sets_[b] = new_out_set;

                    // 2. Calculate in[B] = use[B] U (out[B] - def[B])
                    // CALL INTERVAL FIX: For blocks containing function calls,
                    // augment use[B] with out[B] to force live-out variables
                    // across function calls into callee-saved registers
                    std::set<std::string> effective_use_set = use_sets_[b];
                    if (blocks_with_calls_.count(b)) {
                        if (trace_enabled_) {
                            std::cout << "[LivenessAnalysisPass] Applying call interval fix to block " 
                                      << b->id << " - adding " << new_out_set.size() 
                                      << " live-out variables to use set" << std::endl;
                        }
                        effective_use_set.insert(new_out_set.begin(), new_out_set.end());
                    }
                    
                    std::set<std::string> out_minus_def = out_sets_[b];
                    for (const auto& def_var : def_sets_[b]) {
                        out_minus_def.erase(def_var);
                    }
                    
                    std::set<std::string> new_in_set = effective_use_set;
                    new_in_set.insert(out_minus_def.begin(), out_minus_def.end());

                    // Check if the 'in' set has changed
                    if (in_sets_[b] != new_in_set) {
                        in_sets_[b] = new_in_set;
                        changed = true;
                    }
                }
            }
        }
    } catch (const std::exception& ex) {
        std::cerr << "[LivenessAnalysisPass] Exception in run_data_flow_analysis: " << ex.what() << std::endl;
        throw;
    } catch (...) {
        std::cerr << "[LivenessAnalysisPass] Unknown exception in run_data_flow_analysis" << std::endl;
        throw;
    }
    if (trace_enabled_) {
        std::cout << "[LivenessAnalysisPass] Exiting run_data_flow_analysis()" << std::endl;
    }
}
