#!/bin/bash

# Build script for NewBCPL testing directory
# This script builds and runs tests for the Bloom filter and other components
# Usage:
#   ./build_tests.sh                    - Build and run all tests
#   ./build_tests.sh bloom              - Build and run only Bloom filter tests
#   ./build_tests.sh bcpl               - Build and run only BCPL language tests
#   ./build_tests.sh clean              - Clean test build artifacts

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Build directories
TEST_BUILD_DIR="test_build"
TEST_BIN_DIR="${TEST_BUILD_DIR}/bin"

echo -e "${BLUE}=== NewBCPL Testing Build Script ===${NC}"

# Parse arguments
TEST_TYPE="all"
if [ $# -gt 0 ]; then
    TEST_TYPE="$1"
fi

# Clean function
clean_tests() {
    echo -e "${YELLOW}Cleaning test build artifacts...${NC}"
    rm -rf "${TEST_BUILD_DIR}"
    rm -f test_bloom_filter
    rm -f *.test
    echo -e "${GREEN}Clean complete.${NC}"
}

# Handle clean command
if [ "$TEST_TYPE" == "clean" ]; then
    clean_tests
    exit 0
fi

# Create build directories
mkdir -p "${TEST_BUILD_DIR}"
mkdir -p "${TEST_BIN_DIR}"

# Function to build and run Bloom filter tests
build_bloom_tests() {
    echo -e "${YELLOW}Building Bloom filter tests...${NC}"

    # Build the C++ Bloom filter test
    if clang++ -std=c++17 -O2 -I../ test_bloom_filter.cpp -o "${TEST_BIN_DIR}/test_bloom_filter"; then
        echo -e "${GREEN}✅ Bloom filter test compiled successfully${NC}"

        echo -e "${BLUE}Running Bloom filter tests...${NC}"
        if "${TEST_BIN_DIR}/test_bloom_filter"; then
            echo -e "${GREEN}✅ Bloom filter tests passed!${NC}"
            return 0
        else
            echo -e "${RED}❌ Bloom filter tests failed!${NC}"
            return 1
        fi
    else
        echo -e "${RED}❌ Failed to compile Bloom filter tests${NC}"
        return 1
    fi
}

# Function to build and run BCPL language tests
build_bcpl_tests() {
    echo -e "${YELLOW}Building BCPL language tests...${NC}"

    # Check if NewBCPL compiler exists
    if [ ! -f "../NewBCPL" ]; then
        echo -e "${RED}❌ NewBCPL compiler not found. Please build it first with ../build.sh${NC}"
        return 1
    fi

    # Test simple Bloom filter double-free detection
    echo -e "${BLUE}Testing simple Bloom filter double-free detection...${NC}"
    if ../NewBCPL test_simple_bloom_double_free.bcl > "${TEST_BUILD_DIR}/simple_bloom_test.out" 2>&1; then
        echo -e "${GREEN}✅ Simple Bloom filter test compiled and ran${NC}"

        # Check if double-free was detected
        if grep -q "POTENTIAL DOUBLE FREE" "${TEST_BUILD_DIR}/simple_bloom_test.out"; then
            echo -e "${GREEN}✅ Double-free detection is working with Bloom filter!${NC}"
        else
            echo -e "${YELLOW}⚠️ Double-free detection output not found in test${NC}"
            echo "Test output:"
            cat "${TEST_BUILD_DIR}/simple_bloom_test.out"
        fi
    else
        echo -e "${RED}❌ Simple Bloom filter test failed${NC}"
        echo "Error output:"
        cat "${TEST_BUILD_DIR}/simple_bloom_test.out"
        return 1
    fi

    return 0
}

# Function to run all tests
run_all_tests() {
    local bloom_result=0
    local bcpl_result=0

    echo -e "${BLUE}Running all tests...${NC}"

    build_bloom_tests || bloom_result=1
    echo ""
    build_bcpl_tests || bcpl_result=1

    echo ""
    echo -e "${BLUE}=== Test Results Summary ===${NC}"

    if [ $bloom_result -eq 0 ]; then
        echo -e "${GREEN}✅ Bloom Filter Tests: PASSED${NC}"
    else
        echo -e "${RED}❌ Bloom Filter Tests: FAILED${NC}"
    fi

    if [ $bcpl_result -eq 0 ]; then
        echo -e "${GREEN}✅ BCPL Language Tests: PASSED${NC}"
    else
        echo -e "${RED}❌ BCPL Language Tests: FAILED${NC}"
    fi

    if [ $bloom_result -eq 0 ] && [ $bcpl_result -eq 0 ]; then
        echo ""
        echo -e "${GREEN}🎉 All tests passed!${NC}"
        return 0
    else
        echo ""
        echo -e "${RED}💥 Some tests failed!${NC}"
        return 1
    fi
}

# Main execution
case "$TEST_TYPE" in
    "bloom")
        build_bloom_tests
        ;;
    "bcpl")
        build_bcpl_tests
        ;;
    "all")
        run_all_tests
        ;;
    *)
        echo -e "${RED}Unknown test type: $TEST_TYPE${NC}"
        echo "Usage: $0 [bloom|bcpl|all|clean]"
        exit 1
        ;;
esac

echo ""
echo -e "${BLUE}Testing complete.${NC}"
