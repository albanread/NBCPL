#!/bin/bash
# Usage: ./getcrash.sh <bcpl_file>
# Runs NewBCPL under LLDB with the given BCPL file and invokes the run_bcpl_test command from lldb_script.py

if [ $# -ne 1 ]; then
  echo "Usage: $0 <bcpl_file>"
  exit 1
fi

BCPL_FILE="$1"

# Create a temporary LLDB command file
tmpcmd=$(mktemp /tmp/lldbcmd.XXXXXX)

cat > "$tmpcmd" <<EOF
process handle -s true -n false -p true SIGTRAP
command script import lldb_script.py
file ./NewBCPL
settings set -- target.run-args "$BCPL_FILE" --run
run_bcpl_test
EOF

echo "Launching LLDB with $BCPL_FILE..."
lldb --source "$tmpcmd"

# Clean up
rm "$tmpcmd"
