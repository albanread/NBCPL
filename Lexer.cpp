#include "Lexer.h"
#include "LexerDebug.h"
#include <cctype>
#include <utility>

const std::unordered_map<std::string, TokenType> Lexer::keywords_ = {
    {"LET", TokenType::Let}, {"MANIFEST", TokenType::Manifest}, {"STATIC", TokenType::Static}, {"FSTATIC", TokenType::FStatic},
    {"GLOBAL", TokenType::Global}, {"GLOBALS", TokenType::Globals}, {"FUNCTION", TokenType::Function}, {"ROUTINE", TokenType::Routine},
    {"AND", TokenType::LogicalAnd}, {"NOT", TokenType::LogicalNot}, {"VEC", TokenType::Vec}, {"IF", TokenType::If},
    {"FVEC", TokenType::FVec}, // Added FVEC as a keyword
    {"PAIR", TokenType::Pair}, // Added PAIR as a keyword
    {"FPAIR", TokenType::FPair}, // Added FPAIR as a keyword
    {"QUAD", TokenType::Quad}, // Added QUAD as a keyword
    {"OCT", TokenType::Oct}, // Added OCT as a keyword
    {"FOCT", TokenType::FOct}, // Added FOCT as a keyword
    {"UNLESS", TokenType::Unless}, {"TEST", TokenType::Test}, {"WHILE", TokenType::While},
    {"UNTIL", TokenType::Until}, {"REPEAT", TokenType::Repeat}, {"FOR", TokenType::For},
    {"FOREACH", TokenType::ForEach},
    {"IN", TokenType::In},
    {"SWITCHON", TokenType::Switchon}, {"CASE", TokenType::Case}, {"DEFAULT", TokenType::Default},
    {"GOTO", TokenType::Goto}, {"RETURN", TokenType::Return}, {"FINISH", TokenType::Finish},
    {"RETAIN", TokenType::Retain},
    {"REMANAGE", TokenType::Remanage},
    {"SEND", TokenType::Send},
    {"ACCEPT", TokenType::Accept},
    {"FLOAT", TokenType::FLOAT}, {"FIX", TokenType::FIX}, {"FLET", TokenType::FLet},
    {"FSQRT", TokenType::FSQRT}, // Added FSQRT as a keyword
    {"ENTIER", TokenType::ENTIER}, // Added ENTIER as a keyword
    {"TRUNC", TokenType::TRUNC}, // Added TRUNC as a keyword
    {"LOOP", TokenType::Loop}, {"ENDCASE", TokenType::Endcase}, {"RESULTIS", TokenType::Resultis},
    {"VALOF", TokenType::Valof}, {"FVALOF", TokenType::FValof}, {"BE", TokenType::Be}, {"TO", TokenType::To},
    {"BY", TokenType::By}, {"INTO", TokenType::Into}, {"DO", TokenType::Do}, {"THEN", TokenType::Then}, {"ELSE", TokenType::Else},
    {"FREEVEC", TokenType::FREEVEC},
    {"FREELIST", TokenType::FREEVEC},
    {"DEFER", TokenType::Defer},
    {"CLASS", TokenType::Class},
    {"EXTENDS", TokenType::Extends},
    {"NEW", TokenType::New},
    {"DECL", TokenType::Decl}, // <-- ADD THIS
    {"PUBLIC", TokenType::Public},
    {"PRIVATE", TokenType::Private},
    {"PROTECTED", TokenType::Protected},
    {"OR", TokenType::LogicalOr}, {"TRUE", TokenType::BooleanLiteral}, {"FALSE", TokenType::BooleanLiteral},
    {"REM", TokenType::Remainder}, {"EQV", TokenType::Equivalence}, {"NEQV", TokenType::NotEquivalence},
    {"BREAK", TokenType::Break}, {"GET", TokenType::Get},
    {"STRING", TokenType::String}, {"BRK", TokenType::Brk},
    {"TABLE", TokenType::Table}, {"FTABLE", TokenType::FTable},
    {"LEN", TokenType::Len},
    {"LIST", TokenType::List},
    {"MANIFESTLIST", TokenType::ManifestList},
    {"HD", TokenType::HD},
    {"TL", TokenType::TL},
    {"TYPEOF", TokenType::TypeOf},
    {"FIRST", TokenType::HD},
    {"REST", TokenType::REST},
    {"CONCAT", TokenType::Identifier},
    {"COPYLIST", TokenType::Identifier},
    {"DEEPCOPYLIST", TokenType::Identifier},
    {"FIND", TokenType::Identifier},
    {"FILTER", TokenType::Identifier},
    {"SELF", TokenType::Identifier},
    {"SUPER", TokenType::Super},

    // --- NEW BUILT-IN TYPE CONSTANTS ---
    {"TYPE_INT",    TokenType::TypeInt},
    {"TYPE_FLOAT",  TokenType::TypeFloat},
    {"TYPE_STRING", TokenType::TypeString},
    {"TYPE_LIST",   TokenType::TypeList},
    {"TYPE_VEC",    TokenType::TypeVec},
    // {"TYPE_POINTER",TokenType::TypePointer}, // <-- REMOVED
    {"TYPE_CONST",  TokenType::TypeConst},
    {"TYPE_ANY",    TokenType::TypeAny},
    {"TYPE_TABLE",  TokenType::TypeTable},
    // --- END ---

    // --- NEW TYPE ANNOTATION KEYWORDS ---
    {"AS", TokenType::As},
    {"POINTER", TokenType::Pointer},
    {"OF", TokenType::Of},
    // --- END ---
};

Lexer::Lexer(std::string source, bool trace)
    : source_(std::move(source)),
      position_(0),
      line_(1),
      column_(1),
      trace_enabled_(trace),
      last_token_was_value_(false) // Initialize the state flag
{
    if (trace_enabled_) {
        LexerTrace("Lexer initialized. Trace enabled.");
    }
}

Token Lexer::get_next_token() {
    skip_whitespace_and_comments();

    if (is_at_end()) {
        last_token_was_value_ = false;
        return make_token(TokenType::Eof);
    }

    char current_char = peek_char();
    Token token;

    // CORRECTED: Handle multi-character and single-character block delimiters first.
    if (current_char == '$' && peek_next_char() == '(') {
        advance(); advance();
        token = make_token(TokenType::LBrace);
    } else if (current_char == '$' && peek_next_char() == ')') {
        advance(); advance();
        token = make_token(TokenType::RBrace);
    } else if (current_char == '{') {
        advance();
        token = make_token(TokenType::LBrace);
    } else if (current_char == '}') {
        advance();
        token = make_token(TokenType::RBrace);
    } else if (std::isalpha(current_char) || current_char == '_') {
        token = scan_identifier_or_keyword();
    } else if (std::isdigit(current_char) || current_char == '#') {
        token = scan_number();
    } else if (current_char == '"') {
        token = scan_string();
    } else if (current_char == '\'') {
        token = scan_char();
    } else {
        token = scan_operator();
    }

    // Update the state based on the type of token we are about to return.
    // This makes the lexer ready for the *next* token.
    switch(token.type) {
        case TokenType::Identifier:
        case TokenType::NumberLiteral:
        case TokenType::StringLiteral:
        case TokenType::CharLiteral:
        case TokenType::BooleanLiteral: // Keywords like TRUE/FALSE are values
        case TokenType::RParen:         // An expression in parentheses is a value
        case TokenType::RBrace:         // A VALOF block is a value
            last_token_was_value_ = true;
            break;
        default:
            last_token_was_value_ = false;
            break;
    }

    return token;
}
