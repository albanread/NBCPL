#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>

enum class TokenType {
    Eof, Error,
    Let, Manifest, Static, FStatic, Table, FTable, Len, Global, Globals, Function, Routine, And,
    Retain,
    Remanage,
    Send,
    Accept,
    Virtual, Final,
    Class, Extends, New, Decl, // Added Decl for DECL keyword
    Public, Private, Protected, // Added for visibility modifiers
    Vec, FVec, Pair, FPair, Quad, Oct, FOct, If, Unless, Test, While, Until, Repeat, For, ForEach, FForeach, In, Switchon,
    Case, Default, Goto, Return, Finish, Loop, Endcase, Resultis,
    Valof, FValof, Be, To, By, Into, Do, Or, Break, Get, FLet,
    Then, Else,
    String, Brk, FREEVEC,
    As, Pointer, Of, // --- Added for type annotation syntax ---
    Super, // Added for SUPER keyword
    Identifier, NumberLiteral, StringLiteral, CharLiteral, BooleanLiteral, NullLiteral,
    Assign, Plus, Minus, Multiply, Divide, Remainder, Equal, NotEqual,
    Less, LessEqual, Greater, GreaterEqual, LogicalAnd, BitwiseAnd, LogicalOr, BitwiseOr,     BitwiseNot,
    Dot, // Added Dot for '.' operator
    Pipe, // Added Pipe for '|' operator (lane access)
    MemberAccess, // New: For . and ! operators (legacy, keep for compatibility)
    LogicalNot,
    Equivalence, NotEquivalence, LeftShift, RightShift, Conditional,
    AddressOf, Indirection, VecIndirection, CharIndirection, CharVectorIndirection,
    Bitfield, // For the %% operator
    FloatVecIndirection,
    FLOAT,
    FIX,           // Integer conversion intrinsic
    FSQRT,
    ENTIER,        // ADD THIS LINE
    TRUNC,         // ADD THIS LINE
    List,          // LIST keyword for list expressions
    ManifestList,  // MANIFESTLIST keyword for manifest (read-only) lists

    // --- NEW BUILT-IN TYPE CONSTANTS ---
    TypeInt,
    TypeFloat,
    TypeString,
    TypeList,
    TypeVec,
    TypePointer,
    TypeConst,
    TypeAny,
    TypeTable,
    // --- END ---

    TypeOf,        // TYPEOF keyword for runtime type inspection

    HD,            // Head-of-list operator
    TL,            // Tail-of-list operator
    REST,          // Non-destructive tail-of-list operator
    FREELIST,      // Free a list
    Defer,         // DEFER statement
    LParen, RParen, LBrace, RBrace, LBracket, RBracket, Comma, Semicolon, Colon,
};

std::string to_string(TokenType type);

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    std::string to_string() const;
};

class Lexer {
public:
    Lexer(std::string source, bool trace = false);
    Token get_next_token();
    Token peek() const;

public:
    std::string source_;
private:
    size_t position_;
    int line_;
    int column_;
    bool trace_enabled_;
    bool last_token_was_value_; // Add this state variable
    static const std::unordered_map<std::string, TokenType> keywords_;

    char advance();
    char peek_char() const;
    char peek_next_char() const;
    bool is_at_end() const;
    void skip_whitespace_and_comments();

    Token make_token(TokenType type) const;
    Token make_error_token(const std::string& message) const;
    Token scan_identifier_or_keyword();
    Token scan_number();
    Token scan_string();
    Token scan_char();
    Token scan_operator();
    Token make_token_with_value(TokenType type, const std::string& value) const;
};

#endif // LEXER_H