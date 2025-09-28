import re
import os
import shutil
import glob

def parse_and_split_cpp(cpp_file_path, h_file_path):
    """
    Parses a monolithic C++ implementation file, splits its member functions
    into separate files, and creates a new main implementation file that
    includes the generated files.
    """
    try:
        with open(cpp_file_path, 'r') as f:
            content = f.read()
        with open(h_file_path, 'r') as f:
            header_content = f.read()
    except FileNotFoundError:
        print(f"Error: Could not find file at {cpp_file_path} or {h_file_path}")
        return

    # Create directories for the new files, but do not delete existing content
    generators_dir = "generators"
    helpers_dir = os.path.join(generators_dir, "helpers")
    os.makedirs(helpers_dir, exist_ok=True)

    # A dictionary to store content for each new file
    file_contents = {}

    # Define the file for all non-visitor helpers
    all_helpers_file = os.path.join(helpers_dir, 'gen_all_helpers.cpp')

    # Pre-populate the helpers file with necessary includes to resolve linker errors
    file_contents[all_helpers_file] = f'#include "NewCodeGenerator.h"\n\n'
    file_contents[all_helpers_file] += f'#include <iostream>\n'
    file_contents[all_helpers_file] += f'#include <stdexcept>\n'
    file_contents[all_helpers_file] += f'#include <algorithm>\n'
    file_contents[all_helpers_file] += f'#include <sstream>\n'
    file_contents[all_helpers_file] += f'#include <unordered_set>\n'
    file_contents[all_helpers_file] += f'#include "ClassTable.h"\n'
    file_contents[all_helpers_file] += f'#include "Encoder.h"\n'
    file_contents[all_helpers_file] += f'#include "RegisterManager.h"\n'
    file_contents[all_helpers_file] += f'#include "LabelManager.h"\n'
    file_contents[all_helpers_file] += f'#include "DataGenerator.h"\n'
    file_contents[all_helpers_file] += f'#include "NameMangler.h"\n'
    file_contents[all_helpers_file] += f'#include "CallFrameManager.h"\n'
    file_contents[all_helpers_file] += f'#include "analysis/ASTAnalyzer.h"\n'
    file_contents[all_helpers_file] += f'#include "LivenessAnalysisPass.h"\n'
    file_contents[all_helpers_file] += f'#include "AST.h"\n'
    file_contents[all_helpers_file] += f'#include "ASTVisitor.h"\n'
    file_contents[all_helpers_file] += f'#include "SymbolTable.h"\n'
    file_contents[all_helpers_file] += f'#include "analysis/LiveInterval.h"\n'
    file_contents[all_helpers_file] += f'#include "runtime/ListDataTypes.h"\n'
    file_contents[all_helpers_file] += f'#include "RuntimeManager.h"\n'
    file_contents[all_helpers_file] += f'#include "generators/CodeGenUtils.h"\n\n'

    # Regex to find all NewCodeGenerator member function definitions.
    func_pattern = re.compile(
        r'^(?P<signature>[\w\s\*&:<>\.,]+?)\s+'
        r'NewCodeGenerator::(?P<func_name>[\w~]+)\s*'
        r'\((?P<params>[^)]*)\)\s*'
        r'(?P<const_suffix>\s*const)?\s*'
        r'\{',
        re.DOTALL | re.MULTILINE
    )
    
    # Pre-parse all function signatures and their start/end positions
    function_definitions = []
    matches = list(func_pattern.finditer(content))
    for match in matches:
        start_pos = match.start()
        start_of_body = match.end()

        # Manually scan for the matching closing brace to handle nested braces
        brace_level = 1
        end_of_body = -1
        for j in range(start_of_body, len(content)):
            if content[j] == '{':
                brace_level += 1
            elif content[j] == '}':
                brace_level -= 1
            
            if brace_level == 0:
                end_of_body = j
                break
        
        if end_of_body != -1:
            function_definitions.append({
                'full_text': content[start_pos:end_of_body+1],
                'func_name': match.group('func_name'),
                'start': start_pos,
                'end': end_of_body
            })

    # Separate the header and boilerplate from the first function
    if function_definitions:
        main_file_content = content[:function_definitions[0]['start']].strip() + "\n\n"
    else:
        main_file_content = content.strip() + "\n\n"
        
    # Process each function definition
    for func_def in function_definitions:
        full_function_text = func_def['full_text'].strip() + "\n\n"
        func_name = func_def['func_name']
        
        # Decide the filename based on function name
        if func_name == "visit":
            sig_content = full_function_text.split('{')[0]
            visit_match = re.search(r'visit\(([^&]+?)&', sig_content)
            if visit_match:
                node_type = visit_match.group(1).strip()
                # Special case: skip generic visitor implementations from being redefined
                if node_type in ['Expression', 'ForStatement', 'GlobalVariableDeclaration', 'IfStatement', 'NewExpression', 'SuperMethodAccessExpression', 'SuperMethodCallExpression', 'BinaryOp', 'BitfieldAccessExpression', 'ClassDeclaration', 'ForEachStatement']:
                    filename = f'{generators_dir}/gen_visit_{node_type}.cpp'
                else:
                    filename = all_helpers_file
            else:
                filename = all_helpers_file
        else:
            filename = all_helpers_file

        if filename not in file_contents:
            file_contents[filename] = f'#include "NewCodeGenerator.h"\n\n'

        file_contents[filename] += full_function_text

    # Write the new files
    for filename, content_to_write in file_contents.items():
        with open(filename, 'w') as f:
            f.write(content_to_write)
        
    # Recreate the main file with the includes
    new_main_cpp_content = f'// This file is auto-generated by refactor_codegen_v3.py\n'
    new_main_cpp_content += f'// Do not edit this file directly.\n\n'
    new_main_cpp_content += main_file_content
    
    all_gen_files = sorted(glob.glob(f'{generators_dir}/*.cpp'))
    for filename in all_gen_files:
        new_main_cpp_content += f'#include "{filename}"\n'
    
    with open(cpp_file_path, 'w') as f:
        f.write(new_main_cpp_content)

    print("Refactoring complete.")
    print(f"Main implementation moved to {cpp_file_path}")
    print(f"Functions split into files in {generators_dir}/ and {helpers_dir}/")
    
# Main script execution
if __name__ == "__main__":
    cpp_file = "NewCodeGenerator.cpp"
    h_file = "NewCodeGenerator.h"
    parse_and_split_cpp(cpp_file, h_file)
