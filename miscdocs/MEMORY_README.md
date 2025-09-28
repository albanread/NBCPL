# NewBCPL Memory Management

## Quick Start

NewBCPL provides automatic memory management that eliminates most memory leaks while maintaining the simplicity of BCPL programming.

### Basic Usage

```bcpl
CLASS MyClass $(
    // Class definition
$)

LET START() BE $(
    $(  // Create a scope
        LET obj = NEW MyClass()
        // Use obj...
    $)  // obj.RELEASE() called automatically here
    
    // obj is now cleaned up - no memory leak!
$)
```

### Key Principles

1. **Objects are automatically cleaned up** when they go out of scope
2. **Vectors must be manually managed** with `GETVEC`/`FREEVEC`
3. **Use `RETAIN` sparingly** when you need manual control
4. **Always check metrics** to verify no memory leaks

### Testing Your Memory Management

Run any program with heap tracing to see allocations and deallocations:

```bash
./NewBCPL myprogram.bcl --run --trace-heap
```

Look for this at the end:
```
--- BCPL Runtime Metrics ---
Memory allocations: 5 (120 bytes)
Memory frees: 5 (120 bytes)
Current active allocations: 0 (0 bytes)  ← Should be 0!
```

### Complete Guide

For detailed documentation, examples, and best practices, see:
- [`memory_guide.md`](memory_guide.md) - Comprehensive memory management guide
- [`cleanup_test.bcl`](cleanup_test.bcl) - Working example program

### Common Patterns

**✅ Good - Automatic cleanup:**
```bcpl
LET process_file() BE $(
    LET processor = NEW FileProcessor()
    processor.load("data.txt")
    processor.process()
    // processor cleaned up automatically
$)
```

**❌ Avoid - Manual management unless needed:**
```bcpl
LET process_file() BE $(
    LET processor = NEW FileProcessor()
    RETAIN processor  // Usually unnecessary
    processor.load("data.txt")
    processor.process()
    processor.RELEASE()  // Easy to forget!
$)
```

**✅ Required - Vector management:**
```bcpl
LET process_data() BE $(
    LET data = GETVEC(1000)
    // Use data...
    FREEVEC(data)  // Must manually free vectors
$)
```

### Debugging Memory Issues

1. **Memory leaks**: Check that final metrics show 0 active allocations
2. **Use-after-free**: Don't use objects after their scope ends
3. **Double-free**: Don't call RELEASE twice on the same object

### Implementation Status

- ✅ Basic automatic cleanup: **Working**
- ✅ Heap tracing and metrics: **Working**  
- ✅ Object allocation/deallocation: **Working**
- ⚠️ RETAIN keyword: **Experimental**
- ⚠️ Complex inheritance: **In development**

The core automatic memory management is production-ready and eliminates the vast majority of memory management bugs in typical BCPL programs.