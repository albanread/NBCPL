# CMakeLists.txt for the BCPL Runtime
cmake_minimum_required(VERSION 3.10)
project(BCPLRuntime)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the JIT runtime static library target
add_library(bcpl_runtime_jit STATIC
    # Core runtime files
    jit_runtime.cpp
    jit_heap_bridge.cpp
    RuntimeBridge.cpp
    runtime_string_ops.cpp
    heap_interface.cpp
    runtime_c_globals.cpp
    runtime_freelist.c
    BCPLError.c

    # HeapManager implementation files
    ../HeapManager/HeapManager.cpp
    ../HeapManager/heap_manager_globals.cpp
    ../HeapManager/Heap_allocVec.cpp
    ../HeapManager/Heap_allocString.cpp
    ../HeapManager/Heap_allocList.cpp
    ../HeapManager/Heap_allocObject.cpp
    ../HeapManager/Heap_free.cpp
    ../HeapManager/Heap_resizeVec.cpp
    ../HeapManager/Heap_resizeString.cpp
    ../HeapManager/Heap_dumpHeap.cpp
    ../HeapManager/Heap_dumpHeapSignalSafe.cpp
    ../HeapManager/Heap_printMetrics.cpp
)

# Add the JIT_MODE preprocessor definition
target_compile_definitions(bcpl_runtime_jit PRIVATE JIT_MODE)

# Add necessary include directories, relative to this CMakeLists.txt file
target_include_directories(bcpl_runtime_jit PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR} # The 'runtime' directory itself
    ${CMAKE_CURRENT_SOURCE_DIR}/.. # The top-level project directory for HeapManager.h, etc.
    ${CMAKE_CURRENT_SOURCE_DIR}/../HeapManager
)

# Install the JIT library directly to the install prefix (project root)
install(TARGETS bcpl_runtime_jit DESTINATION .)

# =========================
# Standalone C Runtime Library
# =========================
add_library(bcpl_runtime_c STATIC
    runtime.c
    standalone_heap.c
    runtime_string_ops.c
)

target_include_directories(bcpl_runtime_c PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/.. # For any shared headers
    ${CMAKE_CURRENT_SOURCE_DIR}/../HeapManager
)

install(TARGETS bcpl_runtime_c DESTINATION .)
