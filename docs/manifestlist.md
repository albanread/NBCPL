# MANIFESTLIST Implementation Plan

## Overview

`MANIFESTLIST` is a proposed BCPL language extension that allows the creation of read-only (manifest) lists at compile time. This feature enables the compiler to generate a static, immutable list in the data segment, and to return a direct pointer to it at runtime. This is a significant optimization for constant lists, as it avoids unnecessary heap allocation and copying.

This document summarizes the implementation plan for supporting `MANIFESTLIST` in the compiler, including all affected components and recommended best practices.

---

## 1. Lexical Analysis (Lexer)

**Goal:** Recognize `MANIFESTLIST` as a distinct keyword.

- **Files to Modify:** `Lexer.h`, `Lexer.cpp`, `lex_utils.cpp`
- **Steps:**
  - Add `ManifestList` to the `TokenType` enum in `Lexer.h`.
  - Add `"MANIFESTLIST"` to the keyword map in `Lexer.cpp`.
  - Update `to_string(TokenType)` in `lex_utils.cpp` for debugging and diagnostics.
  - Add lexer tests to confirm correct tokenization.

---

## 2. Syntactic Analysis (Parser)

**Goal:** Parse `MANIFESTLIST(...)` and create a `ListExpression` AST node with a manifest flag.

- **Files to Modify:** `pz_parse_expressions.cpp`
- **Steps:**
  - In `parse_primary_expression`, recognize both `LIST` and `MANIFESTLIST`.
  - Pass a boolean flag (`is_manifest`) to `parse_list_expression`.
  - In `parse_list_expression`, set the `is_manifest` property on the resulting `ListExpression`.
  - Add parser tests for both `LIST(...)` and `MANIFESTLIST(...)`.

---

## 3. AST Node Definition

**Goal:** Store the manifest status in the AST.

- **Files to Modify:** `AST.h`, `AST.cpp`, `AST_Cloner.cpp`
- **Steps:**
  - Add `bool is_manifest = false;` to the `ListExpression` class.
  - Update the constructor and `clone()` method to handle the new flag.
  - Update AST printers/debuggers to display the manifest status.

---

## 4. Code Generation

**Goal:** Generate efficient code for manifest lists.

- **Files to Modify:** `generators/gen_ListExpression.cpp`
- **Steps:**
  - In the `visit(ListExpression& node)` method, check `node.is_manifest`.
    - If `true` (MANIFESTLIST):
      - Get the label for the read-only list from the data generator.
      - Acquire a free register.
      - Emit an `ADRP/ADD` sequence to load the absolute address of the label into the register.
    - If `false` (LIST):
      - Use the existing logic: load the label's address into `X0` and call the `DEEPCOPYLITERALLIST` runtime function.
  - Ensure the data generator emits manifest lists in a read-only section.
  - Add codegen tests to verify correct pointer loading for `MANIFESTLIST`.

---

## 5. Type System and Safety (Optional/Advanced)

**Goal:** Enforce immutability for manifest lists.

- **Files to Modify:** `ASTAnalyzer.cpp`, `NewCodeGenerator.cpp`
- **Steps:**
  - Add new types to `VarType` (e.g., `CONST_POINTER_TO_ANY_LIST`, `CONST_POINTER_TO_INT_LIST`, etc.).
  - In `ASTAnalyzer`, return the const type for manifest lists.
  - In codegen and analyzer, emit errors if a manifest list is assigned to or mutated.
  - Add diagnostics for attempted mutation of manifest lists.

---

## Summary Table

| Stage      | File(s) to Modify                | Task                                             | Complexity |
|------------|----------------------------------|--------------------------------------------------|------------|
| Lexer      | Lexer.h, Lexer.cpp, lex_utils.cpp| Add MANIFESTLIST keyword and token                | Low        |
| Parser     | pz_parse_expressions.cpp         | Parse MANIFESTLIST(...) into AST node             | Low        |
| AST        | AST.h, AST.cpp, AST_Cloner.cpp   | Add is_manifest flag to ListExpression            | Low        |
| Code Gen   | gen_ListExpression.cpp           | Generate pointer load for MANIFESTLIST            | Medium     |
| Safety     | ASTAnalyzer.cpp, NewCodeGenerator.cpp | (Optional) Enforce read-only semantics        | Medium     |

---

## Best Practices & Recommendations

- **Testing:** Add unit and integration tests for lexing, parsing, AST, codegen, and (if implemented) immutability enforcement.
- **Documentation:** Update user documentation and code comments to explain `MANIFESTLIST` semantics and limitations.
- **Error Handling:** Provide clear error messages if a user attempts to mutate a manifest list.
- **Future-Proofing:** Consider extending the type system to support const-typed lists for more granular safety.

---

## Conclusion

This plan provides a clear, incremental path to implementing `MANIFESTLIST` in the compiler. The initial implementation focuses on code generation and AST representation, with optional future work to enforce immutability at the type system level.