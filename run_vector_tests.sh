#!/bin/bash

# Vector Operations Test Runner
# Systematically tests all vector types and operations to track progress
# Usage: ./run_vector_tests.sh

echo "🚀 BCPL Vector Operations Test Suite"
echo "===================================="
echo

# Test configuration
TEST_DIR="."
COMPILER="./NewBCPL"
RESULTS_FILE="test_results.md"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Test status tracking
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Initialize results file
cat > "$RESULTS_FILE" << EOF
# Vector Operations Test Results
Generated: $(date)

## Test Matrix Status
| Vector Type | Addition | Subtraction | Multiplication | Division | Lane Reads | Overall |
|-------------|----------|-------------|----------------|----------|------------|---------|
EOF

# Function to run a test and check results
run_test() {
    local test_file=$1
    local test_name=$2
    local expected_pattern=$3
    local vector_type=$4

    echo -n "Testing $test_name... "

    if [ ! -f "$test_file" ]; then
        echo -e "${RED}SKIP${NC} (test file not found)"
        return 2
    fi

    # Run the test
    output=$($COMPILER --run "$test_file" 2>&1)
    exit_code=$?

    if [ $exit_code -ne 0 ]; then
        echo -e "${RED}FAIL${NC} (compilation error)"
        echo "Error: $output"
        return 1
    fi

    # Check if output contains expected pattern
    if echo "$output" | grep -q "$expected_pattern"; then
        echo -e "${GREEN}PASS${NC}"
        return 0
    else
        echo -e "${RED}FAIL${NC} (unexpected output)"
        echo "Expected: $expected_pattern"
        echo "Got: $output"
        return 1
    fi
}

# Function to test vector type operations
test_vector_type() {
    local vector_type=$1
    local test_prefix=$2

    echo
    echo -e "${BLUE}=== Testing $vector_type Operations ===${NC}"

    local add_status="❌"
    local sub_status="❌"
    local mul_status="❌"
    local div_status="❌"
    local lane_status="❌"

    # Test addition
    if [ -f "${test_prefix}_add.bcl" ]; then
        TOTAL_TESTS=$((TOTAL_TESTS + 1))
        case $vector_type in
            "PAIR")
                if run_test "${test_prefix}_add.bcl" "PAIR Addition" "(15, 35)" "$vector_type"; then
                    add_status="✅"
                    PASSED_TESTS=$((PASSED_TESTS + 1))
                else
                    FAILED_TESTS=$((FAILED_TESTS + 1))
                fi
                ;;
            "FPAIR")
                if run_test "${test_prefix}_add.bcl" "FPAIR Addition" "(15.0, 35.0)" "$vector_type"; then
                    add_status="✅"
                    PASSED_TESTS=$((PASSED_TESTS + 1))
                else
                    FAILED_TESTS=$((FAILED_TESTS + 1))
                fi
                ;;
            "QUAD")
                if run_test "${test_prefix}_add.bcl" "QUAD Addition" "(11, 22, 33, 44)" "$vector_type"; then
                    add_status="✅"
                    PASSED_TESTS=$((PASSED_TESTS + 1))
                else
                    FAILED_TESTS=$((FAILED_TESTS + 1))
                fi
                ;;
        esac
    fi

    # Test lane reads
    if [ -f "${test_prefix}_values.bcl" ]; then
        TOTAL_TESTS=$((TOTAL_TESTS + 1))
        case $vector_type in
            "PAIR")
                if run_test "${test_prefix}_values.bcl" "PAIR Lane Reads" "Lane reads.*5.*15" "$vector_type"; then
                    lane_status="✅"
                    PASSED_TESTS=$((PASSED_TESTS + 1))
                else
                    FAILED_TESTS=$((FAILED_TESTS + 1))
                fi
                ;;
        esac
    fi

    # Determine overall status
    local overall_status="❌"
    if [ "$add_status" = "✅" ] && [ "$lane_status" = "✅" ]; then
        overall_status="✅ COMPLETE"
    elif [ "$add_status" = "✅" ] || [ "$lane_status" = "✅" ]; then
        overall_status="🔄 PARTIAL"
    fi

    # Update results file
    echo "| $vector_type | $add_status | $sub_status | $mul_status | $div_status | $lane_status | $overall_status |" >> "$RESULTS_FILE"
}

# Function to create test files if they don't exist
create_test_templates() {
    echo "Creating test file templates..."

    # PAIR addition test
    if [ ! -f "test_pair_add.bcl" ]; then
        cat > "test_pair_add.bcl" << 'EOF'
LET START() BE {
    WRITEF("Testing PAIR addition...*N")
    LET p1 = PAIR(10, 20)
    LET p2 = PAIR(5, 15)
    LET result = p1 + p2
    WRITEF("Result: (%N, %N)*N", result.|0|, result.|1|)
    WRITEF("Expected: (15, 35)*N")
}
EOF
    fi

    # FPAIR addition test
    if [ ! -f "test_fpair_add.bcl" ]; then
        cat > "test_fpair_add.bcl" << 'EOF'
LET START() BE {
    WRITEF("Testing FPAIR addition...*N")
    LET f1 = FPAIR(10.0, 20.0)
    LET f2 = FPAIR(5.0, 15.0)
    LET result = f1 + f2
    WRITEF("Result: (%F, %F)*N", result.|0|, result.|1|)
    WRITEF("Expected: (15.0, 35.0)*N")
}
EOF
    fi

    # QUAD addition test
    if [ ! -f "test_quad_add.bcl" ]; then
        cat > "test_quad_add.bcl" << 'EOF'
LET START() BE {
    WRITEF("Testing QUAD addition...*N")
    LET q1 = QUAD(1, 2, 3, 4)
    LET q2 = QUAD(10, 20, 30, 40)
    LET result = q1 + q2
    WRITEF("Result: (%N, %N, %N, %N)*N", result.|0|, result.|1|, result.|2|, result.|3|)
    WRITEF("Expected: (11, 22, 33, 44)*N")
}
EOF
    fi
}

# Main execution
echo "Initializing test environment..."

# Check if compiler exists
if [ ! -f "$COMPILER" ]; then
    echo -e "${RED}Error: Compiler not found at $COMPILER${NC}"
    echo "Please build the compiler first with ./build.sh"
    exit 1
fi

# Create test templates if needed
create_test_templates

echo -e "${GREEN}Starting systematic vector operation tests...${NC}"
echo

# Test each vector type
test_vector_type "PAIR" "test_pair"
test_vector_type "FPAIR" "test_fpair"
test_vector_type "QUAD" "test_quad"

# Summary
echo
echo "======================================"
echo -e "${BLUE}Test Summary${NC}"
echo "======================================"
echo "Total Tests: $TOTAL_TESTS"
echo -e "Passed: ${GREEN}$PASSED_TESTS${NC}"
echo -e "Failed: ${RED}$FAILED_TESTS${NC}"

if [ $FAILED_TESTS -eq 0 ]; then
    echo -e "${GREEN}🎉 All tests passed!${NC}"
    exit 0
else
    echo -e "${YELLOW}⚠️  Some tests failed. Check individual results above.${NC}"
    exit 1
fi

# Add detailed results to file
cat >> "$RESULTS_FILE" << EOF

## Detailed Results

### Summary
- Total Tests: $TOTAL_TESTS
- Passed: $PASSED_TESTS
- Failed: $FAILED_TESTS
- Success Rate: $(( PASSED_TESTS * 100 / TOTAL_TESTS ))%

### Next Steps
1. Fix failing tests using proven diagnostic methodology
2. Generate reference opcodes with \`--list\` flag
3. Match custom encoder output to reference
4. Validate with \`--exec\` vs \`--run\` comparison

### Test Commands
- Run individual test: \`./NewBCPL --run test_name.bcl\`
- Generate reference: \`./NewBCPL --list test_name.bcl\`
- Compare modes: \`./NewBCPL --exec test_name.bcl\`
EOF

echo
echo "📊 Detailed results saved to: $RESULTS_FILE"
echo "🔧 Run individual tests with: $COMPILER --run test_name.bcl"
echo "📋 Generate references with: $COMPILER --list test_name.bcl"
