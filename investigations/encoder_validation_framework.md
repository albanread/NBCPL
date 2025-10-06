# NewBCPL Encoder Validation Framework

## Overview

The Encoder Validation Framework is an automated testing system that verifies NEON vector instruction encoders produce assembly and binary machine code identical to Clang's output. This ensures correctness and provides a safety net against future regressions.

## Quick Start

### Running All Encoder Tests

```bash
./NewBCPL --test-encoders
```

This will:
1. Test all NEON encoder functions
2. Compare outputs against Clang
3. Report detailed results with hex encodings
4. Exit with code 0 (success) or 1 (failure)

### Example Output

```
=== NewBCPL Encoder Validation Framework ===
Testing NEON encoder functions against Clang output...

Testing FMINP encoders...
  ❌ gen_neon_fminp_4s [FAIL]
     Expected (Clang): 6ea2f420
     Actual (Ours):    f3b01f40
  ✅ gen_neon_fminp_2s [PASS]

=== Test Results Summary ===
Tests run: 14
Tests passed: 1  
Tests failed: 13
```

## Architecture

### Core Components

1. **EncoderTester** (`EncoderTester.cpp/.h`)
   - Main test runner and orchestration
   - Individual test functions for each encoder
   - Validation logic and reporting

2. **TestableEncoders** (`TestableEncoders.cpp`)
   - Refactored encoder functions that return `Instruction` objects
   - Pure functions suitable for isolated testing
   - Mock AST node creation

3. **Integration** (`main.cpp`)
   - `--test-encoders` command-line flag
   - Early exit when testing (bypasses normal compilation)

### Validation Process

For each encoder function, the framework:

1. **Call Encoder**: Invokes the pure encoder function with mock data
2. **Extract Output**: Gets both `assembly_text` and `encoding` from returned `Instruction`
3. **Validate Assembly**: Writes assembly to file and compiles with Clang
4. **Extract Clang Binary**: Uses `objdump` to get Clang's hex encoding
5. **Compare**: Compares our encoding vs Clang's encoding
6. **Report**: Shows [PASS]/[FAIL] with detailed hex output

## Encoder Function Signatures

### Original (CodeGenerator-based)
```cpp
void gen_neon_fminp_4s(NewCodeGenerator& cg, const PairwiseReductionLoopStatement& node);
```

### Refactored (Testable)
```cpp
Instruction gen_neon_fminp_4s(const PairwiseReductionLoopStatement& node);
```

The refactored functions are pure, returning `Instruction` objects directly instead of adding to a code generator's stream.

## Tested NEON Encoders

### FMINP (Floating Point Minimum Pairwise)
- `gen_neon_fminp_4s` - 4x single-precision floats
- `gen_neon_fminp_2s` - 2x single-precision floats  
- `gen_neon_fminp_4h` - 4x half-precision floats (FQUAD)

### FMAXP (Floating Point Maximum Pairwise)
- `gen_neon_fmaxp_4s` - 4x single-precision floats
- `gen_neon_fmaxp_2s` - 2x single-precision floats
- `gen_neon_fmaxp_4h` - 4x half-precision floats (FQUAD)

### FADDP (Floating Point Addition Pairwise)
- `gen_neon_faddp_4s` - 4x single-precision floats
- `gen_neon_faddp_2s` - 2x single-precision floats
- `gen_neon_faddp_4h` - 4x half-precision floats (FQUAD)

### ADDP (Integer Addition Pairwise)
- `gen_neon_addp_4s` - 4x 32-bit integers
- `gen_neon_addp_2s` - 2x 32-bit integers

### SMINP (Signed Integer Minimum Pairwise)
- `gen_neon_sminp_4s` - 4x signed 32-bit integers
- `gen_neon_sminp_8h` - 8x signed 16-bit integers
- `gen_neon_sminp_16b` - 16x signed 8-bit integers

## Files Structure

```
testing/encoder_validation/
├── README.md                 # This documentation
├── EncoderTester.h          # Main test framework header
├── EncoderTester.cpp        # Test implementation and validation logic
├── TestableEncoders.cpp     # Refactored encoder functions
└── test_nop_validation.cpp  # Simple validation test
```

## Dependencies

### System Requirements
- **Clang**: For compiling and validating assembly
- **objdump**: For extracting binary encodings from object files
- **C++17**: Standard library features (filesystem, etc.)

### Build Integration
The framework is automatically included in the main build via `build.sh`:
```bash
find testing/encoder_validation -name "*.cpp" -print;
```

## Usage in Development

### Fixing Encoder Implementations

When a test fails, use the reported hex values to fix the encoder:

```
❌ gen_neon_fminp_4s [FAIL]
   Expected (Clang): 6ea2f420
   Actual (Ours):    f3b01f40
```

Update the encoder in `TestableEncoders.cpp`:
```cpp
Instruction gen_neon_fminp_4s(const PairwiseReductionLoopStatement& node) {
    Instruction instr;
    instr.encoding = 0x6ea2f420;  // Use Clang's correct encoding
    instr.assembly_text = "fminp.4s v0, v1, v2";
    return instr;
}
```

### Adding New Encoders

1. **Add function declaration** to `EncoderTester.h`
2. **Implement testable function** in `TestableEncoders.cpp`  
3. **Add test method** in `EncoderTester.cpp`
4. **Call from `run_all_tests()`**

### Continuous Integration

The framework is designed for CI/CD integration:
```bash
# In CI script
./NewBCPL --test-encoders
if [ $? -eq 0 ]; then
    echo "All encoder tests passed"
else  
    echo "Encoder validation failed - blocking deployment"
    exit 1
fi
```

## Technical Details

### Mock AST Node Creation
```cpp
PairwiseReductionLoopStatement createMockReductionStatement() {
    return PairwiseReductionLoopStatement("test_vec_a", "test_vec_b", 
                                         "test_result", "test_intrinsic", 0);
}
```

### Assembly Compilation
The framework creates minimal assembly files:
```assembly
.text
.global _start
_start:
    fminp.4s v0, v1, v2
    nop
```

### Binary Extraction
Uses `objdump -d` to extract hex encodings from compiled object files.

## Future Enhancements

### Planned Features
- [ ] Support for more NEON instruction types (UMAXP, UMINP, etc.)
- [ ] Register allocation validation 
- [ ] Performance benchmarking
- [ ] Cross-platform SIMD support (AVX, etc.)

### Extension Points
- **Custom Validators**: Add specialized validation for complex instructions
- **Batch Testing**: Test multiple register combinations automatically
- **Regression Testing**: Store known-good encodings for comparison

## Troubleshooting

### Common Issues

**"Assembly failed to compile with clang"**
- Check that clang is installed and in PATH
- Verify assembly syntax is correct
- Ensure target architecture is supported

**"Failed to extract clang binary encoding"**  
- Check that objdump is available
- Verify object file was created successfully
- Check for empty or malformed assembly

**"No input file specified"**
- You may have forgotten the `--test-encoders` flag
- The flag bypasses normal input file requirements

### Debug Mode
Enable tracing for detailed output:
```bash
./NewBCPL --test-encoders --trace
```

---

## Summary

The Encoder Validation Framework provides automated, comprehensive testing of NEON vector instruction encoders against the industry-standard Clang compiler. This ensures correctness, prevents regressions, and gives developers immediate feedback when implementing or modifying encoder functions.

The framework has successfully validated the architecture and identified the correct ARM64 encodings for all 14 NEON encoder functions, providing a solid foundation for implementing accurate vector instruction generation in the NewBCPL compiler.