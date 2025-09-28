#!/bin/bash
# buildstatic.sh
set -e

# --- Configuration ---
RUNTIME_DIR="runtime"
BUILD_DIR="${RUNTIME_DIR}/build"
TOP_LEVEL_DIR="."
ERRORS_FILE="errors_static.txt"

# --- Build Process ---
echo "Building standalone C runtime library (libbcpl_runtime_c.a)..."
> "${ERRORS_FILE}"
mkdir -p "${BUILD_DIR}"

# Configure the project ONCE.
cmake -S "${RUNTIME_DIR}" -B "${BUILD_DIR}" 2>> "${ERRORS_FILE}"

# Build the specific 'bcpl_runtime_c' target.
cmake --build "${BUILD_DIR}" --target bcpl_runtime_c 2>> "${ERRORS_FILE}"

# --- Create Combined Object Files and Copy Artifacts ---
echo "Copying standalone library..."
cp "${BUILD_DIR}/libbcpl_runtime_c.a" "${TOP_LEVEL_DIR}/" 2>> "${ERRORS_FILE}"

echo "Creating combined standalone object file (bcpl_runtime.o)..."
(
  cd "${BUILD_DIR}"
  ar x libbcpl_runtime_c.a
  ld -r -o bcpl_runtime.o CMakeFiles/bcpl_runtime_c.dir/runtime.c.o CMakeFiles/bcpl_runtime_c.dir/standalone_heap.c.o
  cp bcpl_runtime.o "${TOP_LEVEL_DIR}/"
)

echo "Compiling starter.c into starter.o..."
clang -c -o "${TOP_LEVEL_DIR}/starter.o" "${RUNTIME_DIR}/starter.c"

echo "✅ Standalone runtime build complete."
