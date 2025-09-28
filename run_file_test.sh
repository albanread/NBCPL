#!/bin/bash

# FILE Runtime API Test Runner
# Builds and runs comprehensive tests for all FILE_ commands

set -e

echo "=== FILE Runtime API Test Runner ==="
echo "Building and testing all FILE_ commands..."
echo

# Clean any previous test artifacts
echo "Cleaning previous build artifacts..."
make -f Makefile.file_test clean

# Build the test
echo "Building FILE runtime test..."
make -f Makefile.file_test build

# Run the test
echo "Running FILE runtime tests..."
echo
make -f Makefile.file_test test

echo
echo "=== Test Complete ==="

# Clean up test files
echo "Cleaning up test files..."
make -f Makefile.file_test clean-tests

echo "Done!"
