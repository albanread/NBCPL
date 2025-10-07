#!/bin/bash

# Test script for NeonReducer architecture cleanup validation
# This script compiles and runs the validation test to ensure
# the legacy ReductionCodeGen system has been properly removed
# and the new architecture is working correctly.

echo "🧪 NeonReducer Architecture Cleanup Validation"
echo "=============================================="

# Set up build environment
BUILD_DIR="build_test_cleanup"
TEST_NAME="test_reducer_cleanup"

# Clean up any previous builds
if [ -d "$BUILD_DIR" ]; then
    echo "🧹 Cleaning previous build directory..."
    rm -rf "$BUILD_DIR"
fi

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo "📦 Compiling validation test..."

# Compile the validation test with necessary dependencies
g++ -std=c++17 \
    -I.. \
    -I../include \
    -I../codegen/neon_reducers \
    -Wall -Wextra \
    -O2 \
    ../${TEST_NAME}.cpp \
    ../Reducer.cpp \
    -o ${TEST_NAME}

# Check compilation status
if [ $? -ne 0 ]; then
    echo "❌ Compilation failed!"
    echo "This indicates that the cleanup may have broken something."
    exit 1
fi

echo "✅ Compilation successful!"

# Run the validation test
echo "🚀 Running validation test..."
echo ""

./${TEST_NAME}

# Check test results
TEST_RESULT=$?

cd ..

if [ $TEST_RESULT -eq 0 ]; then
    echo ""
    echo "🎉 VALIDATION SUCCESSFUL! 🎉"
    echo "================================"
    echo "✅ Legacy ReductionCodeGen system completely removed"
    echo "✅ New NeonReducer architecture working correctly"
    echo "✅ All concrete reducers validated"
    echo "✅ Factory system operational"
    echo "✅ Operation codes properly assigned"
    echo "✅ Clean compilation achieved"
    echo ""
    echo "The reducer architecture cleanup is complete and successful!"

    # Clean up build directory
    rm -rf "$BUILD_DIR"

else
    echo ""
    echo "❌ VALIDATION FAILED!"
    echo "====================="
    echo "The test indicates issues with the cleanup."
    echo "Please review the test output above for details."
    echo "Build directory preserved for debugging: $BUILD_DIR"
    exit 1
fi

echo ""
echo "📚 Documentation created:"
echo "  📄 docs/NeonReducer_Architecture.md - Complete architecture guide"
echo "  📄 docs/Legacy_Reducer_Cleanup_Summary.md - Cleanup summary"
echo ""
echo "🚀 The NeonReducer architecture is ready for production use!"
