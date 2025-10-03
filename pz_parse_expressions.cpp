#include "Parser.h"
#include <stdexcept>
#include <vector>
#include "DataTypes.h"

// --- Type Annotation Syntax Helper ---
// Parses [POINTER TO] (LIST | VEC) OF <base_type> | <base_type>
VarType Parser::parse_type_specifier() {
    VarType final_type = VarType::UNKNOWN;

    // 1. Check for optional "^"
    if (match(TokenType::Pointer)) {
        final_type = static_cast<VarType>(static_cast<int64_t>(final_type) | static_cast<int64_t>(VarType::POINTER_TO));
        // DO NOT return here. Continue parsing LIST/VEC/OF and the base type.
    }

    // 2. Check for container type "LIST" or "VEC"
    if (match(TokenType::List)) {
        final_type = static_cast<VarType>(static_cast<int64_t>(final_type) | static_cast<int64_t>(VarType::LIST));
        consume(TokenType::Of, "Expect 'OF' after 'LIST'.");
    } else if (match(TokenType::Vec)) {
        final_type = static_cast<VarType>(static_cast<int64_t>(final_type) | static_cast<int64_t>(VarType::VEC));
        consume(TokenType::Of, "Expect 'OF' after 'VEC'.");
    }

    // 3. Parse the mandatory base type (which can be an identifier or a keyword).
    if (match(TokenType::Identifier)) {
        std::string type_keyword = previous_token_.value;
        std::transform(type_keyword.begin(), type_keyword.end(), type_keyword.begin(), ::toupper);

        if (type_keyword == "INTEGER") {
            final_type = static_cast<VarType>(static_cast<int64_t>(final_type) | static_cast<int64_t>(VarType::INTEGER));
        } else if (type_keyword == "FLOAT") {
            final_type = static_cast<VarType>(static_cast<int64_t>(final_type) | static_cast<int64_t>(VarType::FLOAT));
        } else if (type_keyword == "PAIR") {
            final_type = static_cast<VarType>(static_cast<int64_t>(final_type) | static_cast<int64_t>(VarType::PAIR));
        } else if (type_keyword == "FPAIR") {
            final_type = static_cast<VarType>(static_cast<int64_t>(final_type) | static_cast<int64_t>(VarType::FPAIR));
        } else if (type_keyword == "STRING") {
            final_type = static_cast<VarType>(static_cast<int64_t>(final_type) | static_cast<int64_t>(VarType::STRING));
        } else if (type_keyword == "ANY") {
            final_type = static_cast<VarType>(static_cast<int64_t>(final_type) | static_cast<int64_t>(VarType::ANY));
        } else {
            // Assume it's a class name for legitimate AS clauses (e.g., LET p AS Point = ...)
            // The semantic analyzer will validate if the class exists later
            final_type = static_cast<VarType>(static_cast<int64_t>(final_type) | static_cast<int64_t>(VarType::OBJECT));
            // Note: We need a way to store the class name for semantic analysis
        }
    } else if (match(TokenType::String)) { // <-- ADD THIS CASE
        // Specifically handle the STRING keyword, which is not a generic identifier.
        final_type = static_cast<VarType>(static_cast<int64_t>(final_type) | static_cast<int64_t>(VarType::STRING));
    } else {
        error("Expected a base type identifier (e.g., INTEGER) or type keyword (e.g., STRING).");
        return VarType::UNKNOWN;
    }

    return final_type;
}

// --- Definition for SuperMethodCallExpression destructor to fix vtable error ---

#include "DataTypes.h"

/**
 * @brief Maps a token type to its corresponding BinaryOp::Operator enum.
 */
static BinaryOp::Operator to_binary_op(TokenType type) {
    switch (type) {
        case TokenType::Plus:           return BinaryOp::Operator::Add;
        case TokenType::Minus:          return BinaryOp::Operator::Subtract;
        case TokenType::Multiply:       return BinaryOp::Operator::Multiply;
        case TokenType::Divide:         return BinaryOp::Operator::Divide;
        case TokenType::Remainder:      return BinaryOp::Operator::Remainder;
        case TokenType::Equal:          return BinaryOp::Operator::Equal;
        case TokenType::NotEqual:       return BinaryOp::Operator::NotEqual;
        case TokenType::Less:           return BinaryOp::Operator::Less;
        case TokenType::LessEqual:      return BinaryOp::Operator::LessEqual;
        case TokenType::Greater:        return BinaryOp::Operator::Greater;
        case TokenType::GreaterEqual:   return BinaryOp::Operator::GreaterEqual;
        case TokenType::LogicalAnd:     return BinaryOp::Operator::LogicalAnd;
        case TokenType::BitwiseAnd:     return BinaryOp::Operator::BitwiseAnd;
        case TokenType::LogicalOr:      return BinaryOp::Operator::LogicalOr;
        case TokenType::BitwiseOr:      return BinaryOp::Operator::BitwiseOr;
        case TokenType::Equivalence:    return BinaryOp::Operator::Equivalence;
        case TokenType::NotEquivalence: return BinaryOp::Operator::NotEquivalence;
        case TokenType::LeftShift:      return BinaryOp::Operator::LeftShift;
        case TokenType::RightShift:     return BinaryOp::Operator::RightShift;

        default:
            throw std::logic_error("Invalid token type for a binary operator.");
    }
}

/**
 * @brief Returns the binding power (precedence) of a token. Higher numbers mean higher precedence.
 */
int Parser::get_token_precedence(TokenType type) {
    switch (type) {
        case TokenType::Conditional:        return 1;
        case TokenType::LogicalOr:          return 2;
        case TokenType::BitwiseOr:          return 3;
        case TokenType::LogicalAnd:         return 3;
        case TokenType::Equivalence:
        case TokenType::NotEquivalence:     return 4;
        case TokenType::Equal: case TokenType::NotEqual:
        case TokenType::Less:  case TokenType::LessEqual:
        case TokenType::Greater: case TokenType::GreaterEqual:

            return 5;
        case TokenType::LeftShift: case TokenType::RightShift:
            return 6;
        case TokenType::Plus: case TokenType::Minus:
            return 7;
        case TokenType::Multiply: case TokenType::Divide: case TokenType::Remainder:
            return 8;
        // Postfix operators have the highest precedence.
        case TokenType::LParen:
        case TokenType::VecIndirection:
        case TokenType::CharIndirection:
        case TokenType::FloatVecIndirection:
        case TokenType::Bitfield: // Bitfield operator has high precedence
        case TokenType::Dot: // Member access operator '.'
            return 9;
        default:
            return 0; // Not an operator
    }
}

/**
 * @brief Parses an expression using a Pratt parser (precedence climbing) algorithm.
 * @param precedence The current precedence level.
 */
ExprPtr Parser::parse_expression(int precedence) {
    TraceGuard guard(*this, "parse_expression");
    ExprPtr left;

    // --- Prefix Operators ---
    if (match(TokenType::BitwiseNot)) {
        left = std::make_unique<UnaryOp>(UnaryOp::Operator::BitwiseNot, parse_expression(8));
    } else if (match(TokenType::LogicalNot)) {
        left = std::make_unique<UnaryOp>(UnaryOp::Operator::LogicalNot, parse_expression(8));
    } else if (match(TokenType::AddressOf)) {
        left = std::make_unique<UnaryOp>(UnaryOp::Operator::AddressOf, parse_expression(8));
    } else if (match(TokenType::Indirection)) {
        left = std::make_unique<UnaryOp>(UnaryOp::Operator::Indirection, parse_expression(8));
    } else if (match(TokenType::Minus)) {
        left = std::make_unique<UnaryOp>(UnaryOp::Operator::Negate, parse_expression(8));
    } else if (match(TokenType::HD)) { // ADDED
        left = std::make_unique<UnaryOp>(UnaryOp::Operator::HeadOf, parse_expression(8));
    } else if (match(TokenType::TL)) { // ADDED
        left = std::make_unique<UnaryOp>(UnaryOp::Operator::TailOf, parse_expression(8));
    } else if (match(TokenType::REST)) { // ADDED
        left = std::make_unique<UnaryOp>(UnaryOp::Operator::TailOfNonDestructive, parse_expression(8));
    } else if (match(TokenType::FLOAT)) {
        left = std::make_unique<UnaryOp>(UnaryOp::Operator::FloatConvert, parse_expression(8));
    } else if (match(TokenType::FIX)) {
        left = std::make_unique<UnaryOp>(UnaryOp::Operator::IntegerConvert, parse_expression(8));
    } else {
        left = parse_primary_expression();
    }

    // --- Infix and Postfix Operators ---
    while (left && precedence < get_token_precedence(current_token_.type)) {
        TokenType type = current_token_.type;
        advance();

        if (type == TokenType::LParen) { // Function Call
            std::vector<ExprPtr> args;
            if (!check(TokenType::RParen)) {
                args.push_back(parse_expression());
                while (match(TokenType::Comma)) {
                    if (check(TokenType::RParen)) {
                        break; // Exit the loop gracefully for trailing comma.
                    }
                    args.push_back(parse_expression());
                }
            }
            consume(TokenType::RParen, "Expect ')' after function arguments.");
            left = std::make_unique<FunctionCall>(std::move(left), std::move(args));
        } else if (type == TokenType::VecIndirection) { // Vector Access
            left = std::make_unique<VectorAccess>(std::move(left), parse_expression(9));
        } else if (type == TokenType::CharIndirection) { // Character Indirection
            left = std::make_unique<CharIndirection>(std::move(left), parse_expression(9));
        } else if (type == TokenType::FloatVecIndirection) { // Float Vector Indirection
            left = std::make_unique<FloatVectorIndirection>(std::move(left), parse_expression(9));
        } else if (type == TokenType::Dot) { // Member Access '.' or Pair Access
            std::string member_name = current_token_.value;
            consume(TokenType::Identifier, "Expect member name after '.' operator.");
            
            // Special handling for component access on packed types
            if (member_name == "first" || member_name == "second" || member_name == "third" || member_name == "fourth") {
                // Use QuadAccessExpression for all component access - it can handle both PAIR and QUAD
                // Semantic analysis will validate that only valid components are accessed for each type
                QuadAccessExpression::AccessType access_type;
                if (member_name == "first") access_type = QuadAccessExpression::FIRST;
                else if (member_name == "second") access_type = QuadAccessExpression::SECOND;
                else if (member_name == "third") access_type = QuadAccessExpression::THIRD;
                else access_type = QuadAccessExpression::FOURTH;
                left = std::make_unique<QuadAccessExpression>(std::move(left), access_type);
            } else {
                // Regular member access
                auto member_access_expr = std::make_unique<MemberAccessExpression>(std::move(left), member_name);

                if (match(TokenType::LParen)) {
                    std::vector<ExprPtr> args;
                    if (!check(TokenType::RParen)) {
                        args.push_back(parse_expression());
                        while (match(TokenType::Comma)) {
                            if (check(TokenType::RParen)) {
                                break; // Exit the loop gracefully for trailing comma.
                            }
                            args.push_back(parse_expression());
                        }
                    }
                    consume(TokenType::RParen, "Expect ')' after method arguments.");
                    left = std::make_unique<FunctionCall>(std::move(member_access_expr), std::move(args));
                } else {
                    left = std::move(member_access_expr);
                }
            }
        } else if (type == TokenType::Bitfield) { // Bitfield Access
            consume(TokenType::LParen, "Expect '(' after '%%' operator.");
            ExprPtr start_expr = parse_expression();
            consume(TokenType::Comma, "Expect ',' separating start and width in bit-field.");
            ExprPtr width_expr = parse_expression();
            consume(TokenType::RParen, "Expect ')' to close bit-field expression.");
            left = std::make_unique<BitfieldAccessExpression>(std::move(left), std::move(start_expr), std::move(width_expr));
        } else if (type == TokenType::Conditional) { // Ternary Conditional
            ExprPtr true_expr = parse_expression();
            consume(TokenType::Comma, "Expect ',' in conditional expression.");
            ExprPtr false_expr = parse_expression(1);
            left = std::make_unique<ConditionalExpression>(std::move(left), std::move(true_expr), std::move(false_expr));
        } else { // Binary Operator
            auto op = to_binary_op(type);
            int next_precedence = get_token_precedence(type);
            ExprPtr right = parse_expression(next_precedence);
            if (!right) {
                error("Expected an expression for right operand of binary operator.");
                return nullptr;
            }
            left = std::make_unique<BinaryOp>(op, std::move(left), std::move(right));
        }
    }
    return left;
}

/**
 * @brief Parses the "atoms" of an expression (literals, variables, etc.).
 */
ExprPtr Parser::parse_primary_expression() {
    TraceGuard guard(*this, "parse_primary_expression");

    // --- SUPER.method(...) parsing ---
    if (match(TokenType::Super)) {
        consume(TokenType::Dot, "Expect '.' after 'SUPER'");
        std::string method_name = current_token_.value;
        consume(TokenType::Identifier, "Expect method name after 'SUPER.'");

        // The result of this primary expression is just the method access part.
        // The main parse_expression loop will handle wrapping it in a FunctionCall
        // if it's followed by a parenthesis.
        return std::make_unique<SuperMethodAccessExpression>(method_name);
    }

    // --- PAIR expression parsing ---
    if (match(TokenType::Pair)) {
        consume(TokenType::LParen, "Expect '(' after 'pair'");
        ExprPtr first_expr = parse_expression();
        consume(TokenType::Comma, "Expect ',' between pair elements");
        ExprPtr second_expr = parse_expression();
        consume(TokenType::RParen, "Expect ')' after pair elements");
        return std::make_unique<PairExpression>(std::move(first_expr), std::move(second_expr));
    }

    // --- FPAIR expression parsing ---
    if (match(TokenType::FPair)) {
        consume(TokenType::LParen, "Expect '(' after 'fpair'");
        ExprPtr first_expr = parse_expression();
        consume(TokenType::Comma, "Expect ',' between fpair elements");
        ExprPtr second_expr = parse_expression();
        consume(TokenType::RParen, "Expect ')' after fpair elements");
        return std::make_unique<FPairExpression>(std::move(first_expr), std::move(second_expr));
    }

    // --- QUAD expression parsing ---
    if (match(TokenType::Quad)) {
        consume(TokenType::LParen, "Expect '(' after 'quad'");
        ExprPtr first_expr = parse_expression();
        consume(TokenType::Comma, "Expect ',' between quad elements");
        ExprPtr second_expr = parse_expression();
        consume(TokenType::Comma, "Expect ',' between quad elements");
        ExprPtr third_expr = parse_expression();
        consume(TokenType::Comma, "Expect ',' between quad elements");
        ExprPtr fourth_expr = parse_expression();
        consume(TokenType::RParen, "Expect ')' after quad elements");
        return std::make_unique<QuadExpression>(std::move(first_expr), std::move(second_expr), 
                                               std::move(third_expr), std::move(fourth_expr));
    }

    // --- NEW LOGIC FOR BUILT-IN TYPE CONSTANTS ---
    if (match(TokenType::TypeInt)) {
        // VarType::INTEGER = 1
        return std::make_unique<NumberLiteral>(static_cast<int64_t>(1));
    }
    if (match(TokenType::TypeFloat)) {
        // VarType::FLOAT = 2
        return std::make_unique<NumberLiteral>(static_cast<int64_t>(2));
    }
    if (match(TokenType::TypeString)) {
        // ATOM_STRING = 3
        return std::make_unique<NumberLiteral>(static_cast<int64_t>(3));
    }
    if (match(TokenType::TypeList)) {
        // ATOM_LIST_POINTER = 4
        return std::make_unique<NumberLiteral>(static_cast<int64_t>(4));
    }
    if (match(TokenType::TypeVec)) {
        // VarType::VEC = 256
        return std::make_unique<NumberLiteral>(static_cast<int64_t>(256));
    }
    if (match(TokenType::TypePointer)) {
        // VarType::POINTER_TO = 4096
        return std::make_unique<NumberLiteral>(static_cast<int64_t>(4096));
    }
    if (match(TokenType::TypeConst)) {
        // VarType::CONST = 8192
        return std::make_unique<NumberLiteral>(static_cast<int64_t>(8192));
    }
    if (match(TokenType::TypeAny)) {
        // VarType::ANY = 8
        return std::make_unique<NumberLiteral>(static_cast<int64_t>(8));
    }
    if (match(TokenType::TypeList)) {
        // ATOM_LIST_POINTER = 4
        return std::make_unique<NumberLiteral>(static_cast<int64_t>(4));
    }
    if (match(TokenType::TypeVec)) {
        // VarType::VEC = 256
        return std::make_unique<NumberLiteral>(static_cast<int64_t>(256));
    }
    if (match(TokenType::TypePointer)) {
        // VarType::POINTER_TO = 4096
        return std::make_unique<NumberLiteral>(static_cast<int64_t>(4096));
    }
    if (match(TokenType::TypeConst)) {
        // VarType::CONST = 8192
        return std::make_unique<NumberLiteral>(static_cast<int64_t>(8192));
    }
    if (match(TokenType::TypeAny)) {
        // VarType::ANY = 8
        return std::make_unique<NumberLiteral>(static_cast<int64_t>(8));
    }
    if (match(TokenType::TypeTable)) {
        // VarType::TABLE = 1024
        return std::make_unique<NumberLiteral>(static_cast<int64_t>(1024));
    }
    // --- END OF NEW LOGIC ---

    // --- NEW keyword for object instantiation ---
    if (match(TokenType::New)) {
        std::string class_name = current_token_.value;
        consume(TokenType::Identifier, "Expect class name after 'NEW'.");

        std::vector<ExprPtr> args;
        if (match(TokenType::LParen)) { // Optional arguments for CREATE routine
            if (!check(TokenType::RParen)) {
                args.push_back(parse_expression());
                while (match(TokenType::Comma)) {
                    if (check(TokenType::RParen)) {
                        break; // Exit the loop gracefully for trailing comma.
                    }
                    args.push_back(parse_expression());
                }
            }
            consume(TokenType::RParen, "Expect ')' after constructor arguments.");
        }
        return std::make_unique<NewExpression>(class_name, std::move(args));
    }

    if (match(TokenType::NumberLiteral)) {
        const std::string& val_str = previous_token_.value;
        if (val_str.find('.') != std::string::npos || val_str.find('e') != std::string::npos || val_str.find('E') != std::string::npos) {
            return std::make_unique<NumberLiteral>(std::stod(val_str));
        } else {
            // Handle BCPL-style hex numbers (#X...) and regular numbers
            long long value;
            if (val_str.length() >= 2 && val_str[0] == '#' && (val_str[1] == 'X' || val_str[1] == 'x')) {
                // BCPL-style hex number: #X20 or #x20
                std::string hex_part = val_str.substr(2); // Remove "#X" or "#x"
                value = std::stoll(hex_part, nullptr, 16); // Parse as hexadecimal
            } else {
                // Regular decimal number
                value = std::stoll(val_str);
            }
            return std::make_unique<NumberLiteral>(value);
        }
    }
    if (match(TokenType::StringLiteral)) {
        return std::make_unique<StringLiteral>(previous_token_.value);
    }
    if (match(TokenType::CharLiteral)) {
        return std::make_unique<CharLiteral>(previous_token_.value[0]);
    }
    if (match(TokenType::BooleanLiteral)) {
        return std::make_unique<BooleanLiteral>(previous_token_.value == "TRUE");
    }
    if (match(TokenType::NullLiteral)) {
        return std::make_unique<NullLiteral>();
    }
    // --- LIST and MANIFESTLIST expression parsing ---
    if (check(TokenType::List) || check(TokenType::ManifestList)) { // Use check() instead of match()
        bool is_manifest = check(TokenType::ManifestList);
        advance(); // Now consume LIST/MANIFESTLIST here
        return parse_list_expression(is_manifest);
    }
    // --- LEN intrinsic parsing ---
    if (match(TokenType::Len)) {
        consume(TokenType::LParen, "Expect '(' after LEN.");
        ExprPtr expr = parse_expression();
        consume(TokenType::RParen, "Expect ')' after LEN expression.");
        return std::make_unique<UnaryOp>(UnaryOp::Operator::LengthOf, std::move(expr));
    }
    // --- TYPEOF intrinsic parsing ---
    if (match(TokenType::TypeOf)) {
        consume(TokenType::LParen, "Expect '(' after TYPEOF.");
        ExprPtr expr = parse_expression();
        consume(TokenType::RParen, "Expect ')' after TYPEOF expression.");
        return std::make_unique<UnaryOp>(UnaryOp::Operator::TypeOf, std::move(expr));
    }
    // (Removed HD, TL, REST prefix operator logic; now handled in parse_expression)
    // --- FSQRT intrinsic parsing: must come BEFORE Identifier/function call rule ---
    if (match(TokenType::FSQRT)) {
        consume(TokenType::LParen, "Expect '(' after FSQRT.");
        ExprPtr expr = parse_expression();
        consume(TokenType::RParen, "Expect ')' after FSQRT expression.");
        return std::make_unique<UnaryOp>(UnaryOp::Operator::FloatSqrt, std::move(expr));
    }
    // --- ENTIER intrinsic parsing ---
    if (match(TokenType::ENTIER)) {
        consume(TokenType::LParen, "Expect '(' after ENTIER.");
        ExprPtr expr = parse_expression();
        consume(TokenType::RParen, "Expect ')' after ENTIER expression.");
        return std::make_unique<UnaryOp>(UnaryOp::Operator::FloatFloor, std::move(expr));
    }
    // --- TRUNC intrinsic parsing ---
    if (match(TokenType::TRUNC)) {
        consume(TokenType::LParen, "Expect '(' after TRUNC.");
        ExprPtr expr = parse_expression();
        consume(TokenType::RParen, "Expect ')' after TRUNC expression.");
        return std::make_unique<UnaryOp>(UnaryOp::Operator::FloatTruncate, std::move(expr));
    }
    // --- Identifier/function call rule ---
    if (check(TokenType::Identifier)) {
        auto var = std::make_unique<VariableAccess>(current_token_.value);
        advance();
        return var;
    }
    if (match(TokenType::LParen)) {
        return parse_grouped_expression();
    }
    if (match(TokenType::Valof)) {
        return parse_valof_expression();
    }
    if (match(TokenType::FValof)) {
        return parse_fvalof_expression();
    }
    if (match(TokenType::FSQRT)) {
        consume(TokenType::LParen, "Expect '(' after FSQRT.");
        ExprPtr expr = parse_expression();
        consume(TokenType::RParen, "Expect ')' after FSQRT expression.");
        return std::make_unique<UnaryOp>(UnaryOp::Operator::FloatSqrt, std::move(expr));
    }
    if (match(TokenType::Vec)) {
        // Look ahead to see if a '(' or '[' follows VEC.
        if (check(TokenType::LParen)) {
            // If yes, parse it as a new vector initializer list with parentheses.
            return parse_vec_initializer_expression();
        } else if (check(TokenType::LBracket)) {
            // If '[', parse it as a new vector initializer list with brackets.
            return parse_vec_bracket_initializer_expression();
        } else {
            // Otherwise, parse it as the old "VEC size" allocation.
            return std::make_unique<VecAllocationExpression>(parse_expression());
        }
    }
    if (match(TokenType::FVec)) {
        return std::make_unique<FVecAllocationExpression>(parse_expression());
    }
    if (match(TokenType::String)) {
        return std::make_unique<StringAllocationExpression>(parse_expression());
    }
    if (check(TokenType::Table) || check(TokenType::FTable)) {
        bool is_float = check(TokenType::FTable);
        return parse_table_expression(is_float);
    }

    error("Expected an expression.");
    return nullptr;
}

/**
 * @brief Parses a LIST expression: LIST expr, expr, ...
 * Strict: does not allow trailing commas.
 */
ExprPtr Parser::parse_list_expression(bool is_manifest) {
    TraceGuard guard(*this, "parse_list_expression");

    // NEW: Consume the opening parenthesis
    consume(TokenType::LParen, "Expect '(' after LIST or MANIFESTLIST.");

    std::vector<ExprPtr> initializers;

    // Support LIST() as an empty list literal.
    if (check(TokenType::RParen)) {
        consume(TokenType::RParen, "Expect ')' to close empty list."); // Consume the ')'
        auto node = std::make_unique<ListExpression>(std::vector<ExprPtr>{});
        node->is_manifest = is_manifest;
        return node;
    }

    // Parse comma-separated expressions.
    initializers.push_back(parse_expression());
    // Loop as long as we find a comma separator.
    while (match(TokenType::Comma)) {
        // If a comma is followed by the closing parenthesis, it's a trailing comma.
        if (check(TokenType::RParen)) {
            break; // Exit the loop gracefully.
        }
        // Otherwise, expect another expression.
        initializers.push_back(parse_expression());
    }

    // NEW: Consume the closing parenthesis
    consume(TokenType::RParen, "Expect ')' to close LIST expression.");

    auto node = std::make_unique<ListExpression>(std::move(initializers));
    node->is_manifest = is_manifest;
    return node;
}

// --- VEC initializer list parsing ---
ExprPtr Parser::parse_vec_initializer_expression() {
    TraceGuard guard(*this, "parse_vec_initializer_expression");

    consume(TokenType::LParen, "Expect '(' after VEC for initializer list.");

    std::vector<ExprPtr> initializers;

    // Handle empty VEC()
    if (check(TokenType::RParen)) {
        consume(TokenType::RParen, "Expect ')' to close empty VEC().");
        return std::make_unique<VecInitializerExpression>(std::vector<ExprPtr>{});
    }

    // Parse comma-separated expressions.
    initializers.push_back(parse_expression());
    // Loop as long as we find a comma separator.
    while (match(TokenType::Comma)) {
        // If a comma is followed by the closing parenthesis, it's a trailing comma.
        if (check(TokenType::RParen)) {
            break; // Exit the loop gracefully.
        }
        // Otherwise, expect another expression.
        initializers.push_back(parse_expression());
    }

    consume(TokenType::RParen, "Expect ')' to close VEC initializer list.");

    return std::make_unique<VecInitializerExpression>(std::move(initializers));
}

ExprPtr Parser::parse_vec_bracket_initializer_expression() {
    TraceGuard guard(*this, "parse_vec_bracket_initializer_expression");

    consume(TokenType::LBracket, "Expect '[' after VEC for initializer list.");

    std::vector<ExprPtr> initializers;

    // Handle empty VEC[]
    if (check(TokenType::RBracket)) {
        consume(TokenType::RBracket, "Expect ']' to close empty VEC[].");
        return std::make_unique<VecInitializerExpression>(std::vector<ExprPtr>{});
    }

    // Parse comma-separated expressions.
    initializers.push_back(parse_expression());
    // Loop as long as we find a comma separator.
    while (match(TokenType::Comma)) {
        // If a comma is followed by the closing bracket, it's a trailing comma.
        if (check(TokenType::RBracket)) {
            break; // Exit the loop gracefully.
        }
        // Otherwise, expect another expression.
        initializers.push_back(parse_expression());
    }

    consume(TokenType::RBracket, "Expect ']' to close VEC initializer list.");

    return std::make_unique<VecInitializerExpression>(std::move(initializers));
}

// Parses a TABLE or FTABLE expression: TABLE expr, expr, ...
ExprPtr Parser::parse_table_expression(bool is_float_table) {
    TraceGuard guard(*this, "parse_table_expression");

    advance(); // Consume TABLE or FTABLE token

    // FIX: Consume the opening parenthesis
    consume(TokenType::LParen, "Expect '(' after TABLE or FTABLE.");

    std::vector<ExprPtr> initializers;

    // Handle empty TABLE()
    if (check(TokenType::RParen)) {
        consume(TokenType::RParen, "Expect ')' to close empty TABLE.");
        return std::make_unique<TableExpression>(std::vector<ExprPtr>{}, is_float_table);
    }
    
    // Parse the comma-separated list of initializers
    initializers.push_back(parse_expression());
    // Loop as long as we find a comma separator.
    while (match(TokenType::Comma)) {
        // If a comma is followed by the closing parenthesis, it's a trailing comma.
        if (check(TokenType::RParen)) {
            break; // Exit the loop gracefully.
        }
        // Otherwise, expect another expression.
        initializers.push_back(parse_expression());
    }

    // FIX: Consume the closing parenthesis
    consume(TokenType::RParen, "Expect ')' to close TABLE expression.");

    auto table_expr = std::make_unique<TableExpression>(std::move(initializers), is_float_table);
    return table_expr;
}

/**
 * @brief Parses a parenthesized expression.
 */
ExprPtr Parser::parse_grouped_expression() {
    TraceGuard guard(*this, "parse_grouped_expression");
    ExprPtr expr = parse_expression();
    consume(TokenType::RParen, "Expect ')' after expression.");
    return expr;
}

/**
 * @brief Parses a VALOF block.
 */
ExprPtr Parser::parse_valof_expression() {
    TraceGuard guard(*this, "parse_valof_expression");

    VarType explicit_type = VarType::UNKNOWN;
    // Support optional AS <type> annotation
    if (match(TokenType::As)) {
        explicit_type = parse_type_specifier();
    }

    StmtPtr body = parse_statement();
    auto node = std::make_unique<ValofExpression>(std::move(body));
    node->explicit_return_type = explicit_type;
    return node;
}

/**
 * @brief Parses an FVALOF block.
 */
ExprPtr Parser::parse_fvalof_expression() {
    TraceGuard guard(*this, "parse_fvalof_expression");
    StmtPtr body = parse_statement();
    return std::make_unique<FloatValofExpression>(std::move(body));
}
