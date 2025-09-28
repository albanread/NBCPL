#include "LiveIntervalPass.h"
#include "../LivenessAnalysisPass.h"
#include "../ControlFlowGraph.h"
#include "Visitors/VariableUsageVisitor.h"
#include <iostream>
#include <algorithm>
#include <string>
#include <map>
#include <set>

void LiveIntervalPass::run(const ControlFlowGraph& cfg, const LivenessAnalysisPass& liveness, const std::string& functionName) {
    if (trace_enabled_) {
        std::cout << "[LiveIntervalPass] Building intervals for function: " << functionName << std::endl;
    }

    auto& final_intervals = function_intervals_[functionName];
    final_intervals.clear();

    std::map<std::string, LiveInterval> interval_map;
    int instruction_pos = 0;

    std::vector<BasicBlock*> blocks_in_rpo = cfg.get_blocks_in_rpo();

    for (BasicBlock* block : blocks_in_rpo) {
        if (!block) continue;
        
        int block_start_pos = instruction_pos;

        // --- PASS 1: Find the first and last usage of every variable within this block ---
        std::map<std::string, std::pair<int, int>> block_lifespans; // map<var_name, {first_use, last_use}>

        for (size_t i = 0; i < block->statements.size(); ++i) {
            const auto& stmt = block->statements[i];
            if (!stmt) continue;

            VariableUsageVisitor visitor(symbol_table_);
            stmt->accept(visitor);
            const auto& mentioned_vars = visitor.getVariables();

            for (const auto& var : mentioned_vars) {
                if (block_lifespans.find(var) == block_lifespans.end()) {
                    // First time seeing this variable in the block
                    block_lifespans[var] = { instruction_pos + (int)i, instruction_pos + (int)i };
                } else {
                    // Update the last use position
                    block_lifespans[var].second = instruction_pos + (int)i;
                }
            }
        }
        
        // --- PASS 2: Create or extend global intervals based on block-local lifespans and liveness sets ---
        std::set<std::string> all_vars_in_block;
        for(const auto& p : block_lifespans) all_vars_in_block.insert(p.first);
        const auto& live_in = liveness.get_in_set(block);
        for(const auto& v : live_in) all_vars_in_block.insert(v);
        const auto& live_out = liveness.get_out_set(block);
        for(const auto& v : live_out) all_vars_in_block.insert(v);

        for (const auto& var_name : all_vars_in_block) {
            int start = -1, end = -1;

            auto lifespan_it = block_lifespans.find(var_name);
            if (lifespan_it != block_lifespans.end()) {
                start = lifespan_it->second.first;
                end = lifespan_it->second.second;
            }

            if (live_in.count(var_name)) {
                start = block_start_pos;
            }
            if (live_out.count(var_name)) {
                end = instruction_pos + (int)block->statements.size();
            }

            if (start != -1) { // If the variable was seen at all
                if (interval_map.find(var_name) == interval_map.end()) {
                    // Get variable type to prevent register pool corruption
                    VarType var_type = VarType::INTEGER; // Default to INTEGER
                    if (symbol_table_) {
                        Symbol symbol;
                        if (symbol_table_->lookup(var_name, functionName, symbol)) {
                            var_type = symbol.type;
                        }
                    }
                    interval_map[var_name] = LiveInterval(var_name, start, end, var_type);
                } else {
                    interval_map[var_name].start_point = std::min(interval_map[var_name].start_point, start);
                    interval_map[var_name].end_point = std::max(interval_map[var_name].end_point, end);
                }
            }
        }
        
        instruction_pos += block->statements.size() + 1; // Add 1 to create a gap between blocks
    }

    // --- Ensure _this is always present for class methods ---
    // Heuristic: If functionName contains "::", treat as class method
    if (functionName.find("::") != std::string::npos) {
        if (interval_map.find("_this") == interval_map.end()) {
            // Add _this live interval from start to end of function
            // _this is always an integer (pointer) type
            interval_map["_this"] = LiveInterval("_this", 0, instruction_pos, VarType::INTEGER);
            if (trace_enabled_) {
                std::cout << "[LiveIntervalPass] Injected _this interval for class method: " << functionName
                          << " [0-" << instruction_pos << "]" << std::endl;
            }
        }
    }

    // Finalize
    for (const auto& pair : interval_map) {
        final_intervals.push_back(pair.second);
    }

    std::sort(final_intervals.begin(), final_intervals.end(),
              [](const auto& a, const auto& b) { return a.start_point < b.start_point; });

    if (trace_enabled_) {
        std::cout << "[LiveIntervalPass] Created " << final_intervals.size() << " intervals for function: " << functionName << std::endl;
        for (const auto& interval : final_intervals) {
            std::cout << "  " << interval.var_name << ": [" << interval.start_point
                      << "-" << interval.end_point << "]" << std::endl;
        }
    }
}

const std::vector<LiveInterval>& LiveIntervalPass::getIntervalsFor(const std::string& functionName) const {
    static const std::vector<LiveInterval> empty_vector;
    auto it = function_intervals_.find(functionName);
    if (it != function_intervals_.end()) {
        return it->second;
    }
    return empty_vector;
}
