#ifndef TIMER_MACROS_H
#define TIMER_MACROS_H

#include "runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Convenience Macros for BCPL Timing
//=============================================================================

// Helper to convert C string literals to BCPL format for timing
// This is mainly for internal use and testing
#define TIMER_START_C(name) do { \
    static uint32_t timer_name[] = {name, 0}; \
    TIMER_START(timer_name); \
} while(0)

#define TIMER_END_C(name) do { \
    static uint32_t timer_name[] = {name, 0}; \
    TIMER_END(timer_name); \
} while(0)

//=============================================================================
// RAII-Style Timer for C++ (Optional Enhancement)
//=============================================================================

#ifdef __cplusplus
/**
 * C++ RAII-style scoped timer for automatic cleanup.
 * This provides exception safety and automatic timer cleanup.
 * 
 * Usage:
 *   {
 *     ScopedBCPLTimer timer("my_function");
 *     // Your code here
 *     // Timer automatically ends when scope exits
 *   }
 */
class ScopedBCPLTimer {
private:
    uint32_t* timer_name;
    bool timer_started;

public:
    /**
     * Constructor: starts the timer with the given BCPL string name
     */
    explicit ScopedBCPLTimer(uint32_t* name) : timer_name(name), timer_started(false) {
        if (name) {
            TIMER_START(name);
            timer_started = true;
        }
    }

    /**
     * Constructor: starts the timer with a C string name (converted to BCPL format)
     */
    explicit ScopedBCPLTimer(const char* name) : timer_started(false) {
        if (name) {
            // Create a static BCPL string from C string
            static thread_local uint32_t bcpl_name[256];
            int i = 0;
            while (i < 255 && name[i] != '\0') {
                bcpl_name[i] = (uint32_t)name[i];
                i++;
            }
            bcpl_name[i] = 0;
            timer_name = bcpl_name;
            TIMER_START(timer_name);
            timer_started = true;
        }
    }

    /**
     * Destructor: automatically ends the timer
     */
    ~ScopedBCPLTimer() {
        if (timer_started && timer_name) {
            TIMER_END(timer_name);
        }
    }

    // Delete copy constructor and assignment operator to prevent copying
    ScopedBCPLTimer(const ScopedBCPLTimer&) = delete;
    ScopedBCPLTimer& operator=(const ScopedBCPLTimer&) = delete;
};

/**
 * Macro for easier scoped timing with C++ RAII
 * Usage: SCOPED_TIMER("function_name");
 */
#define SCOPED_TIMER(name) ScopedBCPLTimer _timer(name)

#endif // __cplusplus

//=============================================================================
// Debug Timing Macros (Can be disabled for release builds)
//=============================================================================

#ifdef ENABLE_DEBUG_TIMING
#define DEBUG_TIMER_START(name) TIMER_START(name)
#define DEBUG_TIMER_END(name) TIMER_END(name)
#define DEBUG_TIMER_DISPLAY() TIMER_DISPLAY()
#else
#define DEBUG_TIMER_START(name)
#define DEBUG_TIMER_END(name)
#define DEBUG_TIMER_DISPLAY()
#endif

//=============================================================================
// Utility Functions for BCPL String Conversion
//=============================================================================

/**
 * Create a BCPL string from a C string literal at compile time.
 * This is a helper for testing and internal use.
 * 
 * Note: The resulting string is stored in static memory and should
 * not be modified.
 */
static inline uint32_t* create_bcpl_string_literal(const char* c_str) {
    static uint32_t buffer[256];
    int i = 0;
    while (i < 255 && c_str[i] != '\0') {
        buffer[i] = (uint32_t)c_str[i];
        i++;
    }
    buffer[i] = 0;
    return buffer;
}

/**
 * Macro to create BCPL string literals for timer names
 * Usage: BCPL_STR("my_timer_name")
 */
#define BCPL_STR(str) create_bcpl_string_literal(str)

#ifdef __cplusplus
}
#endif

#endif // TIMER_MACROS_H