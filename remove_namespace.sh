#!/bin/bash

# Script to remove bcpl:: namespace references from the project
# This will address the 10,000 error cascade across hundreds of files

echo "Starting namespace removal script..."

# First, remove "namespace bcpl {" and "} // namespace bcpl" from all files
find . -type f -name "*.h" -o -name "*.cpp" | xargs grep -l "namespace bcpl" | while read file; do
  echo "Removing namespace declaration from $file"
  # Remove the namespace opening line
  sed -i '' -e 's/namespace bcpl {//' "$file"
  # Remove the namespace closing line
  sed -i '' -e 's/} \/\/ namespace bcpl//' "$file"
done

# Next, remove all bcpl:: references from code
find . -type f -name "*.h" -o -name "*.cpp" | xargs grep -l "bcpl::" | while read file; do
  echo "Removing bcpl:: references from $file"
  sed -i '' -e 's/bcpl:://g' "$file"
done

# Special handling for using statements with bcpl namespace
find . -type f -name "*.h" -o -name "*.cpp" | xargs grep -l "using bcpl::" | while read file; do
  echo "Fixing using statements in $file"
  sed -i '' -e 's/using bcpl::/using /g' "$file"
done

# Handle 'using namespace bcpl' statements
find . -type f -name "*.h" -o -name "*.cpp" | xargs grep -l "using namespace bcpl" | while read file; do
  echo "Removing 'using namespace bcpl' from $file"
  sed -i '' -e 's/using namespace bcpl;//g' "$file"
done

# Count the remaining references to double-check
remaining=$(grep -r "bcpl::" --include="*.h" --include="*.cpp" . | wc -l)
echo "Finished processing files. Remaining references to check: $remaining"

echo "Done!"
