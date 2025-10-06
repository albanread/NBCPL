#include "AST.h"
#include "CFGBuilderPass.h"
#include "BasicBlock.h"
#include "Reducer.h"
#include <iostream>
#include <vector>
#include <memory>
#include "analysis/ASTAnalyzer.h"
#include "NameMangler.h"
#include "Symbol.h"
#include "SymbolTable.h"

// Helper function to clone a unique_ptr, assuming it's available from AST_Cloner.cpp
template <typename T>
std::unique_ptr<T> clone_unique_ptr(const std::unique_ptr<T>& original_ptr) {
    if (original_ptr) {
        return std::unique_ptr<T>(static_cast<T*>(original_ptr->clone().release()));
    }
    return nullptr;
}


CFGBuilderPass::CFGBuilderPass(SymbolTable* symbol_table, bool trace_enabled)
    : current_cfg(nullptr),
      current_basic_block(nullptr),
      block_id_counter(0),
      current_block_id_counter(0),
      trace_enabled_(trace_enabled),
      symbol_table_(symbol_table) {
    // Initialize stacks for control flow targets
    break_targets.clear();
    loop_targets.clear();
    endcase_targets.clear();
    
    // Initialize block variable tracking
    block_variable_stack.clear();
}

void CFGBuilderPass::debug_print(const std::string& message) {
    if (trace_enabled_) {
        std::cout << "[CFGBuilderPass] " << message << std::endl;
    }
}

// Helper to check if legacy cleanup should be performed (disabled when SAMM is active)
bool CFGBuilderPass::should_perform_legacy_cleanup() const {
    // If SAMM is enabled, skip legacy cleanup to prevent double-frees
    return !HeapManager::getInstance().isSAMMEnabled();
}

// Helper to create a new basic block and add it to the current CFG
BasicBlock* CFGBuilderPass::create_new_basic_block(const std::string& id_prefix) {
    if (!current_cfg) {
        if (trace_enabled_) {
            std::cerr << "[CFGBuilderPass] ERROR: Cannot create basic block without an active CFG!" << std::endl;
        }
        throw std::runtime_error("Error: Cannot create basic block without an active CFG.");
    }
    BasicBlock* new_bb = nullptr;
    try {
        new_bb = current_cfg->create_block(id_prefix);
        if (!new_bb) {
            std::cerr << "[CFGBuilderPass] ERROR: create_block returned nullptr!" << std::endl;
        } else {
            if (trace_enabled_) {
                std::cout << "[CFGBuilderPass] Created new basic block: " << new_bb->id << std::endl;
            }
        }
    } catch (const std::exception& ex) {
        if (trace_enabled_) {
            std::cerr << "[CFGBuilderPass] Exception in create_new_basic_block: " << ex.what() << std::endl;
        }
        throw;
    } catch (...) {
        if (trace_enabled_) {
            std::cerr << "[CFGBuilderPass] Unknown exception in create_new_basic_block" << std::endl;
        }

         throw;
     }
     return new_bb;
}

// Helper method to check if an expression is a simple variable access
bool CFGBuilderPass::is_simple_variable_access(Expression* expr, std::string& var_name) {
    if (auto* var_access = dynamic_cast<VariableAccess*>(expr)) {
        var_name = var_access->name;
        return true;
    }
    return false;
}



// Helper method to detect if a collection has a known constant size
bool CFGBuilderPass::is_constant_size_collection(Expression* expr, int& size) {
    if (trace_enabled_) {
        std::cout << "[CFGBuilderPass] is_constant_size_collection: Analyzing expression..." << std::endl;
    }
    
    // Check for VecInitializerExpression (VEC [1, 2, 3] or VEC (1, 2, 3))
    if (auto* vec_init = dynamic_cast<VecInitializerExpression*>(expr)) {
        size = static_cast<int>(vec_init->initializers.size());
        if (trace_enabled_) {
            std::cout << "[CFGBuilderPass] OPTIMIZATION: VecInitializerExpression detected, size: " << size << std::endl;
        }
        return true;
    }
    
    // Check for TableExpression (TABLE 1, 2, 3)
    if (auto* table_expr = dynamic_cast<TableExpression*>(expr)) {
        size = static_cast<int>(table_expr->initializers.size());
        if (trace_enabled_) {
            std::cout << "[CFGBuilderPass] OPTIMIZATION: TableExpression detected, size: " << size << std::endl;
        }
        return true;
    }
    
    // Check for VecAllocationExpression with CONSTANT size expression (VEC 5 or VEC BUFFER_SIZE)
    if (auto* vec_alloc = dynamic_cast<VecAllocationExpression*>(expr)) {
        bool has_const_size = false;
        int64_t const_size = ASTAnalyzer::getInstance().evaluate_constant_expression(vec_alloc->size_expr.get(), &has_const_size);
        if (has_const_size) {
            size = static_cast<int>(const_size);
            if (trace_enabled_) {
                std::cout << "[CFGBuilderPass] ENHANCED OPTIMIZATION: VecAllocationExpression with constant size: " << size << std::endl;
            }
            return true;
        } else if (trace_enabled_) {
            std::cout << "[CFGBuilderPass] VecAllocationExpression detected but size is not constant" << std::endl;
        }
    }
    
    // Check for FVecAllocationExpression with CONSTANT size expression (FVEC 3 or FVEC MAX_SIZE)
    if (auto* fvec_alloc = dynamic_cast<FVecAllocationExpression*>(expr)) {
        bool has_const_size = false;
        int64_t const_size = ASTAnalyzer::getInstance().evaluate_constant_expression(fvec_alloc->size_expr.get(), &has_const_size);
        if (has_const_size) {
            size = static_cast<int>(const_size);
            if (trace_enabled_) {
                std::cout << "[CFGBuilderPass] ENHANCED OPTIMIZATION: FVecAllocationExpression with constant size: " << size << std::endl;
            }
            return true;
        } else if (trace_enabled_) {
            std::cout << "[CFGBuilderPass] FVecAllocationExpression detected but size is not constant" << std::endl;
        }
    }
    
    if (trace_enabled_) {
        std::cout << "[CFGBuilderPass] Expression is not a constant-size collection" << std::endl;
    }
    return false;
}





// --- Class context tracking for mangled names ---
void CFGBuilderPass::visit(ClassDeclaration& node) {
    std::string previous_class_name = current_class_name_;
    current_class_name_ = node.name;
    
    if (trace_enabled_) {
        std::cout << "[CFGBuilderPass] Processing class: " << node.name << std::endl;
    }
    
    // Process each class member declaration
    for (const auto& member : node.members) {
        if (!member.declaration) continue;
        
        // Focus on processing methods (functions and routines) for CFG building
        auto* func_decl = dynamic_cast<FunctionDeclaration*>(member.declaration.get());
        auto* routine_decl = dynamic_cast<RoutineDeclaration*>(member.declaration.get());
        
        // Process methods for CFG building
        if (func_decl) {
            if (trace_enabled_) {
                std::cout << "[CFGBuilderPass] Found function method '" << func_decl->name << "' in class '" << node.name << "'" << std::endl;
            }
            member.declaration->accept(*this);
        }
        else if (routine_decl) {
            if (trace_enabled_) {
                std::cout << "[CFGBuilderPass] Found routine method '" << routine_decl->name << "' in class '" << node.name << "'" << std::endl;
            }
            member.declaration->accept(*this);
        }
    }
    
    current_class_name_ = previous_class_name;
}


// Debug method to dump all CFGs - helpful for debugging
void CFGBuilderPass::dump_cfgs() {
    if (!trace_enabled_) return;
    
    std::cout << "\n[CFGBuilderPass] --- DUMPING ALL CFGs ---\n";
    for (const auto& cfg_pair : function_cfgs) {
        std::cout << "[CFGBuilderPass] CFG found for: " << cfg_pair.first << std::endl;
    }
    std::cout << "[CFGBuilderPass] --- END CFG DUMP ---\n";
}

// Helper to end the current basic block and start a new one, adding a fall-through edge
void CFGBuilderPass::end_current_block_and_start_new() {
    if (trace_enabled_) {
        std::cout << "[CFGBuilderPass] end_current_block_and_start_new called." << std::endl;
    }
    if (current_basic_block && !current_basic_block->ends_with_control_flow()) {
        BasicBlock* next_bb = nullptr;
        try {
            next_bb = create_new_basic_block();
            if (!next_bb) {
                std::cerr << "[CFGBuilderPass] ERROR: next_bb is nullptr in end_current_block_and_start_new!" << std::endl;
            } else {
                if (trace_enabled_) {
                    std::cout << "[CFGBuilderPass] Adding edge from " << current_basic_block->id << " to " << next_bb->id << std::endl;
                }
                current_cfg->add_edge(current_basic_block, next_bb);
                current_basic_block = next_bb;
            }
        } catch (const std::exception& ex) {
            if (trace_enabled_) {
                std::cerr << "[CFGBuilderPass] Exception in end_current_block_and_start_new: " << ex.what() << std::endl;
            }
            throw;
        } catch (...) {
            if (trace_enabled_) {
                std::cerr << "[CFGBuilderPass] Unknown exception in end_current_block_and_start_new" << std::endl;
            }
            throw;
        }
    } else if (!current_basic_block) {
        if (trace_enabled_) {
            std::cout << "[CFGBuilderPass] current_basic_block is nullptr, creating new block." << std::endl;
        }
        current_basic_block = create_new_basic_block();
    }
}

void CFGBuilderPass::build(Program& program) {
    if (trace_enabled_) {
        std::cout << "[CFGBuilderPass] build() called." << std::endl;
    }
    function_cfgs.clear();
    current_cfg = nullptr;
    current_basic_block = nullptr;
    label_targets.clear();
    block_id_counter = 0;
    break_targets.clear();
    loop_targets.clear();
    endcase_targets.clear();
    current_class_name_ = ""; // Reset the class context

    try {
        if (trace_enabled_) {
            std::cout << "[CFGBuilderPass] About to accept(Program)" << std::endl;
        }
        program.accept(*this);
        if (trace_enabled_) {
            std::cout << "[CFGBuilderPass] Finished accept(Program)" << std::endl;
        }
    } catch (const std::exception& ex) {
        std::cerr << "[CFGBuilderPass] Exception during build: " << ex.what() << std::endl;
        throw;
    } catch (...) {
        std::cerr << "[CFGBuilderPass] Unknown exception during build" << std::endl;
        throw;
    }
    if (trace_enabled_) {
        std::cout << "[CFGBuilderPass] build() complete." << std::endl;
        // Call our debug method to print all CFGs
        dump_cfgs();
    }
}

BasicBlock* CFGBuilderPass::generate_scope_cleanup_chain(const std::string& cleanup_prefix) {
    if (!symbol_table_ || block_variable_stack.empty()) {
        return current_basic_block;
    }

    BasicBlock* last_cleanup_block = current_basic_block;

    // Create cleanup code in LIFO order (inner scopes first)
    for (auto it = block_variable_stack.rbegin(); it != block_variable_stack.rend(); ++it) {
        const auto& variables_in_scope = *it;
        if (variables_in_scope.empty()) {
            continue; // Skip scopes with no variables to clean
        }

        // Create a new block for this scope's cleanup
        BasicBlock* cleanup_block = create_new_basic_block(cleanup_prefix);
        if (last_cleanup_block && !last_cleanup_block->ends_with_control_flow()) {
            current_cfg->add_edge(last_cleanup_block, cleanup_block);
        }

        // Populate the cleanup block with deallocation calls
        BasicBlock* previous_current = current_basic_block;
        current_basic_block = cleanup_block;
        
        for (const auto& var_name : variables_in_scope) {
            Symbol symbol;
            if (symbol_table_->lookup(var_name, symbol) && symbol.owns_heap_memory) {
                debug_print("Generating scope exit cleanup for: " + var_name);
                
                // Use bitwise operations: list types have both POINTER_TO and LIST flags set
                int64_t type_value = static_cast<int64_t>(symbol.type);
                int64_t list_flags = static_cast<int64_t>(VarType::POINTER_TO) | static_cast<int64_t>(VarType::LIST);
                bool is_list_type = (type_value & list_flags) == list_flags;
                
                if (is_list_type && should_perform_legacy_cleanup()) {
                    debug_print("Generating BCPL_FREE_LIST(" + var_name + ") call");
                    auto var_expr = std::make_unique<VariableAccess>(var_name);
                    auto free_list_expr = std::make_unique<VariableAccess>("BCPL_FREE_LIST");
                    std::vector<ExprPtr> args;
                    args.push_back(std::move(var_expr));
                    auto free_call = std::make_unique<RoutineCallStatement>(std::move(free_list_expr), std::move(args));
                    cleanup_block->add_statement(std::move(free_call));
                } else if (is_list_type) {
                    debug_print("Skipping BCPL_FREE_LIST(" + var_name + ") - SAMM is active");
                }
                // Check for Vectors (POINTER_TO | VEC)
                else {
                    int64_t vector_flags = static_cast<int64_t>(VarType::POINTER_TO) | static_cast<int64_t>(VarType::VEC);
                    bool is_vector_type = (type_value & vector_flags) == vector_flags;
                    
                    // Check for Strings (POINTER_TO | STRING)
                    int64_t string_flags = static_cast<int64_t>(VarType::POINTER_TO) | static_cast<int64_t>(VarType::STRING);
                    bool is_string_type = (type_value & string_flags) == string_flags;
                    
                    if ((is_vector_type || is_string_type) && should_perform_legacy_cleanup()) {
                        // For vectors and strings, call FREEVEC
                        debug_print("Generating FREEVEC(" + var_name + ") call");
                        auto var_expr = std::make_unique<VariableAccess>(var_name);
                        auto freevec_expr = std::make_unique<VariableAccess>("FREEVEC");
                        std::vector<ExprPtr> args;
                        args.push_back(std::move(var_expr));
                        auto free_call = std::make_unique<RoutineCallStatement>(std::move(freevec_expr), std::move(args));
                        cleanup_block->add_statement(std::move(free_call));
                    } else if (symbol.type == VarType::POINTER_TO_OBJECT && should_perform_legacy_cleanup()) {
                        // For objects, call var.RELEASE()
                        debug_print("Generating " + var_name + ".RELEASE() call");
                        auto var_expr = std::make_unique<VariableAccess>(var_name);
                        auto release_expr = std::make_unique<MemberAccessExpression>(std::move(var_expr), "RELEASE");
                        auto release_call = std::make_unique<RoutineCallStatement>(std::move(release_expr), std::vector<ExprPtr>{});
                        cleanup_block->add_statement(std::move(release_call));
                    } else if ((is_vector_type || is_string_type) || symbol.type == VarType::POINTER_TO_OBJECT) {
                        debug_print("Skipping legacy cleanup for " + var_name + " - SAMM is active");
                    }
                }
            }
        }
        
        current_basic_block = previous_current;
        last_cleanup_block = cleanup_block;
    }

    return last_cleanup_block;
}

void CFGBuilderPass::resolve_gotos() {
    if (trace_enabled_) {
        std::cout << "[CFGBuilderPass] Resolving " << unresolved_gotos_.size() << " GOTO statements..." << std::endl;
    }
    for (const auto& pair : unresolved_gotos_) {
        GotoStatement* goto_stmt = pair.first;
        BasicBlock* from_block = pair.second;

        if (auto* var_access = dynamic_cast<VariableAccess*>(goto_stmt->label_expr.get())) {
            const std::string& label_name = var_access->name;
            auto it = label_targets.find(label_name);
            if (it != label_targets.end()) {
                BasicBlock* to_block = it->second;
                current_cfg->add_edge(from_block, to_block);
                if (trace_enabled_) {
                    std::cout << "[CFGBuilderPass]   Added GOTO edge from " << from_block->id << " -> " << to_block->id << " (Label: " << label_name << ")" << std::endl;
                }
            } else {
                std::cerr << "[CFGBuilderPass] ERROR: GOTO target label '" << label_name << "' not found!" << std::endl;
            }
        } else {
             if (trace_enabled_) {
                std::cout << "[CFGBuilderPass]   Skipping edge creation for computed GOTO in block " << from_block->id << std::endl;
            }
        }
    }
}


// --- ASTVisitor Overrides ---

void CFGBuilderPass::visit(Program& node) {
    if (trace_enabled_) {
        std::cout << "[CFGBuilderPass] visit(Program) called." << std::endl;
    }
    int decl_idx = 0;
    for (const auto& decl : node.declarations) {
        if (trace_enabled_) {
            std::cout << "[CFGBuilderPass] Processing declaration #" << decl_idx << std::endl;
        }
        if (!decl) {
            std::cerr << "[CFGBuilderPass] WARNING: Null declaration at index " << decl_idx << std::endl;
            ++decl_idx;
            continue;
        }
        try {
            if (decl->getType() == ASTNode::NodeType::FunctionDecl) {
                if (trace_enabled_) std::cout << "[CFGBuilderPass] Found FunctionDecl at index " << decl_idx << std::endl;
                visit(static_cast<FunctionDeclaration&>(*decl));
            } else if (decl->getType() == ASTNode::NodeType::RoutineDecl) {
                if (trace_enabled_) std::cout << "[CFGBuilderPass] Found RoutineDecl at index " << decl_idx << std::endl;
                visit(static_cast<RoutineDeclaration&>(*decl));
            } else if (decl->getType() == ASTNode::NodeType::ClassDecl) {
                if (trace_enabled_) std::cout << "[CFGBuilderPass] Found ClassDecl at index " << decl_idx << std::endl;
                visit(static_cast<ClassDeclaration&>(*decl));
            }
        } catch (const std::exception& ex) {
            std::cerr << "[CFGBuilderPass] Exception in visit(Program) for declaration #" << decl_idx << ": " << ex.what() << std::endl;
            throw;
        } catch (...) {
            std::cerr << "[CFGBuilderPass] Unknown exception in visit(Program) for declaration #" << decl_idx << std::endl;
            throw;
        }
        ++decl_idx;
    }
    if (trace_enabled_) {
        std::cout << "[CFGBuilderPass] visit(Program) complete." << std::endl;
    }
}

void CFGBuilderPass::visit(FunctionDeclaration& node) {
    auto& analyzer = ASTAnalyzer::getInstance();
    std::string previous_scope = analyzer.get_current_function_scope();
    std::string mangled_name = node.name;
    if (!current_class_name_.empty()) {
        mangled_name = NameMangler::mangleMethod(current_class_name_, node.name);
        if (trace_enabled_) {
            std::cout << "[CFGBuilderPass] Using qualified name for method: " << mangled_name << std::endl;
        }
    }
    analyzer.set_current_function_scope(mangled_name);

    if (trace_enabled_) {
        std::cout << "[CFGBuilderPass] visit(FunctionDeclaration) for function: " << mangled_name << std::endl;
    }
    current_cfg = new ControlFlowGraph(mangled_name);
    function_cfgs[mangled_name].reset(current_cfg);
    
    if (trace_enabled_) {
        std::cout << "[CFGBuilderPass] Created CFG for function: " << mangled_name << std::endl;
    }

    block_id_counter = 0;
    label_targets.clear();
    unresolved_gotos_.clear();
    deferred_statements_.clear();

    current_basic_block = create_new_basic_block("Entry_");
    if (current_basic_block) {
        current_basic_block->is_entry = true;
        current_cfg->entry_block = current_basic_block;
    } else {
        std::cerr << "[CFGBuilderPass] ERROR: Failed to create entry block for function: " << node.name << std::endl;
        analyzer.set_current_function_scope(previous_scope);
        return;
    }

    if (node.body) {
        try {
            node.body->accept(*this);
        } catch (const std::exception& ex) {
            std::cerr << "[CFGBuilderPass] Exception in visit(FunctionDeclaration) body: " << ex.what() << std::endl;
            analyzer.set_current_function_scope(previous_scope);
            throw;
        } catch (...) {
            std::cerr << "[CFGBuilderPass] Unknown exception in visit(FunctionDeclaration) body" << std::endl;
            analyzer.set_current_function_scope(previous_scope);
            throw;
        }
    } else {
        std::cerr << "[CFGBuilderPass] WARNING: Function " << node.name << " has no body." << std::endl;
    }

    if (current_basic_block && !current_basic_block->ends_with_control_flow()) {
        if (!current_cfg->exit_block) {
            current_cfg->exit_block = create_new_basic_block("Exit_");
            if (current_cfg->exit_block) {
                current_cfg->exit_block->is_exit = true;
            } else {
                std::cerr << "[CFGBuilderPass] ERROR: Failed to create exit block for function: " << node.name << std::endl;
            }
        }
        if (current_cfg->exit_block) {
            current_cfg->add_edge(current_basic_block, current_cfg->exit_block);
        }
    }

    // --- DEFER: Insert cleanup block if needed ---
    if (!deferred_statements_.empty() && current_cfg->exit_block) {
        BasicBlock* original_exit_block = current_cfg->exit_block;
        BasicBlock* cleanup_block = create_new_basic_block("DeferCleanup_");

        // Populate the cleanup block in LIFO order.
        current_basic_block = cleanup_block;
        for (auto it = deferred_statements_.rbegin(); it != deferred_statements_.rend(); ++it) {
            if (*it) {
                (*it)->accept(*this);
            }
        }

        // Link the cleanup block to the original, final epilogue.
        current_cfg->add_edge(cleanup_block, original_exit_block);

        // Rewire all predecessors of the original exit block to point to our new cleanup block.
        std::vector<BasicBlock*> predecessors = original_exit_block->predecessors;
        original_exit_block->predecessors.clear();

        for (BasicBlock* pred : predecessors) {
            // If the predecessor is the cleanup block itself, leave its connection to the epilogue alone.
            if (pred == cleanup_block) {
                original_exit_block->predecessors.push_back(cleanup_block); // Re-add the valid connection
                continue;
            }

            // For all other predecessors, remove the old edge and add the new one.
            pred->successors.erase(
                std::remove(pred->successors.begin(), pred->successors.end(), original_exit_block),
                pred->successors.end()
            );
            current_cfg->add_edge(pred, cleanup_block);
        }
    }

    resolve_gotos();

    current_cfg = nullptr;
    current_basic_block = nullptr;
    analyzer.set_current_function_scope(previous_scope);
    if (trace_enabled_) {
        std::cout << "[CFGBuilderPass] visit(FunctionDeclaration) complete for function: " << node.name << std::endl;
    }
}

void CFGBuilderPass::visit(RoutineDeclaration& node) {
    auto& analyzer = ASTAnalyzer::getInstance();
    std::string previous_scope = analyzer.get_current_function_scope();
    std::string mangled_name = node.name;
    if (!current_class_name_.empty()) {
        mangled_name = NameMangler::mangleMethod(current_class_name_, node.name);
        if (trace_enabled_) {
            std::cout << "[CFGBuilderPass] Using qualified name for method: " << mangled_name << std::endl;
        }
    }
    analyzer.set_current_function_scope(mangled_name);

    if (trace_enabled_) {
        std::cout << "[CFGBuilderPass] Creating CFG for routine: " << mangled_name << std::endl;
    }
    current_cfg = new ControlFlowGraph(mangled_name);
    function_cfgs[mangled_name].reset(current_cfg);
    
    if (trace_enabled_) {
        std::cout << "[CFGBuilderPass] Created CFG for routine: " << mangled_name << std::endl;
    }

    block_id_counter = 0;
    label_targets.clear();
    unresolved_gotos_.clear();
    deferred_statements_.clear();

    current_basic_block = create_new_basic_block("Entry_");
    current_basic_block->is_entry = true;
    current_cfg->entry_block = current_basic_block;

    if (node.body) {
        node.body->accept(*this);
    }

    if (current_basic_block && !current_basic_block->ends_with_control_flow()) {
        if (!current_cfg->exit_block) {
            current_cfg->exit_block = create_new_basic_block("Exit_");
            current_cfg->exit_block->is_exit = true;
        }
        current_cfg->add_edge(current_basic_block, current_cfg->exit_block);
    }

    // --- DEFER: Insert cleanup block if needed ---
    if (!deferred_statements_.empty() && current_cfg->exit_block) {
        BasicBlock* original_exit_block = current_cfg->exit_block;
        BasicBlock* cleanup_block = create_new_basic_block("DeferCleanup_");

        // Populate the cleanup block in LIFO order.
        current_basic_block = cleanup_block;
        for (auto it = deferred_statements_.rbegin(); it != deferred_statements_.rend(); ++it) {
            if (*it) {
                (*it)->accept(*this);
            }
        }

        // Link the cleanup block to the original, final epilogue.
        current_cfg->add_edge(cleanup_block, original_exit_block);

        // Rewire all predecessors of the original exit block to point to our new cleanup block.
        std::vector<BasicBlock*> predecessors = original_exit_block->predecessors;
        original_exit_block->predecessors.clear();

        for (BasicBlock* pred : predecessors) {
            // If the predecessor is the cleanup block itself, leave its connection to the epilogue alone.
            if (pred == cleanup_block) {
                original_exit_block->predecessors.push_back(cleanup_block); // Re-add the valid connection
                continue;
            }

            // For all other predecessors, remove the old edge and add the new one.
            pred->successors.erase(
                std::remove(pred->successors.begin(), pred->successors.end(), original_exit_block),
                pred->successors.end()
            );
            current_cfg->add_edge(pred, cleanup_block);
        }
    }

    resolve_gotos();

    current_cfg = nullptr;
    current_basic_block = nullptr;
    analyzer.set_current_function_scope(previous_scope);
}

void CFGBuilderPass::visit(LetDeclaration& node) {
    // A LetDeclaration lowers to a series of assignments in the CFG.
    if (!current_basic_block) {
        end_current_block_and_start_new();
    }

    // For each variable in the declaration...
    for (size_t i = 0; i < node.names.size(); ++i) {
        const std::string& var_name = node.names[i];
        
        // Track this variable as declared in the current block
        if (!block_variable_stack.empty()) {
            block_variable_stack.back().push_back(var_name);
            debug_print("Tracking variable '" + var_name + "' in current block");
        }
        
        // if it has an initializer...
        if (i < node.initializers.size() && node.initializers[i]) {
            // ...create an AssignmentStatement for it.
            auto lhs = std::make_unique<VariableAccess>(var_name);
            
            std::vector<ExprPtr> lhs_vec;
            lhs_vec.push_back(std::move(lhs));

            std::vector<ExprPtr> rhs_vec;
            rhs_vec.push_back(clone_unique_ptr<Expression>(node.initializers[i]));

            auto assignment = std::make_unique<AssignmentStatement>(
                std::move(lhs_vec),
                std::move(rhs_vec)
            );
            // Add the new AssignmentStatement to the basic block.
            current_basic_block->add_statement(std::move(assignment));
        }
    }
}

// --- Other visit methods for statements and expressions ---

// --- DEFER support ---
void CFGBuilderPass::visit(DeferStatement& node) {
    // Take ownership of the deferred statement and add it to our list for later processing.
    deferred_statements_.push_back(std::move(node.deferred_statement));
}

void CFGBuilderPass::visit(AssignmentStatement& node) {
    if (!current_basic_block) end_current_block_and_start_new();
    current_basic_block->add_statement(std::unique_ptr<Statement>(static_cast<Statement*>(node.clone().release())));
    
    // --- FOREACH Optimization: Track VecInitializerExpression assignments ---
    if (node.lhs.size() == 1 && node.rhs.size() == 1) {
        if (auto* var_access = dynamic_cast<VariableAccess*>(node.lhs[0].get())) {
            if (auto* vec_init = dynamic_cast<VecInitializerExpression*>(node.rhs[0].get())) {
                constant_vector_sizes_[var_access->name] = vec_init->initializers.size();
                if (trace_enabled_) {
                    std::cout << "[CFGBuilderPass] OPTIMIZATION: Tracked variable '" << var_access->name 
                              << "' with constant size " << vec_init->initializers.size() << std::endl;
                }
            }
        }
    }
    
    for (const auto& lhs_expr : node.lhs) { if(lhs_expr) lhs_expr->accept(*this); }
    for (const auto& rhs_expr : node.rhs) { if(rhs_expr) rhs_expr->accept(*this); }
}

void CFGBuilderPass::visit(RoutineCallStatement& node) {
    if (!current_basic_block) end_current_block_and_start_new();
    current_basic_block->add_statement(std::unique_ptr<Statement>(static_cast<Statement*>(node.clone().release())));
    if(node.routine_expr) node.routine_expr->accept(*this);
    for (const auto& arg : node.arguments) { if(arg) arg->accept(*this); }
}

void CFGBuilderPass::visit(IfStatement& node) {
    if(node.condition) node.condition->accept(*this);
    if (!current_basic_block) end_current_block_and_start_new();
    current_basic_block->add_statement(std::unique_ptr<Statement>(static_cast<Statement*>(node.clone().release())));

    BasicBlock* condition_block = current_basic_block;
    BasicBlock* then_block = create_new_basic_block("Then_");
    BasicBlock* join_block = create_new_basic_block("Join_");

    current_cfg->add_edge(condition_block, then_block);
    current_cfg->add_edge(condition_block, join_block);

    current_basic_block = then_block;
    if(node.then_branch) node.then_branch->accept(*this);
    if (current_basic_block && !current_basic_block->ends_with_control_flow()) {
        current_cfg->add_edge(current_basic_block, join_block);
    }

    current_basic_block = join_block;
}

void CFGBuilderPass::visit(ForStatement& node) {
    if (trace_enabled_) std::cout << "[CFGBuilderPass] visit(ForStatement) entered." << std::endl;

    if (!current_basic_block) end_current_block_and_start_new();

    // Verify that both start and end expressions are present
    if (!node.start_expr || !node.end_expr) {
        std::string error = "ForStatement missing required ";
        error += !node.start_expr ? "start expression" : "end expression";
        std::cerr << "CFG ERROR: " << error << std::endl;
        throw std::runtime_error(error);
    }

    if (node.start_expr) node.start_expr->accept(*this);

    // Create an assignment statement for the initialization
    auto lhs_init = std::make_unique<VariableAccess>(node.unique_loop_variable_name);
    std::vector<ExprPtr> lhs_vec;
    lhs_vec.push_back(std::move(lhs_init));
    std::vector<ExprPtr> rhs_vec;
    rhs_vec.push_back(clone_unique_ptr(node.start_expr));
    current_basic_block->add_statement(std::make_unique<AssignmentStatement>(std::move(lhs_vec), std::move(rhs_vec)));

    BasicBlock* init_block = current_basic_block;
    BasicBlock* header_block = create_new_basic_block("ForHeader_");
    current_cfg->add_edge(init_block, header_block);

    BasicBlock* body_block = create_new_basic_block("ForBody_");
    BasicBlock* increment_block = create_new_basic_block("ForIncrement_");
    BasicBlock* exit_block = create_new_basic_block("ForExit_");

    break_targets.push_back(exit_block);
    loop_targets.push_back(increment_block);

    // Store the ForStatement in the header block for condition evaluation
    header_block->add_statement(std::unique_ptr<Statement>(static_cast<Statement*>(node.clone().release())));
    
    // Edge from header to body (when loop condition is true)
    current_cfg->add_edge(header_block, body_block);
    // Edge from header to exit (when loop condition is false)
    current_cfg->add_edge(header_block, exit_block);

    // Mark the header block specially to help detect infinite loops
    header_block->is_loop_header = true;
    header_block->loop_variable = node.unique_loop_variable_name;

    // Process the body statements
    current_basic_block = body_block;
    if (node.body) {
        node.body->accept(*this);
    }
    // Connect body to increment block
    if (current_basic_block) {
        current_cfg->add_edge(current_basic_block, increment_block);
    }

    // Build the increment block
    current_basic_block = increment_block;
    
    // Mark this block explicitly as an increment block to help NewCodeGenerator identify it
    increment_block->is_increment_block = true;
    increment_block->loop_variable = node.unique_loop_variable_name;
    
    // Create the increment assignment: loop_var = loop_var + step
    auto lhs_incr = std::make_unique<VariableAccess>(node.unique_loop_variable_name);
    std::vector<ExprPtr> lhs_incr_vec;
    lhs_incr_vec.push_back(std::move(lhs_incr));
    std::vector<ExprPtr> rhs_incr_vec;
    
    // Use step expression if provided, otherwise default to 1
    auto step_val = node.step_expr ? clone_unique_ptr(node.step_expr) : std::make_unique<NumberLiteral>(static_cast<int64_t>(1));
    
    // Create the increment operation: loop_var + step
    rhs_incr_vec.push_back(std::make_unique<BinaryOp>(
        BinaryOp::Operator::Add,
        std::make_unique<VariableAccess>(node.unique_loop_variable_name),
        std::move(step_val)
    ));
    
    // Add the assignment statement to the increment block
    auto incr_stmt = std::make_unique<AssignmentStatement>(std::move(lhs_incr_vec), std::move(rhs_incr_vec));
    
    // Store metadata in the statement to help with debugging
    incr_stmt->statement_role = "for_loop_increment";
    increment_block->add_statement(std::move(incr_stmt));
    
    // Add edge from increment block back to header for next iteration
    current_cfg->add_edge(increment_block, header_block);
    
    if (trace_enabled_) {
        std::cout << "[CFGBuilderPass] Created increment block: " << increment_block->id 
                  << " for loop variable: " << node.unique_loop_variable_name << std::endl;
    }

    current_basic_block = exit_block;
    loop_targets.pop_back();
    break_targets.pop_back();
    if (trace_enabled_) std::cout << "[CFGBuilderPass] visit(ForStatement) exiting." << std::endl;
}

// Helper method to validate the CFG structure, especially for loops
void CFGBuilderPass::validate_block_structure(const std::string& function_name) {
    if (!current_cfg) return;
    
    // Check for blocks with single self-referencing edges (infinite loops)
    for (const auto& block_entry : current_cfg->get_blocks()) {
        BasicBlock* block = block_entry.second.get();
        
        if (block->successors.size() == 1 && block->successors[0] == block) {
            std::string error = "Detected potential infinite loop in function '" + function_name + 
                               "' - block '" + block->id + "' points to itself";
            std::cerr << "CFG ERROR: " << error << std::endl;
            
            // Don't throw - just warn, as this might be intentional in some edge cases
            if (trace_enabled_) {
                std::cout << "[CFGBuilderPass] WARNING: " << error << std::endl;
            }
        }
        
        // Check increment blocks specifically
        if (block->is_increment_block) {
            bool has_increment_stmt = false;
            for (const auto& stmt : block->statements) {
                if (auto assign = dynamic_cast<const AssignmentStatement*>(stmt.get())) {
                    has_increment_stmt = true;
                    break;
                }
            }
            
            if (!has_increment_stmt && trace_enabled_) {
                std::cout << "[CFGBuilderPass] WARNING: Increment block " << block->id 
                          << " does not contain an assignment statement" << std::endl;
            }
        }
    }
}

void CFGBuilderPass::visit(ForEachStatement& node) {
    if (trace_enabled_) std::cout << "[CFGBuilderPass] visit(ForEachStatement) entered." << std::endl;

    if (!current_basic_block) end_current_block_and_start_new();



    // Determine the type of the collection expression (respects SETTYPE and inference)
    auto& analyzer = ASTAnalyzer::getInstance();
    VarType collection_type = analyzer.infer_expression_type(node.collection_expression.get());
    bool is_list = false;

    // Refactored: Use bitwise logic to detect pointer-to-list types
    is_list = (static_cast<int64_t>(collection_type) & (static_cast<int64_t>(VarType::POINTER_TO) | static_cast<int64_t>(VarType::LIST)))
              == (static_cast<int64_t>(VarType::POINTER_TO) | static_cast<int64_t>(VarType::LIST));

    if (is_list) {
        build_list_foreach_cfg(node);
    } else {
        build_vector_foreach_cfg(node);
    }

    // FIX: Pop the break and loop targets from the stack after the FOREACH loop
    // has been fully processed. This must be done for ALL collection types.
    if (!break_targets.empty()) {
        break_targets.pop_back();
    }
    if (!loop_targets.empty()) {
        loop_targets.pop_back();
    }



    if (trace_enabled_) std::cout << "[CFGBuilderPass] visit(ForEachStatement) exiting." << std::endl;
}


void CFGBuilderPass::visit(ReturnStatement& node) {
    if (!current_basic_block) end_current_block_and_start_new();
    
    debug_print("SAMM: Visiting ReturnStatement - SAMM disabled, no scope exit call injected.");
    
    // SAMM: DISABLED - Inject scope exit call before return
    // auto exit_scope_expr = std::make_unique<VariableAccess>("HeapManager_exit_scope");
    // auto exit_scope_call = std::make_unique<RoutineCallStatement>(std::move(exit_scope_expr), std::vector<ExprPtr>{});
    // current_basic_block->add_statement(std::move(exit_scope_call));
    
    debug_print("Visiting ReturnStatement - injecting scope cleanup.");
    
    // Generate cleanup code for all variables in scope before returning
    BasicBlock* final_cleanup_block = generate_scope_cleanup_chain("ReturnCleanup_");
    
    // Add the original RETURN statement to the final cleanup block
    if (final_cleanup_block) {
        BasicBlock* previous_current = current_basic_block;
        current_basic_block = final_cleanup_block;
        current_basic_block->add_statement(std::unique_ptr<Statement>(static_cast<Statement*>(node.clone().release())));
        current_basic_block = previous_current;
    } else {
        current_basic_block->add_statement(std::unique_ptr<Statement>(static_cast<Statement*>(node.clone().release())));
        final_cleanup_block = current_basic_block;
    }
    
    if (!current_cfg->exit_block) {
        current_cfg->exit_block = create_new_basic_block("Exit_");
        current_cfg->exit_block->is_exit = true;
    }
    current_cfg->add_edge(final_cleanup_block, current_cfg->exit_block);
    current_basic_block = nullptr;
}

void CFGBuilderPass::visit(ResultisStatement& node) {
    if (!current_basic_block) end_current_block_and_start_new();
    
    debug_print("SAMM: Visiting ResultisStatement - SAMM disabled, no scope exit call injected.");
    
    // SAMM: DISABLED - Inject scope exit call before resultis
    // auto exit_scope_expr = std::make_unique<VariableAccess>("HeapManager_exit_scope");
    // auto exit_scope_call = std::make_unique<RoutineCallStatement>(std::move(exit_scope_expr), std::vector<ExprPtr>{});
    // current_basic_block->add_statement(std::move(exit_scope_call));
    
    debug_print("Visiting ResultisStatement - injecting scope cleanup.");
    
    // Generate cleanup code for all variables in scope before exiting with result
    BasicBlock* final_cleanup_block = generate_scope_cleanup_chain("ResultisCleanup_");
    
    // Add the original RESULTIS statement to the final cleanup block
    if (final_cleanup_block) {
        BasicBlock* previous_current = current_basic_block;
        current_basic_block = final_cleanup_block;
        current_basic_block->add_statement(std::unique_ptr<Statement>(static_cast<Statement*>(node.clone().release())));
        current_basic_block = previous_current;
    } else {
        current_basic_block->add_statement(std::unique_ptr<Statement>(static_cast<Statement*>(node.clone().release())));
        final_cleanup_block = current_basic_block;
    }
    
    if (!current_cfg->exit_block) {
        current_cfg->exit_block = create_new_basic_block("Exit_");
        current_cfg->exit_block->is_exit = true;
    }
    current_cfg->add_edge(final_cleanup_block, current_cfg->exit_block);
    current_basic_block = nullptr;
}

void CFGBuilderPass::visit(CompoundStatement& node) {
    for (const auto& stmt : node.statements) {
        if (!stmt) continue;
        if (current_basic_block == nullptr) {
            current_basic_block = create_new_basic_block();
        }
        stmt->accept(*this);
    }
}

void CFGBuilderPass::visit(BlockStatement& node) {
    if (trace_enabled_) {
        std::cout << "[CFGBuilderPass] Visiting BlockStatement with SAMM scope management." << std::endl;
    }

    // --- SAMM LEAF FUNCTION OPTIMIZATION ---
    // Check if this function is a leaf function that performs no heap allocations
    auto& analyzer = ASTAnalyzer::getInstance();
    if (current_cfg && !current_cfg->function_name.empty()) {
        const auto& metrics_map = analyzer.get_function_metrics();
        auto metrics_it = metrics_map.find(current_cfg->function_name);
        
        if (metrics_it != metrics_map.end()) {
            const auto& metrics = metrics_it->second;
            
            // OPTIMIZATION: Skip all SAMM calls for allocation-free functions (leaf or entire call trees)
            if (!metrics.performs_heap_allocation) {
                if (trace_enabled_) {
                    std::cout << "[SAMM OPTIMIZATION] Skipping scope calls for allocation-free function: " 
                              << current_cfg->function_name 
                              << (metrics.is_leaf ? " (leaf)" : " (call tree)") << std::endl;
                }
                
                // Just process the block's contents without injecting SAMM calls
                if (symbol_table_) {
                    symbol_table_->enterScope();
                }
                
                // Start tracking variables for this block (still needed for register allocation)
                int this_block_id = ++current_block_id_counter;
                block_variable_stack.push_back(std::vector<std::string>());
                debug_print("Starting optimized block " + std::to_string(this_block_id) + " variable tracking");

                // Process declarations and statements normally
                for (const auto& decl : node.declarations) {
                    if (decl) decl->accept(*this);
                }
                for (const auto& stmt : node.statements) {
                    if (stmt) stmt->accept(*this);
                }
                
                // Clean up block tracking
                if (!block_variable_stack.empty()) {
                    block_variable_stack.pop_back();
                }
                
                if (symbol_table_) {
                    symbol_table_->exitScope();
                }
                
                return; // Exit early - no SAMM calls needed!
            }
        }
    }

    // --- ORIGINAL SAMM LOGIC (for functions that DO need tracking) ---
    if (!current_basic_block) end_current_block_and_start_new();
    
    debug_print("SAMM: Injecting HeapManager_enter_scope() call at block start");
    auto enter_scope_expr = std::make_unique<VariableAccess>("HeapManager_enter_scope");
    auto enter_scope_call = std::make_unique<RoutineCallStatement>(std::move(enter_scope_expr), std::vector<ExprPtr>{});
    current_basic_block->add_statement(std::move(enter_scope_call));
    
    // Register the injected call site with ASTAnalyzer
    if (current_cfg && !current_cfg->function_name.empty()) {
        auto& metrics = analyzer.get_function_metrics_mut()[current_cfg->function_name];
        int call_point = metrics.instruction_count++;
        analyzer.register_call_site(current_cfg->function_name, call_point);
        debug_print("CFGBuilderPass: Registered HeapManager_enter_scope call site at instruction " + 
                   std::to_string(call_point) + " for function '" + current_cfg->function_name + "'");
    }

    // --- Synchronize SymbolTable scope ---
    if (symbol_table_) {
        symbol_table_->enterScope();
    }
    
    // --- Start tracking variables for this block ---
    int this_block_id = ++current_block_id_counter;
    block_variable_stack.push_back(std::vector<std::string>()); // New empty list for this block
    debug_print("Starting block " + std::to_string(this_block_id) + " variable tracking");

    // First, process all DECLARATIONS.
    for (const auto& decl : node.declarations) {
        if (decl) {
            decl->accept(*this);
        }
    }

    // Then, process all STATEMENTS.
    for (const auto& stmt : node.statements) {
        if (stmt) {
            stmt->accept(*this);
        }
    }

    // --- BLOCK-END CLEANUP FOR NORMAL CONTROL FLOW ---
    // This handles cleanup when control flow falls through the end of a block normally.
    // Early exits (RETURN, BREAK, GOTO, etc.) are handled by the respective statement visitors
    // using generate_scope_cleanup_chain().
    if (symbol_table_ && !block_variable_stack.empty()) {
        // Get the list of variables declared in this block
        std::vector<std::string> variables_in_this_block = block_variable_stack.back();
        block_variable_stack.pop_back(); // Remove this block from the stack
        
        if (!variables_in_this_block.empty() && current_basic_block && !current_basic_block->ends_with_control_flow()) {
            debug_print("Generating normal block-end cleanup for " + std::to_string(variables_in_this_block.size()) + " variables");
            
            // For normal fall-through, we can generate cleanup directly in the current block
            // since there are no intervening control flow statements
            for (auto it = variables_in_this_block.rbegin(); it != variables_in_this_block.rend(); ++it) {
                const std::string& var_name = *it;
                
                // Look up the symbol to get its type
                Symbol symbol;
                if (symbol_table_->lookup(var_name, symbol) && symbol.owns_heap_memory) {
                    debug_print("Generating block-end cleanup for: " + var_name);
                    
                    // Use bitwise operations: list types have both POINTER_TO and LIST flags set
                    int64_t type_value = static_cast<int64_t>(symbol.type);
                    int64_t list_flags = static_cast<int64_t>(VarType::POINTER_TO) | static_cast<int64_t>(VarType::LIST);
                    bool is_list_type = (type_value & list_flags) == list_flags;
                    
                    if (is_list_type && should_perform_legacy_cleanup()) {
                        debug_print("Generating BCPL_FREE_LIST(" + var_name + ") call");
                        auto var_expr = std::make_unique<VariableAccess>(var_name);
                        auto free_list_expr = std::make_unique<VariableAccess>("BCPL_FREE_LIST");
                        std::vector<ExprPtr> args;
                        args.push_back(std::move(var_expr));
                        auto free_call = std::make_unique<RoutineCallStatement>(std::move(free_list_expr), std::move(args));
                        current_basic_block->add_statement(std::move(free_call));
                    } else if (is_list_type) {
                        debug_print("Skipping BCPL_FREE_LIST(" + var_name + ") - SAMM is active");
                    }
                    // Check for Vectors (POINTER_TO | VEC)
                    else {
                        int64_t vector_flags = static_cast<int64_t>(VarType::POINTER_TO) | static_cast<int64_t>(VarType::VEC);
                        bool is_vector_type = (type_value & vector_flags) == vector_flags;
                        
                        // Check for Strings (POINTER_TO | STRING)
                        int64_t string_flags = static_cast<int64_t>(VarType::POINTER_TO) | static_cast<int64_t>(VarType::STRING);
                        bool is_string_type = (type_value & string_flags) == string_flags;
                        
                        if ((is_vector_type || is_string_type) && should_perform_legacy_cleanup()) {
                            // For vectors and strings, call FREEVEC
                            debug_print("Generating FREEVEC(" + var_name + ") call");
                            auto var_expr = std::make_unique<VariableAccess>(var_name);
                            auto freevec_expr = std::make_unique<VariableAccess>("FREEVEC");
                            std::vector<ExprPtr> args;
                            args.push_back(std::move(var_expr));
                            auto free_call = std::make_unique<RoutineCallStatement>(std::move(freevec_expr), std::move(args));
                            current_basic_block->add_statement(std::move(free_call));
                        } else if (symbol.type == VarType::POINTER_TO_OBJECT && should_perform_legacy_cleanup()) {
                            // For objects, call var.RELEASE()
                            debug_print("Generating " + var_name + ".RELEASE() call");
                            auto var_expr = std::make_unique<VariableAccess>(var_name);
                            auto release_expr = std::make_unique<MemberAccessExpression>(std::move(var_expr), "RELEASE");
                            auto release_call = std::make_unique<RoutineCallStatement>(std::move(release_expr), std::vector<ExprPtr>{});
                            current_basic_block->add_statement(std::move(release_call));
                        } else if ((is_vector_type || is_string_type) || symbol.type == VarType::POINTER_TO_OBJECT) {
                            debug_print("Skipping legacy cleanup for " + var_name + " - SAMM is active");
                        }
                    }
                }
            }
        }
    }

    // --- SAMM: Inject scope exit call for normal block exit ---
    if (current_basic_block && !current_basic_block->ends_with_control_flow()) {
        debug_print("SAMM: Injecting HeapManager_exit_scope() call for normal block exit");
        auto exit_scope_expr = std::make_unique<VariableAccess>("HeapManager_exit_scope");
        auto exit_scope_call = std::make_unique<RoutineCallStatement>(std::move(exit_scope_expr), std::vector<ExprPtr>{});
        current_basic_block->add_statement(std::move(exit_scope_call));
        
        // Register the injected call site with ASTAnalyzer
        auto& analyzer = ASTAnalyzer::getInstance();
        if (current_cfg && !current_cfg->function_name.empty()) {
            auto& metrics = analyzer.get_function_metrics_mut()[current_cfg->function_name];
            int call_point = metrics.instruction_count++;
            analyzer.register_call_site(current_cfg->function_name, call_point);
            debug_print("CFGBuilderPass: Registered HeapManager_exit_scope call site at instruction " + 
                       std::to_string(call_point) + " for function '" + current_cfg->function_name + "'");
        }
    }

    // --- Synchronize SymbolTable scope on exit ---
    if (symbol_table_) {
        symbol_table_->exitScope();
    }
}


void CFGBuilderPass::visit(LabelTargetStatement& node) {
    if (trace_enabled_) {
        std::cout << "[CFGBuilderPass] Visiting LabelTargetStatement: " << node.labelName << std::endl;
    }

    BasicBlock* predecessor_block = current_basic_block;
    BasicBlock* label_block = create_new_basic_block("Label_" + node.labelName);
    label_targets[node.labelName] = label_block;

    if (predecessor_block && !predecessor_block->ends_with_control_flow()) {
        current_cfg->add_edge(predecessor_block, label_block);
    }

    current_basic_block = label_block;
    current_basic_block->add_statement(std::unique_ptr<Statement>(static_cast<Statement*>(node.clone().release())));
}

// Add stubs for other visit methods to prevent compiler warnings/errors
// These can be filled in as needed.
void CFGBuilderPass::visit(ManifestDeclaration& node) {}
void CFGBuilderPass::visit(StaticDeclaration& node) { if (node.initializer) node.initializer->accept(*this); }
void CFGBuilderPass::visit(GlobalDeclaration& node) {}
void CFGBuilderPass::visit(GlobalVariableDeclaration& node) {}
void CFGBuilderPass::visit(NumberLiteral& node) {}
void CFGBuilderPass::visit(StringLiteral& node) {}
void CFGBuilderPass::visit(CharLiteral& node) {}
void CFGBuilderPass::visit(BooleanLiteral& node) {}
void CFGBuilderPass::visit(VariableAccess& node) {}
void CFGBuilderPass::visit(BinaryOp& node) { if(node.left) node.left->accept(*this); if(node.right) node.right->accept(*this); }
void CFGBuilderPass::visit(UnaryOp& node) { if(node.operand) node.operand->accept(*this); }
void CFGBuilderPass::visit(VectorAccess& node) { if(node.vector_expr) node.vector_expr->accept(*this); if(node.index_expr) node.index_expr->accept(*this); }
void CFGBuilderPass::visit(CharIndirection& node) { if(node.string_expr) node.string_expr->accept(*this); if(node.index_expr) node.index_expr->accept(*this); }
void CFGBuilderPass::visit(FloatVectorIndirection& node) { if(node.vector_expr) node.vector_expr->accept(*this); if(node.index_expr) node.index_expr->accept(*this); }
void CFGBuilderPass::visit(FunctionCall& node) { if(node.function_expr) node.function_expr->accept(*this); for(const auto& arg : node.arguments) { if(arg) arg->accept(*this); } }
void CFGBuilderPass::visit(ConditionalExpression& node) { if(node.condition) node.condition->accept(*this); if(node.true_expr) node.true_expr->accept(*this); if(node.false_expr) node.false_expr->accept(*this); }
void CFGBuilderPass::visit(ValofExpression& node) { if(node.body) node.body->accept(*this); }
void CFGBuilderPass::visit(FloatValofExpression& node) { if(node.body) node.body->accept(*this); }
void CFGBuilderPass::visit(UnlessStatement& node) { /* Similar to IfStatement */ }
void CFGBuilderPass::visit(TestStatement& node) {
    if (trace_enabled_) {
        std::cout << "[CFGBuilderPass] visit(TestStatement) entered." << std::endl;
    }

    if (!current_basic_block) {
        end_current_block_and_start_new();
    }

    // 1. Add a clone of the TEST node's condition to the current block.
    // This allows the code generator to evaluate the condition before branching.
    current_basic_block->add_statement(std::unique_ptr<Statement>(static_cast<Statement*>(node.clone().release())));

    // 2. Create the necessary blocks for the two branches and the join point.
    BasicBlock* condition_block = current_basic_block;
    BasicBlock* then_block = create_new_basic_block("Then_");
    BasicBlock* else_block = create_new_basic_block("Else_");
    BasicBlock* join_block = create_new_basic_block("Join_");

    // 3. Add edges from the condition block to the two branches.
    // The code generator will use this to create the conditional branch.
    current_cfg->add_edge(condition_block, then_block);
    current_cfg->add_edge(condition_block, else_block);

    // 4. Process the 'then' branch.
    current_basic_block = then_block;
    if (node.then_branch) {
        node.then_branch->accept(*this);
    }
    // If the 'then' branch doesn't end with a GOTO or RETURN, it flows to the join block.
    if (current_basic_block && !current_basic_block->ends_with_control_flow()) {
        current_cfg->add_edge(current_basic_block, join_block);
    }

    // 5. Process the 'else' branch.
    current_basic_block = else_block;
    if (node.else_branch) {
        node.else_branch->accept(*this);
    }
    // The 'else' branch also flows to the join block.
    if (current_basic_block && !current_basic_block->ends_with_control_flow()) {
        current_cfg->add_edge(current_basic_block, join_block);
    }

    // 6. The new current block for subsequent statements is the join block.
    current_basic_block = join_block;

    if (trace_enabled_) {
        std::cout << "[CFGBuilderPass] visit(TestStatement) exiting." << std::endl;
    }
}
void CFGBuilderPass::visit(WhileStatement& node) {
    if (trace_enabled_) std::cout << "[CFGBuilderPass] visit(WhileStatement) entered." << std::endl;

    if (!current_basic_block) end_current_block_and_start_new();

    // Verify that condition is present
    if (!node.condition) {
        std::string error = "WhileStatement missing required condition expression";
        std::cerr << "CFG ERROR: " << error << std::endl;
        throw std::runtime_error(error);
    }

    // End the current block and create the loop structure
    BasicBlock* pre_loop_block = current_basic_block;
    BasicBlock* header_block = create_new_basic_block("WhileHeader_");
    current_cfg->add_edge(pre_loop_block, header_block);

    BasicBlock* body_block = create_new_basic_block("WhileBody_");
    BasicBlock* exit_block = create_new_basic_block("WhileExit_");

    // Set up break/loop targets for nested break/loop statements
    break_targets.push_back(exit_block);
    loop_targets.push_back(header_block); // LOOP goes back to condition check

    // Store the WhileStatement in the header block for condition evaluation
    header_block->add_statement(std::unique_ptr<Statement>(static_cast<Statement*>(node.clone().release())));
    
    // Edge from header to body (when loop condition is true)
    current_cfg->add_edge(header_block, body_block);
    // Edge from header to exit (when loop condition is false)
    current_cfg->add_edge(header_block, exit_block);

    // Mark the header block specially to help detect infinite loops
    header_block->is_loop_header = true;

    // Process the body statements
    current_basic_block = body_block;
    if (node.body) {
        node.body->accept(*this);
    }
    
    // Connect body back to header for next iteration (if current_basic_block still exists)
    if (current_basic_block) {
        current_cfg->add_edge(current_basic_block, header_block);
    }

    // Continue with exit block
    current_basic_block = exit_block;
    
    // Clean up break/loop targets
    loop_targets.pop_back();
    break_targets.pop_back();
    
    if (trace_enabled_) std::cout << "[CFGBuilderPass] visit(WhileStatement) exiting." << std::endl;
}
void CFGBuilderPass::visit(UntilStatement& node) { /* Similar to WhileStatement */ }
void CFGBuilderPass::visit(RepeatStatement& node) { /* Loop construct */ }
void CFGBuilderPass::visit(SwitchonStatement& node) {
    if (trace_enabled_) std::cout << "[CFGBuilderPass] visit(SwitchonStatement) entered." << std::endl;

    // --- START OF FIX ---
    // Finalize the current block (which contains the FOREACH setup) and start a new,
    // dedicated block for the SWITCHON's branching logic.
    end_current_block_and_start_new();
    // --- END OF FIX ---

    // 1. Add a clone of the SWITCHON node to the new, current block.
    current_basic_block->add_statement(std::unique_ptr<Statement>(static_cast<Statement*>(node.clone().release())));

    // 2. Create the necessary blocks for control flow.
    BasicBlock* switch_header_block = current_basic_block;
    BasicBlock* join_block = create_new_basic_block("SwitchJoin_");
    endcase_targets.push_back(join_block); // Register the join block for ENDCASE statements.

    std::vector<BasicBlock*> case_blocks;
    for (size_t i = 0; i < node.cases.size(); ++i) {
        case_blocks.push_back(create_new_basic_block("Case_" + std::to_string(i) + "_"));
    }

    BasicBlock* default_block = nullptr;
    if (node.default_case) {
        default_block = create_new_basic_block("DefaultCase_");
    }

    // 3. Add edges from the header to all possible branches.
    for (BasicBlock* case_block : case_blocks) {
        current_cfg->add_edge(switch_header_block, case_block);
    }
    if (default_block) {
        current_cfg->add_edge(switch_header_block, default_block);
    }
    // The final successor is the join block, which acts as the default if no default case exists.
    current_cfg->add_edge(switch_header_block, join_block);

    // 4. Visit the command within each case, setting the current block appropriately.
    for (size_t i = 0; i < node.cases.size(); ++i) {
        current_basic_block = case_blocks[i];
        if (node.cases[i]->command) {
            node.cases[i]->command->accept(*this);
        }
        // If a case doesn't end with a GOTO, RETURN, or ENDCASE, it must fall through to the join block.
        if (current_basic_block && !current_basic_block->ends_with_control_flow()) {
            current_cfg->add_edge(current_basic_block, join_block);
        }
    }

    // 5. Visit the default case command.
    if (default_block) {
        current_basic_block = default_block;
        if (node.default_case->command) {
            node.default_case->command->accept(*this);
        }
        if (current_basic_block && !current_basic_block->ends_with_control_flow()) {
            current_cfg->add_edge(current_basic_block, join_block);
        }
    }

    // 6. The new current block for any code following the switch is the join block.
    current_basic_block = join_block;
    endcase_targets.pop_back();

    if (trace_enabled_) std::cout << "[CFGBuilderPass] visit(SwitchonStatement) exiting." << std::endl;
}
void CFGBuilderPass::visit(CaseStatement& node) {}
void CFGBuilderPass::visit(DefaultStatement& node) {}
void CFGBuilderPass::visit(GotoStatement& node) { 
    if (!current_basic_block) end_current_block_and_start_new(); 
    
    debug_print("SAMM: Visiting GotoStatement - injecting scope exit call.");
    
    // SAMM: Inject scope exit call before goto
    auto exit_scope_expr = std::make_unique<VariableAccess>("HeapManager_exit_scope");
    auto exit_scope_call = std::make_unique<RoutineCallStatement>(std::move(exit_scope_expr), std::vector<ExprPtr>{});
    current_basic_block->add_statement(std::move(exit_scope_call));
    
    // Register the injected call site with ASTAnalyzer
    auto& analyzer = ASTAnalyzer::getInstance();
    if (current_cfg && !current_cfg->function_name.empty()) {
        auto& metrics = analyzer.get_function_metrics_mut()[current_cfg->function_name];
        int call_point = metrics.instruction_count++;
        analyzer.register_call_site(current_cfg->function_name, call_point);
        debug_print("CFGBuilderPass: Registered HeapManager_exit_scope call site at instruction " + 
                   std::to_string(call_point) + " for function '" + current_cfg->function_name + "'");
    }
    
    debug_print("Visiting GotoStatement - injecting scope cleanup.");
    
    // Generate cleanup code for all variables in scope before jumping
    BasicBlock* final_cleanup_block = generate_scope_cleanup_chain("GotoCleanup_");
    
    // Add the original GOTO statement to the final cleanup block
    if (final_cleanup_block) {
        BasicBlock* previous_current = current_basic_block;
        current_basic_block = final_cleanup_block;
        current_basic_block->add_statement(std::unique_ptr<Statement>(static_cast<Statement*>(node.clone().release())));
        unresolved_gotos_.push_back({&node, final_cleanup_block});
        current_basic_block = previous_current;
    } else {
        current_basic_block->add_statement(std::unique_ptr<Statement>(static_cast<Statement*>(node.clone().release())));
        unresolved_gotos_.push_back({&node, current_basic_block});
        final_cleanup_block = current_basic_block;
    }
    
    current_basic_block = nullptr; 
}

void CFGBuilderPass::visit(FinishStatement& node) { 
    if (!current_basic_block) end_current_block_and_start_new(); 
    
    debug_print("SAMM: Visiting FinishStatement - injecting scope exit call.");
    
    // SAMM: Inject scope exit call before finish
    auto exit_scope_expr = std::make_unique<VariableAccess>("HeapManager_exit_scope");
    auto exit_scope_call = std::make_unique<RoutineCallStatement>(std::move(exit_scope_expr), std::vector<ExprPtr>{});
    current_basic_block->add_statement(std::move(exit_scope_call));
    
    // Register the injected call site with ASTAnalyzer
    auto& analyzer = ASTAnalyzer::getInstance();
    if (current_cfg && !current_cfg->function_name.empty()) {
        auto& metrics = analyzer.get_function_metrics_mut()[current_cfg->function_name];
        int call_point = metrics.instruction_count++;
        analyzer.register_call_site(current_cfg->function_name, call_point);
        debug_print("CFGBuilderPass: Registered HeapManager_exit_scope call site at instruction " + 
                   std::to_string(call_point) + " for function '" + current_cfg->function_name + "'");
    }
    
    debug_print("Visiting FinishStatement - injecting scope cleanup.");
    
    // Generate cleanup code for all variables in scope before finishing
    BasicBlock* final_cleanup_block = generate_scope_cleanup_chain("FinishCleanup_");
    
    // Add the original FINISH statement to the final cleanup block
    if (final_cleanup_block) {
        BasicBlock* previous_current = current_basic_block;
        current_basic_block = final_cleanup_block;
        current_basic_block->add_statement(std::unique_ptr<Statement>(static_cast<Statement*>(node.clone().release())));
        current_basic_block = previous_current;
    } else {
        current_basic_block->add_statement(std::unique_ptr<Statement>(static_cast<Statement*>(node.clone().release())));
        final_cleanup_block = current_basic_block;
    }
    
    current_basic_block = nullptr; 
}

void CFGBuilderPass::visit(BreakStatement& node) { 
    if (!current_basic_block) end_current_block_and_start_new(); 
    
    debug_print("SAMM: Visiting BreakStatement - injecting scope exit call.");
    
    // SAMM: Inject scope exit call before break
    auto exit_scope_expr = std::make_unique<VariableAccess>("HeapManager_exit_scope");
    auto exit_scope_call = std::make_unique<RoutineCallStatement>(std::move(exit_scope_expr), std::vector<ExprPtr>{});
    current_basic_block->add_statement(std::move(exit_scope_call));
    
    // Register the injected call site with ASTAnalyzer
    auto& analyzer = ASTAnalyzer::getInstance();
    if (current_cfg && !current_cfg->function_name.empty()) {
        auto& metrics = analyzer.get_function_metrics_mut()[current_cfg->function_name];
        int call_point = metrics.instruction_count++;
        analyzer.register_call_site(current_cfg->function_name, call_point);
        debug_print("CFGBuilderPass: Registered HeapManager_exit_scope call site at instruction " + 
                   std::to_string(call_point) + " for function '" + current_cfg->function_name + "'");
    }
    
    debug_print("Visiting BreakStatement - injecting scope cleanup.");
    
    // Generate cleanup code for all variables in scope before breaking
    BasicBlock* final_cleanup_block = generate_scope_cleanup_chain("BreakCleanup_");
    
    // Add the original BREAK statement to the final cleanup block
    if (final_cleanup_block) {
        BasicBlock* previous_current = current_basic_block;
        current_basic_block = final_cleanup_block;
        current_basic_block->add_statement(std::unique_ptr<Statement>(static_cast<Statement*>(node.clone().release())));
        current_basic_block = previous_current;
    } else {
        current_basic_block->add_statement(std::unique_ptr<Statement>(static_cast<Statement*>(node.clone().release())));
        final_cleanup_block = current_basic_block;
    }
    
    if (!break_targets.empty()) current_cfg->add_edge(final_cleanup_block, break_targets.back()); 
    current_basic_block = nullptr; 
}

void CFGBuilderPass::visit(LoopStatement& node) { 
    if (!current_basic_block) end_current_block_and_start_new(); 
    
    debug_print("SAMM: Visiting LoopStatement - injecting scope exit call.");
    
    // SAMM: Inject scope exit call before loop
    auto exit_scope_expr = std::make_unique<VariableAccess>("HeapManager_exit_scope");
    auto exit_scope_call = std::make_unique<RoutineCallStatement>(std::move(exit_scope_expr), std::vector<ExprPtr>{});
    current_basic_block->add_statement(std::move(exit_scope_call));
    
    // Register the injected call site with ASTAnalyzer
    auto& analyzer = ASTAnalyzer::getInstance();
    if (current_cfg && !current_cfg->function_name.empty()) {
        auto& metrics = analyzer.get_function_metrics_mut()[current_cfg->function_name];
        int call_point = metrics.instruction_count++;
        analyzer.register_call_site(current_cfg->function_name, call_point);
        debug_print("CFGBuilderPass: Registered HeapManager_exit_scope call site at instruction " + 
                   std::to_string(call_point) + " for function '" + current_cfg->function_name + "'");
    }
    
    debug_print("Visiting LoopStatement - injecting scope cleanup.");
    
    // Generate cleanup code for all variables in scope before looping
    BasicBlock* final_cleanup_block = generate_scope_cleanup_chain("LoopCleanup_");
    
    // Add the original LOOP statement to the final cleanup block
    if (final_cleanup_block) {
        BasicBlock* previous_current = current_basic_block;
        current_basic_block = final_cleanup_block;
        current_basic_block->add_statement(std::unique_ptr<Statement>(static_cast<Statement*>(node.clone().release())));
        current_basic_block = previous_current;
    } else {
        current_basic_block->add_statement(std::unique_ptr<Statement>(static_cast<Statement*>(node.clone().release())));
        final_cleanup_block = current_basic_block;
    }
    
    if (!loop_targets.empty()) current_cfg->add_edge(final_cleanup_block, loop_targets.back()); 
    current_basic_block = nullptr; 
}

void CFGBuilderPass::visit(EndcaseStatement& node) { 
    if (!current_basic_block) end_current_block_and_start_new(); 
    
    debug_print("SAMM: Visiting EndcaseStatement - injecting scope exit call.");
    
    // SAMM: Inject scope exit call before endcase
    auto exit_scope_expr = std::make_unique<VariableAccess>("HeapManager_exit_scope");
    auto exit_scope_call = std::make_unique<RoutineCallStatement>(std::move(exit_scope_expr), std::vector<ExprPtr>{});
    current_basic_block->add_statement(std::move(exit_scope_call));
    
    // Register the injected call site with ASTAnalyzer
    auto& analyzer = ASTAnalyzer::getInstance();
    if (current_cfg && !current_cfg->function_name.empty()) {
        auto& metrics = analyzer.get_function_metrics_mut()[current_cfg->function_name];
        int call_point = metrics.instruction_count++;
        analyzer.register_call_site(current_cfg->function_name, call_point);
        debug_print("CFGBuilderPass: Registered HeapManager_exit_scope call site at instruction " + 
                   std::to_string(call_point) + " for function '" + current_cfg->function_name + "'");
    }
    
    debug_print("Visiting EndcaseStatement - injecting scope cleanup.");
    
    // Generate cleanup code for all variables in scope before ending case
    BasicBlock* final_cleanup_block = generate_scope_cleanup_chain("EndcaseCleanup_");
    
    // Add the original ENDCASE statement to the final cleanup block
    if (final_cleanup_block) {
        BasicBlock* previous_current = current_basic_block;
        current_basic_block = final_cleanup_block;
        current_basic_block->add_statement(std::unique_ptr<Statement>(static_cast<Statement*>(node.clone().release())));
        current_basic_block = previous_current;
    } else {
        current_basic_block->add_statement(std::unique_ptr<Statement>(static_cast<Statement*>(node.clone().release())));
        final_cleanup_block = current_basic_block;
    }
    
    if (!endcase_targets.empty()) current_cfg->add_edge(final_cleanup_block, endcase_targets.back()); 
    current_basic_block = nullptr; 
}
void CFGBuilderPass::visit(StringStatement& node) { if (!current_basic_block) end_current_block_and_start_new(); current_basic_block->add_statement(std::unique_ptr<Statement>(static_cast<Statement*>(node.clone().release()))); if(node.size_expr) node.size_expr->accept(*this); }
void CFGBuilderPass::visit(BrkStatement& node) { if (!current_basic_block) end_current_block_and_start_new(); current_basic_block->add_statement(std::unique_ptr<Statement>(static_cast<Statement*>(node.clone().release()))); }
void CFGBuilderPass::visit(FreeStatement& node) {}
void CFGBuilderPass::visit(ConditionalBranchStatement& node) {}
void CFGBuilderPass::visit(SysCall& node) {}
void CFGBuilderPass::visit(VecAllocationExpression& node) {}
// void CFGBuilderPass::visit(FVecAllocationExpression& node) {} // Commented out
void CFGBuilderPass::visit(StringAllocationExpression& node) {}
void CFGBuilderPass::visit(TableExpression& node) {}
// void CFGBuilderPass::visit(BitfieldAccessExpression& node) {} // Commented out
// void CFGBuilderPass::visit(ListExpression& node) {} // Commented out
void CFGBuilderPass::visit(LabelDeclaration& node) {}


void CFGBuilderPass::build_vector_foreach_cfg(ForEachStatement& node) {
    // ====================== START OF OPTIMIZATION ======================
    bool collection_is_empty = false;
    if (auto* vec_init = dynamic_cast<VecInitializerExpression*>(node.collection_expression.get())) {
        if (vec_init->is_empty) collection_is_empty = true;
    } else if (auto* table_expr = dynamic_cast<TableExpression*>(node.collection_expression.get())) {
        if (table_expr->is_empty) collection_is_empty = true;
    }

    if (collection_is_empty) {
        if (trace_enabled_) {
            std::cout << "[CFGBuilderPass] OPTIMIZATION: FOREACH on an empty VEC(), TABLE(), or FTABLE() literal. Eliminating entire loop." << std::endl;
        }
        // By returning here, we generate no blocks (header, body, exit) for the loop.
        return;
    }
    // ======================= END OF OPTIMIZATION =======================

    if (trace_enabled_) std::cout << "[CFGBuilderPass] Building CFG for vector-based FOREACH." << std::endl;

    // --- Step 1: Create unique names for loop control variables ---
    std::string collection_access_name;
    std::string var_name_for_lookup;
    bool use_existing_variable = false;
    
    // Check if the collection is a simple variable access
    if (auto* var_access = dynamic_cast<VariableAccess*>(node.collection_expression.get())) {
        // --- OPTIMIZATION PATH ---
        // Use the existing variable directly. No need for a temporary.
        collection_access_name = var_access->name;
        var_name_for_lookup = var_access->name;
        use_existing_variable = true;
        if (trace_enabled_) {
            std::cout << "[CFGBuilderPass] OPTIMIZATION: Reusing existing variable '" 
                      << collection_access_name << "' for FOREACH collection." << std::endl;
        }
    } else {
        // --- FALLBACK PATH (for complex expressions) ---
        // Create a temporary to hold the result of the collection expression.
        collection_access_name = "_forEach_vec_" + std::to_string(block_id_counter++);
        var_name_for_lookup = collection_access_name;
    }
    
    std::string len_name = "_forEach_len_" + std::to_string(block_id_counter++);
    std::string index_name = "_forEach_idx_" + std::to_string(block_id_counter++);

    // --- ENHANCED OPTIMIZATION: Check if we can use immediate length for constant-size collections ---
    bool use_immediate_length = false;
    size_t constant_length = 0;
    
    // Use enhanced is_constant_size_collection function that supports manifest constants
    int detected_size = 0;
    if (is_constant_size_collection(node.collection_expression.get(), detected_size)) {
        use_immediate_length = true;
        constant_length = static_cast<size_t>(detected_size);
        if (trace_enabled_) {
            std::cout << "[CFGBuilderPass] ENHANCED OPTIMIZATION: Constant-size collection detected with size: " 
                      << constant_length << " (supports manifest constants)" << std::endl;
        }
    }
    // Check if collection is a direct StringLiteral
    else if (auto string_lit = dynamic_cast<StringLiteral*>(node.collection_expression.get())) {
        use_immediate_length = true;
        constant_length = string_lit->value.length();
        if (trace_enabled_) {
            std::cout << "[CFGBuilderPass] OPTIMIZATION: Direct StringLiteral with size: " 
                      << constant_length << std::endl;
        }
    }
    // Check if collection is a variable that was assigned a VecInitializerExpression
    else if (auto var_access = dynamic_cast<VariableAccess*>(node.collection_expression.get())) {
        auto it = constant_vector_sizes_.find(var_access->name);
        if (it != constant_vector_sizes_.end()) {
            use_immediate_length = true;
            constant_length = it->second;
            if (trace_enabled_) {
                std::cout << "[CFGBuilderPass] OPTIMIZATION: Variable '" << var_access->name 
                          << "' has tracked constant size: " << constant_length << std::endl;
            }
        }
    }

    // --- Step 2: Register these new temporary variables with the analyzer ---
    // This ensures the code generator knows about them and allocates space.
    auto& analyzer = ASTAnalyzer::getInstance();
    if (current_cfg && !current_cfg->function_name.empty()) {
        auto metrics_it = analyzer.get_function_metrics_mut().find(current_cfg->function_name);
        if (metrics_it == analyzer.get_function_metrics_mut().end()) {
            std::cerr << "CFGBuilderPass Error: Function metrics not found for: " << current_cfg->function_name << std::endl;
            return;
        }
        auto& metrics = metrics_it->second;
        VarType collection_var_type = analyzer.infer_expression_type(node.collection_expression.get());
        
        // Only register collection variable if we're creating a new temporary
        if (!use_existing_variable) {
            metrics.variable_types[collection_access_name] = collection_var_type;
        }
        
        if (!use_immediate_length) {
            metrics.variable_types[len_name] = VarType::INTEGER;
        }
        metrics.variable_types[index_name] = VarType::INTEGER;
        
        // Update variable count: +1 for index, +1 for length (if not immediate), +1 for collection (if new temporary)
        int new_vars = 1; // Always need index
        if (!use_immediate_length) new_vars++; // Add length if not immediate
        if (!use_existing_variable) new_vars++; // Add collection if creating temporary
        metrics.num_variables += new_vars;

        // ====================== START OF FIX ======================
        // Also register these temporary variables in the main Symbol Table so that
        // the register allocator knows they exist.
        if (symbol_table_) {
            // Add the temporary that holds the collection pointer (only if not reusing existing)
            if (!use_existing_variable) {
                symbol_table_->addSymbol(Symbol(
                    collection_access_name,
                    SymbolKind::LOCAL_VAR,
                    collection_var_type,
                    symbol_table_->currentScopeLevel(),
                    current_cfg->function_name
                ));
            }

            // Add the temporary for the collection's length (only if not using immediate)
            if (!use_immediate_length) {
                symbol_table_->addSymbol(Symbol(
                    len_name,
                    SymbolKind::LOCAL_VAR,
                    VarType::INTEGER,
                    symbol_table_->currentScopeLevel(),
                    current_cfg->function_name
                ));
            }

            // Add the temporary for the loop index
            symbol_table_->addSymbol(Symbol(
                index_name,
                SymbolKind::LOCAL_VAR,
                VarType::INTEGER,
                symbol_table_->currentScopeLevel(),
                current_cfg->function_name
            ));
        }
        // ======================= END OF FIX =======================
    }

    // --- Step 3: Populate the Pre-Header block with initialization code ---
    // This code runs once before the loop begins.
    if (!current_basic_block) end_current_block_and_start_new();

    // LET _collection = <original collection expression> (only if not reusing existing variable)
    if (!use_existing_variable) {
        std::vector<ExprPtr> collection_rhs;
        collection_rhs.push_back(clone_unique_ptr(node.collection_expression));
        // Lower LetDeclaration to AssignmentStatement
        std::vector<ExprPtr> lhs_vec;
        lhs_vec.push_back(std::make_unique<VariableAccess>(collection_access_name));
        current_basic_block->add_statement(std::make_unique<AssignmentStatement>(
            std::move(lhs_vec),
            std::move(collection_rhs)
        ));
        if (trace_enabled_) {
            std::cout << "[CFGBuilderPass] Created collection temporary: " << collection_access_name << std::endl;
        }
    } else {
        if (trace_enabled_) {
            std::cout << "[CFGBuilderPass] OPTIMIZATION: Skipped collection temporary creation, reusing: " << collection_access_name << std::endl;
        }
    }

    // LET _len = LEN(_collection) (skip if using immediate length optimization)
    // LET _len = LEN(_collection) (only if not using immediate)
    if (!use_immediate_length) {
        std::vector<ExprPtr> len_rhs;
        len_rhs.push_back(std::make_unique<UnaryOp>(UnaryOp::Operator::LengthOf, std::make_unique<VariableAccess>(collection_access_name)));
        // Lower LetDeclaration to AssignmentStatement
        std::vector<ExprPtr> lhs_vec;
        lhs_vec.push_back(std::make_unique<VariableAccess>(len_name));
        current_basic_block->add_statement(std::make_unique<AssignmentStatement>(
            std::move(lhs_vec),
            std::move(len_rhs)
        ));
    }

    // LET _idx = 0
    {
        std::vector<ExprPtr> idx_rhs;
        idx_rhs.push_back(std::make_unique<NumberLiteral>(static_cast<int64_t>(0)));
        // Lower LetDeclaration to AssignmentStatement
        std::vector<ExprPtr> lhs_vec;
        lhs_vec.push_back(std::make_unique<VariableAccess>(index_name));
        current_basic_block->add_statement(std::make_unique<AssignmentStatement>(
            std::move(lhs_vec),
            std::move(idx_rhs)
        ));
    }

    // --- Step 4: Create the core basic blocks for the loop structure ---
    BasicBlock* header_block = create_new_basic_block("ForEachHeader_");
    BasicBlock* body_block = create_new_basic_block("ForEachBody_");
    BasicBlock* increment_block = create_new_basic_block("ForEachIncrement_");
    BasicBlock* exit_block = create_new_basic_block("ForEachExit_");

    // --- Step 5: Connect the blocks and populate them ---

    // The initialization block flows into the loop header
    current_cfg->add_edge(current_basic_block, header_block);

    // Populate the header block (the loop condition check)
    // Condition: IF _idx >= _len GOTO exit_block (use immediate if optimized)
    std::unique_ptr<BinaryOp> condition;
    if (use_immediate_length) {
        condition = std::make_unique<BinaryOp>(
            BinaryOp::Operator::Less,
            std::make_unique<VariableAccess>(index_name),
            std::make_unique<NumberLiteral>(static_cast<int64_t>(constant_length))
        );
    } else {
        condition = std::make_unique<BinaryOp>(
            BinaryOp::Operator::Less,
            std::make_unique<VariableAccess>(index_name),
            std::make_unique<VariableAccess>(len_name)
        );
    }
    auto conditional_branch = std::make_unique<ConditionalBranchStatement>("NE", body_block->id, std::move(condition));
    header_block->add_statement(std::move(conditional_branch));

    // The header branches to the exit (if condition false) or the body (if condition true)
    current_cfg->add_edge(header_block, exit_block);
    current_cfg->add_edge(header_block, body_block);

    // Populate the body block
    current_basic_block = body_block;

    // First statement in the body is to get the element: V := _collection ! _idx
    VarType collection_type = analyzer.infer_expression_type(node.collection_expression.get());
    ExprPtr access_expr;
    if (collection_type == VarType::POINTER_TO_STRING) {
        access_expr = std::make_unique<CharIndirection>(std::make_unique<VariableAccess>(collection_access_name), std::make_unique<VariableAccess>(index_name));
    } else if (collection_type == VarType::POINTER_TO_FLOAT_VEC) {
        access_expr = std::make_unique<FloatVectorIndirection>(std::make_unique<VariableAccess>(collection_access_name), std::make_unique<VariableAccess>(index_name));
    } else {
        access_expr = std::make_unique<VectorAccess>(std::make_unique<VariableAccess>(collection_access_name), std::make_unique<VariableAccess>(index_name));
    }
    std::vector<ExprPtr> lhs_vec;
    lhs_vec.push_back(std::make_unique<VariableAccess>(node.loop_variable_name));
    std::vector<ExprPtr> rhs_vec;
    rhs_vec.push_back(std::move(access_expr));
    current_basic_block->add_statement(std::make_unique<AssignmentStatement>(
        std::move(lhs_vec),
        std::move(rhs_vec)
    ));

    // Set up break/loop targets and then process the user's original loop body
    break_targets.push_back(exit_block);
    loop_targets.push_back(increment_block);
    if (node.body) {
        node.body->accept(*this);
    }

    // The user's code flows into the increment block (unless it had a BREAK, LOOP, etc.)
    if (current_basic_block && !current_basic_block->ends_with_control_flow()) {
        current_cfg->add_edge(current_basic_block, increment_block);
    }
    
    // Populate the increment block
    current_basic_block = increment_block;
    std::vector<ExprPtr> lhs_vec2;
    lhs_vec2.push_back(std::make_unique<VariableAccess>(index_name));
    std::vector<ExprPtr> rhs_vec2;
    rhs_vec2.push_back(std::make_unique<BinaryOp>(
        BinaryOp::Operator::Add,
        std::make_unique<VariableAccess>(index_name),
        std::make_unique<NumberLiteral>(static_cast<int64_t>(1))
    ));
    auto increment_stmt = std::make_unique<AssignmentStatement>(
        std::move(lhs_vec2),
        std::move(rhs_vec2)
    );
    increment_block->add_statement(std::move(increment_stmt));

    // The increment block unconditionally loops back to the header
    current_cfg->add_edge(increment_block, header_block);

    // --- Step 6: Finalize ---
    // Code generation will now continue from the exit block.
    current_basic_block = exit_block;
    // --- FIX APPLIED: Removed redundant stack pops. Stack management is now centralized in visit(ForEachStatement). ---

    if (trace_enabled_) std::cout << "[CFGBuilderPass] Correctly built low-level CFG for vector-based FOREACH." << std::endl;
}

void CFGBuilderPass::build_list_foreach_cfg(ForEachStatement& node) {
    // ====================== START OF OPTIMIZATION ======================
    if (auto* list_lit = dynamic_cast<ListExpression*>(node.collection_expression.get())) {
        if (list_lit->is_empty) {
            if (trace_enabled_) {
                std::cout << "[CFGBuilderPass] OPTIMIZATION: FOREACH on an empty LIST() literal. Eliminating entire loop." << std::endl;
            }
            // By returning here, we generate no blocks (header, body, exit) for the loop.
            return;
        }
    }
    // ======================= END OF OPTIMIZATION =======================

    // --- NEW: Check for destructuring FOREACH (X, Y) pattern ---
    if (node.is_destructuring) {
        if (trace_enabled_) {
            std::cout << "[CFGBuilderPass] Building destructuring FOREACH CFG for PAIR unpacking" << std::endl;
        }
        build_destructuring_list_foreach_cfg(node);
        return;
    }

    // --- Pre-header: Initialization ---
    std::string cursor_name = "_forEach_cursor_" + std::to_string(block_id_counter++);
    
    // ====================== START OF NEW OPTIMIZATION ======================
    std::string collection_ptr_name;
    std::string var_name_for_lookup; // A simple variable name, if we have one

    if (is_simple_variable_access(node.collection_expression.get(), var_name_for_lookup)) {
        // OPTIMIZATION PATH: The collection is already a simple variable.
        // Use it directly instead of creating a temporary copy.
        if (trace_enabled_) {
            std::cout << "[CFGBuilderPass] OPTIMIZATION: Using direct variable '" << var_name_for_lookup 
                      << "' for list FOREACH instead of creating a temporary header." << std::endl;
        }
        collection_ptr_name = var_name_for_lookup;
        // No need to create or assign to _forEach_header_*.

    } else {
        // FALLBACK PATH: The collection is a complex expression.
        // We must evaluate it and store the result in a temporary header variable.
        if (trace_enabled_) {
            std::cout << "[CFGBuilderPass] Collection is a complex expression; creating temporary header." << std::endl;
        }
        std::string header_temp_name = "_forEach_header_" + std::to_string(block_id_counter++);
        collection_ptr_name = header_temp_name;
        
        // Register the header temporary with the analyzer and symbol table...
        auto& analyzer = ASTAnalyzer::getInstance();
        auto metrics2_it = analyzer.get_function_metrics_mut().find(current_cfg->function_name);
        if (metrics2_it == analyzer.get_function_metrics_mut().end()) {
            std::cerr << "CFGBuilderPass Error: Function metrics not found for: " << current_cfg->function_name << std::endl;
            return;
        }
        auto& metrics2 = metrics2_it->second;
        metrics2.variable_types[header_temp_name] = VarType::POINTER_TO_ANY_LIST;
        metrics2.num_variables++;

        // ====================== START OF FIX ======================
        // Also register the header temporary variable in the main Symbol Table so that
        // the register allocator knows it exists.
        if (symbol_table_) {
            // Add the header temporary variable
            symbol_table_->addSymbol(Symbol(
                header_temp_name,
                SymbolKind::LOCAL_VAR,
                VarType::POINTER_TO_ANY_LIST,
                symbol_table_->currentScopeLevel(),
                current_cfg->function_name
            ));
        }
        // ======================= END OF FIX =======================
        
        // Emit the assignment: header_temp = <collection_expression>
        std::vector<ExprPtr> header_lhs;
        header_lhs.push_back(std::make_unique<VariableAccess>(header_temp_name));
        std::vector<ExprPtr> header_rhs;
        header_rhs.push_back(clone_unique_ptr(node.collection_expression));
        current_basic_block->add_statement(std::make_unique<AssignmentStatement>(
            std::move(header_lhs),
            std::move(header_rhs)
        ));
    }
    // ======================= END OF NEW OPTIMIZATION =======================

    // --- Register the cursor variable with ASTAnalyzer ---
    auto& analyzer = ASTAnalyzer::getInstance();
    auto metrics_it = analyzer.get_function_metrics_mut().find(current_cfg->function_name);
    if (metrics_it == analyzer.get_function_metrics_mut().end()) {
        std::cerr << "CFGBuilderPass Error: Function metrics not found for: " << current_cfg->function_name << std::endl;
        return;
    }
    auto& metrics = metrics_it->second;
    metrics.variable_types[cursor_name] = VarType::POINTER_TO_LIST_NODE;
    metrics.num_variables++;

    // ====================== START OF FIX ======================
    // Also register the cursor temporary variable in the main Symbol Table so that
    // the register allocator knows it exists.
    if (symbol_table_) {
        // Add the cursor temporary variable
        symbol_table_->addSymbol(Symbol(
            cursor_name,
            SymbolKind::LOCAL_VAR,
            VarType::POINTER_TO_LIST_NODE,
            symbol_table_->currentScopeLevel(),
            current_cfg->function_name
        ));
    }
    // ======================= END OF FIX =======================
    
    // Now, all subsequent logic uses 'collection_ptr_name', which is either the original
    // variable or the new temporary.
    
    // Initialize the cursor: cursor = *(collection_ptr_name + 16)
    std::vector<ExprPtr> cursor_rhs;
    cursor_rhs.push_back(
        std::make_unique<UnaryOp>(
            UnaryOp::Operator::Indirection,
            std::make_unique<BinaryOp>(
                BinaryOp::Operator::Add,
                std::make_unique<VariableAccess>(collection_ptr_name), // <-- USE THE GENERIC NAME
                std::make_unique<NumberLiteral>(static_cast<int64_t>(16))
            )
        )
    );
    std::vector<ExprPtr> lhs_vec;
    lhs_vec.push_back(std::make_unique<VariableAccess>(cursor_name));
    current_basic_block->add_statement(std::make_unique<AssignmentStatement>(
        std::move(lhs_vec),
        std::move(cursor_rhs)
    ));

    // --- Loop Header: Condition Check ---
    BasicBlock* header_block = create_new_basic_block("ForEachHeader_");
    BasicBlock* body_block = create_new_basic_block("ForEachBody_");
    BasicBlock* exit_block = create_new_basic_block("ForEachExit_");

    // Link the pre-header to the loop header
    current_cfg->add_edge(current_basic_block, header_block);

    // Conditional branch: IF _cursor = 0 GOTO exit_block
    auto condition_check = std::make_unique<ConditionalBranchStatement>(
        "EQ", // Branch if Equal to zero
        exit_block->id,
        std::make_unique<VariableAccess>(cursor_name)
    );
    header_block->add_statement(std::move(condition_check));

    // Add the control flow edges from the header
    current_cfg->add_edge(header_block, body_block);   // If condition fails (cursor is not 0), go to body.
    current_cfg->add_edge(header_block, exit_block);   // If condition succeeds (cursor is 0), go to exit.

    // --- Loop Body: Assign Value and Execute User Code ---
    current_basic_block = body_block;

    // At the top of the body, assign the type tag to the type variable if present
    if (!node.type_variable_name.empty()) {
        // LET T = TYPEOF(cursor)
        auto typeof_expr = std::make_unique<UnaryOp>(UnaryOp::Operator::TypeOf, std::make_unique<VariableAccess>(cursor_name));
        std::vector<ExprPtr> typeof_rhs;
        typeof_rhs.push_back(std::move(typeof_expr));
        // Lower LetDeclaration to AssignmentStatement
        std::vector<ExprPtr> lhs_vec;
        lhs_vec.push_back(std::make_unique<VariableAccess>(node.type_variable_name));
        current_basic_block->add_statement(std::make_unique<AssignmentStatement>(
            std::move(lhs_vec),
            std::move(typeof_rhs)
        ));
    }

    // --- STEP 1: Assign the value to the loop variable FIRST ---
    if (!node.type_variable_name.empty()) {
        // Two-variable FOREACH: X is the pointer to the atom, not its value.
        std::vector<ExprPtr> lhs;
        lhs.push_back(std::make_unique<VariableAccess>(node.loop_variable_name));
        std::vector<ExprPtr> rhs;
        rhs.push_back(std::make_unique<VariableAccess>(cursor_name));
        auto assign_value_stmt = std::make_unique<AssignmentStatement>(
            std::move(lhs),
            std::move(rhs)
        );
        current_basic_block->add_statement(std::move(assign_value_stmt));
    } else {
        // One-variable form: S = HD(_cursor)
        std::vector<ExprPtr> lhs;
        lhs.push_back(std::make_unique<VariableAccess>(node.loop_variable_name));
        std::vector<ExprPtr> rhs;
        rhs.push_back(std::make_unique<UnaryOp>(
            node.inferred_element_type == VarType::FLOAT ? UnaryOp::Operator::HeadOfAsFloat : UnaryOp::Operator::HeadOf,
            std::make_unique<VariableAccess>(cursor_name),
            node.inferred_element_type
        ));
        auto assign_value_stmt = std::make_unique<AssignmentStatement>(
            std::move(lhs),
            std::move(rhs)
        );
        current_basic_block->add_statement(std::move(assign_value_stmt));


    }

    // --- STEP 2: Process the user's original loop body ---
    if (node.body) {
        node.body->accept(*this);
    }

    // --- STEP 3: Create the advance block and link to it ---
    BasicBlock* advance_block = create_new_basic_block("ForEachAdvance_");
    if (current_basic_block && !current_basic_block->ends_with_control_flow()) {
        current_cfg->add_edge(current_basic_block, advance_block);
    }

    // --- STEP 4: In the new block, advance the cursor and loop back ---
    current_basic_block = advance_block;
    std::vector<ExprPtr> lhs2;
    lhs2.push_back(std::make_unique<VariableAccess>(cursor_name));
    std::vector<ExprPtr> rhs2;
    rhs2.push_back(std::make_unique<UnaryOp>(
        UnaryOp::Operator::TailOfNonDestructive,
        std::make_unique<VariableAccess>(cursor_name)
    ));
    auto advance_cursor_stmt = std::make_unique<AssignmentStatement>(
        std::move(lhs2),
        std::move(rhs2)
    );
    current_basic_block->add_statement(std::move(advance_cursor_stmt));
    current_cfg->add_edge(current_basic_block, header_block);

    // --- Step 7: Loop Exit and Cleanup ---
    // Set the current block for code generation to the exit block
    current_basic_block = exit_block;

    // Push the exit block onto the break/loop stacks so BREAK/LOOP statements work
    break_targets.push_back(exit_block);
    loop_targets.push_back(header_block);

    // OPTIMIZATION: Mark temporaries as available for reuse after loop completion
    // This will be done in the ForEachStatement visitor after the loop context is parsed
    
    if (trace_enabled_) std::cout << "[CFGBuilderPass] Correctly built low-level CFG for list-based FOREACH." << std::endl;
}



void CFGBuilderPass::build_destructuring_list_foreach_cfg(ForEachStatement& node) {
    if (trace_enabled_) {
        std::cout << "[CFGBuilderPass] Building destructuring list FOREACH CFG for variables: " 
                  << node.loop_variable_name << ", " << node.type_variable_name << std::endl;
    }

    // Generate unique names for temporaries
    std::string cursor_name = "_forEach_cursor_" + std::to_string(block_id_counter++);
    std::string packed_val_name = "_forEach_packed_" + std::to_string(block_id_counter++);
    std::string header_temp_name = "_forEach_header_" + std::to_string(block_id_counter++);

    // Register temporaries with analyzer and symbol table
    auto& analyzer = ASTAnalyzer::getInstance();
    auto metrics_it = analyzer.get_function_metrics_mut().find(current_cfg->function_name);
    if (metrics_it == analyzer.get_function_metrics_mut().end()) {
        std::cerr << "CFGBuilderPass Error: Function metrics not found for: " << current_cfg->function_name << std::endl;
        return;
    }
    auto& metrics = metrics_it->second;
    
    // Register temporaries
    metrics.variable_types[cursor_name] = VarType::POINTER_TO_LIST_NODE;
    metrics.variable_types[packed_val_name] = VarType::INTEGER; // 64-bit packed value
    metrics.variable_types[header_temp_name] = VarType::POINTER_TO_ANY_LIST;
    metrics.num_variables += 3;

    if (symbol_table_) {
        symbol_table_->addSymbol(Symbol(cursor_name, SymbolKind::LOCAL_VAR, VarType::POINTER_TO_LIST_NODE, 
                                       symbol_table_->currentScopeLevel(), current_cfg->function_name));
        symbol_table_->addSymbol(Symbol(packed_val_name, SymbolKind::LOCAL_VAR, VarType::INTEGER,
                                       symbol_table_->currentScopeLevel(), current_cfg->function_name));
        symbol_table_->addSymbol(Symbol(header_temp_name, SymbolKind::LOCAL_VAR, VarType::POINTER_TO_ANY_LIST,
                                       symbol_table_->currentScopeLevel(), current_cfg->function_name));
    }

    // Create basic blocks using CFG factory method
    BasicBlock* header_block = current_cfg->create_block("foreach_destructuring_header_");
    BasicBlock* body_block = current_cfg->create_block("foreach_destructuring_body_");
    BasicBlock* exit_block = current_cfg->create_block("foreach_destructuring_exit_");

    // Add loop targets for break/continue
    break_targets.push_back(exit_block);
    loop_targets.push_back(header_block);

    // PRE-HEADER: Evaluate collection and initialize cursor
    // Store collection in temporary
    std::vector<ExprPtr> rhs_exprs;
    auto cloned_expr = node.collection_expression->clone();
    rhs_exprs.push_back(std::unique_ptr<Expression>(static_cast<Expression*>(cloned_expr.release())));
    std::vector<ExprPtr> lhs_exprs;
    lhs_exprs.push_back(std::make_unique<VariableAccess>(header_temp_name));
    current_basic_block->add_statement(std::make_unique<AssignmentStatement>(std::move(lhs_exprs), std::move(rhs_exprs)));

    // Initialize cursor to head of list: cursor := *(header + 16)
    std::vector<ExprPtr> cursor_lhs;
    cursor_lhs.push_back(std::make_unique<VariableAccess>(cursor_name));
    std::vector<ExprPtr> cursor_rhs;
    auto header_plus_16 = std::make_unique<BinaryOp>(
        BinaryOp::Operator::Add,
        std::make_unique<VariableAccess>(header_temp_name),
        std::make_unique<NumberLiteral>(static_cast<int64_t>(16))
    );
    cursor_rhs.push_back(std::make_unique<UnaryOp>(UnaryOp::Operator::Indirection, std::move(header_plus_16)));
    current_basic_block->add_statement(std::make_unique<AssignmentStatement>(std::move(cursor_lhs), std::move(cursor_rhs)));

    // Jump to header
    current_basic_block->add_statement(std::make_unique<GotoStatement>(
        std::make_unique<VariableAccess>(header_block->id)));
    current_basic_block = header_block;

    // HEADER: Check if cursor is null (end of list)
    auto cursor_null_check = std::make_unique<BinaryOp>(
        BinaryOp::Operator::Equal,
        std::make_unique<VariableAccess>(cursor_name),
        std::make_unique<NullLiteral>()
    );
    header_block->add_statement(std::make_unique<ConditionalBranchStatement>(
        "cursor_null", exit_block->id, std::move(cursor_null_check)));

    // BODY: Extract packed value and destructure
    current_basic_block = body_block;

    // Load packed value: packed_val := *(cursor + 8)
    std::vector<ExprPtr> packed_lhs;
    packed_lhs.push_back(std::make_unique<VariableAccess>(packed_val_name));
    std::vector<ExprPtr> packed_rhs;
    auto cursor_plus_8 = std::make_unique<BinaryOp>(
        BinaryOp::Operator::Add,
        std::make_unique<VariableAccess>(cursor_name),
        std::make_unique<NumberLiteral>(static_cast<int64_t>(8))
    );
    packed_rhs.push_back(std::make_unique<UnaryOp>(UnaryOp::Operator::Indirection, std::move(cursor_plus_8)));
    body_block->add_statement(std::make_unique<AssignmentStatement>(std::move(packed_lhs), std::move(packed_rhs)));

    // Destructure: X := UBFX(packed_val, 0, 32) and Y := UBFX(packed_val, 32, 32)
    // Create X assignment
    std::vector<ExprPtr> x_lhs, y_lhs;
    x_lhs.push_back(std::make_unique<VariableAccess>(node.loop_variable_name));
    std::vector<ExprPtr> x_rhs;
    x_rhs.push_back(std::make_unique<BitfieldAccessExpression>(
        std::make_unique<VariableAccess>(packed_val_name),
        std::make_unique<NumberLiteral>(static_cast<int64_t>(0)),   // start bit
        std::make_unique<NumberLiteral>(static_cast<int64_t>(32))   // width
    ));
    body_block->add_statement(std::make_unique<AssignmentStatement>(std::move(x_lhs), std::move(x_rhs)));

    // Create Y assignment  
    y_lhs.push_back(std::make_unique<VariableAccess>(node.type_variable_name));
    std::vector<ExprPtr> y_rhs;
    y_rhs.push_back(std::make_unique<BitfieldAccessExpression>(
        std::make_unique<VariableAccess>(packed_val_name),
        std::make_unique<NumberLiteral>(static_cast<int64_t>(32)),  // start bit
        std::make_unique<NumberLiteral>(static_cast<int64_t>(32))   // width
    ));
    body_block->add_statement(std::make_unique<AssignmentStatement>(std::move(y_lhs), std::move(y_rhs)));

    // Process user's loop body
    if (node.body) {
        node.body->accept(*this);
    }

    // Advance cursor: cursor := *(cursor + 16)
    std::vector<ExprPtr> advance_lhs;
    advance_lhs.push_back(std::make_unique<VariableAccess>(cursor_name));
    std::vector<ExprPtr> advance_rhs;
    auto cursor_plus_16 = std::make_unique<BinaryOp>(
        BinaryOp::Operator::Add,
        std::make_unique<VariableAccess>(cursor_name),
        std::make_unique<NumberLiteral>(static_cast<int64_t>(16))
    );
    advance_rhs.push_back(std::make_unique<UnaryOp>(UnaryOp::Operator::Indirection, std::move(cursor_plus_16)));
    current_basic_block->add_statement(std::make_unique<AssignmentStatement>(std::move(advance_lhs), std::move(advance_rhs)));

    // Jump back to header
    current_basic_block->add_statement(std::make_unique<GotoStatement>(
        std::make_unique<VariableAccess>(header_block->id)));

    // Continue from exit block
    current_basic_block = exit_block;

    if (trace_enabled_) {
            std::cout << "[CFGBuilderPass] Completed destructuring list FOREACH CFG" << std::endl;
        }
    }

    // === Reduction Statement CFG Builders ===

    void CFGBuilderPass::visit(MinStatement& node) {
        std::cout << "[DEBUG CFG] *** VISITING MinStatement! ***" << std::endl;
        if (trace_enabled_) std::cout << "[CFGBuilderPass] visit(MinStatement) entered." << std::endl;
    
        if (!current_basic_block) end_current_block_and_start_new();
    
        // Generate structured loop for MIN reduction using CFG infrastructure
        std::cout << "[DEBUG CFG] About to call generateReductionCFG for MIN operation" << std::endl;
        generateReductionCFG(node.left_operand.get(), node.right_operand.get(), 
                             node.result_variable, 0); // MIN = 0
    
        if (trace_enabled_) std::cout << "[CFGBuilderPass] visit(MinStatement) exiting." << std::endl;
        std::cout << "[DEBUG CFG] *** MinStatement visit complete! ***" << std::endl;
    }

    void CFGBuilderPass::visit(MaxStatement& node) {
        std::cout << "[DEBUG CFG] *** VISITING MaxStatement! ***" << std::endl;
        if (trace_enabled_) std::cout << "[CFGBuilderPass] visit(MaxStatement) entered." << std::endl;
    
        if (!current_basic_block) end_current_block_and_start_new();
    
        // Generate structured loop for MAX reduction using CFG infrastructure
        std::cout << "[DEBUG CFG] About to call generateReductionCFG for MAX operation" << std::endl;
        generateReductionCFG(node.left_operand.get(), node.right_operand.get(), 
                             node.result_variable, 1); // MAX = 1
    
        if (trace_enabled_) std::cout << "[CFGBuilderPass] visit(MaxStatement) exiting." << std::endl;
        std::cout << "[DEBUG CFG] *** MaxStatement visit complete! ***" << std::endl;
    }

    void CFGBuilderPass::visit(SumStatement& node) {
        std::cout << "[DEBUG CFG] *** VISITING SumStatement! ***" << std::endl;
        if (trace_enabled_) std::cout << "[CFGBuilderPass] visit(SumStatement) entered." << std::endl;
    
        if (!current_basic_block) end_current_block_and_start_new();
    
        // Generate structured loop for SUM reduction using CFG infrastructure  
        std::cout << "[DEBUG CFG] About to call generateReductionCFG for SUM operation" << std::endl;
        generateReductionCFG(node.left_operand.get(), node.right_operand.get(), 
                             node.result_variable, 2); // SUM = 2
    
        if (trace_enabled_) std::cout << "[CFGBuilderPass] visit(SumStatement) exiting." << std::endl;
        std::cout << "[DEBUG CFG] *** SumStatement visit complete! ***" << std::endl;
    }

    void CFGBuilderPass::generateReductionCFG(Expression* left_expr, Expression* right_expr,
                                             const std::string& result_var, int op) {
        if (trace_enabled_) std::cout << "[CFGBuilderPass] Building CFG for reduction operation." << std::endl;
        
        // --- Step 1: Create unique names for reduction control variables ---
        std::string left_vec_name;
        std::string right_vec_name;
        std::string left_size_name = "_reduction_left_size_" + std::to_string(block_id_counter++);
        std::string right_size_name = "_reduction_right_size_" + std::to_string(block_id_counter++);
        std::string chunks_name = "_reduction_chunks_" + std::to_string(block_id_counter++);
        std::string index_name = "_reduction_idx_" + std::to_string(block_id_counter++);
        
        // Check if operands are simple variable accesses (optimization)
        bool left_is_variable = false;
        bool right_is_variable = false;
        
        if (auto* left_var = dynamic_cast<VariableAccess*>(left_expr)) {
            left_vec_name = left_var->name;
            left_is_variable = true;
        } else {
            left_vec_name = "_reduction_left_" + std::to_string(block_id_counter++);
        }
        
        if (auto* right_var = dynamic_cast<VariableAccess*>(right_expr)) {
            right_vec_name = right_var->name;
            right_is_variable = true;
        } else {
            right_vec_name = "_reduction_right_" + std::to_string(block_id_counter++);
        }
        
        // --- Step 2: Register temporary variables in symbol table ---
        if (symbol_table_) {
            if (!left_is_variable) {
                symbol_table_->addSymbol(Symbol(
                    left_vec_name, SymbolKind::LOCAL_VAR, VarType::PAIRS,
                    symbol_table_->currentScopeLevel(), current_cfg->function_name
                ));
            }
            if (!right_is_variable) {
                symbol_table_->addSymbol(Symbol(
                    right_vec_name, SymbolKind::LOCAL_VAR, VarType::PAIRS,
                    symbol_table_->currentScopeLevel(), current_cfg->function_name
                ));
            }
            symbol_table_->addSymbol(Symbol(
                left_size_name, SymbolKind::LOCAL_VAR, VarType::INTEGER,
                symbol_table_->currentScopeLevel(), current_cfg->function_name
            ));
            symbol_table_->addSymbol(Symbol(
                right_size_name, SymbolKind::LOCAL_VAR, VarType::INTEGER,
                symbol_table_->currentScopeLevel(), current_cfg->function_name
            ));
            symbol_table_->addSymbol(Symbol(
                chunks_name, SymbolKind::LOCAL_VAR, VarType::INTEGER,
                symbol_table_->currentScopeLevel(), current_cfg->function_name
            ));
            symbol_table_->addSymbol(Symbol(
                index_name, SymbolKind::LOCAL_VAR, VarType::INTEGER,
                symbol_table_->currentScopeLevel(), current_cfg->function_name
            ));
            symbol_table_->addSymbol(Symbol(
                result_var, SymbolKind::LOCAL_VAR, VarType::PAIRS,
                symbol_table_->currentScopeLevel(), current_cfg->function_name
            ));
        }
        
        // --- Step 3: Populate initialization in current block ---
        if (!current_basic_block) end_current_block_and_start_new();
        
        // Store operand expressions if not variables
        if (!left_is_variable) {
            std::vector<ExprPtr> left_rhs;
            left_rhs.push_back(std::unique_ptr<Expression>(static_cast<Expression*>(left_expr->clone().release())));
            std::vector<ExprPtr> left_lhs;
            left_lhs.push_back(std::make_unique<VariableAccess>(left_vec_name));
            current_basic_block->add_statement(std::make_unique<AssignmentStatement>(
                std::move(left_lhs), std::move(left_rhs)
            ));
        }
        
        if (!right_is_variable) {
            std::vector<ExprPtr> right_rhs;
            right_rhs.push_back(std::unique_ptr<Expression>(static_cast<Expression*>(right_expr->clone().release())));
            std::vector<ExprPtr> right_lhs;
            right_lhs.push_back(std::make_unique<VariableAccess>(right_vec_name));
            current_basic_block->add_statement(std::make_unique<AssignmentStatement>(
                std::move(right_lhs), std::move(right_rhs)
            ));
        }
        
        // Get vector sizes: LET left_size = LEN(left_vec)
        {
            std::vector<ExprPtr> size_rhs;
            size_rhs.push_back(std::make_unique<UnaryOp>(UnaryOp::Operator::LengthOf, std::make_unique<VariableAccess>(left_vec_name)));
            std::vector<ExprPtr> size_lhs;
            size_lhs.push_back(std::make_unique<VariableAccess>(left_size_name));
            current_basic_block->add_statement(std::make_unique<AssignmentStatement>(
                std::move(size_lhs), std::move(size_rhs)
            ));
        }
        
        // Calculate chunks: LET chunks = (left_size + 1) / 2 for PAIRS
        {
            std::vector<ExprPtr> chunks_rhs;
            auto size_plus_one = std::make_unique<BinaryOp>(
                BinaryOp::Operator::Add,
                std::make_unique<VariableAccess>(left_size_name),
                std::make_unique<NumberLiteral>(static_cast<int64_t>(1))
            );
            chunks_rhs.push_back(std::make_unique<BinaryOp>(
                BinaryOp::Operator::Divide,
                std::move(size_plus_one),
                std::make_unique<NumberLiteral>(static_cast<int64_t>(2))
            ));
            std::vector<ExprPtr> chunks_lhs;
            chunks_lhs.push_back(std::make_unique<VariableAccess>(chunks_name));
            current_basic_block->add_statement(std::make_unique<AssignmentStatement>(
                std::move(chunks_lhs), std::move(chunks_rhs)
            ));
        }
        
        // Allocate result vector: LET result = GETVEC(left_size * 2)
        {
            std::vector<ExprPtr> result_rhs;
            auto size_times_two = std::make_unique<BinaryOp>(
                BinaryOp::Operator::Multiply,
                std::make_unique<VariableAccess>(left_size_name),
                std::make_unique<NumberLiteral>(static_cast<int64_t>(2))
            );
            std::vector<ExprPtr> getvec_args;
            getvec_args.push_back(std::move(size_times_two));
            result_rhs.push_back(std::make_unique<FunctionCall>(
                std::make_unique<VariableAccess>("GETVEC"),
                std::move(getvec_args)
            ));
            std::vector<ExprPtr> result_lhs;
            result_lhs.push_back(std::make_unique<VariableAccess>(result_var));
            current_basic_block->add_statement(std::make_unique<AssignmentStatement>(
                std::move(result_lhs), std::move(result_rhs)
            ));
        }
        
        // Initialize index: LET idx = 0
        {
            std::vector<ExprPtr> idx_rhs;
            idx_rhs.push_back(std::make_unique<NumberLiteral>(static_cast<int64_t>(0)));
            std::vector<ExprPtr> idx_lhs;
            idx_lhs.push_back(std::make_unique<VariableAccess>(index_name));
            current_basic_block->add_statement(std::make_unique<AssignmentStatement>(
                std::move(idx_lhs), std::move(idx_rhs)
            ));
        }
        
        // --- Step 4: Create the loop basic blocks ---
        BasicBlock* header_block = create_new_basic_block("ReductionHeader_");
        BasicBlock* body_block = create_new_basic_block("ReductionBody_");  
        BasicBlock* increment_block = create_new_basic_block("ReductionIncrement_");
        BasicBlock* exit_block = create_new_basic_block("ReductionExit_");
        
        // --- Step 5: Connect pre-header to header ---
        current_basic_block->add_successor(header_block);
        header_block->add_predecessor(current_basic_block);
        
        // --- Step 6: Populate header block (loop condition) ---
        current_basic_block = header_block;
        
        // Create ReductionLoopStatement for the body with metadata
        auto reduction_loop_stmt = std::make_unique<ReductionLoopStatement>(
            left_vec_name, right_vec_name, result_var,
            index_name, chunks_name, result_var, op
        );
        
        // Add conditional branch: if (idx < chunks) goto body else goto exit
        auto condition = std::make_unique<BinaryOp>(
            BinaryOp::Operator::Less,
            std::make_unique<VariableAccess>(index_name),
            std::make_unique<VariableAccess>(chunks_name)
        );
        auto if_stmt = std::make_unique<IfStatement>(
            std::move(condition),
            std::make_unique<CompoundStatement>(std::vector<StmtPtr>{}) // Placeholder - flow handled by CFG
        );
        current_basic_block->add_statement(std::move(if_stmt));
        
        // Connect header to body and exit
        current_basic_block->add_successor(body_block);
        current_basic_block->add_successor(exit_block);
        body_block->add_predecessor(current_basic_block);
        exit_block->add_predecessor(current_basic_block);
        
        // --- Step 7: Populate body block (NEON reduction) ---
        current_basic_block = body_block;
        std::cout << "[DEBUG CFG] Adding simple debug statements to body block: " << body_block->id << std::endl;
        
        // Create simple debug statements directly in the body block
        // This proves the loop body is executing
        
        // Create a simple assignment: index := index + 0 (NOP but visible in assembly)
        std::vector<ExprPtr> debug_lhs;
        debug_lhs.push_back(std::make_unique<VariableAccess>(index_name));
        std::vector<ExprPtr> debug_rhs;
        debug_rhs.push_back(std::make_unique<BinaryOp>(
            BinaryOp::Operator::Add,
            std::make_unique<VariableAccess>(index_name),
            std::make_unique<NumberLiteral>(static_cast<int64_t>(0))
        ));
        current_basic_block->add_statement(std::make_unique<AssignmentStatement>(
            std::move(debug_lhs), std::move(debug_rhs)
        ));
        
        std::cout << "[DEBUG CFG] Body block now has " << current_basic_block->statements.size() << " statements" << std::endl;
        
        // Connect body to increment
        current_basic_block->add_successor(increment_block);
        increment_block->add_predecessor(current_basic_block);
        
        // --- Step 8: Populate increment block ---
        current_basic_block = increment_block;
        
        // idx = idx + 1
        {
            std::vector<ExprPtr> inc_rhs;
            inc_rhs.push_back(std::make_unique<BinaryOp>(
                BinaryOp::Operator::Add,
                std::make_unique<VariableAccess>(index_name),
                std::make_unique<NumberLiteral>(static_cast<int64_t>(1))
            ));
            std::vector<ExprPtr> inc_lhs;
            inc_lhs.push_back(std::make_unique<VariableAccess>(index_name));
            current_basic_block->add_statement(std::make_unique<AssignmentStatement>(
                std::move(inc_lhs), std::move(inc_rhs)
            ));
        }
        
        // Connect increment back to header
        current_basic_block->add_successor(header_block);
        header_block->add_predecessor(current_basic_block);
        
        // --- Step 9: Set current block to exit for continuation ---
        current_basic_block = exit_block;
        
        if (trace_enabled_) {
            std::cout << "[CFGBuilderPass] Generated CFG blocks for reduction:" << std::endl;
            std::cout << "  Header: " << header_block->id << std::endl;
            std::cout << "  Body: " << body_block->id << std::endl;
            std::cout << "  Increment: " << increment_block->id << std::endl;
            std::cout << "  Exit: " << exit_block->id << std::endl;
        }
        
        if (trace_enabled_) std::cout << "[CFGBuilderPass] Completed reduction CFG generation." << std::endl;
    }

    // === Helper method to check if expression is PAIRS type ===
    
    bool CFGBuilderPass::isPairsType(Expression* expr) {
        if (!expr || !symbol_table_) {
            return false;
        }
        
        // Check if it's a variable access and look up its type
        if (auto* var_access = dynamic_cast<VariableAccess*>(expr)) {
            Symbol symbol;
            if (symbol_table_->lookup(var_access->name, symbol)) {
                return symbol.type == VarType::PAIRS;
            }
        }
        
        // Check if it's a PAIRS allocation expression
        if (dynamic_cast<PairsAllocationExpression*>(expr)) {
            return true;
        }
        
        return false;
    }

    // === New Overloaded generateReductionCFG for Reducer Interface ===
    
    void CFGBuilderPass::generateReductionCFG(Expression* left_expr, Expression* right_expr,
                                             const std::string& result_var, const Reducer& reducer) {
        if (trace_enabled_) {
            std::cout << "[CFGBuilderPass] Building CFG for " << reducer.getName() 
                      << " reduction operation (modern interface)" << std::endl;
        }
        
        // Check if we're dealing with PAIRS type for component-wise reduction
        if (isPairsType(left_expr)) {
            if (trace_enabled_) {
                std::cout << "[CFGBuilderPass] Detected PAIRS type - generating component-wise reduction" << std::endl;
            }
            generateComponentWiseReductionCFG(left_expr, right_expr, result_var, reducer);
        } else {
            // Delegate to existing implementation using the reducer's operation code
            generateReductionCFG(left_expr, right_expr, result_var, reducer.getReductionOp());
        }
        
        if (trace_enabled_) {
            std::cout << "[CFGBuilderPass] Completed " << reducer.getName() 
                      << " reduction using " << (isPairsType(left_expr) ? "component-wise" : "legacy") 
                      << " implementation" << std::endl;
        }
    }

    // === New Modular ReductionStatement Visitor ===
    
    void CFGBuilderPass::visit(ReductionStatement& node) {
        if (trace_enabled_) {
            std::cout << "[CFGBuilderPass] visit(ReductionStatement) entered for operation: " 
                      << node.reducer->getName() << std::endl;
        }
        
        if (!current_basic_block) end_current_block_and_start_new();

        // Check if we have a pairwise reducer and handle it specially
        if (auto* pairwise_min = dynamic_cast<PairwiseMinReducer*>(node.reducer.get())) {
            generatePairwiseReductionCFG(node.left_operand.get(), node.right_operand.get(),
                                        node.result_variable, *pairwise_min);
        } else if (auto* pairwise_max = dynamic_cast<PairwiseMaxReducer*>(node.reducer.get())) {
            generatePairwiseReductionCFG(node.left_operand.get(), node.right_operand.get(),
                                        node.result_variable, *pairwise_max);
        } else if (auto* pairwise_add = dynamic_cast<PairwiseAddReducer*>(node.reducer.get())) {
            generatePairwiseReductionCFG(node.left_operand.get(), node.right_operand.get(),
                                        node.result_variable, *pairwise_add);
        } else {
            // Handle initialization if the reducer requires it
            if (node.reducer->requiresInitialization()) {
                if (trace_enabled_) {
                    std::cout << "[CFGBuilderPass] Initializing result variable '" << node.result_variable 
                              << "' with initial value: " << node.reducer->getInitialValueString() << std::endl;
                }
                
                std::vector<ExprPtr> result_lhs;
                result_lhs.push_back(std::make_unique<VariableAccess>(node.result_variable));
                std::vector<ExprPtr> result_rhs;
                result_rhs.push_back(node.reducer->getInitialValue());
                current_basic_block->add_statement(std::make_unique<AssignmentStatement>(
                    std::move(result_lhs), std::move(result_rhs)
                ));
            }

            // Generate the reduction CFG using the modern reducer interface
            generateReductionCFG(node.left_operand.get(), node.right_operand.get(),
                                 node.result_variable, *node.reducer);
        }

        if (trace_enabled_) {
            std::cout << "[CFGBuilderPass] visit(ReductionStatement) exiting for operation: " 
                      << node.reducer->getName() << std::endl;
        }
    }

    // === Pairwise Reduction CFG Generation Methods ===
    
    void CFGBuilderPass::generatePairwiseReductionCFG(Expression* left_expr, Expression* right_expr,
                                                     const std::string& result_var, const PairwiseMinReducer& reducer) {
        if (trace_enabled_) {
            std::cout << "[CFGBuilderPass] Building pairwise CFG for " << reducer.getName() 
                      << " operation using NEON intrinsics" << std::endl;
        }

        if (!current_basic_block) end_current_block_and_start_new();

        // Extract vector names from expressions
        std::string left_vector_name, right_vector_name;
        if (auto* left_var = dynamic_cast<VariableAccess*>(left_expr)) {
            left_vector_name = left_var->name;
        } else {
            left_vector_name = "_pairwise_left_" + std::to_string(block_id_counter++);
            // Store the left expression in a temporary variable
            std::vector<ExprPtr> left_lhs;
            left_lhs.push_back(std::make_unique<VariableAccess>(left_vector_name));
            std::vector<ExprPtr> left_rhs;
            left_rhs.push_back(std::unique_ptr<Expression>(static_cast<Expression*>(left_expr->clone().release())));
            current_basic_block->add_statement(std::make_unique<AssignmentStatement>(
                std::move(left_lhs), std::move(left_rhs)
            ));
        }

        if (right_expr) {
            if (auto* right_var = dynamic_cast<VariableAccess*>(right_expr)) {
                right_vector_name = right_var->name;
            } else {
                right_vector_name = "_pairwise_right_" + std::to_string(block_id_counter++);
                // Store the right expression in a temporary variable
                std::vector<ExprPtr> right_lhs;
                right_lhs.push_back(std::make_unique<VariableAccess>(right_vector_name));
                std::vector<ExprPtr> right_rhs;
                right_rhs.push_back(std::unique_ptr<Expression>(static_cast<Expression*>(right_expr->clone().release())));
                current_basic_block->add_statement(std::make_unique<AssignmentStatement>(
                    std::move(right_lhs), std::move(right_rhs)
                ));
            }
        } else {
            // For single-argument pairwise operations, use the left vector as both operands
            right_vector_name = left_vector_name;
        }

        // Generate result vector allocation with proper size calculation
        // For pairwise operations: result_size = input_size / 2
        
        // Create a UnaryOp to get input vector size: LengthOf(input_vector)
        auto len_op = std::make_unique<UnaryOp>(
            UnaryOp::Operator::LengthOf, 
            std::make_unique<VariableAccess>(left_vector_name));
        
        // Create binary division: LengthOf(input_vector) / 2
        auto two_literal = std::make_unique<NumberLiteral>(static_cast<int64_t>(2));
        auto size_div = std::make_unique<BinaryOp>(
            BinaryOp::Operator::Divide,
            std::move(len_op),
            std::move(two_literal)
        );
        
        // Create GETVEC call: GETVEC(input_size / 2)
        std::vector<ExprPtr> getvec_args;
        getvec_args.push_back(std::move(size_div));
        auto getvec_call = std::make_unique<FunctionCall>(
            std::make_unique<VariableAccess>("GETVEC"), std::move(getvec_args));
        
        // Create assignment: result_var = GETVEC(input_size / 2)  
        std::vector<ExprPtr> result_lhs;
        result_lhs.push_back(std::make_unique<VariableAccess>(result_var));
        std::vector<ExprPtr> result_rhs;
        result_rhs.push_back(std::move(getvec_call));
        
        current_basic_block->add_statement(std::make_unique<AssignmentStatement>(
            std::move(result_lhs), std::move(result_rhs)
        ));

        // Create the pairwise reduction loop statement with NEON intrinsic
        auto pairwise_stmt = std::make_unique<PairwiseReductionLoopStatement>(
            left_vector_name, right_vector_name, result_var,
            "llvm.arm.neon.vpmin.v4f32",  // NEON intrinsic for pairwise minimum
            reducer.getReductionOp()
        );

        current_basic_block->add_statement(std::move(pairwise_stmt));

        if (trace_enabled_) {
            std::cout << "[CFGBuilderPass] Completed pairwise " << reducer.getName() 
                      << " CFG generation with NEON intrinsics" << std::endl;
        }
    }

    void CFGBuilderPass::generatePairwiseReductionCFG(Expression* left_expr, Expression* right_expr,
                                                     const std::string& result_var, const PairwiseMaxReducer& reducer) {
        if (trace_enabled_) {
            std::cout << "[CFGBuilderPass] Building pairwise CFG for " << reducer.getName() 
                      << " operation using NEON intrinsics" << std::endl;
        }

        if (!current_basic_block) end_current_block_and_start_new();

        // Extract vector names from expressions (similar to PairwiseMinReducer)
        std::string left_vector_name, right_vector_name;
        if (auto* left_var = dynamic_cast<VariableAccess*>(left_expr)) {
            left_vector_name = left_var->name;
        } else {
            left_vector_name = "_pairwise_left_" + std::to_string(block_id_counter++);
            std::vector<ExprPtr> left_lhs;
            left_lhs.push_back(std::make_unique<VariableAccess>(left_vector_name));
            std::vector<ExprPtr> left_rhs;
            left_rhs.push_back(std::unique_ptr<Expression>(static_cast<Expression*>(left_expr->clone().release())));
            current_basic_block->add_statement(std::make_unique<AssignmentStatement>(
                std::move(left_lhs), std::move(left_rhs)
            ));
        }

        if (right_expr) {
            if (auto* right_var = dynamic_cast<VariableAccess*>(right_expr)) {
                right_vector_name = right_var->name;
            } else {
                right_vector_name = "_pairwise_right_" + std::to_string(block_id_counter++);
                std::vector<ExprPtr> right_lhs;
                right_lhs.push_back(std::make_unique<VariableAccess>(right_vector_name));
                std::vector<ExprPtr> right_rhs;
                right_rhs.push_back(std::unique_ptr<Expression>(static_cast<Expression*>(right_expr->clone().release())));
                current_basic_block->add_statement(std::make_unique<AssignmentStatement>(
                    std::move(right_lhs), std::move(right_rhs)
                ));
            }
        } else {
            // For single-argument pairwise operations, use the left vector as both operands
            right_vector_name = left_vector_name;
        }

        // Generate result vector allocation with proper size calculation
        // For pairwise operations: result_size = input_size / 2
        
        // Create a UnaryOp to get input vector size: LengthOf(input_vector)
        auto len_op = std::make_unique<UnaryOp>(
            UnaryOp::Operator::LengthOf, 
            std::make_unique<VariableAccess>(left_vector_name));
        
        // Create binary division: LengthOf(input_vector) / 2
        auto two_literal = std::make_unique<NumberLiteral>(static_cast<int64_t>(2));
        auto size_div = std::make_unique<BinaryOp>(
            BinaryOp::Operator::Divide,
            std::move(len_op),
            std::move(two_literal)
        );
        
        // Create GETVEC call: GETVEC(input_size / 2)
        std::vector<ExprPtr> getvec_args;
        getvec_args.push_back(std::move(size_div));
        auto getvec_call = std::make_unique<FunctionCall>(
            std::make_unique<VariableAccess>("GETVEC"), std::move(getvec_args));
        
        // Create assignment: result_var = GETVEC(input_size / 2)  
        std::vector<ExprPtr> result_lhs;
        result_lhs.push_back(std::make_unique<VariableAccess>(result_var));
        std::vector<ExprPtr> result_rhs;
        result_rhs.push_back(std::move(getvec_call));
        
        current_basic_block->add_statement(std::make_unique<AssignmentStatement>(
            std::move(result_lhs), std::move(result_rhs)
        ));

        // Create the pairwise reduction loop statement with NEON intrinsic for maximum
        auto pairwise_stmt = std::make_unique<PairwiseReductionLoopStatement>(
            left_vector_name, right_vector_name, result_var,
            "llvm.arm.neon.vpmax.v4f32",  // NEON intrinsic for pairwise maximum
            reducer.getReductionOp()
        );

        current_basic_block->add_statement(std::move(pairwise_stmt));

        if (trace_enabled_) {
            std::cout << "[CFGBuilderPass] Completed pairwise " << reducer.getName() 
                      << " CFG generation with NEON intrinsics" << std::endl;
        }
    }

    void CFGBuilderPass::generatePairwiseReductionCFG(Expression* left_expr, Expression* right_expr,
                                                     const std::string& result_var, const PairwiseAddReducer& reducer) {
        if (trace_enabled_) {
            std::cout << "[CFGBuilderPass] Building pairwise CFG for " << reducer.getName() 
                      << " operation using NEON intrinsics" << std::endl;
        }

        if (!current_basic_block) end_current_block_and_start_new();

        // Extract vector names from expressions (similar to other pairwise reducers)
        std::string left_vector_name, right_vector_name;
        if (auto* left_var = dynamic_cast<VariableAccess*>(left_expr)) {
            left_vector_name = left_var->name;
        } else {
            left_vector_name = "_pairwise_left_" + std::to_string(block_id_counter++);
            std::vector<ExprPtr> left_lhs;
            left_lhs.push_back(std::make_unique<VariableAccess>(left_vector_name));
            std::vector<ExprPtr> left_rhs;
            left_rhs.push_back(std::unique_ptr<Expression>(static_cast<Expression*>(left_expr->clone().release())));
            current_basic_block->add_statement(std::make_unique<AssignmentStatement>(
                std::move(left_lhs), std::move(left_rhs)
            ));
        }

        if (right_expr) {
            if (auto* right_var = dynamic_cast<VariableAccess*>(right_expr)) {
                right_vector_name = right_var->name;
            } else {
                right_vector_name = "_pairwise_right_" + std::to_string(block_id_counter++);
                std::vector<ExprPtr> right_lhs;
                right_lhs.push_back(std::make_unique<VariableAccess>(right_vector_name));
                std::vector<ExprPtr> right_rhs;
                right_rhs.push_back(std::unique_ptr<Expression>(static_cast<Expression*>(right_expr->clone().release())));
                current_basic_block->add_statement(std::make_unique<AssignmentStatement>(
                    std::move(right_lhs), std::move(right_rhs)
                ));
            }
        } else {
            // For single-argument pairwise operations, use the left vector as both operands
            right_vector_name = left_vector_name;
        }

        // Generate result vector allocation with proper size calculation
        // For pairwise operations: result_size = input_size / 2
        
        // Create a UnaryOp to get input vector size: LengthOf(input_vector)
        auto len_op = std::make_unique<UnaryOp>(
            UnaryOp::Operator::LengthOf, 
            std::make_unique<VariableAccess>(left_vector_name));
        
        // Create binary division: LengthOf(input_vector) / 2
        auto two_literal = std::make_unique<NumberLiteral>(static_cast<int64_t>(2));
        auto size_div = std::make_unique<BinaryOp>(
            BinaryOp::Operator::Divide,
            std::move(len_op),
            std::move(two_literal)
        );
        
        // Create GETVEC call: GETVEC(input_size / 2)
        std::vector<ExprPtr> getvec_args;
        getvec_args.push_back(std::move(size_div));
        auto getvec_call = std::make_unique<FunctionCall>(
            std::make_unique<VariableAccess>("GETVEC"), std::move(getvec_args));
        
        // Create assignment: result_var = GETVEC(input_size / 2)  
        std::vector<ExprPtr> result_lhs;
        result_lhs.push_back(std::make_unique<VariableAccess>(result_var));
        std::vector<ExprPtr> result_rhs;
        result_rhs.push_back(std::move(getvec_call));
        
        current_basic_block->add_statement(std::make_unique<AssignmentStatement>(
            std::move(result_lhs), std::move(result_rhs)
        ));

        // Create the pairwise reduction loop statement with NEON intrinsic for addition
        auto pairwise_stmt = std::make_unique<PairwiseReductionLoopStatement>(
            left_vector_name, right_vector_name, result_var,
            "llvm.arm.neon.vpadd.v4i32",  // NEON intrinsic for integer pairwise addition
            reducer.getReductionOp()
        );

        current_basic_block->add_statement(std::move(pairwise_stmt));

        if (trace_enabled_) {
            std::cout << "[CFGBuilderPass] Completed pairwise " << reducer.getName() 
                      << " CFG generation with NEON intrinsics" << std::endl;
        }
    }

    // === Component-wise Reduction CFG Generation for PAIRS with NEON Optimization ===
    
    void CFGBuilderPass::generateComponentWiseReductionCFG(Expression* left_expr, Expression* right_expr,
                                                          const std::string& result_var, const Reducer& reducer) {
        if (trace_enabled_) {
            std::cout << "[CFGBuilderPass] Building NEON-optimized component-wise CFG for " << reducer.getName() 
                      << " reduction on PAIRS collection" << std::endl;
        }

        // --- Step 1: Create unique names for NEON-optimized reduction control variables ---
        std::string left_vec_name;
        std::string left_size_name = "_pairs_size_" + std::to_string(block_id_counter++);
        std::string vector_chunks_name = "_vector_chunks_" + std::to_string(block_id_counter++);
        std::string remainder_name = "_remainder_" + std::to_string(block_id_counter++);
        std::string index_name = "_vec_idx_" + std::to_string(block_id_counter++);
        std::string x_accumulator_name = "_x_acc_" + std::to_string(block_id_counter++);
        std::string y_accumulator_name = "_y_acc_" + std::to_string(block_id_counter++);

        // Check if operand is a simple variable access (optimization)
        bool left_is_variable = false;
        if (auto* left_var = dynamic_cast<VariableAccess*>(left_expr)) {
            left_vec_name = left_var->name;
            left_is_variable = true;
        } else {
            left_vec_name = "_pairs_vec_" + std::to_string(block_id_counter++);
        }

        // --- Step 2: Register NEON-optimized temporary variables in symbol table ---
        if (symbol_table_) {
            if (!left_is_variable) {
                symbol_table_->addSymbol(Symbol(
                    left_vec_name, SymbolKind::LOCAL_VAR, VarType::PAIRS,
                    symbol_table_->currentScopeLevel(), current_cfg->function_name
                ));
            }
            symbol_table_->addSymbol(Symbol(
                left_size_name, SymbolKind::LOCAL_VAR, VarType::INTEGER,
                symbol_table_->currentScopeLevel(), current_cfg->function_name
            ));
            symbol_table_->addSymbol(Symbol(
                vector_chunks_name, SymbolKind::LOCAL_VAR, VarType::INTEGER,
                symbol_table_->currentScopeLevel(), current_cfg->function_name
            ));
            symbol_table_->addSymbol(Symbol(
                remainder_name, SymbolKind::LOCAL_VAR, VarType::INTEGER,
                symbol_table_->currentScopeLevel(), current_cfg->function_name
            ));
            symbol_table_->addSymbol(Symbol(
                index_name, SymbolKind::LOCAL_VAR, VarType::INTEGER,
                symbol_table_->currentScopeLevel(), current_cfg->function_name
            ));
            // NEON vector accumulators (will be mapped to NEON registers in codegen)
            symbol_table_->addSymbol(Symbol(
                x_accumulator_name, SymbolKind::LOCAL_VAR, VarType::QUAD,
                symbol_table_->currentScopeLevel(), current_cfg->function_name
            ));
            symbol_table_->addSymbol(Symbol(
                y_accumulator_name, SymbolKind::LOCAL_VAR, VarType::QUAD,
                symbol_table_->currentScopeLevel(), current_cfg->function_name
            ));
            symbol_table_->addSymbol(Symbol(
                result_var, SymbolKind::LOCAL_VAR, VarType::PAIR,
                symbol_table_->currentScopeLevel(), current_cfg->function_name
            ));
        }

        // --- Step 3: Initialize setup in current block ---
        if (!current_basic_block) end_current_block_and_start_new();

        // Store operand expression if not a variable
        if (!left_is_variable) {
            std::vector<ExprPtr> left_rhs;
            left_rhs.push_back(std::unique_ptr<Expression>(static_cast<Expression*>(left_expr->clone().release())));
            std::vector<ExprPtr> left_lhs;
            left_lhs.push_back(std::make_unique<VariableAccess>(left_vec_name));
            current_basic_block->add_statement(std::make_unique<AssignmentStatement>(
                std::move(left_lhs), std::move(left_rhs)
            ));
        }

        // Get collection size: LET pairs_size = LEN(pairs_vec)
        {
            std::vector<ExprPtr> size_rhs;
            size_rhs.push_back(std::make_unique<UnaryOp>(UnaryOp::Operator::LengthOf, std::make_unique<VariableAccess>(left_vec_name)));
            std::vector<ExprPtr> size_lhs;
            size_lhs.push_back(std::make_unique<VariableAccess>(left_size_name));
            current_basic_block->add_statement(std::make_unique<AssignmentStatement>(
                std::move(size_lhs), std::move(size_rhs)
            ));
        }

        // Calculate vector chunks: LET vector_chunks = pairs_size / 4 (4 pairs per NEON vector)
        {
            std::vector<ExprPtr> chunks_rhs;
            chunks_rhs.push_back(std::make_unique<BinaryOp>(
                BinaryOp::Operator::Divide,
                std::make_unique<VariableAccess>(left_size_name),
                std::make_unique<NumberLiteral>(static_cast<int64_t>(4))
            ));
            std::vector<ExprPtr> chunks_lhs;
            chunks_lhs.push_back(std::make_unique<VariableAccess>(vector_chunks_name));
            current_basic_block->add_statement(std::make_unique<AssignmentStatement>(
                std::move(chunks_lhs), std::move(chunks_rhs)
            ));
        }

        // Calculate remainder: LET remainder = pairs_size % 4
        {
            std::vector<ExprPtr> rem_rhs;
            rem_rhs.push_back(std::make_unique<BinaryOp>(
                BinaryOp::Operator::Remainder,
                std::make_unique<VariableAccess>(left_size_name),
                std::make_unique<NumberLiteral>(static_cast<int64_t>(4))
            ));
            std::vector<ExprPtr> rem_lhs;
            rem_lhs.push_back(std::make_unique<VariableAccess>(remainder_name));
            current_basic_block->add_statement(std::make_unique<AssignmentStatement>(
                std::move(rem_lhs), std::move(rem_rhs)
            ));
        }

        // Initialize NEON accumulators with reducer's initial value
        // x_acc = [initial, initial, initial, initial] (QUAD vector)
        {
            std::vector<ExprPtr> x_acc_rhs;
            std::vector<ExprPtr> quad_elements;
            for (int i = 0; i < 4; i++) {
                quad_elements.push_back(std::unique_ptr<Expression>(static_cast<Expression*>(reducer.getInitialValue()->clone().release())));
            }
            x_acc_rhs.push_back(std::make_unique<QuadExpression>(
                std::move(quad_elements[0]), std::move(quad_elements[1]),
                std::move(quad_elements[2]), std::move(quad_elements[3])
            ));
            std::vector<ExprPtr> x_acc_lhs;
            x_acc_lhs.push_back(std::make_unique<VariableAccess>(x_accumulator_name));
            current_basic_block->add_statement(std::make_unique<AssignmentStatement>(
                std::move(x_acc_lhs), std::move(x_acc_rhs)
            ));
        }

        // y_acc = [initial, initial, initial, initial] (QUAD vector)
        {
            std::vector<ExprPtr> y_acc_rhs;
            std::vector<ExprPtr> quad_elements;
            for (int i = 0; i < 4; i++) {
                quad_elements.push_back(std::unique_ptr<Expression>(static_cast<Expression*>(reducer.getInitialValue()->clone().release())));
            }
            y_acc_rhs.push_back(std::make_unique<QuadExpression>(
                std::move(quad_elements[0]), std::move(quad_elements[1]),
                std::move(quad_elements[2]), std::move(quad_elements[3])
            ));
            std::vector<ExprPtr> y_acc_lhs;
            y_acc_lhs.push_back(std::make_unique<VariableAccess>(y_accumulator_name));
            current_basic_block->add_statement(std::make_unique<AssignmentStatement>(
                std::move(y_acc_lhs), std::move(y_acc_rhs)
            ));
        }

        // Initialize index: LET idx = 0
        {
            std::vector<ExprPtr> idx_rhs;
            idx_rhs.push_back(std::make_unique<NumberLiteral>(static_cast<int64_t>(0)));
            std::vector<ExprPtr> idx_lhs;
            idx_lhs.push_back(std::make_unique<VariableAccess>(index_name));
            current_basic_block->add_statement(std::make_unique<AssignmentStatement>(
                std::move(idx_lhs), std::move(idx_rhs)
            ));
        }

        // --- Step 4: Create NEON-optimized loop blocks ---
        BasicBlock* vector_header_block = create_new_basic_block("NEONVectorHeader_");
        BasicBlock* vector_body_block = create_new_basic_block("NEONVectorBody_");
        BasicBlock* vector_increment_block = create_new_basic_block("NEONVectorIncrement_");
        BasicBlock* remainder_header_block = create_new_basic_block("RemainderHeader_");
        BasicBlock* remainder_body_block = create_new_basic_block("RemainderBody_");
        BasicBlock* remainder_increment_block = create_new_basic_block("RemainderIncrement_");
        BasicBlock* horizontal_reduction_block = create_new_basic_block("HorizontalReduction_");
        BasicBlock* exit_block = create_new_basic_block("ComponentWiseExit_");

        // --- Step 5: Connect to NEON vectorized loop ---
        current_basic_block->add_successor(vector_header_block);
        vector_header_block->add_predecessor(current_basic_block);

        // --- Step 6: NEON vectorized loop header ---
        current_basic_block = vector_header_block;

        // if (idx < vector_chunks) goto vector_body else goto remainder_header
        auto vector_condition = std::make_unique<BinaryOp>(
            BinaryOp::Operator::Less,
            std::make_unique<VariableAccess>(index_name),
            std::make_unique<VariableAccess>(vector_chunks_name)
        );
        auto vector_if_stmt = std::make_unique<IfStatement>(
            std::move(vector_condition),
            std::make_unique<CompoundStatement>(std::vector<StmtPtr>{})
        );
        current_basic_block->add_statement(std::move(vector_if_stmt));

        current_basic_block->add_successor(vector_body_block);
        current_basic_block->add_successor(remainder_header_block);
        vector_body_block->add_predecessor(current_basic_block);
        remainder_header_block->add_predecessor(current_basic_block);

        // --- Step 7: NEON vectorized loop body (VLD2 + vector reduction) ---
        current_basic_block = vector_body_block;

        // This is where the magic happens - we create a specialized NEON reduction statement
        // The code generator will recognize this pattern and emit:
        // 1. VLD2 to de-interleave pairs into separate X and Y vectors
        // 2. VMIN/VMAX/VADD to perform vectorized reduction
        auto neon_reduction_stmt = std::make_unique<PairwiseReductionLoopStatement>(
            left_vec_name,                    // source PAIRS vector
            "",                               // no second vector (single-operand reduction)
            result_var,                       // result variable
            reducer.getName() + "_COMPONENT", // intrinsic name hint for codegen
            reducer.getReductionOp()          // operation code
        );

        // Add metadata for NEON code generation
        neon_reduction_stmt->vector_a_name = left_vec_name;
        neon_reduction_stmt->result_vector_name = x_accumulator_name + "," + y_accumulator_name;
        neon_reduction_stmt->intrinsic_name = "vld2_deinterleave_" + reducer.getName();

        current_basic_block->add_statement(std::move(neon_reduction_stmt));

        current_basic_block->add_successor(vector_increment_block);
        vector_increment_block->add_predecessor(current_basic_block);

        // --- Step 8: NEON vectorized increment ---
        current_basic_block = vector_increment_block;

        // idx = idx + 1 (process next chunk of 4 pairs)
        {
            std::vector<ExprPtr> inc_rhs;
            inc_rhs.push_back(std::make_unique<BinaryOp>(
                BinaryOp::Operator::Add,
                std::make_unique<VariableAccess>(index_name),
                std::make_unique<NumberLiteral>(static_cast<int64_t>(1))
            ));
            std::vector<ExprPtr> inc_lhs;
            inc_lhs.push_back(std::make_unique<VariableAccess>(index_name));
            current_basic_block->add_statement(std::make_unique<AssignmentStatement>(
                std::move(inc_lhs), std::move(inc_rhs)
            ));
        }

        current_basic_block->add_successor(vector_header_block);
        vector_header_block->add_predecessor(current_basic_block);

        // --- Step 9: Handle remaining pairs (scalar fallback) ---
        current_basic_block = remainder_header_block;

        // Reset index for remainder processing: idx = vector_chunks * 4
        {
            std::vector<ExprPtr> idx_rhs;
            idx_rhs.push_back(std::make_unique<BinaryOp>(
                BinaryOp::Operator::Multiply,
                std::make_unique<VariableAccess>(vector_chunks_name),
                std::make_unique<NumberLiteral>(static_cast<int64_t>(4))
            ));
            std::vector<ExprPtr> idx_lhs;
            idx_lhs.push_back(std::make_unique<VariableAccess>(index_name));
            current_basic_block->add_statement(std::make_unique<AssignmentStatement>(
                std::move(idx_lhs), std::move(idx_rhs)
            ));
        }

        // if (idx < pairs_size) goto remainder_body else goto horizontal_reduction
        auto remainder_condition = std::make_unique<BinaryOp>(
            BinaryOp::Operator::Less,
            std::make_unique<VariableAccess>(index_name),
            std::make_unique<VariableAccess>(left_size_name)
        );
        auto remainder_if_stmt = std::make_unique<IfStatement>(
            std::move(remainder_condition),
            std::make_unique<CompoundStatement>(std::vector<StmtPtr>{})
        );
        current_basic_block->add_statement(std::move(remainder_if_stmt));

        current_basic_block->add_successor(remainder_body_block);
        current_basic_block->add_successor(horizontal_reduction_block);
        remainder_body_block->add_predecessor(current_basic_block);
        horizontal_reduction_block->add_predecessor(current_basic_block);

        // --- Step 10: Remainder processing (scalar component-wise) ---
        current_basic_block = remainder_body_block;

        // Process remaining pairs one at a time using scalar operations
        // This will be handled by regular scalar reduction logic
        // (Similar to the original component-wise logic but simpler)

        current_basic_block->add_successor(remainder_increment_block);
        remainder_increment_block->add_predecessor(current_basic_block);

        current_basic_block = remainder_increment_block;

        // idx = idx + 1
        {
            std::vector<ExprPtr> inc_rhs;
            inc_rhs.push_back(std::make_unique<BinaryOp>(
                BinaryOp::Operator::Add,
                std::make_unique<VariableAccess>(index_name),
                std::make_unique<NumberLiteral>(static_cast<int64_t>(1))
            ));
            std::vector<ExprPtr> inc_lhs;
            inc_lhs.push_back(std::make_unique<VariableAccess>(index_name));
            current_basic_block->add_statement(std::make_unique<AssignmentStatement>(
                std::move(inc_lhs), std::move(inc_rhs)
            ));
        }

        current_basic_block->add_successor(remainder_header_block);
        remainder_header_block->add_predecessor(current_basic_block);

        // --- Step 11: Horizontal reduction (reduce NEON vectors to scalars) ---
        current_basic_block = horizontal_reduction_block;

        // Create horizontal reduction statements for X and Y components
        // x_result = PAIRWISE_MIN/MAX/ADD(x_accumulator) until scalar
        // y_result = PAIRWISE_MIN/MAX/ADD(y_accumulator) until scalar

        std::string final_x_name = "_final_x_" + std::to_string(block_id_counter++);
        std::string final_y_name = "_final_y_" + std::to_string(block_id_counter++);

        if (symbol_table_) {
            symbol_table_->addSymbol(Symbol(
                final_x_name, SymbolKind::LOCAL_VAR, VarType::INTEGER,
                symbol_table_->currentScopeLevel(), current_cfg->function_name
            ));
            symbol_table_->addSymbol(Symbol(
                final_y_name, SymbolKind::LOCAL_VAR, VarType::INTEGER,
                symbol_table_->currentScopeLevel(), current_cfg->function_name
            ));
        }

        // Use pairwise reduction to collapse vector to scalar
        std::string pairwise_reducer_name;
        if (reducer.getName() == "MIN") {
            pairwise_reducer_name = "PAIRWISE_MIN";
        } else if (reducer.getName() == "MAX") {
            pairwise_reducer_name = "PAIRWISE_MAX";  
        } else if (reducer.getName() == "SUM") {
            pairwise_reducer_name = "PAIRWISE_ADD";
        } else {
            pairwise_reducer_name = "PAIRWISE_ADD"; // fallback
        }

        auto x_horizontal_stmt = std::make_unique<ReductionStatement>(
            createReducer(pairwise_reducer_name),
            final_x_name,
            std::make_unique<VariableAccess>(x_accumulator_name),
            nullptr
        );
        current_basic_block->add_statement(std::move(x_horizontal_stmt));

        auto y_horizontal_stmt = std::make_unique<ReductionStatement>(
            createReducer(pairwise_reducer_name),
            final_y_name,
            std::make_unique<VariableAccess>(y_accumulator_name),
            nullptr
        );
        current_basic_block->add_statement(std::move(y_horizontal_stmt));

        // Combine final results into result pair: result = PAIR(final_x, final_y)
        {
            std::vector<ExprPtr> result_rhs;
            result_rhs.push_back(std::make_unique<PairExpression>(
                std::make_unique<VariableAccess>(final_x_name),
                std::make_unique<VariableAccess>(final_y_name)
            ));
            std::vector<ExprPtr> result_lhs;
            result_lhs.push_back(std::make_unique<VariableAccess>(result_var));
            current_basic_block->add_statement(std::make_unique<AssignmentStatement>(
                std::move(result_lhs), std::move(result_rhs)
            ));
        }

        current_basic_block->add_successor(exit_block);
        exit_block->add_predecessor(current_basic_block);

        // --- Step 12: Set current block to exit ---
        current_basic_block = exit_block;

        if (trace_enabled_) {
            std::cout << "[CFGBuilderPass] Generated NEON-optimized component-wise CFG blocks for " << reducer.getName() << " reduction:" << std::endl;
            std::cout << "  NEON Vector Header: " << vector_header_block->id << std::endl;
            std::cout << "  NEON Vector Body: " << vector_body_block->id << " (VLD2 + vector ops)" << std::endl;
            std::cout << "  NEON Vector Increment: " << vector_increment_block->id << std::endl;
            std::cout << "  Remainder Header: " << remainder_header_block->id << std::endl;
            std::cout << "  Remainder Body: " << remainder_body_block->id << " (scalar fallback)" << std::endl;
            std::cout << "  Remainder Increment: " << remainder_increment_block->id << std::endl;
            std::cout << "  Horizontal Reduction: " << horizontal_reduction_block->id << " (VPMIN/VPMAX/VPADD)" << std::endl;
            std::cout << "  Exit: " << exit_block->id << std::endl;
        }

        if (trace_enabled_) {
            std::cout << "[CFGBuilderPass] Completed NEON-optimized component-wise reduction CFG generation for PAIRS collection" << std::endl;
            std::cout << "[CFGBuilderPass] Expected NEON instructions: VLD2 (de-interleave), V" 
                      << (reducer.getName() == "MIN" ? "MIN" : reducer.getName() == "MAX" ? "MAX" : "ADD") 
                      << " (vector ops), VP" << (reducer.getName() == "MIN" ? "MIN" : reducer.getName() == "MAX" ? "MAX" : "ADD") 
                      << " (horizontal reduction)" << std::endl;
        }
    }