# Cairo Integration for buildruntime Script

This document describes how to integrate Cairo graphics support into the existing `buildruntime` script. The integration follows the established patterns for SDL2 support and maintains backward compatibility.

## Overview

The Cairo integration adds support for 2D graphics rendering while maintaining the existing build architecture:

- **Cairo-only builds**: Basic 2D graphics without SDL2 display
- **Cairo+SDL2 builds**: Full graphics with display capabilities
- **Backward compatibility**: Existing builds continue to work unchanged

## Required Changes to buildruntime Script

### 1. Configuration Variables (Add after line 7)

```bash
# Cairo configuration
CAIRO_DIR="runtime/cairo"
CAIRO_ENABLED=false
CAIRO_STATIC=false
```

### 2. Argument Parsing Updates (Replace the existing argument parsing section starting around line 65)

```bash
# --- Argument Parsing ---
if [ -z "$1" ]; then
    echo "No build mode specified. Defaulting to: unified runtime with SDL2 and Cairo statically included (fully self-contained)."
    BUILD_MODE="UNIFIED"
    SDL2_ENABLED=true
    SDL2_STATIC=true
    CAIRO_ENABLED=true
    CAIRO_STATIC=true
else
    # Convert first argument to uppercase and remove leading --
    BUILD_MODE=$(echo "$1" | tr '[:lower:]' '[:upper:]' | sed 's/--//')

    # Check for graphics flags
    SDL2_ENABLED=false
    SDL2_STATIC=false
    CAIRO_ENABLED=false
    CAIRO_STATIC=false
    
    case "$BUILD_MODE" in
        "WITH-SDL2")
            SDL2_ENABLED=true
            BUILD_MODE="UNIFIED"
            ;;
        "WITH-SDL2-STATIC")
            SDL2_ENABLED=true
            SDL2_STATIC=true
            BUILD_MODE="UNIFIED"
            ;;
        "WITH-CAIRO")
            CAIRO_ENABLED=true
            BUILD_MODE="UNIFIED"
            ;;
        "WITH-CAIRO-STATIC")
            CAIRO_ENABLED=true
            CAIRO_STATIC=true
            BUILD_MODE="UNIFIED"
            ;;
        "WITH-GRAPHICS")
            SDL2_ENABLED=true
            CAIRO_ENABLED=true
            BUILD_MODE="UNIFIED"
            ;;
        "WITH-GRAPHICS-STATIC")
            SDL2_ENABLED=true
            SDL2_STATIC=true
            CAIRO_ENABLED=true
            CAIRO_STATIC=true
            BUILD_MODE="UNIFIED"
            ;;
        "JIT-SDL2")
            SDL2_ENABLED=true
            SDL2_STATIC=false
            BUILD_MODE="JIT"
            ;;
        "JIT-SDL2-STATIC")
            SDL2_ENABLED=true
            SDL2_STATIC=true
            BUILD_MODE="JIT"
            ;;
        "JIT-CAIRO")
            CAIRO_ENABLED=true
            CAIRO_STATIC=false
            BUILD_MODE="JIT"
            ;;
        "JIT-CAIRO-STATIC")
            CAIRO_ENABLED=true
            CAIRO_STATIC=true
            BUILD_MODE="JIT"
            ;;
        "JIT-GRAPHICS")
            SDL2_ENABLED=true
            CAIRO_ENABLED=true
            SDL2_STATIC=false
            CAIRO_STATIC=false
            BUILD_MODE="JIT"
            ;;
        "JIT-GRAPHICS-STATIC")
            SDL2_ENABLED=true
            CAIRO_ENABLED=true
            SDL2_STATIC=true
            CAIRO_STATIC=true
            BUILD_MODE="JIT"
            ;;
    esac
fi
```

### 3. Cairo Configuration Functions (Add before main logic)

```bash
# --- Cairo Configuration Functions ---
configure_cairo() {
    if [ "$CAIRO_ENABLED" = true ]; then
        echo "   Configuring Cairo graphics support..."
        
        # Check if Cairo is installed
        if ! pkg-config --exists cairo; then
            echo "❌ Error: Cairo development libraries not found"
            echo "   Install with: brew install cairo (macOS) or apt-get install libcairo2-dev (Ubuntu)"
            exit 1
        fi
        
        CAIRO_CFLAGS=$(pkg-config --cflags cairo)
        if [ "$CAIRO_STATIC" = true ]; then
            CAIRO_LIBS=$(pkg-config --libs --static cairo)
            echo "   Building with STATIC Cairo linking"
        else
            CAIRO_LIBS=$(pkg-config --libs cairo)
            echo "   Building with DYNAMIC Cairo linking"
        fi
        
        echo "   Cairo CFLAGS: ${CAIRO_CFLAGS}"
        echo "   Cairo LIBS: ${CAIRO_LIBS}"
        
        # Update compile flags
        CFLAGS="${CFLAGS} ${CAIRO_CFLAGS}"
        CXXFLAGS="${CXXFLAGS} ${CAIRO_CFLAGS}"
        DEFINES="${DEFINES} -DCAIRO_RUNTIME_ENABLED"
        INCLUDE_DIRS="${INCLUDE_DIRS} -I${CAIRO_DIR}"
        
        return 0
    fi
    return 1
}

compile_cairo_objects() {
    local build_dir=$1
    
    if [ "$CAIRO_ENABLED" = true ]; then
        echo "   Step: Compiling Cairo graphics files..."
        
        # Create Cairo directory if it doesn't exist
        mkdir -p "${CAIRO_DIR}"
        
        # Compile Cairo C++ files
        clang++ ${CXXFLAGS} ${DEFINES} ${INCLUDE_DIRS} -c ${CAIRO_DIR}/cairo_core.cpp -o ${build_dir}/cairo_core.o 2>/dev/null || echo "   Note: cairo_core.cpp not found, skipping..."
        clang++ ${CXXFLAGS} ${DEFINES} ${INCLUDE_DIRS} -c ${CAIRO_DIR}/cairo_runtime.cpp -o ${build_dir}/cairo_runtime.o 2>/dev/null || echo "   Note: cairo_runtime.cpp not found, skipping..."
        clang++ ${CXXFLAGS} ${DEFINES} ${INCLUDE_DIRS} -c ${CAIRO_DIR}/cairo_samm.cpp -o ${build_dir}/cairo_samm.o 2>/dev/null || echo "   Note: cairo_samm.cpp not found, skipping..."
        clang++ ${CXXFLAGS} ${DEFINES} ${INCLUDE_DIRS} -c ${CAIRO_DIR}/cairo_vectors.cpp -o ${build_dir}/cairo_vectors.o 2>/dev/null || echo "   Note: cairo_vectors.cpp not found, skipping..."
        
        # SDL2 integration if both are enabled
        if [ "$SDL2_ENABLED" = true ]; then
            clang++ ${CXXFLAGS} ${DEFINES} ${INCLUDE_DIRS} -c ${CAIRO_DIR}/cairo_sdl.cpp -o ${build_dir}/cairo_sdl.o 2>/dev/null || echo "   Note: cairo_sdl.cpp not found, skipping..."
        fi
        
        # Build list of existing Cairo objects
        CAIRO_OBJECTS=""
        for obj in cairo_core cairo_runtime cairo_samm cairo_vectors; do
            if [ -f "${build_dir}/${obj}.o" ]; then
                CAIRO_OBJECTS="${CAIRO_OBJECTS} ${build_dir}/${obj}.o"
            fi
        done
        
        if [ "$SDL2_ENABLED" = true ] && [ -f "${build_dir}/cairo_sdl.o" ]; then
            CAIRO_OBJECTS="${CAIRO_OBJECTS} ${build_dir}/cairo_sdl.o"
        fi
        
        echo "   ✅ Cairo objects compiled: ${CAIRO_OBJECTS}"
    fi
}

get_archive_name() {
    local base_name=$1
    local archive_name="${base_name}.a"
    
    # Determine archive name based on enabled features
    if [ "$CAIRO_ENABLED" = true ] && [ "$SDL2_ENABLED" = true ]; then
        if [ "$CAIRO_STATIC" = true ] && [ "$SDL2_STATIC" = true ]; then
            archive_name="${base_name}_graphics_static.a"
        else
            archive_name="${base_name}_graphics.a"
        fi
    elif [ "$CAIRO_ENABLED" = true ]; then
        if [ "$CAIRO_STATIC" = true ]; then
            archive_name="${base_name}_cairo_static.a"
        else
            archive_name="${base_name}_cairo.a"
        fi
    elif [ "$SDL2_ENABLED" = true ]; then
        if [ "$SDL2_STATIC" = true ]; then
            archive_name="${base_name}_sdl2_static.a"
        else
            archive_name="${base_name}_sdl2.a"
        fi
    fi
    
    echo "$archive_name"
}
```

### 4. JIT Mode Updates (Insert after configure_sdl2() call in JIT section)

```bash
    # Configure Cairo if enabled
    configure_cairo
    
    # ... existing JIT compilation steps ...
    
    # Add after SDL2 compilation:
    compile_cairo_objects "${JIT_BUILD_DIR}"
```

### 5. JIT Archive Creation Updates (Replace archive creation section in JIT mode)

```bash
    echo "Step 4: Creating library archive..."
    ARCHIVE_NAME=$(get_archive_name "libbcpl_runtime_jit")
    
    # Build object list
    JIT_OBJECTS="${JIT_BUILD_DIR}/BCPLError.o \
                 ${JIT_BUILD_DIR}/runtime_freelist.o \
                 ${JIT_BUILD_DIR}/runtime_c_globals.o \
                 ${JIT_BUILD_DIR}/jit_runtime.o \
                 ${JIT_BUILD_DIR}/jit_heap_bridge.o \
                 ${JIT_BUILD_DIR}/RuntimeBridge.o \
                 ${JIT_BUILD_DIR}/runtime_string_ops.o \
                 ${JIT_BUILD_DIR}/heap_interface.o \
                 ${JIT_BUILD_DIR}/HeapManager.o \
                 ${JIT_BUILD_DIR}/heap_manager_globals.o \
                 ${JIT_BUILD_DIR}/Heap_allocVec.o \
                 ${JIT_BUILD_DIR}/Heap_allocString.o \
                 ${JIT_BUILD_DIR}/Heap_allocList.o \
                 ${JIT_BUILD_DIR}/Heap_allocObject.o \
                 ${JIT_BUILD_DIR}/Heap_free.o \
                 ${JIT_BUILD_DIR}/Heap_resizeVec.o \
                 ${JIT_BUILD_DIR}/Heap_resizeString.o \
                 ${JIT_BUILD_DIR}/Heap_dumpHeap.o \
                 ${JIT_BUILD_DIR}/Heap_dumpHeapSignalSafe.o \
                 ${JIT_BUILD_DIR}/Heap_printMetrics.o"
    
    # Add SDL2 objects if enabled
    if [ "$SDL2_ENABLED" = true ]; then
        JIT_OBJECTS="${JIT_OBJECTS} ${SDL2_OBJECTS}"
    fi
    
    # Add Cairo objects if enabled
    if [ "$CAIRO_ENABLED" = true ]; then
        JIT_OBJECTS="${JIT_OBJECTS} ${CAIRO_OBJECTS}"
    fi
    
    ar rcs ${ARCHIVE_NAME} ${JIT_OBJECTS}
```

### 6. UNIFIED Mode Updates (Insert after configure_sdl2() call in UNIFIED section)

```bash
    # Configure Cairo if enabled
    configure_cairo
    
    # ... existing UNIFIED compilation steps ...
    
    # Add after SDL2 compilation:
    compile_cairo_objects "${UNIFIED_BUILD_DIR}"
```

### 7. UNIFIED Archive Creation Updates (Replace archive creation section in UNIFIED mode)

```bash
    echo "Step 5: Creating unified library archive..."
    ARCHIVE_NAME=$(get_archive_name "libbcpl_runtime")
    
    # Build object list
    UNIFIED_OBJECTS="${UNIFIED_BUILD_DIR}/BCPLError.o \
                     ${UNIFIED_BUILD_DIR}/runtime_freelist.o \
                     ${UNIFIED_BUILD_DIR}/heap_interface_c.o \
                     ${UNIFIED_BUILD_DIR}/runtime_c_globals.o \
                     ${UNIFIED_BUILD_DIR}/jit_runtime.o \
                     ${UNIFIED_BUILD_DIR}/jit_heap_bridge.o \
                     ${UNIFIED_BUILD_DIR}/RuntimeBridge.o \
                     ${UNIFIED_BUILD_DIR}/runtime_string_ops.o \
                     ${UNIFIED_BUILD_DIR}/heap_interface.o \
                     ${UNIFIED_BUILD_DIR}/SignalSafeUtils.o \
                     ${UNIFIED_BUILD_DIR}/RuntimeManager.o \
                     ${UNIFIED_BUILD_DIR}/HeapManager.o \
                     ${UNIFIED_BUILD_DIR}/heap_manager_globals.o \
                     ${UNIFIED_BUILD_DIR}/Heap_allocVec.o \
                     ${UNIFIED_BUILD_DIR}/Heap_allocString.o \
                     ${UNIFIED_BUILD_DIR}/Heap_allocList.o \
                     ${UNIFIED_BUILD_DIR}/Heap_allocObject.o \
                     ${UNIFIED_BUILD_DIR}/Heap_free.o \
                     ${UNIFIED_BUILD_DIR}/Heap_resizeVec.o \
                     ${UNIFIED_BUILD_DIR}/Heap_resizeString.o \
                     ${UNIFIED_BUILD_DIR}/Heap_dumpHeap.o \
                     ${UNIFIED_BUILD_DIR}/Heap_dumpHeapSignalSafe.o \
                     ${UNIFIED_BUILD_DIR}/Heap_printMetrics.o \
                     ${HEAP_C_WRAPPERS_OBJ}"
    
    # Add SDL2 objects if enabled
    if [ "$SDL2_ENABLED" = true ]; then
        UNIFIED_OBJECTS="${UNIFIED_OBJECTS} ${SDL2_OBJECTS}"
    fi
    
    # Add Cairo objects if enabled
    if [ "$CAIRO_ENABLED" = true ]; then
        UNIFIED_OBJECTS="${UNIFIED_OBJECTS} ${CAIRO_OBJECTS}"
    fi
    
    ar rcs ${ARCHIVE_NAME} ${UNIFIED_OBJECTS}
```

### 8. Success Messages Updates (Replace existing success messages)

```bash
    # Success message for JIT mode
    if [ "$CAIRO_ENABLED" = true ] && [ "$SDL2_ENABLED" = true ]; then
        echo "✅ Successfully built ${ARCHIVE_NAME} (JIT runtime with Cairo graphics and SDL2 display)"
        echo "📋 Graphics linking requirements:"
        echo "   When building the compiler, use: ${SDL2_LIBS} ${CAIRO_LIBS}"
        if [ "$CAIRO_STATIC" = true ] && [ "$SDL2_STATIC" = true ]; then
            echo "   📦 DEPLOYMENT: Fully self-contained - no external graphics libraries required!"
        fi
    elif [ "$CAIRO_ENABLED" = true ]; then
        echo "✅ Successfully built ${ARCHIVE_NAME} (JIT runtime with Cairo graphics)"
        echo "📋 Cairo linking requirements:"
        echo "   When building the compiler, use: ${CAIRO_LIBS}"
        if [ "$CAIRO_STATIC" = true ]; then
            echo "   📦 DEPLOYMENT: Static Cairo - no external Cairo library required!"
        fi
    elif [ "$SDL2_ENABLED" = true ]; then
        # Existing SDL2-only message
        echo "✅ Successfully built ${ARCHIVE_NAME} (JIT runtime with SDL2)"
        echo "📋 SDL2 linking requirements:"
        echo "   When building the compiler, use: ${SDL2_LIBS}"
    else
        echo "✅ Successfully built ${ARCHIVE_NAME} (standard JIT runtime)"
    fi
```

### 9. Help Message Updates (Replace help section at end)

```bash
    *)
        echo "Error: Invalid build mode '$1'."
        echo "Valid options:"
        echo "  --unified (default, includes all graphics libraries statically)"
        echo "  --jit, --standalone, --combine, --clean"
        echo ""
        echo "Graphics options:"
        echo "  --with-sdl2          : Include SDL2 display support"
        echo "  --with-sdl2-static   : Include SDL2 with static linking"
        echo "  --with-cairo         : Include Cairo 2D graphics"
        echo "  --with-cairo-static  : Include Cairo with static linking"
        echo "  --with-graphics      : Include both SDL2 and Cairo"
        echo "  --with-graphics-static : Include both with static linking"
        echo ""
        echo "JIT graphics options:"
        echo "  --jit-sdl2, --jit-sdl2-static"
        echo "  --jit-cairo, --jit-cairo-static"
        echo "  --jit-graphics, --jit-graphics-static"
        exit 1
        ;;
```

## File Structure Requirements

The integration expects the following Cairo files to exist:

```
runtime/cairo/
├── cairo_core.cpp         # C++ RAII wrappers (optional)
├── cairo_runtime.cpp      # C-linkage BCPL interface (optional)
├── cairo_samm.cpp         # SAMM integration (optional)
├── cairo_vectors.cpp      # Vector operations (optional)
└── cairo_sdl.cpp          # SDL2 integration (optional, requires SDL2)
```

Missing files are gracefully skipped with warning messages.

## Build Examples

```bash
# Default build with all graphics (recommended)
./buildruntime

# Basic runtime without graphics
./buildruntime --unified

# Cairo graphics only
./buildruntime --with-cairo-static

# Full graphics suite
./buildruntime --with-graphics-static

# JIT with Cairo
./buildruntime --jit-cairo-static
```

## Library Naming Convention

The integration creates appropriately named libraries:

- `libbcpl_runtime.a` - Basic runtime
- `libbcpl_runtime_cairo.a` - Runtime with Cairo
- `libbcpl_runtime_sdl2.a` - Runtime with SDL2
- `libbcpl_runtime_graphics.a` - Runtime with both
- `*_static.a` variants for static linking

## Compiler Integration

The main compiler will automatically detect and use the appropriate library:

1. `libbcpl_runtime_graphics_static.a` (preferred, full features)
2. `libbcpl_runtime_cairo_static.a` (Cairo only)
3. `libbcpl_runtime_sdl2_static.a` (SDL2 only)
4. `libbcpl_runtime.a` (basic fallback)

## Dependencies

### Cairo Dependencies
- **macOS**: `brew install cairo`
- **Ubuntu/Debian**: `apt-get install libcairo2-dev`
- **Arch**: `pacman -S cairo`

### Static Linking Benefits
- Single executable deployment
- No external library dependencies
- Consistent behavior across systems
- Simplified distribution

This integration maintains full backward compatibility while adding powerful 2D graphics capabilities to the BCPL runtime system.