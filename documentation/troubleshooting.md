# Troubleshooting Guide: Using Test Scripts with LLDB

## Purpose

This guide explains how to use the BCPL test scripts in this repository for troubleshooting and debugging with LLDB. These scripts exercise key language features and runtime behavior, making them ideal for validating your JIT-compiled BCPL implementation.

---

## Available Test Scripts

| Script Name             | Focus/Description                                                |
|-------------------------|------------------------------------------------------------------|
| `test.bcl`              | "Hello, World!" output; verifies basic I/O and program start.    |
| `test_strings.bcl`      | Prints multiple strings; checks string handling and memory.       |
| `test_functions.bcl`    | Computes factorial; tests functions, variables, arithmetic.       |
| `test_switchon.bcl`     | `SWITCHON` statement; checks control flow/case dispatch.         |
| `test_brk.bcl`          | Invokes `BRK`; ensures breakpoints trigger as expected.          |
| `test_minimal.bcl`      | Minimal entry point; ensures toolchain handles null program.      |
| `test_repeat_loops.bcl` | Loops (`REPEAT`, `WHILE`, `UNTIL`); tests loop logic.            |
| `test_unless.bcl`       | `UNLESS` statement; conditionals.                                |
| `test_routine.bcl`      | Routine definition/call; tests routine execution.                |
| `test_return_finish.bcl`| `RETURN` and `FINISH` behavior; function/routine exit.           |

_Note: This list covers the first ten test scripts found. [View more test scripts on GitHub.](https://github.com/albanread/NewBCPL/search?q=test)_

---

## How to Use with LLDB

### Prerequisites

- Ensure `NewBCPL` JIT executable, the test `.bcl` file, and `lldb_script.py` are in the same directory.
- (Optional) Create a command file such as `lldb_commands.txt` for automation.

### Automated Debugging Session

1. Create `lldb_commands.txt` with the following (example for `test_strings.bcl`):

    ```
    command script import lldb_script.py
    file ./NewBCPL
    settings set -- target.run-args "test_strings.bcl" "--run"
    run_bcpl_test
    ```

2. Run LLDB with the command file:

    ```
    lldb --source lldb_commands.txt
    ```

3. LLDB will start, break on `WRITES`, and dump CPU/register/stack state, aiding rapid troubleshooting.

### Manual Debugging Session

1. Start LLDB manually:

    ```
    lldb ./NewBCPL
    ```

2. Within LLDB:

    ```
    (lldb) command script import lldb_script.py
    (lldb) settings set -- target.run-args "test_strings.bcl" "--run"
    (lldb) run_bcpl_test
    ```

3. Use breakpoints, step through code, and inspect memory/registers as needed.

---

## Troubleshooting Scenarios

- **Corrupted Output or Segfaults:** Use `test_strings.bcl` to verify correct string literal handling and alignment. If only the first string prints or there’s corruption, check endianness and padding (see Developer Diary for resolution details).
- **Breakpoint Not Triggering:** Use `test_brk.bcl` to confirm the BRK instruction stops execution as expected.
- **Incorrect Loop or Control Flow:** Use `test_repeat_loops.bcl` and `test_switchon.bcl` to step through and verify jump/branch logic.
- **Routine and Function Issues:** `test_functions.bcl`, `test_routine.bcl`, and `test_return_finish.bcl` help validate stack frames, returns, and function/routine boundaries.

_Refer to the [Developer Diary](./DEVELOPER_DIARY.md) for real-world troubleshooting examples and solutions._

---

## Tips

- Always match your test script to the feature you’re investigating.
- Use LLDB’s register and stack dumps to diagnose subtle memory or calling convention bugs.
- Update addresses in `lldb_script.py` if your JIT buffer location changes.
- For new or custom tests, base your script on the provided examples for maximum compatibility.

---
