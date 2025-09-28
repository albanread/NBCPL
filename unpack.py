import os
import re

def extract_files(combined_filepath="combined.txt"):
    """
    Reads a combined text file and extracts the individual source files
    based on '// -- start of file:' and '// -- end of file' markers.

    Args:
        combined_filepath (str): The path to the input text file.
    """
    try:
        with open(combined_filepath, 'r', encoding='utf-8') as f:
            content = f.read()
    except FileNotFoundError:
        print(f"Error: The file '{combined_filepath}' was not found.")
        print("Please ensure the script is in the same directory as the text file.")
        return

    # Regex to find the start marker and capture the file path
    start_marker_regex = r"// -- start of file: (.+)"
    
    # Find all start markers and their positions
    matches = list(re.finditer(start_marker_regex, content))
    
    if not matches:
        print("No file start markers ('// -- start of file:') found. Nothing to extract.")
        return

    extracted_count = 0
    # Iterate through the found markers to extract each file
    for i, current_match in enumerate(matches):
        filepath = current_match.group(1).strip()
        
        # The content of the file starts right after the start marker line
        content_start_pos = current_match.end()
        
        # The content ends at the start of the next file's marker, or the end of the string
        if i + 1 < len(matches):
            next_match_start_pos = matches[i+1].start()
            file_block = content[content_start_pos:next_match_start_pos]
        else:
            file_block = content[content_start_pos:]

        # Find the specific end marker to trim any trailing whitespace or comments
        end_marker_pos = file_block.find("// -- end of file")
        if end_marker_pos != -1:
            file_content = file_block[:end_marker_pos].strip()
        else:
            # If no end marker is found (e.g., last file), just strip the block
            file_content = file_block.strip()
            
        print(f"Extracting: {filepath}")

        try:
            # Ensure the directory exists before writing the file
            directory = os.path.dirname(filepath)
            if directory:
                os.makedirs(directory, exist_ok=True)
            
            # Write the extracted content to its own file
            with open(filepath, 'w', encoding='utf-8') as out_file:
                out_file.write(file_content + '\n') # Add a final newline for consistency
            extracted_count += 1
        except IOError as e:
            print(f"  -> Error writing file {filepath}: {e}")
        except Exception as e:
            print(f"  -> An unexpected error occurred for {filepath}: {e}")

    print(f"\nExtraction complete. {extracted_count} files were created.")

if __name__ == "__main__":
    extract_files()
