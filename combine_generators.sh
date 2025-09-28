#!/bin/bash

# Define the output file
output_file="generators/combined.txt"

# Remove the output file if it already exists
if [ -f "$output_file" ]; then
    rm "$output_file"
fi

# Add a header to the combined file
echo "// Combined generator files for code review" > "$output_file"

# Iterate over all generator .cpp files and append their content
for file in generators/gen_*.cpp; do
    echo -e "\n// File: $file\n" >> "$output_file"
    cat "$file" >> "$output_file"
done

echo "All generator files have been combined into $output_file"
