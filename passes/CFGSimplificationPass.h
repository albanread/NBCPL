#ifndef CFG_SIMPLIFICATION_PASS_H
#define CFG_SIMPLIFICATION_PASS_H

#include "../ControlFlowGraph.h"
#include "../BasicBlock.h"
#include <unordered_map>
#include <memory>
#include <string>

// CFGSimplificationPass implements CFG-level optimizations that eliminate
// redundant control flow patterns. The primary optimization is branch chaining
// elimination, where intermediate jump-only blocks are bypassed.
//
// Example transformation:
//   Block A -> Block B (contains only GOTO C) -> Block C
//   Becomes:
//   Block A -> Block C (Block B is removed)
//
// This is superior to peephole optimization because:
// 1. It works on the semantic CFG structure, not textual patterns
// 2. It handles complex control flow like SWITCHON correctly
// 3. It preserves program semantics by updating all predecessors
// 4. It simplifies code generation by removing unnecessary jumps
class CFGSimplificationPass {
public:
    CFGSimplificationPass(bool trace_enabled = false);
    
    // Run the simplification pass on all CFGs
    void run(std::unordered_map<std::string, std::unique_ptr<ControlFlowGraph>>& cfgs);
    
    // Run the simplification pass on a single CFG
    void simplify_cfg(ControlFlowGraph& cfg);
    
    std::string getName() const { return "CFG Simplification Pass"; }

private:
    bool trace_enabled_;
    
    // Statistics for reporting
    struct Statistics {
        int blocks_eliminated = 0;
        int edges_redirected = 0;
        int unnecessary_jumps_removed = 0;
        int functions_processed = 0;
        
        void reset() {
            blocks_eliminated = 0;
            edges_redirected = 0;
            unnecessary_jumps_removed = 0;
            functions_processed = 0;
        }
    } stats_;
    
    // Helper to print debug messages
    void debug_print(const std::string& message);
    
    // Check if a block is a jump-only block that can be bypassed
    bool is_jump_only_block(BasicBlock* block, BasicBlock*& target);
    
    // Get the effective target of a jump-only block
    BasicBlock* get_jump_target(BasicBlock* block);
    
    // Redirect all predecessors of 'from_block' to point to 'to_block'
    void redirect_predecessors(BasicBlock* from_block, BasicBlock* to_block);
    
    // Remove unreachable blocks from the CFG
    void remove_unreachable_blocks(ControlFlowGraph& cfg);
    
    // Check if a statement is an unconditional jump
    bool is_unconditional_jump(const StmtPtr& stmt);
    
    // Check if a block has a redundant jump to its fallthrough successor
    bool has_redundant_jump_to_successor(BasicBlock* block);
    
    // Remove unnecessary jump statements when target is fallthrough successor
    void eliminate_unnecessary_jumps(ControlFlowGraph& cfg);
    
    // Print statistics after optimization
    void print_statistics();
};

#endif // CFG_SIMPLIFICATION_PASS_H