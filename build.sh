#!/bin/bash

# Build script for NewBCPL compiler
# Usage:
#   ./build.sh                 - Clean build (default, recommended)
#   ./build.sh --incr          - Incremental build (faster for development)
#   ./build.sh --runtime=MODE  - Specify runtime mode (jit, standalone, unified)
#
# Clean builds are the default to ensure reliability and catch issues that
# incremental builds might miss. Use --incr only when doing rapid development.

# Exit immediately if a command exits with a non-zero status
set -e

# Define build directories and error log file
BUILD_DIR="build"
OBJ_DIR="${BUILD_DIR}/obj"
BIN_DIR="${BUILD_DIR}/bin"
ERRORS_FILE="errors.txt"
MAX_JOBS=8 # Number of parallel compilation jobs
RUNTIME_MODE="jit" # Default runtime mode: jit, standalone, or unified

# Clear the errors file at the start of the build
> "${ERRORS_FILE}"

# --- Argument Parsing ---
CLEAN_BUILD=true
for arg in "$@"; do
    if [ "$arg" == "--incr" ]; then
        CLEAN_BUILD=false
    elif [ "$arg" == "--runtime=jit" ]; then
        RUNTIME_MODE="jit"
    elif [ "$arg" == "--runtime=standalone" ]; then
        RUNTIME_MODE="standalone"
    elif [ "$arg" == "--runtime=unified" ]; then
        RUNTIME_MODE="unified"
    fi
done

# --- Build Setup ---
if ${CLEAN_BUILD}; then
    echo "Performing clean build (default)..."
    echo "Cleaning previous build artifacts (excluding runtime libraries)..."
    # Preserve any existing runtime libraries before cleaning
    mkdir -p temp_lib_backup
    if [ -f "libbcpl_runtime_jit.a" ]; then
        cp libbcpl_runtime_jit.a temp_lib_backup/
    fi
    if [ -f "libbcpl_runtime_c.a" ]; then
        cp libbcpl_runtime_c.a temp_lib_backup/
    fi
    if [ -f "libbcpl_runtime.a" ]; then
        cp libbcpl_runtime.a temp_lib_backup/
    fi

    rm -rf "${BUILD_DIR}"

    # Restore runtime libraries if they were backed up
    if [ -f "temp_lib_backup/libbcpl_runtime_jit.a" ]; then
        cp temp_lib_backup/libbcpl_runtime_jit.a ./
    fi
    if [ -f "temp_lib_backup/libbcpl_runtime_c.a" ]; then
        cp temp_lib_backup/libbcpl_runtime_c.a ./
    fi
    if [ -f "temp_lib_backup/libbcpl_runtime.a" ]; then
        cp temp_lib_backup/libbcpl_runtime.a ./
    fi
    rm -rf temp_lib_backup
else
    echo "Performing incremental build (--incr)..."
fi

mkdir -p "${OBJ_DIR}"
mkdir -p "${BIN_DIR}"
echo "Created build directories: ${OBJ_DIR} and ${BIN_DIR}"

# Increment version number (patch) before building
echo "Incrementing version number..."
VERSION_HEADER="version.h"
TMP_HEADER="${VERSION_HEADER}.tmp"

awk '
  /^#define BCPL_VERSION_PATCH/ {
    patch = $3 + 1
    print "#define BCPL_VERSION_PATCH " patch
    next
  }
  { print }
' "$VERSION_HEADER" > "$TMP_HEADER" && mv "$TMP_HEADER" "$VERSION_HEADER"

# Find all potential source files
# Core files (excluding files that are explicitly added elsewhere)
CORE_SRC_FILES=$(find . -maxdepth 1 -name "*.cpp" ! -name "main.cpp" ! -name "live_*.cpp" ! -name "peephole_test.cpp" ! -name "test_sdl2_minimal.cpp" ! -name "test_*.cpp" ! -name "debug_samm_simple.cpp" -print; \
                 find encoders -name "*.cpp" -print; \
                 find passes -name "*.cpp" -print; \
                 find linker_helpers -name "*.cpp" -print; \
                 find codegen -name "*.cpp" -print; \
                 find modules -name "*.cpp" -print; \
                 find testing/encoder_validation -name "*.cpp" ! -name "test_nop_validation.cpp" -print; \
                 find . -name "cf_*.cpp" -print; \
                 find . -name "rm_*.cpp" -print; \
                 find . -name "dg_*.cpp" -print; \
                 find format -name "CodeFormatter.cpp" -print; \
                 echo "InstructionDecoder.cpp"; \
                 )

MAIN_SRC_FILE="main.cpp" # Assuming main.cpp exists and contains the main function

# Generated files from the 'generators' subfolder
GENERATED_SRC_FILES=$(find generators -name "gen_*.cpp" -print)

# Helper files from the 'generators/helpers' subfolder
GENERATORS_HELPER_SRC_FILES=$(find generators/helpers -name "*.cpp" -print)

# Lexer-related files
LEXER_SRC_FILES=$(echo -e "lex_operator.cpp\nlex_scanner.cpp\nlex_tokens.cpp\nlex_utils.cpp")

# Analysis files from the 'analysis' subfolder
ANALYSIS_SRC_FILES=$(find analysis -name "*.cpp" -print)

# Peephole pattern files (now used in main build)
PEEPHOLE_PATTERN_FILES=$(echo -e "optimizer/patterns/AddressFusion.cpp\noptimizer/patterns/AddFolding.cpp\noptimizer/patterns/ConstantFolding.cpp\noptimizer/patterns/ControlFlow.cpp\noptimizer/patterns/MemoryOptimization.cpp\noptimizer/patterns/StrengthReduction.cpp")



# We don't need to add AZ_VISIT_SRC_FILES separately as they're already found by the analysis directory search

# Runtime is now built separately by buildruntime.sh
# We link to libbcpl_runtime_jit.a instead of compiling runtime files directly

# HeapManager files
HEAP_MANAGER_FILES=$(find HeapManager -name "*.cpp" -print 2>/dev/null || echo "")

# Live analysis files
LIVE_ANALYSIS_FILES=$(find . -name "live_*.cpp" -print 2>/dev/null || echo "")

# Signal handling files
SIGNAL_FILES="./SignalSafeUtils.cpp\n./SignalHandler.cpp"

# Combine all source files into a single list
ALL_SRC_FILES_RAW=$(echo -e "${CORE_SRC_FILES}\n${MAIN_SRC_FILE}\n${GENERATED_SRC_FILES}\n${GENERATORS_HELPER_SRC_FILES}\n${ANALYSIS_SRC_FILES}\n${LEXER_SRC_FILES}\n${HEAP_MANAGER_FILES}\n${LIVE_ANALYSIS_FILES}\n${SIGNAL_FILES}\n${PEEPHOLE_PATTERN_FILES}")

# Filter files for compilation (incremental build logic)
FILES_TO_COMPILE=""
if ${CLEAN_BUILD}; then
    FILES_TO_COMPILE="${ALL_SRC_FILES_RAW}"
else
    # Loop through all raw source files to determine which ones need recompilation
    echo "${ALL_SRC_FILES_RAW}" | while IFS= read -r src_file; do
        # Extract base filename and construct object file path
        base_filename=$(basename "${src_file}")
        obj_name="${base_filename%.cpp}.o"
        obj_file="${OBJ_DIR}/${obj_name}"

        # Check if object file does not exist or if source file is newer
        if [ ! -f "${obj_file}" ] || [ "${src_file}" -nt "${obj_file}" ]; then
            FILES_TO_COMPILE+="${src_file}\n" # Add to list if recompilation is needed
        fi
    done
fi

# Remove any trailing newline from the list of files to compile
FILES_TO_COMPILE=$(echo -e "${FILES_TO_COMPILE}" | sed '/^$/d')
echo "Debug: Files to compile:"
echo "${FILES_TO_COMPILE}"

# --- Compilation Phase ---
if [ -z "${FILES_TO_COMPILE}" ] && ! ${CLEAN_BUILD}; then
    echo "No source files need recompilation."
else
    echo "Compiling source files (debug enabled, clang++)..."
    echo "Debug: Starting compilation for the following files:"
    echo "${FILES_TO_COMPILE}"
    # Use xargs for parallel compilation, stopping on the first error.
    # Each compilation command is run in a subshell.
    echo "${FILES_TO_COMPILE}" | xargs -P "${MAX_JOBS}" -I {} bash -c '
        src_file="$1"
        obj_dir="$2"
        errors_file="$3"

        base_filename=$(basename "${src_file}")
        obj_name="${base_filename%.cpp}.o"
        obj_file="${obj_dir}/${obj_name}"

        # Compile the source file:
        # -g: Include debug info
        # -fno-omit-frame-pointer: Ensure frame pointers for easier debugging
        # -std=c++17: Use C++17 standard
        # -DSDL2_RUNTIME_ENABLED: Enable SDL2 runtime support in symbol table
        # -I.: Include current directory
        # -I./NewBCPL: Include NewBCPL directory
        # -I./analysis/az_impl: Include analysis/az_impl directory for modular visitors
        # -c: Compile only, do not link
        # -o: Output object file
        # > /dev/null: Redirect stdout (success messages) to null
        # 2>> "${errors_file}": Append stderr (error messages) to the errors file
        if ! clang++ -g -fno-omit-frame-pointer -std=c++17 -DSDL2_RUNTIME_ENABLED -I. -I./NewBCPL -I./analysis/az_impl -I./analysis -I./ -I./include -I./HeapManager -I./runtime -c "${src_file}" -o "${obj_file}" 2>> "${errors_file}"; then
            # If compilation fails, print an error message to stderr and exit the subshell.
            # Exiting the subshell will cause xargs to stop processing further arguments.
            echo "Error: Compilation failed for ${src_file}. See ${errors_file} for details." >&2
            exit 1
        fi
    ' _ {} "${OBJ_DIR}" "${ERRORS_FILE}" # Pass OBJ_DIR and ERRORS_FILE as arguments to the subshell
fi

# Check if any compilation errors occurred by checking the size of the errors file
if [ -s "${ERRORS_FILE}" ]; then
    echo "----------------------------------------"
    echo "BUILD FAILED: Errors detected during compilation. See '${ERRORS_FILE}' for details." >&2
    exit 1 # Exit the script with an error code
fi

# --- Linking Phase ---
# Use unified runtime with static SDL2 by default (fully self-contained)
RUNTIME_LIB="./libbcpl_runtime_sdl2_static.a"
LINK_DESC="unified runtime library with static SDL2 (self-contained)"
echo "📝 Using unified runtime library with static SDL2"

echo "Linking object files with ${LINK_DESC} (debug enabled, clang++)..."

# Link all object files into the final executable with the unified runtime library
# Note: Order of libraries matters. Put runtime library at the end to ensure all symbols are resolved
# Add -lstdc++ for unified runtime that includes C++ components
# Add macOS system frameworks required for static SDL2 linking
EXTRA_LIBS="-lstdc++ -framework Cocoa -framework CoreVideo -framework IOKit -framework Carbon -framework AudioToolbox -framework ForceFeedback -framework CoreAudio -framework CoreFoundation -framework Foundation -framework GameController -framework CoreHaptics -framework Metal"

if ! clang++ -g -std=c++17 -DSDL2_RUNTIME_ENABLED -I. -I./include -I./HeapManager -I./runtime -o "${BIN_DIR}/NewBCPL" "${OBJ_DIR}"/*.o "${RUNTIME_LIB}" -lpthread ${EXTRA_LIBS} 2>> "${ERRORS_FILE}"; then
    echo "Error: Linking failed. See '${ERRORS_FILE}' for details." >&2
    exit 1
fi

# --- Codesigning Phase ---
echo "Codesigning the binary with JIT entitlement..."
# Codesign the executable for JIT entitlements.
# Redirect stderr to the errors file.
if ! codesign --entitlements entitlements.plist --sign "-" "${BIN_DIR}/NewBCPL" 2>> "${ERRORS_FILE}"; then
    echo "Error: Codesigning failed. See '${ERRORS_FILE}' for details." >&2
    exit 1
fi

# --- Post-Build Steps ---
echo "Build complete. Executable is at ${BIN_DIR}/NewBCPL"

# Copy the executable to the NewBCPL folder
if ! cp "${BIN_DIR}/NewBCPL" ../NewBCPL/NewBCPL 2>> "${ERRORS_FILE}"; then
    echo "Error: Failed to copy executable. See '${ERRORS_FILE}' for details." >&2
    exit 1
fi
echo "Executable copied to ./NewBCPL/NewBCPL"

# Compile runtime.c as bcpl_runtime.o in the top folder
# (Removed: runtime build is now handled separately)

rm combined.txt
./combiner.sh . combined.txt

echo "----------------------------------------"
echo "Build process finished successfully."
echo "Runtime mode: ${RUNTIME_MODE}"
echo "Runtime library: ${RUNTIME_LIB}"
