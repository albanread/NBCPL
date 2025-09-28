#include "ASTAnalyzer.h"

// Visitor implementation for ResultisStatement nodes
void ASTAnalyzer::visit(ResultisStatement& node) {
    if (node.expression) {
        node.expression->accept(*this);
        
        // Handle SEND semantics: if this ResultisStatement came from SEND syntax,
        // automatically RETAIN the variable being returned
        if (node.is_send && symbol_table_) {
            if (auto* var_access = dynamic_cast<VariableAccess*>(node.expression.get())) {
                Symbol symbol;
                if (symbol_table_->lookup(var_access->name, symbol)) {
                    if (symbol.owns_heap_memory) {
                        symbol.owns_heap_memory = false;
                        symbol_table_->updateSymbol(var_access->name, symbol);
                        if (trace_enabled_) {
                            std::cerr << "[SEND SEMANTICS] Automatically RETAINed variable '" 
                                      << var_access->name << "'" << std::endl;
                        }
                    } else if (trace_enabled_) {
                        std::cerr << "[SEND WARNING] Variable '" << var_access->name 
                                  << "' was not heap-allocated, SEND has no effect" << std::endl;
                    }
                }
            }
        }
        
        // Determine if this is a floating-point result
        bool returns_float = false;
        
        // Check if the expression is a floating point literal
        if (auto* num_lit = dynamic_cast<NumberLiteral*>(node.expression.get())) {
            if (num_lit->literal_type == NumberLiteral::LiteralType::Float) {
                returns_float = true;
            }
        }
        // Check if the expression is a float operation using type inference
        else if (infer_expression_type(node.expression.get()) == VarType::FLOAT) {
            returns_float = true;
        }
        // Check if expression is a FLOAT() call
        else if (auto* func_call = dynamic_cast<FunctionCall*>(node.expression.get())) {
            if (auto* var_access = dynamic_cast<VariableAccess*>(func_call->function_expr.get())) {
                if (var_access->name == "FLOAT") {
                    returns_float = true;
                }
            }
        }
        
        // Set the current function's return type if it's in a ValofExpression
        if (returns_float && !current_function_scope_.empty()) {
            function_return_types_[current_function_scope_] = VarType::FLOAT;
        }
    }
}
