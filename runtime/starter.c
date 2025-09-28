#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

// Global trace flags required by runtime libraries
bool g_enable_heap_trace = false;
bool g_enable_lexer_trace = false;
bool g_enable_symbols_trace = false;

// Declare the external entry point for the compiled BCPL code.
// This function will be located in the .s file generated from your BCPL source.
extern void START(void);

// Declare runtime initialization functions
extern void initialize_runtime(void);
extern void register_runtime_functions(void);

/**
 * @brief The main entry point for the C runtime environment.
 *
 * This function is called by the OS when the program starts. It initializes
 * the BCPL runtime system and then calls the main entry point of the 
 * compiled BCPL program ("START").
 */
int main(void) {
    // Initialize the BCPL runtime system
    initialize_runtime();
    register_runtime_functions();

    // Enable SAMM before calling the main BCPL entry point.
    extern void HeapManager_setSAMMEnabled(int enabled);
    HeapManager_setSAMMEnabled(1);
    
    // Call the main entry point of the BCPL program.
    START();

    // Although the BCPL program will likely call EXIT,
    // we perform an orderly shutdown as a fallback.
    // TODO: cleanup_runtime is not implemented. Commented out to avoid linker errors.
    // extern void cleanup_runtime(void);
    // cleanup_runtime();
    exit(0);
}