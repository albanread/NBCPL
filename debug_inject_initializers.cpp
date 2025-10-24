#include "ClassPass.h"
#include "NameMangler.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <vector>

// Debug version of inject_initializers to see what's happening
void debug_inject_initializers(ClassDeclaration* class_node, const std::string& class_name) {
    std::cout << "\n[DEBUG_INJECT] ======= Starting inject_initializers for class: " << class_name << " =======" << std::endl;
    
    // Step 1: Find the CREATE routine
    RoutineDeclaration* create_routine = nullptr;
    std::cout << "[DEBUG_INJECT] Looking for CREATE routine..." << std::endl;
    for (const auto& member : class_node->members) {
        if (auto* routine = dynamic_cast<RoutineDeclaration*>(member.declaration.get())) {
            std::cout << "[DEBUG_INJECT] Found routine: " << routine->name << std::endl;
            if (routine->name == "CREATE") {
                create_routine = routine;
                std::cout << "[DEBUG_INJECT] Found CREATE routine!" << std::endl;
                break;
            }
        }
    }
    
    if (!create_routine) {
        std::cout << "[DEBUG_INJECT] ERROR: No CREATE routine found!" << std::endl;
        return;
    }

    // Step 2: Look for LET declarations with initializers
    std::cout << "[DEBUG_INJECT] Looking for LET declarations with initializers..." << std::endl;
    std::vector<StmtPtr> initializers;
    
    for (const auto& member : class_node->members) {
        std::cout << "[DEBUG_INJECT] Examining member..." << std::endl;
        
        if (auto* let = dynamic_cast<LetDeclaration*>(member.declaration.get())) {
            std::cout << "[DEBUG_INJECT] Found LetDeclaration with " << let->names.size() << " names and " << let->initializers.size() << " initializers" << std::endl;
            
            for (size_t i = 0; i < let->names.size(); ++i) {
                std::cout << "[DEBUG_INJECT] Processing name[" << i << "]: " << let->names[i] << std::endl;
                
                if (i < let->initializers.size()) {
                    if (let->initializers[i]) {
                        std::cout << "[DEBUG_INJECT] Found initializer for " << let->names[i] << "!" << std::endl;
                        
                        // Create the assignment: _this.member_name := initializer_value
                        auto lhs = std::make_unique<MemberAccessExpression>(
                            std::make_unique<VariableAccess>("_this"), 
                            let->names[i]
                        );
                        std::vector<ExprPtr> lhs_vec;
                        lhs_vec.push_back(std::move(lhs));
                        
                        std::vector<ExprPtr> rhs_vec;
                        // Note: We need to clone the initializer since we're moving it
                        rhs_vec.push_back(let->initializers[i]->clone());
                        
                        initializers.push_back(std::make_unique<AssignmentStatement>(
                            std::move(lhs_vec), 
                            std::move(rhs_vec)
                        ));
                        
                        std::cout << "[DEBUG_INJECT] Created assignment statement for " << let->names[i] << std::endl;
                    } else {
                        std::cout << "[DEBUG_INJECT] No initializer for " << let->names[i] << " (null pointer)" << std::endl;
                    }
                } else {
                    std::cout << "[DEBUG_INJECT] No initializer for " << let->names[i] << " (index out of range)" << std::endl;
                }
            }
        } else {
            std::cout << "[DEBUG_INJECT] Member is not a LetDeclaration" << std::endl;
        }
    }

    std::cout << "[DEBUG_INJECT] Found " << initializers.size() << " initializers to inject" << std::endl;

    // Step 3: Inject the initializers into the CREATE method
    if (auto* body = dynamic_cast<CompoundStatement*>(create_routine->body.get())) {
        std::cout << "[DEBUG_INJECT] CREATE routine has CompoundStatement body with " << body->statements.size() << " statements" << std::endl;
        
        // Find the position to insert member initializations
        auto insert_pos = body->statements.begin();
        
        // Look for existing SUPER calls and find where to insert before them
        for (auto it = body->statements.begin(); it != body->statements.end(); ++it) {
            if (auto* routine_call = dynamic_cast<RoutineCallStatement*>(it->get())) {
                if (auto* super_call = dynamic_cast<SuperMethodCallExpression*>(routine_call->routine_expr.get())) {
                    insert_pos = it;
                    std::cout << "[DEBUG_INJECT] Found SUPER call, will insert before it" << std::endl;
                    break;
                }
            }
        }
        
        // Insert member initializations first (before any SUPER calls)
        if (!initializers.empty()) {
            std::cout << "[DEBUG_INJECT] Inserting " << initializers.size() << " initializer statements into CREATE method" << std::endl;
            body->statements.insert(insert_pos, std::make_move_iterator(initializers.begin()), std::make_move_iterator(initializers.end()));
            std::cout << "[DEBUG_INJECT] Successfully inserted initializers. CREATE method now has " << body->statements.size() << " statements" << std::endl;
        } else {
            std::cout << "[DEBUG_INJECT] No initializers to insert" << std::endl;
        }
    } else {
        std::cout << "[DEBUG_INJECT] ERROR: CREATE routine body is not a CompoundStatement!" << std::endl;
    }
    
    std::cout << "[DEBUG_INJECT] ======= Finished inject_initializers for class: " << class_name << " =======\n" << std::endl;
}

// Test program to debug the initialization issue
int main() {
    std::cout << "Debug inject_initializers test program" << std::endl;
    std::cout << "This is a standalone debug program to understand the initialization issue." << std::endl;
    std::cout << "Compile and run the actual test with --trace-class-table to see ClassPass debug output." << std::endl;
    return 0;
}