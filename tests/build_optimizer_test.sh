#!/bin/bash

# Exit on any error
set -e

# Compile the PeepholeOptimizer test
echo "Compiling PeepholeOptimizer test..."

clang++ -g -std=c++17 \
  -I.. \
  -I../include \
  -o test_peephole_optimizer \
  test_peephole_optimizer.cpp \
  ../PeepholeOptimizer.cpp \
  ../InstructionComparator.cpp \
  ../InstructionStream.cpp

# Make sure the object files exist
# (test will still run if they don't, but with limited functionality)
echo "Checking object directory structure..."
if [ ! -d "../build/obj" ]; then
  mkdir -p "../build/obj"
fi

# Make the test executable
chmod +x test_peephole_optimizer

# Run the test
echo "Running PeepholeOptimizer test..."
./test_peephole_optimizer

echo "Done!"
