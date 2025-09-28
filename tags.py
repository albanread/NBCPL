import os
import re
import sys

def add_unique_debug_tags(root_dir):
    """
    Recursively finds all .cpp files in a directory and replaces
    '[CODEGEN-DEBUG]' with a uniquely numbered version, e.g., '[CODEGEN-DEBUG-0001]'.
    """
    # Initialize a global counter for unique IDs.
    counter = 0
    files_to_process = []

    # --- Pass 1: Find all .cpp files ---
    # This avoids modifying directory contents while iterating over them.
    for subdir, _, files in os.walk(root_dir):
        for file in files:
            if file.endswith('.cpp'):
                files_to_process.append(os.path.join(subdir, file))

    print(f"Found {len(files_to_process)} C++ source files to process.")

    # --- Pass 2: Read, replace, and write back ---
    for filepath in files_to_process:
        try:
            # Read the entire file content.
            with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()

            # Use a function as the replacement argument to re.sub.
            # This function will be called for every match found.
            def replacer(match):
                nonlocal counter
                # Format the counter with zero-padding to 4 digits.
                replacement_tag = f"[CODEGEN-DEBUG-{counter:04d}]"
                counter += 1
                return replacement_tag

            # Use re.sub to find all occurrences of the exact string '[CODEGEN-DEBUG]'
            # and replace each one using our replacer function.
            new_content, num_replacements = re.subn(r'\[CODEGEN-DEBUG\]', replacer, content)

            # Only write back to the file if changes were actually made.
            if num_replacements > 0:
                print(f"Updating {filepath} ({num_replacements} tags replaced)...")
                with open(filepath, 'w', encoding='utf-8') as f:
                    f.write(new_content)

        except Exception as e:
            print(f"  [ERROR] Could not process file {filepath}: {e}", file=sys.stderr)

    print("\n✅ Success! All debug tags have been uniquely numbered.")
    print(f"   Total tags found and replaced: {counter}")

if __name__ == "__main__":
    # Start the process from the current directory where the script is run.
    target_directory = '.'
    print(f"Starting debug tag numbering in directory: {os.path.abspath(target_directory)}")
    add_unique_debug_tags(target_directory)

