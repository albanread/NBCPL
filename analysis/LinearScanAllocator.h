#ifndef LINEARSCANALLOCATOR_H
#define LINEARSCANALLOCATOR_H

#include "LiveInterval.h"
#include "../analysis/ASTAnalyzer.h"
#include <vector>
#include <string>
#include <map>
#include <list>

class LinearScanAllocator {
public:
    LinearScanAllocator(ASTAnalyzer& analyzer, bool debug = false);

    std::map<std::string, LiveInterval> allocate(
        const std::vector<LiveInterval>& intervals,
        const std::vector<std::string>& int_regs,
        const std::vector<std::string>& float_regs,
        const std::string& current_function_name
    );

private:
    void expire_old_intervals(int current_point, const std::string& current_function_name);
    void spill_at_interval(LiveInterval& current_interval, const std::string& current_function_name, std::map<std::string, LiveInterval>& allocations);
    bool is_float_variable(const std::string& name, const std::string& func_name);
    
    // Phase 2: Conservative call-site awareness - detection only
    bool does_interval_cross_call(const LiveInterval& interval, const std::vector<int>& call_sites) const;

    std::list<LiveInterval> active_intervals_;
    std::vector<std::string> free_int_registers_;
    std::vector<std::string> free_float_registers_;
    ASTAnalyzer& analyzer_;
    bool debug_enabled_;
};

#endif // LINEARSCANALLOCATOR_H