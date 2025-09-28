/*
 * BCPLError.c
 * Signal-safe error reporting for BCPL runtime.
 * Provides thread-local error info for host API and a global circular buffer for crash diagnostics.
 */

#include "BCPLError.h"

#ifdef __cplusplus
extern "C" {
#endif
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_RECENT_ERRORS 16

// --- Signal-safe global circular buffer for recent errors ---
// These symbols are defined in runtime_c_globals.cpp
extern volatile BCPLErrorInfo g_recent_errors[MAX_RECENT_ERRORS];
extern volatile size_t g_recent_error_index;
static pthread_mutex_t g_error_log_mutex = PTHREAD_MUTEX_INITIALIZER;

// --- Thread-local storage for last error on current thread ---
#if __STDC_VERSION__ >= 201112L
#include <threads.h>
static _Thread_local BCPLErrorInfo tls_last_error = { .code = RUNTIME_OK };
#else
static __thread BCPLErrorInfo tls_last_error = { .code = RUNTIME_OK };
#endif

// Internal function to set an error (runtime use only)
void _BCPL_SET_ERROR(BCPLErrorCode code, const char* func, const char* msg) {
    // Populate thread-local error info
    tls_last_error.code = code;
    strncpy(tls_last_error.function_name, func, sizeof(tls_last_error.function_name) - 1);
    tls_last_error.function_name[sizeof(tls_last_error.function_name) - 1] = '\0';
    strncpy(tls_last_error.message, msg, sizeof(tls_last_error.message) - 1);
    tls_last_error.message[sizeof(tls_last_error.message) - 1] = '\0';

    // Log error to signal-safe circular buffer
    pthread_mutex_lock(&g_error_log_mutex);
    size_t current_index = g_recent_error_index++;
    g_recent_errors[current_index % MAX_RECENT_ERRORS] = tls_last_error;
    pthread_mutex_unlock(&g_error_log_mutex);
}

// Public API: get last error for current thread
void BCPL_GET_LAST_ERROR(BCPLErrorInfo* out_info) {
    if (out_info) {
        *out_info = tls_last_error;
        // Optionally clear after read:
        // tls_last_error.code = RUNTIME_OK;
    }
}

// Optional: clear all error logs (for testing or reset)
void BCPL_CLEAR_ERRORS(void) {
    pthread_mutex_lock(&g_error_log_mutex);
    for (size_t i = 0; i < MAX_RECENT_ERRORS; ++i) {
        g_recent_errors[i].code = RUNTIME_OK;
        g_recent_errors[i].function_name[0] = '\0';
        g_recent_errors[i].message[0] = '\0';
    }
    g_recent_error_index = 0;
    pthread_mutex_unlock(&g_error_log_mutex);

#if __STDC_VERSION__ >= 201112L
    tls_last_error.code = RUNTIME_OK;
    tls_last_error.function_name[0] = '\0';
    tls_last_error.message[0] = '\0';
#else
    tls_last_error.code = RUNTIME_OK;
    tls_last_error.function_name[0] = '\0';
    tls_last_error.message[0] = '\0';
#endif
}

// Bounds checking error handler
void BCPL_BOUNDS_ERROR(uint32_t* var_name_ptr, int64_t index, int64_t length) {
    // DEBUG: Print to stdout first to see if this function is being called
    printf("BCPL_BOUNDS_ERROR called! index=%lld, length=%lld\n", (long long)index, (long long)length);
    fflush(stdout);
    
    // Use signal-safe functions only
    const char* error_msg = "FATAL RUNTIME ERROR: Out of bounds access.\n";
    write(STDERR_FILENO, error_msg, strlen(error_msg));
    
    // Print variable name if available
    if (var_name_ptr != NULL) {
        const char* var_prefix = "Variable: ";
        write(STDERR_FILENO, var_prefix, strlen(var_prefix));
        
        // BCPL strings start with length, then data
        uint32_t str_length = var_name_ptr[0];
        if (str_length > 0 && str_length < 256) { // Sanity check
            for (uint32_t i = 0; i < str_length; i++) {
                char c = (char)(var_name_ptr[i + 1] & 0xFF);
                write(STDERR_FILENO, &c, 1);
            }
        }
        write(STDERR_FILENO, "\n", 1);
    }
    
    // Print index and length using simple integer formatting
    char buffer[64];
    const char* index_msg = "Index: ";
    write(STDERR_FILENO, index_msg, strlen(index_msg));
    snprintf(buffer, sizeof(buffer), "%lld\n", (long long)index);
    write(STDERR_FILENO, buffer, strlen(buffer));
    
    const char* length_msg = "Length: ";
    write(STDERR_FILENO, length_msg, strlen(length_msg));
    snprintf(buffer, sizeof(buffer), "%lld\n", (long long)length);
    write(STDERR_FILENO, buffer, strlen(buffer));
    
    // Record error in the error system
    _BCPL_SET_ERROR(ERROR_INVALID_ARGUMENT, "BOUNDS_CHECK", "Vector/string bounds violation");
    
    // Terminate the program
    abort();
}

#ifdef __cplusplus
}
#endif