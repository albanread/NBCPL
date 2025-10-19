#ifndef RUNTIME_TIMING_H
#define RUNTIME_TIMING_H

#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Internal Timing Data Structures
//=============================================================================

// Maximum number of different timer names we can track
#define MAX_TIMER_NAMES 256

// Maximum depth for nested timers (recursion support)
#define MAX_TIMER_DEPTH 64

// Timing metric for a single named timer
typedef struct {
    char name[256];           // Timer name (converted from BCPL string)
    int64_t total_ns;        // Total accumulated time in nanoseconds
    int64_t call_count;      // Number of times this timer was called
    int active;              // 1 if this slot is in use, 0 if free
} TimerMetric;

// Stack entry for handling nested/recursive timers
typedef struct {
    int metric_index;        // Index into metrics array
    struct timespec start_time; // Start time for this timer instance
} TimerStackEntry;

// Timer name stack for handling recursion
typedef struct {
    TimerStackEntry entries[MAX_TIMER_DEPTH];
    int depth;              // Current stack depth
} TimerStack;

// Global timing state
typedef struct {
    TimerMetric metrics[MAX_TIMER_NAMES];  // Array of all timer metrics
    TimerStack stacks[MAX_TIMER_NAMES];    // Stack for each timer name (for recursion)
    int num_active_timers;                 // Number of active timer slots
    int metrics_collected;                 // 1 if any metrics have been collected
    int display_on_exit;                   // 1 to auto-display at program exit
} TimingState;

//=============================================================================
// Internal Functions
//=============================================================================

/**
 * Initialize the timing system.
 * Called automatically on first use.
 */
void init_timing_system(void);

/**
 * Convert a BCPL string to a C string for internal use.
 * 
 * @param bcpl_str Pointer to BCPL string (UTF-32 array)
 * @param c_str    Output buffer for C string (must be at least 256 bytes)
 * @return         1 on success, 0 on failure
 */
int bcpl_to_c_string(const uint32_t* bcpl_str, char* c_str);

/**
 * Find or create a metric slot for the given timer name.
 * 
 * @param name Timer name as C string
 * @return     Index into metrics array, or -1 if no space available
 */
int find_or_create_metric_slot(const char* name);

/**
 * Get current time with high precision.
 * Uses clock_gettime with CLOCK_MONOTONIC for best precision.
 * 
 * @param ts Output timespec structure
 * @return   0 on success, -1 on error
 */
int get_current_time(struct timespec* ts);

/**
 * Calculate difference between two timespec structures in nanoseconds.
 * 
 * @param start Start time
 * @param end   End time  
 * @return      Difference in nanoseconds
 */
int64_t timespec_diff_ns(const struct timespec* start, const struct timespec* end);

/**
 * Format a nanosecond duration into human-readable string.
 * 
 * @param ns     Duration in nanoseconds
 * @param buffer Output buffer (must be at least 32 bytes)
 * @return       Pointer to formatted string in buffer
 */
char* format_duration(int64_t ns, char* buffer);

/**
 * Cleanup function called at program exit.
 * Automatically displays metrics if any were collected.
 */
void timing_cleanup_at_exit(void);

#ifdef __cplusplus
}
#endif

#endif // RUNTIME_TIMING_H