#!/bin/bash
# cleanup_old_runtime.sh - Script to remove old runtime files and update references

echo "=== BCPL Old Runtime Cleanup ==="
echo "This script will safely clean up the old runtime implementation."

# Set up safety measures
set -e # Exit on error
BACKUP_DIR="./old_runtime_backup_$(date +%Y%m%d_%H%M%S)"

echo "Creating backup directory at: $BACKUP_DIR"
mkdir -p "$BACKUP_DIR"

# Files to back up and remove
OLD_RUNTIME_FILES=(
  "runtime.cpp"
  "runtime.h"
)

# Back up the files first
echo "Backing up old runtime files..."
for file in "${OLD_RUNTIME_FILES[@]}"; do
  if [ -f "$file" ]; then
    echo "  Backing up $file"
    cp "$file" "$BACKUP_DIR/"
  else
    echo "  $file not found, skipping"
  fi
done

# Now safely remove the files
echo "Removing old runtime files..."
for file in "${OLD_RUNTIME_FILES[@]}"; do
  if [ -f "$file" ]; then
    echo "  Removing $file"
    rm "$file"
  fi
done

# Update build.sh to remove old runtime references
if [ -f "build.sh" ]; then
  echo "Updating build.sh to remove old runtime references..."
  cp build.sh "$BACKUP_DIR/build.sh"

  # Use sed to remove the line that includes runtime.cpp
  sed -i '' '/^runtime\.cpp$/d' build.sh

  echo "  Updated build.sh"
fi

echo ""
echo "Cleanup complete! Old runtime files have been backed up to: $BACKUP_DIR"
echo "Run './build.sh --clean' to rebuild the project with the new runtime."
echo ""
echo "NOTE: If you encounter any issues, you can restore the files from the backup."
