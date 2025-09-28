
## 2025-07-25 - Runtime Routine Call Optimization

**Summary:** Implemented an optimization in the JIT linker to replace inefficient `MOVZ/MOVK/BLR` sequences with direct `BL` (Branch with Link) instructions for runtime function calls when the target is within the PC-relative branch range (+/- 128MB). This significantly improves performance and reduces code size for frequently called runtime routines.

**Changes:**
- **`gen_FunctionCall.cpp` / `gen_RoutineCallStatement.cpp`:**
    - Added a `[JIT_CALL]` tag to the `assembly_text` of `BLR` instructions generated for runtime function calls. This tag allows the linker to identify potential optimization targets.
- **`Linker.cpp`:**
    - Enhanced the `Linker::process` method to identify the `MOVZ/MOVK/BLR` sequence associated with the `[JIT_CALL]` tag.
    - Implemented logic to dynamically detect the `MOVZ/MOVK` sequence (which can vary in length depending on the target address).
    - If the target runtime function's address is within the `BL` instruction's +/- 128MB range, the `MOVZ/MOVK/BLR` sequence is replaced with a single `BL` instruction followed by `NOP`s to maintain code alignment and address integrity.
    - Added debug tracing to show when an optimization occurs.

**Verification:**
- Created `test_single_write.bcl` to ensure a cache miss for a runtime function (`WRITES`) and trigger the optimization.
- Verified through trace logs (`--trace` flag) that the `[LINKER-OPTIMIZATION]` message appears, confirming the optimization is applied.
- The compiled and optimized code continues to execute correctly.
