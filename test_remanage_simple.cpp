#include "Lexer.h"
#include "Parser.h"
#include "AST.h"
#include <iostream>
#include <memory>

int main() {
    // Test REMANAGE keyword lexing
    std::cout << "Testing REMANAGE lexer..." << std::endl;
    
    std::string source = "REMANAGE x, y, z";
    Lexer lexer(source);
    
    Token token1 = lexer.get_next_token();
    if (token1.type == TokenType::Remanage) {
        std::cout << "✓ REMANAGE token recognized correctly" << std::endl;
    } else {
        std::cout << "✗ REMANAGE token NOT recognized" << std::endl;
        return 1;
    }
    
    // Test REMANAGE statement parsing
    std::cout << "Testing REMANAGE parser..." << std::endl;
    
    std::string program_source = R"(
        START() BE {
            LET x = VEC 10
            RETAIN x
            REMANAGE x
        }
    )";
    
    try {
        Lexer program_lexer(program_source);
        Parser parser(program_lexer);
        
        auto program = parser.parse_program();
        if (program) {
            std::cout << "✓ Program with REMANAGE parsed successfully" << std::endl;
        } else {
            std::cout << "✗ Failed to parse program with REMANAGE" << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cout << "✗ Exception during parsing: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "All REMANAGE tests passed!" << std::endl;
    return 0;
}