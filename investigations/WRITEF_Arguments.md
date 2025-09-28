# Adding New Argument Types to WRITEF: What Needs to Change?

When extending the NewBCPL language and runtime to support new argument types in the `WRITEF` function (such as PAIR, FPAIR, or other custom types), it is essential to update **three separate locations** in the codebase. Each location is responsible for a different phase of the compilation and execution pipeline:

---

## 1. Semantic Analyzer (Compile-Time Checking)

**Location:**  
`NewBCPL/analysis/az_impl/az_visit_RoutineCallStatement.cpp`  
Function: `ASTAnalyzer::parse_writef_format_string`

**Purpose:**  
- Parses the WRITEF format string at compile time.
- Counts and identifies format specifiers (e.g., `%d`, `%f`, `%P`, etc.).
- Ensures the number and types of arguments match the format string.
- Emits compile-time errors or warnings if there is a mismatch.

**What to Update:**  
- Add your new specifier (e.g., `'P'` for PAIR) to the list of recognized specifiers in `parse_writef_format_string`.
- Update `get_expected_type_for_writef_specifier` to map your new specifier to the correct `VarType`.

**Note:**  
Keep the list of recognized specifiers in sync with the code generator and runtime.

---

## 2. Code Generator (Argument Count & Type Validation)

**Location:**  
`NewBCPL/generators/gen_RoutineCallStatement.cpp`  
Function: `parse_format_string`

**Purpose:**  
- Parses the WRITEF format string during code generation.
- Determines the number of arguments to pass to the runtime.
- Selects the correct WRITEF variant (e.g., `WRITEF`, `WRITEF1`, ..., `WRITEF7`).
- May perform additional validation or code generation logic based on argument types.

**What to Update:**  
- Add your new specifier to the `switch` statement in `parse_format_string`, mapping it to the correct `VarType`.
- Ensure the argument count logic includes your new type.

**Note:**  
Add a comment to keep this list in sync with the analyzer and runtime.

---

## 3. Runtime (Format String Parsing & Argument Validation)

**Location:**  
`NewBCPL/runtime/runtime_core.inc`  
Function: `validate_runtime_writef`

**Purpose:**  
- Parses the WRITEF format string at runtime.
- Counts the number of expected arguments based on the format string.
- Validates that the number of provided arguments matches the number of specifiers.
- Emits a runtime error if there is a mismatch.

**What to Update:**  
- Add your new specifier to the `if` condition that counts valid format specifiers.

**Note:**  
Add a comment to keep this list in sync with the analyzer and code generator.

---

## Summary Table

| Phase              | File/Function                                   | What to Update                |
|--------------------|-------------------------------------------------|-------------------------------|
| Semantic Analysis  | az_visit_RoutineCallStatement.cpp / parse_writef_format_string | Recognize new specifier, map to VarType |
| Code Generation    | gen_RoutineCallStatement.cpp / parse_format_string | Recognize new specifier, map to VarType |
| Runtime            | runtime_core.inc / validate_runtime_writef       | Recognize new specifier       |

---

## Best Practice

**Whenever you add a new WRITEF argument type or format specifier:**
- Update all three locations.
- Add or update comments reminding maintainers to keep these lists in sync.
- Consider adding tests for the new type at both compile-time and runtime.

This ensures consistent behavior and prevents hard-to-debug mismatches between compile-time, code generation, and runtime validation.

---