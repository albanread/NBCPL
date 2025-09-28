NewBCPL/book/passes_peephole.md
# Peephole Optimizer: Design and Operation

## Overview

The peephole optimizer in this codebase is a modular, pattern-driven system that scans the instruction stream for short, inefficient sequences and replaces them with more optimal equivalents. It is designed to be extensible, allowing new optimization patterns to be added easily. The optimizer operates after instruction selection and before final code emission, improving code quality by reducing instruction count, eliminating redundancies, and simplifying control flow.

---

## Key Components

### 1. InstructionPattern

Each peephole optimization is encapsulated as an `InstructionPattern` object. This object contains:

- **Pattern Size:** The number of instructions the pattern matches (window size).
- **Matcher Function:** A lambda or function that inspects a window of instructions and returns a `MatchResult` indicating whether the pattern matches and how many instructions are involved.
- **Transformer Function:** A lambda or function that, given a matching window, returns a new vector of optimized instructions to replace the original sequence.
- **Description:** A human-readable string describing the optimization.

### 2. Pattern Registration

Patterns are declared in `optimizer/PeepholePatterns.h` and implemented in `optimizer/patterns/*.cpp` files, grouped by optimization type (e.g., constant folding, control flow, memory optimization).

Patterns are registered in the `PeepholeOptimizer` constructor (see `PeepholeOptimizer.cpp`). This registration determines the order in which patterns are applied.

### 3. Instruction Stream

The optimizer operates on a vector of `Instruction` objects, which represent the program's instructions after selection and before encoding. Each `Instruction` contains opcode, operands, immediates, condition codes, and other metadata.

### 4. Instruction Decoding and Encoding

- **InstructionDecoder:** Utility for extracting opcode, operand types, immediates, and other properties from an `Instruction`.
- **Encoder:** Utility for constructing new `Instruction` objects, especially for fused or architecture-specific instructions.

---

## Optimization Pass Workflow

1. **Initialization:**  
   The `PeepholeOptimizer` is constructed, registering all available patterns.

2. **Pattern Application:**  
   The optimizer iterates over the instruction stream. For each position, it attempts to match each registered pattern in order. If a pattern matches, its transformer is invoked, and the matched instructions are replaced with the optimized sequence. The scan then continues, taking into account the new instruction layout.

3. **Multiple Passes:**  
   The optimizer may run multiple passes or re-scan the stream to catch new opportunities created by previous transformations.

---

## Currently Implemented Patterns

Below is a list of the peephole optimization patterns currently implemented in the codebase, grouped by category:

### Constant Folding and Simplification

- **Constant Folding Pattern**  
  *Placeholder*: Intended to fold arithmetic/logical instructions with constant operands into a single instruction. (Not yet active.)

- **Identity Operation Elimination Pattern**  
  Removes operations like `ADD Xd, Xn, #0` or `SUB Xd, Xn, #0` by replacing them with a simple `MOV` (move) instruction.

- **Redundant Move Pattern**  
  Removes `MOV Xn, Xn` (move from a register to itself).

- **Self Move Elimination Pattern**  
  Alias for the above; removes self-moves for clarity and completeness.

### Address Calculation and Fusion

- **ADR Fusion Pattern**  
  Fuses `ADRP` followed by `ADD` (to the same register and label) into a single `ADR` instruction, reducing instruction count and improving address calculation efficiency.

### Strength Reduction

- **Multiply by Power of Two Pattern**  
  *Placeholder*: Intended to replace multiplication by a power of two with a shift left (`LSL`). (Not yet active.)

- **Divide by Power of Two Pattern**  
  *Placeholder*: Intended to replace division by a power of two with a shift right (`LSR`). (Not yet active.)

- **General Strength Reduction Pattern**  
  *Placeholder*: Reserved for future general strength reduction optimizations.

### Memory Optimization

- **Redundant Load Elimination Pattern**  
  Replaces two consecutive identical loads from the same memory location with a single load followed by a register move.

- **Load-After-Store Elimination Pattern**  
  Removes a load that immediately follows a store to the same address and register, as the value is already known.

- **Dead Store Elimination Pattern**  
  Removes a store that is immediately overwritten by another store to the same address.

- **Redundant Store Elimination Pattern**  
  Removes a store if two consecutive stores to the same address store the same value.

### Control Flow Optimization

- **Compare Zero Branch Pattern**  
  Matches `CMP Xn, #0` followed by a conditional branch. (Currently does not fuse to CBZ/CBNZ, but sets up for this optimization.)

- **Branch Chaining Pattern**  
  Collapses chains of unconditional branches (e.g., `B label1` followed by `label1: B label2`) into a single direct branch.

### Miscellaneous

- **Identical Move Pattern**  
  Removes consecutive identical `MOV` instructions.

---

## Example: Compare Zero Branch Pattern

A typical pattern is `CMP Xn, #0; B.cond label`, which can sometimes be replaced with a single `CBZ` or `CBNZ` instruction on ARM64. The pattern is implemented as follows:

- **Matcher:** Checks for a `CMP` instruction comparing a register to zero, immediately followed by a conditional branch.
- **Transformer:** (To be extended) Replaces the pair with a single `CBZ` or `CBNZ` if the branch condition is EQ or NE.

---

## Extending the Optimizer

To add a new optimization:
1. **Declare** a factory function in `PeepholePatterns.h`.
2. **Implement** the matcher and transformer in a relevant `patterns/*.cpp` file.
3. **Register** the pattern in the optimizer constructor.
4. **Test** the new pattern with relevant instruction sequences.

---

## File Organization

- `optimizer/PeepholePatterns.h` — Pattern declarations.
- `optimizer/patterns/*.cpp` — Pattern implementations, grouped by type.
- `PeepholeOptimizer.h/.cpp` — Core optimizer logic, pattern registration, and application.
- `InstructionDecoder.h/.cpp` — Instruction property extraction.
- `Encoder.h/.cpp` — Instruction construction utilities.

---

## Benefits

- **Modularity:** Easy to add, remove, or modify patterns.
- **Maintainability:** Patterns are self-contained and described.
- **Performance:** Reduces instruction count and improves code quality with minimal overhead.

---

## Summary

The peephole optimizer is a flexible, pattern-based system that improves generated code by recognizing and transforming inefficient instruction sequences. Its design encourages extensibility and clear separation of concerns, making it a powerful tool in the code generation pipeline.

---

### CBZ/CBNZ Fusion Pattern

**CBZ/CBNZ Fusion** is an advanced control flow optimization now implemented in the peephole optimizer for ARM64 targets. This pattern looks for a specific two-instruction sequence:

- A `CMP` instruction that compares a register (`Xn`) to zero. This can be either:
  - `CMP Xn, #0` (immediate zero), or
  - `CMP Xn, XZR` (zero register)
- Immediately followed by a conditional branch instruction:
  - `B.EQ label` (branch if equal/zero)
  - `B.NE label` (branch if not equal/nonzero)

#### What It Does

When this pattern is detected, the optimizer replaces the two-instruction sequence with a single, more efficient instruction:

- If the branch is `B.EQ`, it is replaced with `CBZ Xn, label` (Compare and Branch if Zero)
- If the branch is `B.NE`, it is replaced with `CBNZ Xn, label` (Compare and Branch if Not Zero)

This transformation reduces code size and improves execution speed by eliminating a redundant flag-setting operation and using a dedicated branch instruction supported directly by ARM64 hardware.

#### Example

**Before:**
```
CMP X3, #0
B.EQ label
```
**After:**
```
CBZ X3, label
```

**Before:**
```
CMP X5, XZR
B.NE label
```
**After:**
```
CBNZ X5, label
```

#### Pattern Details

- **Window Size:** 2 instructions
- **Matcher:** Looks for `CMP` against zero (immediate or XZR) followed by `B.EQ` or `B.NE`
- **Transformer:** Emits a single `CBZ` or `CBNZ` with the original register and branch target

This pattern is implemented in the control flow optimization group and is automatically applied during the peephole optimization pass.
