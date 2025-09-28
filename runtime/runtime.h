#ifndef RUNTIME_H
#define RUNTIME_H

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

// SPLIT and JOIN string/list functions
struct ListHeader;
struct ListAtom;
/**
 * BCPL strings are represented as pointers to arrays of 32-bit Unicode code points (UTF-32).
 * The payload pointer points to the character data, not the length prefix.
 */
uint32_t* BCPL_JOIN_LIST(struct ListHeader* list_header, uint32_t* delimiter_payload);

// Concatenate two lists destructively (see heap_interface.cpp)
#ifdef __cplusplus
extern "C" {
#endif
struct ListHeader* BCPL_CONCAT_LISTS(struct ListHeader* list1_header, struct ListHeader* list2_header);
#ifdef __cplusplus
}
#endif
/**
 * BCPL strings are represented as pointers to arrays of 32-bit Unicode code points (UTF-32).
 * The payload pointer points to the character data, not the length prefix.
 */
struct ListHeader* BCPL_SPLIT_STRING(uint32_t* source_payload, uint32_t* delimiter_payload);

//=============================================================================
// Runtime version information
//=============================================================================
#define BCPL_RUNTIME_VERSION "1.0.0"

//=============================================================================
// Heap Management (C Bridge to C++ HeapManager)
//=============================================================================

/**
 * Allocates a vector of 64-bit words.
 *
 * @param num_words Number of words to allocate
 * @param func      Name of the function making the allocation (for debugging)
 * @param var       Name of the variable receiving the allocation (for debugging)
 * @return          Pointer to the allocated memory, or NULL on failure
 */
void* bcpl_alloc_words(int64_t num_words, const char* func, const char* var);

/**
 * Allocates a character string buffer.
 *
 * @param num_chars Number of characters (excluding null terminator)
 * @return          Pointer to the allocated string, or NULL on failure
 */
void* bcpl_alloc_chars(int64_t num_chars);

/**
 * Frees memory allocated by bcpl_alloc_words or bcpl_alloc_chars.
 *
 * @param ptr       Pointer to the memory to free
 */
void bcpl_free(void* ptr);

//=============================================================================
// Core I/O and System
//=============================================================================

/**
 * Writes a BCPL string to standard output.
 *
 * @param s Pointer to a null-terminated BCPL string
 */
void WRITES(uint32_t* s);

/**
 * Writes a floating-point number to standard output.
 *
 * @param f The floating-point value to write
 */
void FWRITE(double f);

/**
 * Writes a formatted string to standard output.
 *
 * @param format_str The format string
 */
void WRITEF(uint32_t* format_str);

/**
 * Writes a formatted string with 1 argument to standard output.
 *
 * @param format_str The format string
 * @param arg1 First argument
 */
void WRITEF1(uint32_t* format_str, int64_t arg1);

/**
 * Writes a formatted string with 2 arguments to standard output.
 *
 * @param format_str The format string
 * @param arg1 First argument
 * @param arg2 Second argument
 */
void WRITEF2(uint32_t* format_str, int64_t arg1, int64_t arg2);

/**
 * Writes a formatted string with 3 arguments to standard output.
 *
 * @param format_str The format string
 * @param arg1 First argument
 * @param arg2 Second argument
 * @param arg3 Third argument
 */
void WRITEF3(uint32_t* format_str, int64_t arg1, int64_t arg2, int64_t arg3);

/**
 * Writes a formatted string with 4 arguments to standard output.
 *
 * @param format_str The format string
 * @param arg1 First argument
 * @param arg2 Second argument
 * @param arg3 Third argument
 * @param arg4 Fourth argument
 */
void WRITEF4(uint32_t* format_str, int64_t arg1, int64_t arg2, int64_t arg3, int64_t arg4);

/**
 * Writes a formatted string with 5 arguments to standard output.
 *
 * @param format_str The format string
 * @param arg1 First argument
 * @param arg2 Second argument
 * @param arg3 Third argument
 * @param arg4 Fourth argument
 * @param arg5 Fifth argument
 */
void WRITEF5(uint32_t* format_str, int64_t arg1, int64_t arg2, int64_t arg3, int64_t arg4, int64_t arg5);

/**
 * Writes a formatted string with 6 arguments to standard output.
 *
 * @param format_str The format string
 * @param arg1 First argument
 * @param arg2 Second argument
 * @param arg3 Third argument
 * @param arg4 Fourth argument
 * @param arg5 Fifth argument
 * @param arg6 Sixth argument
 */
void WRITEF6(uint32_t* format_str, int64_t arg1, int64_t arg2, int64_t arg3, int64_t arg4, int64_t arg5, int64_t arg6);

/**
 * Writes a formatted string with 7 arguments to standard output.
 *
 * @param format_str The format string
 * @param arg1 First argument
 * @param arg2 Second argument
 * @param arg3 Third argument
 * @param arg4 Fourth argument
 * @param arg5 Fifth argument
 * @param arg6 Sixth argument
 * @param arg7 Seventh argument
 */
void WRITEF7(uint32_t* format_str, int64_t arg1, int64_t arg2, int64_t arg3, int64_t arg4, int64_t arg5, int64_t arg6, int64_t arg7);

/**
 * Writes an integer to standard output.
 *
 * @param n The integer to write
 */
void WRITEN(int64_t n);

/**
 * Writes a single character to standard output.
 *
 * @param ch The character code to write
 */
void WRITEC(int64_t ch);

/**
 * Reads a single character from standard input.
 *
 * @return The character read, or -1 on EOF
 */
int64_t RDCH(void);

/**
 * Terminates program execution.
 */
void finish(void);

/**
 * Prints a newline character to standard output and flushes.
 */
void NEWLINE(void);

//=============================================================================
// String Utilities
//=============================================================================

/**
 * Packs a BCPL string into a UTF-8 byte vector.
 *
 * @param bcpl_string Pointer to a null-terminated BCPL string
 * @return            Pointer to the packed UTF-8 byte vector, or NULL on failure
 * @note BCPL strings are represented as pointers to arrays of 32-bit Unicode code points (UTF-32).
 */
void* PACKSTRING(uint32_t* bcpl_string);

/**
 * Unpacks a UTF-8 byte vector into a BCPL string.
 *
 * @param byte_vector Pointer to a UTF-8 byte vector
 * @return            Pointer to the unpacked BCPL string, or NULL on failure
 * @note BCPL strings are represented as pointers to arrays of 32-bit Unicode code points (UTF-32).
 */
uint32_t* UNPACKSTRING(const uint8_t* byte_vector);

/**
 * Returns the length of a BCPL string.
 *
 * @param s Pointer to a BCPL string
 * @return  The number of characters in the string
 * @note BCPL strings are represented as pointers to arrays of 32-bit Unicode code points (UTF-32).
 */
int64_t STRLEN(const uint32_t* s);

/**
 * Compares two BCPL strings.
 *
 * @param s1 Pointer to the first BCPL string
 * @param s2 Pointer to the second BCPL string
 * @return   <0 if s1 < s2, 0 if s1 == s2, >0 if s1 > s2
 * @note BCPL strings are represented as pointers to arrays of 32-bit Unicode code points (UTF-32).
 */
int64_t STRCMP(const uint32_t* s1, const uint32_t* s2);

/**
 * Copies a BCPL string.
 *
 * @param dst Pointer to the destination BCPL string
 * @param src Pointer to the source BCPL string
 * @return    Pointer to the destination string
 * @note BCPL strings are represented as pointers to arrays of 32-bit Unicode code points (UTF-32).
 */
uint32_t* STRCOPY(uint32_t* dst, const uint32_t* src);

//=============================================================================
// File I/O
//=============================================================================

/**
 * Reads an entire file into a BCPL string.
 *
 * @param filename_str Pointer to a BCPL string containing the filename
 * @return             Pointer to a BCPL string containing the file contents, or NULL on failure
 */
uint32_t* SLURP(uint32_t* filename_str);

/**
 * Writes a BCPL string to a file.
 *
 * @param bcpl_string  Pointer to a BCPL string to write
 * @param filename_str Pointer to a BCPL string containing the filename
 */
void SPIT(uint32_t* bcpl_string, uint32_t* filename_str);

/**
 * Prints runtime memory allocation metrics.
 * Shows counts of allocations, frees, and memory usage.
 */
void print_runtime_metrics(void);

//=============================================================================
// Random Number Generation
//=============================================================================

/**
 * Generates a random integer between 0 and max_val (inclusive).
 *
 * @param max_val Maximum value for the random number
 * @return        Random integer between 0 and max_val
 */
int64_t RAND(int64_t max_val);

/**
 * Generates a random floating-point number between 0.0 and 1.0.
 *
 * @return Random double between 0.0 and 1.0
 */
double FRND(void);

/**
 * Generates a random floating-point number between 0.0 and max_val.
 *
 * @param max_val Maximum value for the random number
 * @return        Random double between 0.0 and max_val
 */
double RND(int64_t max_val);

/**
 * Computes the sine of x (x in radians).
 *
 * @param x Angle in radians
 * @return Sine of x
 */
double FSIN(double x);

/**
 * Computes the cosine of x (x in radians).
 *
 * @param x Angle in radians
 * @return Cosine of x
 */
double FCOS(double x);

/**
 * Computes the tangent of x (x in radians).
 *
 * @param x Angle in radians
 * @return Tangent of x
 */
double FTAN(double x);

/**
 * Computes the absolute value of x.
 *
 * @param x Input value
 * @return Absolute value of x
 */
double FABS(double x);

/**
 * Computes the natural logarithm of x.
 *
 * @param x Input value (must be positive)
 * @return Natural logarithm of x
 */
double FLOG(double x);

/**
 * Computes e raised to the power of x.
 *
 * @param x Exponent
 * @return e^x
 */
double FEXP(double x);

/**
 * Converts a floating-point number to an integer.
 *
 * @param x Floating-point value to convert
 * @return Integer part of x (truncated towards zero)
 */
int64_t FIX(double x);

#ifdef __cplusplus
}
#endif

#endif // RUNTIME_H