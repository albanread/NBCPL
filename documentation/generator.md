NewBCPL/generator.md
```

```
# Generator Files Documentation

This document provides an overview of the generator files in the `NewBCPL` project. These files are responsible for various aspects of code generation, including data, expressions, statements, and overall program structure.

---

## 1. `CodeGenerator.cpp`

### Purpose
The `CodeGenerator` class orchestrates the overall code generation process. It integrates helper classes like `ExpressionGenerator` and `StatementGenerator` to handle specific tasks.

### Key Features
- **Initialization**: Sets up helper classes (`ExpressionGenerator`, `StatementGenerator`) with required dependencies.
- **Program Generation**: Processes the program structure and generates assembly code.
- **Section Management**: Handles read-only (`rodata`) and read-write (`data`) sections for literals and variables.

### Methods
- `generate(Program&)`: Main entry point for generating code from a program.
- Visitor methods for declarations, expressions, and statements, delegating tasks to helper classes.

---

## 2. `DataGenerator.cpp`

### Purpose
The `DataGenerator` class manages the generation of data-related sections, including string literals, floating-point literals, and static variables.

### Key Features
- **String Literals**: Registers and generates labels for string literals.
- **Float Literals**: Registers and generates labels for floating-point literals.
- **Static Variables**: Handles static variable declarations and initializers.

### Methods
- `add_string_literal(const std::string&)`: Registers a string literal and returns its label.
- `add_float_literal(double)`: Registers a floating-point literal and returns its label.
- `generate_rodata_section()`: Generates the read-only data section.
- `generate_data_section()`: Generates the read-write data section.

---

## 3. `ExpressionGenerator.cpp`

### Purpose
The `ExpressionGenerator` class is responsible for generating code for expressions. It computes values and stores results in registers.

### Key Features
- **Dynamic Dispatch**: Uses `dynamic_cast` to identify and process different expression types.
- **Immediate Values**: Handles large immediate values using `MOVZ` and `MOVK` instructions.
- **Function Calls**: Generates code for function calls, including argument passing and result handling.

### Methods
- `generate(Expression&)`: Main dispatcher for expression types.
- `generate_number_literal(NumberLiteral&)`: Generates code for numeric literals.
- `generate_string_literal(StringLiteral&)`: Generates code for string literals.
- `generate_binary_op(BinaryOp&)`: Handles binary operations like addition, subtraction, and logical comparisons.

---

## 4. `StatementGenerator.cpp`

### Purpose
The `StatementGenerator` class generates code for statements, including control flow, assignments, and routine calls.

### Key Features
- **Control Flow**: Handles `if`, `while`, `for`, and other control flow statements.
- **Assignments**: Generates code for variable and vector assignments.
- **Routine Calls**: Generates code for calling routines and managing return values.

### Methods
- `generate(Statement&)`: Main dispatcher for statement types.
- `generate_assignment(AssignmentStatement&)`: Handles variable assignments.
- `generate_if(IfStatement&)`: Generates code for `if` statements.
- `generate_while(WhileStatement&)`: Generates code for `while` loops.
- `generate_routine_call(RoutineCallStatement&)`: Generates code for routine calls.

---

## Summary

These generator files collectively form the backbone of the code generation process in the `NewBCPL` project. Each file focuses on a specific aspect of generation, ensuring modularity and maintainability. Together, they enable the translation of high-level constructs into efficient assembly code.