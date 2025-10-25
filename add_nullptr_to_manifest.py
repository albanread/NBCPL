#!/usr/bin/env python3
"""
Script to add nullptr parameter_types to all existing RuntimeFunctionDescriptor entries
in runtime_api.cpp to support the new parameter type system.
"""

import re
import sys


def process_manifest_file(filename):
    """Add nullptr to all manifest entries that don't already have parameter_types."""

    with open(filename, "r") as f:
        content = f.read()

    # Pattern to match manifest entries - looking for entries that end with "CATEGORY" }, without parameter_types
    # This matches the pattern: "description", "CATEGORY" },
    pattern = r'(".*?",\s*"[^"]*")\s*\},'

    def replacement(match):
        # Check if this line already has parameter_types (contains 'nullptr' or array name)
        line = match.group(0)
        if "nullptr" in line or "_params" in line:
            return line  # Already has parameter_types, leave unchanged

        # Add nullptr before the closing }
        return match.group(1) + ", nullptr\n    },"

    # Apply the replacement
    new_content = re.sub(pattern, replacement, content, flags=re.MULTILINE)

    # Write back to file
    with open(filename, "w") as f:
        f.write(new_content)

    print(f"Updated {filename} - added nullptr to manifest entries")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python3 add_nullptr_to_manifest.py runtime_api.cpp")
        sys.exit(1)

    filename = sys.argv[1]
    process_manifest_file(filename)
    print("Done!")
