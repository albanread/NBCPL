// runtime_timing.inc
// Timing and performance metrics implementation for BCPL runtime
// This file is included by runtime.c (for standalone builds) and runtime_bridge.cpp (for JIT)
// Do not include this file directly in other source files.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef __APPLE__
#include <mach/mach_time.h>
#endif

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
// Timing Data Structures
//=============================================================================

#define MAX_TIMER_NAMES 256
#define MAX_TIMER_DEPTH 64

typedef struct {
    char name[256];
    int64_t total_ns;
    int64_t call_count;
    int active;
} TimerMetric;

typedef struct {
    int metric_index;
    struct timespec start_time;
} TimerStackEntry;

typedef struct {
    TimerStackEntry entries[MAX_TIMER_DEPTH];
    int depth;
} TimerStack;

typedef struct {
    TimerMetric metrics[MAX_TIMER_NAMES];
    TimerStack stacks[MAX_TIMER_NAMES];
    int num_active_timers;
    int metrics_collected;
    int display_on_exit;
} TimingState;

static TimingState g_timing_state = {0};
static int g_timing_initialized = 0;

//=============================================================================
// Internal Helper Functions
//=============================================================================

static void init_timing_system(void) {
    if (g_timing_initialized) return;

    TIMING_LOCK();
    if (!g_timing_initialized) {
        memset(&g_timing_state, 0, sizeof(TimingState));
        g_timing_state.display_on_exit = 1;
        g_timing_initialized = 1;
    }
    TIMING_UNLOCK();
}

static int bcpl_to_c_string_timing(const uint32_t* bcpl_str, char* c_str) {
    if (!bcpl_str || !c_str) return 0;

    int i = 0;
    while (i < 255 && bcpl_str[i] != 0) {
        if (bcpl_str[i] > 127) {
            c_str[i] = '?';
        } else {
            c_str[i] = (char)bcpl_str[i];
        }
        i++;
    }
    c_str[i] = '\0';
    return 1;
}

static int find_or_create_metric_slot(const char* name) {
    for (int i = 0; i < MAX_TIMER_NAMES; i++) {
        if (g_timing_state.metrics[i].active &&
            strcmp(g_timing_state.metrics[i].name, name) == 0) {
            return i;
        }
    }

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

    return -1;
}

static int get_current_time_timing(struct timespec* ts) {
    if (!ts) return -1;

#ifdef __APPLE__
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
    return clock_gettime(CLOCK_MONOTONIC, ts);
#endif
}

static int64_t timespec_diff_ns(const struct timespec* start, const struct timespec* end) {
    if (!start || !end) return 0;

    int64_t sec_diff = end->tv_sec - start->tv_sec;
    int64_t nsec_diff = end->tv_nsec - start->tv_nsec;

    return sec_diff * 1000000000LL + nsec_diff;
}

static char* format_duration_timing(int64_t ns, char* buffer) {
    if (!buffer) return NULL;

    double ns_d = (double)ns;

    if (ns_d >= 1e9) {
        snprintf(buffer, 32, "%.3f s", ns_d / 1e9);
    } else if (ns_d >= 1e6) {
        snprintf(buffer, 32, "%.3f ms", ns_d / 1e6);
    } else if (ns_d >= 1e3) {
        snprintf(buffer, 32, "%.3f µs", ns_d / 1e3);
    } else {
        snprintf(buffer, 32, "%lld ns", (long long)ns);
    }

    return buffer;
}

//=============================================================================
// Public API Implementation
//=============================================================================

#ifdef __cplusplus
extern "C" {
#endif

void TIMER_START(uint32_t* name_str) {
    if (!name_str) return;

    init_timing_system();

    char c_name[256];
    if (!bcpl_to_c_string_timing(name_str, c_name)) {
        return;
    }

    TIMING_LOCK();

    int metric_index = find_or_create_metric_slot(c_name);
    if (metric_index < 0) {
        TIMING_UNLOCK();
        return;
    }

    struct timespec start_time;
    if (get_current_time_timing(&start_time) != 0) {
        TIMING_UNLOCK();
        return;
    }

    TimerStack* stack = &g_timing_state.stacks[metric_index];
    if (stack->depth >= MAX_TIMER_DEPTH) {
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

    struct timespec end_time;
    if (get_current_time_timing(&end_time) != 0) {
        return;
    }

    init_timing_system();

    char c_name[256];
    if (!bcpl_to_c_string_timing(name_str, c_name)) {
        return;
    }

    TIMING_LOCK();

    int metric_index = find_or_create_metric_slot(c_name);
    if (metric_index < 0) {
        TIMING_UNLOCK();
        return;
    }

    TimerStack* stack = &g_timing_state.stacks[metric_index];
    if (stack->depth <= 0) {
        TIMING_UNLOCK();
        return;
    }

    stack->depth--;
    TimerStackEntry* entry = &stack->entries[stack->depth];

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

    int max_name_len = 8;
    for (int i = 0; i < MAX_TIMER_NAMES; i++) {
        if (g_timing_state.metrics[i].active) {
            int name_len = strlen(g_timing_state.metrics[i].name);
            if (name_len > max_name_len) {
                max_name_len = name_len;
            }
        }
    }
    max_name_len += 2;

    printf("%-*s %8s %12s %12s\n",
           max_name_len, "Function", "Calls", "Total", "Average");

    for (int i = 0; i < max_name_len + 8 + 12 + 12 + 3; i++) {
        printf("-");
    }
    printf("\n");

    char total_buf[32], avg_buf[32];
    for (int i = 0; i < MAX_TIMER_NAMES; i++) {
        TimerMetric* metric = &g_timing_state.metrics[i];
        if (!metric->active || metric->call_count == 0) continue;

        int64_t avg_ns = metric->total_ns / metric->call_count;

        format_duration_timing(metric->total_ns, total_buf);
        format_duration_timing(avg_ns, avg_buf);

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
    if (!bcpl_to_c_string_timing(name_str, c_name)) return 0;

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
    if (!bcpl_to_c_string_timing(name_str, c_name)) return 0;

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

#ifdef __cplusplus
}
#endif

// Auto-display function called at program exit
static void timing_display_at_exit(void) {
    if (g_timing_state.metrics_collected && g_timing_state.display_on_exit) {
        TIMER_DISPLAY();
    }
}

// Register the exit handler
static void register_timing_exit_handler(void) {
    static int handler_registered = 0;
    if (!handler_registered) {
        atexit(timing_display_at_exit);
        handler_registered = 1;
    }
}

// Call this during runtime initialization
static void initialize_timing_at_startup(void) {
    init_timing_system();
    register_timing_exit_handler();
}
