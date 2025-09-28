# NewBCPL Memory Management Guide

## Table of Contents
1. [Overview](#overview)
2. [Memory Allocation Types](#memory-allocation-types)
3. [Automatic Memory Management](#automatic-memory-management)
4. [Manual Memory Management](#manual-memory-management)
5. [The RETAIN Keyword](#the-retain-keyword)
6. [Best Practices](#best-practices)
7. [Debugging Memory Issues](#debugging-memory-issues)
8. [Advanced Topics](#advanced-topics)
9. [Examples](#examples)

## Overview

NewBCPL provides automatic memory management with scope-based cleanup, similar to RAII (Resource Acquisition Is Initialization) in C++. The compiler automatically tracks heap allocations and inserts cleanup code when objects go out of scope, preventing memory leaks while maintaining the simplicity of BCPL.

### Key Features
- **Automatic cleanup**: Objects are automatically freed when they go out of scope
- **Ownership tracking**: The compiler tracks which variables own heap memory
- **Manual override**: Use `RETAIN` to disable automatic cleanup when needed
- **Memory safety**: Built-in protection against double-free and null pointer issues
- **Debugging support**: Comprehensive heap tracing and metrics

### Implementation Status
✅ **Working Features:**
- Basic object allocation with `NEW`
- Automatic cleanup on scope exit
- Heap tracing and metrics
- Double-free protection

⚠️ **Limited/Experimental Features:**
- `RETAIN` keyword (syntax supported, semantics under development)
- Complex method calls (some edge cases may fail)
- Vector management with `GETVEC/FREEVEC` (basic functionality works)

🔧 **In Development:**
- Advanced object inheritance patterns
- Cross-scope object sharing
- Performance optimizations

## Memory Allocation Types

### 1. Object Allocation with NEW

Objects are allocated on the heap using the `NEW` keyword:

```bcpl
CLASS MyClass $(
    LET value = 0
    
    ROUTINE CREATE(initial_value) BE $(
        value := initial_value
    $)
    
    ROUTINE RELEASE() BE $(
        // Custom cleanup code can go here
        // Default RELEASE calls OBJECT_HEAP_FREE automatically
    $)
$)

LET START() BE $(
    LET obj = NEW MyClass(42)  // Allocates object on heap
    // obj.RELEASE() is automatically called when obj goes out of scope
$)
```

### 2. Vector Allocation with GETVEC

Dynamic arrays are allocated using `GETVEC`:

```bcpl
LET START() BE $(
    LET size = 100
    LET vector = GETVEC(size)  // Allocates array of 'size' elements
    
    // Use the vector...
    vector!0 := 42
    vector!1 := 123
    
    // Must manually free vectors
    FREEVEC(vector)
$)
```

### 3. String Allocation

Dynamic strings are allocated automatically:

```bcpl
LET START() BE $(
    LET str = "Hello, World!"  // Stack-allocated string literal
    LET dynamic_str = NEWSTRING(100)  // Heap-allocated string
    
    // dynamic_str is automatically cleaned up
$)
```

### 4. Static Allocation with TABLE

Read-only data can be allocated statically:

```bcpl
LET START() BE $(
    LET static_array = TABLE(1, 2, 3, 4, 5)  // No heap allocation
    // No cleanup needed - stored in program data section
$)
```

## Automatic Memory Management

### How It Works

The NewBCPL compiler uses a sophisticated analysis system to automatically manage memory:

1. **Ownership Tracking**: When you write `LET obj = NEW MyClass()`, the compiler marks `obj` with `owns_heap_memory = true`
2. **Scope Analysis**: The compiler analyzes variable scopes and determines when objects should be cleaned up
3. **Automatic Insertion**: At scope exit points, the compiler automatically inserts `obj.RELEASE()` calls
4. **Liveness Analysis**: The compiler ensures cleanup code is properly scheduled to avoid use-after-free bugs

### Example: Automatic Cleanup

```bcpl
LET START() BE $(
    $(  // Inner scope begins
        LET obj = NEW MyClass()
        LET another = NEW MyClass()
        
        // Use objects...
        obj.doSomething()
        another.doSomething()
        
    $)  // Inner scope ends
        // Compiler automatically inserts:
        // another.RELEASE()
        // obj.RELEASE()
        // (cleanup happens in reverse order of declaration)
$)
```

### What Gets Automatic Cleanup

✅ **Automatically cleaned up:**
- Objects created with `NEW`
- Dynamic strings created with `NEWSTRING`
- Lists created with `LIST()`
- Any heap allocation tracked by the compiler

❌ **NOT automatically cleaned up:**
- Vectors created with `GETVEC` (must use `FREEVEC`)
- Static data created with `TABLE`
- Objects marked with `RETAIN`

## Manual Memory Management

### When Manual Management is Needed

1. **Vectors**: Always require manual `FREEVEC`
2. **Retained objects**: Objects you want to keep alive beyond their scope
3. **Shared ownership**: Objects used by multiple parts of your program
4. **Performance critical code**: Where you need precise control over cleanup timing

### Vector Management

```bcpl
LET START() BE $(
    LET size = 1000
    LET data = GETVEC(size)
    
    // Initialize and use the vector
    FOR i = 0 TO size-1 DO
        data!i := i * i
    
    // Print some values
    FOR i = 0 TO 9 DO $(
        WRITEN(data!i)
        WRITEC(' ')
    $)
    WRITEC('\n')
    
    // MUST manually free
    FREEVEC(data)
$)
```

### Manual Object Release

```bcpl
LET START() BE $(
    LET obj = NEW MyClass()
    
    // Use object...
    obj.doWork()
    
    // Manually release early (optional)
    obj.RELEASE()
    
    // obj is now invalid - don't use it!
$)
```

## The RETAIN Keyword

The `RETAIN` keyword disables automatic cleanup for specific variables, giving you manual control over their lifetime.

### Basic RETAIN Usage

```bcpl
LET create_object() = VALOF $(
    LET obj = NEW MyClass()
    RETAIN obj  // Disable automatic cleanup
    RESULTIS obj  // Safe to return - won't be freed
$)

LET START() BE $(
    LET my_obj = create_object()
    
    // Use the object
    my_obj.doSomething()
    
    // Must manually release retained objects
    my_obj.RELEASE()
$)
```

### RETAIN with Declaration

```bcpl
LET START() BE $(
    // Declare and immediately retain
    RETAIN LET persistent_obj = NEW MyClass()
    
    // Object persists beyond this scope
    // Must manually call persistent_obj.RELEASE() when done
$)
```

### When to Use RETAIN

1. **Factory Functions**: When creating objects to return to callers
2. **Global State**: Objects that need to persist for the program's lifetime
3. **Shared Objects**: Objects used by multiple functions or modules
4. **Performance**: Avoiding automatic cleanup overhead in tight loops

## Best Practices

### 1. Prefer Automatic Management

Let the compiler handle memory management whenever possible:

```bcpl
// GOOD: Let compiler handle cleanup
LET process_data() BE $(
    LET processor = NEW DataProcessor()
    processor.loadData("input.txt")
    processor.process()
    processor.saveResults("output.txt")
    // processor.RELEASE() called automatically
$)

// AVOID: Manual management unless necessary
LET process_data_manual() BE $(
    LET processor = NEW DataProcessor()
    RETAIN processor
    processor.loadData("input.txt")
    processor.process()
    processor.saveResults("output.txt")
    processor.RELEASE()  // Easy to forget!
$)
```

### 2. Use Narrow Scopes

Keep object lifetimes as short as possible:

```bcpl
LET START() BE $(
    // Process each file in its own scope
    FOREACH filename IN file_list DO $(
        LET processor = NEW FileProcessor()
        processor.process(filename)
        // processor cleaned up at end of each iteration
    $)
$)
```

### 3. Document RETAIN Usage

Always document why you're using RETAIN:

```bcpl
LET create_cache() = VALOF $(
    LET cache = NEW CacheManager()
    RETAIN cache  // Needed: cache persists beyond function scope
    cache.initialize()
    RESULTIS cache
$)
```

### 4. Pair RETAIN with RELEASE

Every RETAIN should have a corresponding RELEASE:

```bcpl
LET g_cache = ?  // Global cache object

LET initialize_system() BE $(
    g_cache := create_cache()  // Returns retained object
$)

LET shutdown_system() BE $(
    g_cache.RELEASE()  // Clean up retained object
    g_cache := ?
$)
```

### 5. Check for Memory Leaks

Use heap tracing to verify your memory management:

```bash
./NewBCPL myprogram.bcl --run --trace-heap
```

## Debugging Memory Issues

### Heap Tracing

Enable detailed heap tracing to see all allocations and deallocations:

```bash
# Basic heap tracing
./NewBCPL program.bcl --run --trace-heap

# More detailed analysis
./NewBCPL program.bcl --run --trace-heap --trace-runtime
```

Example output:
```
OBJECT_HEAP_ALLOC: Object allocated at 0x600000918000
OBJECT_HEAP_FREE: Freeing object at 0x600000918000

--- BCPL Runtime Metrics ---
Memory allocations: 5 (120 bytes)
Memory frees: 5 (120 bytes)
Vector allocations: 2
String allocations: 1
Current active allocations: 0 (0 bytes)  ← Should be 0!
```

### Common Memory Issues

#### 1. Memory Leaks
**Symptom**: "Current active allocations" > 0 at program end

**Causes**:
- Forgot to RELEASE a retained object
- Circular references between objects
- Exception thrown before cleanup

**Solution**:
```bcpl
// Use narrow scopes and avoid RETAIN when possible
LET START() BE $(
    $(
        LET obj = NEW MyClass()  // No RETAIN needed
        // obj automatically cleaned up
    $)
    // Check metrics here - should show 0 active allocations
$)
```

#### 2. Double-Free Errors
**Symptom**: Program crashes or warnings about freeing already-freed memory

**Causes**:
- Calling RELEASE twice on the same object
- Object freed automatically but also manually released

**Solution**:
```bcpl
LET START() BE $(
    LET obj = NEW MyClass()
    obj.RELEASE()  // Manual release
    obj := ?       // Clear the pointer
    // Don't use obj after this point
$)
```

#### 3. Use-After-Free
**Symptom**: Crashes or corrupted data when accessing objects

**Causes**:
- Using object after calling RELEASE
- Using object after it was automatically cleaned up

**Solution**:
```bcpl
LET START() BE $(
    LET obj = ?
    $(
        obj := NEW MyClass()
        // Use obj here
    $)
    // obj is now invalid - automatic cleanup happened
    // obj.doSomething()  // ← BUG: use-after-free
$)
```

### Debugging Techniques

1. **Add explicit WRITES statements** around object creation/destruction
2. **Use narrow scopes** to isolate memory issues
3. **Check metrics** after each major operation
4. **Use valgrind or similar tools** for additional validation

## Advanced Topics

### Compiler Implementation Details

The automatic memory management system involves several compiler passes:

1. **ASTAnalyzer**: Marks variables with `owns_heap_memory` flag
2. **LivenessAnalysis**: Determines variable lifetimes and cleanup points  
3. **CFGBuilder**: Inserts cleanup code at scope exit points
4. **RetainAnalysis**: Processes RETAIN statements and disables automatic cleanup

### Custom RELEASE Methods

You can override the default RELEASE behavior:

```bcpl
CLASS ResourceManager $(
    LET file_handle = ?
    LET buffer = ?
    
    ROUTINE CREATE(filename) BE $(
        file_handle := open_file(filename)
        buffer := GETVEC(1024)
    $)
    
    ROUTINE RELEASE() BE $(
        // Custom cleanup logic
        IF file_handle ~= ? THEN close_file(file_handle)
        IF buffer ~= ? THEN FREEVEC(buffer)
        
        // Call parent RELEASE (if any) or default heap free
        SUPER.RELEASE()  // Or let default OBJECT_HEAP_FREE handle it
    $)
$)
```

### Performance Considerations

- Automatic cleanup has minimal overhead (just a few instructions per scope exit)
- RETAIN/RELEASE has no runtime cost beyond explicit calls
- Vector allocation/deallocation is faster than object allocation
- Static TABLE allocation has zero runtime cost

## Examples

### Simple Working Example

Here's a basic example that demonstrates the core automatic memory management:

```bcpl
// cleanup_example.bcl - Basic automatic cleanup demonstration
CLASS MyClass $(
    // No members needed for this example
$)

LET START() BE $(
    // This inner block creates a limited scope
    $(
        // Object allocated on heap, tracked by compiler
        LET p = NEW MyClass()
        
        // Use the object here...
        // (object methods, assignments, etc.)
        
    $) // Scope ends here - p.RELEASE() automatically called
    
    // Object is now cleaned up
$)
```

To test this example:
```bash
./NewBCPL cleanup_example.bcl --run --trace-heap
```

Expected output shows automatic allocation and cleanup:
```
OBJECT_HEAP_ALLOC: Object allocated at 0x...
OBJECT_HEAP_FREE: Freeing object at 0x...
Memory allocations: 1 (24 bytes)
Memory frees: 1 (24 bytes) 
Current active allocations: 0 (0 bytes)  ← Perfect!
```

### Complete Object Lifecycle Example

```bcpl
CLASS DataProcessor $(
    LET buffer = ?
    LET size = 0
    
    ROUTINE CREATE(buffer_size) BE $(
        size := buffer_size
        buffer := GETVEC(size)
        WRITES("DataProcessor created with buffer size: ")
        WRITEN(size)
        WRITEC('\n')
    $)
    
    ROUTINE process(data) BE $(
        // Process data using buffer
        FOR i = 0 TO size-1 DO
            buffer!i := data + i
    $)
    
    ROUTINE RELEASE() BE $(
        WRITES("DataProcessor cleanup: freeing buffer\n")
        IF buffer ~= ? THEN $(
            FREEVEC(buffer)
            buffer := ?
        $)
        // Default object cleanup happens automatically
    $)
$)

LET START() BE $(
    WRITES("=== DataProcessor Test ===\n")
    
    $(  // Scope for automatic cleanup
        LET processor = NEW DataProcessor(1000)
        processor.process(42)
        WRITES("Processing complete\n")
    $)  // processor.RELEASE() called automatically here
    
    WRITES("=== Test Complete ===\n")
$)
```

### Factory Pattern with RETAIN

```bcpl
// Factory function that creates and returns objects
LET create_configured_processor(config_file) = VALOF $(
    LET processor = NEW DataProcessor(2048)
    RETAIN processor  // Must retain - will be returned
    
    // Configure the processor
    processor.loadConfig(config_file)
    processor.initialize()
    
    RESULTIS processor
$)

LET START() BE $(
    // Create processor using factory
    LET my_processor = create_configured_processor("config.txt")
    
    // Use the processor
    my_processor.process_file("input.txt")
    my_processor.save_results("output.txt")
    
    // Must manually release retained object
    my_processor.RELEASE()
$)
```

### Mixed Memory Management

```bcpl
LET START() BE $(
    // Automatic object management
    LET obj = NEW MyClass()
    
    // Manual vector management  
    LET data = GETVEC(1000)
    
    // Static data (no management needed)
    LET constants = TABLE(1, 2, 3, 4, 5)
    
    // Use all three types
    obj.process(data, constants)
    
    // Manual cleanup for vector
    FREEVEC(data)
    
    // obj cleaned up automatically
    // constants need no cleanup
$)
```

---

## Summary

NewBCPL's memory management system provides the best of both worlds: automatic cleanup for safety and convenience, with manual control when needed. The key principles are:

1. **Let the compiler handle object cleanup** whenever possible
2. **Use RETAIN sparingly** and always pair with RELEASE  
3. **Always manually manage vectors** with GETVEC/FREEVEC
4. **Use heap tracing** to verify correct memory management
5. **Keep object lifetimes short** with narrow scopes

With these guidelines, you can write memory-safe BCPL programs that are both efficient and maintainable.