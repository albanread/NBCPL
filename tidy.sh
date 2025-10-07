NewBCPL/tidy.sh
#!/bin/bash
# Move stray .bcl, .bcpl, .s, .lst files into tests/bcl_tests

set -e

DEST="tests/bcl_tests"

# Ensure destination exists
mkdir -p "$DEST"

# Find and move files, excluding those already in the destination
find . -type f \( -name "*.bcl" -o -name "*.bcpl" -o -name "*.s" -o -name "*.lst" \) \
  ! -path "./$DEST/*" \
  ! -path "./$DEST" \
  -print0 | while IFS= read -r -d '' file; do
    # Remove leading ./ from filename
    fname=$(basename "$file")
    # Move file to destination, overwrite if exists
    mv -f "$file" "$DEST/$fname"
    echo "Moved: $file -> $DEST/$fname"
done
