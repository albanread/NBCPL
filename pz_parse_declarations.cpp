#include "Parser.h"
#include <stdexcept>
#include <vector>

// --- Class context tracking ---
static std::string current_class_name_;

DeclPtr Parser::parse_member_declaration() {
    TraceGuard guard(*this, "parse_member_declaration");

    if (check(TokenType::Decl)) {
        advance(); // Consume 'DECL'
        std::vector<std::string> names;
        do {
            names.push_back(current_token_.value);
            consume(TokenType::Identifier, "Expect identifier in DECL.");
        } while (match(TokenType::Comma));
        return std::unique_ptr<Declaration>{new LetDeclaration(std::move(names), std::vector<ExprPtr>{})};
    }

    if (check(TokenType::Let) || check(TokenType::FLet)) {
        return parse_member_let_declaration();
    }

    if (check(TokenType::Function) || check(TokenType::Routine)) {
        return parse_function_or_routine_declaration();
    }

    error("Expected DECL, LET, FLET, FUNCTION, or ROUTINE for class member declaration.");
    advance(); // IMPORTANT: Consume the token to prevent an infinite loop on error.
    return nullptr;
}

/**
 * @brief Parses a LET or FLET declaration specifically for class members.
 * This ensures it always produces a LetDeclaration node, never a GlobalVariableDeclaration.
 */
DeclPtr Parser::parse_member_let_declaration() {
    TraceGuard guard(*this, "parse_member_let_declaration");

    bool is_float = check(TokenType::FLet);
    advance(); // Consume LET or FLET

    std::string name = current_token_.value;
    consume(TokenType::Identifier, "Expect identifier after LET/FLET.");

    // If this is a function/routine declaration, inject _this
    if (check(TokenType::LParen)) {
        bool inject_this = !current_class_name_.empty();
        std::vector<std::string> params;
        consume(TokenType::LParen, "Expect '(' after function/method name.");
        if (!check(TokenType::RParen)) {
            do {
                params.push_back(current_token_.value);
                consume(TokenType::Identifier, "Expect parameter name.");
            } while (match(TokenType::Comma));
        }
        consume(TokenType::RParen, "Expect ')' after parameters.");
        if (inject_this) {
            params.insert(params.begin(), "_this");
        }
        return parse_function_or_routine_body(name, std::move(params));
    } else {
        // Member variable declaration
        std::vector<std::string> names;
        names.push_back(name);
        while (match(TokenType::Comma)) {
            names.push_back(current_token_.value);
            consume(TokenType::Identifier, "Expect identifier after comma in LET/FLET.");
        }

        // If the next token is another declaration keyword, this is a member variable
        // declaration without an initializer. We can return immediately.
        if (check(TokenType::Let) || check(TokenType::FLet) ||
            check(TokenType::Routine) || check(TokenType::Function) ||
            check(TokenType::RBrace)) {
            auto decl = std::make_unique<LetDeclaration>(std::move(names), std::vector<ExprPtr>{});
            decl->is_float_declaration = is_float;
            return decl;
        }

        consume(TokenType::Equal, "Expect '=' after name(s) in LET/FLET declaration.");

        std::vector<ExprPtr> initializers;
        do {
            initializers.push_back(parse_expression());
        } while (match(TokenType::Comma));

        // Allow destructuring assignment: 2 names, 1 initializer (for PAIR/FPAIR unpacking)
        if (names.size() != initializers.size()) {
            if (names.size() == 2 && initializers.size() == 1) {
                // This is potentially a destructuring LET declaration - semantic analysis will validate
                // that the single initializer expression is a PAIR or FPAIR type
                // Detected potential destructuring LET declaration (2 names, 1 initializer)
            } else {
                error("[ERROR-005] Mismatch between number of names and initializers in LET/FLET declaration.");
                return nullptr;
            }
        }

        auto decl = std::make_unique<LetDeclaration>(std::move(names), std::move(initializers));
        decl->is_float_declaration = is_float;
        return decl;
    }
}

/**
 * @brief Checks if the current token can start a non-LET declaration.
 * This is used by the main parsing loop to decide whether to call parse_declaration.
 */
bool Parser::is_declaration_start() {
    // --- START OF FIX ---
    // A label is now considered a statement, so we remove the check from here.
    // if (check(TokenType::Identifier) && lexer_.peek().type == TokenType::Colon) {
    //     return true;
    // }
    // --- END OF FIX ---
    // Check for other declaration-starting keywords.
    switch (current_token_.type) {
        case TokenType::Manifest:
        case TokenType::Static:
        case TokenType::Global:
        case TokenType::Globals:
        case TokenType::Class: // Added for CLASS declarations
            return true;
        default:
            return false;
    }
}

/**
 * @brief Parses any non-LET declaration based on the current token.
 */
DeclPtr Parser::parse_declaration() {
    TraceGuard guard(*this, "parse_declaration");

    // Check for a label declaration first, as it starts with an identifier.
    if (check(TokenType::Identifier) && lexer_.peek().type == TokenType::Colon) {
        return parse_label_declaration();
    }

    // Handle other declaration types.
    switch (current_token_.type) {
        case TokenType::Manifest: return parse_manifest_declaration();
        case TokenType::Static:   return parse_static_declaration();
        case TokenType::Global:   return parse_global_declaration();
        case TokenType::Globals:  return parse_globals_declaration();
        case TokenType::Class:    return parse_class_declaration();

        // Handle RETAIN ... = ... as a valid declaration
        case TokenType::Retain: {
            // Try to parse RETAIN ... = ... as a declaration.
            // We expect: RETAIN <identifier> [, <identifier>]* = <expr> [, <expr>]*
            // If after the identifiers and commas we see '=', treat as a declaration.
            // Otherwise, treat as a statement (legacy RETAIN).
            // We'll use a local token buffer to check for '=' after identifiers/commas.
            int id_count = 0;
            int comma_count = 0;
            bool found_equal = false;
            Token temp_token = lexer_.peek();
            if (temp_token.type == TokenType::Identifier) {
                // Scan ahead for identifiers and commas
                size_t scan_pos = 1;
                while (true) {
                    Token next_token = lexer_.peek();
                    if (next_token.type == TokenType::Comma) {
                        ++comma_count;
                        advance();
                    } else if (next_token.type == TokenType::Identifier) {
                        ++id_count;
                        advance();
                    } else if (next_token.type == TokenType::Equal) {
                        found_equal = true;
                        break;
                    } else {
                        break;
                    }
                }
            }
            if (found_equal) {
                return parse_retaining_declaration();
            }
            // Otherwise, treat as a statement (legacy RETAIN)
            error("RETAIN used as a statement, not a declaration. Use RETAIN <var> = <expr> for declaration.");
            advance();
            return nullptr;
        }

        case TokenType::Let:
        case TokenType::FLet:
            return parse_toplevel_let_declaration();

        // FUNCTION/ROUTINE are no longer valid at the top level

        default:
            error("Unknown or unexpected declaration type.");
            advance();
            return nullptr;
    }
}

/**
 * @brief Parses a label declaration of the form: LABEL: <statement>
 */
DeclPtr Parser::parse_label_declaration() {
    TraceGuard guard(*this, "parse_label_declaration");
    std::string name = current_token_.value;
    consume(TokenType::Identifier, "Expect identifier for label name.");
    consume(TokenType::Colon, "Expect ':' after label name.");
    auto command = parse_statement();
    return std::make_unique<LabelDeclaration>(name, std::move(command));
}

/**
 * @brief Parses a MANIFEST block: MANIFEST $( NAME = 123; ... $)
 */
DeclPtr Parser::parse_manifest_declaration() {
    TraceGuard guard(*this, "parse_manifest_declaration");
    consume(TokenType::Manifest, "Expect 'MANIFEST'.");
    consume(TokenType::LBrace, "Expect '$(' or '{' after MANIFEST.");

    // Loop through all declarations inside the MANIFEST block.
    while (!check(TokenType::RBrace) && !is_at_end()) {
        std::string name = current_token_.value;
        consume(TokenType::Identifier, "Expect identifier in manifest declaration.");
        consume(TokenType::Equal, "Expect '=' in manifest declaration.");

        // Handle BCPL-style hex numbers (#X...) and regular numbers
        long long value;
        std::string token_value = current_token_.value;
        if (token_value.length() >= 2 && token_value[0] == '#' && (token_value[1] == 'X' || token_value[1] == 'x')) {
            // BCPL-style hex number: #X20 or #x20
            std::string hex_part = token_value.substr(2); // Remove "#X" or "#x"
            value = std::stoll(hex_part, nullptr, 16); // Parse as hexadecimal
        } else {
            // Regular decimal number
            value = std::stoll(token_value);
        }
        consume(TokenType::NumberLiteral, "Expect a number for manifest value.");
        
        // Add the declaration directly to the program's declaration list.
        program_->declarations.push_back(std::make_unique<ManifestDeclaration>(name, value));

        // Consume optional semicolon or comma separator.
        // Newlines are handled naturally since they're whitespace - no separator needed.
        if (check(TokenType::Semicolon) || check(TokenType::Comma)) {
            advance();
        }
        // If no explicit separator, that's fine - allows newline-separated declarations
    }

    consume(TokenType::RBrace, "Expect '$)' or '}' to close MANIFEST block.");
    
    // Return nullptr because we have already added all declarations to the program.
    // This prevents the main loop from adding a duplicate null declaration.
    return nullptr;
}

/**
 * @brief Parses a STATIC block: STATIC $( NAME = 123 $)
 */
DeclPtr Parser::parse_static_declaration() {
    TraceGuard guard(*this, "parse_static_declaration");
    // Support both STATIC and FSTATIC
    bool is_float = false;
    if (check(TokenType::FStatic)) {
        is_float = true;
        consume(TokenType::FStatic, "Expect 'FSTATIC'.");
    } else {
        consume(TokenType::Static, "Expect 'STATIC'.");
    }
    consume(TokenType::LBrace, "Expect '$(' after STATIC/FSTATIC.");

    std::string name = current_token_.value;
    consume(TokenType::Identifier, "Expect identifier in static declaration.");
    consume(TokenType::Equal, "Expect '=' in static declaration.");
    return nullptr;
}

/**
 * @brief Parses a RETAIN ... = ... declaration.
 * Creates a LetDeclaration node with the is_retained flag set.
 */
DeclPtr Parser::parse_retaining_declaration() {
    TraceGuard guard(*this, "parse_retaining_declaration");
    consume(TokenType::Retain, "Expect 'RETAIN'.");

    std::vector<std::string> names;
    do {
        names.push_back(current_token_.value);
        consume(TokenType::Identifier, "Expect identifier in RETAIN declaration.");
    } while (match(TokenType::Comma));

    consume(TokenType::Equal, "Expect '=' after name(s) in RETAIN declaration.");

    std::vector<ExprPtr> initializers;
    do {
        initializers.push_back(parse_expression());
    } while (match(TokenType::Comma));

    if (names.size() != initializers.size()) {
        error("Mismatch between number of names and initializers in RETAIN declaration.");
        return nullptr;
    }

    auto let_decl = std::make_unique<LetDeclaration>(std::move(names), std::move(initializers));
    let_decl->is_retained = true; // <-- SET THE FLAG
    return let_decl;
}


/**
 * @brief Parses a GLOBAL block: GLOBAL $( G1:0; G2:1 $)
 */
DeclPtr Parser::parse_global_declaration() {
    TraceGuard guard(*this, "parse_global_declaration");
    std::vector<std::pair<std::string, int>> globals;
    consume(TokenType::Global, "Expect 'GLOBAL'.");
    consume(TokenType::LBrace, "Expect '$(' after GLOBAL.");

    do {
        std::string name = current_token_.value;
        consume(TokenType::Identifier, "Expect identifier in global declaration.");
        consume(TokenType::Colon, "Expect ':' separating global name and offset.");
        int offset = std::stoi(current_token_.value);
        consume(TokenType::NumberLiteral, "Expect number for global offset.");
        globals.push_back({name, offset});

    } while (match(TokenType::Semicolon) || match(TokenType::Comma));

    consume(TokenType::RBrace, "Expect '$)' to close GLOBAL block.");
    return std::make_unique<GlobalDeclaration>(std::move(globals));
}

/**
 * @brief Parses a GET directive: GET "filename"
 */

/**
 * @brief Parses a CLASS declaration: CLASS Name [EXTENDS Parent] $( ... $)
 */
DeclPtr Parser::parse_class_declaration() {
    TraceGuard guard(*this, "parse_class_declaration");
    consume(TokenType::Class, "Expect 'CLASS'.");

    std::string class_name = current_token_.value;
    consume(TokenType::Identifier, "Expect class name after 'CLASS'.");

    std::string parent_class_name = "";
    if (match(TokenType::Extends)) {
        parent_class_name = current_token_.value;
        consume(TokenType::Identifier, "Expect parent class name after 'EXTENDS'.");
    }

    consume(TokenType::LBrace, "Expect '$(' or '{' after class name or EXTENDS clause.");

    // Set class context
    current_class_name_ = class_name;

    // Track current visibility level - default to PUBLIC
    Visibility current_visibility = Visibility::Public;

    // Parse member declarations inside the class body
    std::vector<ClassMember> members;
    while (!check(TokenType::RBrace) && !is_at_end()) {
        // Check for visibility modifiers
        if (check(TokenType::Public) || check(TokenType::Private) || check(TokenType::Protected)) {
            switch (current_token_.type) {
                case TokenType::Public:
                    current_visibility = Visibility::Public;
                    break;
                case TokenType::Private:
                    current_visibility = Visibility::Private;
                    break;
                case TokenType::Protected:
                    current_visibility = Visibility::Protected;
                    break;
                default:
                    break;
            }
            advance(); // Consume the visibility keyword
            consume(TokenType::Colon, "Expect ':' after visibility modifier.");
            continue; // Parse next declaration with new visibility
        }

        DeclPtr member = parse_member_declaration();
        if (member) { // Only add valid declarations
            members.emplace_back(std::move(member), current_visibility);
        }
    }
    consume(TokenType::RBrace, "Expect '$)' or '}' to close class declaration.");

    // Clear class context
    current_class_name_.clear();

    return std::make_unique<ClassDeclaration>(class_name, parent_class_name, std::move(members));
}


DeclPtr Parser::parse_function_or_routine_declaration() {
    TraceGuard guard(*this, "parse_function_or_routine_declaration");

    bool is_function = match(TokenType::Function);
    bool is_routine = !is_function && match(TokenType::Routine);

    std::string name = current_token_.value;
    consume(TokenType::Identifier, "Expect name after FUNCTION/ROUTINE keyword.");

    // Inject _this if inside a class
    bool inject_this = !current_class_name_.empty();
    std::vector<std::string> params;
    consume(TokenType::LParen, "Expect '(' after function/routine name.");
    if (!check(TokenType::RParen)) {
        do {
            params.push_back(current_token_.value);
            consume(TokenType::Identifier, "Expect parameter name.");
        } while (match(TokenType::Comma));
    }
    consume(TokenType::RParen, "Expect ')' after parameters.");
    if (inject_this) {
        params.insert(params.begin(), "_this");
    }

    if (is_function) {
        consume(TokenType::Equal, "Expect '=' after function signature.");

        ExprPtr body;
        if (match(TokenType::Valof)) {
            body = parse_valof_expression();
        } else if (match(TokenType::FValof)) {
            body = parse_fvalof_expression();
        } else {
            error("Expect 'VALOF' or 'FVALOF' for FUNCTION body.");
            return nullptr;
        }
        return std::make_unique<FunctionDeclaration>(name, std::move(params), std::move(body));
    } else if (is_routine) {
        consume(TokenType::Be, "A ROUTINE must be defined with 'BE'.");
        auto body = parse_statement();
        return std::make_unique<RoutineDeclaration>(name, std::move(params), std::move(body));
    }

    error("Expected 'FUNCTION' or 'ROUTINE' for class member.");
    return nullptr;
}

/**
 * @brief Parses a GLOBALS block declaration.
 * GLOBALS $( LET var1 = expr1; FLET float_var = 3.14 $)
 * Transforms each LET/FLET into individual GlobalVariableDeclaration nodes
 */
DeclPtr Parser::parse_globals_declaration() {
    TraceGuard guard(*this, "parse_globals_declaration");
    consume(TokenType::Globals, "Expect 'GLOBALS'.");
    consume(TokenType::LBrace, "Expect '$(' or '{' after GLOBALS.");

    // Parse each LET/FLET declaration inside the GLOBALS block
    while (!check(TokenType::RBrace) && !is_at_end()) {
        if (check(TokenType::Let) || check(TokenType::FLet)) {
            bool is_float = check(TokenType::FLet);
            advance(); // Consume LET or FLET

            // Parse variable name
            std::string var_name = current_token_.value;
            consume(TokenType::Identifier, "Expect variable name after LET/FLET.");
            consume(TokenType::Equal, "Expect '=' after variable name.");
            
            // Parse initializer expression
            ExprPtr initializer = parse_expression();
            
            // Create individual GlobalVariableDeclaration for this variable
            std::vector<std::string> names = {var_name};
            std::vector<ExprPtr> initializers;
            initializers.push_back(std::move(initializer));
            
            auto global_decl = std::make_unique<GlobalVariableDeclaration>(
                std::move(names), std::move(initializers));
            global_decl->is_float_declaration = is_float;
            
            // Add directly to program
            program_->declarations.push_back(std::move(global_decl));
        } else {
            error("Only LET or FLET declarations are allowed inside a GLOBALS block.");
            synchronize();
            break;
        }

        // Consume optional separators
        if (check(TokenType::Semicolon) || check(TokenType::Comma)) {
            advance();
        }
    }

    consume(TokenType::RBrace, "Expect '$)' or '}' to close GLOBALS block.");
    return nullptr; // No single declaration to return since we added them all
}

/**
 * @brief Helper function (kept for header compatibility but unused in simplified approach)
 */
DeclPtr Parser::parse_global_let_in_block() {
    // This function is now unused but kept for header compatibility
    return nullptr;
}
