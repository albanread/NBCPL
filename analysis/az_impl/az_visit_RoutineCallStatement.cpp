#include "ASTAnalyzer.h"
#include "RuntimeManager.h"
#include <iostream>
#include <vector>
#include <string>

/**
 * @brief Visits a RoutineCallStatement node.
 * Tracks local and runtime routine calls, and traverses arguments.
 */
void ASTAnalyzer::visit(RoutineCallStatement& node) {
    if (trace_enabled_) std::cout << "[ANALYZER TRACE] Entered ASTAnalyzer::visit(RoutineCallStatement&) for node at " << &node << std::endl;
    if (auto* var_access = dynamic_cast<VariableAccess*>(node.routine_expr.get())) {
        // --- SETTYPE INTRINSIC LOGIC ---
        if (var_access->name == "SETTYPE" && node.arguments.size() == 2) {
            auto* target_var = dynamic_cast<VariableAccess*>(node.arguments[0].get());
            auto* type_const_expr = node.arguments[1].get();

            if (target_var && type_const_expr) {
                // Statically evaluate the type constant expression.
                bool has_value;
                int64_t type_val = evaluate_constant_expression(type_const_expr, &has_value);

                if (has_value) {
                    VarType new_type = static_cast<VarType>(type_val);

                    // Update the type in the current function's metrics and the symbol table.
                    if (!current_function_scope_.empty()) {
                        function_metrics_[current_function_scope_].variable_types[target_var->name] = new_type;
                    }
                    if (symbol_table_) {
                        symbol_table_->updateSymbolType(target_var->name, new_type);
                    }
                }
            }
            // Return immediately to prevent code generation for this pseudo-call.
            return;
        }
        // --- END SETTYPE LOGIC ---

        // --- WRITEF VALIDATION LOGIC ---
        if (var_access->name == "WRITEF") {
            validate_writef_format_types(node);
        }
        // --- END WRITEF VALIDATION ---

        // --- CALL SITE TRACKING ---
        // Track the approximate instruction point for this routine call
        if (!current_function_scope_.empty()) {
            // Generate a synthetic instruction index for this call point
            int call_point = function_metrics_[current_function_scope_].instruction_count++;

            // Add this call site to the function's list
            function_call_sites_[current_function_scope_].push_back(call_point);
        }

        // --- LEAF DETECTION (GENERIC) ---
        // If we reached here and it wasn't SETTYPE, it's a real routine call.
        if (!current_function_scope_.empty()) {
            function_metrics_[current_function_scope_].is_leaf = false;
        }
        // --- END LEAF DETECTION ---

        if (is_local_routine(var_access->name)) {
            function_metrics_[current_function_scope_].num_local_routine_calls++;
            // Register in call graph for heap allocation propagation
            register_function_call(current_function_scope_, var_access->name);
            if (trace_enabled_) std::cout << "[ANALYZER TRACE]   Detected call to local routine: " << var_access->name << std::endl;
        } else if (RuntimeManager::instance().is_function_registered(var_access->name)) {
            function_metrics_[current_function_scope_].num_runtime_calls++;
            function_metrics_[current_function_scope_].accesses_globals = true; // <-- Ensure X28 is loaded for runtime calls
            // Runtime functions don't need to be in call graph as they don't perform heap allocation
            if (trace_enabled_) std::cout << "[ANALYZER TRACE]   Detected call to runtime function: " << var_access->name << ", Type: " << (get_runtime_function_type(var_access->name) == FunctionType::FLOAT ? "FLOAT" : "INTEGER") << std::endl;
        }
    }
    for (const auto& arg : node.arguments) {
        if (arg) arg->accept(*this);
    }

    // Increment the approximate instruction counter to account for the call instructions
    if (!current_function_scope_.empty()) {
        // Approximate the number of instructions generated for this call
        // Each argument + the call itself + possible cleanup
        function_metrics_[current_function_scope_].instruction_count += node.arguments.size() + 2;
    }
}

/**
 * @brief Validates WRITEF format string against provided argument types.
 */
void ASTAnalyzer::validate_writef_format_types(RoutineCallStatement& node) {
    if (node.arguments.empty()) {
        semantic_errors_.push_back("ERROR: WRITEF requires at least a format string argument");
        return;
    }

    // Check if the first argument is a string literal (format string)
    auto* format_literal = dynamic_cast<StringLiteral*>(node.arguments[0].get());
    if (!format_literal) {
        // Not a literal format string - we can't validate at compile time
        if (trace_enabled_) {
            std::cout << "[ANALYZER TRACE] WRITEF format string is not a literal - skipping validation" << std::endl;
        }
        return;
    }

    std::string format_str = format_literal->value;
    std::vector<char> specifiers = parse_writef_format_string(format_str);

    // Debug print: List all detected WRITEF format specifiers and the count if trace is enabled
    if (trace_enabled_) {
        std::cout << "[ANALYZER TRACE] WRITEF format specifiers detected: [";
        for (size_t i = 0; i < specifiers.size(); ++i) {
            std::cout << "%" << specifiers[i];
            if (i + 1 < specifiers.size()) std::cout << ", ";
        }
        std::cout << "] (count = " << specifiers.size() << ")" << std::endl;
    }

    // Check argument count
    size_t value_arg_count = node.arguments.size() - 1; // Exclude format string
    if (specifiers.size() != value_arg_count) {
        semantic_errors_.push_back("ERROR: WRITEF format string expects " +
                                 std::to_string(specifiers.size()) +
                                 " arguments but " + std::to_string(value_arg_count) +
                                 " were provided");
        return;
    }

    // Validate each argument type against its format specifier
    for (size_t i = 0; i < specifiers.size(); ++i) {
        VarType actual_type = infer_expression_type(node.arguments[i + 1].get());
        VarType expected_type = get_expected_type_for_writef_specifier(specifiers[i]);

        if (!are_types_compatible_for_writef(actual_type, expected_type)) {
            std::string error_msg = "WARNING: WRITEF format specifier %" +
                                  std::string(1, specifiers[i]) +
                                  " expects " + var_type_to_string(expected_type) +
                                  " but " + var_type_to_string(actual_type) +
                                  " provided at argument " + std::to_string(i + 2);
            semantic_errors_.push_back(error_msg);
        }
    }
}
/**
 * @brief Traverses the WRITEF format string to count and list all expected value specifiers.
 * * NOTE: This function's sole responsibility is to accurately count value arguments
 * and return their type specifiers. The runtime/type checker handles the actual
 * semantics of aliases (N/F) and custom specifiers (P, Q, S).
 * * @param format_str The string literal passed as the first argument to WRITEF.
 * @return std::vector<char> A list of characters representing the expected value types.
 */
std::vector<char> ASTAnalyzer::parse_writef_format_string(const std::string& format_str) {
    std::vector<char> specifiers;

    // NOTE: Keep this list in sync with the code generator's parse_format_string in gen_RoutineCallStatement.cpp!
    for (size_t i = 0; i < format_str.length(); ++i) {
        if (format_str[i] == '%' && i + 1 < format_str.length()) {
            char spec = format_str[i + 1];

            // List of all recognized specifiers that require a corresponding value argument.
            // This is the CRITICAL fix ensuring %P is counted.
            if (spec == 'd' || spec == 'f' || spec == 's' || spec == 'P' || spec == 'Q' ||
                spec == 'S' || spec == 'N' || spec == 'F' || spec == 'x' || spec == 'X' ||
                spec == 'o' || spec == 'c' || spec == 'R') {

                // Pushes the character as seen, letting the type checker handle its meaning.
                specifiers.push_back(spec);
            }
            i++;
        }

    }

    return specifiers;
}
/**
 * @brief Gets the expected VarType for a WRITEF format specifier.
 */
VarType ASTAnalyzer::get_expected_type_for_writef_specifier(char specifier) {
    switch (specifier) {
        case 'd': return VarType::INTEGER;
        case 'N': return VarType::INTEGER;
        case 'x': return VarType::INTEGER;
        case 'X': return VarType::ANY;  // %X accepts any value type for hex formatting
        case 'o': return VarType::INTEGER;
        case 'c': return VarType::INTEGER;
        case 'f': return VarType::FLOAT;
        case 'F': return VarType::FLOAT;
        case 's': return VarType::STRING;
        case 'P': return VarType::PAIR;
        case 'Q': return VarType::FPAIR;
        case 'R': return VarType::QUAD;
        default: return VarType::UNKNOWN;
    }
}

/**
 * @brief Checks if actual and expected types are compatible for WRITEF.
 */
bool ASTAnalyzer::are_types_compatible_for_writef(VarType actual, VarType expected) {
    if (actual == expected) {
        return true;
    }

    // %X (%ANY) accepts any value type for hexadecimal formatting
    if (expected == VarType::ANY) {
        return true;
    }

    // Allow some reasonable conversions
    if (expected == VarType::INTEGER && actual == VarType::UNKNOWN) {
        return true; // Unknown types default to integer-like behavior
    }

    if (expected == VarType::FLOAT && actual == VarType::INTEGER) {
        return true; // Integer can be promoted to float
    }

    // String types
    if (expected == VarType::STRING &&
        (actual == VarType::STRING || actual == VarType::POINTER_TO_STRING)) {
        return true;
    }

    return false;
}


/**
 * @brief Converts a VarType (potentially a composite bitmask) to a human-readable string,
 * using '^' as the shorthand for the POINTER_TO modifier.
 */
std::string ASTAnalyzer::var_type_to_string(VarType type) {

    // Check if the POINTER_TO flag is set.
    bool is_pointer = (static_cast<int64_t>(type) & static_cast<int64_t>(VarType::POINTER_TO)) != 0;

    // If it's a pointer, mask out the POINTER_TO modifier to get the base type.
    VarType base_type = type;
    if (is_pointer) {
        base_type = static_cast<VarType>(
            static_cast<int64_t>(type) & ~static_cast<int64_t>(VarType::POINTER_TO)
        );
    }

    // --- 1. Determine the Base Type String ---
    std::string base_type_str;

    switch (base_type) {
        // Simple Types
        case VarType::INTEGER:      base_type_str = "INTEGER"; break;
        case VarType::FLOAT:        base_type_str = "FLOAT"; break;
        case VarType::STRING:       base_type_str = "STRING"; break;
        case VarType::ANY:          base_type_str = "ANY"; break;
        case VarType::PAIR:         base_type_str = "PAIR"; break;
        case VarType::FPAIR:        base_type_str = "FPAIR"; break;
        case VarType::QUAD:         base_type_str = "QUAD"; break;
        case VarType::FQUAD:        base_type_str = "FQUAD"; break;

        // Container Types
        case VarType::VEC:          base_type_str = "VEC"; break;
        case VarType::LIST:         base_type_str = "LIST"; break;
        case VarType::TABLE:        base_type_str = "TABLE"; break;
        case VarType::OBJECT:       base_type_str = "OBJECT"; break;

        // Modifiers or Unhandled Base Types (This should rarely be hit if flags are correctly applied)
        case VarType::POINTER_TO:
        case VarType::CONST:
        case VarType::NOTUSED:
        case VarType::UNKNOWN:
        default:
            // If the base type is UNKNOWN, or we hit a default case, report the UNKNOWN state.
            return "UNKNOWN";
    }

    // --- 2. Apply Pointer and other Modifiers ---

    // Start with the base type string
    std::string final_type_str = base_type_str;

    // Apply the POINTER_TO modifier using the ^ shorthand
    if (is_pointer) {
        // For example, if base_type_str is "INTEGER", this returns "^INTEGER"
        final_type_str = "^" + final_type_str;
    }

    // Add logic here for other modifiers if needed (e.g., CONST)
    /*
    if ((static_cast<int64_t>(type) & static_cast<int64_t>(VarType::CONST)) != 0) {
        final_type_str = "CONST " + final_type_str;
    }
    */

    return final_type_str;
}
