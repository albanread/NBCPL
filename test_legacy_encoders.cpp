#include <iostream>
#include "testing/encoder_validation/EncoderTester.h"

int main() {
    std::cout << "=== Legacy Encoder Integration Test ===" << std::endl;
    
    EncoderTester tester;
    
    // Test basic functionality
    std::cout << "Testing basic NOP encoder..." << std::endl;
    bool nop_result = tester.runValidation("test_nop", create_test_nop_instruction());
    
    if (nop_result) {
        std::cout << "✅ Basic validation framework works!" << std::endl;
    } else {
        std::cout << "❌ Basic validation framework failed!" << std::endl;
        return 1;
    }
    
    // Test individual legacy encoders
    std::cout << "\nTesting legacy scalar encoders..." << std::endl;
    
    // Test scalar NOP
    bool scalar_nop = tester.run_single_test("scalar_nop");
    std::cout << "scalar_nop: " << (scalar_nop ? "PASS" : "FAIL") << std::endl;
    
    // Test scalar ADD
    bool scalar_add = tester.run_single_test("scalar_add_reg");
    std::cout << "scalar_add_reg: " << (scalar_add ? "PASS" : "FAIL") << std::endl;
    
    // Test floating point operations
    std::cout << "\nTesting floating point encoders..." << std::endl;
    bool fp_fadd = tester.run_single_test("fp_fadd_reg");
    std::cout << "fp_fadd_reg: " << (fp_fadd ? "PASS" : "FAIL") << std::endl;
    
    // Test vector operations
    std::cout << "\nTesting vector encoders..." << std::endl;
    bool vec_fadd_4s = tester.run_single_test("vec_fadd_4s");
    std::cout << "vec_fadd_4s: " << (vec_fadd_4s ? "PASS" : "FAIL") << std::endl;
    
    // Test pattern matching
    std::cout << "\nTesting pattern matching..." << std::endl;
    bool scalar_pattern = tester.run_pattern_tests("scalar_*");
    std::cout << "scalar_* pattern: " << (scalar_pattern ? "PASS" : "FAIL") << std::endl;
    
    // Summary
    int total_tests = 6;
    int passed = 0;
    if (scalar_nop) passed++;
    if (scalar_add) passed++;
    if (fp_fadd) passed++;
    if (vec_fadd_4s) passed++;
    if (scalar_pattern) passed++;
    
    std::cout << "\n=== Summary ===" << std::endl;
    std::cout << "Tests passed: " << passed << "/" << total_tests << std::endl;
    
    if (passed == total_tests) {
        std::cout << "✅ All legacy encoder tests PASSED!" << std::endl;
        return 0;
    } else {
        std::cout << "❌ Some legacy encoder tests FAILED!" << std::endl;
        return 1;
    }
}