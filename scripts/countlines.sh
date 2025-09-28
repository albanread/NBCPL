#!/bin/bash
# countlines.sh - Count lines of C++/C source code in .h, .cpp, .c files

# Usage: ./countlines.sh [directory]
# If no directory is specified, use the current directory.

DIR="${1:-.}"

FILE_LIST=$(find "$DIR" -maxdepth 4 -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.c" \))

# Get unique folders containing source files
FOLDER_LIST=$(echo "$FILE_LIST" | xargs -n1 dirname | sort | uniq)

TOTAL_FILES=0
TOTAL_LINES=0

for folder in $FOLDER_LIST; do
    FILES_IN_FOLDER=$(find "$folder" -maxdepth 1 -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.c" \))
    FILE_COUNT=$(echo "$FILES_IN_FOLDER" | wc -l)
    if [ "$FILE_COUNT" -eq 0 ]; then
        continue
    fi
    LINE_COUNT=$(echo "$FILES_IN_FOLDER" | xargs cat 2>/dev/null | wc -l)
    echo "Folder: $folder"
    echo "  Files: $FILE_COUNT"
    echo "  Lines: $LINE_COUNT"
    TOTAL_FILES=$((TOTAL_FILES + FILE_COUNT))
    TOTAL_LINES=$((TOTAL_LINES + LINE_COUNT))
done

echo "----------------------"
echo "Total files: $TOTAL_FILES"
echo "Total lines: $TOTAL_LINES"
