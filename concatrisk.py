import os

def find_and_save_runtime_files(search_directory, output_file):
    """
    Finds C++ source (.cpp) and header (.h) files with 'runtime' or 'heap'
    in their names, or files located in a folder named 'runtime', and
    saves their content into a single output file.

    Args:
        search_directory (str): The path to the directory to start the search from.
        output_file (str): The path to the file where the content will be saved.
    """
    # Use a set to store the paths of found files to avoid duplicates
    found_files = set()
    allowed_extensions = ('.cpp', '.h')

    print(f"Starting search in: {os.path.abspath(search_directory)}")
    print(f"Filtering for file types: {allowed_extensions}")


    # Walk through the directory tree starting from the search_directory
    for dirpath, dirnames, filenames in os.walk(search_directory):
        # Condition 1: Check if the current directory is named 'runtime'
        if os.path.basename(dirpath).lower() == 'runtime':
            for filename in filenames:
                # Check if the file has one of the allowed extensions
                if filename.lower().endswith(allowed_extensions):
                    full_path = os.path.join(dirpath, filename)
                    found_files.add(full_path)
                    print(f"  - Found file in 'runtime' folder: {full_path}")

        # Condition 2: Check files in the current directory (dirpath)
        for filename in filenames:
            # Check if 'runtime' or 'heap' is in the filename and it has an allowed extension
            if (('runtime' in filename.lower() or 'heap' in filename.lower()) and
                    filename.lower().endswith(allowed_extensions)):
                full_path = os.path.join(dirpath, filename)
                found_files.add(full_path)
                print(f"  - Found file by name: {full_path}")

    # Check if any files were found
    if not found_files:
        print("\nNo matching files were found. 'runtime.txt' was not created.")
        return

    print(f"\nFound {len(found_files)} unique files. Consolidating into '{output_file}'...")

    # Open the output file in write mode ('w') to create it or overwrite if it exists
    try:
        with open(output_file, 'w', encoding='utf-8', errors='ignore') as outfile:
            # Write a header for the consolidated file
            outfile.write("=" * 80 + "\n")
            outfile.write(f"Consolidated Runtime Files (.cpp, .h)\n")
            outfile.write(f"Found {len(found_files)} files.\n")
            outfile.write("=" * 80 + "\n\n")

            # Loop through the unique file paths
            for filepath in sorted(list(found_files)):
                try:
                    # Write a C++ style comment block as a separator with the file's path
                    outfile.write(f"//{'=' * 77}\n")
                    outfile.write(f"// File: {os.path.abspath(filepath)}\n")
                    outfile.write(f"//{'=' * 77}\n\n")

                    # Open and read the content of the found file
                    with open(filepath, 'r', encoding='utf-8', errors='ignore') as infile:
                        content = infile.read()
                        outfile.write(content)
                        # Ensure there's a newline at the end of the content
                        if not content.endswith('\n'):
                            outfile.write('\n')
                        # Add extra newlines for better separation between files
                        outfile.write('\n\n')

                except Exception as e:
                    # Handle cases where a file might be unreadable
                    error_message = f"// --- Could not read file: {os.path.abspath(filepath)} | Error: {e}\n\n"
                    outfile.write(error_message)
                    print(f"  - Warning: {error_message.strip()}")

        print(f"Successfully saved all content to '{os.path.abspath(output_file)}'")

    except IOError as e:
        print(f"Error: Could not write to output file '{output_file}'. Reason: {e}")


if __name__ == "__main__":
    # --- CONFIGURATION ---
    # Set the directory you want to search in.
    # Use "." to search the current directory and all its subdirectories.
    # Or provide a specific path like "C:/Users/YourUser/Documents".
    START_SEARCH_DIR = "."

    # Set the name of the output file.
    OUTPUT_FILENAME = "runtime.txt"

    # --- SCRIPT EXECUTION ---
    find_and_save_runtime_files(START_SEARCH_DIR, OUTPUT_FILENAME)

