import re
import sys

def generate_visitor_methods(header_path):
    """
    Parses an AST.h file to generate C++ visitor method declarations.

    Args:
        header_path: The path to the AST.h file.
    """
    try:
        with open(header_path, 'r') as f:
            content = f.read()
    except FileNotFoundError:
        print(f"Error: File not found at '{header_path}'", file=sys.stderr)
        return

    # Regex to find class names inheriting from Declaration, Expression, or Statement
    # It looks for "class ClassName : public (Declaration|Expression|Statement)"
    pattern = re.compile(r"class\s+(\w+)\s*:\s*public\s+(?:Declaration|Expression|Statement)")
    
    matches = pattern.findall(content)

    if not matches:
        print("// No matching AST node classes found.", file=sys.stderr)
        return

    declarations = []
    expressions = []
    statements = []

    # Categorize each found class
    for class_name in matches:
        # A bit of a hack: determine category by searching for the full class definition
        if f"class {class_name} : public Declaration" in content:
            declarations.append(class_name)
        elif f"class {class_name} : public Expression" in content:
            expressions.append(class_name)
        elif f"class {class_name} : public Statement" in content:
            statements.append(class_name)

    # --- Generate the C++ code ---
    print("// --- Visitor Methods (Auto-generated) ---")
    
    print("\n    // Declarations")
    for class_name in sorted(declarations):
        print(f"    void visit({class_name}& node) override;")

    print("\n    // Expressions")
    for class_name in sorted(expressions):
        print(f"    void visit({class_name}& node) override;")

    print("\n    // Statements")
    for class_name in sorted(statements):
        print(f"    void visit({class_name}& node) override;")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python generate_visitors.py <path_to_AST.h>", file=sys.stderr)
        sys.exit(1)
    
    ast_header_file = sys.argv[1]
    generate_visitor_methods(ast_header_file)

