#include "../HeapManager/HeapManager.h"   // For C++ HeapManager and tracking
#include "runtime.h"                      // Include the primary runtime header instead
#include "runtime_core.inc"               // Shared BCPL runtime logic
#include "runtime_string_utils.inc"       // Shared string helpers
#include "runtime_io.inc"                 // Shared I/O helpers

// No main() here: this file is for building the JIT runtime library (libbcpl_runtime_jit.a)
