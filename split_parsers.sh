#!/bin/bash
#
# =============================================================================
# C++ Function Extractor (Adapted Script)
#
# Description:
#   Finds all files matching `Parser*.cpp` in the current directory,
#   extracts each top-level function, and saves it into a separate file
#   in the `parsers/` subdirectory.
#
# Instructions:
#   1. Save this entire block of code to a file (e.g., `split_parsers.sh`).
#   2. Make the script executable by running this command in your terminal:
#      chmod +x split_parsers.sh
#   3. Run the script from the same directory as your C++ files:
#      ./split_parsers.sh
#
# Dependencies:
#   Requires 'gawk' (GNU Awk). If you don't have it, install it with Homebrew:
#   brew install gawk
# =============================================================================

# --- Pre-flight Checks ---

# 1. Verify that gawk is installed.
if ! command -v gawk &> /dev/null; then
    echo "Error: gawk is not installed." >&2
    echo "Please install it to continue. On macOS with Homebrew, run:" >&2
    echo "brew install gawk" >&2
    exit 1
fi

# 2. Find target files safely.
# The 'nullglob' option makes the pattern expand to nothing if no files match.
shopt -s nullglob
files=(parse*.cpp) # Changed from Encoder*.cpp to Parser*.cpp
shopt -u nullglob # Turn nullglob off again

# 3. Exit if no matching files were found.
if [ ${#files[@]} -eq 0 ]; then
    echo "No 'parse*.cpp' files found in the current directory. Nothing to do."
    exit 0
fi

# --- Main Execution ---

# Create the output directory if it doesn't already exist.
mkdir -p parsers # Changed from encoders to parsers
echo "Output directory 'parsers/' is ready."
echo "Found ${#files[@]} file(s) to process..."

# Loop through each found file.
for file in "${files[@]}"; do
    echo "----------------------------------------"
    echo "Processing '$file'..."

    # Pipe the file content into the gawk script for processing.
    # Using a here document (<< 'EOF') to pass the gawk script avoids shell
    # interpretation issues with special characters like parentheses.
    gawk -f - "$file" << 'EOF'
        # Helper function to save the captured code block to a file.
        function save_block() {
            # Only save if a valid function name was identified.
            if (func_name != "") {
                # Define the output filename structure.
                # Changed directory from encoders to parsers and prefix from enc_ to parse_
                filename = "parsers/parse_" func_name ".cpp"

                # Write the buffered content to the new file.
                print buffer > filename

                # Print a confirmation message to the user (sent to stderr).
                # Updated message to reflect new directory and prefix
                print "  -> Extracted function: " func_name " to " filename | "cat >&2"
            }
        }

        # BEGIN block: Runs once before processing the file to initialize state.
        BEGIN {
            level = 0       # Tracks the nesting level of curly braces {}.
            buffer = ""     # Stores the lines of the current function.
            func_name = ""  # Stores the identified name of the function.
        }

        # Main block: Runs for every line in the input file.
        {
            # If we are not inside any block and the line is blank, just skip it.
            if (level == 0 && /^[ \t]*$/) {
                next
            }

            # Store the brace level *before* processing the current line.
            prev_level = level

            # Update the brace level by counting `{` and `}` on the current line.
            # gawk's gsub() function returns the number of substitutions made.
            level += gsub(/{/, "{")
            level -= gsub(/}/, "}")

            # Append the current line (with a newline) to our buffer.
            buffer = (buffer ? buffer "\n" : "") $0

            # DETECT START: Check if we just entered a new top-level block.
            if (prev_level == 0 && level > 0) {
                # This is likely a function. We need to parse its name.
                # Create a "clean" version of the buffer to avoid matching
                # patterns inside comments or strings.
                clean_buffer = buffer
                gsub(/\/\*.*\*\//, "", clean_buffer) # Remove multi-line comments
                gsub(/\/\/.*/, "", clean_buffer)    # Remove single-line comments

                # Attempt to match a function signature: "some_name(...)".
                # This regex captures a valid C/C++ identifier (including namespaces)
                # that is followed by an opening parenthesis.
                if (match(clean_buffer, /([a-zA-Z_][a-zA-Z0-9_:]*)[ \t\n]*\(/, m)) {
                    func_name = m[1]
                    # Sanitize name for filename (e.g., MyClass::MyFunc -> MyClass__MyFunc).
                    gsub(/::/, "__", func_name)
                } else {
                    func_name = "" # Reset if no valid name found.
                }
            }

            # DETECT END: Check if we just exited a top-level block.
            if (level == 0 && buffer != "") {
                save_block()
                # Reset state for the next function.
                buffer = ""
                func_name = ""
            }
        }

        # END block: Runs once after the entire file is processed.
        # Ensures the very last function in the file is saved if it wasn't followed by a newline.
        END {
            if (buffer != "") {
                save_block()
            }
        }
EOF
done

echo "----------------------------------------"
echo "Extraction complete."
echo "Check the 'parsers/' directory for results."y

