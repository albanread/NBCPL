#!/bin/bash

# Define the output file and the source directory
OUTPUT_FILE="combined_encoders.txt"
SOURCE_DIR="encoders"

# Ensure the output file is empty before starting
> "$OUTPUT_FILE"

# Check if the source directory exists
if [ ! -d "$SOURCE_DIR" ]; then
  echo "Error: Directory '$SOURCE_DIR' not found."
  exit 1
fi

# Find all files in the source directory and process them
find "$SOURCE_DIR" -type f -print0 | while IFS= read -r -d $'\0' file; do
  # Add a comment with the filename to the output file
  echo "// $file" >> "$OUTPUT_FILE"
  # Append the content of the file to the output file
  cat "$file" >> "$OUTPUT_FILE"
  # Add a newline for separation between files
  echo "" >> "$OUTPUT_FILE"
done

echo "✅ Success! All files from '$SOURCE_DIR' have been combined into '$OUTPUT_FILE'."

