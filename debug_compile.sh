#!/bin/bash

# debug_compile.sh
# Simple debug script to check BCPL compilation step by step

echo "=== BCPL Compilation Debug ==="
echo

echo "1. Checking if NewBCPL compiler exists..."
if [ -f "./NewBCPL" ]; then
    echo "✅ NewBCPL compiler found"
else
    echo "❌ NewBCPL compiler not found"
    exit 1
fi

echo
echo "2. Testing basic compiler invocation..."
./NewBCPL --help 2>&1 | head -5

echo
echo "3. Creating simple test file..."
cat > simple_test.bcl << 'EOF'
LET START() = VALOF {
    WRITEF("Hello World*N")
    RESULTIS 0
}
EOF

echo
echo "4. Testing basic compilation (no FILE_ API)..."
./NewBCPL simple_test.bcl 2>&1

echo
echo "5. Testing execution..."
./NewBCPL --run simple_test.bcl 2>&1

echo
echo "6. Creating FILE_ API test..."
cat > file_test.bcl << 'EOF'
LET START() = VALOF {
    WRITEF("Testing FILE_ API...*N")
    LET handle = FILE_OPEN_WRITE("test.txt")
    WRITEF("File handle: %d*N", handle)
    RESULTIS 0
}
EOF

echo
echo "7. Testing FILE_ API compilation..."
./NewBCPL file_test.bcl 2>&1

echo
echo "8. Testing FILE_ API execution..."
./NewBCPL --run file_test.bcl 2>&1

echo
echo "=== Debug Complete ==="
