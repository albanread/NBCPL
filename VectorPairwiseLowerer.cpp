#include "VectorPairwiseLowerer.h"
#include "analysis/ASTAnalyzer.h"
#include <iostream>
#include <sstream>

VectorPairwiseLowerer::VectorPairwiseLowerer(SymbolTable* symbol_table, ASTAnalyzer* analyzer, bool enable_debug)
    : symbol_table_(symbol_table), analyzer_(analyzer), current_function_name_(""), debug_enabled_(enable_debug), transformations_made_(false), temp_var_counter_(0) {
}

bool VectorPairwiseLowerer::lower(Program& program) {
    transformations_made_ = false;
    temp_var_counter_ = 0;
    
    debugPrint("Starting vector pairwise lowering pass...");
    debugPrint("Debug enabled: " + std::string(debug_enabled_ ? "true" : "false"));
    
    program.accept(*this);
    
    if (transformations_made_) {
        debugPrint("Vector lowering completed - transformations were made");
    } else {
        debugPrint("Vector lowering completed - no transformations needed");
    }
    
    return transformations_made_;
}

void VectorPairwiseLowerer::visit(Program& node) {
    debugPrint("visit(Program): processing " + std::to_string(node.declarations.size()) + " declarations");
    for (auto& decl : node.declarations) {
        if (decl) {
            debugPrint("visit(Program): processing declaration of type " + std::to_string(static_cast<int>(decl->getType())));
            decl->accept(*this);
        }
    }
    debugPrint("visit(Program): completed");
}

void VectorPairwiseLowerer::visit(FunctionDeclaration& node) {
    debugPrint("visit(FunctionDeclaration): processing function " + node.name);
    setCurrentFunction(node.name);
    if (node.body) {
        node.body->accept(*this);
    }
}

void VectorPairwiseLowerer::visit(RoutineDeclaration& node) {
    debugPrint("visit(RoutineDeclaration): processing routine " + node.name);
    setCurrentFunction(node.name);
    if (node.body) {
        node.body->accept(*this);
    } else {
        debugPrint("visit(RoutineDeclaration): routine has no body");
    }
}

void VectorPairwiseLowerer::visit(BlockStatement& node) {
    transformStatements(node.statements);
}

void VectorPairwiseLowerer::visit(AssignmentStatement& node) {
    // Transform RHS expressions that might contain vector operations
    for (auto& rhs_expr : node.rhs) {
        if (rhs_expr) {
            transformExpression(rhs_expr);
        }
    }
}

void VectorPairwiseLowerer::visit(IfStatement& node) {
    if (node.condition) {
        transformExpression(node.condition);
    }
    if (node.then_branch) {
        node.then_branch->accept(*this);
    }
}

void VectorPairwiseLowerer::visit(UnlessStatement& node) {
    if (node.condition) {
        transformExpression(node.condition);
    }
    if (node.then_branch) {
        node.then_branch->accept(*this);
    }
}

void VectorPairwiseLowerer::visit(WhileStatement& node) {
    if (node.condition) {
        transformExpression(node.condition);
    }
    if (node.body) {
        node.body->accept(*this);
    }
}

void VectorPairwiseLowerer::visit(UntilStatement& node) {
    if (node.condition) {
        transformExpression(node.condition);
    }
    if (node.body) {
        node.body->accept(*this);
    }
}

void VectorPairwiseLowerer::visit(RepeatStatement& node) {
    if (node.body) {
        node.body->accept(*this);
    }
}

void VectorPairwiseLowerer::visit(ForStatement& node) {
    if (node.start_expr) {
        transformExpression(node.start_expr);
    }
    if (node.end_expr) {
        transformExpression(node.end_expr);
    }
    if (node.step_expr) {
        transformExpression(node.step_expr);
    }
    if (node.body) {
        node.body->accept(*this);
    }
}

void VectorPairwiseLowerer::visit(ValofExpression& node) {
    if (node.body) {
        node.body->accept(*this);
    }
}

void VectorPairwiseLowerer::visit(FloatValofExpression& node) {
    if (node.body) {
        node.body->accept(*this);
    }
}

void VectorPairwiseLowerer::visit(ConditionalExpression& node) {
    if (node.condition) {
        transformExpression(node.condition);
    }
    if (node.true_expr) {
        transformExpression(node.true_expr);
    }
    if (node.false_expr) {
        transformExpression(node.false_expr);
    }
}

bool VectorPairwiseLowerer::isVectorOperation(const BinaryOp* binary_op) const {
    if (!binary_op || !binary_op->left || !binary_op->right) {
        debugPrint("isVectorOperation: binary_op is null or missing operands");
        return false;
    }
    
    // Check if operation is arithmetic
    if (binary_op->op != BinaryOp::Operator::Add &&
        binary_op->op != BinaryOp::Operator::Subtract &&
        binary_op->op != BinaryOp::Operator::Multiply &&
        binary_op->op != BinaryOp::Operator::Divide) {
        debugPrint("isVectorOperation: not an arithmetic operation");
        return false;
    }
    
    // Use ASTAnalyzer to infer types
    ASTAnalyzer& analyzer = ASTAnalyzer::getInstance();
    VarType left_type = analyzer.infer_expression_type(binary_op->left.get());
    VarType right_type = analyzer.infer_expression_type(binary_op->right.get());
    
    debugPrint("isVectorOperation: left_type=" + vartype_to_string(left_type) + 
               ", right_type=" + vartype_to_string(right_type));
    
    // Check for vector operations - handle both direct vector types and pointer-to-vector types
    bool is_vector = (left_type == VarType::PAIRS && right_type == VarType::PAIRS) ||
                     (left_type == VarType::FPAIRS && right_type == VarType::FPAIRS) ||
                     (left_type == VarType::QUADS && right_type == VarType::QUADS) ||
                     (left_type == VarType::FQUADS && right_type == VarType::FQUADS) ||
                     // Handle pointer-to-vector types (e.g., POINTER_TO_PAIRS)
                     (left_type == VarType::POINTER_TO_PAIRS && right_type == VarType::POINTER_TO_PAIRS) ||
                     (left_type == VarType::POINTER_TO_FPAIRS && right_type == VarType::POINTER_TO_FPAIRS) ||
                     (left_type == VarType::POINTER_TO_QUADS && right_type == VarType::POINTER_TO_QUADS) ||
                     (left_type == VarType::POINTER_TO_FQUADS && right_type == VarType::POINTER_TO_FQUADS);
    
    debugPrint("isVectorOperation: result=" + std::string(is_vector ? "true" : "false"));
    return is_vector;
}

VarType VectorPairwiseLowerer::getElementType(VarType vector_type) const {
    switch (vector_type) {
        case VarType::PAIRS: return VarType::PAIR;
        case VarType::FPAIRS: return VarType::FPAIR;
        case VarType::QUADS: return VarType::QUAD;
        case VarType::FQUADS: return VarType::FQUAD;
        // Handle pointer-to-vector types
        case VarType::POINTER_TO_PAIRS: return VarType::PAIR;
        case VarType::POINTER_TO_FPAIRS: return VarType::FPAIR;
        case VarType::POINTER_TO_QUADS: return VarType::QUAD;
        case VarType::POINTER_TO_FQUADS: return VarType::FQUAD;
        default: return VarType::UNKNOWN;
    }
}

std::string VectorPairwiseLowerer::getAllocationExpressionType(VarType vector_type) const {
    switch (vector_type) {
        case VarType::PAIRS: return "PAIRS";
        case VarType::FPAIRS: return "FPAIRS";
        case VarType::QUADS: return "QUADS";
        case VarType::FQUADS: return "FQUADS";
        // Handle pointer-to-vector types
        case VarType::POINTER_TO_PAIRS: return "PAIRS";
        case VarType::POINTER_TO_FPAIRS: return "FPAIRS";
        case VarType::POINTER_TO_QUADS: return "QUADS";
        case VarType::POINTER_TO_FQUADS: return "FQUADS";
        default: return "VEC";
    }
}

std::unique_ptr<BlockStatement> VectorPairwiseLowerer::lowerVectorOperation(const BinaryOp* binary_op, const std::string& dest_var_name) {
    // Get operand information - use the analyzer instance passed to constructor
    VarType left_type = analyzer_->infer_expression_type(binary_op->left.get());
    VarType result_type = left_type; // Vector operations preserve type
    
    // For pointer-to-vector types, we need to extract the base vector type for allocation
    VarType base_vector_type = left_type;
    if (left_type == VarType::POINTER_TO_PAIRS) {
        base_vector_type = VarType::PAIRS;
    } else if (left_type == VarType::POINTER_TO_FPAIRS) {
        base_vector_type = VarType::FPAIRS;
    } else if (left_type == VarType::POINTER_TO_QUADS) {
        base_vector_type = VarType::QUADS;
    } else if (left_type == VarType::POINTER_TO_FQUADS) {
        base_vector_type = VarType::FQUADS;
    }
    
    // Generate unique temporary variable names
    std::string loop_var_name = createTempVarName("__vec_i");
    std::string length_var_name = createTempVarName("__vec_len");
    
    // Register the loop variable in symbol table since we manage it ourselves with WHILE loop
    registerTempVariable(loop_var_name, VarType::INTEGER);
    
    // Create the block that will contain our lowered code
    auto lowered_block = std::make_unique<BlockStatement>(
        std::vector<DeclPtr>{}, std::vector<StmtPtr>{}
    );
    
    // Step 1: Get vector length - LET __vec_len = LEN(left_operand)
    std::vector<ExprPtr> length_lhs;
    length_lhs.push_back(std::make_unique<VariableAccess>(length_var_name));
    
    std::vector<ExprPtr> length_rhs;
    
    // Create LEN function call instead of UnaryOp::LengthOf
    std::vector<ExprPtr> len_args;
    len_args.push_back(std::unique_ptr<Expression>(static_cast<Expression*>(binary_op->left->clone().release())));
    
    length_rhs.push_back(std::make_unique<FunctionCall>(
        std::make_unique<VariableAccess>("LEN"),
        std::move(len_args)
    ));
    
    // Create the length assignment statement
    auto length_assignment = std::make_unique<AssignmentStatement>(
        std::move(length_lhs), std::move(length_rhs)
    );
    
    // Register the length variable in symbol table
    registerTempVariable(length_var_name, VarType::INTEGER);
    
    // Analyze the length assignment statement immediately after creation
    length_assignment->accept(*analyzer_);
    
    lowered_block->statements.push_back(std::move(length_assignment));
    
    // Step 2: Allocate destination vector with correct length - LET dest_var = PAIRS(__vec_len)
    std::vector<ExprPtr> dest_lhs;
    dest_lhs.push_back(std::make_unique<VariableAccess>(dest_var_name));
    
    std::vector<ExprPtr> dest_rhs;
    std::string alloc_type = getAllocationExpressionType(base_vector_type);
    
    // Create the appropriate allocation expression
    if (alloc_type == "PAIRS") {
        dest_rhs.push_back(std::make_unique<PairsAllocationExpression>(
            std::make_unique<VariableAccess>(length_var_name)
        ));
    } else if (alloc_type == "FPAIRS") {
        dest_rhs.push_back(std::make_unique<FPairsAllocationExpression>(
            std::make_unique<VariableAccess>(length_var_name)
        ));
    } else if (alloc_type == "QUADS") {
        dest_rhs.push_back(std::make_unique<QuadsAllocationExpression>(
            std::make_unique<VariableAccess>(length_var_name)
        ));
    } else if (alloc_type == "FQUADS") {
        dest_rhs.push_back(std::make_unique<FQuadsAllocationExpression>(
            std::make_unique<VariableAccess>(length_var_name)
        ));
    } else {
        // For other vector types, use VEC allocation
        dest_rhs.push_back(std::make_unique<VecAllocationExpression>(
            std::make_unique<VariableAccess>(length_var_name)
        ));
    }
    
    // Create the destination allocation assignment statement
    auto dest_assignment = std::make_unique<AssignmentStatement>(
        std::move(dest_lhs), std::move(dest_rhs)
    );
    
    // Analyze the destination assignment statement immediately after creation
    dest_assignment->accept(*analyzer_);
    
    lowered_block->statements.push_back(std::move(dest_assignment));
    
    // Step 3: Create optimized loops for 128-bit NEON operations
    // For PAIRS vectors, process 2 elements at a time (128-bit) when possible
    
    if (base_vector_type == VarType::PAIRS || base_vector_type == VarType::POINTER_TO_PAIRS || 
        base_vector_type == VarType::FPAIRS || base_vector_type == VarType::POINTER_TO_FPAIRS) {
        // Generate 128-bit optimized loop for pairs of elements
        
        // Main loop: Process 2 elements at a time using 128-bit NEON (for even lengths >= 2)
        auto main_loop_body = std::make_unique<BlockStatement>(
            std::vector<DeclPtr>{}, std::vector<StmtPtr>{}
        );
        
        // Extract vector names for the specialized 128-bit NEON operation
        std::string left_vector_name = "UNKNOWN_LEFT";
        std::string right_vector_name = "UNKNOWN_RIGHT";
        
        // Extract variable names from the binary operation operands
        if (auto left_var = dynamic_cast<VariableAccess*>(binary_op->left.get())) {
            left_vector_name = left_var->name;
        }
        if (auto right_var = dynamic_cast<VariableAccess*>(binary_op->right.get())) {
            right_vector_name = right_var->name;
        }
        // Create appropriate 128-bit NEON operation statement based on vector type
        // This will be recognized by the code generator and emit optimized NEON instructions
        // We manage the loop variable ourselves with WHILE loop infrastructure
        std::unique_ptr<Statement> neon_pair_op;
        
        if (base_vector_type == VarType::FPAIRS || base_vector_type == VarType::POINTER_TO_FPAIRS) {
            // Generate floating point NEON operation for FPAIRS
            debugPrint("Generating Neon128BitFPairOpStatement for FPAIRS vector operation");
            neon_pair_op = std::make_unique<Neon128BitFPairOpStatement>(
                dest_var_name,           // destination vector
                left_vector_name,        // left operand vector  
                right_vector_name,       // right operand vector
                loop_var_name,           // loop index variable we manage
                static_cast<int>(binary_op->op)  // operation type (Add, Subtract, Multiply, etc.)
            );
        } else if (base_vector_type == VarType::FQUADS || base_vector_type == VarType::POINTER_TO_FQUADS) {
            // Generate floating point NEON operation for FQUADS
            debugPrint("Generating Neon128BitFQuadOpStatement for FQUADS vector operation");
            neon_pair_op = std::make_unique<Neon128BitFQuadOpStatement>(
                dest_var_name,           // destination vector
                left_vector_name,        // left operand vector  
                right_vector_name,       // right operand vector
                loop_var_name,           // loop index variable we manage
                static_cast<int>(binary_op->op)  // operation type (Add, Subtract, Multiply, etc.)
            );
        } else if (base_vector_type == VarType::QUADS || base_vector_type == VarType::POINTER_TO_QUADS) {
            // Generate integer NEON operation for QUADS
            debugPrint("Generating Neon128BitQuadOpStatement for QUADS vector operation");
            neon_pair_op = std::make_unique<Neon128BitQuadOpStatement>(
                dest_var_name,           // destination vector
                left_vector_name,        // left operand vector  
                right_vector_name,       // right operand vector
                loop_var_name,           // loop index variable we manage
                static_cast<int>(binary_op->op)  // operation type (Add, Subtract, Multiply, etc.)
            );
        } else {
            // Generate integer NEON operation for PAIRS (default)
            debugPrint("Generating Neon128BitPairOpStatement for PAIRS vector operation");
            neon_pair_op = std::make_unique<Neon128BitPairOpStatement>(
                dest_var_name,           // destination vector
                left_vector_name,        // left operand vector  
                right_vector_name,       // right operand vector
                loop_var_name,           // loop index variable we manage
                static_cast<int>(binary_op->op)  // operation type (Add, Subtract, Multiply, etc.)
            );
        }
        
        // NEON operation will be added to loop body after loop variable initialization
        
        // Initialize loop variable: __vec_i = 0
        std::vector<ExprPtr> loop_init_lhs;
        loop_init_lhs.push_back(std::make_unique<VariableAccess>(loop_var_name));
        std::vector<ExprPtr> loop_init_rhs;
        loop_init_rhs.push_back(std::make_unique<NumberLiteral>(static_cast<int64_t>(0)));
        auto loop_init = std::make_unique<AssignmentStatement>(
            std::move(loop_init_lhs), std::move(loop_init_rhs), "loop_init"
        );
        loop_init->accept(*analyzer_);
        lowered_block->statements.push_back(std::move(loop_init));
        
        // Add NEON operation to loop body
        main_loop_body->statements.push_back(std::move(neon_pair_op));
        
        // Add loop increment: __vec_i = __vec_i + 2
        std::vector<ExprPtr> loop_incr_lhs;
        loop_incr_lhs.push_back(std::make_unique<VariableAccess>(loop_var_name));
        std::vector<ExprPtr> loop_incr_rhs;
        loop_incr_rhs.push_back(std::make_unique<BinaryOp>(
            BinaryOp::Operator::Add,
            std::make_unique<VariableAccess>(loop_var_name),
            std::make_unique<NumberLiteral>(static_cast<int64_t>(2))
        ));
        auto loop_incr = std::make_unique<AssignmentStatement>(
            std::move(loop_incr_lhs), std::move(loop_incr_rhs), "loop_increment"
        );
        main_loop_body->statements.push_back(std::move(loop_incr));
        
        // Main loop: WHILE __vec_i <= __vec_len-2 DO (process 2 elements at a time)
        auto loop_condition = std::make_unique<BinaryOp>(
            BinaryOp::Operator::LessEqual,
            std::make_unique<VariableAccess>(loop_var_name),
            std::make_unique<BinaryOp>(
                BinaryOp::Operator::Subtract,
                std::make_unique<VariableAccess>(length_var_name),
                std::make_unique<NumberLiteral>(static_cast<int64_t>(2))
            )
        );
        
        auto main_while_loop = std::make_unique<WhileStatement>(
            std::move(loop_condition),
            std::move(main_loop_body)
        );
        
        main_while_loop->accept(*analyzer_);
        lowered_block->statements.push_back(std::move(main_while_loop));
        
        // Cleanup loop: Handle the last element if vector length is odd
        // IF (__vec_len & 1) == 1 THEN dest_var[__vec_len-1] = left[__vec_len-1] op right[__vec_len-1]
        
        // Condition: (__vec_len & 1) == 1
        auto odd_check = std::make_unique<BinaryOp>(
            BinaryOp::Operator::Equal,
            std::make_unique<BinaryOp>(
                BinaryOp::Operator::BitwiseAnd,
                std::make_unique<VariableAccess>(length_var_name),
                std::make_unique<NumberLiteral>(static_cast<int64_t>(1))
            ),
            std::make_unique<NumberLiteral>(static_cast<int64_t>(1))
        );
        
        // Cleanup assignment for the last odd element
        std::vector<ExprPtr> cleanup_lhs;
        cleanup_lhs.push_back(std::make_unique<VectorAccess>(
            std::make_unique<VariableAccess>(dest_var_name),
            std::make_unique<BinaryOp>(
                BinaryOp::Operator::Subtract,
                std::make_unique<VariableAccess>(length_var_name),
                std::make_unique<NumberLiteral>(static_cast<int64_t>(1))
            )
        ));
        
        std::vector<ExprPtr> cleanup_rhs;
        cleanup_rhs.push_back(std::make_unique<BinaryOp>(
            binary_op->op,
            std::make_unique<VectorAccess>(
                std::unique_ptr<Expression>(static_cast<Expression*>(binary_op->left->clone().release())),
                std::make_unique<BinaryOp>(
                    BinaryOp::Operator::Subtract,
                    std::make_unique<VariableAccess>(length_var_name),
                    std::make_unique<NumberLiteral>(static_cast<int64_t>(1))
                )
            ),
            std::make_unique<VectorAccess>(
                std::unique_ptr<Expression>(static_cast<Expression*>(binary_op->right->clone().release())),
                std::make_unique<BinaryOp>(
                    BinaryOp::Operator::Subtract,
                    std::make_unique<VariableAccess>(length_var_name),
                    std::make_unique<NumberLiteral>(static_cast<int64_t>(1))
                )
            )
        ));
        
        auto cleanup_assignment = std::make_unique<AssignmentStatement>(
            std::move(cleanup_lhs), std::move(cleanup_rhs)
        );
        cleanup_assignment->accept(*analyzer_);
        
        auto cleanup_body = std::make_unique<BlockStatement>(
            std::vector<DeclPtr>{}, std::vector<StmtPtr>{}
        );
        cleanup_body->statements.push_back(std::move(cleanup_assignment));
        
        auto cleanup_if = std::make_unique<IfStatement>(
            std::move(odd_check),
            std::move(cleanup_body)
        );
        
        cleanup_if->accept(*analyzer_);
        lowered_block->statements.push_back(std::move(cleanup_if));
        
    } else {
        // For non-PAIRS vectors, use the original single-element loop
        auto loop_body = std::make_unique<BlockStatement>(
            std::vector<DeclPtr>{}, std::vector<StmtPtr>{}
        );
        
        // Loop body: dest_var[__vec_i] = left[__vec_i] op right[__vec_i]
        std::vector<ExprPtr> loop_assign_lhs;
        loop_assign_lhs.push_back(std::make_unique<VectorAccess>(
            std::make_unique<VariableAccess>(dest_var_name),
            std::make_unique<VariableAccess>(loop_var_name)
        ));
        
        std::vector<ExprPtr> loop_assign_rhs;
        loop_assign_rhs.push_back(std::make_unique<BinaryOp>(
            binary_op->op,
            std::make_unique<VectorAccess>(
                std::unique_ptr<Expression>(static_cast<Expression*>(binary_op->left->clone().release())),
                std::make_unique<VariableAccess>(loop_var_name)
            ),
            std::make_unique<VectorAccess>(
                std::unique_ptr<Expression>(static_cast<Expression*>(binary_op->right->clone().release())),
                std::make_unique<VariableAccess>(loop_var_name)
            )
        ));
        
        // Create the loop body assignment
        auto loop_assignment = std::make_unique<AssignmentStatement>(
            std::move(loop_assign_lhs), std::move(loop_assign_rhs)
        );
        loop_assignment->accept(*analyzer_);
        loop_body->statements.push_back(std::move(loop_assignment));
        
        // Create the FOR loop: FOR __vec_i = 0 TO __vec_len - 1 (BCPL vectors are 0-indexed)
        auto for_loop = std::make_unique<ForStatement>(
            loop_var_name,
            std::make_unique<NumberLiteral>(static_cast<int64_t>(0)), // start: 0
            std::make_unique<BinaryOp>(                                // end: __vec_len - 1
                BinaryOp::Operator::Subtract,
                std::make_unique<VariableAccess>(length_var_name),
                std::make_unique<NumberLiteral>(static_cast<int64_t>(1))
            ),
            std::move(loop_body),
            nullptr  // step (default to 1)
        );
        
        for_loop->accept(*analyzer_);
        lowered_block->statements.push_back(std::move(for_loop));
    }
    
    // Analyze the entire lowered block to ensure all nested nodes are properly analyzed
    lowered_block->accept(*analyzer_);
    
    debugPrint("Lowered vector " + vartype_to_string(left_type) + " operation to loop writing directly to destination: " + dest_var_name);
    
    return lowered_block;
}

std::string VectorPairwiseLowerer::createTempVarName(const std::string& prefix) {
    return prefix + "_" + std::to_string(temp_var_counter_++);
}

bool VectorPairwiseLowerer::transformExpression(ExprPtr& expr) {
    if (!expr) {
        debugPrint("transformExpression: expr is null");
        return false;
    }
    
    debugPrint("transformExpression: examining expression type " + std::to_string(static_cast<int>(expr->getType())));
    
    // Check if this expression is a vector operation
    if (auto* binary_op = dynamic_cast<BinaryOp*>(expr.get())) {
        debugPrint("transformExpression: found BinaryOp, checking if it's a vector operation");
        if (isVectorOperation(binary_op)) {
            debugPrint("Found vector operation to lower: " + 
                      vartype_to_string(ASTAnalyzer::getInstance().infer_expression_type(binary_op->left.get())) +
                      " + " + 
                      vartype_to_string(ASTAnalyzer::getInstance().infer_expression_type(binary_op->right.get())));
            
            // This is tricky - we need to replace the expression with a reference to the result
            // of a lowered block. For now, we'll mark it and handle it at the statement level.
            // In a real implementation, this would require more sophisticated AST transformation.
            
            // For Stage 3, we'll implement a simplified approach where we detect vector operations
            // in assignment statements and replace the entire assignment with the lowered block.
            return true;
        } else {
            debugPrint("transformExpression: BinaryOp is not a vector operation");
        }
    } else {
        debugPrint("transformExpression: not a BinaryOp");
    }
    
    // Recursively check child expressions
    bool transformed = false;
    
    if (auto* binary_op = dynamic_cast<BinaryOp*>(expr.get())) {
        transformed |= transformExpression(binary_op->left);
        transformed |= transformExpression(binary_op->right);
    } else if (auto* unary_op = dynamic_cast<UnaryOp*>(expr.get())) {
        transformed |= transformExpression(unary_op->operand);
    } else if (auto* cond_expr = dynamic_cast<ConditionalExpression*>(expr.get())) {
        transformed |= transformExpression(cond_expr->condition);
        transformed |= transformExpression(cond_expr->true_expr);
        transformed |= transformExpression(cond_expr->false_expr);
    }
    // Add more expression types as needed
    
    return transformed;
}

bool VectorPairwiseLowerer::transformStatements(std::vector<StmtPtr>& statements) {
    bool transformed = false;
    
    debugPrint("transformStatements: processing " + std::to_string(statements.size()) + " statements");
    
    for (size_t i = 0; i < statements.size(); ++i) {
        auto& stmt = statements[i];
        if (!stmt) {
            debugPrint("transformStatements: statement " + std::to_string(i) + " is null");
            continue;
        }
        
        debugPrint("transformStatements: examining statement " + std::to_string(i) + 
                   " of type " + std::to_string(static_cast<int>(stmt->getType())));
        
        // Check if this is an assignment with a vector operation on the RHS
        if (auto* assign_stmt = dynamic_cast<AssignmentStatement*>(stmt.get())) {
            debugPrint("transformStatements: found AssignmentStatement with " + 
                       std::to_string(assign_stmt->rhs.size()) + " RHS expressions");
            
            for (size_t j = 0; j < assign_stmt->rhs.size(); ++j) {
                auto& rhs_expr = assign_stmt->rhs[j];
                debugPrint("transformStatements: examining RHS expression " + std::to_string(j));
                
                if (auto* binary_op = dynamic_cast<BinaryOp*>(rhs_expr.get())) {
                    debugPrint("transformStatements: found BinaryOp in RHS");
                    if (isVectorOperation(binary_op)) {
                        debugPrint("Transforming assignment with vector operation");
                        
                        // Get the destination variable name from LHS
                        std::string dest_var_name;
                        if (!assign_stmt->lhs.empty()) {
                            if (auto* var_access = dynamic_cast<VariableAccess*>(assign_stmt->lhs[0].get())) {
                                dest_var_name = var_access->name;
                                debugPrint("Destination variable: " + dest_var_name);
                            } else {
                                debugPrint("ERROR: LHS is not a simple variable access!");
                                continue;
                            }
                        } else {
                            debugPrint("ERROR: No LHS variables found!");
                            continue;
                        }
                        
                        // Generate the lowered block that writes directly to the destination
                        auto lowered_block = lowerVectorOperation(binary_op, dest_var_name);
                        
                        // Replace the assignment statement with the lowered block
                        statements[i] = std::move(lowered_block);
                        
                        transformed = true;
                        transformations_made_ = true;
                        break; // Only handle one vector operation per assignment for now
                    }
                }
            }
        }
        
        // Recursively visit child statements
        stmt->accept(*this);
    }
    
    return transformed;
}

void VectorPairwiseLowerer::debugPrint(const std::string& message) const {
    if (debug_enabled_) {
        std::cout << "[VectorPairwiseLowerer] " << message << std::endl;
    }
}

void VectorPairwiseLowerer::registerTempVariable(const std::string& var_name, VarType var_type) {
    if (symbol_table_) {
        Symbol temp_symbol(
            var_name,
            SymbolKind::LOCAL_VAR,
            var_type,
            symbol_table_->currentScopeLevel(),
            current_function_name_
        );
        
        if (symbol_table_->addSymbol(temp_symbol)) {
            debugPrint("Registered temporary variable: " + var_name + " (type: " + vartype_to_string(var_type) + ")");
        } else {
            debugPrint("Failed to register temporary variable: " + var_name);
        }
    } else {
        debugPrint("Warning: No symbol table available for registering " + var_name);
    }
}

void VectorPairwiseLowerer::setCurrentFunction(const std::string& function_name) {
    current_function_name_ = function_name;
    debugPrint("Set current function context: " + function_name);
}