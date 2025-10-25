#!/bin/bash

# comp.sh - Wrapper script for the Python build system
# Usage:
#   ./comp.sh                 - Clean build (default, recommended)
#   ./comp.sh --incr          - Incremental build (faster for development)
#   ./comp.sh --runtime=MODE  - Specify runtime mode (jit, standalone, unified)
#   ./comp.sh --help          - Show this help message
#
# This script runs the Python build system (build.py) which provides:
# - Beautiful progress bars with tqdm
# - Better error handling
# - Reliable parallel compilation
# - Cross-platform compatibility

# Check for help option
if [[ "$1" == "--help" || "$1" == "-h" ]]; then
    echo "comp.sh - NewBCPL Python Build System"
    echo ""
    echo "USAGE:"
    echo "  ./comp.sh                 Clean build (default, recommended)"
    echo "  ./comp.sh --incr          Incremental build (faster for development)"
    echo "  ./comp.sh --runtime=MODE  Specify runtime mode (jit, standalone, unified)"
    echo "  ./comp.sh --help          Show this help message"
    echo ""
    echo "FEATURES:"
    echo "  ✅ Beautiful progress bars with file count and speed"
    echo "  ✅ Parallel compilation using all CPU cores"
    echo "  ✅ Better error handling and diagnostics"
    echo "  ✅ Cross-platform compatibility"
    echo "  ✅ Real-time compilation feedback"
    echo ""
    echo "REQUIREMENTS:"
    echo "  - Python 3.x"
    echo "  - tqdm library (pip3 install tqdm)"
    echo ""
    echo "FALLBACK:"
    echo "  If Python build fails, use: ./build.sh"
    echo ""
    exit 0
fi

# Check if Python 3 is available
if ! command -v python3 &> /dev/null; then
    echo "❌ Error: Python 3 is required but not found."
    echo "Please install Python 3 or use ./build.sh as fallback."
    exit 1
fi

# Check if tqdm is available
if ! python3 -c "import tqdm" 2>/dev/null; then
    echo "❌ Error: tqdm library is required but not found."
    echo "Please install with: pip3 install tqdm"
    echo "Or use ./build.sh as fallback."
    exit 1
fi

# Check if build.py exists
if [ ! -f "build.py" ]; then
    echo "❌ Error: build.py not found in current directory."
    echo "Please run this script from the NewBCPL directory."
    exit 1
fi

# Make sure build.py is executable
chmod +x build.py

# Run the Python build script with all arguments
echo "🚀 Running Python build system..."
./build.py "$@"

# Capture exit code and provide feedback
exit_code=$?

if [ $exit_code -eq 0 ]; then
    echo ""
    echo "🎉 Build completed successfully using Python build system!"
    echo "   💡 Tip: Use './comp.sh --incr' for faster incremental builds"
    echo "   📚 Run './comp.sh --help' for more options"
else
    echo ""
    echo "❌ Python build failed (exit code: $exit_code)"
    echo "   🔧 Fallback option: ./build.sh"
    echo "   📋 Check errors.txt for detailed error information"
fi

exit $exit_code
