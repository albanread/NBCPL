// runtime_timing.c
// High-precision timing and performance metrics for BCPL runtime
// Cross-platform implementation using POSIX clock_gettime

#include "runtime_timing.h"
#include "runtime.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef __APPLE__
#include <mach/mach_time.h>
#endif

//=============================================================================
// Global State
//=============================================================================

static TimingState g_timing_state = {0};
static int g_timing_initialized = 0;

// Thread safety mutex (if needed)
#ifdef _POSIX_THREADS
#include <pthread.h>
static pthread_mutex_t g_timing_mutex = PTHREAD_MUTEX_INITIALIZER;
#define TIMING_LOCK() pthread_mutex_lock(&g_timing_mutex)
#define TIMING_UNLOCK() pthread_mutex_unlock(&g_timing_mutex)
#else
#define TIMING_LOCK() 
#define TIMING_UNLOCK()
#endif

//=============================================================================
// Internal Implementation
//=============================================================================

void init_timing_system(void) {
    if (g_timing_initialized) return;
    
    TIMING_LOCK();
    if (!g_timing_initialized) {
        memset(&g_timing_state, 0, sizeof(TimingState));
        g_timing_state.display_on_exit = 1; // Auto-display by default
        
        // Register cleanup function to run at program exit
        atexit(timing_cleanup_at_exit);
        
        g_timing_initialized = 1;
    }
    TIMING_UNLOCK();
}

int bcpl_to_c_string(const uint32_t* bcpl_str, char* c_str) {
    if (!bcpl_str || !c_str) return 0;
    
    int i = 0;
    while (i < 255 && bcpl_str[i] != 0) {
        // Convert UTF-32 to ASCII (for timer names, we assume ASCII)
        if (bcpl_str[i] > 127) {
            c_str[i] = '?'; // Replace non-ASCII with placeholder
        } else {
            c_str[i] = (char)bcpl_str[i];
        }
        i++;
    }
    c_str[i] = '\0';
    return 1;
}

int find_or_create_metric_slot(const char* name) {
    // First, look for existing metric with this name
    for (int i = 0; i < MAX_TIMER_NAMES; i++) {
        if (g_timing_state.metrics[i].active && 
            strcmp(g_timing_state.metrics[i].name, name) == 0) {
            return i;
        }
    }
    
    // Not found, create new slot
    for (int i = 0; i < MAX_TIMER_NAMES; i++) {
        if (!g_timing_state.metrics[i].active) {
            strncpy(g_timing_state.metrics[i].name, name, 255);
            g_timing_state.metrics[i].name[255] = '\0';
            g_timing_state.metrics[i].total_ns = 0;
            g_timing_state.metrics[i].call_count = 0;
            g_timing_state.metrics[i].active = 1;
            g_timing_state.num_active_timers++;
            return i;
        }
    }
    
    // No space available
    return -1;
}

int get_current_time(struct timespec* ts) {
    if (!ts) return -1;
    
#ifdef __APPLE__
    // macOS doesn't have clock_gettime in older versions, use mach_absolute_time
    static mach_timebase_info_data_t timebase_info = {0};
    if (timebase_info.denom == 0) {
        mach_timebase_info(&timebase_info);
    }
    
    uint64_t mach_time = mach_absolute_time();
    uint64_t nanos = mach_time * timebase_info.numer / timebase_info.denom;
    
    ts->tv_sec = nanos / 1000000000ULL;
    ts->tv_nsec = nanos % 1000000000ULL;
    return 0;
#else
    // Linux and other POSIX systems
    return clock_gettime(CLOCK_MONOTONIC, ts);
#endif
}

int64_t timespec_diff_ns(const struct timespec* start, const struct timespec* end) {
    if (!start || !end) return 0;
    
    int64_t sec_diff = end->tv_sec - start->tv_sec;
    int64_t nsec_diff = end->tv_nsec - start->tv_nsec;
    
    return sec_diff * 1000000000LL + nsec_diff;
}

char* format_duration(int64_t ns, char* buffer) {
    if (!buffer) return NULL;
    
    double ns_d = (double)ns;
    
    if (ns_d >= 1e9) {
        // Seconds
        snprintf(buffer, 32, "%.3f s", ns_d / 1e9);
    } else if (ns_d >= 1e6) {
        // Milliseconds  
        snprintf(buffer, 32, "%.3f ms", ns_d / 1e6);
    } else if (ns_d >= 1e3) {
        // Microseconds
        snprintf(buffer, 32, "%.3f µs", ns_d / 1e3);
    } else {
        // Nanoseconds
        snprintf(buffer, 32, "%lld ns", (long long)ns);
    }
    
    return buffer;
}

void timing_cleanup_at_exit(void) {
    if (g_timing_state.metrics_collected && g_timing_state.display_on_exit) {
        TIMER_DISPLAY();
    }
}

//=============================================================================
// Public API Implementation
//=============================================================================

void TIMER_START(uint32_t* name_str) {
    if (!name_str) return;
    
    init_timing_system();
    
    char c_name[256];
    if (!bcpl_to_c_string(name_str, c_name)) {
        fprintf(stderr, "TIMER_START: Failed to convert timer name\n");
        return;
    }
    
    TIMING_LOCK();
    
    int metric_index = find_or_create_metric_slot(c_name);
    if (metric_index < 0) {
        fprintf(stderr, "TIMER_START: No space for timer '%s'\n", c_name);
        TIMING_UNLOCK();
        return;
    }
    
    // Get current time
    struct timespec start_time;
    if (get_current_time(&start_time) != 0) {
        fprintf(stderr, "TIMER_START: Failed to get current time\n");
        TIMING_UNLOCK();
        return;
    }
    
    // Push onto timer stack for this metric
    TimerStack* stack = &g_timing_state.stacks[metric_index];
    if (stack->depth >= MAX_TIMER_DEPTH) {
        fprintf(stderr, "TIMER_START: Timer stack overflow for '%s'\n", c_name);
        TIMING_UNLOCK();
        return;
    }
    
    TimerStackEntry* entry = &stack->entries[stack->depth];
    entry->metric_index = metric_index;
    entry->start_time = start_time;
    stack->depth++;
    
    g_timing_state.metrics_collected = 1;
    
    TIMING_UNLOCK();
}

void TIMER_END(uint32_t* name_str) {
    if (!name_str) return;
    
    // Get end time immediately for accuracy
    struct timespec end_time;
    if (get_current_time(&end_time) != 0) {
        fprintf(stderr, "TIMER_END: Failed to get current time\n");
        return;
    }
    
    init_timing_system();
    
    char c_name[256];
    if (!bcpl_to_c_string(name_str, c_name)) {
        fprintf(stderr, "TIMER_END: Failed to convert timer name\n");
        return;
    }
    
    TIMING_LOCK();
    
    int metric_index = find_or_create_metric_slot(c_name);
    if (metric_index < 0) {
        fprintf(stderr, "TIMER_END: Timer '%s' not found\n", c_name);
        TIMING_UNLOCK();
        return;
    }
    
    // Pop from timer stack for this metric
    TimerStack* stack = &g_timing_state.stacks[metric_index];
    if (stack->depth <= 0) {
        fprintf(stderr, "TIMER_END: No matching TIMER_START for '%s'\n", c_name);
        TIMING_UNLOCK();
        return;
    }
    
    stack->depth--;
    TimerStackEntry* entry = &stack->entries[stack->depth];
    
    // Calculate duration and update metrics
    int64_t duration_ns = timespec_diff_ns(&entry->start_time, &end_time);
    
    TimerMetric* metric = &g_timing_state.metrics[metric_index];
    metric->total_ns += duration_ns;
    metric->call_count++;
    
    TIMING_UNLOCK();
}

void TIMER_DISPLAY(void) {
    init_timing_system();
    
    TIMING_LOCK();
    
    printf("--- Performance Metrics ---\n");
    
    if (g_timing_state.num_active_timers == 0) {
        printf("  (No timing metrics collected)\n");
        TIMING_UNLOCK();
        return;
    }
    
    // Find column widths for nice formatting
    int max_name_len = 8; // Minimum width for "Function" header
    for (int i = 0; i < MAX_TIMER_NAMES; i++) {
        if (g_timing_state.metrics[i].active) {
            int name_len = strlen(g_timing_state.metrics[i].name);
            if (name_len > max_name_len) {
                max_name_len = name_len;
            }
        }
    }
    max_name_len += 2; // Add padding
    
    // Print header
    printf("%-*s %8s %12s %12s\n", 
           max_name_len, "Function", "Calls", "Total", "Average");
    
    for (int i = 0; i < max_name_len + 8 + 12 + 12 + 3; i++) {
        printf("-");
    }
    printf("\n");
    
    // Print metrics data
    char total_buf[32], avg_buf[32];
    for (int i = 0; i < MAX_TIMER_NAMES; i++) {
        TimerMetric* metric = &g_timing_state.metrics[i];
        if (!metric->active || metric->call_count == 0) continue;
        
        int64_t avg_ns = metric->total_ns / metric->call_count;
        
        format_duration(metric->total_ns, total_buf);
        format_duration(avg_ns, avg_buf);
        
        printf("%-*s %8lld %12s %12s\n", 
               max_name_len, metric->name, 
               (long long)metric->call_count,
               total_buf, avg_buf);
    }
    
    TIMING_UNLOCK();
}

void TIMER_CLEAR(void) {
    init_timing_system();
    
    TIMING_LOCK();
    
    // Reset all metrics
    for (int i = 0; i < MAX_TIMER_NAMES; i++) {
        g_timing_state.metrics[i].active = 0;
        g_timing_state.metrics[i].total_ns = 0;
        g_timing_state.metrics[i].call_count = 0;
        g_timing_state.stacks[i].depth = 0;
    }
    
    g_timing_state.num_active_timers = 0;
    g_timing_state.metrics_collected = 0;
    
    TIMING_UNLOCK();
}

int64_t TIMER_GET_TOTAL_NS(uint32_t* name_str) {
    if (!name_str) return 0;
    
    init_timing_system();
    
    char c_name[256];
    if (!bcpl_to_c_string(name_str, c_name)) return 0;
    
    TIMING_LOCK();
    
    for (int i = 0; i < MAX_TIMER_NAMES; i++) {
        if (g_timing_state.metrics[i].active && 
            strcmp(g_timing_state.metrics[i].name, c_name) == 0) {
            int64_t result = g_timing_state.metrics[i].total_ns;
            TIMING_UNLOCK();
            return result;
        }
    }
    
    TIMING_UNLOCK();
    return 0;
}

int64_t TIMER_GET_CALL_COUNT(uint32_t* name_str) {
    if (!name_str) return 0;
    
    init_timing_system();
    
    char c_name[256];
    if (!bcpl_to_c_string(name_str, c_name)) return 0;
    
    TIMING_LOCK();
    
    for (int i = 0; i < MAX_TIMER_NAMES; i++) {
        if (g_timing_state.metrics[i].active && 
            strcmp(g_timing_state.metrics[i].name, c_name) == 0) {
            int64_t result = g_timing_state.metrics[i].call_count;
            TIMING_UNLOCK();
            return result;
        }
    }
    
    TIMING_UNLOCK();
    return 0;
}