#!/bin/bash

# --- Script to combine all .cpp files into one ---

# Check if the correct number of arguments is provided
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <source_directory> <output_file>"
    exit 1
fi

SOURCE_DIR="$1"
OUTPUT_FILE="$2"

# Check if the source directory exists
if [ ! -d "$SOURCE_DIR" ]; then
    echo "❌ Error: Source directory '$SOURCE_DIR' not found."
    exit 1
fi

# Create or clear the output file
> "$OUTPUT_FILE"

echo "🔎 Searching for .cpp files in '$SOURCE_DIR'..."

# Find all files ending in .cpp, sort them, and process them.
# The -print0 and read -d '' combo safely handles filenames with spaces.
find "$SOURCE_DIR" -type f -name "*.cpp" -print0 | sort -z | while IFS= read -r -d '' file; do
    # Get the path relative to the source directory for a cleaner comment
    relative_path="${file#$SOURCE_DIR/}"
    
    echo "Processing: $relative_path"

    # 1. Append the start comment to the output file
    echo "// -- start of file: $relative_path" >> "$OUTPUT_FILE"
    
    # 2. Append the content of the found file
    cat "$file" >> "$OUTPUT_FILE"
    
    # 3. Append the end comment, with newlines for nice spacing
    echo -e "\n// -- end of file\n" >> "$OUTPUT_FILE"
done

echo -e "\n✅ Success! All .cpp files have been combined into '$OUTPUT_FILE'."
