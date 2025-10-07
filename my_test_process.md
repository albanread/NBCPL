NewBCPL/my_test_process.md
# Repeatable Test Process for Encoder Coverage

refer to: 

## Purpose

This document describes a repeatable, step-by-step process for ensuring every encoder function in the codebase is covered by automated validation tests. It is designed for expert engineers to systematically achieve full coverage, especially when dealing with large numbers of encoder implementations.

---

## Step-by-Step Process

### 1. Identify Encoder Functions

- For each file in the `encoders` folder, locate all functions that return an `Instruction` object.
- Exclude branch instructions and linker-dependent encoders (e.g., files containing `branch`, `br`, `adrp`, `return`, `cbnz`, `cbz`, `svc`, etc.), unless explicitly required.

### 2. Check Test Coverage

- For each encoder function found:
  - Check if it is present in the test schedule (i.e., registered in `encoder_test_map` in `EncoderTester.cpp`).
  - Check if it is wrapped by a function in `TestableEncoders.cpp` and called by the test framework.

### 3. Annotate Source File

- At the top of the encoder source file, add a comment indicating its test status:
  - If present and tested:  
    `// This encoder is present in the test schedule and has passed automated validation.`
  - If present but not tested:  
    `// This encoder is present in the test schedule but has NOT passed automated validation.`
  - If not present:  
    `// This encoder is NOT present in the test schedule. Test will be added via wrapper and results updated here.`

### 4. Create Wrapper Function

- If the encoder is not tested:
  - Write a wrapper function in `TestableEncoders.cpp` that calls the encoder with representative arguments and returns its `Instruction`.
  - Example:
    ```cpp
    Instruction test_create_add_reg() {
        return Encoder::create_add_reg("x0", "x1", "x2");
    }
    ```
  - Add declaration to `EncoderTester.h` at the bottom of the file where other wrapper declarations are located:
    ```cpp
    Instruction test_create_add_reg();
    ```

### 5. Add Test Method to EncoderTester Class

- In `EncoderTester.h`, declare a test method:
  ```cpp
  bool test_create_add_reg();
  ```
- In `EncoderTester.cpp`, implement the test method (outside any other function):
  ```cpp
  bool EncoderTester::test_create_add_reg() {
      Instruction instr = ::test_create_add_reg(); // Calls wrapper
      return runValidation("create_add_reg", instr);
  }
  ```
- **CRITICAL**: Use the global scope operator `::` when calling the wrapper function to avoid recursive calls. Without `::`, the method will call itself instead of the global wrapper function, causing compilation errors.

### 6. Register in Test Table

- In `EncoderTester.cpp`, add the test to `encoder_test_map` in `initialize_test_map()`:
  ```cpp
  encoder_test_map["create_add_reg"] = [this]() { return this->test_create_add_reg(); };
  ```

### 7. Check Diagnostics

- **CRITICAL**: Always run diagnostics before building:
  ```bash
  # Check for syntax/semantic errors
  # Fix any errors found before proceeding
  ```
- Fix any errors in the code before proceeding to build.
- **NOTE**: Ignore false positive diagnostics about missing headers like `'Encoder.h' file not found` or `'BitPatcher.h' file not found`. The build.sh script properly sets up include paths, so these diagnostics are misleading. Focus only on actual syntax/semantic errors in your code changes.

### 8. Build Runtime and Main Project

- Build the runtime library first:
  ```bash
  ./buildruntime
  ```
- Then build the main project:
  ```bash
  ./build.sh
  ```
- Check `errors.txt` for any build errors and fix them.
- **Always fix build errors before proceeding to testing.**

### 9. Run the Test

- Run the test suite:
  ```bash
  ./NewBCPL --test-encoders
  ```
- Observe the result for the new test.
- Verify test count increased (should show "Tests run: X+1" where X was the previous count).

### 10. Update Source File Comment

- If the test passes, update the comment at the top of the encoder source file to:
  `// This encoder is present in the test schedule and has passed automated validation.`
- If the test fails, note the failure and begin debugging.

### 11. Repeat

- Move to the next encoder file and repeat steps 1-10.
- Continue until all non-excluded encoder functions are covered.

---

## Notes

- Use representative arguments for wrapper functions to ensure meaningful validation.
- For encoders with multiple modes or overloads, consider adding multiple wrapper tests.
- **ALWAYS** check diagnostics before making changes and before building.
- **ALWAYS** fix any errors found in diagnostics or build before proceeding.
- The test framework automatically runs tests through the test map - do NOT add direct calls in `run_all_tests()`.
- Maintain this process document and update as needed for new encoder types or changes in the test framework.

## Error Handling Protocol

### If Diagnostics Show Errors:
1. Read the error messages carefully
2. Fix the underlying code issues (not just symptoms)
3. Re-run diagnostics to verify fixes
4. Only proceed to build when diagnostics are clean

### If Build Fails:
1. Check `errors.txt` for detailed error messages
2. Fix compilation/linking errors
3. Re-run build
4. Only proceed to testing when build succeeds

### If Tests Fail:
1. Examine the test output for specific failure details
2. Debug the encoder implementation or test setup
3. Re-run tests after fixes
4. Update source file comments only after tests pass

---

## Automation

- This process can be partially automated with scripts to:
  - List all encoder functions returning `Instruction`.
  - Compare to the test schedule.
  - Generate boilerplate wrappers and test table entries.
- Manual review is required for argument selection and exclusion of branch/linker-dependent encoders.

---

## Goal

**Achieve 100% automated test coverage for all encoder functions, with clear documentation and repeatable engineering process.**