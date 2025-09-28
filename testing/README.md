# NewBCPL Testing Directory

This directory contains tests and testing utilities for the NewBCPL compiler and runtime system.

## Overview

The testing directory is **excluded** from the main build process (`build.sh`) to keep development builds fast and avoid cluttering the main codebase with test artifacts.

## Test Categories

### 1. Bloom Filter Tests (`test_bloom_filter.cpp`)
- **Purpose**: Unit tests for the Bloom filter implementation used in double-free detection
- **Language**: C++
- **Tests**:
  - Basic operations (add/check)
  - False positive behavior
  - Memory usage verification
  - Clear functionality
  - Large-scale operations
  - Double-free detection simulation

### 2. BCPL Language Tests (`*.bcl`)
- **Purpose**: Integration tests for Bloom filter double-free detection in BCPL programs
- **Language**: BCPL
- **Tests**:
  - `test_simple_bloom_double_free.bcl`: Basic double-free detection test
  - `test_bloom_filter_double_free.bcl`: Comprehensive double-free testing

## Building and Running Tests

### Quick Start
```bash
cd testing
./build_tests.sh
```

### Specific Test Types
```bash
# Run only Bloom filter unit tests
./build_tests.sh bloom

# Run only BCPL integration tests
./build_tests.sh bcpl

# Clean test artifacts
./build_tests.sh clean
```

### Manual Testing
```bash
# Compile and run C++ Bloom filter tests manually
clang++ -std=c++17 -O2 -I../ test_bloom_filter.cpp -o test_bloom_filter
./test_bloom_filter

# Run BCPL tests manually (requires NewBCPL compiler to be built)
../NewBCPL test_simple_bloom_double_free.bcl
```

## Test Requirements

### Prerequisites
1. **For Bloom Filter Tests**: 
   - clang++ with C++17 support
   - No external dependencies

2. **For BCPL Tests**:
   - NewBCPL compiler must be built (`../build.sh`)
   - Runtime libraries must be available (`../buildruntime`)

## Expected Test Results

### Bloom Filter Tests
- ✅ Basic operations should pass without false negatives
- ✅ False positive rate should be reasonable (< 10% for test configuration)
- ✅ Memory usage should match expected values
- ✅ Clear functionality should reset the filter
- ✅ Large-scale tests should handle 100K+ elements efficiently

### BCPL Integration Tests
- ✅ Normal vector allocation and free should work
- ✅ Double-free attempts should be detected and reported
- ✅ Runtime metrics should show Bloom filter statistics
- ⚠️  Some false positives may occur (this is expected behavior)

## Bloom Filter Configuration

The tests use different Bloom filter configurations:

| Filter Type | Memory Usage | Capacity | False Positive Rate | Use Case |
|-------------|--------------|----------|-------------------|----------|
| `TestDoubleFreeBloomFilter` | ~60KB | ~100K items | ~1% | Testing |
| `DoubleFreeBloomFilter` | ~1.2MB | ~1M items | ~0.1% | Production |
| `ConservativeDoubleFreeBloomFilter` | ~900KB | ~500K items | ~0.01% | Conservative |

## Debugging Test Failures

### Bloom Filter Test Failures
1. Check compiler version and C++17 support
2. Verify include paths are correct
3. Look for assertion failures in test output

### BCPL Test Failures
1. Ensure NewBCPL compiler is built and working
2. Check that runtime libraries are up to date
3. Verify HeapManager compilation with Bloom filter changes
4. Look for compilation errors in BCPL syntax

### Common Issues
- **"NewBCPL compiler not found"**: Run `../build.sh` first
- **Bloom filter compilation errors**: Check that `BloomFilter.h` is accessible
- **Runtime errors**: Ensure `./buildruntime` was run successfully

## Adding New Tests

### C++ Unit Tests
1. Add test functions to `test_bloom_filter.cpp`
2. Follow the pattern: `test_function_name()`
3. Use `assert()` for verification
4. Add a call to your test in `main()`

### BCPL Integration Tests
1. Create new `.bcl` files in this directory
2. Keep tests simple (compiler is in development)
3. Focus on specific functionality
4. Add test execution to `build_tests.sh`

## Integration with CI/CD

This testing framework is designed to be easily integrated into continuous integration systems:

```bash
# Example CI script
cd NewBCPL
./buildruntime
./build.sh
cd testing
./build_tests.sh
```

## Performance Considerations

- Bloom filter tests run quickly (< 1 second)
- BCPL tests depend on compiler performance
- Large-scale tests may take a few seconds
- Memory usage tests verify efficiency claims

## Troubleshooting

If tests fail consistently:

1. **Check Dependencies**: Ensure all required tools are installed
2. **Verify Build**: Make sure main NewBCPL build succeeds
3. **Check Output**: Look at test output files in `test_build/`
4. **Manual Testing**: Try running components individually
5. **Clean Build**: Use `./build_tests.sh clean` and retry

## Future Enhancements

Planned testing improvements:
- Automated performance benchmarks
- Memory leak detection tests
- Stress testing with large allocations
- Comparison with std::unordered_set performance
- Cross-platform testing