# NewBCPL Build Systems

This project provides two build systems for maximum flexibility and reliability.

## 🚀 Primary Build System: `comp.sh` (Python-based)

**Recommended for daily development work.**

```bash
./comp.sh                 # Clean build (default)
./comp.sh --incr          # Incremental build (faster)
./comp.sh --runtime=MODE  # Specify runtime mode
./comp.sh --help          # Show help
```

### Features
- ✅ **Beautiful progress bars** - Real-time progress with file count, speed, and completion percentage
- ✅ **Parallel compilation** - Uses all available CPU cores automatically
- ✅ **Better error handling** - Clear error messages and diagnostics
- ✅ **Cross-platform** - Works consistently on macOS, Linux, and Windows
- ✅ **Real-time feedback** - Shows exactly what's happening during build
- ✅ **No hanging issues** - Reliable completion every time

### Requirements
- Python 3.x
- `tqdm` library: `pip3 install tqdm`

### Example Output
```
🚀 Running Python build system...
Compiling: 100%|██████████| 411/411 [00:30<00:00, 13.45files/s, Success=411, Failed=0]
✅ Compilation succeeded, please continue
📝 Linking object files...
🎉 Build completed successfully using Python build system!
```

## 🔧 Fallback Build System: `build.sh` (Bash-based)

**Emergency backup when Python system is unavailable.**

```bash
./build.sh                # Clean build (default)
./build.sh --incr         # Incremental build
./build.sh --runtime=MODE # Specify runtime mode
```

### When to Use
- Python 3 is not available
- `tqdm` library cannot be installed
- Debugging build system issues
- Emergency situations

## 🏗️ Build Process Overview

Both systems follow the same build pipeline:

1. **Setup Phase**
   - Clean previous artifacts (clean builds only)
   - Create build directories
   - Increment version number

2. **Compilation Phase**
   - Discover all source files (.cpp)
   - Exclude test and debug files
   - Compile in parallel using all CPU cores
   - Track progress and handle errors

3. **Linking Phase**
   - Determine runtime library to use
   - Link object files with appropriate libraries
   - Handle platform-specific frameworks

4. **Post-Build Phase**
   - Code sign executable (macOS)
   - Copy executable to target location
   - Combine source files (optional)

## 📁 Source File Discovery

The build systems automatically find and compile:

- **Core files**: `*.cpp` in root directory
- **Encoders**: `encoders/**/*.cpp`
- **Passes**: `passes/**/*.cpp`
- **Code generation**: `codegen/**/*.cpp`
- **Analysis**: `analysis/**/*.cpp` (including subdirectories)
- **Generators**: `generators/**/*.cpp`
- **Heap management**: `HeapManager/*.cpp`
- **Optimizer patterns**: `optimizer/patterns/*.cpp`
- **Live analysis**: `live_*.cpp`
- **Signal handling**: `SignalSafeUtils.cpp`, `SignalHandler.cpp`

### Excluded Files
- `test_*.cpp` - Test files
- `debug_*.cpp` - Debug files  
- Files in `test*` directories
- `peephole_test.cpp`
- `test_nop_validation.cpp`

## ⚡ Performance Comparison

| Aspect | Python (`comp.sh`) | Bash (`build.sh`) |
|--------|-------------------|-------------------|
| **Progress Feedback** | Real-time progress bar | Simple text output |
| **Error Handling** | Detailed diagnostics | Basic error messages |
| **Parallel Compilation** | Reliable, monitored | Basic xargs |
| **Build Speed** | ~30 seconds (411 files) | Similar speed |
| **Reliability** | No hanging issues | Occasional hangs |
| **User Experience** | Excellent visual feedback | Minimal feedback |

## 🛠️ Troubleshooting

### Python Build Issues
```bash
# Missing tqdm
pip3 install tqdm

# Permission issues
chmod +x comp.sh build.py

# Missing Python 3
# Install Python 3 for your platform
```

### General Build Issues
```bash
# Check for compilation errors
cat errors.txt

# Clean build to resolve dependency issues
./comp.sh  # or ./build.sh

# Use fallback system
./build.sh
```

## 🔄 Migration from Bash to Python

If you've been using `build.sh`, simply switch to `comp.sh`:

```bash
# Old way
./build.sh --incr

# New way (same arguments)
./comp.sh --incr
```

All command-line arguments are identical between the two systems.

## 📈 Future Improvements

The Python build system enables future enhancements:

- **Build caching** - Intelligent dependency tracking
- **Distributed compilation** - Network-based parallel builds
- **Build analytics** - Compilation time analysis
- **IDE integration** - Better tooling support
- **Custom build profiles** - Different optimization levels

## 🎯 Recommendation

**Use `comp.sh` for all development work.** It provides a significantly better developer experience with reliable builds, beautiful progress feedback, and excellent error handling.

Keep `build.sh` available as a fallback for emergency situations or environments where Python is not available.