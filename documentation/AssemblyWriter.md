NewBCPL/AssemblyWriter.md
```

```
# AssemblyWriter Class Documentation

## Overview

The `AssemblyWriter` class is responsible for generating and writing assembly code to files. It provides functionality to define directives, format instructions, and manage labels in the assembly output.

---

## Purpose

The `AssemblyWriter` class serves as a utility for creating assembly files from a list of instructions and labels. It ensures proper formatting and alignment of assembly code, making it suitable for execution by an assembler.

---

## Key Features

- **Directive Writing**: Adds standard directives like `.section`, `.align`, and `.globl` to the assembly file.
- **Instruction Formatting**: Formats instructions with their address, encoding, and assembly text.
- **Label Management**: Handles defined labels and associates them with specific instruction addresses.

---

## Methods

### `write_directives(std::ofstream& ofs)`
Writes standard directives to the assembly file. These directives include:
- `.section .text`: Specifies the text section for executable code.
- `.align 2`: Aligns instructions to a 4-byte boundary.
- `.globl _main`: Declares `_main` as the global entry point.
- `_main:`: Defines the `_main` label.

### `write_to_file(const std::string& path, const std::vector<Instruction>& instructions, const LabelManager& label_manager)`
Writes the assembly code to a file specified by `path`. This method:
1. Opens the file for writing.
2. Writes directives using `write_directives`.
3. Iterates through the list of instructions and writes their assembly text.
4. Associates labels with their corresponding instruction addresses.

---

## Example Usage

### Writing Assembly Code to a File
```cpp
#include "AssemblyWriter.h"
#include "Instruction.h"
#include "LabelManager.h"

std::vector<Instruction> instructions = {
    Instruction(0x1000, "mov x0, x1"),
    Instruction(0x1004, "add x0, x0, x2"),
    Instruction(0x1008, "ret")
};

LabelManager label_manager;
label_manager.define_label("start", 0x1000);

AssemblyWriter writer;
writer.write_to_file("output.s", instructions, label_manager);
```

### Output (`output.s`)
```
.section .text
.align 2
.globl _main
_main:
start:
    mov x0, x1
    add x0, x0, x2
    ret
```

---

## Dependencies

- **`Instruction` Class**: Represents individual assembly instructions with their address, encoding, and text.
- **`LabelManager` Class**: Manages labels and their associations with instruction addresses.

---

## Notes

- The `AssemblyWriter` class assumes that the input instructions are properly formatted and valid for the target architecture.
- Labels must be defined in the `LabelManager` before calling `write_to_file`.

---

## Summary

The `AssemblyWriter` class is a crucial component for generating assembly files in the `NewBCPL` project. It simplifies the process of formatting and writing assembly code, ensuring compatibility with assemblers and facilitating the execution of generated programs.