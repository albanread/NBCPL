NewBCPL/file_locations.md
# NewBCPL Project File & Folder Locations

This document provides an overview of the major folders and files in the NewBCPL project, highlighting their purpose and typical contents. This is intended to help developers quickly locate relevant code, documentation, and resources.

---

## Top-Level Folders

- **HeapManager/**  
  Implementation of heap management routines. Contains source files for allocation, deallocation, and heap utilities.

- **analysis/**  
  Code for static analysis, symbol discovery, and allocation strategies. Includes subfolders for visitors and implementation details.

- **build/**  
  (Currently empty) Intended for build artifacts or scripts.

- **build_status/**  
  Status tracking files for build steps.

- **docs/**  
  Documentation files for specific modules and features.

- **documentation/**  
  Extensive markdown documentation for core components, compiler flow, developer diary, and troubleshooting.

- **encoders/**  
  Source files for instruction encoding, including ARM64 and optimization routines.

- **format/**  
  Code formatting utilities.

- **generators/**  
  Contains code generation routines for various BCPL statements and expressions.  
  - **helpers/**: Utility code for generators.

- **include/**  
  Header files for signal handling and compiler interfaces.

- **linker_helpers/**  
  Helper routines for linker operations and relocations.

- **optimizer/**  
  Optimization patterns and peephole pattern definitions.  
  - **patterns/**: Specific optimization patterns.

- **passes/**  
  Compiler passes such as manifest resolution and short-circuit logic.

- **results/**  
  Output files from tests and compilation (e.g., `.s` assembly files).

- **runtime/**  
  BCPL runtime implementation, including C/C++ sources, headers, and documentation.  
  - **build/**: Build scripts or artifacts for runtime.

- **scripts/**  
  Utility shell scripts for codebase management.

- **test/**  
  Test sources for specific features and optimizations.

- **tests/**  
  Extensive BCPL test cases, including `.bcl`, `.s`, and `.cpp` files.  
  - **include/**: Test-specific headers.

---

## Notable Top-Level Files

- **AST.cpp / AST.h / ASTVisitor.h**  
  Abstract Syntax Tree implementation and visitor pattern.

- **BasicBlock.cpp / BasicBlock.h**  
  Basic block representation for control flow.

- **CFGBuilderPass.cpp / CFGBuilderPass.h**  
  Control Flow Graph builder pass.

- **Encoder.cpp / Encoder.h / EncoderExtended.h**  
  Instruction encoding logic.

- **JITExecutor.cpp / JITExecutor.h**  
  Just-In-Time execution support.

- **Lexer.cpp / Lexer.h / LexerDebug.h**  
  Lexical analysis routines.

- **Linker.cpp / Linker.h**  
  Linking logic for compiled code.

- **Optimizer.cpp / Optimizer.h**  
  Main optimizer routines.

- **Parser.cpp / Parser.h**  
  Parsing logic for BCPL source.

- **RegisterManager.cpp / RegisterManager.h**  
  Register allocation and management.

- **RuntimeManager.cpp / RuntimeManager.h**  
  Runtime management logic.

- **SymbolTable.cpp / SymbolTable.h**  
  Symbol table implementation.

- **main.cpp**  
  Main entry point for the compiler or runtime.

---

## File Naming Conventions

- Many functions and features are implemented in their own `.cpp` files (e.g., `cf_add.cpp`, `rm_mark_dirty.cpp`), especially for modularity and ease of maintenance.
- Test files are typically named after the feature or optimization being tested.
- Documentation files use `.md` and are grouped by topic.

---

## Guidance for Editing

- **Function Implementation:**  
  If a function is implemented in its own file (e.g., `HeapManager/Heap_allocList.cpp`), edits should be made directly in that file.
- **Modular Structure:**  
  Subfolders (such as `generators/helpers`, `optimizer/patterns`, `analysis/Visitors`) contain specialized code and should be referenced for related changes.

---

## Updating This Document

If you add new folders or files, please update this document to keep it current for all contributors.
