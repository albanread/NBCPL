#!/bin/bash

# Test script for the BCPL preprocessor
# This script tests the preprocessor functionality with the preprocessor_test.bcl file

# Ensure the build directory exists
mkdir -p build/bin

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${BLUE}Testing BCPL Preprocessor${NC}"
echo "================================================"

# Clean build to ensure the preprocessor is compiled
echo -e "${YELLOW}Building compiler with preprocessor...${NC}"
./build.sh --clean

# First, run with preprocessor enabled (default)
echo -e "\n${YELLOW}Running test with preprocessor enabled:${NC}"
./build/bin/NewBCPL --trace-preprocessor -I./tests/include ./tests/preprocessor_test.bcl --run

# Check exit status
if [ $? -eq 0 ]; then
  echo -e "${GREEN}Test with preprocessor passed!${NC}"
else
  echo -e "${RED}Test with preprocessor failed!${NC}"
fi

# Now run without preprocessor to show the difference
echo -e "\n${YELLOW}Running test with preprocessor disabled:${NC}"
./build/bin/NewBCPL --no-preprocessor ./tests/preprocessor_test.bcl --run 2>&1 | grep -i error

# Check exit status (should fail due to missing includes)
if [ $? -eq 0 ]; then
  echo -e "${GREEN}Test without preprocessor correctly showed errors as expected.${NC}"
else
  echo -e "${RED}Test without preprocessor didn't show expected errors.${NC}"
fi

# Test with include path specified
echo -e "\n${YELLOW}Testing with explicit include path:${NC}"
./build/bin/NewBCPL -I./tests/include ./tests/preprocessor_test.bcl --run

# Check exit status
if [ $? -eq 0 ]; then
  echo -e "${GREEN}Test with explicit include path passed!${NC}"
else
  echo -e "${RED}Test with explicit include path failed!${NC}"
fi

echo -e "\n${BLUE}Preprocessor tests completed${NC}"
