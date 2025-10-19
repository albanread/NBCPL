# BCPL Runtime Timing API Design

## Overview

This document describes the design and implementation of the timing and performance metrics system for the NewBCPL compiler runtime. The system provides high-precision timing capabilities that integrate seamlessly with BCPL programs while maintaining the simplicity and elegance of the BCPL language.

## Design Goals

1. **BCPL-Native API**: Functions follow BCPL naming conventions (uppercase) and integrate naturally with existing runtime functions
2. **Automatic Display**: Timing metrics are automatically displayed when programs finish, requiring no explicit cleanup code
3. **High Precision**: Nanosecond-level timing accuracy using platform-appropriate high-resolution timers
4. **Recursion Support**: Handle nested and recursive function calls correctly using internal timer stacks
5. **Thread Safety**: Safe to use in multi-threaded environments with proper mutex protection
6. **Cross-Platform**: Works on both macOS and Linux with optimized timing implementations
7. **Zero Overhead When Unused**: No performance impact when timing functions are not called

## API Reference

### Core Timing Functions

#### `TIMER_START(name_str)`
Starts a timer with the given name.
- **Parameters**: `name_str` - Pointer to a BCPL string containing the timer name
- **Behavior**: Multiple timers with the same name are supported (handles recursion)
- **Example**:
```bcpl
TIMER_START("my_function")
```

#### `TIMER_END(name_str)`
Ends the most recently started timer with the given name.
- **Parameters**: `name_str` - Pointer to a BCPL string containing the timer name  
- **Behavior**: Accumulates elapsed time in metrics, handles nested calls via LIFO stack
- **Example**:
```bcpl
TIMER_END("my_function")
```

### Display and Control Functions

#### `TIMER_DISPLAY()`
Displays all collected timing metrics to standard output.
- **Behavior**: Called automatically at program exit if any metrics were collected
- **Output Format**: Tabular display showing Function, Calls, Total time, Average time

#### `TIMER_CLEAR()`
Clears all timing metrics.
- **Use Case**: Useful for resetting measurements between test runs or program phases

### Query Functions

#### `TIMER_GET_TOTAL_NS(name_str)`
Returns the total accumulated time in nanoseconds for a named timer.
- **Return**: `int64_t` - Total time in nanoseconds, or 0 if timer doesn't exist

#### `TIMER_GET_CALL_COUNT(name_str)`
Returns the number of times a named timer has been called.
- **Return**: `int64_t` - Call count, or 0 if timer doesn't exist

## Usage Patterns

### Basic Timing
```bcpl
LET START() BE
$(
    TIMER_START("main_function")
    
    // Your program logic here
    WRITEF("Doing some work*N")
    
    TIMER_END("main_function")
    // Metrics displayed automatically at program exit
$)
```

### Nested Timing
```bcpl
LET my_algorithm() BE
$(
    TIMER_START("algorithm")
    
    TIMER_START("initialization")
    // Initialization code
    TIMER_END("initialization")
    
    TIMER_START("processing")  
    // Main processing
    TIMER_END("processing")
    
    TIMER_END("algorithm")
$)
```

### Recursive Function Timing
```bcpl
LET fibonacci(n) = VALOF
$(
    TIMER_START("fibonacci")
    
    LET result = ?
    IF n <= 1 THEN
        result := n
    ELSE
        result := fibonacci(n-1) + fibonacci(n-2)
        
    TIMER_END("fibonacci")
    RESULTIS result
$)
```

### Repeated Operations
```bcpl
FOR i = 1 TO 100 DO
$(
    TIMER_START("loop_iteration")
    // Work for each iteration
    TIMER_END("loop_iteration")
$)
// Shows accumulated time for all 100 iterations
```

## Implementation Details

### Data Structures

The timing system uses several key data structures:

- **TimerMetric**: Stores name, total time, call count for each unique timer
- **TimerStack**: LIFO stack for handling nested/recursive calls per timer name
- **TimingState**: Global state containing all metrics and stacks

### Thread Safety

All timing functions are protected by mutexes when compiled with pthread support:
- `TIMING_LOCK()` / `TIMING_UNLOCK()` macros provide cross-platform locking
- Atomic operations ensure consistent state updates

### High-Precision Timing

Platform-specific implementations provide optimal timing:

**macOS**: Uses `mach_absolute_time()` with `mach_timebase_info` for conversion
```c
uint64_t mach_time = mach_absolute_time();
uint64_t nanos = mach_time * timebase_info.numer / timebase_info.denom;
```

**Linux**: Uses `clock_gettime(CLOCK_MONOTONIC, &ts)` for monotonic time
```c
clock_gettime(CLOCK_MONOTONIC, &ts);
```

### Memory Management

- Fixed-size arrays avoid dynamic allocation overhead
- Maximum 256 different timer names supported
- Maximum 64 levels of nesting per timer name
- Configurable limits via `MAX_TIMER_NAMES` and `MAX_TIMER_DEPTH`

### String Handling

BCPL strings (UTF-32 arrays) are converted to C strings for internal storage:
- Supports ASCII timer names (non-ASCII converted to '?')
- Maximum 255 characters per timer name
- Null-terminated C string storage for efficient comparisons

## Output Format

The timing display produces formatted output like:
```
--- Performance Metrics ---
Function               Calls        Total      Average
----------------------------------------------------- 
algorithm                 1     145.234 ms   145.234 ms
fibonacci                21      98.765 ms     4.703 ms
initialization            1       2.456 ms     2.456 ms
processing                1     142.123 ms   142.123 ms
```

Time units are automatically selected for readability:
- **Seconds** (s): >= 1,000,000,000 ns
- **Milliseconds** (ms): >= 1,000,000 ns  
- **Microseconds** (µs): >= 1,000 ns
- **Nanoseconds** (ns): < 1,000 ns

## Integration Points

### Runtime Initialization
- Timing system initializes lazily on first use
- Exit handler registered via `atexit()` for automatic cleanup
- Integrated with existing `finish()` function

### Compiler Integration
- Timer functions added to `runtime.h` interface
- Available to all BCPL programs automatically
- No special compiler flags or includes required

### Build System
- `runtime_timing.inc` included in both standalone and JIT builds
- Cross-platform compilation handled automatically
- Thread safety optional based on pthread availability

## Performance Considerations

### Overhead Analysis
- Timer start/end: ~100-500 nanoseconds per call pair
- String conversion: One-time cost per unique timer name
- Mutex locking: Minimal overhead with modern pthread implementations
- Memory usage: ~32KB for maximum configuration (256 timers × 64 depth)

### Best Practices
1. Use descriptive but concise timer names
2. Avoid timing very short operations (< 1 microsecond)
3. Balance timing granularity with measurement overhead
4. Use `TIMER_CLEAR()` between distinct measurement phases
5. Consider disabling timing for production builds if needed

### Optimization Opportunities
- Static timer name registration could eliminate string conversion overhead
- Lock-free implementations possible for single-threaded scenarios  
- Timer name interning could reduce memory usage
- Compile-time timer name validation could catch typos

## Extension Points

### Debug Features
The system supports conditional compilation for debug-only timing:
```c
#ifdef ENABLE_DEBUG_TIMING
#define DEBUG_TIMER_START(name) TIMER_START(name)
#define DEBUG_TIMER_END(name) TIMER_END(name)
#else
#define DEBUG_TIMER_START(name)
#define DEBUG_TIMER_END(name)  
#endif
```

### C++ Integration
For C++ code interfacing with the runtime:
```cpp
class ScopedBCPLTimer {
public:
    explicit ScopedBCPLTimer(const char* name);
    ~ScopedBCPLTimer();
};

#define SCOPED_TIMER(name) ScopedBCPLTimer _timer(name)
```

### Future Enhancements
- Statistical analysis (min/max/standard deviation)
- Sampling-based profiling for lower overhead
- Integration with external profiling tools
- Web-based metrics visualization
- Call stack capture for detailed analysis
- Memory usage tracking alongside timing

## Comparison with Other Systems

### Advantages over Manual Timing
- Automatic aggregation and formatting
- Built-in recursion/nesting support  
- Thread-safe by default
- Integrated cleanup and display

### Advantages over External Profilers
- Zero setup required - works out of the box
- BCPL-native API design
- Selective instrumentation at source level
- No external dependencies

### Trade-offs vs. Hardware Profilers
- Higher overhead than hardware performance counters
- Software-only timing (no cache miss analysis)
- Portable across architectures
- Easier to understand and debug

## Conclusion

The BCPL timing API provides a powerful yet simple way to measure performance in BCPL programs. It follows the language's philosophy of simplicity while providing enterprise-grade features like thread safety and high precision. The automatic display feature makes it particularly suitable for development, testing, and performance analysis workflows.

The design balances ease of use with performance, providing nanosecond precision while maintaining the clean, readable code style that BCPL programmers expect. Integration with the existing runtime ensures that timing capabilities are available immediately without additional setup or configuration.