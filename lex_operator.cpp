#include "Lexer.h"
#include <string>
#include <cctype>

Token Lexer::scan_operator() {
    char c = advance();

    // Handle textual "OR" as LogicalOr
    if (c == 'O' || c == 'o') {
        if ((peek_char() == 'R' || peek_char() == 'r')) {
            advance();
            // Make sure it's not part of a longer identifier
            if (!std::isalnum(peek_char()) && peek_char() != '_') {
                return make_token(TokenType::LogicalOr);
            }
        }
    }

    switch (c) {
        case '(': return make_token(TokenType::LParen);
        case ')': return make_token(TokenType::RParen);
        case '[': return make_token(TokenType::LBracket);
        case ']': return make_token(TokenType::RBracket);
        case ',': return make_token(TokenType::Comma);
        case ';': return make_token(TokenType::Semicolon);

        // ADD THIS CASE
        case '^': return make_token(TokenType::Pointer);

        case '@': return make_token(TokenType::AddressOf);
        case '.': return make_token(TokenType::Dot);
        case '!':
            if (last_token_was_value_) {
                // Infix: variable!expression (legacy vector access)
                return make_token(TokenType::VecIndirection);
            } else {
                // Prefix: !variable
                return make_token(TokenType::Indirection);
            }
        case '%':
            if (peek_char() == '%') {
                advance(); // Consume the second '%'
                return make_token(TokenType::Bitfield);
            } else {
                if (last_token_was_value_) {
                    // Infix: variable%expression
                    return make_token(TokenType::CharVectorIndirection);
                } else {
                    // Prefix: %variable
                    return make_token(TokenType::CharIndirection);
                }
            }
        case '&':
            if (peek_char() == '&') {
                advance();
                return make_token(TokenType::LogicalAnd);
            }
            return make_token(TokenType::BitwiseAnd);
        case '|':
            if (peek_char() == '|') {
                advance();
                return make_token(TokenType::LogicalOr);
            }
            return make_token(TokenType::BitwiseOr);
        case '?': return make_token(TokenType::NullLiteral);
        case '+':
            return make_token(TokenType::Plus);
        case '*':
            return make_token(TokenType::Multiply);
        case '/':
            return make_token(TokenType::Divide);
        case ':':
            if (peek_char() == '=') { advance(); return make_token(TokenType::Assign); }
            return make_token(TokenType::Colon);
        case '~':
            if (peek_char() == '=') {
                advance();
                return make_token(TokenType::NotEqual);
            }
            // Standalone '~' is BitwiseNot
            return make_token(TokenType::BitwiseNot);
        case '<':
            if (peek_char() == '=') {
                advance();
                return make_token(TokenType::LessEqual);
            }
            if (peek_char() == '<') { advance(); return make_token(TokenType::LeftShift); }
            return make_token(TokenType::Less);
        case '>':
            if (peek_char() == '=') {
                advance();
                return make_token(TokenType::GreaterEqual);
            }
            if (peek_char() == '>') { advance(); return make_token(TokenType::RightShift); }
            return make_token(TokenType::Greater);
        case '=':
             return make_token(TokenType::Equal);
        case '-':
            if (peek_char() == '>') { advance(); return make_token(TokenType::Conditional); }
            return make_token(TokenType::Minus);
        case '$':
            if (peek_char() == '(') { advance(); return make_token(TokenType::LBrace); }
            break;
        case '#':
             if (peek_char() == '%') { advance(); return make_token(TokenType::FloatVecIndirection); }
             break;
    }
    return make_error_token("Unexpected character: " + std::string(1, c));
}
