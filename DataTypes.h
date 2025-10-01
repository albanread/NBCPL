#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <string>
#include <vector>
#include <cstdint>
#include <memory>
#include <optional>
#include <map>
#include <set>
#include <iostream>

// Forward declare Expression class to avoid circular dependency
class Expression;

// Define ExprPtr using the forward declaration
using ExprPtr = std::unique_ptr<Expression>;

// Define VarType here so it's available to all files that include DataTypes.h
enum class VarType : int64_t {
    UNKNOWN      = 0,
    
    // Simple types (low bits, low priority)
    INTEGER      = 1 << 0, // 1
    FLOAT        = 1 << 1, // 2
    STRING       = 1 << 2, // 4
    ANY          = 1 << 3, // 8
    PAIR         = 1 << 4, // 16
    FPAIR        = 1 << 5, // 32
    
    // Container types (higher bits, higher priority)
    VEC          = 1 << 10, // 1024
    LIST         = 1 << 11, // 2048  
    TABLE        = 1 << 12, // 4096
    OBJECT       = 1 << 13, // 8192
    
    // Type Modifiers (highest bits, highest priority when combined)
    POINTER_TO   = 1 << 20, // 1048576
    CONST        = 1 << 21, // 2097152
    NOTUSED      = 1 << 22, // 4194304

    // --- New Composite Type ---
    POINTER_TO_OBJECT = POINTER_TO | OBJECT,

    // --- Examples of Combined Types (for convenience, optional) ---
    POINTER_TO_INT_LIST      = POINTER_TO | LIST | INTEGER,      // 1048576 | 2048 | 1 = 1050625
    POINTER_TO_FLOAT_LIST    = POINTER_TO | LIST | FLOAT,        // 1048576 | 2048 | 2 = 1050626
    POINTER_TO_STRING_LIST   = POINTER_TO | LIST | STRING,       // 1048576 | 2048 | 4 = 1050628
    POINTER_TO_ANY_LIST      = POINTER_TO | LIST | ANY,          // 1048576 | 2048 | 8 = 1050632
    CONST_POINTER_TO_INT_LIST    = CONST | POINTER_TO | LIST | INTEGER,   // 2097152 | 1048576 | 2048 | 1 = 3147777
    CONST_POINTER_TO_FLOAT_LIST  = CONST | POINTER_TO | LIST | FLOAT,     // 2097152 | 1048576 | 2048 | 2 = 3147778
    CONST_POINTER_TO_STRING_LIST = CONST | POINTER_TO | LIST | STRING,    // 2097152 | 1048576 | 2048 | 4 = 3147780
    CONST_POINTER_TO_ANY_LIST    = CONST | POINTER_TO | LIST | ANY,       // 2097152 | 1048576 | 2048 | 8 = 3147784

    // --- Legacy/compatibility composite types ---
    POINTER_TO_INT_VEC      = POINTER_TO | VEC | INTEGER,
    POINTER_TO_FLOAT_VEC    = POINTER_TO | VEC | FLOAT,
    POINTER_TO_STRING       = POINTER_TO | STRING,
    POINTER_TO_TABLE        = POINTER_TO | TABLE,
    POINTER_TO_FLOAT        = POINTER_TO | FLOAT,
    POINTER_TO_INT          = POINTER_TO | INTEGER,
    POINTER_TO_LIST_NODE    = POINTER_TO | LIST
};

// Helper function to check for const list types
inline bool is_const_list_type(VarType t) {
    // A const list must have the POINTER_TO, LIST, and CONST flags set.
    const int64_t required_flags = static_cast<int64_t>(VarType::POINTER_TO) |
                                  static_cast<int64_t>(VarType::LIST) |
                                  static_cast<int64_t>(VarType::CONST);
    return (static_cast<int64_t>(t) & required_flags) == required_flags;
}

enum class FunctionType {
    STANDARD, // For functions using X registers (int/pointer)
    FLOAT     // For functions using D registers (float/double)
};

// Utility to display VarType as a string based on its bitfield flags
// Simplified type priority system: since bits are ordered by complexity,
// we can just use the numeric value directly for priority comparison
inline bool should_update_type(VarType current_type, VarType new_type) {
    int64_t current_value = static_cast<int64_t>(current_type);
    int64_t new_value = static_cast<int64_t>(new_type);
    
    // Allow update if new type has higher or equal numeric value
    // Higher bits = more complex/specific types = higher priority
    return new_value >= current_value;
}

inline std::string vartype_to_string(VarType t) {
    int64_t v = static_cast<int64_t>(t);
    if (v == 0) return "UNKNOWN";

    std::string result;
    if (v & static_cast<int64_t>(VarType::CONST)) result += "CONST|";
    if (v & static_cast<int64_t>(VarType::POINTER_TO)) result += "POINTER_TO|";
    if (v & static_cast<int64_t>(VarType::LIST)) result += "LIST|";
    if (v & static_cast<int64_t>(VarType::VEC)) result += "VEC|";
    if (v & static_cast<int64_t>(VarType::TABLE)) result += "TABLE|";
    if (v & static_cast<int64_t>(VarType::OBJECT)) result += "OBJECT|";
    if (v & static_cast<int64_t>(VarType::INTEGER)) result += "INTEGER|";
    if (v & static_cast<int64_t>(VarType::FLOAT)) result += "FLOAT|";
    if (v & static_cast<int64_t>(VarType::STRING)) result += "STRING|";
    if (v & static_cast<int64_t>(VarType::ANY)) result += "ANY|";
    if (v & static_cast<int64_t>(VarType::PAIR)) result += "PAIR|";
    if (v & static_cast<int64_t>(VarType::FPAIR)) result += "FPAIR|";
    if (v & static_cast<int64_t>(VarType::NOTUSED)) result += "NOTUSED|";

    // Remove trailing '|'
    if (!result.empty() && result.back() == '|') result.pop_back();
    return result;
}

// Structure to hold information about a string literal
struct StringLiteralInfo {
    std::string label;
    std::u32string value;
};

// Structure to hold information about a float literal
struct FloatLiteralInfo {
    std::string label;
    double value;
};

// Structure to hold information about a pair literal
struct PairLiteralInfo {
    std::string label;
    int64_t first_value;
    int64_t second_value;
};

// Structure to hold function metrics, including parameter types for granular inference
struct FunctionMetrics {
    // ... existing members ...
    int num_parameters = 0;
    int num_variables = 0;      // INTEGER variables
    int num_runtime_calls = 0;
    int num_local_function_calls = 0;
    int num_local_routine_calls = 0;
    bool has_vector_allocations = false;
    bool accesses_globals = false;
    std::map<std::string, int> parameter_indices;
    int max_live_variables = 0; // Track peak register pressure for this function
    int instruction_count = 0;  // Track approximate number of instructions for call site detection

    // START of new members
    int num_float_parameters = 0; // For future use
    int num_float_variables = 0;
    std::map<std::string, VarType> variable_types;

    // --- Optimization Flags ---
    // Generic Leaf Detection: True if the function makes NO calls.
    bool is_leaf = true; // Assume true until a call is found.

    // SAMM Optimization: True if the function performs any heap allocation.
    // If false and is_leaf is true, SAMM scope calls can be skipped entirely.
    bool performs_heap_allocation = false;

    // Trivial Accessor Detection: True if the method only returns a member variable or SELF.
    bool is_trivial_accessor = false;
    // Trivial Setter Detection: True if the method only sets a member variable from a parameter.
    bool is_trivial_setter = false;
    // If is_trivial_accessor or is_trivial_setter is true, the name of the accessed member.
    // Use the special value "_this_ptr" if the method returns SELF/_this.
    std::string accessed_member_name = "";
    // Inlining: True if the method is safe to inline at call sites (trivial accessor/setter).
    bool is_safe_to_inline = false;
    // --- End Optimization Flags ---

    // END of new members

    std::map<std::string, VarType> parameter_types; // Maps parameter name to its type

    // Tracks how many temporary callee-saved registers are required for this function
    int required_callee_saved_temps = 0;
    
    // Set of specific callee-saved registers that must be saved for this function
    std::set<std::string> required_callee_saved_regs;
    
    // Track if function has expressions that need callee-saved temps (like N * FUNC(N-1))
    bool has_call_preserving_expressions = false;
};

// Structure to hold information about a static variable
struct StaticVariableInfo {
    std::string label;
    ExprPtr initializer;
};

#endif // DATA_TYPES_H
