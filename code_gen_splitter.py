import os
import re

def split_new_code_generator(cpp_file_path, h_file_path, output_dir="generators"):
    """
    Splits the NewCodeGenerator.cpp file into a core file and individual
    files for each AST visitor method.

    Args:
        cpp_file_path (str): Path to the original NewCodeGenerator.cpp file.
        h_file_path (str): Path to the original NewCodeGenerator.h file.
        output_dir (str): Directory to create for the generated files.
    """
    print(f"Starting refactoring of {cpp_file_path}...")

    # Ensure the output directory exists
    os.makedirs(output_dir, exist_ok=True)
    print(f"Created output directory: {output_dir}")

    # Read the content of the C++ implementation file
    with open(cpp_file_path, 'r') as f:
        content = f.read()

    # Common includes for all generated .cpp files
    common_includes = [
        '#include "NewCodeGenerator.h"',
        '#include "LabelManager.h"',
        '#include "analysis/ASTAnalyzer.h"',
        '#include <iostream>',
        '#include <stdexcept>'
    ]
    includes_str = "\n".join(common_includes) + "\n\n"

    # Define core functions that should remain in the main file
    core_functions = {
        "NewCodeGenerator::NewCodeGenerator",
        "NewCodeGenerator::generate_code",
        "NewCodeGenerator::debug_print",
        "NewCodeGenerator::debug_print_level",
        "NewCodeGenerator::emit", # Overloaded, will catch both
        "NewCodeGenerator::is_local_variable",
        "NewCodeGenerator::get_variable_register",
        "NewCodeGenerator::store_variable_register",
        "NewCodeGenerator::generate_expression_code",
        "NewCodeGenerator::generate_statement_code",
        "NewCodeGenerator::enter_scope",
        "NewCodeGenerator::exit_scope",
        "NewCodeGenerator::process_declarations",
        "NewCodeGenerator::process_declaration",
    }

    # Regex to find function definitions. This is a bit tricky for C++ due to nested braces.
    # We'll use a more robust line-by-line parsing with brace counting.
    lines = content.splitlines()

    # Store the original header includes to keep them in the core file
    original_header_includes = []
    in_header_includes = True
    for line in lines:
        if in_header_includes and line.strip().startswith('#include'):
            original_header_includes.append(line)
        elif in_header_includes and line.strip() == '':
            continue # Allow empty lines in includes block
        else:
            in_header_includes = False
            break # End of initial includes

    functions_to_split = {}
    current_function_name = None
    current_function_lines = []
    brace_count = 0
    in_function = False

    for line_num, line in enumerate(lines):
        # Look for function signatures
        match = re.search(r'(?:void|std::string|bool)\s+NewCodeGenerator::(\w+)(?:\s*\(.*\))?\s*\{', line)
        if match:
            # Found a potential function start. Check for visit methods first.
            visit_match = re.search(r'void\s+NewCodeGenerator::visit\((\w+)& node\)\s*\{', line)
            if visit_match:
                func_name = "visit(" + visit_match.group(1) + "& node)"
                file_base_name = f"gen_{visit_match.group(1)}.cpp"

                # Check for overloaded emit
                if visit_match.group(1) == "emit": # This should not happen for visit methods
                    pass # Handled by core_functions check

                current_function_name = func_name
                in_function = True
                brace_count = line.count('{') - line.count('}')
                current_function_lines = [line]
                continue # Move to next line

            # Check for other core functions
            for core_func_prefix in core_functions:
                if core_func_prefix in line:
                    # This is a core function, keep it for the core file
                    current_function_name = core_func_prefix
                    in_function = True
                    brace_count = line.count('{') - line.count('}')
                    current_function_lines = [line]
                    break
            if in_function:
                continue

        if in_function:
            current_function_lines.append(line)
            brace_count += line.count('{')
            brace_count -= line.count('}')

            if brace_count == 0:
                # End of function
                functions_to_split[current_function_name] = "\n".join(current_function_lines)
                current_function_name = None
                current_function_lines = []
                in_function = False

    # Separate core functions from visit functions
    core_file_content = []
    visit_files_content = {}

    for func_name, func_body in functions_to_split.items():
        is_core = False
        for core_func_prefix in core_functions:
            if core_func_prefix in func_name:
                is_core = True
                break

        if is_core:
            core_file_content.append(func_body)
        elif func_name.startswith("visit("):
            # Extract the AST node name from "visit(NodeName& node)"
            node_name = re.search(r'visit\((\w+)& node\)', func_name).group(1)
            file_base_name = f"gen_{node_name}.cpp"
            visit_files_content[file_base_name] = func_body
        else:
            # Fallback for any other functions not explicitly categorized
            print(f"Warning: Function '{func_name}' not categorized as core or visit. Adding to core file.")
            core_file_content.append(func_body)


    # Write the core file
    with open(os.path.join(output_dir, "NewCodeGenerator_core.cpp"), 'w') as f:
        f.write("\n".join(original_header_includes) + "\n\n") # Keep original includes
        f.write("\n\n".join(core_file_content))
    print(f"Generated {os.path.join(output_dir, 'NewCodeGenerator_core.cpp')}")

    # Write individual visit files
    for filename, func_body in visit_files_content.items():
        with open(os.path.join(output_dir, filename), 'w') as f:
            f.write(includes_str)
            f.write(func_body)
        print(f"Generated {os.path.join(output_dir, filename)}")

    print("Refactoring complete.")
    print("\nNext steps:")
    print("1. Replace the original NewCodeGenerator.cpp with NewCodeGenerator_core.cpp.")
    print("2. Add all generated .cpp files in the 'generators' directory to your build system.")
    print("3. Ensure your build system links all these new object files together.")
    print("4. You might need to adjust the includes in NewCodeGenerator.h if it was relying on specific definitions being in the .cpp file (unlikely for a header).")


# --- Example Usage (assuming files are in the current directory) ---
# You would run this script from the directory containing your NewCodeGenerator.cpp and .h files.
# For demonstration, I'll assume they are in the current working directory.
if __name__ == "__main__":
    # You might need to adjust these paths based on where your files are
    cpp_file = "NewCodeGenerator.cpp"
    h_file = "NewCodeGenerator.h"

    # Create dummy files for testing if they don't exist
    if not os.path.exists(cpp_file):
        print(f"Creating dummy {cpp_file} for demonstration.")
        dummy_cpp_content = """
#include "NewCodeGenerator.h"
#include "LabelManager.h"
#include "analysis/ASTAnalyzer.h"
#include <iostream>
#include <stdexcept>

NewCodeGenerator::NewCodeGenerator(InstructionStream& instruction_stream,
                                 RegisterManager& register_manager,
                                 LabelManager& label_manager,
                                 const ASTAnalyzer& analyzer,
                                 bool debug,
                                 int debug_level)
    : instruction_stream_(instruction_stream),
      register_manager_(register_manager),
      label_manager_(label_manager),
      debug_enabled_(debug),
      debug_level(debug_level),
      analyzer_(analyzer),
      current_frame_manager_(nullptr),
      expression_result_reg_("")
{
    std::cout << "NewCodeGenerator constructor called." << std::endl;
}

void NewCodeGenerator::debug_print(const std::string& message) const {
    if (debug_enabled_) {
        std::cerr << "// [CODEGEN-DEBUG] " << message << std::endl;
    }
}

void NewCodeGenerator::generate_code(Program& program) {
    debug_print("Starting code generation for program.");
    program.accept(*this);
    debug_print("Code generation finished.");
}

void NewCodeGenerator::emit(const Instruction& instr) {
    // Emits a single instruction
}

void NewCodeGenerator::emit(const std::vector<Instruction>& instrs) {
    // Emits multiple instructions
}

void NewCodeGenerator::visit(Program& node) {
    debug_print("Visiting Program node.");
    // Program logic here
    for (const auto& decl : node.declarations) {
        if (decl) {
            process_declaration(*decl);
        }
    }
    for (const auto& stmt : node.statements) {
        if (stmt) {
            generate_statement_code(*stmt);
        }
    }
    debug_print("Finished visiting Program node.");
}

void NewCodeGenerator::visit(LetDeclaration& node) {
    debug_print("Visiting LetDeclaration node.");
    // LetDeclaration logic here
    if (node.initializers[0]) {
        generate_expression_code(*node.initializers[0]);
    }
    std::cout << "LetDeclaration for " << node.names[0] << std::endl;
}

void NewCodeGenerator::visit(NumberLiteral& node) {
    debug_print("Visiting NumberLiteral node.");
    // NumberLiteral logic here
    std::cout << "NumberLiteral: " << node.int_value << std::endl;
}

void NewCodeGenerator::generate_expression_code(Expression& expr) {
    expr.accept(*this);
}

void NewCodeGenerator::generate_statement_code(Statement& stmt) {
    stmt.accept(*this);
}

void NewCodeGenerator::process_declaration(Declaration& decl) {
    decl.accept(*this);
}
        """
        with open(cpp_file, "w") as f:
            f.write(dummy_cpp_content)

    if not os.path.exists(h_file):
        print(f"Creating dummy {h_file} for demonstration.")
        dummy_h_content = """
#ifndef NEW_CODE_GENERATOR_H
#define NEW_CODE_GENERATOR_H

#include "AST.h" // Assuming AST.h defines Program, LetDeclaration, NumberLiteral etc.
#include "Encoder.h"
#include "InstructionStream.h"
#include "CallFrameManager.h"
#include "RegisterManager.h"
#include <vector>
#include <string>
#include <map>
#include <stack>
#include <memory>

class LabelManager;
class ASTAnalyzer;

class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    virtual void visit(Program& node) = 0;
    virtual void visit(LetDeclaration& node) = 0;
    virtual void visit(NumberLiteral& node) = 0;
    // ... other visit methods
};

class NewCodeGenerator : public ASTVisitor {
public:
    NewCodeGenerator(InstructionStream& instruction_stream,
                     RegisterManager& register_manager,
                     LabelManager& label_manager,
                     const ASTAnalyzer& analyzer,
                     bool debug = false,
                     int debug_level = 5);

    void generate_code(Program& program);

    void visit(Program& node) override;
    void visit(LetDeclaration& node) override;
    void visit(NumberLiteral& node) override;

private:
    InstructionStream& instruction_stream_;
    RegisterManager& register_manager_;
    LabelManager& label_manager_;
    bool debug_enabled_;
    int debug_level;
    const ASTAnalyzer& analyzer_;
    std::unique_ptr<CallFrameManager> current_frame_manager_;
    std::map<std::string, int> current_scope_symbols_;
    std::stack<std::map<std::string, int>> scope_stack_;
    std::string expression_result_reg_;

    void debug_print(const std::string& message) const;
    void emit(const Instruction& instr);
    void emit(const std::vector<Instruction>& instrs);
    void generate_expression_code(Expression& expr);
    void generate_statement_code(Statement& stmt);
    void process_declaration(Declaration& decl);
};

#endif // NEW_CODE_GENERATOR_H
        """
        with open(h_file, "w") as f:
            f.write(dummy_h_content)

    split_new_code_generator(cpp_file, h_file)
