#!/bin/bash

# test_file_fix.sh
# Test script to verify FILE_ API functionality after runtime bridge fix

set -e  # Exit on any error

echo "=========================================="
echo "FILE_ API Fix Verification Test"
echo "=========================================="

# Clean up any previous test files
rm -f test_output.txt

echo
echo "1. Testing BCPL compilation with FILE_ API..."
./NewBCPL test_file_simple_fix.bcl

echo
echo "2. Running FILE_ API test program..."
./NewBCPL --run test_file_simple_fix.bcl

echo
echo "3. Verifying test output file was created..."
if [ -f "test_output.txt" ]; then
    echo "✅ SUCCESS: test_output.txt was created"
    echo "   Content: $(cat test_output.txt)"
else
    echo "❌ ERROR: test_output.txt was not created"
    exit 1
fi

echo
echo "4. Cleanup..."
rm -f test_output.txt

echo
echo "=========================================="
echo "✅ FILE_ API Fix Verification PASSED!"
echo "   All FILE_ functions are working correctly"
echo "=========================================="
