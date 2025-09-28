// simple_token_test.cpp
// Simple focused test for compiler token objects
// Tests basic allocation and performance patterns

#include <iostream>
#include <vector>
#include <chrono>
#include <string>
#include <cassert>

// Simple token type enum (subset for testing)
enum class TokenType {
    Identifier,
    NumberLiteral,
    StringLiteral,
    Plus,
    Minus,
    EOF_TOKEN
};

const char* token_type_to_string(TokenType type) {
    switch (type) {
        case TokenType::Identifier: return "IDENTIFIER";
        case TokenType::NumberLiteral: return "NUMBER";
        case TokenType::StringLiteral: return "STRING";
        case TokenType::Plus: return "PLUS";
        case TokenType::Minus: return "MINUS";
        case TokenType::EOF_TOKEN: return "EOF";
        default: return "UNKNOWN";
    }
}

// Simple compiler token with small string optimization
class CompilerToken {
public:
    TokenType type;
    int32_t line;
    int32_t column;
    
private:
    static constexpr size_t SMALL_STRING_SIZE = 23;
    
    union StringStorage {
        char small_str[SMALL_STRING_SIZE + 1]; // +1 for null terminator
        struct {
            char* ptr;
            size_t length;
            size_t capacity;
        } large_str;
    } value_storage_;
    
    bool is_small_string_;

public:
    CompilerToken(TokenType t, int line_num, int col_num, const std::string& val)
        : type(t), line(line_num), column(col_num) {
        set_value(val);
    }
    
    ~CompilerToken() {
        if (!is_small_string_ && value_storage_.large_str.ptr) {
            delete[] value_storage_.large_str.ptr;
        }
    }
    
    // Move constructor
    CompilerToken(CompilerToken&& other) noexcept
        : type(other.type), line(other.line), column(other.column), is_small_string_(other.is_small_string_) {
        if (is_small_string_) {
            std::strcpy(value_storage_.small_str, other.value_storage_.small_str);
        } else {
            value_storage_.large_str = other.value_storage_.large_str;
            other.value_storage_.large_str.ptr = nullptr; // Take ownership
            other.is_small_string_ = true; // Mark other as using small string to avoid double free
            other.value_storage_.small_str[0] = '\0'; // Clear small string
        }
    }
    
    // Delete copy constructor to avoid issues
    CompilerToken(const CompilerToken&) = delete;
    CompilerToken& operator=(const CompilerToken&) = delete;
    CompilerToken& operator=(CompilerToken&&) = delete;
    
    std::string get_value() const {
        if (is_small_string_) {
            return std::string(value_storage_.small_str);
        } else {
            return std::string(value_storage_.large_str.ptr, value_storage_.large_str.length);
        }
    }
    
    void set_value(const std::string& val) {
        // Clean up existing large string if any
        if (!is_small_string_ && value_storage_.large_str.ptr) {
            delete[] value_storage_.large_str.ptr;
        }
        
        if (val.length() <= SMALL_STRING_SIZE) {
            is_small_string_ = true;
            std::strcpy(value_storage_.small_str, val.c_str());
        } else {
            is_small_string_ = false;
            value_storage_.large_str.length = val.length();
            value_storage_.large_str.capacity = val.length() + 1;
            value_storage_.large_str.ptr = new char[value_storage_.large_str.capacity];
            std::strcpy(value_storage_.large_str.ptr, val.c_str());
        }
    }
    
    bool uses_small_string_optimization() const {
        return is_small_string_;
    }
    
    size_t estimated_size() const {
        if (is_small_string_) {
            return sizeof(*this);
        } else {
            return sizeof(*this) + value_storage_.large_str.capacity;
        }
    }
    
    std::string to_string() const {
        return std::string(token_type_to_string(type)) + 
               " (" + std::to_string(line) + ":" + std::to_string(column) + 
               ") \"" + get_value() + "\"";
    }
};

class SimpleTokenTest {
private:
    std::vector<std::string> sample_identifiers_ = {
        "x", "y", "z", "i", "j", "k", "main", "foo", "bar", "test",
        "variable", "function", "class", "method", "parameter", "result"
    };
    
    std::vector<std::string> sample_numbers_ = {
        "0", "1", "42", "123", "456", "999", "1000", "12345"
    };
    
    std::vector<std::string> sample_strings_ = {
        "hello", "world", "test", "string", "short", "a", "ab", "abc"
    };

public:
    void test_basic_functionality() {
        std::cout << "\n=== Basic Functionality Test ===" << std::endl;
        
        // Test small string optimization
        CompilerToken short_token(TokenType::Identifier, 1, 1, "x");
        assert(short_token.uses_small_string_optimization());
        assert(short_token.get_value() == "x");
        
        // Test large string
        std::string long_value(50, 'a'); // 50 characters
        CompilerToken long_token(TokenType::StringLiteral, 2, 5, long_value);
        assert(!long_token.uses_small_string_optimization());
        assert(long_token.get_value() == long_value);
        
        std::cout << "Short token: " << short_token.to_string() << std::endl;
        std::cout << "Long token size: " << long_token.estimated_size() << " bytes" << std::endl;
        std::cout << "Basic functionality tests passed!" << std::endl;
    }
    
    void test_allocation_performance() {
        std::cout << "\n=== Allocation Performance Test ===" << std::endl;
        
        const int NUM_TOKENS = 50000;
        std::cout << "Creating " << NUM_TOKENS << " tokens..." << std::endl;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        std::vector<std::unique_ptr<CompilerToken>> tokens;
        tokens.reserve(NUM_TOKENS);
        
        for (int i = 0; i < NUM_TOKENS; ++i) {
            TokenType type = (i % 3 == 0) ? TokenType::Identifier :
                           (i % 3 == 1) ? TokenType::NumberLiteral : 
                           TokenType::StringLiteral;
            
            std::string value;
            switch (type) {
                case TokenType::Identifier:
                    value = sample_identifiers_[i % sample_identifiers_.size()];
                    break;
                case TokenType::NumberLiteral:
                    value = sample_numbers_[i % sample_numbers_.size()];
                    break;
                case TokenType::StringLiteral:
                    value = sample_strings_[i % sample_strings_.size()];
                    break;
                default:
                    value = "default";
                    break;
            }
            
            auto token = std::make_unique<CompilerToken>(
                type, (i / 100) + 1, (i % 100) + 1, value
            );
            tokens.push_back(std::move(token));
        }
        
        auto mid = std::chrono::high_resolution_clock::now();
        
        // Test access patterns
        size_t total_chars = 0;
        int small_string_count = 0;
        
        for (const auto& token : tokens) {
            total_chars += token->get_value().length();
            if (token->uses_small_string_optimization()) {
                small_string_count++;
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        
        auto create_time = std::chrono::duration_cast<std::chrono::microseconds>(mid - start);
        auto total_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "Creation time: " << create_time.count() << " μs" << std::endl;
        std::cout << "Total time: " << total_time.count() << " μs" << std::endl;
        std::cout << "Tokens per second: " << (NUM_TOKENS * 1000000LL) / create_time.count() << std::endl;
        std::cout << "Total characters: " << total_chars << std::endl;
        std::cout << "Small string optimized: " << small_string_count << "/" << NUM_TOKENS 
                  << " (" << (100.0 * small_string_count / NUM_TOKENS) << "%)" << std::endl;
    }
    
    void test_memory_efficiency() {
        std::cout << "\n=== Memory Efficiency Test ===" << std::endl;
        
        // Test different string lengths
        std::vector<std::pair<std::string, std::string>> test_cases = {
            {"Short", "x"},
            {"Medium", "variable_name"},
            {"At_limit", std::string(23, 'a')}, // Exactly at small string limit
            {"Over_limit", std::string(24, 'b')}, // Just over the limit
            {"Long", std::string(100, 'c')}      // Much longer
        };
        
        for (const auto& test_case : test_cases) {
            CompilerToken token(TokenType::Identifier, 1, 1, test_case.second);
            std::cout << test_case.first << " (" << test_case.second.length() << " chars): "
                      << token.estimated_size() << " bytes, "
                      << (token.uses_small_string_optimization() ? "small" : "large") << " string"
                      << std::endl;
        }
    }
    
    void run_all_tests() {
        std::cout << "=== Simple Compiler Token Test Suite ===" << std::endl;
        std::cout << "Testing basic token allocation and optimization patterns" << std::endl;
        
        test_basic_functionality();
        test_allocation_performance();
        test_memory_efficiency();
        
        std::cout << "\n=== All Tests Completed Successfully! ===" << std::endl;
    }
};

int main() {
    try {
        SimpleTokenTest test;
        test.run_all_tests();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}