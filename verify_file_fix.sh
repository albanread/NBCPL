#!/bin/bash

# verify_file_fix.sh
# Simple test script to verify FILE_ API fixes are working

set -e  # Exit on any error

echo "=================================================="
echo "FILE_ API Fix Verification Script"
echo "=================================================="

# Function to print status messages
print_status() {
    echo "[$1] $2"
}

print_status "INFO" "Starting FILE_ API fix verification..."

# Clean up any previous test files
rm -f test_output.txt verify_test_result.txt

# Create a simple test BCPL program
cat > verify_file_test.bcl << 'EOF'
// verify_file_test.bcl
// Simple test to verify FILE_ API functionality

LET START() = VALOF {
    LET filename = "verify_test_result.txt"
    LET handle = 0
    LET result = 0

    WRITEF("FILE_ API Verification Test*N")

    // Test 1: Open file for writing
    handle := FILE_OPEN_WRITE(filename)
    TEST handle = 0 THEN {
        WRITEF("ERROR: Failed to open file for writing*N")
        RESULTIS 1
    } ELSE {
        WRITEF("SUCCESS: File opened for writing*N")
    }

    // Test 2: Write string to file
    result := FILE_WRITES(handle, "FILE_ API is working!")
    TEST result = 0 THEN {
        WRITEF("ERROR: Failed to write to file*N")
        FILE_CLOSE(handle)
        RESULTIS 2
    } ELSE {
        WRITEF("SUCCESS: Wrote %d bytes*N", result)
    }

    // Test 3: Close file
    result := FILE_CLOSE(handle)
    TEST result = 0 THEN {
        WRITEF("SUCCESS: File closed*N")
    } ELSE {
        WRITEF("ERROR: Failed to close file*N")
        RESULTIS 3
    }

    WRITEF("All FILE_ API tests passed!*N")
    RESULTIS 0
}
EOF

print_status "INFO" "Created test BCPL program"

# Test 1: Compilation
print_status "TEST" "Testing BCPL compilation with FILE_ API..."
if ./NewBCPL verify_file_test.bcl 2>&1; then
    print_status "PASS" "Compilation successful"
else
    print_status "FAIL" "Compilation failed"
    exit 1
fi

# Test 2: Execution
print_status "TEST" "Testing FILE_ API execution..."
if ./NewBCPL --run verify_file_test.bcl 2>&1; then
    print_status "PASS" "Execution successful"
else
    print_status "FAIL" "Execution failed"
    exit 1
fi

# Test 3: Verify output file was created
print_status "TEST" "Verifying output file creation..."
if [ -f "verify_test_result.txt" ]; then
    content=$(cat verify_test_result.txt)
    print_status "PASS" "Output file created with content: '$content'"
else
    print_status "FAIL" "Output file was not created"
    exit 1
fi

# Test 4: Verify file content
expected_content="FILE_ API is working!"
if [ "$content" = "$expected_content" ]; then
    print_status "PASS" "File content matches expected output"
else
    print_status "FAIL" "File content mismatch. Expected: '$expected_content', Got: '$content'"
    exit 1
fi

# Cleanup
rm -f verify_file_test.bcl verify_test_result.txt

print_status "SUCCESS" "All FILE_ API fixes verified successfully!"
echo "=================================================="
echo "✅ FILE_ API is now working correctly"
echo "✅ No hanging or infinite loops detected"
echo "✅ All file operations completed successfully"
echo "=================================================="
