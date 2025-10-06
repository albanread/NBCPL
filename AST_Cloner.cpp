#include "AST.h"

// --- Helper Functions for Cloning ---

/**
 * @brief  Clones a vector of unique_ptrs of any ASTNode-derived type.
 */
template <typename T>
std::vector<std::unique_ptr<T>> clone_vector(const std::vector<std::unique_ptr<T>>& original_vec) {
    std::vector<std::unique_ptr<T>> cloned_vec;
    cloned_vec.reserve(original_vec.size());
    for (const auto& item : original_vec) {
        if (item) {
            cloned_vec.push_back(std::unique_ptr<T>(static_cast<T*>(item->clone().release())));
        } else {
            cloned_vec.push_back(nullptr);
        }
    }
    return cloned_vec;
}

/**
 * @brief  Clones a single unique_ptr of any ASTNode-derived type.
 */
template <typename T>
std::unique_ptr<T> clone_unique_ptr(const std::unique_ptr<T>& original_ptr) {
    if (original_ptr) {
        return std::unique_ptr<T>(static_cast<T*>(original_ptr->clone().release()));
    }
    return nullptr;
}

ASTNodePtr SuperMethodAccessExpression::clone() const {
    return std::make_unique<SuperMethodAccessExpression>(member_name);
}

ASTNodePtr SuperMethodCallExpression::clone() const {
    return std::make_unique<SuperMethodCallExpression>(member_name, clone_vector(arguments));
}



// --- AST Node Clone Method Implementations ---

ASTNodePtr Program::clone() const {
    auto cloned_program = std::make_unique<Program>();
    cloned_program->declarations = clone_vector(declarations);
    cloned_program->statements = clone_vector(statements);
    return cloned_program;
}

// --- Declarations ---
ASTNodePtr LetDeclaration::clone() const {
    auto cloned = std::make_unique<LetDeclaration>(names, clone_vector(initializers));
    cloned->is_float_declaration = this->is_float_declaration;
    cloned->explicit_type = this->explicit_type;
    cloned->is_retained = this->is_retained;
    return cloned;
}

ASTNodePtr ManifestDeclaration::clone() const {
    return std::make_unique<ManifestDeclaration>(name, value);
}

ASTNodePtr StaticDeclaration::clone() const {
    auto cloned = std::make_unique<StaticDeclaration>(name, clone_unique_ptr(initializer));
    cloned->is_float_declaration = this->is_float_declaration;
    return cloned;
}

ASTNodePtr GlobalDeclaration::clone() const {
    return std::make_unique<GlobalDeclaration>(globals);
}

ASTNodePtr GlobalVariableDeclaration::clone() const {
    return std::make_unique<GlobalVariableDeclaration>(names, clone_vector(initializers));
}

ASTNodePtr ClassDeclaration::clone() const {
    std::vector<ClassMember> cloned_members;
    for (const auto& member : members) {
        cloned_members.emplace_back(
            std::unique_ptr<Declaration>(static_cast<Declaration*>(member.declaration->clone().release())),
            member.visibility
        );
    }
    return std::make_unique<ClassDeclaration>(name, parent_class_name, std::move(cloned_members));
}

ASTNodePtr FunctionDeclaration::clone() const {
    return std::make_unique<FunctionDeclaration>(name, parameters, clone_unique_ptr(body));
}

ASTNodePtr RoutineDeclaration::clone() const {
    return std::make_unique<RoutineDeclaration>(name, parameters, clone_unique_ptr(body));
}

ASTNodePtr LabelDeclaration::clone() const {
    return std::make_unique<LabelDeclaration>(name, clone_unique_ptr(command));
}



// --- Expressions ---
ASTNodePtr NumberLiteral::clone() const {
    if (literal_type == LiteralType::Integer) {
        return std::make_unique<NumberLiteral>(int_value);
    } else {
        return std::make_unique<NumberLiteral>(float_value);
    }
}

ASTNodePtr StringLiteral::clone() const {
    return std::make_unique<StringLiteral>(value);
}

ASTNodePtr CharLiteral::clone() const {
    return std::make_unique<CharLiteral>(value);
}

ASTNodePtr BooleanLiteral::clone() const {
    return std::make_unique<BooleanLiteral>(value);
}

ASTNodePtr NullLiteral::clone() const {
    return std::make_unique<NullLiteral>();
}

ASTNodePtr VariableAccess::clone() const {
    return std::make_unique<VariableAccess>(name);
}

ASTNodePtr BinaryOp::clone() const {
    return std::make_unique<BinaryOp>(op, clone_unique_ptr(left), clone_unique_ptr(right));
}

ASTNodePtr UnaryOp::clone() const {
    return std::make_unique<UnaryOp>(op, clone_unique_ptr(operand));
}

ASTNodePtr VectorAccess::clone() const {
    return std::make_unique<VectorAccess>(clone_unique_ptr(vector_expr), clone_unique_ptr(index_expr));
}

ASTNodePtr CharIndirection::clone() const {
    return std::make_unique<CharIndirection>(clone_unique_ptr(string_expr), clone_unique_ptr(index_expr));
}

ASTNodePtr FloatVectorIndirection::clone() const {
    return std::make_unique<FloatVectorIndirection>(clone_unique_ptr(vector_expr), clone_unique_ptr(index_expr));
}

ASTNodePtr FunctionCall::clone() const {
    return std::make_unique<FunctionCall>(clone_unique_ptr(function_expr), clone_vector(arguments));
}

ASTNodePtr SysCall::clone() const {
    return std::make_unique<SysCall>(
        this->function_name,
        clone_unique_ptr(this->syscall_number),
        clone_vector(this->arguments)
    );
}

ASTNodePtr BitfieldAccessExpression::clone() const {
    return std::make_unique<BitfieldAccessExpression>(
        clone_unique_ptr(base_expr),
        clone_unique_ptr(start_bit_expr),
        clone_unique_ptr(width_expr)
    );
}

ASTNodePtr ConditionalExpression::clone() const {
    return std::make_unique<ConditionalExpression>(clone_unique_ptr(condition), clone_unique_ptr(true_expr), clone_unique_ptr(false_expr));
}

ASTNodePtr ValofExpression::clone() const {
    auto cloned = std::make_unique<ValofExpression>(clone_unique_ptr(body));
    cloned->explicit_return_type = this->explicit_return_type;
    return cloned;
}

ASTNodePtr FloatValofExpression::clone() const {
    return std::make_unique<FloatValofExpression>(clone_unique_ptr(body));
}

ASTNodePtr VecAllocationExpression::clone() const {
    return std::make_unique<VecAllocationExpression>(clone_unique_ptr(size_expr));
}

ASTNodePtr FVecAllocationExpression::clone() const {
    return std::make_unique<FVecAllocationExpression>(clone_unique_ptr(size_expr));
}

ASTNodePtr PairsAllocationExpression::clone() const {
    return std::make_unique<PairsAllocationExpression>(clone_unique_ptr(size_expr));
}

ASTNodePtr FPairsAllocationExpression::clone() const {
    return std::make_unique<FPairsAllocationExpression>(clone_unique_ptr(size_expr));
}

ASTNodePtr StringAllocationExpression::clone() const {
    return std::make_unique<StringAllocationExpression>(clone_unique_ptr(size_expr));
}

ASTNodePtr TableExpression::clone() const {
    return std::make_unique<TableExpression>(clone_vector(initializers), is_float_table);
}

ASTNodePtr ListExpression::clone() const {
    return std::make_unique<ListExpression>(clone_vector(initializers), is_manifest);
}

ASTNodePtr NewExpression::clone() const {
    return std::make_unique<NewExpression>(class_name, clone_vector(constructor_arguments), assigned_variable_name);
}

ASTNodePtr MemberAccessExpression::clone() const {
    return std::make_unique<MemberAccessExpression>(clone_unique_ptr(object_expr), member_name);
}

ASTNodePtr VecInitializerExpression::clone() const {
    return std::make_unique<VecInitializerExpression>(clone_vector(initializers));
}

// --- Statements ---
ASTNodePtr AssignmentStatement::clone() const {
    return std::make_unique<AssignmentStatement>(clone_vector(lhs), clone_vector(rhs));
}

ASTNodePtr RoutineCallStatement::clone() const {
    return std::make_unique<RoutineCallStatement>(clone_unique_ptr(routine_expr), clone_vector(arguments));
}

ASTNodePtr IfStatement::clone() const {
    return std::make_unique<IfStatement>(clone_unique_ptr(condition), clone_unique_ptr(then_branch));
}

ASTNodePtr UnlessStatement::clone() const {
    return std::make_unique<UnlessStatement>(clone_unique_ptr(condition), clone_unique_ptr(then_branch));
}

ASTNodePtr TestStatement::clone() const {
    return std::make_unique<TestStatement>(clone_unique_ptr(condition), clone_unique_ptr(then_branch), clone_unique_ptr(else_branch));
}

ASTNodePtr WhileStatement::clone() const {
    return std::make_unique<WhileStatement>(clone_unique_ptr(condition), clone_unique_ptr(body));
}

ASTNodePtr UntilStatement::clone() const {
    return std::make_unique<UntilStatement>(clone_unique_ptr(condition), clone_unique_ptr(body));
}

ASTNodePtr RepeatStatement::clone() const {
    return std::make_unique<RepeatStatement>(loop_type, clone_unique_ptr(body), clone_unique_ptr(condition));
}

ASTNodePtr ForStatement::clone() const {
    auto cloned = std::make_unique<ForStatement>(loop_variable, clone_unique_ptr(start_expr), clone_unique_ptr(end_expr), clone_unique_ptr(body), clone_unique_ptr(step_expr));
    cloned->unique_loop_variable_name = unique_loop_variable_name;
    cloned->unique_step_variable_name = unique_step_variable_name;
    cloned->unique_end_variable_name = unique_end_variable_name;
    return cloned;
}

ASTNodePtr ForEachStatement::clone() const {
    return std::make_unique<ForEachStatement>(
        loop_variable_name,
        type_variable_name,
        clone_unique_ptr(collection_expression),
        clone_unique_ptr(body),
        filter_type,
        is_destructuring
    );
}

ASTNodePtr SwitchonStatement::clone() const {
    std::vector<std::unique_ptr<CaseStatement>> cloned_cases;
    cloned_cases.reserve(cases.size());
    for (const auto& case_stmt : cases) {
        if (case_stmt) {
            cloned_cases.push_back(std::unique_ptr<CaseStatement>(static_cast<CaseStatement*>(case_stmt->clone().release())));
        } else {
            cloned_cases.push_back(nullptr);
        }
    }
    std::unique_ptr<DefaultStatement> cloned_default_case = clone_unique_ptr(default_case);
    return std::make_unique<SwitchonStatement>(
        clone_unique_ptr(expression),
        std::move(cloned_cases),
        std::move(cloned_default_case)
    );
}

ASTNodePtr CaseStatement::clone() const {
    auto cloned_case = std::make_unique<CaseStatement>(
        clone_unique_ptr(constant_expr),
        clone_unique_ptr(command)
    );
    cloned_case->resolved_constant_value = resolved_constant_value;
    return cloned_case;
}

ASTNodePtr DefaultStatement::clone() const {
    return std::make_unique<DefaultStatement>(clone_unique_ptr(command));
}

ASTNodePtr GotoStatement::clone() const {
    return std::make_unique<GotoStatement>(clone_unique_ptr(label_expr));
}

ASTNodePtr ReturnStatement::clone() const {
    return std::make_unique<ReturnStatement>();
}

ASTNodePtr FinishStatement::clone() const {
    return std::make_unique<FinishStatement>();
}

ASTNodePtr BreakStatement::clone() const {
    return std::make_unique<BreakStatement>();
}

ASTNodePtr BrkStatement::clone() const {
    return std::make_unique<BrkStatement>();
}

ASTNodePtr LoopStatement::clone() const {
    return std::make_unique<LoopStatement>();
}

ASTNodePtr EndcaseStatement::clone() const {
    return std::make_unique<EndcaseStatement>();
}

ASTNodePtr ResultisStatement::clone() const {
    return std::make_unique<ResultisStatement>(clone_unique_ptr(expression), is_send);
}

ASTNodePtr FreeStatement::clone() const {
    return std::make_unique<FreeStatement>(clone_unique_ptr(list_expr));
}

ASTNodePtr CompoundStatement::clone() const {
    return std::make_unique<CompoundStatement>(clone_vector(statements));
}

ASTNodePtr BlockStatement::clone() const {
    return std::make_unique<BlockStatement>(clone_vector(declarations), clone_vector(statements));
}

ASTNodePtr StringStatement::clone() const {
    return std::make_unique<StringStatement>(clone_unique_ptr(size_expr));
}

ASTNodePtr LabelTargetStatement::clone() const {
    return std::make_unique<LabelTargetStatement>(labelName);
}

ASTNodePtr ConditionalBranchStatement::clone() const {
    return std::make_unique<ConditionalBranchStatement>(condition, targetLabel, clone_unique_ptr(condition_expr));
}

ASTNodePtr DeferStatement::clone() const {
    return std::make_unique<DeferStatement>(clone_unique_ptr(deferred_statement));
}
