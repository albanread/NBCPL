#include "GlobalInitializerPass.h"
#include <iostream>

// Main entry point for the pass
void GlobalInitializerPass::run(Program& program) {
    // 1. Traverse the AST to find all global initializers and move them
    //    into the collected_initializers_ vector.
    visit(program);

    // 2. If we found any initializers, proceed with injection.
    if (!collected_initializers_.empty()) {
        std::cout << "[GlobalInitPass] Found " << collected_initializers_.size() 
                  << " global initializers to hoist." << std::endl;

        // 3. Create the new Global routine from the collected statements.
        auto init_body = std::make_unique<CompoundStatement>(std::move(collected_initializers_));
        auto global_init_routine = std::make_unique<RoutineDeclaration>(
            "Global", 
            std::vector<std::string>{}, 
            std::move(init_body)
        );
        // Add it to the program's declarations.
        program.declarations.push_back(std::move(global_init_routine));

        // 4. Find the user's START routine and inject a call to Global.
        RoutineDeclaration* start_routine = find_start_routine(program);
        if (start_routine) {
            inject_global_init_call(start_routine);
            std::cout << "[GlobalInitPass] Injected call to Global into START routine." << std::endl;
        } else {
            std::cerr << "[GlobalInitPass] WARNING: START routine not found. Global initializers will not be executed." << std::endl;
        }
    }
}

// Visit the root Program node to traverse all declarations.
void GlobalInitializerPass::visit(Program& node) {
    // We only need to visit top-level declarations.
    for (auto& decl : node.declarations) {
        if (decl) {
            decl->accept(*this);
        }
    }
}

// This is the core logic: find initializers, create assignment statements,
// and then remove the initializers from the original declaration.
void GlobalInitializerPass::visit(GlobalVariableDeclaration& node) {
    for (size_t i = 0; i < node.names.size(); ++i) {
        if (i < node.initializers.size() && node.initializers[i]) {
            // Create an AssignmentStatement: <global_var> := <initializer>
            std::vector<ExprPtr> lhs_vec;
            lhs_vec.push_back(std::make_unique<VariableAccess>(node.names[i]));

            std::vector<ExprPtr> rhs_vec;
            // Move the initializer from the declaration to the new assignment
            rhs_vec.push_back(std::move(node.initializers[i]));
            
            auto assignment = std::make_unique<AssignmentStatement>(std::move(lhs_vec), std::move(rhs_vec));
            collected_initializers_.push_back(std::move(assignment));

            // The initializer has been moved, so the original declaration no longer has it.
            // This is automatically handled by std::move.
        }
    }
    // Ensure the initializers vector is now empty or full of nullptrs
    node.initializers.clear();
}

// Helper to find the START routine in the program
RoutineDeclaration* GlobalInitializerPass::find_start_routine(Program& program) {
    for (auto& decl : program.declarations) {
        if (auto* routine = dynamic_cast<RoutineDeclaration*>(decl.get())) {
            if (routine->name == "START") {
                return routine;
            }
        }
    }
    return nullptr;
}

// Helper to inject a call to Global at the start of a routine
void GlobalInitializerPass::inject_global_init_call(RoutineDeclaration* routine) {
    if (!routine || !routine->body) {
        return;
    }

    auto call_init = std::make_unique<RoutineCallStatement>(
        std::make_unique<VariableAccess>("Global"),
        std::vector<ExprPtr>{}
    );
    
    // Prepend the call to the body of START.
    if (auto* body_block = dynamic_cast<CompoundStatement*>(routine->body.get())) {
        body_block->statements.insert(body_block->statements.begin(), std::move(call_init));
    } else if (auto* block_stmt = dynamic_cast<BlockStatement*>(routine->body.get())) {
        block_stmt->statements.insert(block_stmt->statements.begin(), std::move(call_init));
    } else {
        std::cerr << "[GlobalInitPass] ERROR: Could not inject call. START routine body is neither CompoundStatement nor BlockStatement." << std::endl;
    }
}