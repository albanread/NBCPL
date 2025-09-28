#ifndef BCPL_ERROR_H
#define BCPL_ERROR_H

#include <stdint.h>
#include <stddef.h>

// Machine-readable error codes for BCPL runtime
typedef enum {
    RUNTIME_OK = 0,
    ERROR_OUT_OF_MEMORY,
    ERROR_INVALID_POINTER,     // e.g., freeing an untracked pointer
    ERROR_DOUBLE_FREE,         // e.g., freeing the same pointer twice
    ERROR_INVALID_ARGUMENT,    // e.g., negative size for allocation
    ERROR_FILE_NOT_FOUND,
    ERROR_FILE_IO
} BCPLErrorCode;

// Signal-safe struct to hold error info
// Use fixed-size arrays to avoid pointers to potentially invalid memory
typedef struct {
    BCPLErrorCode code;
    char function_name[64];
    char message[128];
    // Optionally add timestamp or thread ID fields in future
} BCPLErrorInfo;

#define MAX_RECENT_ERRORS 16

// Public API functions
#ifdef __cplusplus
extern "C" {
#endif

void BCPL_GET_LAST_ERROR(BCPLErrorInfo* out_info);
void BCPL_CLEAR_ERRORS(void);
void BCPL_CHECK_AND_DISPLAY_ERRORS(void);

// Internal function for setting errors (used by runtime)
void _BCPL_SET_ERROR(BCPLErrorCode code, const char* func, const char* msg);

// Bounds checking error handler
void BCPL_BOUNDS_ERROR(uint32_t* var_name_ptr, int64_t index, int64_t length);

// Global error tracking variables
extern volatile BCPLErrorInfo g_recent_errors[MAX_RECENT_ERRORS];
extern volatile size_t g_recent_error_index;

#ifdef __cplusplus
}
#endif

#endif // BCPL_ERROR_H