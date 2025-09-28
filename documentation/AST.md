NewBCPL/AST.md
```

```
# Abstract Syntax Tree (AST) Documentation

## Overview

The Abstract Syntax Tree (AST) is a core component of the `NewBCPL` project. It represents the hierarchical structure of the source code, enabling semantic analysis, optimization, and code generation. Each node in the AST corresponds to a specific construct in the BCPL language, such as expressions, statements, or declarations.

---

## Key Classes

### 1. `ASTNode`
- **Purpose**: Base class for all AST nodes.
- **Features**:
  - Provides a common interface for all derived nodes.
  - Supports visitor pattern for traversal and manipulation.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the node.

---

### 2. `Program`
- **Purpose**: Represents the entire program.
- **Features**:
  - Contains declarations and statements.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the program.

---

### 3. Declarations
#### `LetDeclaration`
- **Purpose**: Represents variable declarations with optional initializers.
- **Features**:
  - Supports multiple variable declarations in a single statement.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the declaration.

#### `ManifestDeclaration`
- **Purpose**: Represents manifest constants.
- **Features**:
  - Stores a name and value for the constant.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the declaration.

#### `StaticDeclaration`
- **Purpose**: Represents static variables with initializers.
- **Features**:
  - Ensures the initializer is constant.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the declaration.

#### `GlobalDeclaration`
- **Purpose**: Represents global variables.
- **Features**:
  - Stores the name and offset of the variable.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the declaration.

#### `FunctionDeclaration`
- **Purpose**: Represents function definitions.
- **Features**:
  - Contains parameters and a body.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the declaration.

#### `RoutineDeclaration`
- **Purpose**: Represents routine definitions.
- **Features**:
  - Similar to functions but with specific BCPL semantics.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the declaration.

#### `LabelDeclaration`
- **Purpose**: Represents labels for control flow.
- **Features**:
  - Associates a name with a command.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the declaration.

#### `GetDirective`
- **Purpose**: Represents `GET` directives for including files.
- **Features**:
  - Stores the filename to be included.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the directive.

---

### 4. Expressions
#### `NumberLiteral`
- **Purpose**: Represents numeric literals (integer or floating-point).
- **Features**:
  - Differentiates between integer and floating-point types.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the literal.

#### `StringLiteral`
- **Purpose**: Represents string literals.
- **Features**:
  - Stores the string value.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the literal.

#### `CharLiteral`
- **Purpose**: Represents character literals.
- **Features**:
  - Stores the character value.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the literal.

#### `BooleanLiteral`
- **Purpose**: Represents boolean literals (`TRUE` or `FALSE`).
- **Features**:
  - Stores the boolean value.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the literal.

#### `VariableAccess`
- **Purpose**: Represents access to variables.
- **Features**:
  - Stores the variable name.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the access.

#### `BinaryOp`
- **Purpose**: Represents binary operations (e.g., addition, subtraction).
- **Features**:
  - Stores the operator and operands.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the operation.

#### `UnaryOp`
- **Purpose**: Represents unary operations (e.g., negation).
- **Features**:
  - Stores the operator and operand.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the operation.

#### `VectorAccess`
- **Purpose**: Represents access to vector elements.
- **Features**:
  - Stores the vector and index expressions.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the access.

#### `CharIndirection`
- **Purpose**: Represents character indirection (`*(S + E)`).
- **Features**:
  - Stores the string and index expressions.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the indirection.

#### `FloatVectorIndirection`
- **Purpose**: Represents floating-point vector indirection.
- **Features**:
  - Stores the vector and index expressions.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the indirection.

#### `FunctionCall`
- **Purpose**: Represents function calls.
- **Features**:
  - Stores the function expression and arguments.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the call.

#### `ConditionalExpression`
- **Purpose**: Represents conditional expressions (`if-then-else`).
- **Features**:
  - Stores the condition, true expression, and false expression.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the expression.

#### `ValofExpression`
- **Purpose**: Represents `VALOF` expressions.
- **Features**:
  - Stores the body of the expression.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the expression.

---

### 5. Statements
#### `AssignmentStatement`
- **Purpose**: Represents variable assignments.
- **Features**:
  - Supports multiple assignments in a single statement.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the statement.

#### `RoutineCallStatement`
- **Purpose**: Represents routine calls.
- **Features**:
  - Stores the routine expression and arguments.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the statement.

#### `IfStatement`
- **Purpose**: Represents `if` statements.
- **Features**:
  - Stores the condition and then branch.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the statement.

#### `UnlessStatement`
- **Purpose**: Represents `unless` statements.
- **Features**:
  - Stores the condition and then branch.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the statement.

#### `TestStatement`
- **Purpose**: Represents `test` statements.
- **Features**:
  - Stores the condition, then branch, and optional else branch.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the statement.

#### `WhileStatement`
- **Purpose**: Represents `while` loops.
- **Features**:
  - Stores the condition and body.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the statement.

#### `UntilStatement`
- **Purpose**: Represents `until` loops.
- **Features**:
  - Stores the condition and body.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the statement.

#### `RepeatStatement`
- **Purpose**: Represents `repeat` loops.
- **Features**:
  - Stores the loop type, body, and optional condition.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the statement.

#### `ForStatement`
- **Purpose**: Represents `for` loops.
- **Features**:
  - Stores the loop variable, start expression, end expression, body, and optional step expression.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the statement.

#### `SwitchonStatement`
- **Purpose**: Represents `switchon` statements.
- **Features**:
  - Stores the expression, cases, and optional default case.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the statement.

#### `GotoStatement`
- **Purpose**: Represents `goto` statements.
- **Features**:
  - Stores the label expression.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the statement.

#### `ReturnStatement`
- **Purpose**: Represents `return` statements.
- **Features**:
  - Indicates the end of a function or routine.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the statement.

#### `FinishStatement`
- **Purpose**: Represents `finish` statements.
- **Features**:
  - Indicates program termination.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the statement.

#### `BreakStatement`
- **Purpose**: Represents `break` statements.
- **Features**:
  - Exits the current loop.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the statement.

#### `LoopStatement`
- **Purpose**: Represents `loop` statements.
- **Features**:
  - Indicates an infinite loop.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the statement.

#### `EndcaseStatement`
- **Purpose**: Represents `endcase` statements.
- **Features**:
  - Indicates the end of a case block.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the statement.

#### `ResultisStatement`
- **Purpose**: Represents `resultis` statements.
- **Features**:
  - Stores the result expression.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the statement.

#### `CompoundStatement`
- **Purpose**: Represents compound statements (multiple statements grouped together).
- **Features**:
  - Stores a list of statements.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the statement.

#### `BlockStatement`
- **Purpose**: Represents block statements (declarations and statements grouped together).
- **Features**:
  - Stores a list of declarations and statements.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the statement.

#### `StringStatement`
- **Purpose**: Represents string-related statements.
- **Features**:
  - Stores the size expression.
- **Methods**:
  - `accept(ASTVisitor&)`: Accepts a visitor for traversal.
  - `clone()`: Creates a deep copy of the statement.

---

## Visitor Pattern

The AST uses the visitor pattern to enable traversal and manipulation. Each node implements an `accept` method, which calls the corresponding `visit` method on the visitor.

---

## Cloning

All AST nodes support deep cloning via the `clone` method. This ensures that modifications to one AST instance do not affect others.

---

## Summary

The AST provides a flexible and extensible framework for representing BCPL programs. Its modular design allows for efficient traversal, manipulation, and code generation.