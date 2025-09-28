NewBCPL/BasicBlock.md
```

```
# BasicBlock Class Documentation

## Overview

The `BasicBlock` class represents a fundamental unit of code in the control flow graph (CFG). It encapsulates a sequence of statements and maintains relationships with predecessor and successor blocks, enabling structured control flow analysis and optimization.

---

## Class Definition

### Constructor

```cpp
BasicBlock(std::string id, bool is_entry, bool is_exit, std::string label_name);
```

- **Parameters**:
  - `id`: Unique identifier for the block.
  - `is_entry`: Indicates if the block is an entry point.
  - `is_exit`: Indicates if the block is an exit point.
  - `label_name`: Name of the label associated with the block.

---

### Methods

#### `add_statement`

```cpp
void add_statement(StmtPtr stmt);
```

- **Description**: Adds a statement to the block.
- **Parameters**:
  - `stmt`: A pointer to the statement to be added.

---

#### `add_successor`

```cpp
void add_successor(BasicBlock* successor);
```

- **Description**: Adds a successor block to the current block.
- **Parameters**:
  - `successor`: Pointer to the successor block.

---

#### `add_predecessor`

```cpp
void add_predecessor(BasicBlock* predecessor);
```

- **Description**: Adds a predecessor block to the current block.
- **Parameters**:
  - `predecessor`: Pointer to the predecessor block.

---

#### `ends_with_control_flow`

```cpp
bool ends_with_control_flow() const;
```

- **Description**: Checks if the block ends with a control flow statement.
- **Returns**: `true` if the last statement is a control flow statement; otherwise, `false`.

---

## Control Flow Statements

The following statement types are considered control flow statements:
- `GotoStmt`
- `ReturnStmt`
- `FinishStmt`
- `BreakStmt`
- `LoopStmt`
- `EndcaseStmt`

---

## Usage Example

```cpp
BasicBlock block("block1", true, false, "label1");
block.add_statement(std::make_unique<GotoStatement>("label2"));
block.add_successor(&another_block);
block.add_predecessor(&previous_block);

if (block.ends_with_control_flow()) {
    std::cout << "Block ends with a control flow statement." << std::endl;
}
```

---

## Summary

The `BasicBlock` class is a key component in constructing and analyzing control flow graphs. It provides methods to manage statements and relationships between blocks, facilitating optimizations and transformations in the compiler pipeline.