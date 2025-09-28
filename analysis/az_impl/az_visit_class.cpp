#include "../ASTAnalyzer.h"
#include "../../DataTypes.h"

void ASTAnalyzer::visit(ClassDeclaration& node) {
    if (trace_enabled_) std::cout << "[ANALYZER TRACE] Visiting class." << std::endl;

    // 1. Set the class context for analyzing method bodies
    std::string previous_class_name = current_class_name_;
    current_class_name_ = node.name;

    if (!class_table_) {
        std::cerr << "[ASTAnalyzer] Error: class_table_ is null.\n";
        current_class_name_ = previous_class_name;
        return;
    }

    ClassTableEntry* entry = class_table_->get_class(node.name);
    if (!entry) {
        std::cerr << "[ASTAnalyzer] Error: Class '" << node.name << "' not found in table. ClassPass must run first.\n";
        current_class_name_ = previous_class_name;
        return;
    }

    // 2. Iterate through members to UPDATE their types
    for (const auto& member : node.members) {
        const auto& decl_ptr = member.declaration;
        if (!decl_ptr) continue;

        // Find member variables and update their types
        if (auto* let = dynamic_cast<LetDeclaration*>(decl_ptr.get())) {
            for (const auto& var_name : let->names) {
                auto it = entry->member_variables.find(var_name);
                if (it != entry->member_variables.end()) {
                    // This is the ONLY modification we make to member variables
                    it->second.type = let->is_float_declaration ? VarType::FLOAT : VarType::INTEGER;
                } else {
                    std::cerr << "[ASTAnalyzer] Warning: Member '" << var_name << "' in class '"
                              << node.name << "' not found in ClassTable for type update.\n";
                }
            }
        }
        // Recursively visit methods to analyze their bodies
        else if (dynamic_cast<FunctionDeclaration*>(decl_ptr.get()) || dynamic_cast<RoutineDeclaration*>(decl_ptr.get())) {
            decl_ptr->accept(*this);
        }
    }

    // 3. Restore the previous class context
    current_class_name_ = previous_class_name;
}
