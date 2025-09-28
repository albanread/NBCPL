#include "CFGSimplificationPass.h"
#include "../AST.h"
#include <iostream>
#include <unordered_set>
#include <vector>
#include <algorithm>

CFGSimplificationPass::CFGSimplificationPass(bool trace_enabled)
    : trace_enabled_(trace_enabled) {}

void CFGSimplificationPass::run(std::unordered_map<std::string, std::unique_ptr<ControlFlowGraph>>& cfgs) {
    debug_print("Starting CFG Simplification Pass");
    stats_.reset();
    
    for (auto& pair : cfgs) {
        const std::string& function_name = pair.first;
        ControlFlowGraph& cfg = *pair.second;
        
        debug_print("Processing function: " + function_name);
        stats_.functions_processed++;
        
        simplify_cfg(cfg);
    }
    
    print_statistics();
    debug_print("CFG Simplification Pass completed");
}

void CFGSimplificationPass::simplify_cfg(ControlFlowGraph& cfg) {
    bool changed = true;
    int iteration = 0;
    
    // Iterate until no more changes are made (fixed point)
    while (changed) {
        changed = false;
        iteration++;
        debug_print("  Iteration " + std::to_string(iteration));
        
        // Collect blocks to process (avoid modifying while iterating)
        std::vector<BasicBlock*> blocks_to_check;
        for (const auto& pair : cfg.blocks) {
            blocks_to_check.push_back(pair.second.get());
        }
        
        // Look for jump-only blocks that can be bypassed
        for (BasicBlock* block : blocks_to_check) {
            // Skip entry and exit blocks - they have special meaning
            if (block->is_entry || block->is_exit) {
                continue;
            }
            
            BasicBlock* target = nullptr;
            if (is_jump_only_block(block, target) && target != nullptr) {
                debug_print("    Found jump-only block: " + block->id + " -> " + target->id);
                
                // Don't create self-loops or eliminate blocks that are their own target
                if (block == target) {
                    continue;
                }
                
                // Don't eliminate blocks that have no predecessors (unreachable code)
                if (block->predecessors.empty()) {
                    continue;
                }
                
                // Redirect all predecessors of this block to point to the target
                redirect_predecessors(block, target);
                changed = true;
                stats_.blocks_eliminated++;
                
                debug_print("    Eliminated block: " + block->id);
            }
        }
        
        // Look for unnecessary jumps to fallthrough successors
        eliminate_unnecessary_jumps(cfg);
        
        // Check if any unnecessary jumps were removed
        if (stats_.unnecessary_jumps_removed > 0) {
            changed = true;
        }
        
        // Remove unreachable blocks after redirection
        if (changed) {
            remove_unreachable_blocks(cfg);
        }
    }
    
    debug_print("  Simplification completed after " + std::to_string(iteration) + " iterations");
}

bool CFGSimplificationPass::is_jump_only_block(BasicBlock* block, BasicBlock*& target) {
    if (!block) {
        return false;
    }
    
    // Case 1: Empty block with exactly one successor (fall-through)
    if (block->statements.empty()) {
        if (block->successors.size() == 1) {
            target = block->successors[0];
            return true;
        }
        return false;
    }
    
    // Case 2: Block with exactly one statement that is an unconditional jump
    if (block->statements.size() == 1) {
        const StmtPtr& stmt = block->statements[0];
        
        if (is_unconditional_jump(stmt)) {
            // For unconditional jumps, the target should be in successors
            if (block->successors.size() == 1) {
                target = block->successors[0];
                return true;
            }
            // Special case: RETURN and FINISH statements don't have successors
            // but they effectively "jump" to the exit block
            if ((stmt->getType() == ASTNode::NodeType::ReturnStmt ||
                 stmt->getType() == ASTNode::NodeType::FinishStmt) &&
                block->successors.empty()) {
                target = nullptr; // Indicates jump to exit
                return false; // Don't optimize these for now
            }
        }
    }
    
    return false;
}

BasicBlock* CFGSimplificationPass::get_jump_target(BasicBlock* block) {
    BasicBlock* target = nullptr;
    if (is_jump_only_block(block, target)) {
        return target;
    }
    return nullptr;
}

void CFGSimplificationPass::redirect_predecessors(BasicBlock* from_block, BasicBlock* to_block) {
    // Make a copy of predecessors list since we'll be modifying it
    std::vector<BasicBlock*> predecessors = from_block->predecessors;
    
    for (BasicBlock* pred : predecessors) {
        // Replace from_block with to_block in predecessor's successor list
        auto& successors = pred->successors;
        for (size_t i = 0; i < successors.size(); ++i) {
            if (successors[i] == from_block) {
                successors[i] = to_block;
                stats_.edges_redirected++;
                debug_print("      Redirected edge: " + pred->id + " -> " + to_block->id);
            }
        }
        
        // Remove pred from from_block's predecessors
        auto& from_preds = from_block->predecessors;
        from_preds.erase(
            std::remove(from_preds.begin(), from_preds.end(), pred),
            from_preds.end()
        );
        
        // Add pred to to_block's predecessors (avoid duplicates)
        auto& to_preds = to_block->predecessors;
        if (std::find(to_preds.begin(), to_preds.end(), pred) == to_preds.end()) {
            to_preds.push_back(pred);
        }
    }
    
    // Clear from_block's predecessor list
    from_block->predecessors.clear();
}

void CFGSimplificationPass::remove_unreachable_blocks(ControlFlowGraph& cfg) {
    // Find all reachable blocks starting from entry
    std::unordered_set<BasicBlock*> reachable;
    std::vector<BasicBlock*> worklist;
    
    if (cfg.entry_block) {
        worklist.push_back(cfg.entry_block);
        reachable.insert(cfg.entry_block);
    }
    
    // DFS to find all reachable blocks
    while (!worklist.empty()) {
        BasicBlock* current = worklist.back();
        worklist.pop_back();
        
        for (BasicBlock* successor : current->successors) {
            if (reachable.find(successor) == reachable.end()) {
                reachable.insert(successor);
                worklist.push_back(successor);
            }
        }
    }
    
    // Remove unreachable blocks
    auto it = cfg.blocks.begin();
    while (it != cfg.blocks.end()) {
        BasicBlock* block = it->second.get();
        if (reachable.find(block) == reachable.end()) {
            debug_print("    Removing unreachable block: " + block->id);
            
            // Remove this block from any successor's predecessor lists
            for (BasicBlock* successor : block->successors) {
                auto& preds = successor->predecessors;
                preds.erase(
                    std::remove(preds.begin(), preds.end(), block),
                    preds.end()
                );
            }
            
            it = cfg.blocks.erase(it);
        } else {
            ++it;
        }
    }
}

bool CFGSimplificationPass::is_unconditional_jump(const StmtPtr& stmt) {
    if (!stmt) {
        return false;
    }
    
    switch (stmt->getType()) {
        case ASTNode::NodeType::GotoStmt:
        case ASTNode::NodeType::ReturnStmt:
        case ASTNode::NodeType::FinishStmt:
        case ASTNode::NodeType::BreakStmt:
        case ASTNode::NodeType::LoopStmt:
        case ASTNode::NodeType::EndcaseStmt:
        case ASTNode::NodeType::ResultisStmt:
            return true;
        default:
            return false;
    }
}

bool CFGSimplificationPass::has_redundant_jump_to_successor(BasicBlock* block) {
    // Block must have exactly one successor and at least one statement
    if (block->successors.size() != 1 || block->statements.empty()) {
        return false;
    }
    
    const StmtPtr& last_stmt = block->statements.back();
    
    // Check if last statement is a GOTO statement
    if (last_stmt->getType() == ASTNode::NodeType::GotoStmt) {
        const auto* goto_stmt = dynamic_cast<const GotoStatement*>(last_stmt.get());
        if (!goto_stmt || !goto_stmt->label_expr) {
            return false;
        }
        
        // Check if the GOTO target matches the block's single successor
        const auto* var_access = dynamic_cast<const VariableAccess*>(goto_stmt->label_expr.get());
        if (var_access) {
            BasicBlock* successor = block->successors[0];
            // Match against successor's label name or ID
            return (var_access->name == successor->label_name || 
                    var_access->name == successor->id);
        }
    }
    
    return false;
}

void CFGSimplificationPass::eliminate_unnecessary_jumps(ControlFlowGraph& cfg) {
    for (const auto& block_pair : cfg.blocks) {
        BasicBlock* block = block_pair.second.get();
        
        // Skip entry and exit blocks
        if (block->is_entry || block->is_exit) {
            continue;
        }
        
        if (has_redundant_jump_to_successor(block)) {
            debug_print("    Removing unnecessary jump from block: " + block->id + 
                       " to successor: " + block->successors[0]->id);
            
            // Remove the redundant jump statement
            block->statements.pop_back();
            stats_.unnecessary_jumps_removed++;
        }
    }
}

void CFGSimplificationPass::debug_print(const std::string& message) {
    if (trace_enabled_) {
        std::cout << "[CFGSimplificationPass] " << message << std::endl;
    }
}

void CFGSimplificationPass::print_statistics() {
    if (trace_enabled_) {
        std::cout << "\n[CFGSimplificationPass] Statistics:" << std::endl;
        std::cout << "  Functions processed: " << stats_.functions_processed << std::endl;
        std::cout << "  Blocks eliminated: " << stats_.blocks_eliminated << std::endl;
        std::cout << "  Edges redirected: " << stats_.edges_redirected << std::endl;
        std::cout << "  Unnecessary jumps removed: " << stats_.unnecessary_jumps_removed << std::endl;
    }
}