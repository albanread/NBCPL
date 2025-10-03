#include "Parser.h"
#include "LexerDebug.h" // For trace logging
#include <algorithm>
#include <stdexcept>
#include <utility> // For std::move

/**
 * Maps a token type to its corresponding BinaryOp::Operator enum.
 */

static BinaryOp::Operator to_binary_op(TokenType type) {
  switch (type) {
  // Integer Arithmetic
  case TokenType::Plus:
    return BinaryOp::Operator::Add;
  case TokenType::Minus:
    return BinaryOp::Operator::Subtract;
  case TokenType::Multiply:
    return BinaryOp::Operator::Multiply;
  case TokenType::Divide:
    return BinaryOp::Operator::Divide;
  case TokenType::Remainder:
    return BinaryOp::Operator::Remainder;

  // Equality and Relational
  case TokenType::Equal:
    return BinaryOp::Operator::Equal;
  case TokenType::NotEqual:
    return BinaryOp::Operator::NotEqual;
  case TokenType::Less:
    return BinaryOp::Operator::Less;
  case TokenType::LessEqual:
    return BinaryOp::Operator::LessEqual;
  case TokenType::Greater:
    return BinaryOp::Operator::Greater;
  case TokenType::GreaterEqual:
    return BinaryOp::Operator::GreaterEqual;

  // Logical and Equivalence
  case TokenType::LogicalAnd:
    return BinaryOp::Operator::LogicalAnd;
  case TokenType::LogicalOr:
    return BinaryOp::Operator::LogicalOr;
  case TokenType::Equivalence:
    return BinaryOp::Operator::Equivalence;
  case TokenType::NotEquivalence:
    return BinaryOp::Operator::NotEquivalence;

  // Bitwise Shifts
  case TokenType::LeftShift:
    return BinaryOp::Operator::LeftShift;
  case TokenType::RightShift:
    return BinaryOp::Operator::RightShift;



  default:
    throw std::logic_error("Invalid token type for a binary operator.");
  }
}

/**
 * @brief Constructs the parser, primes the first token, and initializes state.
 * @param lexer A reference to the lexer providing the token stream.
 * @param trace Flag to enable debug tracing.
 */
Parser::Parser(Lexer &lexer, bool trace)
    : lexer_(lexer), trace_enabled_(trace), fatal_error_(false),
      trace_depth_(0) {
  // Prime the pump by advancing to the first token.
  advance();
}

/**
 * @brief Main entry point. Parses the entire token stream and returns the
 * complete AST.
 */
ProgramPtr Parser::parse_program() {
  TraceGuard guard(*this, "parse_program");
  program_ = std::make_unique<Program>();

  while (!is_at_end() && !fatal_error_) {
    try {
      if (check(TokenType::Let) || check(TokenType::FLet)) {
        // At the top level, LET/FLET can be a function or a global variable.
        // Both are considered Declarations.
        program_->declarations.push_back(parse_toplevel_let_declaration());
      } else if (is_declaration_start()) {
        program_->declarations.push_back(parse_declaration());
      } else {
        error("Executable statements are not allowed in the global scope.");
        synchronize();
        break; // Exit on finding a statement at the top level.
      }
    } catch (const std::runtime_error &) {
      // In case of an error, synchronize to the next likely
      // statement/declaration.
      synchronize();
    }
  }
  return std::move(program_);
}

// --- New: parse_toplevel_let_declaration for top-level LET/FLET handling ---
// //
DeclPtr Parser::parse_toplevel_let_declaration() {
  TraceGuard guard(*this, "parse_toplevel_let_declaration");

  bool is_float = check(TokenType::FLet);
  advance(); // Consume LET or FLET

  std::string name = current_token_.value;
  consume(TokenType::Identifier, "Expect identifier after LET/FLET.");

  if (check(TokenType::LParen)) {
    // This is a function declaration, which is already a DeclPtr.
    std::vector<std::string> params;
    consume(TokenType::LParen, "Expect '(' after function name.");
    if (!check(TokenType::RParen)) {
      do {
        params.push_back(current_token_.value);
        consume(TokenType::Identifier, "Expect parameter name.");
      } while (match(TokenType::Comma));
    }
    consume(TokenType::RParen, "Expect ')' after function parameters.");
    // No _this injection for top-level functions
    return parse_function_or_routine_body(name, std::move(params), is_float);
  } else {
    // This is a global variable declaration. Use the new AST node.
    std::vector<std::string> names;
    names.push_back(name);
    while (match(TokenType::Comma)) {
      names.push_back(current_token_.value);
      consume(TokenType::Identifier,
              "Expect identifier after comma in LET/FLET.");
    }

    // If the next token is another declaration keyword, this is a member
    // variable declaration without an initializer. We can return immediately.
    if (check(TokenType::Let) || check(TokenType::FLet) ||
        check(TokenType::Routine) || check(TokenType::Function) ||
        check(TokenType::RBrace)) { // RBrace marks the end of the class
      auto decl = std::make_unique<GlobalVariableDeclaration>(
          std::move(names), std::vector<ExprPtr>{});
      decl->is_float_declaration = is_float;
      return decl;
    }

    consume(TokenType::Equal,
            "Expect '=' after name(s) in LET/FLET declaration.");

    std::vector<ExprPtr> initializers;
    do {
      initializers.push_back(parse_expression());
    } while (match(TokenType::Comma));

    // Allow destructuring assignment: 2 names, 1 initializer (for PAIR/FPAIR unpacking)
    // or 4 names, 1 initializer (for QUAD unpacking)
    if (names.size() != initializers.size()) {
      if ((names.size() == 2 && initializers.size() == 1) || 
          (names.size() == 4 && initializers.size() == 1)) {
        // This is potentially a destructuring LET declaration - semantic analysis will validate
        // that the single initializer expression is a PAIR/FPAIR type (for 2 names) or QUAD type (for 4 names)
        // Detected potential destructuring LET declaration (2/4 names, 1 initializer)
      } else {
        error("[ERROR-004] Mismatch between number of names and initializers in LET/FLET "
              "declaration.");
        return nullptr;
      }
    }

    auto decl = std::make_unique<GlobalVariableDeclaration>(
        std::move(names), std::move(initializers));
    decl->is_float_declaration = is_float;
    return decl;
  }
}

// --- New: Helper for parameter parsing with _this injection --- //
std::vector<std::string> Parser::parse_parameter_list(bool inject_this) {
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
    if (std::find(params.begin(), params.end(), "_this") == params.end()) {
      params.insert(params.begin(), "_this");
    }
  }
  return params;
}

/**
 * @brief The central dispatcher for all LET/FLET constructs.
 * This is the core of the new parser logic. It uses lookahead to decide
 * whether to parse a function declaration or a variable statement.
 */
void Parser::parse_let_construct() {
  TraceGuard guard(*this, "parse_let_construct");

  bool is_float_declaration = check(TokenType::FLet);
  advance(); // Consume LET or FLET

  std::string name = current_token_.value;
  consume(TokenType::Identifier, "Expect identifier after LET/FLET.");

  // --- Lookahead Logic ---
  if (check(TokenType::LParen)) {
    // Sequence is 'LET IDENTIFIER (', so this is a function/routine
    // declaration.
    std::vector<std::string> params;
    consume(TokenType::LParen, "Expect '(' after function name.");
    if (!check(TokenType::RParen)) {
      do {
        params.push_back(current_token_.value);
        consume(TokenType::Identifier, "Expect parameter name.");
      } while (match(TokenType::Comma));
    }
    consume(TokenType::RParen, "Expect ')' after function parameters.");

    // Parse the body and add the resulting node to the declarations list.
    program_->declarations.push_back(
        parse_function_or_routine_body(name, std::move(params), is_float_declaration));
  } else {
    // Sequence is 'LET IDENTIFIER =', so it's a variable assignment statement.
    std::vector<std::string> names;
    names.push_back(name);
    while (match(TokenType::Comma)) {
      names.push_back(current_token_.value);
      consume(TokenType::Identifier,
              "Expect identifier after comma in LET/FLET list.");
    }

    consume(TokenType::Equal,
            "Expect '=' after name(s) in LET/FLET declaration.");

    std::vector<ExprPtr> initializers;
    do {
      initializers.push_back(parse_expression());
    } while (match(TokenType::Comma));

    // Allow destructuring assignment: 2 names, 1 initializer (for PAIR/FPAIR unpacking)
    // or 4 names, 1 initializer (for QUAD unpacking)
    if (names.size() != initializers.size()) {
      if ((names.size() == 2 && initializers.size() == 1) || 
          (names.size() == 4 && initializers.size() == 1)) {
        // This is potentially a destructuring LET declaration - semantic analysis will validate
        // that the single initializer expression is a PAIR/FPAIR type (for 2 names) or QUAD type (for 4 names)
        // Detected potential destructuring LET declaration (2/4 names, 1 initializer)
      } else {
        error("[ERROR-004] Mismatch between number of names and initializers in LET/FLET "
              "declaration.");
        return;
      }
    }

    auto let_decl = std::make_unique<LetDeclaration>(std::move(names),
                                                     std::move(initializers));
    let_decl->is_float_declaration = is_float_declaration;

    // --- Automatic DEFER for RELEASE ---
    // If the initializer is a NEW expression, generate a DEFER statement for
    // RELEASE. Only handle the single-variable case for now.
    if (let_decl->names.size() == 1 && let_decl->initializers.size() == 1) {
      auto *new_expr =
          dynamic_cast<NewExpression *>(let_decl->initializers[0].get());
      if (new_expr) {
        // Create my_obj.RELEASE() as a RoutineCallStatement
        auto var_access = std::make_unique<VariableAccess>(let_decl->names[0]);
        auto member_access = std::make_unique<MemberAccessExpression>(
            std::move(var_access), "RELEASE");
        auto routine_call = std::make_unique<RoutineCallStatement>(
            std::move(member_access), std::vector<ExprPtr>{});
        auto defer_stmt =
            std::make_unique<DeferStatement>(std::move(routine_call));
        // Add both the LET and the DEFER to the statements list
        // (REMOVED: let_decl is a Declaration, not a Statement)
        program_->statements.push_back(std::move(defer_stmt));
        return;
      }
    }

    // Add the resulting node to the statements list.
    // (REMOVED: let_decl is a Declaration, not a Statement)
    // If this is a declaration, add it to a declarations vector instead, or
    // lower to assignments if needed.
  }
}

/**
 * @brief Parses the body of a function or routine after the name and parameters
 * have been consumed.
 */
DeclPtr
Parser::parse_function_or_routine_body(const std::string &name,
                                       std::vector<std::string> params, bool is_float) {
  TraceGuard guard(*this, "parse_function_or_routine_body");

  // Check for VIRTUAL and FINAL modifiers before FUNCTION/ROUTINE
  bool is_virtual = false;
  bool is_final = false;
  while (check(TokenType::Virtual) || check(TokenType::Final)) {
    if (match(TokenType::Virtual)) {
      is_virtual = true;
    }
    if (match(TokenType::Final)) {
      is_final = true;
    }
  }

  if (match(TokenType::Equal)) {
    // Function with a return value (body is an expression).
    auto body = parse_expression();
    auto func_decl = std::make_unique<FunctionDeclaration>(
        name, std::move(params), std::move(body), is_float);
    func_decl->is_virtual = is_virtual;
    func_decl->is_final = is_final;
    return func_decl;
  }
  if (match(TokenType::Be)) {
    // DEVELOPER NOTE:
    // The keyword `ROUTINE` does not exist in the BCPL source language.
    // This rule parses the `LET name() BE <statement>` syntax and creates an
    // internal `RoutineDeclaration` AST node. This distinction between the
    // source syntax and the compiler's internal representation is important
    // to avoid confusion.
    // Routine without a return value (body is a statement).
    auto body = parse_statement();
    auto routine_decl = std::make_unique<RoutineDeclaration>(
        name, std::move(params), std::move(body));
    routine_decl->is_virtual = is_virtual;
    routine_decl->is_final = is_final;
    return routine_decl;
  }
  // If neither '=' nor 'BE' is found, report error and return nullptr.
  error("Expect '=' or 'BE' in function/routine declaration.");
  return nullptr;
}

// --- Core Parser Methods (Implementations) ---

void Parser::advance() {
  previous_token_ = std::move(current_token_);
  while (true) {
    current_token_ = lexer_.get_next_token();
    if (current_token_.type != TokenType::Error)
      break;
    error("Lexical error: " + current_token_.value);
  }
}

bool Parser::match(TokenType type) {
  if (check(type)) {
    advance();
    return true;
  }
  return false;
}

void Parser::consume(TokenType expected, const std::string &error_message) {
  if (check(expected)) {
    advance();
    return;
  }
  // New, more informative error generation
  std::string detailed_message = "Expected token " + to_string(expected) +
                                 " but got " + to_string(current_token_.type) +
                                 ". " + error_message;
  error(detailed_message); // Call our enhanced error method
  throw std::runtime_error("Parsing error.");
}

bool Parser::check(TokenType type) const {
  if (is_at_end())
    return false;
  return current_token_.type == type;
}

bool Parser::is_at_end() const { return current_token_.type == TokenType::Eof; }

void Parser::error(const std::string &message) {
  std::string error_msg = "[L" + std::to_string(previous_token_.line) + " C" +
                          std::to_string(previous_token_.column) +
                          "] Error: " + message;
  errors_.push_back(error_msg); // Accumulate error messages
  LexerTrace(error_msg);
  fatal_error_ = true;
  throw std::runtime_error("Parser error: " + message);
}

void Parser::synchronize() {
  advance();
  while (!is_at_end()) {
    if (previous_token_.type == TokenType::Semicolon)
      return;
    switch (current_token_.type) {
    case TokenType::Let:
    case TokenType::Function:
    case TokenType::Routine:
    case TokenType::If:
    case TokenType::For:
    case TokenType::While:
    case TokenType::Return:
      return; // Return on tokens that can start a new statement/declaration.
    default:
      break;
    }
    advance();
  }
}

void Parser::trace(const std::string &rule_name) {
  if (!trace_enabled_)
    return;
  std::string indent(trace_depth_ * 2, ' ');
  LexerTrace(indent + rule_name);
}

Parser::TraceGuard::TraceGuard(Parser &parser, const std::string &name)
    : parser_(parser) {
  parser_.trace(">> Entering " + name);
  parser_.trace_depth_++;
}

Parser::TraceGuard::~TraceGuard() {
  parser_.trace_depth_--;
  parser_.trace("<< Exiting");
}
