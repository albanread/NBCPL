#include "SignalHandler.h"
#include "SignalSafeUtils.h"
#include "HeapManager/HeapManager.h"
#include "JITExecutor.h"
#include <csignal>
#include <execinfo.h>
#include <cstdlib>
#include <unistd.h>
#include <cstdio>


// External globals used in handler
extern const char* g_source_code;
extern JITExecutor* g_jit_executor;

// Error log globals for crash diagnostics
extern "C" {
    #include "runtime/BCPLError.h"
    extern volatile BCPLErrorInfo g_recent_errors[16];
    extern volatile size_t g_recent_error_index;
}

void SignalHandler::fatal_signal_handler(int signum, siginfo_t* info, void* context) {
    const char* signame;
    switch (signum) {
        case SIGSEGV: signame = "SIGSEGV"; break;
        case SIGBUS:  signame = "SIGBUS";  break;
        case SIGILL:  signame = "SIGILL";  break;
        case SIGFPE:  signame = "SIGFPE";  break;
        case SIGABRT: signame = "SIGABRT"; break;
        case SIGTRAP: signame = "SIGTRAP"; break;
        default:      signame = "UNKNOWN"; break;
    }
    safe_print("Fatal Signal (");
    safe_print(signame);
    safe_print(") caught.\n");

#if defined(__APPLE__) && defined(__aarch64__)
    // ucontext_t* uc = reinterpret_cast<ucontext_t*>(context); // Removed duplicate definition
    // __darwin_arm_thread_state64* state = &uc->uc_mcontext->__ss; // Removed duplicate definition
    ucontext_t* uc = reinterpret_cast<ucontext_t*>(context);
    __darwin_arm_thread_state64* state = &uc->uc_mcontext->__ss;
    __darwin_arm_neon_state64* neon_state = &uc->uc_mcontext->__ns;

    char num_buf[20], hex_buf[20];
    uint32_t cpsr = state->__cpsr;
    bool n = (cpsr >> 31) & 1, z = (cpsr >> 30) & 1, c = (cpsr >> 29) & 1, v = (cpsr >> 28) & 1;

    safe_print("\n--- Processor Flags (CPSR) ---\n");
    u64_to_hex(cpsr, hex_buf);
    safe_print("Value: ");
    safe_print(hex_buf);
    safe_print("\nFlags: N=");
    safe_print(n ? "1" : "0");
    safe_print(" Z=");
    safe_print(z ? "1" : "0");
    safe_print(" C=");
    safe_print(c ? "1" : "0");
    safe_print(" V=");
    safe_print(v ? "1" : "0");
    safe_print("\n|----------------------------|\n");

    safe_print("Register dump (Apple ARM64):\n");

    for (int i = 0; i < 28; i++) {
        int_to_dec(i, num_buf);
        u64_to_hex(state->__x[i], hex_buf);
        safe_print("| X");
        safe_print(num_buf);
        safe_print("   | ");
        safe_print(hex_buf);
        safe_print(" |\n");
    }
    safe_print("|----------------------------|\n");
    u64_to_hex(state->__x[28], hex_buf); safe_print("| DP    | "); safe_print(hex_buf); safe_print(" |\n");
    u64_to_hex(state->__fp, hex_buf); safe_print("| FP    | "); safe_print(hex_buf); safe_print(" |\n");
    u64_to_hex(state->__lr, hex_buf); safe_print("| LR    | "); safe_print(hex_buf); safe_print(" |\n");
    u64_to_hex(state->__sp, hex_buf); safe_print("| SP    | "); safe_print(hex_buf); safe_print(" |\n");
    u64_to_hex(state->__pc, hex_buf); safe_print("| PC    | "); safe_print(hex_buf); safe_print(" |\n");
    safe_print("|----------------------------|\n");

    // Access NEON/SIMD state for floating-point registers (Apple ARM64)
    safe_print("Floating-point register dump (Apple ARM64):\n");
    safe_print("| Reg   | Hex Value (64-bit)     |\n");
    safe_print("|-------+------------------------|\n");

    // Loop for D0 to D12
    for (int i = 0; i <= 12; i++) {
        uint64_t d_val = (uint64_t)neon_state->__v[i]; // Get lower 64 bits of V register
        int_to_dec(i, num_buf);
        u64_to_hex(d_val, hex_buf);
        safe_print("| D");
        safe_print(num_buf);
        safe_print("   | ");
        safe_print(hex_buf);
        safe_print(" |\n");
    }
    safe_print("|----------------------------|\n");

    if (g_jit_executor) {
        g_jit_executor->dump_jit_stack_from_signal(state->__sp);
    }

    HeapManager::getInstance().dumpHeapSignalSafe();
#endif

    if (g_source_code && *g_source_code != '\0') {
        safe_print("\n--- Source Code ---\n");
        safe_print(g_source_code);
        safe_print("\n-------------------\n\n");
    }

#if defined(__APPLE__) && defined(__aarch64__)
    // Reuse existing variable 'uc' defined earlier in the function
    // Reuse existing variable 'state' defined earlier in the function

    // Reuse existing variables 'line_buf', 'num_buf', and 'hex_buf' defined earlier in the function
    // Reuse existing variable 'cpsr' defined earlier in the function
    // Reuse existing variables 'n', 'z', 'c', and 'v' defined earlier in the function

    safe_print("\n--- Processor Flags (CPSR) ---\n");
    u64_to_hex(cpsr, hex_buf);
    safe_print("Value: ");
    safe_print(hex_buf);
    safe_print("\nFlags: N=");
    safe_print(n ? "1" : "0");
    safe_print(" Z=");
    safe_print(z ? "1" : "0");
    safe_print(" C=");
    safe_print(c ? "1" : "0");
    safe_print(" V=");
    safe_print(v ? "1" : "0");
    safe_print("\n|----------------------------|\n");

    safe_print("Register dump (Apple ARM64):\n");

    for (int i = 0; i < 28; i++) {
        int_to_dec(i, num_buf);
        u64_to_hex(state->__x[i], hex_buf);
        safe_print("| X");
        safe_print(num_buf);
        safe_print("   | ");
        safe_print(hex_buf);
        safe_print(" |\n");
    }
    safe_print("|----------------------------|\n");
    u64_to_hex(state->__x[28], hex_buf); safe_print("| DP    | "); safe_print(hex_buf); safe_print(" |\n");
    u64_to_hex(state->__fp, hex_buf); safe_print("| FP    | "); safe_print(hex_buf); safe_print(" |\n");
    u64_to_hex(state->__lr, hex_buf); safe_print("| LR    | "); safe_print(hex_buf); safe_print(" |\n");
    u64_to_hex(state->__sp, hex_buf); safe_print("| SP    | "); safe_print(hex_buf); safe_print(" |\n");
    u64_to_hex(state->__pc, hex_buf); safe_print("| PC    | "); safe_print(hex_buf); safe_print(" |\n");
    safe_print("|----------------------------|\n");

    // Attempt to access NEON/SIMD state for floating-point registers (Apple ARM64)
    safe_print("Floating-point register dump (Apple ARM64):\n");
    // On macOS, the NEON/SIMD state is not available in __darwin_arm_thread_state64.
    // Print a fallback message.
    safe_print("  [Floating-point register state not available in this context.]\n");
    safe_print("|----------------------------|\n");

    if (g_jit_executor) {
        g_jit_executor->dump_jit_stack_from_signal(state->__sp);
    }

    HeapManager::getInstance().dumpHeapSignalSafe();



#endif

    // Print recent error log (cross-platform)
    safe_print("\n--- Recent Runtime Errors ---\n");
    char index_buf[20];
    size_t last_error_idx = g_recent_error_index;
    size_t start_idx = (last_error_idx > 16) ? (last_error_idx - 16) : 0;

    // Debug: Print error buffer status
    u64_to_hex(last_error_idx, index_buf);
    safe_print("DEBUG: error_index=");
    safe_print(index_buf);
    safe_print(" start_idx=");
    u64_to_hex(start_idx, index_buf);
    safe_print(index_buf);
    safe_print("\n");

    for (size_t i = start_idx; i < last_error_idx; ++i) {
        const volatile BCPLErrorInfo& err = g_recent_errors[i % 16];
        
        // Debug: Print each slot status
        u64_to_hex(i % 16, index_buf);
        safe_print("DEBUG: slot ");
        safe_print(index_buf);
        safe_print(" code=");
        u64_to_hex(err.code, index_buf);
        safe_print(index_buf);
        safe_print("\n");
        
        if (err.code == RUNTIME_OK) continue;

        int_to_dec(err.code, index_buf);
        safe_print("Error ");
        safe_print(index_buf);
        safe_print(": ");
        safe_print((const char*)err.message);
        safe_print(" in function '");
        safe_print((const char*)err.function_name);
        safe_print("'\n");
    }
    safe_print("---------------------------\n");

    safe_print("Stack trace:\n");
    void* callstack[128];
    int frames = backtrace(callstack, 128);
    char** strs = backtrace_symbols(callstack, frames);
    if (strs) {
        for (int i = 0; i < frames; ++i) {
            safe_print("  "); safe_print(strs[i]); safe_print("\n");
        }
        free(strs);
    }
    _exit(1);
}

void SignalHandler::setup() {
    struct sigaction sa;
    sa.sa_sigaction = SignalHandler::fatal_signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGSEGV, &sa, nullptr);
    sigaction(SIGBUS, &sa, nullptr);
    sigaction(SIGILL, &sa, nullptr);
    sigaction(SIGFPE, &sa, nullptr);
    sigaction(SIGABRT, &sa, nullptr);
    sigaction(SIGTRAP, &sa, nullptr);
}
