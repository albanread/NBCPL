#include "ClassPass.h"
#include "NameMangler.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <vector>

// --- Constructor ---
ClassPass::ClassPass(ClassTable& class_table, SymbolTable& symbol_table)
    : class_table_(class_table), symbol_table_(symbol_table) {}

// --- Main entry point ---
void ClassPass::run(Program& program) {
    if (trace_enabled_) std::cout << "[CLASSPASS] Starting class discovery phase..." << std::endl;
    discover_classes(program);
    if (trace_enabled_) std::cout << "[CLASSPASS] Starting class finalization phase..." << std::endl;
    finalize_layouts();
    
    if (trace_enabled_) {
        validate_all_class_layouts();
    }
}

// --- Discovery phase ---
void ClassPass::discover_classes(Program& program) {
    program.accept(*this);
}

void ClassPass::visit(Program& node) {
    for (const auto& decl : node.declarations) {
        if (decl) decl->accept(*this);
    }
}

void ClassPass::visit(ClassDeclaration& node) {
    if (trace_enabled_) {
        std::cout << "[CLASSPASS] Discovered class: " << node.name;
        if (!node.parent_class_name.empty())
            std::cout << " (extends " << node.parent_class_name << ")";
        std::cout << std::endl;
    }
    if (!class_table_.class_exists(node.name)) {
        class_table_.add_class(node.name, node.parent_class_name);
    }
    class_ast_nodes_[node.name] = &node;
}

// --- Finalization phase ---
void ClassPass::finalize_layouts() {
    std::unordered_set<std::string> resolution_path;
    for (const auto& pair : class_ast_nodes_) {
        const std::string& class_name = pair.first;
        ClassTableEntry* entry = class_table_.get_class(class_name);
        if (!entry->is_layout_finalized) {
            finalize_class_layout(class_name, resolution_path);
            if (trace_enabled_) {
                log_class_memory_layout(class_name);
            }
        }
    }
}

void ClassPass::finalize_class_layout(const std::string& class_name, std::unordered_set<std::string>& resolution_path) {
    if (resolution_path.count(class_name)) {
        throw std::runtime_error("Circular inheritance detected involving class: " + class_name);
    }
    resolution_path.insert(class_name);

    ClassTableEntry* entry = class_table_.get_class(class_name);
    if (!entry || entry->is_layout_finalized) {
        resolution_path.erase(class_name);
        return;
    }

    ClassDeclaration* class_node = class_ast_nodes_.at(class_name);
    size_t current_offset = sizeof(int64_t); // Start after vtable pointer

    // --- Step 1: Handle Inheritance ---
    ClassTableEntry* parent_entry = nullptr;
    if (!entry->parent_name.empty()) {
        parent_entry = class_table_.get_class(entry->parent_name);
        if (!parent_entry) throw std::runtime_error("Parent class not found: " + entry->parent_name);
        if (!parent_entry->is_layout_finalized) {
            finalize_class_layout(entry->parent_name, resolution_path);
        }
        entry->parent_ptr = parent_entry;
        
        // Inherit everything from parent
        entry->member_variables = parent_entry->member_variables;
        entry->member_methods = parent_entry->member_methods;
        entry->vtable_blueprint = parent_entry->vtable_blueprint;
        entry->simple_name_to_method = parent_entry->simple_name_to_method;
        current_offset = parent_entry->instance_size;
    }

    // --- Step 2: Ensure VTable has reserved slots ---
    if (entry->vtable_blueprint.size() < 2) {
        entry->vtable_blueprint.resize(2, "");
    }
    
    // --- Step 3: Process this class's own members and find user-defined RELEASE ---
    bool has_user_defined_release = false;
    for (const auto& member : class_node->members) {
        if (auto* let = dynamic_cast<LetDeclaration*>(member.declaration.get())) {
            for (const auto& var_name : let->names) {
                if (entry->member_variables.find(var_name) == entry->member_variables.end()) {
                    ClassMemberInfo member_info;
                    member_info.name = var_name;
                    member_info.type = VarType::UNKNOWN;
                    member_info.offset = current_offset;
                    member_info.visibility = member.visibility;
                    entry->add_member_variable(member_info);
                    if (trace_enabled_) std::cout << "[CLASSPASS] Member variable '" << var_name << "' in class '" << class_name << "' assigned offset " << current_offset << " with visibility " << (int)member.visibility << std::endl;
                    current_offset += sizeof(int64_t);
                }
            }
        } else if (auto* func = dynamic_cast<FunctionDeclaration*>(member.declaration.get())) {
            process_method(entry, func->name, false, false, func->is_virtual, func->is_final, member.visibility);
        } else if (auto* routine = dynamic_cast<RoutineDeclaration*>(member.declaration.get())) {
            if (routine->name == "RELEASE") {
                has_user_defined_release = true;
            }
            process_method(entry, routine->name, true, false, routine->is_virtual, routine->is_final, member.visibility);
        }
    }

    // --- Step 4: If user provided a RELEASE, inject the superclass call ---
    if (has_user_defined_release) {
        inject_superclass_release_calls(class_node, entry);
    }

    // --- Step 5: Synthesize default CREATE and RELEASE methods if they weren't inherited or user-defined ---
    if (entry->vtable_blueprint[0].empty()) {
        process_method(entry, "CREATE", true, true /* is_synthetic */, false, false, Visibility::Public);
        auto new_create = std::make_unique<RoutineDeclaration>("CREATE", std::vector<std::string>{}, std::make_unique<CompoundStatement>(std::vector<StmtPtr>{}));
        class_node->members.emplace_back(std::move(new_create), Visibility::Public);
    }
    
    if (entry->vtable_blueprint[1].empty()) {
        // Skip RELEASE method synthesis when SAMM is enabled to prevent double-frees
        if (HeapManager::getInstance().isSAMMEnabled()) {
            if (trace_enabled_) {
                std::cout << "[CLASSPASS] Skipping RELEASE synthesis for class " << class_name 
                          << " - SAMM is active" << std::endl;
            }
            // Still need to register the method in vtable, but with empty body
            process_method(entry, "RELEASE", true, true /* is_synthetic */, false, false, Visibility::Public);
            auto empty_body = std::make_unique<CompoundStatement>(std::vector<StmtPtr>{});
            auto new_release = std::make_unique<RoutineDeclaration>("RELEASE", std::vector<std::string>{}, std::move(empty_body));
            class_node->members.emplace_back(std::move(new_release), Visibility::Public);
        } else {
            process_method(entry, "RELEASE", true, true /* is_synthetic */, false, false, Visibility::Public);

            StmtPtr release_body;
            if (!entry->parent_name.empty()) {
                // Has a parent: body is SUPER.RELEASE()
                auto super_call = std::make_unique<SuperMethodCallExpression>("RELEASE", std::vector<ExprPtr>{});
                release_body = std::make_unique<RoutineCallStatement>(std::move(super_call), std::vector<ExprPtr>{});
            } else {
                // No parent: body is OBJECT_HEAP_FREE(_this)
                auto free_func = std::make_unique<VariableAccess>("OBJECT_HEAP_FREE");
                std::vector<ExprPtr> args;
                args.push_back(std::make_unique<VariableAccess>("_this"));
                release_body = std::make_unique<RoutineCallStatement>(std::move(free_func), std::move(args));
            }
            std::vector<StmtPtr> body_stmts;
            body_stmts.push_back(std::move(release_body));
            auto new_body = std::make_unique<CompoundStatement>(std::move(body_stmts));
            auto new_release = std::make_unique<RoutineDeclaration>("RELEASE", std::vector<std::string>{}, std::move(new_body));
            class_node->members.emplace_back(std::move(new_release), Visibility::Public);
        }
    }
    
    // --- Step 6: Inject initializers into the CREATE method ---
    inject_initializers(class_node, class_name);
    
    entry->instance_size = current_offset;
    entry->is_layout_finalized = true;

    if (trace_enabled_) std::cout << "[CLASSPASS] Finalized class: " << class_name << std::endl;
    resolution_path.erase(class_name);
}

void ClassPass::process_method(ClassTableEntry* entry, const std::string& method_name, bool is_routine, bool is_synthetic, bool is_virtual, bool is_final, Visibility visibility) {
    ClassMethodInfo minfo;
    minfo.name = method_name;
    minfo.qualified_name = NameMangler::mangleMethod(entry->name, method_name);
    minfo.is_virtual = is_virtual;
    minfo.is_final = is_final;
    minfo.visibility = visibility;

    // Check for special methods
    if (method_name == "CREATE") {
        minfo.vtable_slot = 0;
        entry->vtable_blueprint[0] = minfo.qualified_name;
        if (trace_enabled_ && !is_synthetic) std::cout << "[CLASSPASS] Method '" << method_name << "' placed in reserved vtable slot 0." << std::endl;
    } else if (method_name == "RELEASE") {
        minfo.vtable_slot = 1;
        entry->vtable_blueprint[1] = minfo.qualified_name;
        if (trace_enabled_) std::cout << "[CLASSPASS] Method '" << method_name << "' placed in reserved vtable slot 1." << std::endl;
    } else {
        // Check for override
        ClassMethodInfo* parent_method = entry->findMethod(method_name, false);
        if (parent_method) {
            // Check final: parent method must not be final
            if (parent_method->is_final) {
                std::cerr << "[CLASSPASS ERROR] Cannot override final method '" << method_name << "' in class '" << entry->name << "'" << std::endl;
                return;
            }
            // Inherit the slot from the parent method
            minfo.vtable_slot = parent_method->vtable_slot;
            entry->vtable_blueprint[minfo.vtable_slot] = minfo.qualified_name;
            if (trace_enabled_) std::cout << "[CLASSPASS] Method '" << method_name << "' overrides parent method at slot " << minfo.vtable_slot << std::endl;
        } else {
            // Assign new slot
            minfo.vtable_slot = entry->vtable_blueprint.size();
            entry->vtable_blueprint.push_back(minfo.qualified_name);
            if (trace_enabled_) std::cout << "[CLASSPASS] Method '" << method_name << "' assigned new slot " << minfo.vtable_slot << std::endl;
        }
    }
    // Store virtual/final info in method info
    entry->add_member_method(minfo);
}

void ClassPass::inject_initializers(ClassDeclaration* class_node, const std::string& class_name) {
    RoutineDeclaration* create_routine = nullptr;
    for (const auto& member : class_node->members) {
        if (auto* routine = dynamic_cast<RoutineDeclaration*>(member.declaration.get())) {
            if (routine->name == "CREATE") {
                create_routine = routine;
                break;
            }
        }
    }
    if (!create_routine) return; // Should not happen

    std::vector<StmtPtr> initializers;
    for (const auto& member : class_node->members) {
        if (auto* let = dynamic_cast<LetDeclaration*>(member.declaration.get())) {
            for (size_t i = 0; i < let->names.size(); ++i) {
                if (i < let->initializers.size() && let->initializers[i]) {
                    auto lhs = std::make_unique<MemberAccessExpression>(std::make_unique<VariableAccess>("_this"), let->names[i]);
                    std::vector<ExprPtr> lhs_vec;
                    lhs_vec.push_back(std::move(lhs));
                    std::vector<ExprPtr> rhs_vec;
                    rhs_vec.push_back(std::move(let->initializers[i]));
                    initializers.push_back(std::make_unique<AssignmentStatement>(std::move(lhs_vec), std::move(rhs_vec)));
                }
            }
        }
    }

    if (auto* body = dynamic_cast<CompoundStatement*>(create_routine->body.get())) {
        // Find the position to insert member initializations and SUPER call
        auto insert_pos = body->statements.begin();
        
        // Look for existing SUPER calls and find where to insert before them
        for (auto it = body->statements.begin(); it != body->statements.end(); ++it) {
            if (auto* routine_call = dynamic_cast<RoutineCallStatement*>(it->get())) {
                if (auto* super_call = dynamic_cast<SuperMethodCallExpression*>(routine_call->routine_expr.get())) {
                    insert_pos = it;
                    break;
                }
            }
        }
        
        // Insert member initializations first (before any SUPER calls)
        if (!initializers.empty()) {
            body->statements.insert(insert_pos, std::make_move_iterator(initializers.begin()), std::make_move_iterator(initializers.end()));
        }
        
        // Check if this class has a parent and needs a SUPER.CREATE() call
        const ClassTableEntry* entry = class_table_.get_class(class_name);
        if (entry && !entry->parent_name.empty()) {
            // Check if there's already a SUPER.CREATE call in the method
            bool has_super_create = false;
            for (const auto& stmt : body->statements) {
                if (auto* routine_call = dynamic_cast<RoutineCallStatement*>(stmt.get())) {
                    if (auto* super_call = dynamic_cast<SuperMethodCallExpression*>(routine_call->routine_expr.get())) {
                        if (super_call->member_name == "CREATE") {
                            has_super_create = true;
                            break;
                        }
                    }
                }
            }
            
            // If no SUPER.CREATE call exists, add one after member initializations
            if (!has_super_create) {
                // Create SUPER.CREATE() call with appropriate parameters from CREATE method signature
                std::vector<ExprPtr> super_args;
                for (const auto& param : create_routine->parameters) {
                    super_args.push_back(std::make_unique<VariableAccess>(param));
                }
                
                auto super_call = std::make_unique<SuperMethodCallExpression>("CREATE", std::move(super_args));
                auto super_stmt = std::make_unique<RoutineCallStatement>(std::move(super_call), std::vector<ExprPtr>{});
                
                // Insert after member initializations
                auto super_insert_pos = body->statements.begin() + initializers.size();
                body->statements.insert(super_insert_pos, std::move(super_stmt));
            }
        }
    }
}

// Inject automatic SUPER.RELEASE() chaining for user-defined RELEASE methods
void ClassPass::inject_superclass_release_calls(ClassDeclaration* class_node, ClassTableEntry* entry) {
    for (const auto& member : class_node->members) {
        if (auto* routine = dynamic_cast<RoutineDeclaration*>(member.declaration.get())) {
            if (routine->name == "RELEASE") {
                // Get the body, assuming it's a CompoundStatement
                if (auto* body = dynamic_cast<CompoundStatement*>(routine->body.get())) {
                    
                    if (entry->parent_name.empty()) {
                        // THIS IS A BASE CLASS: Inject OBJECT_HEAP_FREE(SELF) only if SAMM is disabled
                        if (!HeapManager::getInstance().isSAMMEnabled()) {
                            // 1. Create the function call expression: OBJECT_HEAP_FREE(SELF)
                            auto free_func = std::make_unique<VariableAccess>("OBJECT_HEAP_FREE");
                            std::vector<ExprPtr> args;
                            args.push_back(std::make_unique<VariableAccess>("SELF"));
                            
                            // 2. Create the statement for the call
                            auto free_stmt = std::make_unique<RoutineCallStatement>(std::move(free_func), std::move(args));
                            
                            // 3. Add it to the end of the user's code
                            body->statements.push_back(std::move(free_stmt));
                            
                            if (trace_enabled_) {
                                std::cout << "[CLASSPASS] Injected OBJECT_HEAP_FREE(SELF) into user-defined RELEASE for base class '" << entry->name << "'." << std::endl;
                            }
                        } else {
                            if (trace_enabled_) {
                                std::cout << "[CLASSPASS] Skipping OBJECT_HEAP_FREE injection for base class '" << entry->name << "' - SAMM is active" << std::endl;
                            }
                        }

                    } else {
                        // THIS IS A DERIVED CLASS: Inject SUPER.RELEASE() only if SAMM is disabled
                        if (!HeapManager::getInstance().isSAMMEnabled()) {
                            auto super_call = std::make_unique<SuperMethodCallExpression>("RELEASE", std::vector<ExprPtr>{});
                            auto super_stmt = std::make_unique<RoutineCallStatement>(std::move(super_call), std::vector<ExprPtr>{});
                            body->statements.push_back(std::move(super_stmt));
                            
                            if (trace_enabled_) {
                                std::cout << "[CLASSPASS] Injected SUPER.RELEASE() into user-defined RELEASE for derived class '" << entry->name << "'." << std::endl;
                            }
                        } else {
                            if (trace_enabled_) {
                                std::cout << "[CLASSPASS] Skipping SUPER.RELEASE() injection for derived class '" << entry->name << "' - SAMM is active" << std::endl;
                            }
                        }
                    }
                }
                return; // Found and processed it
            }
        }
    }
}

// Validate the memory layout of all finalized classes
bool ClassPass::validate_all_class_layouts() const {
    if (!trace_enabled_) return true;

    std::cout << "\n[CLASSPASS] Validating memory layouts for all classes..." << std::endl;

    bool all_valid = true;
    for (const auto& pair : class_table_.entries()) {
        const std::string& class_name = pair.first;
        const ClassTableEntry* entry = pair.second.get();

        if (entry->is_layout_finalized) {
            bool is_valid = validate_class_layout(class_name);
            if (!is_valid) {
                all_valid = false;
                std::cerr << "[CLASSPASS ERROR] Invalid memory layout detected for class '"
                          << class_name << "'" << std::endl;
            } else {
                std::cout << "[CLASSPASS] Class '" << class_name << "' has valid memory layout" << std::endl;
                
                // Log the vtable for debugging
                std::cout << "[CLASSPASS] Vtable for class '" << class_name << "' has "
                          << entry->vtable_blueprint.size() << " slots:" << std::endl;
                for (size_t i = 0; i < entry->vtable_blueprint.size(); ++i) {
                    std::cout << "[CLASSPASS]   Slot " << i << ": " << entry->vtable_blueprint[i] << std::endl;
                }
            }
        }
    }

    if (all_valid) {
        std::cout << "[CLASSPASS] All class memory layouts are valid!" << std::endl;
    } else {
        std::cerr << "[CLASSPASS ERROR] Some class memory layouts have errors!" << std::endl;
    }

    return all_valid;
}

void ClassPass::log_class_memory_layout(const std::string& class_name) const {
    if (!trace_enabled_) return;

    const ClassTableEntry* entry = class_table_.get_class(class_name);
    if (!entry) {
        std::cerr << "[CLASSPASS ERROR] Cannot log layout for non-existent class: " << class_name << std::endl;
        return;
    }

    if (trace_enabled_) {
        std::cout << "\n[CLASSPASS] Memory layout for class '" << class_name
                  << "' (size: " << entry->instance_size << " bytes)" << std::endl;

        std::cout << "  [Offset 0]: vtable (8 bytes)" << std::endl;

        std::vector<std::pair<std::string, size_t>> sorted_members;
        for (const auto& pair : entry->member_variables) {
            sorted_members.emplace_back(pair.first, pair.second.offset);
        }

        // Sort members by offset
        std::sort(sorted_members.begin(), sorted_members.end(),
                  [](const auto& a, const auto& b) { return a.second < b.second; });

        for (const auto& pair : sorted_members) {
            std::cout << "  [Offset " << pair.second << "]: " << pair.first << " (8 bytes)" << std::endl;
        }

        std::cout << "\n  Vtable entries (" << entry->vtable_blueprint.size() << " slots):" << std::endl;
        for (size_t i = 0; i < entry->vtable_blueprint.size(); ++i) {
            std::cout << "    [Slot " << i << "]: " << entry->vtable_blueprint[i] << std::endl;
        }

        std::cout << std::endl;
    }
}

// Validate a single class's memory layout for correctness
bool ClassPass::validate_class_layout(const std::string& class_name) const {
    const ClassTableEntry* entry = class_table_.get_class(class_name);
    if (!entry) {
        std::cerr << "[CLASSPASS ERROR] Cannot validate non-existent class: " << class_name << std::endl;
        return false;
    }

    bool is_valid = true;

    // Check 1: Ensure no member variables have offset 0 (vtable pointer location)
    for (const auto& pair : entry->member_variables) {
        if (pair.second.offset < sizeof(int64_t)) {
            std::cerr << "[CLASSPASS ERROR] Member '" << pair.first
                      << "' in class '" << class_name
                      << "' has invalid offset " << pair.second.offset
                      << " (must be >= " << sizeof(int64_t) << ")" << std::endl;
            is_valid = false;
        }
    }
    
    // Check vtable method overrides
    if (!entry->parent_name.empty()) {
        const ClassTableEntry* parent = class_table_.get_class(entry->parent_name);
        if (parent && parent->is_layout_finalized) {
            // Build a map of method names to slots for quick lookup
            std::unordered_map<std::string, size_t> method_name_map;
            
            // First pass: Extract simple method names from parent's vtable
            for (size_t i = 0; i < parent->vtable_blueprint.size(); i++) {
                const std::string& qualified_name = parent->vtable_blueprint[i];
                size_t pos = qualified_name.rfind("::");
                if (pos != std::string::npos) {
                    std::string simple_name = qualified_name.substr(pos + 2);
                    method_name_map[simple_name] = i;
                }
            }
            
            // Second pass: Check if child methods with the same simple name
            // properly override the parent's vtable slots
            for (const auto& method_pair : entry->member_methods) {
                const std::string& qualified_name = method_pair.first;
                const ClassMethodInfo& method_info = method_pair.second;
                if (qualified_name.find(class_name + "::") == 0) {  // Only check this class's own methods
                    const std::string& simple_name = method_info.name;
                    
                    // If parent has a method with this name, check for proper override
                    if (method_name_map.count(simple_name) > 0) {
                        size_t expected_slot = method_name_map[simple_name];
                        
                        // Make sure this class overrides that slot in its vtable
                        if (expected_slot >= entry->vtable_blueprint.size() ||
                            entry->vtable_blueprint[expected_slot] != qualified_name) {
                            std::cerr << "[CLASSPASS ERROR] Method override issue in class '" << class_name
                                     << "': Method '" << simple_name << "' should override slot "
                                     << expected_slot << " but doesn't" << std::endl;
                            is_valid = false;
                        }
                    }
                }
            }
        }
    }

    // Check 2: If this is a derived class, ensure it includes parent members
    if (!entry->parent_name.empty()) {
        const ClassTableEntry* parent = class_table_.get_class(entry->parent_name);
        if (!parent) {
            std::cerr << "[CLASSPASS ERROR] Parent class not found: " << entry->parent_name << std::endl;
            return false;
        }

        // Ensure all parent members exist in this class
        for (const auto& parent_pair : parent->member_variables) {
            if (entry->member_variables.find(parent_pair.first) == entry->member_variables.end()) {
                std::cerr << "[CLASSPASS ERROR] Parent member '" << parent_pair.first
                          << "' not found in derived class '" << class_name << "'" << std::endl;
                is_valid = false;
            }
        }

        // Check that instance size is at least as large as parent's
        if (entry->instance_size < parent->instance_size) {
            std::cerr << "[CLASSPASS ERROR] Class '" << class_name
                      << "' has instance size " << entry->instance_size
                      << " which is smaller than parent class '" << entry->parent_name
                      << "' size " << parent->instance_size << std::endl;
            is_valid = false;
        }
    }

    // Check 3: Ensure instance size is large enough for all member variables
    for (const auto& pair : entry->member_variables) {
        size_t member_end = pair.second.offset + sizeof(int64_t);
        if (member_end > entry->instance_size) {
            std::cerr << "[CLASSPASS ERROR] Class '" << class_name
                      << "' has instance size " << entry->instance_size
                      << " which is too small for member '" << pair.first
                      << "' ending at offset " << member_end << std::endl;
            is_valid = false;
        }
    }

    return is_valid;
}

void ClassPass::visit(FunctionDeclaration& node) {
    if (current_class_name_.empty()) {
        return; // Only process methods within a class context
    }
    ClassTableEntry* entry = class_table_.get_class(current_class_name_);
    if (entry) {
        process_method(entry, node.name, false, false, node.is_virtual, node.is_final, Visibility::Public);
    }
    ASTVisitor::visit(node);
}

void ClassPass::visit(RoutineDeclaration& node) {
    if (current_class_name_.empty()) {
        return; // Only process methods within a class context
    }
    ClassTableEntry* entry = class_table_.get_class(current_class_name_);
    if (entry) {
        process_method(entry, node.name, true, false, node.is_virtual, node.is_final, Visibility::Public);
    }
    ASTVisitor::visit(node);
}
