#!/bin/bash

# Test script to build and test all runtime modes
# This verifies that the unified runtime system works correctly

set -e # Exit on any error

echo "=== Testing All Runtime Modes ==="
echo

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    local status=$1
    local message=$2
    case $status in
        "SUCCESS")
            echo -e "${GREEN}✓ $message${NC}"
            ;;
        "ERROR")
            echo -e "${RED}✗ $message${NC}"
            ;;
        "INFO")
            echo -e "${YELLOW}ℹ $message${NC}"
            ;;
    esac
}

# Function to test a specific runtime mode
test_runtime_mode() {
    local mode=$1
    local mode_name=$2

    echo "----------------------------------------"
    print_status "INFO" "Testing $mode_name runtime mode"
    echo "----------------------------------------"

    # Clean previous builds
    print_status "INFO" "Cleaning previous builds..."
    ./buildruntime --clean > /dev/null 2>&1 || true
    ./build.sh --clean > /dev/null 2>&1 || true

    # Build the runtime library
    print_status "INFO" "Building $mode_name runtime library..."
    if ./buildruntime --$mode > runtime_build.log 2>&1; then
        print_status "SUCCESS" "$mode_name runtime library built successfully"
    else
        print_status "ERROR" "$mode_name runtime library build failed"
        echo "Build log:"
        cat runtime_build.log
        return 1
    fi

    # Build the compiler with the specified runtime
    print_status "INFO" "Building compiler with $mode_name runtime..."
    if ./build.sh --runtime=$mode > compiler_build.log 2>&1; then
        print_status "SUCCESS" "Compiler built successfully with $mode_name runtime"
    else
        print_status "ERROR" "Compiler build failed with $mode_name runtime"
        echo "Build log:"
        cat compiler_build.log
        return 1
    fi

    # Test compilation of the test program
    print_status "INFO" "Compiling test program with $mode_name runtime..."
    if ./build/bin/NewBCPL --run test_unified_runtime.bcl > test_output_$mode.log 2>&1; then
        print_status "SUCCESS" "Test program executed successfully with $mode_name runtime"
        echo "Test output:"
        cat test_output_$mode.log
    else
        print_status "ERROR" "Test program failed with $mode_name runtime"
        echo "Error output:"
        cat test_output_$mode.log
        return 1
    fi

    echo
    return 0
}

# Function to compare runtime outputs
compare_outputs() {
    print_status "INFO" "Comparing outputs across runtime modes..."

    # Extract the actual test results (ignore timing differences)
    for mode in jit standalone unified; do
        if [ -f "test_output_$mode.log" ]; then
            grep -E "(✓|✗)" test_output_$mode.log > results_$mode.txt 2>/dev/null || true
        fi
    done

    # Compare results
    local all_same=true
    if [ -f "results_jit.txt" ] && [ -f "results_standalone.txt" ] && [ -f "results_unified.txt" ]; then
        if ! diff -q results_jit.txt results_standalone.txt > /dev/null; then
            print_status "ERROR" "JIT and standalone results differ"
            all_same=false
        fi
        if ! diff -q results_jit.txt results_unified.txt > /dev/null; then
            print_status "ERROR" "JIT and unified results differ"
            all_same=false
        fi
        if ! diff -q results_standalone.txt results_unified.txt > /dev/null; then
            print_status "ERROR" "Standalone and unified results differ"
            all_same=false
        fi

        if $all_same; then
            print_status "SUCCESS" "All runtime modes produce identical results"
        fi
    else
        print_status "ERROR" "Some result files are missing - cannot compare"
    fi

    # Cleanup comparison files
    rm -f results_*.txt
}

# Main test execution
main() {
    # Check if required files exist
    if [ ! -f "buildruntime" ] || [ ! -f "build.sh" ] || [ ! -f "test_unified_runtime.bcl" ]; then
        print_status "ERROR" "Required files missing. Make sure you're in the NewBCPL directory."
        exit 1
    fi

    # Make scripts executable
    chmod +x buildruntime build.sh

    local failed_tests=0

    # Test JIT mode
    if ! test_runtime_mode "jit" "JIT"; then
        failed_tests=$((failed_tests + 1))
    fi

    # Test standalone mode
    if ! test_runtime_mode "standalone" "Standalone"; then
        failed_tests=$((failed_tests + 1))
    fi

    # Test unified mode
    if ! test_runtime_mode "unified" "Unified"; then
        failed_tests=$((failed_tests + 1))
    fi

    # Compare outputs if all tests passed
    if [ $failed_tests -eq 0 ]; then
        compare_outputs
    fi

    # Final summary
    echo "========================================"
    if [ $failed_tests -eq 0 ]; then
        print_status "SUCCESS" "All runtime modes tested successfully!"
        print_status "INFO" "The unified runtime system is working correctly."
    else
        print_status "ERROR" "$failed_tests runtime mode(s) failed"
        exit 1
    fi

    # Cleanup log files
    rm -f runtime_build.log compiler_build.log test_output_*.log

    echo "========================================"
}

# Trap to cleanup on exit
cleanup() {
    rm -f runtime_build.log compiler_build.log test_output_*.log results_*.txt
}
trap cleanup EXIT

# Run the main function
main "$@"
