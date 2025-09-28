#ifndef LIVEINTERVALPASS_H
#define LIVEINTERVALPASS_H

#include "LiveInterval.h"
#include "../SymbolTable.h"
#include <map>
#include <vector>
#include <string>

class ControlFlowGraph;
class LivenessAnalysisPass;

class LiveIntervalPass {
public:
    explicit LiveIntervalPass(SymbolTable* symbol_table = nullptr, bool trace = false) 
        : symbol_table_(symbol_table), trace_enabled_(trace) {}

    void run(const ControlFlowGraph& cfg, const LivenessAnalysisPass& liveness, const std::string& functionName);

    const std::vector<LiveInterval>& getIntervalsFor(const std::string& functionName) const;

private:
    std::map<std::string, std::vector<LiveInterval>> function_intervals_;
    SymbolTable* symbol_table_;
    bool trace_enabled_;
};

#endif // LIVEINTERVALPASS_H