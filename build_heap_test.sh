#!/bin/bash

# build_heap_test.sh
# Build script for HeapManager test program

set -e  # Exit on any error

echo "HeapManager Test Builder"
echo "======================="

# Check if we're in the right directory
if [ ! -f "HeapManager/HeapManager.h" ]; then
    echo "ERROR: Must run from NewBCPL root directory"
    echo "Expected to find HeapManager/HeapManager.h"
    exit 1
fi

# Check if unified runtime exists
RUNTIME_LIB="./libbcpl_runtime_sdl2_static.a"
if [ ! -f "$RUNTIME_LIB" ]; then
    echo "ERROR: Runtime library not found: $RUNTIME_LIB"
    echo "Please build the main project first:"
    echo "  ./build.sh"
    exit 1
fi

echo "✓ Runtime library found: $RUNTIME_LIB"

# Clean previous build
echo "Cleaning previous build..."
make -f Makefile.heap_test clean > /dev/null 2>&1 || true

# Build the test
echo "Building HeapManager test..."
if make -f Makefile.heap_test all; then
    echo "✓ Build successful!"

    # Check if executable was created
    if [ -f "test_heap_manager" ]; then
        echo "✓ Executable created: test_heap_manager"

        # Ask if user wants to run tests immediately
        echo ""
        read -p "Run tests now? (y/n): " -r
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            echo ""
            echo "Running HeapManager tests..."
            echo "==========================="
            ./test_heap_manager
        else
            echo ""
            echo "Test executable ready. Run with:"
            echo "  ./test_heap_manager"
            echo "Or:"
            echo "  make -f Makefile.heap_test test"
        fi
    else
        echo "ERROR: Executable not created!"
        exit 1
    fi
else
    echo "ERROR: Build failed!"
    echo ""
    echo "Common issues:"
    echo "- Missing includes or headers"
    echo "- Runtime library version mismatch"
    echo "- Missing dependencies"
    echo ""
    echo "Try a debug build for more info:"
    echo "  make -f Makefile.heap_test debug"
    exit 1
fi

echo ""
echo "Build complete!"
