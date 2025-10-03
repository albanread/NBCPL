#include "DataTypes.h"
#include "NewCodeGenerator.h"
#include <unordered_set>
#include "ClassTable.h"
#include "Encoder.h"
#include "RegisterManager.h"
#include "LabelManager.h"
#include "DataGenerator.h"
#include "NameMangler.h"
#include "CallFrameManager.h"
#include "analysis/ASTAnalyzer.h" // Required for analyzer_ and its methods
#include "LivenessAnalysisPass.h" // <-- Corrected include for liveness analysis
#include "analysis/LiveIntervalPass.h"
#include "AST.h" // Required for ASTNode types, Expression, Statement, etc.
#include "ASTVisitor.h"
#include <iostream>   // For std::cout, std::cerr
#include <stdexcept>  // For std::runtime_error
#include <algorithm>  // For std::sort
#include <sstream>    // For std::stringstream
#include <vector>     // For std::vector
#include "InstructionDecoder.h" // For duplicate MOV detection
#include <map>        // For std::map
#include <stack>      // For std::stack
#include "analysis/LiveInterval.h"
#include "analysis/LinearScanAllocator.h"
#include "runtime/ListDataTypes.h"
#include "RuntimeManager.h"
#include "generators/CodeGenUtils.h"
#include "HeapManager/HeapManager.h"
#include "VeneerManager.h"
#include "ExternalFunctionScanner.h"
using codegen_utils::emit_push_reg;
using codegen_utils::emit_pop_reg;



// --- Set/Clear current_class_name_ for class context tracking ---
void NewCodeGenerator::visit(ClassDeclaration& node) {
    debug_print("Visiting ClassDeclaration for '" + node.name + "'. Setting class context.");

    // Save the previous context in case of nested classes
    std::string previous_class_name = current_class_name_;
    current_class_name_ = node.name;

    // VERY IMPORTANT: Only visit the declarations that produce executable code,
    // which are the methods (Functions and Routines).
    // Do NOT visit the member variables (LetDeclaration/DECL).
    for (const auto& member : node.members) {
        if (dynamic_cast<FunctionDeclaration*>(member.declaration.get()) ||
            dynamic_cast<RoutineDeclaration*>(member.declaration.get()))
        {
            member.declaration->accept(*this);
        }
    }

    // Restore the previous context
    current_class_name_ = previous_class_name;
    debug_print("Finished with ClassDeclaration for '" + node.name + "'. Restoring context.");
}

// Empty visitor for ForEachStatement (CFGBuilderPass lowers this to simpler nodes)
void NewCodeGenerator::visit(ForEachStatement& node) {
    // No codegen needed here.
}

/**
 * @brief Generates the necessary cleanup code for a symbol that owns heap memory.
 * This includes checking for null, determining the object type, and calling the
 * appropriate deallocation routine (e.g., a RELEASE method or a runtime function).
 * @param symbol The symbol for which to generate cleanup code.
 */
void NewCodeGenerator::generate_cleanup_code_for_symbol(const Symbol& symbol) {
    // Skip legacy cleanup if SAMM is enabled to prevent double-frees
    if (HeapManager::getInstance().isSAMMEnabled()) {
        debug_print("Skipping legacy cleanup for symbol: " + symbol.name + " - SAMM is active");
        return;
    }
    
    debug_print("Generating cleanup code for symbol: " + symbol.name);

    // Labels for the null check
    std::string null_check_label = label_manager_.create_label();

    // 1. Load the variable's current value (which is a pointer) into a register.
    std::string ptr_reg = get_variable_register(symbol.name);

    // 2. Check if the pointer is null. If it is, skip the cleanup.
    emit(Encoder::create_cmp_imm(ptr_reg, 0));
    emit(Encoder::create_branch_conditional("EQ", null_check_label));

    // 3. Determine which cleanup routine to call based on the symbol's type.
    if (symbol.type == VarType::POINTER_TO_OBJECT) {
        // --- Object Cleanup (RELEASE method call) ---
        const ClassTableEntry* class_entry = class_table_->get_class(symbol.class_name);
        if (!class_entry || !class_entry->destructor) {
            // This should ideally be caught as a semantic error earlier.
            debug_print("WARNING: Class " + symbol.class_name + " has no RELEASE method for cleanup.");
        } else {
            // The object pointer is already in ptr_reg. Move it to X0 for the call.
            emit(Encoder::create_mov_reg("X0", ptr_reg));

            // Perform a virtual call to the RELEASE method (slot in the vtable).
            size_t vtable_offset = class_entry->destructor->vtable_slot * 8;
            std::string vtable_ptr_reg = register_manager_.acquire_scratch_reg(*this);
            std::string method_addr_reg = register_manager_.acquire_scratch_reg(*this);

            emit(Encoder::create_ldr_imm(vtable_ptr_reg, "X0", 0, "Load vtable for RELEASE"));
            emit(Encoder::create_ldr_imm(method_addr_reg, vtable_ptr_reg, vtable_offset, "Load RELEASE address"));
            emit(Encoder::create_branch_with_link_register(method_addr_reg));

            register_manager_.release_register(vtable_ptr_reg);
            register_manager_.release_register(method_addr_reg);
        }
    } else if (symbol.type == VarType::POINTER_TO_ANY_LIST || symbol.type == VarType::POINTER_TO_INT_LIST || symbol.type == VarType::POINTER_TO_FLOAT_LIST) {
        // --- List Cleanup (Standard) ---
        emit(Encoder::create_mov_reg("X0", ptr_reg));
        emit(Encoder::create_branch_with_link("BCPL_FREE_LIST"));
    } else {
        // --- Vector, String, Table Cleanup ---
        emit(Encoder::create_mov_reg("X0", ptr_reg));
        emit(Encoder::create_branch_with_link("FREEVEC"));
    }

    // 4. Define the label that is jumped to if the pointer was null.
    instruction_stream_.define_label(null_check_label);
    
    // Release the register that held the pointer.
    register_manager_.release_register(ptr_reg);
}

// Helper to get the class name for an object expression (for method calls)
std::string NewCodeGenerator::get_class_name_for_expression(Expression* expr) const {
    if (!expr) return "";

    // Case 1: NewExpression (e.g., new MyClass())
    if (auto* new_expr = dynamic_cast<NewExpression*>(expr)) {
        debug_print("Expression is a NewExpression with class_name: " + new_expr->class_name);
        return new_expr->class_name;
    }

    // Case 2: VariableAccess (look up in symbol table)
    if (auto* var_access = dynamic_cast<VariableAccess*>(expr)) {
        debug_print("Expression is a VariableAccess to: " + var_access->name);
        if (symbol_table_) {
            Symbol symbol;
            if (symbol_table_->lookup(var_access->name, symbol)) {
                if (!symbol.class_name.empty()) {
                    debug_print("Found symbol with class_name: " + symbol.class_name);
                    return symbol.class_name;
                } else {
                    debug_print("Symbol found but has no class_name");
                }
            } else {
                debug_print("Symbol not found in symbol table");
            }
        }
    }

    // Case 3: MemberAccessExpression (recursive resolution)
    if (auto* member_access = dynamic_cast<MemberAccessExpression*>(expr)) {
        debug_print("Expression is a MemberAccessExpression, looking up member: " + member_access->member_name);
        std::string object_class_name = get_class_name_for_expression(member_access->object_expr.get());

        if (!object_class_name.empty() && class_table_) {
            const ClassTableEntry* class_entry = class_table_->get_class(object_class_name);
            if (class_entry) {
                // Check if this member is a method that returns an object
                ClassMethodInfo* method_info = const_cast<ClassTableEntry*>(class_entry)->lookup_method(member_access->member_name);
                if (method_info) {
                    debug_print("Member is a method, but we can't determine its return type statically");
                    // Future improvement: track method return types
                }

                // Check if this is a member variable access
                auto var_it = class_entry->member_variables.find(member_access->member_name);
                if (var_it != class_entry->member_variables.end()) {
                    // Future improvement: track variable types
                    debug_print("Member is a variable, but we can't determine its type statically");
                }
            }
        }
    }

    // Case 4: FunctionCall (e.g., getPoint() that returns an object)
    if (auto* func_call = dynamic_cast<FunctionCall*>(expr)) {
        debug_print("Expression is a FunctionCall, checking return type");
        
        // Use the type analyzer to determine the return type
        ASTAnalyzer& analyzer = ASTAnalyzer::getInstance();
        VarType return_type = analyzer.infer_expression_type(func_call);
        
        debug_print("Function call return type: " + std::to_string(static_cast<int>(return_type)));
        
        // Check if this is a pointer to an object
        if ((static_cast<int64_t>(return_type) & static_cast<int64_t>(VarType::POINTER_TO)) &&
            (static_cast<int64_t>(return_type) & static_cast<int64_t>(VarType::OBJECT))) {
            
            // For now, we need to determine which specific class this is
            // This is a limitation - in a more sophisticated system, we'd track specific class types
            // For this implementation, we'll make some educated guesses based on context
            
            if (auto* func_var = dynamic_cast<VariableAccess*>(func_call->function_expr.get())) {
                debug_print("Function call to: " + func_var->name);
                
                // Heuristic: if the function name suggests it returns a specific type
                if (func_var->name.find("Point") != std::string::npos || 
                    func_var->name == "getPoint") {
                    debug_print("Inferred class name 'Point' from function name");
                    return "Point";
                }
                
                // Add more heuristics as needed for other classes
                // For now, default to Point for any function returning POINTER_TO_OBJECT
                debug_print("Defaulting to 'Point' class for POINTER_TO_OBJECT return type");
                return "Point";
            }
        }
    }

    // Add more cases as needed for your language's semantics
    // If not found, return empty string or throw
    debug_print("[WARNING] Could not determine class name for method call object expression.");
    std::cerr << "[CodeGen ERROR] Could not determine class name for method call object expression." << std::endl;
    return "";
}

// --- Constructor ---
// Initializes all member references and unique_ptr.
NewCodeGenerator::NewCodeGenerator(InstructionStream& instruction_stream,
                                   RegisterManager& register_manager,
                                   LabelManager& label_manager,
                                   bool debug,
                                   int debug_level,
                                   DataGenerator& data_generator,
                                   unsigned long long text_segment_size,
                                   const CFGBuilderPass& cfg_builder,
                                   ASTAnalyzer& analyzer,
                                   std::unique_ptr<SymbolTable> symbol_table,
                                   const std::map<std::string, std::map<std::string, LiveInterval>>& all_allocations,
                                   bool is_jit_mode,
                                   ClassTable* class_table,
                                   LivenessAnalysisPass& liveness_analyzer,
                                   bool bounds_checking_enabled)
: instruction_stream_(instruction_stream),
  register_manager_(register_manager),
  label_manager_(label_manager),
  debug_enabled_(debug),
  debug_level(debug_level),
  data_generator_(data_generator),
  data_segment_base_addr_(text_segment_size),
  cfg_builder_(cfg_builder),
  analyzer_(analyzer),
  symbol_table_(std::move(symbol_table)),
  all_allocations_(all_allocations),
  is_jit_mode_(is_jit_mode),
  class_table_(class_table),
  liveness_analyzer_(liveness_analyzer),
  bounds_checking_enabled_(bounds_checking_enabled)
{
    x28_is_loaded_in_current_function_ = false;
    // Set the class table pointer in DataGenerator for vtable and class info
    data_generator_.set_class_table(class_table_);
    // Set the symbol table pointer in DataGenerator for offset propagation
    data_generator_.set_symbol_table(symbol_table_.get());
}

// Private helper for type inference during code generation (without calling back to analyzer)
VarType NewCodeGenerator::infer_expression_type_local(const Expression* expr) const {
    if (!expr) {
        return VarType::INTEGER;
    }
    
    // Handle basic literal types
    if (auto* lit = dynamic_cast<const NumberLiteral*>(expr)) {
        return (lit->literal_type == NumberLiteral::LiteralType::Float) ? VarType::FLOAT : VarType::INTEGER;
    }
    
    if (dynamic_cast<const StringLiteral*>(expr)) {
        return VarType::POINTER_TO_STRING;
    }
    
    if (dynamic_cast<const CharLiteral*>(expr)) {
        return VarType::INTEGER; // Char literals are treated as integers
    }
    
    if (dynamic_cast<const BooleanLiteral*>(expr)) {
        return VarType::INTEGER; // Boolean literals are treated as integers
    }
    
    if (dynamic_cast<const NullLiteral*>(expr)) {
        return static_cast<VarType>(static_cast<int64_t>(VarType::POINTER_TO) | static_cast<int64_t>(VarType::ANY));
    }
    
    // Handle variable access using pre-computed metrics
    if (auto* var = dynamic_cast<const VariableAccess*>(expr)) {
        // Look up variable type from function metrics
        auto metrics_it = analyzer_.get_function_metrics().find(current_function_name_);
        if (metrics_it != analyzer_.get_function_metrics().end()) {
            const auto& metrics = metrics_it->second;
            
            // Check parameter types first
            auto param_it = metrics.parameter_types.find(var->name);
            if (param_it != metrics.parameter_types.end()) {
                return param_it->second;
            }
            
            // Check variable types
            auto var_it = metrics.variable_types.find(var->name);
            if (var_it != metrics.variable_types.end()) {
                return var_it->second;
            }
        }
        // Default to INTEGER if not found
        return VarType::INTEGER;
    }
    
    // Handle function calls using pre-computed return types
    if (auto* call = dynamic_cast<const FunctionCall*>(expr)) {
        if (auto* func_var = dynamic_cast<const VariableAccess*>(call->function_expr.get())) {
            // Handle built-in functions
            std::string func_name = func_var->name;
            if (func_name == "AS_INT") return VarType::INTEGER;
            if (func_name == "AS_FLOAT") return VarType::FLOAT;
            if (func_name == "AS_STRING") return VarType::POINTER_TO_STRING;
            if (func_name == "AS_LIST") return VarType::POINTER_TO_ANY_LIST;
            if (func_name == "LIST" || func_name == "COPYLIST" || func_name == "DEEPCOPYLIST") return VarType::POINTER_TO_ANY_LIST;
            if (func_name == "SPLIT") return VarType::POINTER_TO_STRING_LIST;
            if (func_name == "JOIN") return VarType::POINTER_TO_STRING;
            
            auto& return_types = analyzer_.get_function_return_types();
            auto it = return_types.find(func_var->name);
            if (it != return_types.end()) {
                return it->second;
            }
        }
        return VarType::INTEGER; // Default for unknown functions
    }
    
    // Handle binary operations with type promotion rules
    if (auto* bin_op = dynamic_cast<const BinaryOp*>(expr)) {
        // Comparison operations always return INTEGER (boolean 0/1)
        if (bin_op->op == BinaryOp::Operator::Equal ||
            bin_op->op == BinaryOp::Operator::NotEqual ||
            bin_op->op == BinaryOp::Operator::Less ||
            bin_op->op == BinaryOp::Operator::LessEqual ||
            bin_op->op == BinaryOp::Operator::Greater ||
            bin_op->op == BinaryOp::Operator::GreaterEqual ||
            bin_op->op == BinaryOp::Operator::Equivalence) {
            return VarType::INTEGER;
        }
        
        // For arithmetic operations: if either operand is float, result is float
        VarType left_type = infer_expression_type_local(bin_op->left.get());
        VarType right_type = infer_expression_type_local(bin_op->right.get());
        
        if (left_type == VarType::FLOAT || right_type == VarType::FLOAT) {
            return VarType::FLOAT;
        }
        return VarType::INTEGER;
    }
    
    // Handle unary operations
    if (auto* un_op = dynamic_cast<const UnaryOp*>(expr)) {
        if (un_op->op == UnaryOp::Operator::FloatConvert || 
            un_op->op == UnaryOp::Operator::FloatSqrt ||
            un_op->op == UnaryOp::Operator::FloatFloor ||
            un_op->op == UnaryOp::Operator::FloatTruncate ||
            un_op->op == UnaryOp::Operator::HeadOfAsFloat) {
            return VarType::FLOAT;
        }
        if (un_op->op == UnaryOp::Operator::IntegerConvert ||
            un_op->op == UnaryOp::Operator::LengthOf ||
            un_op->op == UnaryOp::Operator::HeadOf) {
            return VarType::INTEGER;
        }
        if (un_op->op == UnaryOp::Operator::AddressOf) {
            VarType base_type = infer_expression_type_local(un_op->operand.get());
            if (base_type == VarType::FLOAT) return VarType::POINTER_TO_FLOAT;
            if (base_type == VarType::INTEGER) return VarType::POINTER_TO_INT;
            return VarType::POINTER_TO_INT; // Default
        }
        if (un_op->op == UnaryOp::Operator::Indirection) {
            VarType ptr_type = infer_expression_type_local(un_op->operand.get());
            if (ptr_type == VarType::POINTER_TO_FLOAT) return VarType::FLOAT;
            if (ptr_type == VarType::POINTER_TO_INT) return VarType::INTEGER;
            return VarType::INTEGER; // Default
        }
        // For other unary ops, inherit the operand type
        return infer_expression_type_local(un_op->operand.get());
    }
    
    // Handle vector access
    if (auto* vec_access = dynamic_cast<const VectorAccess*>(expr)) {
        VarType vec_type = infer_expression_type_local(vec_access->vector_expr.get());
        if (vec_type == VarType::POINTER_TO_FLOAT_VEC) return VarType::FLOAT;
        return VarType::INTEGER; // Default for other vector types
    }
    
    // Handle allocation expressions
    if (dynamic_cast<const VecAllocationExpression*>(expr)) {
        return VarType::POINTER_TO_INT_VEC;
    }
    
    if (dynamic_cast<const FVecAllocationExpression*>(expr)) {
        return VarType::POINTER_TO_FLOAT_VEC;
    }
    
    if (dynamic_cast<const StringAllocationExpression*>(expr)) {
        return VarType::POINTER_TO_STRING;
    }
    
    // Handle list expressions
    if (auto* list_expr = dynamic_cast<const ListExpression*>(expr)) {
        if (list_expr->initializers.empty()) {
            return list_expr->is_manifest ? VarType::CONST_POINTER_TO_ANY_LIST : VarType::POINTER_TO_ANY_LIST;
        }
        // Infer from first element
        VarType first_type = infer_expression_type_local(list_expr->initializers[0].get());
        if (first_type == VarType::FLOAT) {
            return list_expr->is_manifest ? VarType::CONST_POINTER_TO_FLOAT_LIST : VarType::POINTER_TO_FLOAT_LIST;
        }
        if (first_type == VarType::INTEGER) {
            return list_expr->is_manifest ? VarType::CONST_POINTER_TO_INT_LIST : VarType::POINTER_TO_INT_LIST;
        }
        return list_expr->is_manifest ? VarType::CONST_POINTER_TO_ANY_LIST : VarType::POINTER_TO_ANY_LIST;
    }
    
    // Handle table expressions  
    if (auto* table = dynamic_cast<const TableExpression*>(expr)) {
        return table->is_float_table ? VarType::POINTER_TO_FLOAT_VEC : VarType::POINTER_TO_INT_VEC;
    }
    
    // For any other expression types, default to INTEGER
    return VarType::INTEGER;
}

// Helper method to get bounds error label for current function
std::string NewCodeGenerator::get_bounds_error_label_for_current_function() {
    std::string label = ".L_bounds_error_handler_" + current_function_name_;
    function_needs_bounds_error_handler_[current_function_name_] = true;
    return label;
}

// --- Manual spill/restore functions removed in Phase 4 ---
// Variables are now intelligently allocated by LinearScanAllocator:
// - Call-crossing variables get callee-saved registers (preserved automatically)
// - Local-only variables get caller-saved registers (don't cross calls)

// --- BitfieldAccessExpression codegen (read) ---
void NewCodeGenerator::visit(BitfieldAccessExpression& node) {
    debug_print("Visiting BitfieldAccessExpression node (Read).");

    auto* start_lit = dynamic_cast<NumberLiteral*>(node.start_bit_expr.get());
    auto* width_lit = dynamic_cast<NumberLiteral*>(node.width_expr.get());

    // Optimized Path: Use UBFX if start and width are constants.
    if (start_lit && width_lit) {
        generate_expression_code(*node.base_expr);
        std::string word_reg = expression_result_reg_;
        std::string dest_reg = register_manager_.acquire_scratch_reg(*this);

        emit(Encoder::opt_create_ubfx(dest_reg, word_reg,
                                      start_lit->int_value, width_lit->int_value));

        expression_result_reg_ = dest_reg;
        register_manager_.release_register(word_reg);

    } else {
        // Fallback Path: For variable start/width, use the manual shift-and-mask.
        generate_expression_code(*node.base_expr);
        std::string word_reg = expression_result_reg_;

        generate_expression_code(*node.start_bit_expr);
        std::string start_reg = expression_result_reg_;

        generate_expression_code(*node.width_expr);
        std::string width_reg = expression_result_reg_;

        std::string shifted_reg = register_manager_.acquire_scratch_reg(*this);
        std::string mask_reg = register_manager_.acquire_scratch_reg(*this);
        std::string one_reg = register_manager_.acquire_scratch_reg(*this);

        emit(Encoder::create_lsr_reg(shifted_reg, word_reg, start_reg));
        emit(Encoder::create_movz_imm(one_reg, 1));
        emit(Encoder::create_lsl_reg(mask_reg, one_reg, width_reg));
        emit(Encoder::create_sub_imm(mask_reg, mask_reg, 1));
        emit(Encoder::create_and_reg(shifted_reg, shifted_reg, mask_reg));

        expression_result_reg_ = shifted_reg;

        register_manager_.release_register(word_reg);
        register_manager_.release_register(start_reg);
        register_manager_.release_register(width_reg);
        register_manager_.release_register(mask_reg);
        register_manager_.release_register(one_reg);
    }
}

void NewCodeGenerator::visit(SuperMethodAccessExpression& node) {
    debug_print("Visiting SuperMethodAccessExpression for parent method: " + node.member_name);

    // 1. Get the '_this' pointer for the CURRENT object from its stack slot.
    std::string this_ptr_reg = get_variable_register("_this");

    // 2. Find the PARENT class entry in the ClassTable.
    const ClassTableEntry* class_entry = class_table_->get_class(current_class_name_);
    if (!class_entry || class_entry->parent_name.empty()) {
        throw std::runtime_error("SUPER call in class with no parent: " + current_class_name_);
    }
    const ClassTableEntry* parent_entry = class_table_->get_class(class_entry->parent_name);
    if (!parent_entry) {
        throw std::runtime_error("Parent class not found: " + class_entry->parent_name);
    }

    // 3. Find the method in the PARENT's vtable.
    ClassMethodInfo* method_info = parent_entry->findMethod(node.member_name, false);
    if (!method_info) {
        throw std::runtime_error("SUPER: Parent method '" + node.member_name + "' not found.");
    }
    // For SUPER calls, emit a direct call to the parent method (non-virtual).
    // The result of this expression is the mangled name of the parent method.
    // We set expression_result_reg_ to the parent's qualified name for use by the caller.
    expression_result_reg_ = method_info->qualified_name;
}

// Look up a symbol in the symbol table
void NewCodeGenerator::visit(BinaryOp& node) {
    debug_print("Visiting BinaryOp node.");

    // For LogicalAnd and LogicalOr operations, implement short-circuit logic
    if (node.op == BinaryOp::Operator::LogicalAnd) {
        debug_print("Generating short-circuit code for LogicalAnd");
        generate_short_circuit_and(node);
        return;
    } else if (node.op == BinaryOp::Operator::LogicalOr) {
        debug_print("Generating short-circuit code for LogicalOr");
        generate_short_circuit_or(node);
        return;
    }

    // --- START NEW LOGIC FOR MIXED-TYPES ---
    // First, recursively determine the type of each sub-expression.
    VarType left_type = infer_expression_type_local(node.left.get());
    VarType right_type = infer_expression_type_local(node.right.get());
    
    debug_print("BinaryOp type inference: left_type=" + std::to_string(static_cast<int>(left_type)) + 
                ", right_type=" + std::to_string(static_cast<int>(right_type)));

    // 1. Evaluate the LEFT side of the expression first.
    debug_print("About to evaluate LEFT side of binary operation");
    generate_expression_code(*node.left);
    std::string left_reg = expression_result_reg_;
    debug_print("LEFT side evaluation complete. left_reg = '" + left_reg + "'");
    
    if (left_reg.empty()) {
        debug_print("ERROR: LEFT side evaluation returned empty register!");
        throw std::runtime_error("LEFT side of binary operation resulted in empty register");
    }

    // 2. If the right side is a function call, preserve the left result.
    bool right_is_function_call = node.right && node.right->getType() == ASTNode::NodeType::FunctionCallExpr;
    // If the right side is a function call, use a pre-reserved temp register.
    // The register manager should provide the next available temp register.
    if (right_is_function_call) {
        std::string temp_reg = register_manager_.get_next_available_temp_reg();
        emit(Encoder::create_mov_reg(temp_reg, left_reg));
        register_manager_.release_register(left_reg);
        left_reg = temp_reg;
    }

    // 3. OPTIMIZATION: Check if right operand is a constant for immediate value optimization
    bool right_is_constant = false;
    int64_t constant_value = 0;
    std::string right_reg;
    
    if (auto* number_lit = dynamic_cast<NumberLiteral*>(node.right.get())) {
        if (number_lit->literal_type == NumberLiteral::LiteralType::Integer) {
            right_is_constant = true;
            constant_value = number_lit->int_value;
            debug_print("OPTIMIZATION: Right operand is constant: " + std::to_string(constant_value));
        }
    }
    
    if (!right_is_constant) {
        // Evaluate the RIGHT side normally
        debug_print("About to evaluate RIGHT side of binary operation");
        generate_expression_code(*node.right);
        right_reg = expression_result_reg_;
        debug_print("RIGHT side evaluation complete. right_reg = '" + right_reg + "'");
        
        if (right_reg.empty()) {
            debug_print("ERROR: RIGHT side evaluation returned empty register!");
            throw std::runtime_error("RIGHT side of binary operation resulted in empty register");
        }
    }

    // 4. If we pushed the left operand, restore it.
    // No pop is needed; value is already in a callee-saved register if function call was on the right.

    // Check if a type promotion is needed (only for non-constant operands).
    debug_print("Checking type promotion: left_reg=" + left_reg + ", right_reg=" + right_reg);
    if (!right_is_constant && left_type == VarType::FLOAT && right_type == VarType::INTEGER) {
        debug_print("PROMOTING RIGHT: Converting integer in " + right_reg + " to float");
        // Promote right operand from Integer (in an X register) to Float (in a D register).
        std::string fp_reg = register_manager_.acquire_fp_scratch_reg();
        emit(Encoder::create_scvtf_reg(fp_reg, right_reg)); // SCVTF instruction
        register_manager_.release_register(right_reg);
        right_reg = fp_reg; // The right operand is now in a float register.
        debug_print("RIGHT PROMOTED: now in " + right_reg);
    } else if (!right_is_constant && left_type == VarType::INTEGER && right_type == VarType::FLOAT) {
        debug_print("PROMOTING LEFT: Converting integer in " + left_reg + " to float");
        // Promote left operand.
        std::string fp_reg = register_manager_.acquire_fp_scratch_reg();
        emit(Encoder::create_scvtf_reg(fp_reg, left_reg));
        register_manager_.release_register(left_reg);
        left_reg = fp_reg; // The left operand is now in a float register.
        debug_print("LEFT PROMOTED: now in " + left_reg);
    } else {
        debug_print("NO PROMOTION NEEDED or both same type or constant operand");
    }
    // --- END NEW LOGIC ---

    bool is_float_op = (left_type == VarType::FLOAT || right_type == VarType::FLOAT);
    bool is_pair_op = (left_type == VarType::PAIR && right_type == VarType::PAIR);
    bool is_fpair_op = (left_type == VarType::FPAIR && right_type == VarType::FPAIR);
    
    // Scalar-PAIR operations: PAIR + scalar or scalar + PAIR
    bool is_scalar_pair_op = ((left_type == VarType::PAIR && right_type == VarType::INTEGER) ||
                              (left_type == VarType::INTEGER && right_type == VarType::PAIR) ||
                              (left_type == VarType::PAIR && right_type == VarType::FLOAT) ||
                              (left_type == VarType::FLOAT && right_type == VarType::PAIR));
    
    // Scalar-FPAIR operations: FPAIR + scalar or scalar + FPAIR
    bool is_scalar_fpair_op = ((left_type == VarType::FPAIR && right_type == VarType::FLOAT) ||
                               (left_type == VarType::FLOAT && right_type == VarType::FPAIR) ||
                               (left_type == VarType::FPAIR && right_type == VarType::INTEGER) ||
                               (left_type == VarType::INTEGER && right_type == VarType::FPAIR));

    // ====================== START OF FIX ======================
    // Check if left_reg is a variable's home register from the LinearScanAllocator.
    // If so, we must copy it to avoid corruption during destructive operations.
    bool left_is_variable_home = false;
    if (auto* var_access = dynamic_cast<VariableAccess*>(node.left.get())) {
        // Query the correct allocator results, not the old RegisterManager
        auto func_alloc_it = all_allocations_.find(current_function_name_);
        if (func_alloc_it != all_allocations_.end()) {
            const auto& current_function_allocs = func_alloc_it->second;
            auto var_alloc_it = current_function_allocs.find(var_access->name);
            if (var_alloc_it != current_function_allocs.end()) {
                const LiveInterval& allocation = var_alloc_it->second;
                if (!allocation.is_spilled && allocation.assigned_register == left_reg) {
                    left_is_variable_home = true;
                }
            }
        }
    }

    // If left_reg is a variable's home register, copy it to a scratch register first
    if (left_is_variable_home) {
        std::string original_home_reg = left_reg; // Store original register before reassignment
        std::string scratch_reg;
        if (is_float_op) {
            scratch_reg = register_manager_.acquire_fp_scratch_reg();
            emit(Encoder::create_fmov_reg(scratch_reg, left_reg));
        } else {
            scratch_reg = register_manager_.acquire_scratch_reg(*this);
            emit(Encoder::create_mov_reg(scratch_reg, left_reg));
        }
        left_reg = scratch_reg; // Use the copy for destructive operations
        debug_print("Protected variable home register by copying to scratch register.");
    }
    // ======================= END OF FIX =======================

    // Handle FPAIR arithmetic operations using NEON SIMD instructions (floating point)
    if (is_fpair_op) {
        debug_print("Generating FPAIR arithmetic using NEON SIMD floating-point instructions");
        
        // Check if we support this operation for FPAIRs
        if (node.op != BinaryOp::Operator::Add && 
            node.op != BinaryOp::Operator::Subtract && 
            node.op != BinaryOp::Operator::Multiply &&
            node.op != BinaryOp::Operator::Divide) {
            throw std::runtime_error("Unsupported binary operation on FPAIR types: only +, -, *, / are supported");
        }
        
        // NEON SIMD approach for optimal floating-point performance:
        // 1. Move FPAIR values from X registers to D registers (64-bit NEON registers)
        // 2. Use NEON .2S (2 x 32-bit float lanes) instructions for component-wise arithmetic
        // 3. Move result back to X register
        
        // Move left FPAIR (X register) to NEON D register
        std::string left_neon_reg = register_manager_.acquire_fp_scratch_reg();
        emit(Encoder::create_fmov_x_to_d(left_neon_reg, left_reg));
        debug_print("Moved left FPAIR from " + left_reg + " to NEON register " + left_neon_reg);
        
        // Move right FPAIR (X register) to NEON D register  
        std::string right_neon_reg = register_manager_.acquire_fp_scratch_reg();
        emit(Encoder::create_fmov_x_to_d(right_neon_reg, right_reg));
        debug_print("Moved right FPAIR from " + right_reg + " to NEON register " + right_neon_reg);
        
        // Convert D register names to V register names for vector operations
        std::string left_vector_reg = "V" + left_neon_reg.substr(1);
        std::string right_vector_reg = "V" + right_neon_reg.substr(1);
        
        // Perform component-wise floating-point arithmetic using NEON vector instructions
        switch (node.op) {
            case BinaryOp::Operator::Add:
                emit(Encoder::create_fadd_vector_reg(left_vector_reg, left_vector_reg, right_vector_reg, "2S"));
                debug_print("FPAIR addition: " + left_vector_reg + ".2S = " + left_vector_reg + ".2S + " + right_vector_reg + ".2S");
                break;
            case BinaryOp::Operator::Subtract:
                emit(Encoder::enc_create_fsub_vector_reg(left_vector_reg, left_vector_reg, right_vector_reg, "2S"));
                debug_print("FPAIR subtraction: " + left_vector_reg + ".2S = " + left_vector_reg + ".2S - " + right_vector_reg + ".2S");
                break;
            case BinaryOp::Operator::Multiply:
                emit(Encoder::create_fmul_vector_reg(left_vector_reg, left_vector_reg, right_vector_reg, "2S"));
                debug_print("FPAIR multiplication: " + left_vector_reg + ".2S = " + left_vector_reg + ".2S * " + right_vector_reg + ".2S");
                break;
            case BinaryOp::Operator::Divide:
                emit(Encoder::enc_create_fdiv_vector_reg(left_vector_reg, left_vector_reg, right_vector_reg, "2S"));
                debug_print("FPAIR division: " + left_vector_reg + ".2S = " + left_vector_reg + ".2S / " + right_vector_reg + ".2S");
                break;
            default:
                throw std::runtime_error("Unsupported FPAIR operation");
        }
        
        // Move result back from NEON D register to X register
        emit(Encoder::create_fmov_d_to_x(left_reg, left_neon_reg));
        debug_print("Moved FPAIR result from NEON register " + left_neon_reg + " back to " + left_reg);
        
        // Release NEON scratch registers
        register_manager_.release_register(left_neon_reg);
        register_manager_.release_register(right_neon_reg);
        if (!right_is_constant) {
            register_manager_.release_register(right_reg);
        }
        
        expression_result_reg_ = left_reg;
        debug_print("FPAIR SIMD arithmetic complete. Result in " + expression_result_reg_);
        return;
    }

    // Handle scalar-FPAIR arithmetic operations using NEON SIMD instructions
    if (is_scalar_fpair_op) {
        debug_print("Generating scalar-FPAIR arithmetic using NEON SIMD with DUP instruction");
        
        // Check if we support this operation for scalar-FPAIR
        if (node.op != BinaryOp::Operator::Add && 
            node.op != BinaryOp::Operator::Subtract && 
            node.op != BinaryOp::Operator::Multiply &&
            node.op != BinaryOp::Operator::Divide) {
            throw std::runtime_error("Unsupported scalar-FPAIR operation: only +, -, *, / are supported");
        }
        
        // Determine which operand is the FPAIR and which is the scalar
        bool fpair_is_left = (left_type == VarType::FPAIR);
        std::string fpair_reg = fpair_is_left ? left_reg : right_reg;
        std::string scalar_reg;
        VarType scalar_type = fpair_is_left ? right_type : left_type;
        
        // Handle scalar operand (might be constant or register)
        if (fpair_is_left && right_is_constant) {
            // FPAIR op constant: load constant into register
            scalar_reg = register_manager_.get_free_register(*this);
            emit(Encoder::create_movz_imm(scalar_reg, static_cast<uint64_t>(constant_value)));
            debug_print("Loaded constant " + std::to_string(constant_value) + " into " + scalar_reg);
        } else if (!fpair_is_left && right_is_constant) {
            // This shouldn't happen (constant op FPAIR), but handle it
            throw std::runtime_error("Constant-FPAIR operations not supported (non-commutative)");
        } else {
            // Normal register operand
            scalar_reg = fpair_is_left ? right_reg : left_reg;
        }
        
        // Convert scalar to float if it's an integer
        if (scalar_type == VarType::INTEGER) {
            std::string float_reg = register_manager_.acquire_fp_scratch_reg();
            emit(Encoder::create_scvtf_reg(float_reg, scalar_reg));
            register_manager_.release_register(scalar_reg);
            scalar_reg = float_reg;
            debug_print("Converted integer scalar to float: " + scalar_reg);
        }
        
        // Move FPAIR to NEON D register
        std::string fpair_neon_reg = register_manager_.acquire_fp_scratch_reg();
        emit(Encoder::create_fmov_x_to_d(fpair_neon_reg, fpair_reg));
        debug_print("Moved FPAIR from " + fpair_reg + " to NEON register " + fpair_neon_reg);
        
        // Duplicate scalar across NEON vector lanes using DUP
        std::string scalar_vector_reg = register_manager_.acquire_fp_scratch_reg();
        std::string scalar_vector_name = "V" + scalar_vector_reg.substr(1);
        
        // Convert scalar register to appropriate format for DUP
        std::string scalar_for_dup;
        if (scalar_reg[0] == 'D') {
            // If scalar is in D register, move to general register for DUP
            std::string temp_gen_reg = register_manager_.get_free_register(*this);
            emit(Encoder::create_fmov_d_to_x(temp_gen_reg, scalar_reg));
            scalar_for_dup = temp_gen_reg;
            debug_print("Moved float scalar from " + scalar_reg + " to general register " + scalar_for_dup + " for DUP");
        } else {
            // Scalar is already in general register
            scalar_for_dup = scalar_reg;
        }
        
        // Convert X register to W register for 32-bit DUP operation
        if (scalar_for_dup[0] == 'X') {
            scalar_for_dup[0] = 'W';
            debug_print("Converted X register to W register for DUP: " + scalar_for_dup);
        }
        
        emit(Encoder::enc_create_dup_scalar(scalar_vector_name, scalar_for_dup, "2S"));
        debug_print("Duplicated scalar " + scalar_for_dup + " to vector " + scalar_vector_name + ".2S");
        
        // Convert register names to V registers for vector operations
        std::string fpair_vector_reg = "V" + fpair_neon_reg.substr(1);
        
        // Perform component-wise floating-point arithmetic
        // Handle non-commutative operations (subtraction, division) properly
        if (!fpair_is_left && (node.op == BinaryOp::Operator::Subtract || node.op == BinaryOp::Operator::Divide)) {
            // scalar - fpair or scalar / fpair: need to swap operands in the vector operation
            switch (node.op) {
                case BinaryOp::Operator::Subtract:
                    emit(Encoder::enc_create_fsub_vector_reg(scalar_vector_name, scalar_vector_name, fpair_vector_reg, "2S"));
                    debug_print("Scalar-FPAIR subtraction: " + scalar_vector_name + ".2S - " + fpair_vector_reg + ".2S");
                    fpair_vector_reg = scalar_vector_name; // Result is in scalar vector register
                    break;
                case BinaryOp::Operator::Divide:
                    emit(Encoder::enc_create_fdiv_vector_reg(scalar_vector_name, scalar_vector_name, fpair_vector_reg, "2S"));
                    debug_print("Scalar-FPAIR division: " + scalar_vector_name + ".2S / " + fpair_vector_reg + ".2S");
                    fpair_vector_reg = scalar_vector_name; // Result is in scalar vector register
                    break;
                default:
                    throw std::runtime_error("Unsupported operation for scalar-FPAIR (swapped operands)");
                    break;
            }
        } else {
            // fpair op scalar or commutative operations (addition, multiplication)
            switch (node.op) {
                case BinaryOp::Operator::Add:
                    emit(Encoder::create_fadd_vector_reg(fpair_vector_reg, fpair_vector_reg, scalar_vector_name, "2S"));
                    debug_print("Scalar-FPAIR addition: " + fpair_vector_reg + ".2S + " + scalar_vector_name + ".2S");
                    break;
                case BinaryOp::Operator::Subtract:
                    emit(Encoder::enc_create_fsub_vector_reg(fpair_vector_reg, fpair_vector_reg, scalar_vector_name, "2S"));
                    debug_print("Scalar-FPAIR subtraction: " + fpair_vector_reg + ".2S - " + scalar_vector_name + ".2S");
                    break;
                case BinaryOp::Operator::Multiply:
                    emit(Encoder::create_fmul_vector_reg(fpair_vector_reg, fpair_vector_reg, scalar_vector_name, "2S"));
                    debug_print("Scalar-FPAIR multiplication: " + fpair_vector_reg + ".2S * " + scalar_vector_name + ".2S");
                    break;
                case BinaryOp::Operator::Divide:
                    emit(Encoder::enc_create_fdiv_vector_reg(fpair_vector_reg, fpair_vector_reg, scalar_vector_name, "2S"));
                    debug_print("Scalar-FPAIR division: " + fpair_vector_reg + ".2S / " + scalar_vector_name + ".2S");
                    break;
                default:
                    throw std::runtime_error("Unsupported operation for FPAIR-scalar operations");
                    break;
            }
        }
        
        // Move result back to general register
        std::string result_neon_reg = fpair_vector_reg.substr(1); // Remove 'V' prefix for 'D' register
        emit(Encoder::create_fmov_d_to_x(left_reg, result_neon_reg));
        debug_print("Moved scalar-FPAIR result back to " + left_reg);
        
        // Release NEON scratch registers
        register_manager_.release_register(fpair_neon_reg);
        register_manager_.release_register(scalar_vector_reg);
        if (scalar_type == VarType::INTEGER) {
            register_manager_.release_register(scalar_reg); // Release converted float register
        }
        if (!right_is_constant) {
            register_manager_.release_register(right_reg);
        }
        
        expression_result_reg_ = left_reg;
        debug_print("Scalar-FPAIR SIMD arithmetic complete. Result in " + expression_result_reg_);
        return;
    }

    // Handle scalar-PAIR arithmetic operations using NEON SIMD instructions
    if (is_scalar_pair_op) {
        debug_print("Generating scalar-PAIR arithmetic using NEON SIMD with DUP instruction");
        
        // Check if we support this operation for scalar-PAIR
        if (node.op != BinaryOp::Operator::Add && 
            node.op != BinaryOp::Operator::Subtract && 
            node.op != BinaryOp::Operator::Multiply &&
            node.op != BinaryOp::Operator::Divide) {
            throw std::runtime_error("Unsupported scalar-PAIR operation: only +, -, *, / are supported");
        }
        
        // Determine which operand is the PAIR and which is the scalar
        bool pair_is_left = (left_type == VarType::PAIR);
        std::string pair_reg = pair_is_left ? left_reg : right_reg;
        std::string scalar_reg;
        VarType scalar_type = pair_is_left ? right_type : left_type;
        
        // Handle scalar operand (might be constant or register)
        if (pair_is_left && right_is_constant) {
            // PAIR op constant: load constant into register
            scalar_reg = register_manager_.get_free_register(*this);
            emit(Encoder::create_movz_imm(scalar_reg, static_cast<uint64_t>(constant_value)));
            debug_print("Loaded constant " + std::to_string(constant_value) + " into " + scalar_reg);
        } else if (!pair_is_left && right_is_constant) {
            // This shouldn't happen (constant op PAIR), but handle it
            throw std::runtime_error("Constant-PAIR operations not supported (non-commutative)");
        } else {
            // Normal register operand
            scalar_reg = pair_is_left ? right_reg : left_reg;
        }
        
        // Convert scalar to appropriate type if needed
        if (scalar_type == VarType::FLOAT) {
            // For PAIR + FLOAT, we need to convert float to integer for integer vector operations
            // Or we could promote the result to FPAIR - let's promote to FPAIR for precision
            debug_print("Converting PAIR+FLOAT operation to FPAIR operation for precision");
            // This case should be handled by type promotion in the type inference
            // For now, throw an error to indicate this needs special handling
            throw std::runtime_error("PAIR+FLOAT operations need special type promotion handling");
        }
        
        // Move PAIR to NEON D register
        std::string pair_neon_reg = register_manager_.acquire_fp_scratch_reg();
        emit(Encoder::create_fmov_x_to_d(pair_neon_reg, pair_reg));
        debug_print("Moved PAIR from " + pair_reg + " to NEON register " + pair_neon_reg);
        
        // Duplicate scalar across NEON vector lanes using DUP
        std::string scalar_vector_reg = register_manager_.acquire_fp_scratch_reg();
        std::string scalar_vector_name = "V" + scalar_vector_reg.substr(1);
        
        // Convert X register to W register for 32-bit DUP operation
        std::string scalar_for_dup = scalar_reg;
        if (scalar_for_dup[0] == 'X') {
            scalar_for_dup[0] = 'W';
            debug_print("Converted X register to W register for DUP: " + scalar_for_dup);
        }
        
        emit(Encoder::enc_create_dup_scalar(scalar_vector_name, scalar_for_dup, "2S"));
        debug_print("Duplicated scalar " + scalar_reg + " to vector " + scalar_vector_name + ".2S");
        
        // Convert register names to V registers for vector operations
        std::string pair_vector_reg = "V" + pair_neon_reg.substr(1);
        
        // Perform component-wise integer arithmetic
        // Handle non-commutative operations (subtraction, division) properly
        if (!pair_is_left && (node.op == BinaryOp::Operator::Subtract || node.op == BinaryOp::Operator::Divide)) {
            // scalar - pair or scalar / pair: need to swap operands in the vector operation
            switch (node.op) {
                case BinaryOp::Operator::Subtract:
                    emit(Encoder::create_sub_vector_reg(scalar_vector_name, scalar_vector_name, pair_vector_reg, "2S"));
                    debug_print("Scalar-PAIR subtraction: " + scalar_vector_name + ".2S - " + pair_vector_reg + ".2S");
                    pair_vector_reg = scalar_vector_name; // Result is in scalar vector register
                    break;
                case BinaryOp::Operator::Divide:
                    // Integer division is not directly supported by NEON, need to use scalar division
                    throw std::runtime_error("Integer division in scalar-PAIR operations not yet implemented");
                    break;
                default:
                    throw std::runtime_error("Unsupported operation for scalar-PAIR (swapped operands)");
                    break;
            }
        } else {
            // pair op scalar or commutative operations (addition, multiplication)
            switch (node.op) {
                case BinaryOp::Operator::Add:
                    emit(Encoder::create_add_vector_reg(pair_vector_reg, pair_vector_reg, scalar_vector_name, "2S"));
                    debug_print("Scalar-PAIR addition: " + pair_vector_reg + ".2S + " + scalar_vector_name + ".2S");
                    break;
                case BinaryOp::Operator::Subtract:
                    emit(Encoder::create_sub_vector_reg(pair_vector_reg, pair_vector_reg, scalar_vector_name, "2S"));
                    debug_print("Scalar-PAIR subtraction: " + pair_vector_reg + ".2S - " + scalar_vector_name + ".2S");
                    break;
                case BinaryOp::Operator::Multiply:
                    emit(Encoder::create_mul_vector_reg(pair_vector_reg, pair_vector_reg, scalar_vector_name, "2S"));
                    debug_print("Scalar-PAIR multiplication: " + pair_vector_reg + ".2S * " + scalar_vector_name + ".2S");
                    break;
                case BinaryOp::Operator::Divide:
                    // Integer division is not directly supported by NEON
                    throw std::runtime_error("Integer division in scalar-PAIR operations not yet implemented");
                    break;
                default:
                    throw std::runtime_error("Unsupported operation for PAIR-scalar operations");
                    break;
            }
        }
        
        // Move result back to general register
        emit(Encoder::create_fmov_d_to_x(left_reg, pair_neon_reg));
        debug_print("Moved PAIR result from NEON register " + pair_neon_reg + " back to " + left_reg);
        debug_print("Moved scalar-PAIR result back to " + left_reg);
        
        // Release NEON scratch registers
        register_manager_.release_register(pair_neon_reg);
        register_manager_.release_register(scalar_vector_reg);
        if (!right_is_constant) {
            register_manager_.release_register(right_reg);
        }
        
        expression_result_reg_ = left_reg;
        debug_print("Scalar-PAIR SIMD arithmetic complete. Result in " + expression_result_reg_);
        return;
    }

    // Handle PAIR arithmetic operations using NEON SIMD instructions
    // Skip this section for comparison operations
    if (is_pair_op && node.op != BinaryOp::Operator::Equal && node.op != BinaryOp::Operator::NotEqual) {
        debug_print("Generating PAIR arithmetic using NEON SIMD instructions");
        
        // Check if we support this operation for PAIRs
        if (node.op != BinaryOp::Operator::Add && 
            node.op != BinaryOp::Operator::Subtract && 
            node.op != BinaryOp::Operator::Multiply) {
            throw std::runtime_error("Unsupported binary operation on PAIR types: only +, -, *, ==, ~= are supported (division not supported for integer PAIRs)");
        }
        
        // NEON SIMD approach for optimal performance:
        // 1. Move PAIR values from X registers to D registers (64-bit NEON registers)
        // 2. Use NEON .2S (2 x 32-bit signed integer lanes) instructions for component-wise arithmetic
        // 3. Move result back to X register
        
        // Move left PAIR (X register) to NEON D register
        std::string left_neon_reg = register_manager_.acquire_fp_scratch_reg();
        emit(Encoder::create_fmov_x_to_d(left_neon_reg, left_reg));
        debug_print("Moved left PAIR from " + left_reg + " to NEON register " + left_neon_reg);
        
        // Move right PAIR (X register) to NEON D register
        std::string right_neon_reg = register_manager_.acquire_fp_scratch_reg();
        emit(Encoder::create_fmov_x_to_d(right_neon_reg, right_reg));
        debug_print("Moved right PAIR from " + right_reg + " to NEON register " + right_neon_reg);
        
        // Convert D register names to V register names for vector operations
        std::string left_vector_reg = left_neon_reg;
        std::string right_vector_reg = right_neon_reg;
        if (left_neon_reg[0] == 'D') {
            left_vector_reg = "V" + left_neon_reg.substr(1);
        }
        if (right_neon_reg[0] == 'D') {
            right_vector_reg = "V" + right_neon_reg.substr(1);
        }
        
        // Perform component-wise arithmetic using NEON .2S arrangement (two 32-bit signed integers)
        switch (node.op) {
            case BinaryOp::Operator::Add:
                emit(Encoder::create_add_vector_reg(left_vector_reg, left_vector_reg, right_vector_reg, "2S"));
                debug_print("PAIR addition: " + left_vector_reg + ".2S = " + left_vector_reg + ".2S + " + right_vector_reg + ".2S");
                break;
            case BinaryOp::Operator::Subtract:
                emit(Encoder::create_sub_vector_reg(left_vector_reg, left_vector_reg, right_vector_reg, "2S"));
                debug_print("PAIR subtraction: " + left_vector_reg + ".2S = " + left_vector_reg + ".2S - " + right_vector_reg + ".2S");
                break;
            case BinaryOp::Operator::Multiply:
                emit(Encoder::create_mul_vector_reg(left_vector_reg, left_vector_reg, right_vector_reg, "2S"));
                debug_print("PAIR multiplication: " + left_vector_reg + ".2S = " + left_vector_reg + ".2S * " + right_vector_reg + ".2S");
                break;
            default:
                throw std::runtime_error("Unsupported PAIR operation");
        }
        
        // Move result back from NEON D register to X register
        emit(Encoder::create_fmov_d_to_x(left_reg, left_neon_reg));
        debug_print("Moved PAIR result from NEON register " + left_neon_reg + " back to " + left_reg);
        
        // Release NEON scratch registers
        register_manager_.release_register(left_neon_reg);
        register_manager_.release_register(right_neon_reg);
        if (!right_is_constant) {
            register_manager_.release_register(right_reg);
        }
        
        expression_result_reg_ = left_reg;
        debug_print("PAIR SIMD arithmetic complete. Result in " + expression_result_reg_);
        return;
    }

    // Handle different binary operators
    switch (node.op) {
        case BinaryOp::Operator::Add:
            if (is_float_op) {
                emit(Encoder::create_fadd_reg(left_reg, left_reg, right_reg));
            } else {
                // OPTIMIZATION: Use immediate ADD if right operand is constant
                if (right_is_constant) {
                    // Check if the constant fits into a 12-bit immediate for ADD
                    if (constant_value >= 0 && constant_value <= 4095) {
                        // Small Constant Path: Use efficient ADD immediate instruction
                        emit(Encoder::create_add_imm(left_reg, left_reg, static_cast<int>(constant_value)));
                        debug_print("OPTIMIZATION: Generated ADD with immediate: " + std::to_string(constant_value));
                    } else {
                        // Large Constant Path: Load constant into temp register first
                        debug_print("OPTIMIZATION: ADD value > 4095. Loading into temp register for addition.");
                        std::string temp_reg = register_manager_.acquire_scratch_reg(*this);
                        emit(Encoder::create_movz_movk_abs64(temp_reg, constant_value, ""));
                        emit(Encoder::create_add_reg(left_reg, left_reg, temp_reg));
                        register_manager_.release_register(temp_reg);
                        debug_print("OPTIMIZATION: Generated ADD with temp register for large constant: " + std::to_string(constant_value));
                    }
                } else {
                    emit(Encoder::create_add_reg(left_reg, left_reg, right_reg));
                }
            }
            break;
        case BinaryOp::Operator::Subtract:
            if (is_float_op) {
                emit(Encoder::create_fsub_reg(left_reg, left_reg, right_reg));
            } else {
                // OPTIMIZATION: Use immediate SUB if right operand is constant
                if (right_is_constant) {
                    // Check if the constant fits into a 12-bit immediate for SUB
                    if (constant_value >= 0 && constant_value <= 4095) {
                        // Small Constant Path: Use efficient SUB immediate instruction
                        emit(Encoder::create_sub_imm(left_reg, left_reg, static_cast<int>(constant_value)));
                        debug_print("OPTIMIZATION: Generated SUB with immediate: " + std::to_string(constant_value));
                    } else {
                        // Large Constant Path: Load constant into temp register first
                        debug_print("OPTIMIZATION: SUB value > 4095. Loading into temp register for subtraction.");
                        std::string temp_reg = register_manager_.acquire_scratch_reg(*this);
                        emit(Encoder::create_movz_movk_abs64(temp_reg, constant_value, ""));
                        emit(Encoder::create_sub_reg(left_reg, left_reg, temp_reg));
                        register_manager_.release_register(temp_reg);
                        debug_print("OPTIMIZATION: Generated SUB with temp register for large constant: " + std::to_string(constant_value));
                    }
                } else {
                    emit(Encoder::create_sub_reg(left_reg, left_reg, right_reg));
                }
            }
            break;
        case BinaryOp::Operator::Multiply:
            if (is_float_op) {
                if (right_is_constant) {
                    std::string temp_reg = register_manager_.acquire_fp_scratch_reg();
                    emit(Encoder::create_fmov_d_to_x(temp_reg, std::to_string(constant_value)));
                    emit(Encoder::create_fmul_reg(left_reg, left_reg, temp_reg));
                    register_manager_.release_register(temp_reg);
                } else {
                    emit(Encoder::create_fmul_reg(left_reg, left_reg, right_reg));
                }
            } else {
                if (right_is_constant) {
                    std::string temp_reg = register_manager_.acquire_scratch_reg(*this);
                    emit(Encoder::create_movz_movk_abs64(temp_reg, constant_value, ""));
                    emit(Encoder::create_mul_reg(left_reg, left_reg, temp_reg));
                    register_manager_.release_register(temp_reg);
                } else {
                    emit(Encoder::create_mul_reg(left_reg, left_reg, right_reg));
                }
            }
            break;
        case BinaryOp::Operator::Divide:
            if (is_float_op) {
                if (right_is_constant) {
                    std::string temp_reg = register_manager_.acquire_fp_scratch_reg();
                    emit(Encoder::create_fmov_d_to_x(temp_reg, std::to_string(constant_value)));
                    emit(Encoder::create_fdiv_reg(left_reg, left_reg, temp_reg));
                    register_manager_.release_register(temp_reg);
                } else {
                    emit(Encoder::create_fdiv_reg(left_reg, left_reg, right_reg));
                }
            } else {
                if (right_is_constant) {
                    std::string temp_reg = register_manager_.acquire_scratch_reg(*this);
                    emit(Encoder::create_movz_movk_abs64(temp_reg, constant_value, ""));
                    emit(Encoder::create_sdiv_reg(left_reg, left_reg, temp_reg));
                    register_manager_.release_register(temp_reg);
                } else {
                    emit(Encoder::create_sdiv_reg(left_reg, left_reg, right_reg));
                }
            }
            break;
        case BinaryOp::Operator::Remainder:
            // Only valid for integer types
            if (right_is_constant) {
                std::string temp_reg = register_manager_.acquire_scratch_reg(*this);
                emit(Encoder::create_movz_movk_abs64(temp_reg, constant_value, ""));
                emit(Encoder::create_sdiv_reg("X16", left_reg, temp_reg)); // Temporary register X16
                emit(Encoder::create_mul_reg("X16", "X16", temp_reg));
                register_manager_.release_register(temp_reg);
            } else {
                emit(Encoder::create_sdiv_reg("X16", left_reg, right_reg)); // Temporary register X16
                emit(Encoder::create_mul_reg("X16", "X16", right_reg));
            }
            emit(Encoder::create_sub_reg(left_reg, left_reg, "X16"));
            break;
        case BinaryOp::Operator::Equal:
            {
                std::string dest_reg = register_manager_.acquire_scratch_reg(*this);
                
                // Optimized PAIR comparison: treat as 64-bit integer
                if (is_pair_op) {
                    // For integer PAIRs, we can do a single 64-bit comparison
                    // This is safe because both 32-bit components are laid out contiguously
                    if (right_is_constant) {
                        emit(Encoder::create_cmp_imm(left_reg, static_cast<int>(constant_value)));
                    } else {
                        emit(Encoder::create_cmp_reg(left_reg, right_reg));
                        register_manager_.release_register(right_reg);
                    }
                    emit(Encoder::create_cset(dest_reg, "EQ"));
                    register_manager_.release_register(left_reg);
                    expression_result_reg_ = dest_reg;
                }
                else if (is_float_op) {
                    if (right_is_constant) {
                        // Load constant into temp register for float comparison
                        std::string temp_reg = register_manager_.acquire_fp_scratch_reg();
                        emit(Encoder::create_fmov_d_to_x(temp_reg, std::to_string(constant_value)));
                        emit(Encoder::create_fcmp_reg(left_reg, temp_reg));
                        register_manager_.release_register(temp_reg);
                    } else {
                        emit(Encoder::create_fcmp_reg(left_reg, right_reg));
                        register_manager_.release_register(right_reg);
                    }
                    emit(Encoder::create_cset(dest_reg, "EQ"));
                } else {
                    if (right_is_constant) {
                        emit(Encoder::create_cmp_imm(left_reg, static_cast<int>(constant_value)));
                    } else {
                        emit(Encoder::create_cmp_reg(left_reg, right_reg));
                        register_manager_.release_register(right_reg);
                    }
                    emit(Encoder::create_cset(dest_reg, "EQ"));
                }
                register_manager_.release_register(left_reg);
                expression_result_reg_ = dest_reg;
            }
            break;
        case BinaryOp::Operator::NotEqual:
            {
                std::string dest_reg = register_manager_.acquire_scratch_reg(*this);
                
                // Optimized PAIR comparison: treat as 64-bit integer
                if (is_pair_op) {
                    // For integer PAIRs, we can do a single 64-bit comparison
                    if (right_is_constant) {
                        emit(Encoder::create_cmp_imm(left_reg, static_cast<int>(constant_value)));
                    } else {
                        emit(Encoder::create_cmp_reg(left_reg, right_reg));
                        register_manager_.release_register(right_reg);
                    }
                    emit(Encoder::create_cset(dest_reg, "NE"));
                    register_manager_.release_register(left_reg);
                    expression_result_reg_ = dest_reg;
                }
                else if (is_float_op) {
                    if (right_is_constant) {
                        // Load constant into temp register for float comparison
                        std::string temp_reg = register_manager_.acquire_fp_scratch_reg();
                        emit(Encoder::create_fmov_d_to_x(temp_reg, std::to_string(constant_value)));
                        emit(Encoder::create_fcmp_reg(left_reg, temp_reg));
                        register_manager_.release_register(temp_reg);
                    } else {
                        emit(Encoder::create_fcmp_reg(left_reg, right_reg));
                        register_manager_.release_register(right_reg);
                    }
                    emit(Encoder::create_cset(dest_reg, "NE"));
                } else {
                    if (right_is_constant) {
                        emit(Encoder::create_cmp_imm(left_reg, static_cast<int>(constant_value)));
                    } else {
                        emit(Encoder::create_cmp_reg(left_reg, right_reg));
                        register_manager_.release_register(right_reg);
                    }
                    emit(Encoder::create_cset(dest_reg, "NE"));
                }
                register_manager_.release_register(left_reg);
                expression_result_reg_ = dest_reg;
            }
            break;
        case BinaryOp::Operator::Less:
            {
                std::string dest_reg = register_manager_.acquire_scratch_reg(*this);
                if (is_float_op) {
                    if (right_is_constant) {
                        // Load constant into temp register for float comparison
                        std::string temp_reg = register_manager_.acquire_fp_scratch_reg();
                        emit(Encoder::create_fmov_d_to_x(temp_reg, std::to_string(constant_value)));
                        emit(Encoder::create_fcmp_reg(left_reg, temp_reg));
                        register_manager_.release_register(temp_reg);
                    } else {
                        emit(Encoder::create_fcmp_reg(left_reg, right_reg));
                        register_manager_.release_register(right_reg);
                    }
                    emit(Encoder::create_cset(dest_reg, "LT"));
                } else {
                    if (right_is_constant) {
                        emit(Encoder::create_cmp_imm(left_reg, static_cast<int>(constant_value)));
                    } else {
                        emit(Encoder::create_cmp_reg(left_reg, right_reg));
                        register_manager_.release_register(right_reg);
                    }
                    emit(Encoder::create_cset(dest_reg, "LT"));
                }
                register_manager_.release_register(left_reg);
                expression_result_reg_ = dest_reg;
            }
            break;
        case BinaryOp::Operator::LessEqual:
            {
                std::string dest_reg = register_manager_.acquire_scratch_reg(*this);
                if (is_float_op) {
                    if (right_is_constant) {
                        // Load constant into temp register for float comparison
                        std::string temp_reg = register_manager_.acquire_fp_scratch_reg();
                        emit(Encoder::create_fmov_d_to_x(temp_reg, std::to_string(constant_value)));
                        emit(Encoder::create_fcmp_reg(left_reg, temp_reg));
                        register_manager_.release_register(temp_reg);
                    } else {
                        emit(Encoder::create_fcmp_reg(left_reg, right_reg));
                        register_manager_.release_register(right_reg);
                    }
                    emit(Encoder::create_cset(dest_reg, "LE"));
                } else {
                    if (right_is_constant) {
                        emit(Encoder::create_cmp_imm(left_reg, static_cast<int>(constant_value)));
                    } else {
                        emit(Encoder::create_cmp_reg(left_reg, right_reg));
                        register_manager_.release_register(right_reg);
                    }
                    emit(Encoder::create_cset(dest_reg, "LE"));
                }
                register_manager_.release_register(left_reg);
                expression_result_reg_ = dest_reg;
            }
            break;
        case BinaryOp::Operator::Greater:
            {
                std::string dest_reg = register_manager_.acquire_scratch_reg(*this);
                if (is_float_op) {
                    if (right_is_constant) {
                        // Load constant into temp register for float comparison
                        std::string temp_reg = register_manager_.acquire_fp_scratch_reg();
                        emit(Encoder::create_fmov_d_to_x(temp_reg, std::to_string(constant_value)));
                        emit(Encoder::create_fcmp_reg(left_reg, temp_reg));
                        register_manager_.release_register(temp_reg);
                    } else {
                        emit(Encoder::create_fcmp_reg(left_reg, right_reg));
                        register_manager_.release_register(right_reg);
                    }
                    emit(Encoder::create_cset(dest_reg, "GT"));
                } else {
                    if (right_is_constant) {
                        emit(Encoder::create_cmp_imm(left_reg, static_cast<int>(constant_value)));
                    } else {
                        emit(Encoder::create_cmp_reg(left_reg, right_reg));
                        register_manager_.release_register(right_reg);
                    }
                    emit(Encoder::create_cset(dest_reg, "GT"));
                }
                register_manager_.release_register(left_reg);
                expression_result_reg_ = dest_reg;
            }
            break;
        case BinaryOp::Operator::GreaterEqual:
            {
                std::string dest_reg = register_manager_.acquire_scratch_reg(*this);
                if (is_float_op) {
                    if (right_is_constant) {
                        // Load constant into temp register for float comparison
                        std::string temp_reg = register_manager_.acquire_fp_scratch_reg();
                        emit(Encoder::create_fmov_d_to_x(temp_reg, std::to_string(constant_value)));
                        emit(Encoder::create_fcmp_reg(left_reg, temp_reg));
                        register_manager_.release_register(temp_reg);
                    } else {
                        emit(Encoder::create_fcmp_reg(left_reg, right_reg));
                        register_manager_.release_register(right_reg);
                    }
                    emit(Encoder::create_cset(dest_reg, "GE"));
                } else {
                    if (right_is_constant) {
                        emit(Encoder::create_cmp_imm(left_reg, static_cast<int>(constant_value)));
                    } else {
                        emit(Encoder::create_cmp_reg(left_reg, right_reg));
                        register_manager_.release_register(right_reg);
                    }
                    emit(Encoder::create_cset(dest_reg, "GE"));
                }
                register_manager_.release_register(left_reg);
                expression_result_reg_ = dest_reg;
            }
            break;
        case BinaryOp::Operator::BitwiseAnd:
            if (right_is_constant) {
                std::string temp_reg = register_manager_.acquire_scratch_reg(*this);
                emit(Encoder::create_movz_movk_abs64(temp_reg, constant_value, ""));
                emit(Encoder::create_and_reg(left_reg, left_reg, temp_reg));
                register_manager_.release_register(temp_reg);
            } else {
                emit(Encoder::create_and_reg(left_reg, left_reg, right_reg));
            }
            break;
        case BinaryOp::Operator::LogicalAnd:
            // Short-circuit logic for logical AND should be handled here
            generate_short_circuit_and(node);
            break;
        case BinaryOp::Operator::LogicalOr:
            if (right_is_constant) {
                std::string temp_reg = register_manager_.acquire_scratch_reg(*this);
                emit(Encoder::create_movz_movk_abs64(temp_reg, constant_value, ""));
                emit(Encoder::create_orr_reg(left_reg, left_reg, temp_reg));
                register_manager_.release_register(temp_reg);
            } else {
                emit(Encoder::create_orr_reg(left_reg, left_reg, right_reg));
            }
            break;
        case BinaryOp::Operator::Equivalence:
            // Use logical XOR for equivalence
            {
                std::string dest_reg = register_manager_.acquire_scratch_reg(*this);
                if (right_is_constant) {
                    emit(Encoder::create_cmp_imm(left_reg, static_cast<int>(constant_value)));
                } else {
                    emit(Encoder::create_cmp_reg(left_reg, right_reg));
                    register_manager_.release_register(right_reg);
                }
                emit(Encoder::create_cset(dest_reg, "EQ"));
                register_manager_.release_register(left_reg);
                expression_result_reg_ = dest_reg;
            }
            break;
        case BinaryOp::Operator::LeftShift:
            if (right_is_constant) {
                std::string temp_reg = register_manager_.acquire_scratch_reg(*this);
                emit(Encoder::create_movz_movk_abs64(temp_reg, constant_value, ""));
                emit(Encoder::create_lsl_reg(left_reg, left_reg, temp_reg));
                register_manager_.release_register(temp_reg);
            } else {
                emit(Encoder::create_lsl_reg(left_reg, left_reg, right_reg));
            }
            break;
        case BinaryOp::Operator::RightShift:
            if (right_is_constant) {
                std::string temp_reg = register_manager_.acquire_scratch_reg(*this);
                emit(Encoder::create_movz_movk_abs64(temp_reg, constant_value, ""));
                emit(Encoder::create_lsr_reg(left_reg, left_reg, temp_reg));
                register_manager_.release_register(temp_reg);
            } else {
                emit(Encoder::create_lsr_reg(left_reg, left_reg, right_reg));
            }
            break;

        default:
            // Handle unknown or unsupported operators
            std::cerr << "Error: Unknown binary operator.\n";
            break;
    }

    // After the operation, the right-hand side register is no longer needed and can be freed.
    // OPTIMIZATION: Only release if we actually allocated a register for the right operand
    if (!right_is_constant) {
        register_manager_.release_register(right_reg);
        debug_print("Released right-hand operand register: " + right_reg);
    } else {
        debug_print("OPTIMIZATION: No right register to release (used immediate value)");
    }

    // Only assign expression_result_reg_ = left_reg for arithmetic and bitwise operations,
    // not for comparison operations (which already set expression_result_reg_).
    switch (node.op) {
        case BinaryOp::Operator::Add:
        case BinaryOp::Operator::Subtract:
        case BinaryOp::Operator::Multiply:
        case BinaryOp::Operator::Divide:
        case BinaryOp::Operator::Remainder:
        case BinaryOp::Operator::BitwiseAnd:
        case BinaryOp::Operator::LogicalOr:
        case BinaryOp::Operator::LeftShift:
        case BinaryOp::Operator::RightShift:

            expression_result_reg_ = left_reg;
            break;
        default:
            // For comparisons and logical AND, expression_result_reg_ is already set correctly.
            break;
    }

    debug_print("Finished visiting BinaryOp node. Result in " + expression_result_reg_);
}

// Generates code for short-circuit logical AND (&&)
void NewCodeGenerator::generate_short_circuit_and(BinaryOp& node) {
    // Generate code for the left operand
    generate_expression_code(*node.left);
    std::string condition_reg = expression_result_reg_;

    // Create labels for the false case and the end
    std::string false_label = label_manager_.create_label();
    std::string end_label = label_manager_.create_label();

    // If the left operand is false (0), jump to the false label
    // In BCPL, 0 is false, anything else is true
    emit(Encoder::create_cmp_imm(condition_reg, 0));
    emit(Encoder::create_branch_conditional("EQ", false_label));

    // Left operand is true, evaluate the right operand
    generate_expression_code(*node.right);
    // The result is already in expression_result_reg_

    // Unconditionally jump to end
    emit(Encoder::create_branch_unconditional(end_label));

    // False case: just load 0 (false)
    instruction_stream_.define_label(false_label);
    emit(Encoder::create_movz_imm(expression_result_reg_, 0, 0));

    // End label
    instruction_stream_.define_label(end_label);
}

// Generates code for short-circuit logical OR (||)
void NewCodeGenerator::generate_short_circuit_or(BinaryOp& node) {
    // Generate code for the left operand
    generate_expression_code(*node.left);
    std::string condition_reg = expression_result_reg_;

    // Create labels for the true case and the end
    std::string true_label = label_manager_.create_label();
    std::string end_label = label_manager_.create_label();

    // If the left operand is true (not 0), jump to the true label
    emit(Encoder::create_cmp_imm(condition_reg, 0));
    emit(Encoder::create_branch_conditional("NE", true_label));

    // Left operand is false, evaluate the right operand
    generate_expression_code(*node.right);
    // The result is already in expression_result_reg_

    // Unconditionally jump to end
    emit(Encoder::create_branch_unconditional(end_label));

    // True case: load -1 (true in BCPL)
    instruction_stream_.define_label(true_label);
    emit(Encoder::create_movz_imm(expression_result_reg_, 0xFFFF, 0));
    emit(Encoder::create_movk_imm(expression_result_reg_, 0xFFFF, 16));
    emit(Encoder::create_movk_imm(expression_result_reg_, 0xFFFF, 32));
    emit(Encoder::create_movk_imm(expression_result_reg_, 0xFFFF, 48));

    // End label
    instruction_stream_.define_label(end_label);
}

bool NewCodeGenerator::lookup_symbol(const std::string& name, Symbol& symbol) const {
    if (!symbol_table_) {
        return false;
    }
    
    // 1. First, try to find the symbol in the context of the current function.
    // This correctly finds local variables and parameters.
    if (!current_function_name_.empty()) {
        if (symbol_table_->lookup(name, current_function_name_, symbol)) {
            return true;
        }
    }
    
    // 2. If not found locally, try to find the symbol in the "Global" scope.
    // This is the crucial fallback that finds global variables.
    if (symbol_table_->lookup(name, "Global", symbol)) {
        return true;
    }
    
    // Fallback for symbols without a specific function context (like runtime functions)
    return symbol_table_->lookup(name, symbol);
}

// --- Linear Scan Register Allocation ---
// REMOVED: performLinearScan method - all register allocation must be done upfront
// This method violated the separation of concerns between analysis and code generation phases

// Helper method to check if a live interval crosses any function call sites
bool NewCodeGenerator::does_interval_cross_call(const LiveInterval& interval, const std::vector<int>& call_sites) const {
    for (int call_site : call_sites) {
        if (interval.start_point < call_site && interval.end_point > call_site) {
            return true;
        }
    }
    return false;
}

// Updates the stack offsets for all spilled variables after prologue generation
void NewCodeGenerator::update_spill_offsets() {
if (!current_frame_manager_) {
    debug_print("WARNING: No current frame manager when updating spill offsets");
    return;
}

bool any_updates = false;
for (auto& [var_name, interval] : current_function_allocation_) {
    if (var_name == "WRITES" || var_name == "WRITEN" ||
        var_name == "FWRITE" || var_name == "WRITEF" ||
        var_name == "READN" ||
        data_generator_.is_global_variable(var_name)) {
        // Skip runtime functions and globals
        continue;
    }

    if (interval.is_spilled && interval.stack_offset == -1) {
        try {
            if (current_frame_manager_->has_local(var_name)) {
                interval.stack_offset = current_frame_manager_->get_offset(var_name);
                debug_print("Updated stack offset for spilled variable '" + var_name +
                            "': " + std::to_string(interval.stack_offset));
                any_updates = true;
            } else {
                // Add it to the frame manager if it's not there
                debug_print("Adding missing variable '" + var_name + "' to frame manager");
                // Default to INTEGER if type is unknown - look up from function metrics
                VarType type = VarType::INTEGER; // Default
                auto metrics_it = analyzer_.get_function_metrics().find(current_function_name_);
                if (metrics_it != analyzer_.get_function_metrics().end()) {
                    const auto& metrics = metrics_it->second;
                    auto var_it = metrics.variable_types.find(var_name);
                    if (var_it != metrics.variable_types.end()) {
                        type = var_it->second;
                    }
                }
                current_frame_manager_->add_local(var_name, type);

                // Now try to get its offset
                try {
                    interval.stack_offset = current_frame_manager_->get_offset(var_name);
                    debug_print("Added and updated stack offset for variable '" + var_name +
                                "': " + std::to_string(interval.stack_offset));
                    any_updates = true;
                } catch (const std::runtime_error& e) {
                    debug_print("ERROR: Failed to get offset after adding variable '" + var_name +
                                "': " + std::string(e.what()));
                }
            }
        } catch (const std::runtime_error& e) {
            debug_print("ERROR: Failed to get offset for spilled variable '" + var_name +
                        "': " + std::string(e.what()));
        }
    }
}

    if (any_updates) {
        debug_print("Updated stack offsets for spilled variables after prologue generation");
    }
}



// --- Public Entry Point Methods ---

// The main entry point for starting code generation.
void NewCodeGenerator::generate_code(Program& program) {
    debug_print("Starting code generation for program.");
    
    // STEP 1: Pre-Analysis Scan - Find all external function calls
    debug_print("Step 1: Performing pre-analysis scan for external functions...");
    std::set<std::string> external_functions = external_scanner_.scan(program);
    
    debug_print("Found " + std::to_string(external_functions.size()) + " unique external functions:");
    for (const std::string& func_name : external_functions) {
        debug_print("  - " + func_name);
    }
    
    // STEP 2: Generate veneer section using standard pipeline
    debug_print("Step 2: Generating veneer section using standard pipeline...");
    veneer_manager_.generate_veneers(external_functions, instruction_stream_, label_manager_);
    
    // STEP 3: Generate main function code
    debug_print("Step 3: Generating main function code...");
    // Delegates to visit(Program& node), which is implemented in generators/gen_Program.cpp.
    // This starts the AST traversal.
    visit(program);
    
    data_generator_.calculate_global_offsets();
    data_generator_.generate_rodata_section(instruction_stream_);
    // --- START OF FIX ---
    // This call was missing. It defines L__data_segment_base and global variables.
    data_generator_.generate_data_section(instruction_stream_);
    // --- END OF FIX ---
    debug_print("Code generation finished.");
}

// Generic fallback for unsupported expressions
void NewCodeGenerator::visit(Expression& node) {
    debug_print("Visiting generic Expression node.");
    // Handle unsupported expressions or log a warning
    std::cerr << "[WARNING] Unsupported expression encountered during code generation." << std::endl;
}

// --- Single-Buffer Veneer Management Implementation ---

void NewCodeGenerator::initialize_veneer_manager(uint64_t base_address) {
    code_buffer_base_address_ = base_address;
    veneer_manager_.initialize(base_address);
    debug_print("Initialized veneer manager with base address: 0x" + 
                std::to_string(base_address));
}

uint64_t NewCodeGenerator::get_veneer_address(const std::string& function_name) const {
    return veneer_manager_.get_veneer_address(function_name);
}

uint64_t NewCodeGenerator::get_main_code_start_address() const {
    return veneer_manager_.get_main_code_start_address();
}

// This method is no longer needed in the new standardized system
// External function scanning and veneer generation are now integrated
// into the main generate_code() method

// generate_veneer_section() method removed - veneers are now generated
// directly into the instruction stream using the standard pipeline

void NewCodeGenerator::update_code_buffer_base_address(uint64_t base_address) {
    if (code_buffer_base_address_ != base_address) {
        code_buffer_base_address_ = base_address;
        veneer_manager_.initialize(base_address);
        debug_print("Updated code buffer base address to: 0x" + std::to_string(base_address));
    }
}

// --- OOP: visit(SuperMethodCallExpression&) ---
// Generates code for SUPER.method(args...) calls.
void NewCodeGenerator::visit(SuperMethodCallExpression& node) {
    debug_print("Visiting SuperMethodCallExpression node for parent method: " + node.member_name);

    // Phase 4: Manual spilling removed - LinearScanAllocator ensures call-crossing
    // variables are allocated to callee-saved registers (preserved automatically)

    // 1. Evaluate arguments and store results in temp registers.
    std::vector<std::string> arg_regs;
    for (auto& arg : node.arguments) {
        generate_expression_code(*arg);
        // It's safer to spill these to temporary registers if they aren't already.
        std::string temp_reg = register_manager_.acquire_scratch_reg(*this);
        emit(Encoder::create_mov_reg(temp_reg, expression_result_reg_));
        register_manager_.release_register(expression_result_reg_);
        arg_regs.push_back(temp_reg);
    }

    // 2. Get the CURRENT object's '_this' pointer using the allocator.
    std::string this_reg = get_variable_register("_this");

    // 3. Set up arguments for the parent method call.
    //    - Arg 0 (X0) is always the '_this' pointer.
    //    - Subsequent arguments (initialX, initialY) go into X1, X2, etc.
    emit(Encoder::create_mov_reg("X0", this_reg));
    register_manager_.release_register(this_reg);

    for (size_t i = 0; i < arg_regs.size(); ++i) {
        std::string target_reg = "X" + std::to_string(i + 1); // Arguments start at X1
        emit(Encoder::create_mov_reg(target_reg, arg_regs[i]));
        register_manager_.release_register(arg_regs[i]);
    }

    // 4. Get parent class and method info to find the correct vtable slot.
    const ClassTableEntry* class_entry = class_table_->get_class(current_class_name_);
    if (!class_entry) throw std::runtime_error("SUPER: Current class not found in ClassTable.");
    const ClassTableEntry* parent_entry = class_table_->get_class(class_entry->parent_name);
    if (!parent_entry) throw std::runtime_error("SUPER: Parent class not found in ClassTable.");

    // Use findMethod which correctly searches the inheritance hierarchy.
    ClassMethodInfo* parent_method_info = parent_entry->findMethod(node.member_name, false);
    if (!parent_method_info) {
        throw std::runtime_error("SUPER: Parent method '" + node.member_name + "' not found.");
    }
    size_t vtable_offset = parent_method_info->vtable_slot * 8;

    // 5. Generate vtable lookup code.
    std::string vtable_ptr_reg = register_manager_.acquire_scratch_reg(*this);
    std::string method_addr_reg = register_manager_.acquire_scratch_reg(*this);

    // LDR vtable_ptr_reg, [X0, #0] : Load object's vtable pointer using the '_this' pointer in X0.
    emit(Encoder::create_ldr_imm(vtable_ptr_reg, "X0", 0, "Load vtable pointer for SUPER call"));

    // LDR method_addr_reg, [vtable_ptr_reg, #vtable_offset] : Load parent method address.
    emit(Encoder::create_ldr_imm(method_addr_reg, vtable_ptr_reg, vtable_offset, "Load parent method address for SUPER call"));

    register_manager_.release_register(vtable_ptr_reg);

    // 7. Make the indirect call: BLR method_addr_reg
    emit(Encoder::create_branch_with_link_register(method_addr_reg));
    register_manager_.release_register(method_addr_reg);

    // --- Invalidate caller-saved registers after the SUPER call ---
    register_manager_.invalidate_caller_saved_registers();
    // Phase 4: Manual restore removed - callee-saved registers preserved automatically

    // 8. The result of this expression is in X0 (by convention)
    expression_result_reg_ = "X0";
}

// --- OOP: handle_member_access_assignment ---
// Handles assignment to a class member field (e.g., obj.field := value)
void NewCodeGenerator::handle_member_access_assignment(MemberAccessExpression* member_access, const std::string& value_to_store_reg) {
    debug_print("Handling member access assignment for: " + member_access->member_name);

    // 1. Generate code for the object expression (e.g., '_this') to get its pointer.
    generate_expression_code(*member_access->object_expr);
    std::string object_ptr_reg = expression_result_reg_;

    // 2. Determine the class of the object to look up the member offset.
    std::string class_name = get_class_name_for_expression(member_access->object_expr.get());
    if (class_name.empty()) {
        throw std::runtime_error("Could not determine class for member assignment.");
    }
    const ClassTableEntry* class_entry = class_table_->get_class(class_name);
    if (!class_entry) {
        throw std::runtime_error("Class '" + class_name + "' not found in ClassTable.");
    }

    // 3. Find the member's offset from the ClassTable.
    auto member_it = class_entry->member_variables.find(member_access->member_name);
    if (member_it == class_entry->member_variables.end()) {
        throw std::runtime_error("Member '" + member_access->member_name + "' not found in class '" + class_name + "'.");
    }
    const ClassMemberInfo& member_info = member_it->second;
    size_t offset = member_info.offset;

    // 4. Emit the appropriate STR (Store Register) instruction based on member type.
    if (member_info.type == VarType::FLOAT) {
        // Check if the value is coming from a floating-point source
        if (value_to_store_reg[0] == 'D' || value_to_store_reg[0] == 'd') {
            // Source is already a D register, store directly
            emit(Instruction(0, "// Store to float member " + member_access->member_name));
            emit(Encoder::create_str_fp_imm(value_to_store_reg, object_ptr_reg, offset));
        } else {
            // Source is an X register, convert to float before storing
            std::string fp_reg = register_manager_.get_free_float_register();
            emit(Instruction(0, "// Convert integer to float before storing"));
            emit(Encoder::create_scvtf_reg(fp_reg, value_to_store_reg));
            emit(Encoder::create_str_fp_imm(fp_reg, object_ptr_reg, offset));
            register_manager_.release_fp_register(fp_reg);
        }
    } else {
        // For integer members, check if the source is a floating-point register
        if (value_to_store_reg[0] == 'D' || value_to_store_reg[0] == 'd') {
            // Convert float to integer before storing
            std::string int_reg = register_manager_.acquire_scratch_reg(*this);
            emit(Instruction(0, "// Convert float to integer before storing"));
            emit(Encoder::create_fcvtzs_reg(int_reg, value_to_store_reg));
            emit(Encoder::create_str_imm(int_reg, object_ptr_reg, offset, "Store to member " + member_access->member_name));
            register_manager_.release_register(int_reg);
        } else {
            // Source is already an X register, store directly
            emit(Encoder::create_str_imm(value_to_store_reg, object_ptr_reg, offset, "Store to member " + member_access->member_name));
        }
    }

    // 5. Clean up the used registers.
    // --- START OF NEW LOGIC ---
    // After storing, synchronize the home register for the member variable.
    auto alloc_it = current_function_allocation_.find(member_access->member_name);
    if (alloc_it != current_function_allocation_.end()) {
        const LiveInterval& allocation = alloc_it->second;
        if (!allocation.is_spilled && !allocation.assigned_register.empty()) {
            const std::string& home_reg = allocation.assigned_register;
            if (home_reg != value_to_store_reg) {
                debug_print("  Updating home register " + home_reg + " for member '" + member_access->member_name + "' after store.");
                if (register_manager_.is_fp_register(home_reg)) {
                    emit(Encoder::create_fmov_reg(home_reg, value_to_store_reg));
                } else {
                    emit(Encoder::create_mov_reg(home_reg, value_to_store_reg));
                }
            }
        }
    }
    // --- END OF NEW LOGIC ---

    register_manager_.release_register(object_ptr_reg);
    register_manager_.release_register(value_to_store_reg);
}

// --- OOP: visit(NewExpression&) ---
// Allocates memory for a new object of the given class using the ClassTable.
void NewCodeGenerator::visit(NewExpression& node) {
    debug_print("Visiting NewExpression node for class: " + node.class_name);
    debug_print("  - Constructor arguments count: " + std::to_string(node.constructor_arguments.size()));
    debug_print("  - Assigned variable name: " + (node.assigned_variable_name.empty() ? "[empty]" : node.assigned_variable_name));

    const ClassTableEntry* entry = class_table_->get_class(node.class_name);
    if (!entry) {
        throw std::runtime_error("Class '" + node.class_name + "' not found for NEW expression.");
    }

    // --- STEP 1: Allocate Memory ---
    size_t object_size = entry->instance_size;
    emit(Encoder::create_movz_movk_abs64("X0", object_size, "")); // Arg for allocator
    emit(Encoder::create_branch_with_link("OBJECT_HEAP_ALLOC"));
    // Result: pointer to new object is in X0.

    // Preserve the object pointer in a callee-saved register across the constructor call
    std::string obj_ptr_reg = register_manager_.get_next_available_temp_reg();
    emit(Encoder::create_mov_reg(obj_ptr_reg, "X0"));

    // --- STEP 2: Store VTable Pointer ---
    std::string vtable_reg = register_manager_.acquire_scratch_reg(*this);
    std::string vtable_label = node.class_name + "_vtable";
    emit(Encoder::create_adrp(vtable_reg, vtable_label));
    emit(Encoder::create_add_literal(vtable_reg, vtable_reg, vtable_label));
    emit(Encoder::create_str_imm(vtable_reg, obj_ptr_reg, 0, "store vtable ptr"));
    register_manager_.release_register(vtable_reg);

    // --- STEP 3: Call the CREATE Routine (Constructor) ---
    if (entry->constructor) {
        debug_print("Calling CREATE routine for " + node.class_name);
        ClassMethodInfo* create_method_info = const_cast<ClassTableEntry*>(entry)->lookup_method("CREATE");
        if (!create_method_info) {
            throw std::runtime_error("CREATE method metadata not found in class table for class: " + node.class_name);
        }
        debug_print("  - CREATE method found at vtable slot: " + std::to_string(create_method_info->vtable_slot));
        debug_print("  - Will pass " + std::to_string(node.constructor_arguments.size()) + " arguments to CREATE");

        // Argument 0: The 'this' pointer goes in X0.
        emit(Encoder::create_mov_reg("X0", obj_ptr_reg));

        // Arguments 1..N: The arguments from the NEW expression.
        for (size_t i = 0; i < node.constructor_arguments.size(); ++i) {
            generate_expression_code(*node.constructor_arguments[i]);
            std::string arg_reg = expression_result_reg_;
            std::string dest_reg = "X" + std::to_string(i + 1);
            if (arg_reg != dest_reg) {
                emit(Encoder::create_mov_reg(dest_reg, arg_reg));
                register_manager_.release_register(arg_reg);
            }
        }

        // Load method address from the vtable (get slot from ClassMethodInfo)
        size_t create_slot = create_method_info->vtable_slot;
        std::string vtable_ptr_reg = register_manager_.acquire_scratch_reg(*this);
        std::string method_addr_reg = register_manager_.acquire_scratch_reg(*this);

        emit(Encoder::create_ldr_imm(vtable_ptr_reg, obj_ptr_reg, 0, "Load vtable pointer for CREATE call"));
        emit(Encoder::create_ldr_imm(method_addr_reg, vtable_ptr_reg, create_slot * 8, "Load CREATE method address"));

        // Make the call
        emit(Encoder::create_branch_with_link_register(method_addr_reg));

        register_manager_.release_register(vtable_ptr_reg);
        register_manager_.release_register(method_addr_reg);
    }

    // --- STEP 4: The result of the NEW expression is the object pointer ---
    expression_result_reg_ = obj_ptr_reg;
}

// --- OOP: MemberAccessExpression codegen --- //


// --- Assignment helpers ---

// Helper function to handle class member variable assignment
void NewCodeGenerator::handle_member_variable_assignment(const std::string& var_name, const std::string& value_to_store_reg) {
    debug_print("Handling assignment for class member: " + var_name);

    const ClassTableEntry* class_entry = class_table_->get_class(current_class_name_);
    if (!class_entry) {
        throw std::runtime_error("Could not find class entry for '" + current_class_name_ + "' during member assignment.");
    }

    auto member_it = class_entry->member_variables.find(var_name);
    if (member_it == class_entry->member_variables.end()) {
        throw std::runtime_error("Compiler consistency error: Member '" + var_name + "' not found in class '" + current_class_name_ + "' for assignment.");
    }

    const auto& member_info = member_it->second;

    debug_print("Member variable '" + var_name + "' found in class '" + current_class_name_ +
                "' with offset " + std::to_string(member_info.offset));

    // CRITICAL DEBUG: Verify if offset is overlapping with vtable
    if (member_info.offset == 0) {
        debug_print("*** CRITICAL ERROR: Member '" + var_name + "' offset is 0, will overwrite vtable pointer! ***");
    }

    // Use get_variable_register to retrieve the register for _this.
    std::string this_ptr_reg = get_variable_register("_this");
    emit(Encoder::create_str_imm(value_to_store_reg, this_ptr_reg, member_info.offset, "Store to member " + var_name));

    // --- START OF NEW LOGIC ---
    // After storing to memory, synchronize the variable's home register.
    auto alloc_it = current_function_allocation_.find(var_name);
    if (alloc_it != current_function_allocation_.end()) {
        const LiveInterval& allocation = alloc_it->second;
        if (!allocation.is_spilled && !allocation.assigned_register.empty()) {
            const std::string& home_reg = allocation.assigned_register;
            if (home_reg != value_to_store_reg) {
                debug_print("  Updating home register " + home_reg + " for member '" + var_name + "' after store.");
                if (register_manager_.is_fp_register(home_reg)) {
                    emit(Encoder::create_fmov_reg(home_reg, value_to_store_reg));
                } else {
                    emit(Encoder::create_mov_reg(home_reg, value_to_store_reg));
                }
            }
        }
    }
    // --- END OF NEW LOGIC ---

// Clean up
register_manager_.release_register(this_ptr_reg);
}

void NewCodeGenerator::handle_variable_assignment(VariableAccess* var_access, const std::string& value_to_store_reg) {
    debug_print("Handling assignment for variable: " + var_access->name);

    // Check if this is a class member variable assignment
    if (!current_class_name_.empty() && class_table_) {
        const ClassTableEntry* class_entry = class_table_->get_class(current_class_name_);
        if (class_entry && class_entry->member_variables.count(var_access->name)) {
            handle_member_variable_assignment(var_access->name, value_to_store_reg);
            // After storing, we still need to release the value register
            register_manager_.release_register(value_to_store_reg);
            return; // We are done
        }
    }

    bool is_source_float = register_manager_.is_fp_register(value_to_store_reg);
    
    // Get destination variable type from SymbolTable (handles all variable scopes)
    Symbol symbol;
    VarType dest_type = VarType::UNKNOWN;
    if (lookup_symbol(var_access->name, symbol)) {
        dest_type = symbol.type;
    } else {
        // Fallback for safety, though it shouldn't be needed for known variables
        dest_type = current_frame_manager_->get_variable_type(var_access->name);
    }
    
    bool is_dest_float = (dest_type == VarType::FLOAT);

    std::string final_reg_to_store = value_to_store_reg;
    bool reg_was_converted = false;

    if (is_source_float && !is_dest_float) {
        // Coerce float value to integer
        debug_print("Coercing float value from " + value_to_store_reg + " to integer for variable '" + var_access->name + "'.");
        std::string int_reg = register_manager_.acquire_scratch_reg(*this);
        emit(Encoder::create_fcvtzs_reg(int_reg, value_to_store_reg)); // Convert float to signed int
        final_reg_to_store = int_reg;
        reg_was_converted = true;
    } else if (!is_source_float && is_dest_float) {
        // Coerce integer value to float
        debug_print("Coercing integer value from " + value_to_store_reg + " to float for variable '" + var_access->name + "'.");
        std::string fp_reg = register_manager_.acquire_fp_scratch_reg();
        emit(Encoder::create_scvtf_reg(fp_reg, value_to_store_reg)); // Convert signed int to float
        final_reg_to_store = fp_reg;
        reg_was_converted = true;
    }

    store_variable_register(var_access->name, final_reg_to_store);

    // --- Synchronize the variable's home register after store ---
    auto alloc_it = current_function_allocation_.find(var_access->name);
    if (alloc_it != current_function_allocation_.end()) {
        const LiveInterval& allocation = alloc_it->second;
        if (!allocation.is_spilled && !allocation.assigned_register.empty()) {
            const std::string& home_reg = allocation.assigned_register;
            if (home_reg != final_reg_to_store) {
                debug_print("  Updating home register " + home_reg + " for variable '" + var_access->name + "' after store.");
                if (register_manager_.is_fp_register(home_reg)) {
                    emit(Encoder::create_fmov_reg(home_reg, final_reg_to_store));
                } else {
                    emit(Encoder::create_mov_reg(home_reg, final_reg_to_store));
                }
            }
        }
    }

    // Release the original register if a new one was created for conversion.
    if (reg_was_converted) {
        register_manager_.release_register(value_to_store_reg);
    }
    // Always release the final register used for the store.
    register_manager_.release_register(final_reg_to_store);
}

void NewCodeGenerator::handle_vector_assignment(VectorAccess* vec_access, const std::string& value_to_store_reg) {
    // 1. Evaluate vector_expr to get the base address (e.g., V)
    generate_expression_code(*vec_access->vector_expr);
    std::string vector_base_reg = expression_result_reg_;

    // 2. Evaluate index_expr to get the index (e.g., 0)
    generate_expression_code(*vec_access->index_expr);
    std::string index_reg = expression_result_reg_;

    // 3. Calculate the byte offset: index * 8 (for 64-bit words)
    // FIX: Use a temporary register for the offset calculation to avoid corrupting the loop variable
    std::string offset_reg = register_manager_.acquire_scratch_reg(*this);
    emit(Encoder::create_lsl_imm(offset_reg, index_reg, 3));
    debug_print("Calculated byte offset for vector assignment.");

    // 4. Calculate the effective memory address: base + offset
    std::string effective_addr_reg = register_manager_.get_free_register(*this);
    emit(Encoder::create_add_reg(effective_addr_reg, vector_base_reg, offset_reg));
    debug_print("Calculated effective address for vector assignment.");

    // Release registers used for address calculation
    register_manager_.release_register(vector_base_reg);
    register_manager_.release_register(index_reg);
    register_manager_.release_scratch_reg(offset_reg);

    // 5. Store the RHS value to the effective address
    emit(Encoder::create_str_imm(value_to_store_reg, effective_addr_reg, 0));
    debug_print("Stored value to vector element.");

    // ====================== START OF FIX ======================
    // After storing, we must synchronize the home register for the vector variable itself,
    // in case it was spilled during the address calculation for the element.
    if (auto* base_var = dynamic_cast<VariableAccess*>(vec_access->vector_expr.get())) {
        auto alloc_it = current_function_allocation_.find(base_var->name);
        if (alloc_it != current_function_allocation_.end()) {
            const LiveInterval& allocation = alloc_it->second;
            if (!allocation.is_spilled && !allocation.assigned_register.empty()) {
                const std::string& home_reg = allocation.assigned_register;
                if (home_reg != vector_base_reg) {
                    debug_print("  Synchronizing home register " + home_reg + " for vector '" + base_var->name + "' after store.");
                    emit(Encoder::create_mov_reg(home_reg, vector_base_reg));
                }
            }
        }
    }
    // ======================= END OF FIX =======================

    // Release registers used in the store
    register_manager_.release_register(value_to_store_reg);
    register_manager_.release_register(effective_addr_reg);
}

void NewCodeGenerator::handle_char_indirection_assignment(CharIndirection* char_indirection, const std::string& value_to_store_reg) {
    // 1. Evaluate string_expr to get the base address (e.g., S)
    generate_expression_code(*char_indirection->string_expr);
    std::string string_base_reg = expression_result_reg_;

    // 2. Evaluate index_expr to get the index (e.g., 0)
    generate_expression_code(*char_indirection->index_expr);
    std::string index_reg = expression_result_reg_;

    // 3. Calculate the byte offset: index * 4 (for 32-bit characters in BCPL)
    emit(Encoder::create_lsl_imm(index_reg, index_reg, 2));
    debug_print("Calculated byte offset for char indirection assignment.");

    // 4. Calculate the effective memory address: base + offset
    std::string effective_addr_reg = register_manager_.get_free_register(*this);
    emit(Encoder::create_add_reg(effective_addr_reg, string_base_reg, index_reg));
    debug_print("Calculated effective address for char indirection assignment.");

    // Release registers used for address calculation
    register_manager_.release_register(string_base_reg);
    register_manager_.release_register(index_reg);

    // 5. Store the RHS value to the effective address (using STR for 32-bit word for char)
    // FIX: Use the new 32-bit store instruction for character assignment
    emit(Encoder::create_str_word_imm(value_to_store_reg, effective_addr_reg, 0));
    debug_print("Stored value to character element.");

    // ====================== START OF FIX ======================
    // Add the same synchronization logic here for the string base pointer.
    if (auto* base_var = dynamic_cast<VariableAccess*>(char_indirection->string_expr.get())) {
        auto alloc_it = current_function_allocation_.find(base_var->name);
        if (alloc_it != current_function_allocation_.end()) {
            const LiveInterval& allocation = alloc_it->second;
            if (!allocation.is_spilled && !allocation.assigned_register.empty()) {
                const std::string& home_reg = allocation.assigned_register;
                if (home_reg != string_base_reg) {
                    debug_print("  Synchronizing home register " + home_reg + " for string '" + base_var->name + "' after store.");
                    emit(Encoder::create_mov_reg(home_reg, string_base_reg));
                }
            }
        }
    }
    // ======================= END OF FIX =======================

    // Release registers used in the store
    register_manager_.release_register(value_to_store_reg);
    register_manager_.release_register(effective_addr_reg);
}



// --- Common Helper Methods (private implementations) ---
// These methods are shared across various visit functions and encapsulate common tasks.

// Emits a sequence of instructions to the instruction stream.
void NewCodeGenerator::emit(const std::vector<Instruction>& instrs) {
    for (const auto& instr : instrs) {
        instruction_stream_.add(instr);
    }
}

// Emits a single instruction to the instruction stream.
void NewCodeGenerator::emit(const Instruction& instr) {
    debug_print_level("Emitting instruction: " + instr.assembly_text, 5);
    
    // Prevent consecutive identical MOV instructions (unless protected by nopeep)
    if (!instr.nopeep && !instruction_stream_.empty()) {
        const Instruction& last_instr = instruction_stream_.get_last_instruction();
        
        // Check if both are MOV instructions with same operands
        if (InstructionDecoder::getOpcode(instr) == InstructionDecoder::OpType::MOV &&
            InstructionDecoder::getOpcode(last_instr) == InstructionDecoder::OpType::MOV &&
            !InstructionDecoder::usesImmediate(instr) && 
            !InstructionDecoder::usesImmediate(last_instr) &&
            InstructionDecoder::getDestReg(instr) == InstructionDecoder::getDestReg(last_instr) &&
            InstructionDecoder::getSrcReg1(instr) == InstructionDecoder::getSrcReg1(last_instr)) {
            
            debug_print_level("Skipping duplicate MOV instruction: " + instr.assembly_text, 3);
            return; // Skip adding this duplicate instruction
        }
    }
    
    instruction_stream_.add(instr);
}

// Stub implementation for emitting comments
void NewCodeGenerator::emit_comment(const std::string& comment) {
    debug_print_level("Comment: " + comment, 5);
    // Use an existing Instruction constructor that takes a comment
    emit(Instruction(0, "// " + comment));
}

// Helper for general debugging output.
void NewCodeGenerator::debug_print(const std::string& message) const {
    if (debug_enabled_) {
        std::cout << "[DEBUG] " << message << std::endl;
    }
}

// Helper for debugging output with a configurable level.
void NewCodeGenerator::debug_print_level(const std::string& message, int level) const {
    if (debug_enabled_ && level <= debug_level) {
        std::cout << "[DEBUG LEVEL " << level << "] " << message << std::endl;
    }
}

// Enters a new scope by saving the current symbol table and clearing it.
// Used for BLOCK statements, function/routine bodies, etc.
void NewCodeGenerator::enter_scope() {
    scope_stack_.push(current_scope_symbols_); // Save the current scope
    current_scope_symbols_.clear();            // Start a fresh scope
    debug_print("Entered new scope. Scope stack size: " + std::to_string(scope_stack_.size()));
}

// Exits the current scope by restoring the previous symbol table.
void NewCodeGenerator::exit_scope() {
    if (scope_stack_.empty()) {
        throw std::runtime_error("Attempted to exit scope when no scope was active.");
    }
    current_scope_symbols_ = scope_stack_.top(); // Restore previous scope
    scope_stack_.pop();                          // Remove from stack
    debug_print("Exited scope. Scope stack size: " + std::to_string(scope_stack_.size()));
}

// Handles the common logic for recursively generating code for an expression.
// The result of the expression should be stored in 'expression_result_reg_'.
void NewCodeGenerator::generate_expression_code(Expression& expr) {
    // Delegates to the specific visit method for the expression type (e.g., visit(NumberLiteral&)).
    // That visit method is responsible for setting expression_result_reg_.
    expr.accept(*this);
}

// Handles the common logic for recursively generating code for a statement.
// Statements do not typically have a "result" register.
void NewCodeGenerator::generate_statement_code(Statement& stmt) {
    // Delegates to the specific visit method for the statement type (e.g., visit(AssignmentStatement&)).
    stmt.accept(*this);
}

// Processes a list of declarations (e.g., from Program or BlockStatement).
void NewCodeGenerator::process_declarations(const std::vector<DeclPtr>& declarations) {
    for (const auto& decl_ptr : declarations) {
        if (decl_ptr) {
            debug_print_level(
                std::string("process_declarations: About to process declaration node at ") +
                std::to_string(reinterpret_cast<uintptr_t>(decl_ptr.get())),
                4
            );
            process_declaration(*decl_ptr); // Calls the helper for a single declaration
        }
    }
}

// Processes a single declaration.
void NewCodeGenerator::process_declaration(Declaration& decl) {
    // This helper ensures that the correct polymorphic visit method is called
    // based on the declaration's dynamic type (e.g., visit(LetDeclaration&)).
    decl.accept(*this);
}

void NewCodeGenerator::generate_float_to_int_truncation(const std::string& dest_x_reg, const std::string& src_d_reg) {
    // FCVTZS <Xd>, <Dn> (Floating-point Convert to Signed integer, rounding toward Zero)
    emit(Encoder::create_fcvtzs_reg(dest_x_reg, src_d_reg));
    debug_print("Generated FCVTZS to truncate " + src_d_reg + " to " + dest_x_reg);
}


// --- Variable Access/Storage Helpers ---
// These methods manage loading values from variables into registers and storing values from registers back to variables.
// get_variable_register moved under generators.

// Stores a register's value into a variable's memory location (stack for locals, data segment for globals).



// --- CORE: generate_function_like_code implementation ---
// This helper encapsulates the common logic for generating prologues, epilogues,
// and managing the call frame for functions and routines.
void NewCodeGenerator::generate_function_like_code(
    const std::string& name,
    const std::vector<std::string>& parameters, // Function/Routine parameters
    ASTNode& body_node,
    bool is_function_returning_value
) {

    std::string previous_analyzer_scope = analyzer_.get_current_function_scope();

    // --- FIX: Check if name is already qualified before setting it as the current function scope ---
    std::string unqualified_name = name;
    size_t qualifier_pos = name.find("::");
    bool is_already_qualified = (qualifier_pos != std::string::npos);

    if (is_already_qualified) {
        unqualified_name = name.substr(qualifier_pos + 2);
        debug_print("Name '" + name + "' is already qualified. Using unqualified part '" + unqualified_name + "' for scope.");
    }
    analyzer_.set_current_function_scope(unqualified_name);

    // --- FIX: CONSTRUCT THE MANGLED NAME FOR METRICS LOOKUP ---
    // CRITICAL FIX: Use the name as-is if it's already qualified, never double-qualify
    std::string metrics_lookup_name = name;

    // Do not add class prefix if name already has a qualifier
    if (!is_already_qualified && !current_class_name_.empty()) {
        metrics_lookup_name = NameMangler::mangleMethod(current_class_name_, name);
        debug_print("Method context detected. Using qualified name for metrics lookup: " + metrics_lookup_name);
    } else {
        debug_print("Using name as-is for metrics lookup: " + metrics_lookup_name);
    }
    // --- END OF FIX ---

    current_function_allocation_.clear();
    current_function_name_ = name; // Track the current function name

    // Use the potentially qualified name for return type lookup
    auto& return_types = analyzer_.get_function_return_types();
    auto return_type_it = return_types.find(metrics_lookup_name);
    if (return_type_it != return_types.end()) {
        current_function_return_type_ = return_type_it->second;
    } else {
        debug_print("WARNING: Function '" + metrics_lookup_name + "' not found in return types map. Defaulting to INTEGER.");
        current_function_return_type_ = VarType::INTEGER;
    }
    current_scope_name_ = name;    // Initialize scope tracking
    block_id_counter_ = 0;         // Reset block ID counter for each new function
    debug_print("DEBUG: generate_function_like_code called for: " + name);
    debug_print("Generating function-like code for: " + name);
    x28_is_loaded_in_current_function_ = false;
    
    // --- Clear local value tracking for new function ---
    clear_local_value_tracking();

    // --- SYNC REGISTERMANAGER WITH LINEARSCAN ALLOCATOR ---
    // This prevents the "two sources of truth" bug where scratch registers
    // conflict with variable registers allocated by LinearScanAllocator
    register_manager_.sync_with_allocator(all_allocations_, unqualified_name);
    debug_print("Synced RegisterManager with LinearScanAllocator for function: " + unqualified_name);

    // Create and store the unique epilogue label for this function
    current_function_epilogue_label_ = label_manager_.create_label();

    // --- FIX: Construct the correct mangled name for lookup ---
    // Create a lookup name for function metrics that may need to be qualified for class methods
    std::string entry_point_label = name; // Default to the simple name
    std::string lookup_name = name;       // Name to use for symbol table lookup

    // --- FIX: Check if name is already qualified before adding class prefix ---
    // Check if we are inside a class context by looking at the AST.
    // IMPORTANT: Use the previously computed is_already_qualified flag for consistency
    if (class_table_ && !current_class_name_.empty() && !is_already_qualified) {
        // We are compiling a method. Use NameMangler for consistent name lookup.
        lookup_name = NameMangler::mangleMethod(current_class_name_, name);
        debug_print("Method context detected. Using qualified name for lookup: " + lookup_name);
    } else {
        debug_print("Using name as-is for lookup: " + lookup_name);
    }
    // --- END OF FIX ---

    Symbol symbol;
    // Use the potentially mangled 'lookup_name' for the lookup
    if (symbol_table_ && symbol_table_->lookup(lookup_name, symbol) && !symbol.class_name.empty()) {
        // This is a class method. The symbol name is already mangled.
        entry_point_label = symbol.name; // Use the full name from the symbol
        debug_print("Generating code for method. Using mangled label from symbol: " + entry_point_label);
    }

    // --- TRIVIAL ACCESSOR OPTIMIZATION PATH (Fast Path) ---
    // Check the analyzer result using the finalized metrics_lookup_name.
    if (analyzer_.is_trivial_accessor_method(metrics_lookup_name)) {
        debug_print("✅ Detected TRIVIAL ACCESSOR method: '" + metrics_lookup_name + "'. Generating optimized code.");

        // 1. Define the entry point label
        instruction_stream_.define_label(entry_point_label);

        // 2. Get the member information from the analyzer metrics.
        auto metrics_it = analyzer_.get_function_metrics().find(metrics_lookup_name);
        if (metrics_it == analyzer_.get_function_metrics().end()) {
            debug_print("NewCodeGenerator Error: Function metrics not found for trivial accessor: " + metrics_lookup_name);
            return;
        }
        const auto& metrics = metrics_it->second;
        const std::string& member_name = metrics.accessed_member_name;

        // Handle the special case where the accessor returns 'SELF'/'_this'.
        if (member_name == "_this_ptr") {
            // The '_this' pointer is already in X0 (by calling convention). We just return.
            emit(Encoder::create_return());
            analyzer_.set_current_function_scope(previous_analyzer_scope);
            return; // Skip standard path.
        }

        // 3. Determine the class context.
        std::string defining_class_name = current_class_name_;
        if (defining_class_name.empty()) {
            // Extract from the mangled name if current context is empty.
            size_t pos = metrics_lookup_name.find("::");
            if (pos != std::string::npos) {
                defining_class_name = metrics_lookup_name.substr(0, pos);
            }
        }

        if (defining_class_name.empty() || !class_table_) {
             throw std::runtime_error("Cannot generate optimized accessor for '" + name + "' without class context or ClassTable.");
        }

        // 4. Look up the member offset and type.
        const ClassTableEntry* class_entry = class_table_->get_class(defining_class_name);
        if (!class_entry) {
             throw std::runtime_error("Class entry not found for optimized accessor: " + defining_class_name);
        }

        auto member_it = class_entry->member_variables.find(member_name);
        if (member_it == class_entry->member_variables.end()) {
            throw std::runtime_error("Internal Error: Member '" + member_name + "' not found during optimized accessor generation.");
        }

        // Assuming ClassMemberInfo structure is accessible.
        const ClassMemberInfo& member_info = member_it->second;
        size_t offset = member_info.offset;
        VarType member_type = member_info.type;

        // 5. Generate the optimized assembly.
        // The '_this' pointer is in X0. Load from [X0 + offset] into the return register.

        if (member_type == VarType::FLOAT) {
            // LDR D0, [X0, #offset]
            emit(Encoder::create_ldr_fp_imm("D0", "X0", offset, "Optimized accessor load (float)"));
        } else {
            // LDR X0, [X0, #offset] (Result overwrites X0)
            emit(Encoder::create_ldr_imm("X0", "X0", offset, "Optimized accessor load (int/ptr)"));
        }

        // 6. Return immediately
        emit(Encoder::create_return());

        // 7. Cleanup analyzer scope and exit early.
        analyzer_.set_current_function_scope(previous_analyzer_scope);
        return; // IMPORTANT: Skip the standard path.
    }
    // --- END TRIVIAL ACCESSOR OPTIMIZATION PATH ---

    // --- TRIVIAL SETTER OPTIMIZATION PATH (Fast Path) ---
    // Check the analyzer result using the finalized metrics_lookup_name.
    if (analyzer_.is_trivial_setter_method(metrics_lookup_name)) {
        debug_print("✅ Detected TRIVIAL SETTER method: '" + metrics_lookup_name + "'. Generating optimized code.");

        // 1. Define the entry point label
        instruction_stream_.define_label(entry_point_label);

        // 2. Get the member information from the analyzer metrics.
        auto metrics_it = analyzer_.get_function_metrics().find(metrics_lookup_name);
        if (metrics_it == analyzer_.get_function_metrics().end()) {
            debug_print("NewCodeGenerator Error: Function metrics not found for trivial setter: " + metrics_lookup_name);
            return;
        }
        const auto& metrics = metrics_it->second;
        const std::string& member_name = metrics.accessed_member_name;

        // 3. Determine the class context.
        std::string defining_class_name = current_class_name_;
        if (defining_class_name.empty()) {
            // Extract from the mangled name if current context is empty.
            size_t pos = metrics_lookup_name.find("::");
            if (pos != std::string::npos) {
                defining_class_name = metrics_lookup_name.substr(0, pos);
            }
        }

        if (defining_class_name.empty() || !class_table_) {
             throw std::runtime_error("Cannot generate optimized setter for '" + name + "' without class context or ClassTable.");
        }

        // 4. Look up the member offset and type.
        const ClassTableEntry* class_entry = class_table_->get_class(defining_class_name);
        if (!class_entry) {
             throw std::runtime_error("Class entry not found for optimized setter: " + defining_class_name);
        }

        auto member_it = class_entry->member_variables.find(member_name);
        if (member_it == class_entry->member_variables.end()) {
            throw std::runtime_error("Internal Error: Member '" + member_name + "' not found during optimized setter generation.");
        }

        // Assuming ClassMemberInfo structure is accessible.
        const ClassMemberInfo& member_info = member_it->second;
        size_t offset = member_info.offset;

        // 5. Generate the optimized assembly.
        // The '_this' pointer is in X0. Store X1 to [X0 + offset].
        emit(Encoder::create_str_imm("X1", "X0", offset, "Optimized setter store"));

        // 6. Return immediately
        emit(Encoder::create_return());

        // 7. Cleanup analyzer scope and exit early.
        analyzer_.set_current_function_scope(previous_analyzer_scope);
        return; // IMPORTANT: Skip the standard path.
    }
    // --- END TRIVIAL SETTER OPTIMIZATION PATH ---

    // Define the entry point label for the function/routine using the corrected label.
    instruction_stream_.define_label(entry_point_label);

    // Set up a new CallFrameManager for this function/routine.
    current_frame_manager_ = std::make_unique<CallFrameManager>(register_manager_, name, debug_enabled_);

    // Reserve callee-saved registers for temporary needs (before prologue generation)
    auto metrics_it = analyzer_.get_function_metrics().find(metrics_lookup_name);

    // --- ADD THIS DEBUG BLOCK ---
    if (debug_enabled_) {
        if (metrics_it != analyzer_.get_function_metrics().end()) {
            const auto& metrics = metrics_it->second;
            std::cerr << "[METRICS DEBUG] Found metrics for '" << metrics_lookup_name << "'." << std::endl;
            std::cerr << "  >> Parameter Count: " << metrics.num_parameters << std::endl;
            std::cerr << "  >> Local Var Count: " << metrics.num_variables << std::endl;
            std::cerr << "  >> Variable Types Map Size: " << metrics.variable_types.size() << std::endl;
        } else {
            std::cerr << "[METRICS DEBUG] FAILED to find metrics for '" << metrics_lookup_name << "'." << std::endl;
        }
    }
    // --- END DEBUG BLOCK ---

    if (metrics_it != analyzer_.get_function_metrics().end()) {
        int required_callee_saved_temps = metrics_it->second.required_callee_saved_temps;
        for (int i = 0; i < required_callee_saved_temps; ++i) {
            current_frame_manager_->force_save_register(
                register_manager_.get_callee_saved_temp_reg_name(i)
            );
        }
    }

    // Decide register pool based on global/runtime access and reset register manager accordingly.
    bool accesses_globals = analyzer_.function_accesses_globals(name);



    // Inform the CallFrameManager about the active pool.
    current_frame_manager_->set_active_register_pool(!accesses_globals); // Use extended pool if NOT accessing globals


    if (accesses_globals) {
        current_frame_manager_->setUsesGlobalPointers(true);
    }

    // --- NEW PIPELINE: Run register allocation before prologue ---
    // 1. Build live intervals and run register allocation
    LiveIntervalPass live_interval_pass;
    auto cfg_it = cfg_builder_.get_cfgs().find(metrics_lookup_name);
    if (cfg_it == cfg_builder_.get_cfgs().end()) {
        debug_print("NewCodeGenerator Error: CFG not found for: " + metrics_lookup_name);
        return;
    }
    live_interval_pass.run(*cfg_it->second, liveness_analyzer_, metrics_lookup_name);
    std::vector<LiveInterval> intervals = live_interval_pass.getIntervalsFor(metrics_lookup_name);
    
    // Filter out global variables from intervals before register allocation
    std::vector<LiveInterval> filtered_intervals;
    for (const auto& interval : intervals) {
        // Check if this is a global variable using the symbol table
        bool is_global_var = false;
        Symbol symbol;
        if (lookup_symbol(interval.var_name, symbol)) {
            is_global_var = symbol.is_global();
        }
        
        if (!is_global_var) {
            filtered_intervals.push_back(interval);
        } else {
            debug_print("Filtered out global variable from register allocation: " + interval.var_name);
        }
    }
    
    // 4. Inform the CallFrameManager about the parameters BEFORE generating the prologue.
    // Use analyzer's parameter_types to determine type for each parameter.
    auto function_metrics_it = analyzer_.get_function_metrics().find(metrics_lookup_name);
    const std::map<std::string, VarType>* param_types = nullptr;
    if (function_metrics_it != analyzer_.get_function_metrics().end()) {
        param_types = &function_metrics_it->second.parameter_types;
    }



    // --- FIX START: Add parameters as locals to the CallFrameManager ---
    debug_print("Registering all parameters as locals in CallFrameManager for '" + name + "'.");
    if (function_metrics_it != analyzer_.get_function_metrics().end()) {
        const auto& metrics = function_metrics_it->second;
        debug_print("DEBUG: metrics.variable_types size for '" + name + "': " + std::to_string(metrics.variable_types.size()));
        for (const auto& var_pair : metrics.variable_types) {
            debug_print("DEBUG: variable_types['" + var_pair.first + "'] = " + vartype_to_string(var_pair.second));
        }
        for (const auto& p_name : parameters) {
            // Check if the parameter has a type defined in the metrics
            auto type_it = metrics.variable_types.find(p_name);
            VarType p_type = (type_it != metrics.variable_types.end()) ? type_it->second : VarType::UNKNOWN;
            debug_print("DEBUG: Parameter '" + p_name + "' type from metrics: " + vartype_to_string(p_type));
            current_frame_manager_->add_local(p_name, p_type);
            debug_print("Added parameter '" + p_name + "' to CallFrameManager locals.");
        }
    }
    // --- FIX END ---

    // Now, add any other local variables discovered by the analyzer.
    debug_print("Registering all local variables from ASTAnalyzer metrics for '" + name + "'.");

    // --- FIX START: Add parameters as locals to the CallFrameManager ---
    if (function_metrics_it != analyzer_.get_function_metrics().end()) {
        const auto& metrics = function_metrics_it->second;
        for (const auto& p_name : parameters) {
            // Check if the parameter has a type defined in the metrics
            auto type_it = metrics.variable_types.find(p_name);
            VarType p_type = (type_it != metrics.variable_types.end()) ? type_it->second : VarType::UNKNOWN;
            
            // Add the parameter as a local to ensure it gets an offset
            current_frame_manager_->add_local(p_name, p_type);
            debug_print("Added parameter '" + p_name + "' to CallFrameManager locals.");
        }
    }
    // --- FIX END ---

    // Now, add any other local variables discovered by the analyzer.
    if (function_metrics_it != analyzer_.get_function_metrics().end()) {
        const auto& metrics = function_metrics_it->second;
        for (const auto& var_pair : metrics.variable_types) {
            const std::string& var_name = var_pair.first;
            const VarType& var_type = var_pair.second;

            // Check if this is a global variable - if so, don't treat it as a local
            Symbol symbol;
            bool is_global_var = false;
            if (lookup_symbol(var_name, symbol)) {
                is_global_var = symbol.is_global();
            }

            // Only add if not already registered as a local and not a global variable
            if (!current_frame_manager_->has_local(var_name) && !is_global_var) {
                current_frame_manager_->add_local(var_name, var_type);
                debug_print("Registered local '" + var_name + "' from analyzer metrics.");
            } else if (is_global_var) {
                debug_print("Skipped global variable '" + var_name + "' - not registering as local.");
            }
        }
    }
    // -- END OF NEW LOGIC --
    enter_scope();

    // --- USE PRE-COMPUTED REGISTER ALLOCATION RESULTS ---
    debug_print("Using pre-computed register allocation for '" + name + "'.");
    
    // Look up the pre-computed allocation results from the upfront allocation phase
    auto func_alloc_it = all_allocations_.find(name);
    if (func_alloc_it == all_allocations_.end()) {
        throw std::runtime_error("FATAL: No pre-computed register allocation found for function '" + name + "'.");
    }
    
    auto allocations = func_alloc_it->second;
    current_function_allocation_ = allocations;

    // Count actual spills and inform CallFrameManager
    int actual_spill_count = 0;
    for (const auto& [var_name, interval] : allocations) {
        if (interval.is_spilled) {
            actual_spill_count++;
            // Pre-allocate the spill slot so the frame size calculation is correct
            current_frame_manager_->get_spill_offset(var_name);
            debug_print("Pre-allocated spill slot for variable: " + var_name);
        }
    }
    
    if (actual_spill_count > 0) {
        debug_print("Pre-calculated " + std::to_string(actual_spill_count) + " spill slots for the stack frame.");
    } else {
        debug_print("No spills needed - all variables fit in registers.");
    }

    // Identify used callee-saved registers from allocation results
    std::set<std::string> used_callee_saved;
    for (const auto& [var_name, interval] : allocations) {
        if (!interval.is_spilled && RegisterManager::is_callee_saved(interval.assigned_register)) {
            used_callee_saved.insert(interval.assigned_register);
        }
    }

    // Configure CallFrameManager to save only those registers
    for (const auto& reg : used_callee_saved) {
        current_frame_manager_->force_save_register(reg);
    }

    // --- Reserve callee-saved registers based on register pressure ---
    int max_live = 0;
    auto& metrics_map = analyzer_.get_function_metrics();
    auto it = metrics_map.find(name);
    if (it != metrics_map.end()) {
        max_live = it->second.max_live_variables;
    }
    current_frame_manager_->reserve_registers_based_on_pressure(max_live);

    // --- ANALYZER-DRIVEN REGISTER RESERVATION ---
    // Use pre-computed register requirements from the analyzer
    if (!accesses_globals) {
        // Ask the register manager which callee-saved registers it ended up using.
        // This will include X19 or X28 if they were allocated.
        auto used_callee_regs = register_manager_.get_in_use_callee_saved_registers();
        for (const auto& reg : used_callee_regs) {
            // Explicitly tell the frame manager it MUST save and restore this register.
            current_frame_manager_->force_save_register(reg);
        }
    }

    // Check for analyzer-computed register requirements
    if (metrics_it != analyzer_.get_function_metrics().end()) {
        const auto& metrics = metrics_it->second;
        
        // Force save specific registers identified by the analyzer
        for (const auto& reg : metrics.required_callee_saved_regs) {
            current_frame_manager_->force_save_register(reg);
            debug_print("Analyzer-driven register reservation: forcing save of " + reg);
        }
        
        // If the function has call-preserving expressions, ensure we have temp registers
        if (metrics.has_call_preserving_expressions && metrics.required_callee_saved_temps > 0) {
            debug_print("Function has call-preserving expressions - reserving " + 
                       std::to_string(metrics.required_callee_saved_temps) + " callee-saved temp registers");
            
            // Reserve additional temp registers if needed
            std::vector<std::string> temp_regs = {"X20", "X21", "X22", "X23", "X24", "X25"};
            for (int i = 0; i < metrics.required_callee_saved_temps && i < temp_regs.size(); ++i) {
                if (metrics.required_callee_saved_regs.find(temp_regs[i]) == metrics.required_callee_saved_regs.end()) {
                    current_frame_manager_->force_save_register(temp_regs[i]);
                    debug_print("Additional temp register reservation: forcing save of " + temp_regs[i]);
                }
            }
        }
    }

    // Now, when the prologue is generated, it will have the correct list of registers to save.
    debug_print("Attempting to generate prologue for '" + name + "'.");
    for (const auto& instr : current_frame_manager_->generate_prologue()) {
        emit(instr);
    }



    // --- NEW: Correctly handle incoming parameters using allocation plan ---
    debug_print("Mapping incoming parameters to home registers or stack slots as per allocation plan.");

    auto param_alloc_it = all_allocations_.find(metrics_lookup_name);
    if (param_alloc_it != all_allocations_.end()) {
        const auto& current_function_allocs = param_alloc_it->second;

        // We only need to do this for parameters passed in registers (first 8).
        for (size_t i = 0; i < parameters.size() && i < 8; ++i) {
            const std::string& param_name = parameters[i];

            // Find the allocation decision for this parameter.
            auto var_alloc_it = current_function_allocs.find(param_name);
            if (var_alloc_it == current_function_allocs.end()) {
                continue; // This parameter isn't used, so no allocation for it.
            }

            // --- FIX: For constructors (CREATE routines), skip MOVs from ABI registers to home registers ---
            const LiveInterval& allocation = var_alloc_it->second;
            VarType param_type = current_frame_manager_->get_variable_type(param_name);

            // Skip NOTUSED parameters - no need to move them from ABI registers
            if (param_type == VarType::NOTUSED) {
                debug_print("  Skipping NOTUSED parameter: " + param_name);
                continue;
            }

            // Determine the source argument register based on position and type.
            std::string arg_reg;
            if (param_type == VarType::FLOAT) {
                arg_reg = "D" + std::to_string(i);
            } else {
                arg_reg = "X" + std::to_string(i);
            }

            // If the parameter is spilled, do not move it here; it will be loaded from the stack when accessed.
            if (allocation.is_spilled || allocation.assigned_register.empty()) {
                continue;
            }

            // Parameter is assigned to a register: move to home register if needed
            const std::string& home_reg = allocation.assigned_register;
            if (home_reg == arg_reg) {
                debug_print("  Parameter '" + param_name + "' is already in its home register (" + home_reg + "). No MOV needed.");
                register_manager_.set_initialized(home_reg, true);
                continue;
            }
            debug_print("  Moving '" + param_name + "' from arg reg " + arg_reg + " to home reg " + home_reg + ".");
            if (param_type == VarType::FLOAT) {
                emit(Encoder::create_fmov_reg(home_reg, arg_reg));
            } else {
                std::string comment = "Move parameter '" + param_name + "' from " + arg_reg + " to " + home_reg;
                emit(Encoder::create_mov_reg_comment(home_reg, arg_reg, comment));
            }
            register_manager_.set_initialized(home_reg, true);
        }
    }

    // Update stack offsets for spilled variables now that the prologue has been generated
    update_spill_offsets();

    // Register all local variables in the current frame with the allocation system
    for (const auto& local_name : current_frame_manager_->get_local_variable_names()) {
        if (current_function_allocation_.find(local_name) == current_function_allocation_.end()) {
            // Create a new interval for this variable
            LiveInterval new_interval;
            new_interval.var_name = local_name;
            new_interval.start_point = 0;  // Conservative estimate
            new_interval.end_point = 1000; // Conservative estimate
            new_interval.is_spilled = true; // Default to spilled

            try {
                new_interval.stack_offset = current_frame_manager_->get_offset(local_name);
                current_function_allocation_[local_name] = new_interval;
                debug_print("Registered local variable '" + local_name + "' with the allocation system (spilled)");
            } catch (const std::runtime_error& e) {
                debug_print("WARNING: Could not get stack offset for local variable '" + local_name + "': " + std::string(e.what()));
            }
        }
    }




    // If this function accesses global variables, set up X28 as the global data base pointer.
    if (analyzer_.function_accesses_globals(name)) {
        if (is_jit_mode_) {
            if (data_segment_base_addr_ == 0) {
                throw std::runtime_error("JIT mode requires a valid data_segment_base_addr.");
            }
            emit(Encoder::create_movz_movk_jit_addr("X28", data_segment_base_addr_, "L__data_segment_base"));
            x28_is_loaded_in_current_function_ = true;
            debug_print("Emitted JIT address load sequence for global base pointer (X28).");
            register_manager_.set_initialized("X28", true);
        } else {
            // Static mode: emit ADRP + ADD for X28
            emit(Encoder::create_adrp("X28", "L__data_segment_base"));
            emit(Encoder::create_add_literal("X28", "X28", "L__data_segment_base"));
            x28_is_loaded_in_current_function_ = true;
            debug_print("Emitted ADRP+ADD sequence for global base pointer (X28) in static mode.");
            register_manager_.set_initialized("X28", true);
        }
    }

    // IMPORTANT: We've already stored parameters earlier, this is a duplicate.
    // Commenting out to avoid double-storing parameters which causes issues.
    /*
    for (size_t i = 0; i < parameters.size(); ++i) {
        const std::string& param_name = parameters[i];
        int offset = current_frame_manager_->get_offset(param_name);

        if (current_function_return_type_ == VarType::FLOAT) {
            std::string arg_reg = "D" + std::to_string(i);
            emit(Encoder::create_str_fp_imm(arg_reg, "X29", offset));
        } else {
            std::string arg_reg = "X" + std::to_string(i);
            emit(Encoder::create_str_imm(arg_reg, "X29", offset, param_name));
        }
    }
    */
    debug_print("Skipping duplicate parameter storage");
    debug_print(current_frame_manager_->display_frame_layout());

    // --- CFG-DRIVEN CODE GENERATION LOOP ---
    // Look up the CFG for this function
    const auto& cfgs = cfg_builder_.get_cfgs();

    // --- FIX: More consistent CFG lookup logic ---
    // Try lookups in a specific order to ensure we find the right CFG

    // First, dump all available CFGs for debugging
    debug_print("--- DEBUG: Available CFGs in the map ---");
    for (const auto& cfg_pair : cfgs) {
        debug_print("  Available CFG: '" + cfg_pair.first + "'");
    }
    debug_print("--- End of CFG list ---");

    // 1. First try with lookup_name, which is correctly qualified for method context
    auto cfg_it_lookup = cfgs.find(lookup_name);
    if (cfg_it_lookup != cfgs.end()) {
        debug_print("Found CFG using lookup_name: " + lookup_name);
    }

    // 2. If not found, try with the metrics_lookup_name we determined earlier
    else if (lookup_name != metrics_lookup_name) {
        debug_print("CFG not found with lookup_name '" + lookup_name + "', trying metrics_lookup_name '" + metrics_lookup_name + "'");
        cfg_it = cfgs.find(metrics_lookup_name);
        if (cfg_it != cfgs.end()) {
            debug_print("Found CFG using metrics_lookup_name: " + metrics_lookup_name);
        }
    }

    // 3. If still not found, try the original name
    if (cfg_it == cfgs.end()) {
        debug_print("CFG not found, trying original name '" + name + "'");
        cfg_it = cfgs.find(name);
        if (cfg_it != cfgs.end()) {
            debug_print("Found CFG using original name: " + name);
        }
    }

    // 4. Last resort - try with class prefix if name isn't already qualified
    if (cfg_it == cfgs.end() && !is_already_qualified && !current_class_name_.empty()) {
        std::string qualified_name = NameMangler::mangleMethod(current_class_name_, name);
        debug_print("CFG still not found, trying qualified name: " + qualified_name);
        cfg_it = cfgs.find(qualified_name);
    }
    // --- END OF FIX ---

    if (cfg_it == cfgs.end()) {
        throw std::runtime_error("Could not find CFG for function: " + name);
    }
    const ControlFlowGraph* cfg = cfg_it->second.get();

    // Create a sorted list of blocks for deterministic code output
    std::vector<BasicBlock*> blocks;
    for (const auto& pair : cfg->get_blocks()) {
        blocks.push_back(pair.second.get());
    }
    std::sort(blocks.begin(), blocks.end(), [](auto* a, auto* b) {
        // Entry block should always come first
        if (a->is_entry) return true;
        if (b->is_entry) return false;
        // Otherwise sort alphabetically by ID
        return a->id < b->id;
    });

    // --- MAIN CODE GENERATION LOOP ---
    for (BasicBlock* block : blocks) {
        // Every basic block starts with a label
        instruction_stream_.define_label(block->id);

        // Generate code for each statement within the block
        for (const auto& stmt : block->statements) {
            stmt->accept(*this);
        }

        // Generate the branching logic to connect this block to its successors
        generate_block_epilogue(block);
    }
    // --- END OF CFG-DRIVEN LOOP ---



    // --- START MODIFICATION: Insert Cleanup Logic ---
    debug_print("Performing end-of-function scope cleanup for '" + name + "'.");
    if (symbol_table_) {
        // Get all symbols for the function's top-level scope.
        // The scope level is managed by the Function/RoutineDeclaration visitors.
        auto symbols_in_scope = symbol_table_->getSymbolsInScope(symbol_table_->currentScopeLevel());

        // Iterate in reverse to clean up in the opposite order of declaration.
        for (auto it = symbols_in_scope.rbegin(); it != symbols_in_scope.rend(); ++it) {
            const Symbol& symbol = *it;
            // Only clean up local variables and parameters that own heap memory.
            if ((symbol.kind == SymbolKind::LOCAL_VAR || symbol.kind == SymbolKind::PARAMETER) && symbol.owns_heap_memory) {
                generate_cleanup_code_for_symbol(symbol);
            }
        }
    }
    // --- END MODIFICATION ---

    // --- Generate bounds error handler stub if needed ---
    if (bounds_checking_enabled_ && function_needs_bounds_error_handler_[name]) {
        std::string error_label = ".L_bounds_error_handler_" + name;
        debug_print("Defining bounds error handler label: " + error_label);
        instruction_stream_.define_label(error_label);
        
        // The bounds check should have left index in some register and length in another.
        // For simplicity, we'll reconstruct the values here since the exact registers
        // may vary. The vector pointer should still be available.
        
        // For now, set up basic error call with NULL variable name
        // X0 = variable name (NULL)
        for (const auto& instr : Encoder::create_movz_movk_abs64("X0", 0, "NULL variable name for bounds error")) {
            emit(instr);
        }
        
        // X1 = index (we'll use a placeholder value since exact preservation is complex)
        for (const auto& instr : Encoder::create_movz_movk_abs64("X1", 0, "Index placeholder")) {
            emit(instr);
        }
        
        // X2 = length (we'll use a placeholder value since exact preservation is complex)  
        for (const auto& instr : Encoder::create_movz_movk_abs64("X2", 0, "Length placeholder")) {
            emit(instr);
        }
        
        // Call the runtime error handler
        emit(Encoder::create_branch_with_link("BCPL_BOUNDS_ERROR"));
        
        // This should never return, but add a BRK just in case
        emit(Encoder::create_brk(0));
    }

    // --- NEW: Define the shared function exit point here ---
    debug_print("Defining epilogue label: " + current_function_epilogue_label_);
    instruction_stream_.define_label(current_function_epilogue_label_);

    debug_print("Attempting to generate epilogue for '" + name + "'.");
    emit(current_frame_manager_->generate_epilogue());

    // Mark global base pointers as not initialized in the epilogue
    register_manager_.set_initialized("X28", false);
    register_manager_.set_initialized("X19", false);

    // Exit the scope
    exit_scope();

    // Clean up the CallFrameManager unique_ptr.
    current_frame_manager_.reset();

    // --- RESTORE ANALYZER SCOPE ---
    analyzer_.set_current_function_scope(previous_analyzer_scope);
}

// --- CFG-driven codegen: block epilogue logic ---
void NewCodeGenerator::generate_block_epilogue(BasicBlock* block) {
    if (block->successors.empty()) {
        // This block ends with a RETURN, FINISH, etc. which should have already
        // emitted a branch to the main epilogue. If not, it's a fallthrough to the end.
        if (!block->ends_with_control_flow()) {
            emit(Encoder::create_branch_unconditional(current_function_epilogue_label_));
        }
        return;
    }

    else if (block->successors.size() == 1) {
        // Check if this is a block that ends with a LoopStatement
        const Statement* last_stmt = block->statements.empty() ? nullptr : block->statements.back().get();

        if (dynamic_cast<const LoopStatement*>(last_stmt)) {
            // Generate an unconditional branch to the loop's start (the successor)
            debug_print("Generating branch for LOOP statement based on CFG");
            if (debug_enabled_) {
                std::cerr << "DEBUG: LOOP codegen - Emitting branch from block " << block->id
                          << " to target " << block->successors[0]->id << "\n";
            }
            // Add a safeguard against infinite loops
            if (block->successors[0]->id == block->id) {
                debug_print("ERROR: Detected potential infinite loop - LOOP block pointing to itself");
                throw std::runtime_error("Potential infinite loop detected in block " + block->id);
            }
            emit(Encoder::create_branch_unconditional(block->successors[0]->id));
            return;
        }

        // Special handling for increment blocks to guard against infinite loops
        if (block->id.find("ForIncrement_") != std::string::npos) {
            // Verify increment block structure - it should have exactly one statement which is an assignment
            if (block->statements.size() != 1 || !dynamic_cast<const AssignmentStatement*>(block->statements[0].get())) {
                debug_print("WARNING: Increment block doesn't have expected structure. This could cause incorrect code generation.");
            }

            // Add a safeguard against infinite loops by checking that the successor isn't the same block
            if (block->successors[0]->id == block->id) {
                debug_print("ERROR: Detected potential infinite loop - increment block pointing to itself");
                throw std::runtime_error("Potential infinite loop detected in block " + block->id);
            }
        }

        // For all blocks with a single successor, emit an unconditional branch
        emit(Encoder::create_branch_unconditional(block->successors[0]->id));
        return;
    }

    else if (block->successors.size() == 2) {
        // Conditional branch. The last statement in the block determines the condition.
        const Statement* last_stmt = block->statements.empty() ? nullptr : block->statements.back().get();

        if (const auto* if_stmt = dynamic_cast<const IfStatement*>(last_stmt)) {
            // Handle IF statement: successors[0] is 'then' block, successors[1] is 'else' or join block.
            generate_expression_code(*if_stmt->condition);
            std::string cond_reg = expression_result_reg_;
            emit(Encoder::create_cmp_reg(cond_reg, "XZR")); // is condition false?
            register_manager_.release_register(cond_reg);

            // If condition is FALSE (equal to zero), jump to the join/else block (second successor).
            emit(Encoder::create_branch_conditional("EQ", block->successors[1]->id));
            // Otherwise, fall through to the unconditional branch to the THEN block (first successor).
            emit(Encoder::create_branch_unconditional(block->successors[0]->id));
        } else if (const auto* unless_stmt = dynamic_cast<const UnlessStatement*>(last_stmt)) {
            // Handle UNLESS statement: successors[0] is 'then' block (executed when condition is false),
            // successors[1] is join block (executed when condition is true).
            generate_expression_code(*unless_stmt->condition);
            std::string cond_reg = expression_result_reg_;
            emit(Encoder::create_cmp_reg(cond_reg, "XZR")); // is condition false?
            register_manager_.release_register(cond_reg);

            // If condition is TRUE (not equal to zero), jump to the second successor.
            emit(Encoder::create_branch_conditional("NE", block->successors[1]->id));
            // Otherwise, fall through to the unconditional branch to the first successor.
            emit(Encoder::create_branch_unconditional(block->successors[0]->id));
        } else if (const auto* test_stmt = dynamic_cast<const TestStatement*>(last_stmt)) {
            // Handle TEST statement: successors[0] is 'then' block, successors[1] is 'else' block.
            generate_expression_code(*test_stmt->condition);
            std::string cond_reg = expression_result_reg_;
            emit(Encoder::create_cmp_reg(cond_reg, "XZR")); // is condition false?
            register_manager_.release_register(cond_reg);

            // If condition is FALSE (equal to zero), jump to the second successor.
            emit(Encoder::create_branch_conditional("EQ", block->successors[1]->id));
            // Otherwise, fall through to the unconditional branch to the first successor.
            emit(Encoder::create_branch_unconditional(block->successors[0]->id));
        } else if (const auto* cond_branch_stmt = dynamic_cast<const ConditionalBranchStatement*>(last_stmt)) {
            // Handle ConditionalBranchStatement: this is already a low-level conditional branch
            // The ConditionalBranchStatement should have already been processed by its visitor
            // and emitted the appropriate conditional branch instruction.
            // For blocks with two successors, we need to emit the conditional branch here.
            generate_expression_code(*cond_branch_stmt->condition_expr);
            std::string cond_reg = expression_result_reg_;
            emit(Encoder::create_cmp_reg(cond_reg, "XZR"));
            register_manager_.release_register(cond_reg);

            // Emit conditional branch using the condition and target from ConditionalBranchStatement
            emit(Encoder::create_branch_conditional(cond_branch_stmt->condition, block->successors[1]->id));
            emit(Encoder::create_branch_unconditional(block->successors[0]->id));
        } else if (const auto* for_stmt = dynamic_cast<const ForStatement*>(last_stmt)) {
            // Handle ForStatement: evaluate loop condition (loop_var <= end_expr)
            // OPTIMIZATION: Use immediate values for constant expressions to reduce register pressure
            // Generate code to check: loop_variable <= end_expr
            // If true, continue to loop body (first successor)
            // If false, exit loop (second successor)

            if (debug_enabled_) {
                std::cerr << "DEBUG: Generating ForStatement condition check in block " << block->id << "\n";
                std::cerr << "DEBUG: Block has " << block->successors.size() << " successors\n";
                for (size_t i = 0; i < block->successors.size(); ++i) {
                    std::cerr << "DEBUG:   Successor[" << i << "]: " << block->successors[i]->id << "\n";
                }
                if (for_stmt->is_end_expr_constant) {
                    std::cerr << "DEBUG: OPTIMIZATION: Using immediate value for end expression: " << for_stmt->constant_end_value << "\n";
                }
            }

            // Load current loop variable value: use register if available, otherwise load from stack
            std::string loop_var_reg = register_manager_.acquire_scratch_reg(*this);
            auto alloc_it = current_function_allocation_.find(for_stmt->unique_loop_variable_name);
            if (alloc_it != current_function_allocation_.end() && !alloc_it->second.is_spilled && !alloc_it->second.assigned_register.empty()) {
                // Variable is in a register
                emit(Encoder::create_mov_reg(loop_var_reg, alloc_it->second.assigned_register));
            } else {
                // Variable is spilled, load from stack
                try {
                    int offset = current_frame_manager_->get_offset(for_stmt->unique_loop_variable_name);
                    emit(Encoder::create_ldr_imm(loop_var_reg, "X29", offset, for_stmt->unique_loop_variable_name));
                } catch (const std::exception& e) {
                    debug_print("ERROR: Loop variable not found in current frame: " + for_stmt->unique_loop_variable_name);
                    throw std::runtime_error("Loop variable not found: " + for_stmt->unique_loop_variable_name +
                                            " in block " + block->id);
                }
            }
            // Mark the loop variable register as initialized after loading
            register_manager_.set_initialized(loop_var_reg, true);

            // OPTIMIZATION: Use immediate value for constant end expressions
            if (for_stmt->is_end_expr_constant) {
                // ** ENHANCED LOGIC: Handle immediate range limitations **
                
                // Check if the constant fits into a 12-bit immediate for CMP
                if (for_stmt->constant_end_value >= 0 && for_stmt->constant_end_value <= 4095) {
                    // Small Constant Path: Use the existing efficient CMP immediate instruction
                    emit(Encoder::create_cmp_imm(loop_var_reg, for_stmt->constant_end_value));
                    if (debug_enabled_) {
                        std::cerr << "DEBUG: OPTIMIZATION: Generated CMP with immediate: " << for_stmt->constant_end_value << "\n";
                    }
                } else {
                    // Large Constant Path: Load the constant into a temporary register first
                    debug_print("OPTIMIZATION: End value > 4095. Loading into temp register for comparison.");
                    std::string temp_reg = register_manager_.acquire_scratch_reg(*this);
                    
                    // Emit MOVZ/MOVK sequence to load the large constant
                    emit(Encoder::create_movz_movk_abs64(temp_reg, for_stmt->constant_end_value, ""));
                    
                    // Now perform a register-to-register comparison
                    emit(Encoder::create_cmp_reg(loop_var_reg, temp_reg));
                    
                    // We are done with the temporary register
                    register_manager_.release_register(temp_reg);
                    
                    if (debug_enabled_) {
                        std::cerr << "DEBUG: OPTIMIZATION: Generated CMP with temp register for large constant: " << for_stmt->constant_end_value << "\n";
                    }
                }
            } else {
                // Evaluate end expression and compare with register
                try {
                    generate_expression_code(*for_stmt->end_expr);
                } catch (const std::exception& e) {
                    debug_print("ERROR: Failed to generate code for loop end expression: " + std::string(e.what()));
                    throw;
                }
                std::string end_reg = expression_result_reg_;

                // Compare loop_var with end_value
                emit(Encoder::create_cmp_reg(loop_var_reg, end_reg));
                // Note: end_reg will be released when the expression evaluation context ends
            }

            register_manager_.release_register(loop_var_reg);

            // Only generate comparison and branches; do NOT increment here.
            emit(Encoder::create_branch_conditional("GT", block->successors[1]->id));
            emit(Encoder::create_branch_unconditional(block->successors[0]->id));
        } else if (const auto* while_stmt = dynamic_cast<const WhileStatement*>(last_stmt)) {
            // Handle WhileStatement: evaluate loop condition
            // If true, continue to loop body (first successor)
            // If false, exit loop (second successor)

            generate_expression_code(*while_stmt->condition);
            std::string cond_reg = expression_result_reg_;
            emit(Encoder::create_cmp_reg(cond_reg, "XZR"));
            register_manager_.release_register(cond_reg);

            // If condition is FALSE (equal to zero), exit loop (branch to second successor)
            emit(Encoder::create_branch_conditional("EQ", block->successors[1]->id));
            // Otherwise, continue to loop body (first successor)
            emit(Encoder::create_branch_unconditional(block->successors[0]->id));
        } else if (const auto* until_stmt = dynamic_cast<const UntilStatement*>(last_stmt)) {
            // Handle UntilStatement: evaluate loop condition (opposite of while)
            // If false, continue to loop body (first successor)
            // If true, exit loop (second successor)

            generate_expression_code(*until_stmt->condition);
            std::string cond_reg = expression_result_reg_;
            emit(Encoder::create_cmp_reg(cond_reg, "XZR"));
            register_manager_.release_register(cond_reg);

            // If condition is TRUE (not equal to zero), exit loop (branch to second successor)
            emit(Encoder::create_branch_conditional("NE", block->successors[1]->id));
            // Otherwise, continue to loop body (first successor)
            emit(Encoder::create_branch_unconditional(block->successors[0]->id));
        } else if (const auto* repeat_stmt = dynamic_cast<const RepeatStatement*>(last_stmt)) {
            // Handle RepeatStatement with conditions (REPEAT...WHILE or REPEAT...UNTIL)
            if (repeat_stmt->loop_type == RepeatStatement::LoopType::RepeatWhile) {
                // REPEAT <body> WHILE <condition>
                // If true, loop back (first successor), if false, exit (second successor)
                generate_expression_code(*repeat_stmt->condition);
                std::string cond_reg = expression_result_reg_;
                emit(Encoder::create_cmp_reg(cond_reg, "XZR"));
                register_manager_.release_register(cond_reg);

                // If condition is FALSE, exit loop (branch to second successor)
                emit(Encoder::create_branch_conditional("EQ", block->successors[1]->id));
                // Otherwise, loop back (first successor)
                emit(Encoder::create_branch_unconditional(block->successors[0]->id));
            } else if (repeat_stmt->loop_type == RepeatStatement::LoopType::RepeatUntil) {
                // REPEAT <body> UNTIL <condition>
                // If true, exit (first successor), if false, loop back (second successor)
                generate_expression_code(*repeat_stmt->condition);
                std::string cond_reg = expression_result_reg_;
                emit(Encoder::create_cmp_reg(cond_reg, "XZR"));
                register_manager_.release_register(cond_reg);

                // For REPEAT UNTIL: condition TRUE -> exit loop, condition FALSE -> loop back
                // CFGBuilderPass adds edges in this order:
                // successors[0] = loop_exit (for TRUE condition)
                // successors[1] = loop_entry (for FALSE condition)

                // If condition is TRUE (not equal to zero), exit the loop (to successors[0])
                emit(Encoder::create_branch_conditional("NE", block->successors[0]->id));
                // Otherwise (if condition is FALSE), loop back (to successors[1])
                emit(Encoder::create_branch_unconditional(block->successors[1]->id));
            } else {
                // Simple REPEAT loop without condition (always loops back)
                emit(Encoder::create_branch_unconditional(block->successors[0]->id));
            }
        } else if (const auto* switchon = dynamic_cast<const SwitchonStatement*>(last_stmt)) {
            debug_print("Epilogue: Generating two-way branch for SwitchonStatement.");

            // 1. Evaluate the switch expression.
            generate_expression_code(*switchon->expression);
            std::string switch_reg = expression_result_reg_;

            // 2. For each CASE, emit a comparison and conditional branch.
            for (size_t i = 0; i < switchon->cases.size(); ++i) {
                const auto& case_stmt = switchon->cases[i];
                if (!case_stmt->resolved_constant_value.has_value()) {
                    throw std::runtime_error("CaseStatement missing resolved constant value during codegen.");
                }
                int64_t case_value = case_stmt->resolved_constant_value.value();

                emit(Encoder::create_cmp_imm(switch_reg, case_value));
                emit(Encoder::create_branch_conditional("EQ", block->successors[i]->id));
            }

            // 3. After checking all cases, branch to the appropriate block.
            if (switchon->default_case) {
                emit(Encoder::create_branch_unconditional(block->successors[switchon->cases.size()]->id));
            } else {
                emit(Encoder::create_branch_unconditional(block->successors.back()->id));
            }

            register_manager_.release_register(switch_reg);
            return; // Epilogue for this block is complete.
        } else {
            std::string error_msg = "Block has two successors but last statement is not a recognized conditional.";
            if (last_stmt) {
                error_msg += " Last statement type: " + std::to_string(static_cast<int>(last_stmt->getType()));
                error_msg += " (";
                switch (last_stmt->getType()) {
                    case ASTNode::NodeType::IfStmt: error_msg += "IfStmt"; break;
                    case ASTNode::NodeType::UnlessStmt: error_msg += "UnlessStmt"; break;
                    case ASTNode::NodeType::TestStmt: error_msg += "TestStmt"; break;
                    case ASTNode::NodeType::ConditionalBranchStmt: error_msg += "ConditionalBranchStmt"; break;
                    case ASTNode::NodeType::LabelTargetStmt: error_msg += "LabelTargetStmt"; break;
                    case ASTNode::NodeType::AssignmentStmt: error_msg += "AssignmentStmt"; break;
                    case ASTNode::NodeType::RoutineCallStmt: error_msg += "RoutineCallStmt"; break;
                    case ASTNode::NodeType::WhileStmt: error_msg += "WhileStmt"; break;
                    case ASTNode::NodeType::ForStmt: error_msg += "ForStmt"; break;
                    case ASTNode::NodeType::UntilStmt: error_msg += "UntilStmt"; break;
                    case ASTNode::NodeType::RepeatStmt: error_msg += "RepeatStmt"; break;
                    default: error_msg += "Unknown"; break;
                }
                error_msg += ")";
            } else {
                error_msg += " Block has no statements.";
            }
            throw std::runtime_error(error_msg);
        }
        return;
    }
    else if (block->successors.size() > 2) {
        // --- START OF THE DEFINITIVE FIX ---
        // The last statement in the block should be a SwitchonStatement
        const Statement* last_stmt = block->statements.empty() ? nullptr : block->statements.back().get();
        if (const auto* switchon = dynamic_cast<const SwitchonStatement*>(last_stmt)) {
            debug_print("Epilogue: Generating multi-way branch for SwitchonStatement.");

            // 1. Evaluate the switch expression. The result is in expression_result_reg_.
            generate_expression_code(*switchon->expression);
            std::string switch_reg = expression_result_reg_;

            // 2. For each CASE, emit a comparison and conditional branch.
            //    The CFG successor order must match the AST case order, which it does.
            for (size_t i = 0; i < switchon->cases.size(); ++i) {
                const auto& case_stmt = switchon->cases[i];
                if (!case_stmt->resolved_constant_value.has_value()) {
                    throw std::runtime_error("CaseStatement missing resolved constant value during codegen.");
                }
                int64_t case_value = case_stmt->resolved_constant_value.value();

                // Compare the value in switch_reg with the case's constant value.
                emit(Encoder::create_cmp_imm(switch_reg, case_value));

                // If equal, branch to the corresponding case block (successors[i]).
                emit(Encoder::create_branch_conditional("EQ", block->successors[i]->id));
            }

            // 3. After checking all cases, branch to the appropriate block.
            if (switchon->default_case) {
                // If a DEFAULT case exists, branch to its block.
                // In the CFG, the DEFAULT block is the successor after all CASEs.
                emit(Encoder::create_branch_unconditional(block->successors[switchon->cases.size()]->id));
            } else {
                // If no DEFAULT case, branch to the final JOIN block.
                // The JOIN block is always the last successor.
                emit(Encoder::create_branch_unconditional(block->successors.back()->id));
            }

            register_manager_.release_register(switch_reg);
            return; // Epilogue for this block is complete.
        }
        // --- END OF THE DEFINITIVE FIX ---

        // fallback: error if not a SwitchonStatement
        std::string error_msg = "Block with >2 successors expected to end with SwitchonStatement, but got ";
        if (last_stmt) {
            error_msg += std::to_string(static_cast<int>(last_stmt->getType()));
        } else {
            error_msg += "no statement";
        }
        throw std::runtime_error(error_msg);
    }
}


// --- ASTVisitor Implementations ---
// These are declared in NewCodeGenerator.h but their implementations
// are located in separate files (e.g., generators/gen_Program.cpp).

// CFG-driven: IfStatement visitor only evaluates condition, branching is handled by block epilogue.
void NewCodeGenerator::visit(IfStatement& node) {
    debug_print("Visiting IfStatement node (NOTE: branching is handled by block epilogue).");
    // No branching logic here; only evaluate condition if needed elsewhere.
}

void NewCodeGenerator::visit(GlobalVariableDeclaration& node) {
    debug_print("Visiting GlobalVariableDeclaration node for: " + node.names[0]);
    for (size_t i = 0; i < node.names.size(); ++i) {
        const auto& name = node.names[i];
        const auto* initializer_ptr = (i < node.initializers.size()) ? node.initializers[i].get() : nullptr;

        // Add the global variable to the data generator to reserve space
        // and store its initial value.
        if (initializer_ptr) {
            data_generator_.add_global_variable(name, clone_unique_ptr(node.initializers[i]));
        } else {
            data_generator_.add_global_variable(name, nullptr);
        }
        debug_print("Registered global variable '" + name + "' with the DataGenerator.");
    }
}

// --- At-Risk Parameter Detection ---
// Identifies parameters that are passed in registers but used after function calls,
// making them vulnerable to register corruption by nested calls.
std::vector<NewCodeGenerator::AtRiskParameterInfo> NewCodeGenerator::find_at_risk_parameters(
    const std::string& function_name, 
    const std::vector<std::string>& parameters, 
    ASTNode& body_node
) {
    std::vector<AtRiskParameterInfo> at_risk_params;
    
    debug_print("Analyzing at-risk parameters for function: " + function_name);
    
    // Get function metrics to check for function calls
    auto metrics_it = analyzer_.get_function_metrics().find(function_name);
    
    if (metrics_it == analyzer_.get_function_metrics().end()) {
        debug_print("No metrics found for function: " + function_name);
        return at_risk_params;
    }
    
    const auto& metrics = metrics_it->second;
    
    // If this function doesn't make any calls, no parameters are at risk
    int total_calls = metrics.num_runtime_calls + metrics.num_local_function_calls + metrics.num_local_routine_calls;
    if (total_calls == 0) {
        debug_print("Function makes no calls - no parameters at risk");
        return at_risk_params;
    }
    
    // Check each parameter (only first 8 are passed in registers)
    for (size_t i = 0; i < parameters.size() && i < 8; ++i) {
        const std::string& param_name = parameters[i];
        
        // Get parameter type
        VarType param_type = VarType::INTEGER; // Default
        auto type_it = metrics.parameter_types.find(param_name);
        if (type_it != metrics.parameter_types.end()) {
            param_type = type_it->second;
        }
        
        // Skip NOTUSED parameters
        if (param_type == VarType::NOTUSED) {
            continue;
        }
        
        // Determine the ABI register for this parameter
        std::string abi_register;
        if (param_type == VarType::FLOAT) {
            abi_register = "D" + std::to_string(i);
        } else {
            abi_register = "X" + std::to_string(i);
        }
        
        // Check if this parameter is used after the first function call
        // For now, we'll be conservative and assume any parameter used in a function
        // with calls is at risk. A more sophisticated analysis could check the
        // actual usage patterns.
        bool is_used_after_call = true; // Conservative assumption
        
        if (is_used_after_call) {
            AtRiskParameterInfo info;
            info.name = param_name;
            info.abi_register = abi_register;
            info.type = param_type;
            
            at_risk_params.push_back(info);
            debug_print("Parameter '" + param_name + "' is at risk (in " + abi_register + ")");
        }
    }
    
    debug_print("Found " + std::to_string(at_risk_params.size()) + " at-risk parameters");
    return at_risk_params;
}


