// runtime_core.inc
// Core implementation of BCPL runtime functions
// This file is included by runtime.c (for standalone builds) and runtime_bridge.cpp (for JIT)
// Do not include this file directly in other source files.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <time.h>
#include <math.h>

// Static variable to track random number generator initialization
static int rand_initialized = 0;

// UTF-8 output helper for WRITEC
static void write_utf8_char(int64_t ch) {
    if (ch < 0 || ch > 0x10FFFF) {
        // Invalid Unicode codepoint
        putchar('?');
        return;
    }

    if (ch < 0x80) {
        // 1-byte sequence (ASCII)
        putchar((char)ch);
    } else if (ch < 0x800) {
        // 2-byte sequence
        putchar(0xC0 | ((ch >> 6) & 0x1F));
        putchar(0x80 | (ch & 0x3F));
    } else if (ch < 0x10000) {
        // 3-byte sequence
        putchar(0xE0 | ((ch >> 12) & 0x0F));
        putchar(0x80 | ((ch >> 6) & 0x3F));
        putchar(0x80 | (ch & 0x3F));
    } else {
        // 4-byte sequence
        putchar(0xF0 | ((ch >> 18) & 0x07));
        putchar(0x80 | ((ch >> 12) & 0x3F));
        putchar(0x80 | ((ch >> 6) & 0x3F));
        putchar(0x80 | (ch & 0x3F));
    }
}

// Helper function to process escape sequences
// Processes current character and advances pointer if needed
static void write_char_with_escapes(uint32_t** p) {
    uint32_t c = **p;

    if (c == '\\' && *((*p) + 1) != 0) {
        (*p)++; // Skip the backslash
        uint32_t next_char = **p;

        switch (next_char) {
            case 'n':
                putchar('\n');
                break;
            case 't':
                putchar('\t');
                break;
            case 'r':
                putchar('\r');
                break;
            case 'b':
                putchar('\b');
                break;
            case 'f':
                putchar('\f');
                break;
            case 'v':
                putchar('\v');
                break;
            case 'a':
                putchar('\a');
                break;
            case '\\':
                putchar('\\');
                break;
            case '"':
                putchar('"');
                break;
            case '\'':
                putchar('\'');
                break;
            case '0':
                putchar('\0');
                break;
            default:
                // If not a recognized escape, print the backslash and the character
                putchar('\\');
                write_utf8_char(next_char);
                break;
        }
    } else {
        write_utf8_char(c);
    }
}

#include  <ctype.h>
static void get_printable_char(uint32_t code, char* buffer, size_t buffer_size) {
    if (code > 255) { // Not a standard ASCII character
        snprintf(buffer, buffer_size, ".");
        return;
    }

    char c = (char)code;
    switch (c) {
        case '\n': snprintf(buffer, buffer_size, "\\n"); break;
        case '\r': snprintf(buffer, buffer_size, "\\r"); break;
        case '\t': snprintf(buffer, buffer_size, "\\t"); break;
        case '\0': snprintf(buffer, buffer_size, "\\0"); break;
        default:
            if (isprint(c)) {
                snprintf(buffer, buffer_size, "%c", c);
            } else {
                snprintf(buffer, buffer_size, ".");
            }
            break;
    }
}




/**
 * @brief Prints a BCPL-style string.
 * @param s A pointer to the string data. The first word (length) is skipped, and
 * 32-bit characters are read and printed until a null terminator (0) is found.
 */
void WRITES(uint32_t* s) {
    // 1. Handle a null pointer.
    if (!s) {
        fputs("(null)", stdout);
        fflush(stdout);
        return;
    }

    // 2. Start reading from the second word (s[1]), skipping the length at s[0].
    uint32_t* p = s;

    // 3. Loop until a null terminator is found.
    while (*p != 0) {
        // Process escape sequences
        write_char_with_escapes(&p);
        p++; // Move to the next character
    }

    fflush(stdout);
}





void FWRITE(double f) {
    printf("%g", f);
    fflush(stdout);
}

// Runtime validation for dynamic format strings
static void validate_runtime_writef(uint32_t* format_str, int64_t* args, int argc) {
    if (!format_str) return;

    uint32_t* p = format_str;
    int format_specs = 0;

    // Count format specifiers in the string
    while (*p != 0) {
        if (*p == '%' && *(p + 1) != 0) {
            p++; // Skip the '%'
            uint32_t spec = *p;

            // NOTE: Keep this list in sync with the analyzer and code generator!
            // Count actual format specifiers (not %%)
            if (spec != '%' && (spec == 'd' || spec == 'i' || spec == 'N' || spec == 'x' || spec == 'X' ||
                               spec == 'o' || spec == 'f' || spec == 'F' || spec == 'c' || spec == 's' ||
                               spec == 'P' || spec == 'Q' || spec == 'S')) {
                format_specs++;
            }
        }
        p++;
    }

    // Validate argument count matches format specifiers
    if (format_specs != argc) {
        printf("RUNTIME ERROR: WRITEF format string expects %d arguments but %d provided\n",
               format_specs, argc);
        fflush(stdout);
        // Don't call exit() in runtime - just return and let normal error handling proceed
        return;
    }
}

// Single implementation function that handles all format parsing
static void WRITEF_impl(uint32_t* format_str, int64_t* args, int argc) {
    if (!format_str) {
        printf("(null format)");
        fflush(stdout);
        return;
    }

    // Validate runtime format string and argument count
    validate_runtime_writef(format_str, args, argc);

    uint32_t* p = format_str;
    int args_used = 0;

    while (*p != 0) {
        if (*p == '%' && *(p + 1) != 0) {
            p++; // Skip the '%'



            if (args_used < argc) {

                switch (*p) {
                    case 'd': case 'i': case 'N':
                        printf("%lld", (long long)args[args_used]);
                        break;
                    case 'x':
                        printf("%llx", (unsigned long long)args[args_used]);
                        break;
                    case 'X':
                        printf("%llX", (unsigned long long)args[args_used]);
                        break;
                    case 'o':
                        printf("%llo", (unsigned long long)args[args_used]);
                        break;
                    case 'f': case 'F':
                        // Note: For WRITEF, float arguments are passed in X registers (not D registers)
                        // as per our ABI for variadic functions. We cast the int64_t back to double.
                        printf("%f", *(double*)&args[args_used]);
                        break;
                    case 'P': { // PAIR: two 32-bit signed ints packed in int64_t
                        int32_t x = (int32_t)(args[args_used] & 0xFFFFFFFF);
                        int32_t y = (int32_t)((args[args_used] >> 32) & 0xFFFFFFFF);
                        printf("(%d, %d)", x, y);
                        break;
                    }
                    case 'Q': { // FPAIR: two 32-bit floats packed in int64_t
                        union { int64_t i; float f[2]; } u;
                        u.i = args[args_used];
                        printf("(%g, %g)", u.f[0], u.f[1]);
                        break;
                    }
                    case 'c':
                        write_utf8_char(args[args_used]);
                        break;
                    case 's':
                        WRITES((uint32_t*)args[args_used]);
                        break;
                    case '%':
                        putchar('%');
                        args_used--; // Don't consume an argument for %%
                        break;
                    default:
                        putchar('%');
                        write_utf8_char(*p);
                        args_used--; // Don't consume an argument for unknown format
                        break;
                }
                args_used++;
            } else {
                // No more arguments available, print format specifier literally
                putchar('%');
                write_utf8_char(*p);
            }
        } else {
            write_char_with_escapes(&p);
        }
        p++;
    }
    fflush(stdout);
}

// Thin wrapper functions that package arguments and call the implementation

void WRITEF(uint32_t* format_str) {
    WRITEF_impl(format_str, NULL, 0);
}

void WRITEF1(uint32_t* format_str, int64_t arg1) {
    int64_t args[] = {arg1};
    WRITEF_impl(format_str, args, 1);
}

void WRITEF2(uint32_t* format_str, int64_t arg1, int64_t arg2) {
    int64_t args[] = {arg1, arg2};
    WRITEF_impl(format_str, args, 2);
}

void WRITEF3(uint32_t* format_str, int64_t arg1, int64_t arg2, int64_t arg3) {
    int64_t args[] = {arg1, arg2, arg3};
    WRITEF_impl(format_str, args, 3);
}

void WRITEF4(uint32_t* format_str, int64_t arg1, int64_t arg2, int64_t arg3, int64_t arg4) {
    int64_t args[] = {arg1, arg2, arg3, arg4};
    WRITEF_impl(format_str, args, 4);
}

void WRITEF5(uint32_t* format_str, int64_t arg1, int64_t arg2, int64_t arg3, int64_t arg4, int64_t arg5) {
    int64_t args[] = {arg1, arg2, arg3, arg4, arg5};
    WRITEF_impl(format_str, args, 5);
}

void WRITEF6(uint32_t* format_str, int64_t arg1, int64_t arg2, int64_t arg3, int64_t arg4, int64_t arg5, int64_t arg6) {
    int64_t args[] = {arg1, arg2, arg3, arg4, arg5, arg6};
    WRITEF_impl(format_str, args, 6);
}

void WRITEF7(uint32_t* format_str, int64_t arg1, int64_t arg2, int64_t arg3, int64_t arg4, int64_t arg5, int64_t arg6, int64_t arg7) {
    int64_t args[] = {arg1, arg2, arg3, arg4, arg5, arg6, arg7};
    WRITEF_impl(format_str, args, 7);
}

void WRITEN(int64_t n) {
    printf("%lld", (long long)n);
    fflush(stdout);
}

void WRITEC(int64_t ch) {
    write_utf8_char(ch);
    fflush(stdout);
}

int64_t RDCH(void) {
    // This is a simplified implementation that only handles ASCII.
    // A full implementation would need to decode UTF-8 sequences.
    int c = getchar();
    if (c == EOF) return -1;
    return (int64_t)c;
}

void finish(void) {
    exit(0);
}

int64_t STRLEN(const uint32_t* s) {
    if (!s) return 0;

    int64_t len = 0;
    while (s[len] != 0) {
        len++;
    }

    return len;
}

int64_t STRCMP(const uint32_t* s1, const uint32_t* s2) {
    if (!s1 && !s2) return 0;
    if (!s1) return -1;
    if (!s2) return 1;

    int i = 0;
    while (s1[i] == s2[i]) {
        if (s1[i] == 0) return 0; // Equal strings
        i++;
    }

    return (int64_t)s1[i] - (int64_t)s2[i];
}

uint32_t* STRCOPY(uint32_t* dst, const uint32_t* src) {
    if (!dst) return NULL;
    if (!src) {
        dst[0] = 0; // Empty string if source is NULL
        return dst;
    }

    int i = 0;
    while (src[i] != 0) {
        dst[i] = src[i];
        i++;
    }
    dst[i] = 0; // Add null terminator

    return dst;
}

// Random number functions
int64_t RAND(int64_t max_val) {
    // Initialize the random number generator on first use
    if (!rand_initialized) {
        srand((unsigned int)time(NULL));
        rand_initialized = 1;
    }

    // Handle edge cases
    if (max_val <= 0) {
        return 0;
    }

    // Generate a random number in the range [0, max_val]
    // Using modulo can introduce bias, but for most use cases this is acceptable
    return (int64_t)rand() % (max_val + 1);
}

double FRND(void) {
    // Initialize the random number generator on first use
    if (!rand_initialized) {
        srand((unsigned int)time(NULL));
        rand_initialized = 1;
    }

    // Generate a random double between 0.0 and 1.0
    return (double)rand() / (double)RAND_MAX;
}

double RND(int64_t max_val) {
    // Initialize the random number generator on first use
    if (!rand_initialized) {
        srand((unsigned int)time(NULL));
        rand_initialized = 1;
    }

    // Handle edge cases
    if (max_val <= 0) {
        return 0.0;
    }

    // Generate a random double between 0.0 and max_val
    return ((double)rand() / (double)RAND_MAX) * (double)max_val;
}

// Float math functions
double FSIN(double x) {
    return sin(x);
}

double FCOS(double x) {
    return cos(x);
}

double FTAN(double x) {
    return tan(x);
}

double FABS(double x) {
    return fabs(x);
}

double FLOG(double x) {
    return log(x);
}

double FEXP(double x) {
    return exp(x);
}

// Type conversion functions
int64_t FIX(double x) {
    return (int64_t)x;
}
