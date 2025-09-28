# New BCPL compiler

  for a compiler developer.
---

# **ARM64 BCPL Compiler: C ABI Implementation Guide**

This document outlines the core conventions for the ARM64 BCPL compiler, ensuring compliance with the standard C Application Binary Interface (ABI). Adhering to these rules is essential for interoperability with C libraries and the host operating system.

## **Core Principles**

The primary goal is to map BCPL language constructs to the ARM64 architecture in a predictable and compliant manner. This involves a strict adherence to ABI rules for register usage and stack management.

* **Stack Alignment:** The stack pointer (**sp**) must be 16-byte aligned at all times.
* **Interoperability:** Correct ABI implementation allows BCPL code to call C functions and be called by C functions seamlessly.

---

## **Register Allocation Strategy**

Registers are partitioned into distinct roles. A disciplined register management strategy is crucial for efficiency and correctness.

### **Function Arguments & Return Value**

* **Arguments (x0-x7):** The first eight word-sized arguments are passed in registers **x0** through **x7**.
* **Return Value (x0):** The function result, set by RESULTIS, is returned in **x0**. **x0** should not be used for temporary expression evaluation if it holds a required argument.

### **General-Purpose & Scratch Registers**

* **Instruction Generation Scratch (x9):** This register is reserved exclusively for the compiler's internal use during instruction generation. It should not be used for user variable allocation.
* **Expression Scratch (x10-x15):** These are caller-saved registers available for temporary values during expression evaluation. Their contents must be considered destroyed after any function call.
* **Linker/OS Reserved (x16, x17):** These are scratch registers (IP0, IP1) but are often used by the linker. Use them with caution.
* **Platform Reserved (x18):** This register is reserved for platform-specific OS use and must not be used by the compiler.

### **Variable Storage**

* **Callee-Saved Registers (x19-x27):** These registers are designated for holding local variables. If a function uses any of these registers, it **must** save their original values to the stack in its prologue and restore them in its epilogue. The caller can trust that their values are preserved across function calls.

---

## **Stack Frame Management**

A proper C ABI-compliant stack frame is mandatory for every function. The stack grows downwards.

### **Function Prologue**

1. **Allocate Frame:** Decrement the stack pointer (**sp**) to reserve space for the entire stack frame. This includes space for the frame pointer/link register pair, saved callee-saved registers, and all local variables (including backed-up arguments).
2. **Save Frame/Link Registers:** Store the frame pointer (**x29**) and the link register (**x30**) as a pair to the newly allocated stack frame.
3. **Establish New Frame Pointer:** Set the new frame pointer: mov x29, sp.
4. **Save Callee-Saved Registers:** If the function uses any registers from the **x19-x27** range, save them to the stack.
5. **Backup Arguments:** Copy any necessary arguments from registers **x0-x7** to their designated local variable slots on the stack. This frees up the argument registers and allows arguments to be accessed if their address is taken.

### **Local Variables**

* **Hidden \_result:** Each function should have a hidden \_result variable allocated in its stack frame. The RESULTIS keyword assigns a value to this memory location.
* **Spilling:** The register manager should keep locals in registers (**x19-x27**) when possible. Variables are only "spilled" (saved) to their stack slots when a register is needed for another purpose.

### **Function Calls**

Before calling another function (a "callee"):

1. **Save Caller-Saved Registers:** Any live values in the scratch registers (**x0-x18**) must be saved to the caller's stack frame.
2. **Restore:** After the function returns, these saved values must be restored.

### **Function Epilogue**

1. **Set Return Value:** Load the value from the hidden \_result variable into **x0**.
2. **Restore Callee-Saved Registers:** Restore registers **x19-x27** from the stack if they were saved.
3. **Restore Frame/Link Registers:** Restore **x29** and **x30** from the stack.
4. **Deallocate Frame:** Increment the stack pointer (**sp**) to its original position before the function was called.
5. **Return:** Execute the ret instruction.

---

## **BCPL to C ABI Considerations**

* **Word Size:** The BCPL "word" is treated as a 64-bit value to match the ARM64 architecture's native register and pointer size.
* **Typeless Nature:** Since BCPL is typeless, all arguments and return values are handled as 64-bit words. It is the programmer's responsibility to ensure that the data passed to and from C functions is interpreted correctly.


---

# ** Unified Register Manager Design**

This document outlines the design requirements and public API for the unified register manager. This component is responsible for all general-purpose register allocations, serving as the single source of truth for the code generator.

## **1\. Core Design Requirements**

The register manager centralizes the complex logic of register allocation, freeing the code generator to focus on instruction selection. It must be designed with the following principles in mind.

### **State Tracking**

The manager must maintain the real-time state of every register it controls. For each register, it must track:

* **Availability:** Whether the register is free or in-use.
* **Binding:** If in-use, which variable or temporary expression it is currently bound to.
* **Dirty Status:** A "dirty" flag indicating if the register's value has been modified since it was loaded from memory. This is crucial for optimizing spills, as a clean register doesn't need to be written back to the stack.

### **ABI Partitioning**

The manager must have baked-in knowledge of the ARM64 C ABI and partition the register file accordingly:

* **Variable Registers (Callee-Saved):** x19-x27. The manager's primary pool for long-lived variables. It knows these must be saved/restored in the function prologue/epilogue if used.
* **Expression Scratch (Caller-Saved):** x10-x15. The primary pool for temporary values needed for intermediate calculations. The manager knows these are volatile and do not survive function calls.
* **Compiler-Reserved Scratch:** x9. Reserved for the code generator's internal operations and managed like any other scratch register.
* **Ignored Registers:** The manager will not allocate ABI-specialized registers like x29 (FP) or x30 (LR).

### **Lifetime Management & Spilling**

The manager must differentiate between requests for short-lived scratch registers and long-lived variable registers.

* It must implement a **spilling strategy** for when a request is made but no registers in the desired pool are free. A good heuristic is to spill the register whose next use is furthest in the future.
* It is solely responsible for generating the necessary STR (spill) and LDR (fill) instructions to move values between registers and their home slots on the stack.

---

## **2\. API Exposed to the Code Generator**

The code generator interacts with the register manager through a clean, well-defined API. The manager encapsulates all the complex state and spilling logic.

### **Variable Management**

These functions manage registers for variables that have a home on the stack.
acquire\_reg\_for\_variable(variable\_name)

* Gets a register to hold a variable.
* **Action:**
  1. Checks if the variable is already in a register. If so, returns that register.
  2. If not, it allocates a free callee-saved register (x19-x27).
  3. If no callee-saved register is free, it spills one and re-assigns it.
  4. Generates code to load the variable's value from its stack slot into the allocated register.
* **Returns:** The register (x19-x27) now holding the variable's value.

release\_reg\_for\_variable(variable\_name)

* Releases a variable's register, writing back if necessary.
* **Action:**
  1. Finds the register associated with variable\_name.
  2. If the register is marked as "dirty," generates code to store its value back to the variable's stack slot.
  3. Marks the register as free.

### **Expression & Scratch Management**

These functions manage short-lived registers for calculations.
acquire\_scratch\_reg()

* Gets a temporary register for an expression.
* **Action:**
  1. Allocates a free caller-saved register (x9-x15).
  2. If none are free, it will spill a variable from a callee-saved register to free it up for temporary use (a less common, higher-cost scenario).
* **Returns:** A free scratch register (x9-x15).

release\_scratch\_reg(register)

* Returns a scratch register to the free pool.
* **Action:** Marks the specified register as free. No spilling logic is involved as scratch values are temporary by nature.

### **Function Call Management**

These functions are critical for maintaining state across function calls.
spill\_all\_caller\_saved()

* Saves the state of all active scratch registers before a function call.
* **Action:**
  1. Iterates through all in-use caller-saved registers (x9-x15).
  2. For each one, generates code to spill its value to a reserved temporary area on the stack.
* **Returns:** A list or context object representing the registers that were saved, to be passed to restore\_caller\_saved.

restore\_caller\_saved(saved\_list)

* Restores the state of scratch registers after a function call returns.
* **Action:** For each register in the saved\_list, generates code to load its value from the temporary stack area back into the original register.

---

## **3\. Example Workflow**

Consider the BCPL statement: LET a \= b \+ 5

1. **Get b:** Code generator calls reg \= acquire\_reg\_for\_variable("b"). The manager returns x19 and loads b's value into it.
2. **Get scratch for 5:** Code generator calls scratch\_reg \= acquire\_scratch\_reg(). The manager returns x10.
3. **Instruction Selection:** The code generator now has its registers. It emits MOV x10, \#5.
4. **Perform addition:** It emits ADD x19, x19, x10. The result for a is now in x19.
5. **Release scratch:** It calls release\_scratch\_reg(x10). The manager marks x10 as free.
6. **Update a:** x19 now holds the new value for a. The code generator tells the manager to re-assign the register: remap\_reg\_to\_variable(x19, "a") and mark\_dirty(x19).
7. **Finalize:** Later, when a is no longer needed or at the function's end, release\_reg\_for\_variable("a") is called, and the manager, seeing x19 is dirty, saves its contents to a's stack slot.


CallFrameManager design document.
---

# **README.md: CallFrameManager Design**

This document outlines the design, requirements, and interfaces for the CallFrameManager. This component is responsible for all logic and state associated with the creation and management of a function's stack frame, ensuring C ABI compliance.

## **Core Requirements**

1. **ABI Compliance:** The manager must strictly adhere to the ARM64 C ABI, particularly concerning 16-byte stack alignment and the layout for saving the frame pointer (x29) and link register (x30).
2. **Encapsulation:** It must encapsulate all details of the stack layout. The Code Generator should not perform any manual offset calculations.
3. **Dynamic Sizing:** The manager must dynamically calculate the total size of the stack frame as local variables and register save requirements are identified.
4. **Code Generation:** It must be capable of generating the complete, final instruction sequences for a function's prologue and epilogue.

---

## **Internal State**

An instance of CallFrameManager is created for each function being compiled and tracks the following state:

* **variable\_offsets**: A map or dictionary that stores the calculated byte offset for each named local variable (e.g., {"my\_var": \-8, "\_result": \-16}). Offsets are negative relative to the frame pointer (x29).
* **current\_size**: An integer tracking the total size of the stack frame in bytes.
* **callee\_saved\_registers\_to\_save**: A list of the callee-saved registers (x19-x27) that are used by the function and must be saved in the prologue.
* **is\_prologue\_generated**: A boolean flag to prevent the prologue from being generated more than once.

---

## **Special Handling for the \_result Variable 📝**

To support BCPL's RESULTIS keyword, the CallFrameManager provides special handling for a hidden \_result variable.

* **Implicit Declaration:** When a new CallFrameManager is initialized for a function, it automatically calls its own add\_local("\_result") method. This ensures that every function frame has a reserved stack slot for the return value, regardless of whether RESULTIS is used.
* **Code Generator Interaction:** When the Code Generator encounters a RESULTIS expression, it treats \_result as a normal variable. It acquires a register for the expression's value and then generates a STR instruction to store that register's content to the stack location provided by frame\_manager.get\_offset("\_result").
* **Epilogue Integration:** The generate\_epilogue method has a specific, hardcoded step. Just before restoring the stack and returning, it generates an instruction to load the value from the \_result stack slot directly into the return value register (x0). For example: LDR x0, \[x29, \#offset\_of\_result\]. This guarantees the correct value is in x0 upon function exit.

---

## **Public API**

The CallFrameManager exposes a clean interface to the Code Generator.
add\_local(variable\_name, size\_in\_bytes=8)

* Informs the manager of a new local variable that needs space on the stack.
* **Action:** It calculates a new offset for the variable, updates the variable\_offsets map, and increases current\_size. It ensures the stack remains 16-byte aligned.

get\_offset(variable\_name)

* Retrieves the stack offset for a previously declared local variable.
* **Action:** Looks up the variable in the variable\_offsets map.
* **Returns:** The integer byte offset relative to the frame pointer (x29).

generate\_prologue()

* Constructs the complete assembly instruction sequence for the function prologue.
* **Action:**
  1. It queries the **Register Manager** to get the list of all dirty callee-saved registers.
  2. It calculates the final, aligned total\_size of the frame.
  3. It generates the instructions to:
     * Decrement the stack pointer: sub sp, sp, \#total\_size.
     * Save the frame and link register pair: stp x29, x30, \[sp, \#offset\].
     * Establish the new frame pointer: mov x29, sp.
     * Save all necessary callee-saved registers to the stack.
* **Returns:** A string or list of strings containing the prologue assembly code.

generate\_epilogue()

* Constructs the complete assembly instruction sequence for the function epilogue.
* **Action:** Based on the state saved during the prologue generation, it generates instructions to:
  * **Load the result into x0 from the \_result stack slot.**
  * Restore all callee-saved registers from the stack.
  * Restore the original frame and link registers.
  * Deallocate the stack frame: add sp, sp, \#total\_size.
  * Return: ret.
* **Returns:** A string or list of strings containing the epilogue assembly code.

---

## **Interaction with the Register Manager**

The CallFrameManager and **Register Manager** must collaborate closely, particularly when generating the prologue.
The workflow is as follows:

1. During code generation for a function's body, the **Code Generator** acquires registers from the **Register Manager**. If it uses a callee-saved register like x19 for a variable, the **Register Manager** marks x19 as in-use.
2. When the **Code Generator** is ready to create the prologue, it calls frame\_manager.generate\_prologue().
3. Inside generate\_prologue(), the CallFrameManager **must query the Register Manager** for a list of all in-use callee-saved registers.
4. The CallFrameManager uses this list to allocate the correct amount of space on the stack and to generate the specific stp instructions to save only the registers that are actually used, making the code more efficient.

o provide visibility into the compiler's operation, the CallFrameManager should offer a method to display its planned layout for debugging purposes.

### Debugging API Method
display_frame_layout()
Generates a human-readable string representing the current state of the stack frame. The Code Generator can call this at any point (e.g., after all locals are declared) and print the output.
Action: It iterates through its internal state (variable_offsets, callee_saved_registers_to_save, etc.) and formats it into a clear, textual diagram.
Returns: A formatted string showing the stack layout.
Example Output Format
A call to display_frame_layout() for a function with two local variables (a, b) that uses registers x19 and x20 would produce output similar to this:

--- Call Frame Layout (Total Size: 48 bytes) ---
  Offset | Content
---------------------------------------------------
  +16    | Argument 9 (from caller's frame)
  +8     | (Caller's SP)
  ±0     | Old Frame Pointer (x29)      <-- FP (x29) points here
  -8     | Old Link Register (x30)
  -16    | Saved x19
  -24    | Saved x20
  -32    | Local: "_result"
  -40    | Local: "a"
  -48    | Local: "b"                   <-- SP points here
---------------------------------------------------
This visualization provides an immediate, clear picture of what is stored in the frame and at what offset, which is invaluable for debugging the Code Generator and Register Manager.

# **BCPL Compiler Optimizer Documentation**

## **Overview**

The BCPL optimiser is a crucial stage in the compilation pipeline that operates on the Abstract Syntax Tree (AST). Its primary purpose is to transform the initial AST generated by the parser into a new, functionally equivalent AST that represents more efficient code.
This process involves a series of optimisation techniques that reduce the number of calculations performed at runtime, leading to faster execution and potentially smaller code size. The optimiser is implemented as a recursive AST-to-AST transformer using the visitor pattern.
---

## **Core Components**

The optimisation process is divided into two main components that work together:

* **Optimizer Class**: This is the main orchestrator. It traverses the entire AST and applies general optimisations like constant folding, strength reduction, and dead code elimination. When it encounters a FOR loop, it delegates the specialized task of loop optimisation to the LoopOptimizer.
* **LoopOptimizer Module**: This is a dedicated helper module responsible for performing Loop-Invariant Code Motion (LICM). It analyzes the body of a FOR loop to find expressions that can be safely moved outside the loop, preventing redundant calculations.

---

## **Supported Optimisations**

The optimiser currently performs the following key transformations:

### **1\. Constant Folding**

This technique evaluates expressions whose operands are all known constants at compile time.

* **Description**: Instead of generating code to perform a calculation at runtime, the compiler computes the result and embeds it directly into the code. This applies to arithmetic (+, \*), comparison (\<, \>), and logical operators.
* **Example**:
  * **Before**: LET X \= 10 \+ (20 \* 2\)
  * **After**: The expression is evaluated, and the AST is transformed as if the code were LET X \= 50\.

### **2\. Algebraic Identity Simplification**

This simplifies mathematical expressions by applying algebraic rules.

* **Description**: Expressions involving identity values (like adding 0 or multiplying by 1\) are reduced to their simplest form, removing the unnecessary operation entirely.
* **Example**:
  * **Before**: WRITEN(Z \+ 0\)
  * **After**: The \+ 0 operation is eliminated, and the AST is transformed to be equivalent to WRITEN(Z).

### **3\. Strength Reduction**

This optimisation replaces computationally expensive instructions with equivalent, but cheaper (faster) ones.

* **Description**: It identifies specific operations that can be performed more efficiently.
* **Example**:
  * **Before**: WRITEN(X \* 2\)
  * **After**: Multiplication by two is replaced with a single bitwise left shift (X \<\< 1), which is much faster on most processors. The AST is transformed accordingly.

### **4\. Dead Code Elimination**

This removes code that can never be executed or has no effect on the program's output.

* **Description**: The optimiser evaluates constant conditional statements (IF, TEST). If the condition is provably true or false at compile time, the unreachable branch is completely removed from the AST.
* **Example**:
  * **Before**: IF 10 \> 20 THEN WRITES("This will never print")
  * **After**: The optimiser evaluates 10 \> 20 to false and removes the entire IF statement from the code. No assembly is generated for it.

### **5\. Loop-Invariant Code Motion (LICM)**

This is a powerful loop optimisation that moves calculations from inside a loop to outside it if their results do not change between iterations.

* **Description**: The LoopOptimizer analyzes a FOR loop to find expressions that only depend on variables that are not modified within the loop. It "hoists" these calculations, computing them once before the loop starts and storing the result in a temporary variable.
* **Example**:
  * **Before**:
    FOR I \= 1 TO 100 DO $(
        C := C \+ (A \* B)
    $)

  * **After (Conceptual)**:
    LET \_licm\_temp\_0 \= A \* B
    FOR I \= 1 TO 100 DO $(
        C := C \+ \_licm\_temp\_0
    $)


# **BCPL Character and String Implementation**

## **1\. Overview**

This document specifies the representation and handling of characters and strings for the modern BCPL JIT compiler. The design is driven by two primary goals:

1. **Unicode Support:** To handle modern text, characters must support the full Unicode character set.
2. **C ABI Compatibility:** To interface seamlessly with the host operating system's C standard library, the string format must be directly compatible.

## **2\. Character Model**

### **2.1. Representation**

A BCPL character is a **32-bit unsigned integer** representing a Unicode code point. This allows for the representation of all characters in the Basic Multilingual Plane (BMP) and supplementary planes.

### **2.2. Character Constants**

A character constant, written as 'c', is parsed by the compiler and represented as its corresponding 32-bit integer value in the generated machine code. For example, 'A' becomes the integer value 65\.

## **3\. String Model**

### **3.1. Representation**

A BCPL string is a **pointer to a sequence of one or more 32-bit characters, terminated by a 32-bit zero value (NULL terminator)**.
This zero-terminated format is chosen for direct compatibility with C library functions (e.g., printf, fopen), which eliminates the need for costly conversions at the boundary between BCPL code and system libraries.

### **3.2. String Literals**

When the JIT compiler encounters a string literal like "ABC", it will create a corresponding constant in a read-only data segment of the program. The memory layout will be as follows:

| Address Offset | Hex Value (32-bit) | Character |
| :---- | :---- | :---- |
| \+0 | 0x00000041 | 'A' |
| \+4 | 0x00000042 | 'B' |
| \+8 | 0x00000043 | 'C' |
| \+12 | 0x00000000 | NULL Terminator |

The value of the string expression in the code will be the starting address of this data.

## **4\. Character Indirection Operator: %**

To provide ergonomic access to 32-bit characters within a string, a new dyadic operator, %, is introduced.

* **Syntax:** S % E
* **Description:** Yields the 32-bit character at index E of the string pointed to by S.
* **Precedence:** It has the same left-to-right precedence as the vector subscripting operator (\!).

### **4.1. JIT Implementation**

The JIT compiler must translate the % operator into a scaled memory-load instruction. Given a string pointer S in a base register (e.g., x1) and an integer index E in another register (e.g., x2), the AArch64 implementation is:
; BCPL: LET CHAR \= S % E
; x1 contains the address of the string S
; x2 contains the index E

; Load the 32-bit word (W-register) from the address \[base \+ index\*4\]
; LSL \#2 performs a logical shift left by 2, which is equivalent to multiplying by 4\.
LDR W0, \[X1, X2, LSL \#2\]

; W0 now holds the character value.

This single instruction efficiently performs the required address calculation and memory access.

## 5. The STRING Statement (Non-Standard Extension)

### 5.1. Overview

The `STRING` statement is a non-standard extension to BCPL, introduced in this JIT compiler. It is functionally equivalent to the standard `VEC` statement, but is intended specifically for Unicode string/vector allocation.

### 5.2. Syntax

    STRING n

Where `n` is the number of 32-bit elements to allocate.

### 5.3. Semantics

- `STRING n` allocates a vector of `n` 32-bit values, initialized to zero, and returns a pointer to the first element.
- The allocated vector is intended to be used as a mutable Unicode string, where each element is a 32-bit Unicode code point.
- The memory layout and access semantics are identical to those of `VEC`, but the use of `STRING` makes the intent explicit in source code.

### 5.4. Example

    LET S = STRING 100
    S % 0 := 'A'
    S % 1 := 'B'
    S % 2 := 0x1F600  // Unicode 😀
    S % 3 := 0

This allocates a string buffer for up to 100 Unicode code points, and stores 'A', 'B', and 😀 at the start, followed by a null terminator.

### 5.5. Implementation Notes

- The compiler translates `STRING n` exactly as it would `VEC n`, allocating a zero-initialized vector of 32-bit words.
- The distinction is purely for code clarity and intent; there is no runtime difference.

# **BCPL Floating-Point Model**

## **1\. Overview**

This document specifies the model for handling 64-bit floating-point numbers in the modern BCPL JIT compiler. The design adheres to the typeless nature of BCPL by introducing a new set of operators for floating-point arithmetic, rather than adding a distinct floating-point type.

## **2\. Representation**

A floating-point number is stored in a standard 64-bit machine word and conforms to the **IEEE 754 double-precision** binary format. The BCPL runtime and JIT-compiled code do not distinguish between integer and floating-point values at the memory level; the distinction is made only by the operators used in expressions.

## **3\. Floating-Point Literals**

The syntax is extended to support floating-point literals. A number literal containing a decimal point (.) or the letter e (for scientific notation) will be parsed as a 64-bit floating-point value.

* **Examples:** 3.14159, 0.0, 1e-5, \-123.456e+10

The JIT compiler will store the IEEE 754 representation of these literals in a read-only data segment.

## **4\. Floating-Point Operators**

A new set of "dotted" operators is introduced for floating-point arithmetic. These operators instruct the JIT to use floating-point hardware instructions.

| Operator | Description | AArch64 Instruction |
| :---- | :---- | :---- |
| \+. | Floating-Point Addition | FADD |
| \-. | Floating-Point Subtraction | FSUB |
| \*. | Floating-Point Multiplication | FMUL |
| /. | Floating-Point Division | FDIV |
| .% | Floating-Point Vector Indirection | LDR (scaled) |

These operators have the same precedence as their integer counterparts.

### **4.1. Floating-Point Relational Operators**

A corresponding set of dotted relational operators is also introduced. These operators perform a floating-point comparison.

* \=., \~=., \<., \<=., \>., \>=.

### **4.2. Floating-Point Vector Indirection (.%)**

To support efficient access to arrays of floating-point numbers (stored in standard BCPL vectors), the .% operator is introduced.

* **Syntax:** V .% E
* **Description:** Yields the 64-bit floating-point value at index E of the vector pointed to by V.
* **JIT Implementation:** The JIT compiler must translate this into a scaled memory-load instruction that fetches a 64-bit double-precision value. Given a vector pointer V in a base register (e.g., x1) and an integer index E in another register (e.g., x2), the AArch64 implementation is:
  ; BCPL: LET F \= V .% E
  ; x1 contains the address of the vector V
  ; x2 contains the index E

  ; Load the 64-bit double-precision float (D-register) from the address
  ; \[base \+ index\*8\]. LSL \#3 performs a logical shift left by 3, which
  ; is equivalent to multiplying by 8\.
  LDR D0, \[X1, X2, LSL \#3\]

  ; D0 now holds the floating-point value.

## **5\. Conversion Functions**

To explicitly convert between integer and floating-point interpretations of data, two essential library functions will be provided by the runtime.

### **FLOAT(n)**

* **Description:** Converts a 64-bit integer n into its closest 64-bit floating-point representation.
* **JIT Implementation:** This will be compiled to a single AArch64 instruction: SCVTF D0, X0 (Signed integer Convert to Floating-point).

### **TRUNC(f)**

* **Description:** Converts a 64-bit floating-point number f into a 64-bit integer by truncating towards zero.
* **JIT Implementation:** This will be compiled to a single AArch64 instruction: FCVTZS X0, D0 (Floating-point Convert to Signed integer, rounding toward Zero).

These intrinsic functions provide a clear and efficient mechanism for type interpretation without breaking the fundamental typelessness of the language.

# **BCPL on ARM64 Execution Model Summary for Compiler Writers (32-bit Characters)**

This document outlines the execution model for a BCPL compiler targeting ARM64 on macOS, specifically for a 64-bit Cintcode implementation where individual characters (and "byte" operations) are treated as **32-bit words**. It is intended to guide a code generator by detailing data representation, register usage, memory access, and control flow mechanisms, aligning with the AArch64 Procedure Call Standard (AAPCS64).

### **1\. Data Representation**

* **Word Size:** All fundamental data types—integers, pointers, and floating-point numbers—are **64-bit**.
* **Characters/Words (for "byte" operations):** When operations akin to PBYT are performed, they operate on **32-bit words**. A "character" in this context is a 32-bit value.
* **Integers:** Represented as 64-bit signed integers.
* **Pointers:** 64-bit memory addresses.
* **Floating-Point Numbers:** 64-bit IEEE 754 double-precision format.

### **2\. Register Usage and Mapping**

The Cintcode virtual machine registers represented a BCPL virtual machine

| Cintcode Register | ARM64 Register | Description | Volatility (AAPCS64) |
| :---- | :---- | :---- | :---- |
| A (Accumulator) | x0 | Primary accumulator, first function argument, function return value (int/ptr). | Caller-saved |
| B (Auxiliary) | x1 | Second accumulator, second function argument. | Caller-saved |
| C (Char/Work) | x2 | Third function argument. Holds 32-bit words for PBYT/XPBYT operations. | Caller-saved |
| P (Stack Frame) | x29 (FP) | Frame Pointer: Base of the current stack frame. | Callee-saved |
| G (Global) | x28 | Global Pointer: Base of the Global Vector. | Callee-saved |
| ST (Status) | x27 | System/debug status register (Cintpos context). | Callee-saved |
| PC (Program Counter) | x30 (LR) | Link Register: Holds return address. Implicitly managed by BL/RET. | Callee-saved |
| Count | x26 | Debugging register for instruction counts. | Callee-saved |
| MW (Memory Word) | x25 | Used for extending 32-bit w operands to 64-bit. | Callee-saved |

**Floating-Point Register Usage:**

* For floating-point operations, the A and B values will typically be moved to d0 and d1 (double-precision) respectively.
* d0 is used for floating-point return values.
* d0 \- d7 are caller-saved and used for passing floating-point arguments.
* d8 \- d15 are callee-saved.
* d16 \- d31 are caller-saved.

**General-Purpose Register Usage (x3 \- x7):** These registers are available for passing additional arguments in function calls, following AAPCS64. They are caller-saved. Temporary values can also be stored in other caller-saved registers (x9 \- x15) as needed.

### **3\. Memory Model and Addressing**

* **Endianness:** macOS ARM64 uses **little-endian** byte ordering. Code generation must account for this.
* **Memory Access:**
  * All *word-sized* memory accesses (loads/stores of 64-bit values) should be 8-byte aligned for optimal performance.
  * LDR (Load Register) and STR (Store Register) instructions are used for 64-bit word operations.
  * Offsets for local (P\!n) and global (G\!n) variables are expressed as byte offsets relative to x29 (FP) and x28 (G), respectively. A word offset k translates to a byte offset k \* 8\.
  * Indirect memory access (e.g., A\!n) uses the value in x0 as the base address, with n\*8 as the byte offset.
* **32-bit Word Operations (GBYT, PBYT, XPBYT):**
  * These instructions now operate on 32-bit quantities.
  * LDR Wt, \[Xn, Xm\] (Load 32-bit word from Xn \+ Xm) is used for GBYT. Wt will be the 32-bit part of x0.
  * STR Wt, \[Xn, Xm\] (Store 32-bit word from Wt to Xn \+ Xm) is used for PBYT and XPBYT. Wt will be the 32-bit part of x2 (C register).
  * Addresses for these operations are byte-level addresses. ARM64 handles unaligned 32-bit loads/stores, but performance is best with 4-byte alignment.
* **Stack:** The stack grows downwards. Stack frames are managed via x29 (FP) and sp (Stack Pointer).

### **4\. Instruction Translation Principles**

* **Immediate Values:**
  * Small immediates (up to 4095 for arithmetic/logical, or specific ranges for MOVZ/MOVN) can be encoded directly.
  * Larger 64-bit immediates will require MOVZ/MOVK pairs or loading from a literal pool (ADRP/ADD/LDR).
  * The MW (x25) register is used to extend 32-bit w operands (e.g., in LPW, AW) to their full 64-bit value by adding its contents to the sign-extended 32-bit w. MW must then be reset to zero.
* **Arithmetic and Logical Operations:**
  * Direct mapping to ARM64 instructions (e.g., ADD, SUB, MUL, SDIV, UDIV, LSL, LSR, AND, ORR, EOR, NEG, MVN).
  * DIV/MOD require careful handling for division by zero (raise exception 5).
* **Comparisons and Conditional Jumps:**
  * Comparisons (CMP) set condition flags.
  * Conditional branches (B.EQ, B.NE, B.LT, B.GT, B.LE, B.GE) are used based on the desired relational operator.
  * FHOP usually translates to a CMP followed by a conditional move (CSET) or a specific sequence to set x0 to 0 (false) or \-1 (true).
* **Bitfield Operations (SELLD, SELST):** Translated to ARM64 bitfield instructions like UBFX (unsigned bitfield extract), SBFX (signed bitfield extract), BFI (bitfield insert). SELST operations require careful extraction, modification, and re-insertion of the bitfield.

### **5\. Function/Routine Calling Convention (AAPCS64)**

* **Arguments:**
  * First eight integer/pointer arguments are passed in x0 through x7.
  * First eight floating-point arguments are passed in d0 through d7.
  * Additional arguments (beyond 8\) are passed on the stack, pushed in reverse order.
* **Return Values:**
  * 64-bit integer/pointer results returned in x0.
  * 64-bit floating-point results returned in d0.
* **Stack Frame Management (Kn, K b/h/w, RTN):**
  * **Function Prologue:**
    1. Push FP (x29) and LR (x30) onto the stack (paired store STP FP, LR, \[SP, \#-16\]\!).
    2. Move SP to FP (MOV FP, SP).
    3. Allocate space for local variables and spill registers by decrementing SP (SUB SP, SP, \#frame\_size).
    4. Save any callee-saved registers (x19-x28, d8-d15) if they are used by the function.
  * **Function Epilogue:**
    1. Restore callee-saved registers.
    2. Deallocate stack space (MOV SP, FP).
    3. Restore FP and LR (LDP FP, LR, \[SP\], \#16).
    4. Return to caller (RET).
* **Function Pointers:** Functions can be called via address in a register (BLR \<reg\>).

### **6\. Special Considerations**

* **MDIV (Multi-Divide):** This complex instruction, involving 128-bit products, should be delegated to an external C helper function, or implemented as a sequence of ARM64 instructions that handle 64x64-bit multiplication and division to produce 128-bit results and remainders.
* **SYS (System Call):** Directs control to an underlying C dosys function which handles specific system functionalities. Compiler should generate a call to this C function, passing parameters in x0, x1 etc.
* **CHGCO (Change Coroutine):** This instruction performs a context switch between coroutines. This is highly architecture-specific and typically implemented as a C function that saves and restores necessary registers (including FP, LR, and all callee-saved registers) for the active and target coroutines.
* **Debugging (BRK):** Maps to BRK \#0x1 for debugger interaction.
* **Compiler-Generated Code:** The code generator must manage jump targets and literal pool generation for addresses and large immediates, especially for instructions like LL, LF, SWL, and SWB.


# **Notes on a Modernised BCPL Runtime for Unix**

## **1\. Introduction and Philosophy**

This document outlines a design for a modern BCPL runtime system, specifically targeting Unix-like operating systems (Linux, macOS, BSD). The primary goals are:

1. **Unix Philosophy:** Embrace standard Unix mechanisms for memory management, I/O, and program loading rather than recreating low-level facilities.
2. **JIT-Oriented:** Design the runtime to support a Just-In-Time (JIT) compiler that translates BCPL source directly into executable machine code in memory.
3. **Safety and Portability:** Leverage the host OS to provide a more stable and secure environment than the original, bare-metal-adjacent runtimes.
4. **Simplicity:** Retain the spirit of BCPL by keeping the runtime layer thin and understandable.

The runtime itself would be an executable (e.g., bcpl\_jit) that hosts the compiler and manages the execution environment for the compiled code.

## **2\. Memory Model**

The original BCPL memory model consisted of the Global Vector, the Stack, and dynamically allocated vectors. We can modernize these concepts to map cleanly onto a modern OS.

### **2.1. The Stack**

In a JIT model, the native machine stack can and should be used directly. This is the most efficient approach.

* **Function Calls:** A BCPL function or routine call (F(A, B)) would be compiled into a standard native CALL instruction.
* **Stack Frames:** The compiler would generate conventional function prologues and epilogues. A typical stack frame for a BCPL function would contain:
  * The return address (pushed by the CALL instruction).
  * The saved frame pointer of the caller.
  * Space for its local variables (LET declarations).
  * Arguments passed to any functions it calls.
* **Arguments:** Arguments are passed by value, as per the BCPL specification. The JIT would place arguments in registers or on the stack according to the native C ABI (Application Binary Interface) of the target architecture (e.g., System V ABI for x86-64). This makes calling external C functions seamless.
* **Recursion:** Using the native stack automatically provides support for recursion. Stack overflow becomes a standard OS-level exception.

### **2.2. Dynamic Memory (LET V \= VEC K)**

The original runtime managed a heap for vector allocation. This can be directly replaced by the standard C library allocator, which is highly optimized and integrated with OS virtual memory.

* **Allocation:** A VEC declaration would be JIT-compiled into a call to the C malloc function. The runtime would link against the C standard library.
  // BCPL: LET MyVec \= VEC 100
  // JIT compiles to code equivalent to:
  void\* MyVec \= malloc( (100 \+ 1\) \* sizeof(word) );

* **Deallocation:** BCPL has no free command; memory is reclaimed when a block is exited. The runtime must track VEC allocations made within a block. When the block exits, the JIT-compiled epilogue would call free on each vector allocated within that block's scope.

### **2.3. Modernising the Global Vector**

The Global Vector was BCPL's primary mechanism for linking separately compiled modules. It was a shared, raw block of memory where modules could access data and functions via known integer offsets. This concept is obsolete in a modern OS with dynamic linking.

* **Proposed Replacement:** A hash table or a similar map structure within the runtime can serve as a **Symbol Table** for the JIT.
* **GLOBAL Declaration:** GLOBAL $( F:100, V:101 $) would not allocate from a vector. Instead, it would register the names F and V in the runtime's symbol table. These names are marked as "external".
* **JIT Linking:** When the JIT compiles a function or static variable, it places the name and its in-memory machine-code address into the symbol table. When it encounters a call to an external name, it records a placeholder.
* **Final Linking Step:** After all code is JIT-compiled, the runtime performs a "linking" pass. It iterates through all placeholder calls and patches them with the final addresses looked up from the symbol table. This mimics what a traditional linker does and is extremely fast.

This approach provides the same functionality as the Global Vector but with the benefits of symbolic naming and without the rigidity of fixed offsets.

## **3\. Program Execution and the JIT Process**

The bcpl\_jit runtime would orchestrate the entire lifecycle of a program run.

1. **Invocation:**
   ./bcpl\_jit my\_program.b

2. **Bootstrap:** The runtime starts up, initializes its internal state (like the symbol table), and opens my\_program.b.
3. **Memory Allocation:** The runtime allocates a region of memory using mmap on Unix. This memory must be marked as writable and readable (PROT\_WRITE | PROT\_READ).
4. **JIT Compilation:** The runtime's built-in BCPL compiler reads the source file, parses it, and emits native machine code directly into the mmap-ed region. As it compiles functions and routines, it populates the symbol table with their names and addresses.
5. **Memory Protection:** After compilation is complete, the runtime uses mprotect to change the memory region's permissions to readable and executable (PROT\_READ | PROT\_EXEC). This is a critical security step (W^X).
6. **Finding the Entry Point:** The runtime looks up the START symbol in its table to find the address of the main routine.
7. **Execution:** The runtime casts the address of the START routine to a C function pointer and calls it, passing the program's command-line arguments in a format the BCPL program can understand (e.g., as a vector of strings).
8. **Termination:** When the START routine returns or calls FINISH, the runtime calls exit() with the appropriate status code.

## **4\. Modernising the Standard Library**

The BCPL library functions can be implemented as thin wrappers over the host's C standard library. The JIT must know the C ABI to call them correctly.

| BCPL Function | Modern Unix/C Implementation | Notes |
| :---- | :---- | :---- |
| findinput(S) | fopen(S, "r") | Returns a FILE\* pointer, which is just a word. |
| findoutput(S) | fopen(S, "w") | Returns a FILE\* pointer. |
| selectinput(F) | Set a global "current input" to F | The runtime maintains FILE\* current\_in. |
| rdch() | fgetc(current\_in) | Maps directly. ENDSTREAMCH becomes EOF. |
| wrch(CH) | fputc(CH, current\_out) | Maps directly. |
| endread() | fclose(current\_in) | Maps directly. |
| stop(N) / finish | exit(N) | The standard process termination call. |
| time() | clock() or clock\_gettime() | Provides process execution time. |
| readn(), writen(N) | Implemented in the runtime using fscanf/fprintf. |  |

## **5\. Error Handling**

* **Compile-Time Errors:** Handled by the JIT compiler and reported to the user before execution begins.
* **Runtime Errors:** The original ABORT routine was called on faults like illegal memory access. In a modern system, this is handled by the OS.
  * The runtime should install **signal handlers** for SIGSEGV (segmentation fault), SIGFPE (floating-point/integer error), etc.
  * When a signal is caught, the handler can provide a rich **stack trace**. The JIT would need to have generated a map of instruction address ranges to BCPL function names, allowing the signal handler to print a meaningful, symbolic backtrace rather than raw hex addresses.

# **BCPL Language Syntax Reference**

This document provides a comprehensive overview of the BCPL (Basic Combined Programming Language) syntax, drawing from the 1969 TX-2 BCPL Reference Manual and the 1974 BCPL Programming Manual by M. Richards.

## **1\. Lexical Elements**

### **1.1. Identifiers**

An identifier is a sequence of letters and digits, starting with a letter. In some implementations, the underscore \_ is also permitted.

### **1.2. Numbers**

BCPL supports several number formats:

* **Decimal:** A sequence of decimal digits (e.g., 123).
* **Octal:** The symbol \# followed by a sequence of octal digits (e.g., \#777).
* **Hexadecimal:** The characters \#X followed by a sequence of hexadecimal digits (e.g., \#X1A).

### **1.3. String Constants**

A string constant is a sequence of up to 255 characters enclosed in double quotes ("). Special characters can be represented using an escape sequence starting with \*:

* \*n or \*N: Newline
* \*t or \*T: Tab
* \*s or \*S: Space
* \*b or \*B: Backspace
* \*p or \*P: Newpage
* \*c or \*C: Carriage Return
* \*": Double quote
* \*\*: Asterisk

### **1.4. Character Constants**

A character constant is a single character enclosed in single quotes ('). The same escape sequences as string constants apply.

### **1.5. Truth Values**

* TRUE: Represents the integer value 1 (or all ones in some implementations).
* FALSE: Represents the integer value 0\.

### **1.6. Comments**

* //: A single-line comment, extending to the end of the line.
* /\* ... \*/: A multi-line comment.

## **2\. Expressions**

### **2.1. Operator Precedence**

| Precedence | Operator(s) | Associativity | Description |
| :---- | :---- | :---- | :---- |
| Highest | () | \- | Function Call |
|  | \!, OF | Left-to-right | Subscripting, Field |
|  | @, \! | Right-to-left | Address-of, Indirection |
|  | \*, /, REM | Left-to-right | Multiplication, Division |
|  | \+, \- | Left-to-right | Addition, Subtraction |
|  | \<\<, \>\> | Left-to-right | Shift Operators |
|  | \=, \~=, \<, \<=, \>, \>= | Left-to-right | Relational Operators |
|  | & | Left-to-right | Logical AND |
|  | \` | , NEQV, EQV\` | Left-to-right |
|  | \-\> | Right-to-left | Conditional Expression |
|  | TABLE | \- | Table Constructor |
| Lowest | VALOF | \- | Value-of Block |

### **2.2. Addressing Operators**

* **@E (Address of):** Returns the memory address (Lvalue) of E.
* **\!E (Indirection):** Returns the contents (Rvalue) of the memory location pointed to by E.
* **V\!E (Vector Subscript):** Accesses the E-th element of vector V. V\!E is equivalent to \!(V+E).

### **2.3. Arithmetic Operators**

* \+ (Addition), \- (Subtraction)
* \* (Multiplication), / (Division)
* REM (Remainder)

### **2.4. Relational Operators**

* \= (Equal), \~= (Not Equal)
* \< (Less than), \<= (Less than or equal)
* \> (Greater than), \>= (Greater than or equal)

### **2.5. Logical Operators**

* \~E (Logical NOT)
* E1 & E2 (Logical AND)
* E1 | E2 (Logical OR)
* E1 EQV E2 (Equivalence)
* E1 NEQV E2 (Not Equivalent, XOR)

### **2.6. Shift Operators**

* E1 \<\< E2 (Left shift)
* E1 \>\> E2 (Right shift)

### **2.7. Conditional Expression**

E1 \-\> E2, E3
If E1 is TRUE, the result is E2; otherwise, the result is E3.

### **2.8. VALOF Expression**

VALOF C
Executes the command C until a RESULTIS E command is encountered. The value of the VALOF expression is the value of E.

## **3\. Commands**

### **3.1. Assignment**

\<LHS\> := E
Assigns the value of expression E to the left-hand side LHS. The LHS can be a variable, a vector element (V\!i), or an indirection (\!addr).
Multiple assignments are also possible:
L1, L2 := E1, E2

### **3.2. Routine Call**

R(E1, E2, ...)
Calls the routine R with the given expressions as arguments.

### **3.3. Conditional Commands**

* IF E THEN C
* UNLESS E THEN C
* TEST E THEN C1 OR C2

### **3.4. Repetitive Commands**

* WHILE E DO C
* UNTIL E DO C
* C REPEAT
* C REPEATWHILE E
* C REPEATUNTIL E
* FOR N \= E1 TO E2 BY K DO C

### **3.5. SWITCHON Command**

SWITCHON E INTO $(
  CASE K1: C1
  CASE K2: C2
  ...
  DEFAULT: CD
$)

Transfers control to the command C associated with the case constant K that matches the value of E. If no case matches, control goes to the DEFAULT command.

### **3.6. Control Transfer**

* GOTO E: Unconditional jump to a label.
* RETURN: Returns from a routine.
* FINISH: Terminates the program.
* BREAK: Exits the innermost loop.
* LOOP: Restarts the innermost loop.
* ENDCASE: Exits a SWITCHON block.
* RESULTIS E: Returns a value from a VALOF block.

## **4\. Declarations**

### **4.1. LET**

LET N1, N2, ... \= E1, E2, ...
Declares one or more dynamic variables and initializes them.

### **4.2. MANIFEST**

MANIFEST $( N1 \= K1; N2 \= K2 ... $)
Declares compile-time constants.

### **4.3. STATIC**

STATIC $( N1 \= K1; N2 \= K2 ... $)
Declares static variables with initial values.

### **4.4. GLOBAL**

GLOBAL $( N1: K1; N2: K2 ... $)
Declares global variables at specific offsets in the global vector.

### **4.5. VEC**

LET V \= VEC K
Declares a dynamic vector V with K+1 elements (indices 0 to K).

### **4.6. Function and Routine Declarations**

* **Function:** LET F(P1, P2) \= E
* **Routine:** LET R(P1, P2) BE C

### **4.7. AND**

LET F1() \= E1 AND F2() \= E2
Allows for simultaneous declarations, typically used for mutually recursive functions.

### **4.8. Label**

L: C
Declares a label L for the command C.

## **5\. Program Structure**

### **5.1. Section Brackets ($( and $))**

Used to group declarations and commands into blocks. They can be tagged for matching, e.g., $(LOOP ... $)LOOP.
In some early implementations, such as for the TX-2 computer, curly braces ({ and }) were used as direct hardware representations for $( and $) respectively.

### **5.2. Compound Command**

A sequence of commands enclosed in section brackets:
$( C1; C2; ... $)

### **5.3. Block**

A sequence of declarations followed by commands, enclosed in section brackets:
$( D1; D2; ...; C1; C2; ... $)

### **5.4. GET Directive**

GET "filename"
Includes the content of another source file.


# Adding New Optimization Passes

The BCPL optimizer has been refactored to use a modular pass-based architecture. This document explains how to add new optimization passes to the system.

## Architecture Overview

The optimizer uses the following components:

- **OptimizationPass**: Base interface that all optimization passes must implement
- **PassManager**: Coordinates and sequences multiple optimization passes
- **Optimizer**: Main interface that uses PassManager to apply optimizations

## Current Passes

The system currently includes these optimization passes:

1. **ConstantFoldingPass**: Performs constant folding and algebraic simplifications
   - Evaluates constant expressions at compile time (e.g., `2 + 3` → `5`)
   - Applies algebraic identities (e.g., `x * 1` → `x`, `x + 0` → `x`)
   - Optimizes conditional expressions with constant conditions
   - Performs strength reduction (e.g., `x * 2` → `x << 1`)

2. **LoopInvariantCodeMotionPass**: Moves loop-invariant code outside loops
   - Identifies expressions that don't depend on loop variables
   - Hoists invariant computations out of loop bodies
   - Reduces redundant computation in loops

## Adding a New Pass

To add a new optimization pass, follow these steps:

### 1. Create the Pass Header File

Create a new header file (e.g., `MyOptimizationPass.h`):

```cpp
#ifndef MY_OPTIMIZATION_PASS_H
#define MY_OPTIMIZATION_PASS_H

#include "OptimizationPass.h"
#include "AST.h"

class MyOptimizationPass : public OptimizationPass {
public:
    ProgramPtr apply(ProgramPtr program) override;
    std::string getName() const override;

private:
    // Add any helper methods or visitor pattern methods here
    // Follow the pattern used in ConstantFoldingPass.h
};

#endif // MY_OPTIMIZATION_PASS_H
```

### 2. Implement the Pass

Create the implementation file (e.g., `MyOptimizationPass.cpp`):

```cpp
#include "MyOptimizationPass.h"

ProgramPtr MyOptimizationPass::apply(ProgramPtr program) {
    // Implement your optimization logic here
    // You can use the visitor pattern like other passes
    return program; // Return the optimized program
}

std::string MyOptimizationPass::getName() const {
    return "My Optimization Pass";
}
```

### 3. Update CMakeLists.txt

Add your new source file to the executable in `CMakeLists.txt`:

```cmake
add_executable(compiler
    # ... existing files ...
    MyOptimizationPass.cpp
    # ... rest of files ...
)
```

### 4. Register the Pass

Add your pass to the default pass pipeline in `Optimizer.cpp`:

```cpp
#include "MyOptimizationPass.h"

void Optimizer::setupDefaultPasses() {
    // Register the default optimization passes
    passManager.addPass(std::make_unique<ConstantFoldingPass>(manifests));
    passManager.addPass(std::make_unique<LoopInvariantCodeMotionPass>(manifests));
    passManager.addPass(std::make_unique<MyOptimizationPass>()); // Add your pass
}
```

### 5. Build and Test

```bash
make clean && make compiler
./compiler your_test_file.b
```

## Pass Design Guidelines

### Visitor Pattern

Most passes use the visitor pattern to traverse and transform the AST. Follow this structure:

```cpp
class MyPass : public OptimizationPass {
private:
    // Visitor dispatchers
    ExprPtr visit(Expression* node);
    StmtPtr visit(Statement* node);
    DeclPtr visit(Declaration* node);

    // Specific node visitors
    ExprPtr visit(BinaryOp* node);
    StmtPtr visit(ForStatement* node);
    // ... etc for each AST node type
};
```

### State Management

If your pass needs to maintain state:

```cpp
class MyPass : public OptimizationPass {
private:
    // Pass-specific state
    std::unordered_map<std::string, Value> symbolTable;
    bool inLoopContext = false;

    // Reset state for each program
    void resetState();
};
```

### Manifest Constants

Access manifest constants through the manifests parameter:

```cpp
class MyPass : public OptimizationPass {
public:
    MyPass(std::unordered_map<std::string, int64_t>& manifests)
        : manifests(manifests) {}

private:
    std::unordered_map<std::string, int64_t>& manifests;
};
```

## Best Practices

1. **Keep passes focused**: Each pass should have a single optimization goal
2. **Preserve semantics**: Never change the meaning of the program
3. **Handle all node types**: Ensure your visitor methods handle all AST node types
4. **Test thoroughly**: Verify your optimizations don't break existing functionality
5. **Document behavior**: Clearly document what optimizations your pass performs

## Example: Dead Code Elimination Pass

Here's a skeleton for a dead code elimination pass:

```cpp
class DeadCodeEliminationPass : public OptimizationPass {
public:
    ProgramPtr apply(ProgramPtr program) override {
        return visit(program.get());
    }

    std::string getName() const override {
        return "Dead Code Elimination Pass";
    }

private:
    std::set<std::string> usedVariables;

    // First pass: collect used variables
    void collectUsedVariables(Program* program);

    // Second pass: remove unused declarations
    ProgramPtr visit(Program* node);
    DeclPtr visit(LetDeclaration* node);
    // ... other visitors
};
```

This modular architecture makes it easy to add new optimizations while keeping the codebase maintainable and the optimization pipeline flexible.

# BCPL Compiler Test Process with Clang


This work is hard, it is not easy or trivial like writing flappy birds or a saas web page
This is tough rewarding work, bringing back 1960s tech with a modern edge.

To achieve this you will be careful and cautious and restrained.
You will conserve your resources, and approach one issue at a time.


## 1. Build the Compiler
Run the build script to compile the BCPL compiler and all necessary components:
```sh
./build.sh
```

## 2. Generate Test Assembly Files
Use the provided script to generate ARM64 assembly files from the BCPL test sources:
```sh
./gen_all_asm.sh
```
This will process all test `.bcpl` files and output corresponding `.asm` files in the `results/` directory.

## 3. Assemble with Clang
For each generated `.asm` file, use clang to assemble it and check for errors:

ONE AT A TIME you cant look at all of them

```sh
clang -c results/test_repeat1.asm -o results/test_repeat1.o
```
Repeat for all relevant `.asm` files.

## 4. Investigate and Fix Faults
FIX ONE ISSUE AT A TIME BE CAREFUL a confirmed fix is a FEAT, git commit a confirmed fix!!!

- If clang reports errors (e.g., unknown instructions, fixup kind errors, or syntax issues), note the error message and the line number.
- Investigate the code generator or relevant part of the compiler that emits the problematic assembly.
- Update the code generator to emit assembly that is compatible with clang (e.g., use PC-relative loads for string literals instead of unsupported pseudo-instructions).
- Rebuild the compiler and regenerate the assembly files.
- Re-run the clang assembly step to verify the fix.

## 5. Iterate
Repeat the process for each test case and each error until all test assembly files assemble cleanly with clang.

---

This process ensures your compiler’s output is compatible with clang and helps you systematically identify and fix code generation issues.


# BCPL Instructions Implementation for AArch64

This document describes the implementation of BCPL instructions for the AArch64 architecture. Each instruction is implemented to maintain BCPL's semantics while utilizing AArch64's capabilities efficiently.

## Register Conventions

- `X0`: A register (accumulator, first argument, return value)
- `X1`: B register (second argument)
- `X2`: C register (third argument)
- `X25`: MW register
- `X28`: Global pointer (G)
- `X29`: Frame pointer (FP)
- `X30`: Link register (LR)
- `SP/XZR`: Stack pointer/Zero register (31)

## Basic Load Instructions

### Immediate Loading
- `Ln` (0 ≤ n ≤ 10): `B := A; A := n`
- `LM1`: `B := A; A := -1`
- `L b` (0 ≤ b ≤ 255): `B := A; A := b`
- `LM b` (0 ≤ b ≤ 255): `B := A; A := -b`
- `LH h` (0 ≤ h ≤ 65535): `B := A; A := h`
- `LMH h` (0 ≤ h ≤ 65535): `B := A; A := -h`
- `LW w`: `B := A; A := w` (32-bit value)
- `MW w`: `MW := w` (loads to X25)

### Stack and Global Access
- `LPn` (3 ≤ n ≤ 16): `B := A; A := P!n`
- `LP b`: `B := A; A := P!b`
- `LPH h`: `B := A; A := P!h`
- `LPW w`: `B := A; A := P!w`
- `LG b`: `B := A; A := G!b`
- `LG1 b`: `B := A; A := G!(b + 256)`
- `LGH h`: `B := A; A := G!h`

## Indirect Load Instructions

- `GBYT`: `A := B%A` (byte load)
- `RV`: `A := A!0`
- `RVn` (1 ≤ n ≤ 6): `A := A!n`
- `RVPn` (3 ≤ n ≤ 7): `A := P!n!A`
- `L0Pn` (3 ≤ n ≤ 12): `B := A; A := P!n!0`
- `L1Pn` (3 ≤ n ≤ 6): `B := A; A := P!n!1`

## Expression Operators

### Arithmetic
- `NEG`: `A := -A`
- `NOT`: `A := ~A`
- `MUL`: `A := B * A`
- `DIV`: `A := B / A`
- `MOD`: `A := B MOD A`
- `ADD`: `A := B + A`
- `SUB`: `A := B - A`

### Bitwise
- `LSH`: `A := B << A`
- `RSH`: `A := B >> A`
- `AND`: `A := B & A`
- `OR`: `A := B | A`
- `XOR`: `A := B XOR A`

### Immediate Arithmetic
- `An` (1 ≤ n ≤ 5): `A := A + n`
- `Sn` (1 ≤ n ≤ 4): `A := A - n`
- `A b`: `A := A + b`
- `AH h`: `A := A + h`
- `AW w`: `A := A + w`

## Store Instructions

### Simple Store
- `SPn` (3 ≤ n ≤ 16): `P!n := A`
- `SP b`: `P!b := A`
- `SPH h`: `P!h := A`
- `SPW w`: `P!w := A`
- `SG b`: `G!b := A`
- `SG1 b`: `G!(b+256) := A`
- `SGH h`: `G!h := A`

### Indirect Store
- `PBYT`: `B%A := C`
- `XPBYT`: `A%B := C`
- `ST`: `A!0 := B`
- `STn` (1 ≤ n ≤ 3): `A!n := B`

## Function Calls

- `Kn` (3 ≤ n ≤ 11): Call A with stack increment n
- `K b`, `KH h`, `KW w`: Call A with variable stack increment
- `KnG b`: Call G!b with stack increment n
- `RTN`: Return from function

## Control Flow

- `J Ln`: Unconditional jump
- `Jrel Ln`: Conditional jump based on comparison
- `GOTO`: Jump to address in A
- `FHOP`: False hop (for conditional expressions)

## Switch Instructions

- `SWL`: Label vector switch
- `SWB`: Binary chop switch

## Floating-Point Operations

FLTOP instructions for floating-point operations:
- `FLTOP 1 b`: Convert to float and multiply by 10^b
- `FLTOP 3`: Integer to float conversion
- `FLTOP 4`: Float to integer conversion
- `FLTOP 5`: Absolute value
- `FLTOP 6-9`: Basic arithmetic (#*, #/, #+, #-)
- `FLTOP 10-11`: Unary operations (#+, #-)
- `FLTOP 12-17`: Comparisons (#=, #~=, #<, #>, #<=, #>=)

## System and Miscellaneous

- `XCH`: Exchange A and B
- `ATB`: Copy A to B
- `ATC`: Copy A to C
- `BTC`: Copy B to C
- `NOP`: No operation
- `SYS`: System call
- `BRK`: Debugger breakpoint
- `CHGCO`: Coroutine context switch

## Implementation Notes

1. All memory accesses are 64-bit aligned (8-byte boundaries)
2. Stack grows downward
3. Function calls preserve caller-saved registers
4. Floating-point operations use AArch64's NEON/FP unit
5. Switch tables use either direct jumps or binary search depending on density
6. System calls follow AArch64 ABI conventions

## Error Handling

- Division by zero raises exception 5
- Invalid memory access causes segmentation fault
- Stack overflow checking is implementation-dependent

 **Guide to Building the Instruction Generator**

This guide outlines the architecture for a new AArch64 instruction generator. A core design goal is to support two distinct output modes from a single, unified intermediate representation:

1. **JIT Mode (--run):** Directly generate executable code in memory for immediate execution.
2. **Assembly Mode (--asm):** Generate a Clang-compatible assembly text file for external assembly and linking.

This is achieved through a modular architecture based on a strict **separation of concerns**.

## **1\. The V2 Architecture: The Core and The Emitters**

The system is composed of a shared **Core** and two distinct **Emitters**:

* **Core Components:**
  * **Encoder 🏭:** Creates Instruction objects, generating both binary encoding and assembly text.
  * **InstructionStream 📋:** Holds the sequence of instructions for a function.
  * **LabelManager 🔖:** Creates and defines all labels.
  * **Linker 🔗:** Resolves all addresses and labels, producing a finalized, abstract representation of the code.
* **Final Emitters:**
  * **AssemblyWriter ✍️:** Consumes the Linker's output to write a .s assembly file.
  * **CodeBuffer 🚀:** Consumes the Linker's output to write machine code into executable memory.

---

## **2\. Shared Core Components**

### **Instruction and Label Data Types**

The Instruction struct is the key data packet, containing everything needed for both output modes:

* uint32\_t encoding; (For the CodeBuffer)
* std::string assembly\_text; (For the AssemblyWriter)
* RelocationType, Label target\_label, size\_t address

### **Encoder, LabelManager, and InstructionStream**

These components work together as previously described to build a complete, abstract representation of a function's code before any final addresses are known.

### **The Linker (The Central Hub)**

The Linker is the final step of the abstract pipeline. Its output is the "source of truth" that both emitters will use.

* **API:** std::vector\<Instruction\> process(const InstructionStream& stream, const LabelManager& manager);
* **Function:** It takes the abstract stream and produces a fully resolved std::vector\<Instruction\>, where every instruction has its final address and its binary encoding has been fully patched.

---

## **3\. The Final Emitters: Choosing Your Output**

After the Linker has done its work, the CodeGenerator chooses which emitter to use based on the command-line flag.

### **AssemblyWriter (for \--asm mode)**

This component writes a human-readable assembly file compatible with standard toolchains.

* **Design:** A simple utility that iterates over the Linker's output.
* **API:** void write\_to\_file(const std::string& path, const std::vector\<Instruction\>& instructions, ...);
* **Logic:**
  1. Prints file-level assembler directives (.section .text, .globl \_main).
  2. Iterates through the finalized instructions vector.
  3. Before printing each instruction, it checks if a label is defined at that instruction's address and prints the label name (e.g., .L1:).
  4. It then prints the assembly\_text field of the instruction, indented.

### **CodeBuffer (for \--run mode)**

This component places the binary code into memory for immediate JIT execution.

* **Design:** A utility that abstracts OS-specific memory allocation (mmap/VirtualAlloc).
* **API:** void\* commit(const std::vector\<uint8\_t\>& code);
* **Logic:**
  1. Extracts just the encoding field from each Instruction in the Linker's output.
  2. Packs these 32-bit integers into a std::vector\<uint8\_t\>.
  3. Allocates executable memory and copies the byte vector into it.
  4. Returns a function pointer to the new code block.

---

## **4\. The Complete Workflow**

C++

// 1\. Setup
LabelManager label\_manager;
InstructionStream stream(\&label\_manager);
Linker linker;

// 2\. Generate abstract instructions (common to both modes)
stream.add(Encoder::create\_load\_immediate(X0, 123));
// ...

// 3\. Finalize the abstract representation
std::vector\<Instruction\> finalized\_instructions \= linker.process(stream, label\_manager);

// 4\. CHOOSE THE EMITTER
if (mode \== "--asm") {
    AssemblyWriter writer;
    writer.write\_to\_file("output.s", finalized\_instructions, ...);
}
else if (mode \== "--run") {
    CodeBuffer code\_buffer;

    // Extract binary from the finalized instructions
    std::vector\<uint8\_t\> machine\_code;
    for (const auto& instr : finalized\_instructions) {
        // ... push bytes of instr.encoding into machine\_code ...
    }

    // Commit to executable memory and get a function pointer
    auto\* func\_ptr \= static\_cast\<void(\*)()\>(code\_buffer.commit(machine\_code));
    func\_ptr(); // Execute the code
}

### The JITMemoryManager

The JITMemoryManager is a utility class that handles the low-level, cross-platform details of allocating a block of memory, writing machine code into it, and making it executable. Its main job is to provide a safe and simple interface for the final stage of a Just-In-Time (JIT) compiler.

## Key Design Principles
W^X (Write XOR Execute) Security: This is the most important concept. Memory is either writable or executable, but never both at the same time. This prevents accidental or malicious modification of code that is running.
RAII and Move Semantics: The class manages its own memory resources. It automatically deallocates memory when it goes out of scope. It cannot be copied but can be efficiently moved using std::move to transfer ownership of the memory block.
Platform Agnostic: It provides a single API that works across Windows, macOS, and Linux, abstracting away the platform-specific calls like VirtualAlloc or mmap.
Page Alignment: All memory allocations are automatically rounded up to the system's page size to ensure optimal performance and meet platform requirements. A request for 6 bytes will result in a 4KB allocation on most systems.
## Primary JIT Workflow
The intended use follows a strict sequence:
Instantiate & Allocate: Create a JITMemoryManager object and call allocate(size).
Get Pointer & Write Code: Call getMemoryPointer() to get the raw void* buffer. The compiler's final output (the machine code byte vector) is then copied into this buffer. At this stage, the memory is writable but not executable.
Make Executable: Call makeExecutable(). This flips the memory permissions to read-only and executable.
Execute: Cast the memory pointer to a function pointer of the correct signature and call it to run the JIT-compiled code.
Cleanup: The memory is automatically released when the JITMemoryManager object is destroyed.
## API Highlights
Lifecycle:
allocate(size): Reserves memory and makes it writable.
deallocate(): Manually releases memory; called automatically by the destructor.
Permissions:
makeExecutable(): Makes memory read/execute (the main step before running code).
makeWritable(): Makes memory read/write again for code patching or recompilation.
Accessors:
getMemoryPointer(): Returns the void* to the memory block.
isExecutable(): Returns a boolean indicating the current permission state.
## Crucial Considerations
Error Handling: All functions can throw a JITMemoryManagerException on failure (e.g., allocating zero bytes, OS-level errors). All calls should be wrapped in a try...catch block.
Thread Safety: The class is not thread-safe. If the compiler is multi-threaded, each thread that generates code must have its own separate JITMemoryManager instance. External locking is required if an instance is ever shared.

Based on the provided files, here are the condensed notes on the `ConstantFoldingPass`.

### Core Purpose

The **`ConstantFoldingPass`** is a compile-time optimization that traverses the Abstract Syntax Tree (AST) to evaluate constant expressions and apply algebraic simplifications, reducing the complexity of the program before code generation.

---

### Key Optimizations Performed

* **Constant Folding**:
    * Evaluates binary operations (`+`, `-`, `*`, `/`) on two integer literals or two float literals at compile time.
    * Evaluates comparison operators (`==`, `!=`, `<`, `<=`, `>`, `>=`) on two integer literals, replacing them with `0` (false) or `-1` (true).

* **Manifest Constant Substitution**:
    * Replaces any access to a variable that is a `MANIFEST` constant with its corresponding `NumberLiteral` value. This relies on a pre-populated map of manifest names to their values.

* **Algebraic Simplification (Identity & Null Elements)**:
    * Simplifies expressions by applying identity rules (e.g., `x + 0` becomes `x`; `x * 1` becomes `x`).
    * Reduces expressions using null elements (e.g., `x * 0` becomes `0`).

* **Strength Reduction**:
    * Replaces expensive multiplication or division by a power of two with cheaper bit-shift operations (e.g., `x * 2` is transformed into `x << 1`).

* **Constant Condition Elimination**:
    * For conditional structures (`IF`, `TEST`, and ternary expressions), if the condition evaluates to a compile-time constant, the pass eliminates the branch.
    * It replaces the entire structure with either the `then` or `else` block, effectively removing dead code.

---

### Implementation Strategy

* **Recursive Visitor Pattern**: The pass uses the visitor pattern to traverse every node in the AST. It recursively visits the children of a node before processing the node itself, ensuring optimizations are applied from the leaves of the tree upwards.
* **Functional AST Transformation**: Instead of modifying the AST in place, each `visit` method returns a **new, optimized** smart pointer (`ExprPtr`, `StmtPtr`) to a node. The parent node then uses these new, optimized children to construct a new version of itself. This functional approach avoids side effects.
* **State Dependency**: The pass is initialized with a reference to a map of manifest constants (`std::unordered_map<std::string, int64_t>& manifests`), which it uses for the substitution optimization.

### The CommonSubexpressionEliminationPass (CSE)
The CommonSubexpressionEliminationPass (CSE) is a compiler optimization that aims to improve performance by finding identical subexpressions within the code, computing them only once, storing the result in a temporary variable, and then reusing that temporary variable for all subsequent occurrences.

High-Level Strategy
The pass works by transforming code like this:

x := a + b
y := a + b
Into this:

_cse_temp_0 := a + b  // 1. Hoist the calculation
x := _cse_temp_0      // 2. Replace the first use
y := _cse_temp_0      // 3. Replace the second use
This is achieved through a three-step process for each expression:
Identify: Create a unique, canonical representation of a subexpression.
Hoist: On the first encounter, generate a new temporary variable and assign the subexpression's result to it.
Replace: On all encounters (including the first), replace the subexpression with an access to the temporary variable.
Implementation Details
Canonical Representation: A key function, expressionToString(), converts expression nodes into a unique string format (e.g., a BinaryOp for a + b becomes "(OpPlus a b)"). This string is used to identify identical expressions.
availableExpressions Map: The pass maintains a stateful map (std::map<std::string, std::string>). This map stores the canonical expression string as the key and the name of the temporary variable holding its result as the value.
AST Transformation: The pass dynamically rewrites the Abstract Syntax Tree (AST). When it hoists a new subexpression, it transforms a single Assignment statement into a CompoundStatement containing two new assignments: one for the temporary variable and one for the original target variable.
Visitor Pattern: The pass traverses the AST using the visitor pattern, recursively visiting nodes to analyze expressions and transform statements.
Scope and Critical Limitations
Scope: The optimization is applied on a per-function basis. The map of available expressions is cleared at the beginning of each new function declaration.
Basic Block Limitation: The current implementation is only safe for use within a single basic block (a straight line of code with no branches in or out). It does not correctly account for control flow. The availableExpressions map is not cleared at labels, after function calls with potential side effects, or when a variable within a subexpression is reassigned. This could lead to incorrect optimizations across branches or loops. For example, in x := a + b; a := 5; y := a + b, the pass would incorrectly replace the second a + b with the result of the first.

LoopInvariantCodeMotionPass
The LoopInvariantCodeMotionPass is an optimization pass designed to perform Loop Invariant Code Motion (LICM). Its primary goal is to identify computations within loops that produce the same result in every iteration (i.e., are "loop-invariant") and move them outside the loop. This reduces redundant computations, leading to more efficient code execution.

Purpose
The main purpose of this pass is to enhance program performance by:
Reducing Redundant Computations: Expressions whose values do not change during loop execution are computed only once before the loop begins, rather than repeatedly in each iteration.
Improving Execution Speed: By moving invariant computations out of the loop, the total number of operations executed within the loop decreases, leading to faster execution times.
Implementation Details
The LoopInvariantCodeMotionPass is implemented as a visitor over the Abstract Syntax Tree (AST) of the program. It inherits from OptimizationPass and overrides the apply method to initiate the transformation.

Constructor and Initialization
The constructor LoopInvariantCodeMotionPass(std::unordered_map<std::string, int64_t>& manifests) initializes the pass with a reference to a map of manifests. These manifests likely contain compile-time constants or other global information that might be relevant for determining loop invariance.
Core Logic (apply method)
The apply(ProgramPtr program) method is the entry point for the optimization. It calls the visit method with the root Program node, initiating the AST traversal and transformation.
Visitor Pattern Implementation
The pass uses the visitor pattern to traverse the AST. It defines visit methods for different types of AST nodes: Expression, Statement, Declaration, and Program.
Dispatchers: The visit(Expression* node), visit(Statement* node), and visit(Declaration* node) methods act as dispatchers, using dynamic_cast to determine the concrete type of the node and then forwarding the call to the appropriate specialized visit method. If an unsupported node type is encountered, a std::runtime_error is thrown.
Expression Visitors: For most expression types (e.g., NumberLiteral, BinaryOp, FunctionCall), the visit methods simply reconstruct the expression node by recursively visiting its children. This implies that the current implementation primarily focuses on identifying loop-invariant statements or expressions within statements, rather than performing LICM on arbitrary expressions in isolation. The original node is effectively copied, with its children potentially optimized.
Declaration Visitors:
visit(Program* node): Iterates through declarations in the program, applies the visitor to each, and constructs a new Program with the potentially optimized declarations.
visit(LetDeclaration* node): Visits the initializers of LetDeclaration nodes.
visit(FunctionDeclaration* node): Visits the body (either statement or expression) of a function declaration.
GlobalDeclaration, ManifestDeclaration, GetDirective nodes are explicitly returned as nullptr from visit(Declaration* node), implying they are not subject to this optimization or are handled elsewhere.
Statement Visitors: This is where the core LICM logic is applied for loops.
visit(CompoundStatement* node): Iterates through and visits each statement within a compound statement.
visit(ForStatement* node): This is the crucial part of the LICM pass. Instead of implementing the LICM logic directly, it delegates to an external LoopOptimizer::process method. This method is called with the ForStatement node and a temporary Optimizer instance. The manifests from the LoopInvariantCodeMotionPass are set on this temporary Optimizer instance, suggesting that the LoopOptimizer::process method utilizes these manifests to determine loop invariance.
Other statement visitors (e.g., Assignment, IfStatement, WhileStatement, RepeatStatement, SwitchonStatement) primarily traverse their child nodes and reconstruct the statement, allowing nested loops and expressions within them to be processed.
DeclarationStatement nodes are visited to optimize their contained declarations. If a declaration is optimized away (returns nullptr), the DeclarationStatement itself will also be optimized away.
Dependencies
OptimizationPass.h: Provides the base interface for optimization passes.
AST.h: Defines the Abstract Syntax Tree nodes that the pass operates on.
LoopOptimizer.h: Contains the core logic for loop optimization, specifically used by LoopInvariantCodeMotionPass for ForStatement nodes.
Optimizer.h: An overarching optimizer class, an instance of which is temporarily created to utilize the LoopOptimizer::process method.
<unordered_map>, <memory>, <vector>, <stdexcept>: Standard C++ library includes for data structures and error handling.
Key Design Points
Delegation to LoopOptimizer: The pass offloads the complex LICM analysis and transformation for ForStatement nodes to a separate LoopOptimizer utility. This promotes modularity and reusability of the core loop optimization logic.
Visitor Pattern: Consistent use of the visitor pattern simplifies AST traversal and transformation.
Smart Pointers: The use of ProgramPtr, ExprPtr, StmtPtr, and DeclPtr (likely std::unique_ptr aliases) indicates modern C++ practices for memory management.
Manifests: The manifests map is passed around, indicating that global constants or configuration values can influence the optimization decisions, particularly in determining loop invariance.
Potential Enhancements/Considerations
Detailed LICM Logic: The current code delegates to LoopOptimizer::process. Understanding the internal workings of LoopOptimizer (e.g., how it identifies loop-invariant expressions, handles side effects, and correctly hoists code) is crucial for a complete picture.
Side Effects: A robust LICM implementation must carefully analyze expressions for side effects. Expressions with side effects (e.g., function calls that modify global state, I/O operations) generally cannot be hoisted out of a loop without changing program semantics.
Pre-header Blocks: Typically, hoisted loop-invariant code is moved to a "pre-header" basic block that executes unconditionally before the loop entry. The LoopOptimizer should handle the creation and population of such blocks.
Loop Entry Dominance: For an expression to be safely hoisted, it must dominate all loop exits. This ensures that the hoisted code is executed only if the loop is entered.
Variable Liveness: After hoisting, the hoisted expression's result should be stored in a temporary variable, and subsequent uses within the loop should refer to this temporary.
More Granular LICM: While the ForStatement is the explicit target, loop-invariant code could also exist in WhileStatement or RepeatStatement bodies, which are currently only traversed. Extending LICM to these loop types might be beneficial.
Error Handling: The throw std::runtime_error for unsupported nodes is a valid approach for development, but in a production compiler, more specific error messages or a fallback mechanism might be desired.



# **Guide to Building the Instruction Generator**

This guide outlines the architecture for a new AArch64 instruction generator. A core design goal is to support two distinct output modes from a single, unified intermediate representation:

1. **JIT Mode (--run):** Directly generate executable code in memory for immediate execution.
2. **Assembly Mode (--asm):** Generate a Clang-compatible assembly text file for external assembly and linking.

This is achieved through a modular architecture based on a strict **separation of concerns**.

## **1\. The V2 Architecture: The Core and The Emitters**

The system is composed of a shared **Core** and two distinct **Emitters**:

* **Core Components:**
  * **Encoder 🏭:** Creates Instruction objects, generating both binary encoding and assembly text.
  * **InstructionStream 📋:** Holds the sequence of instructions for a function.
  * **LabelManager 🔖:** Creates and defines all labels.
  * **Linker 🔗:** Resolves all addresses and labels, producing a finalized, abstract representation of the code.
* **Final Emitters:**
  * **AssemblyWriter ✍️:** Consumes the Linker's output to write a .s assembly file.
  * **CodeBuffer 🚀:** Consumes the Linker's output to write machine code into executable memory.

---

## **2\. Shared Core Components**

### **Instruction and Label Data Types**

The Instruction struct is the key data packet, containing everything needed for both output modes:

* uint32\_t encoding; (For the CodeBuffer)
* std::string assembly\_text; (For the AssemblyWriter)
* RelocationType, Label target\_label, size\_t address

### **Encoder, LabelManager, and InstructionStream**

These components work together as previously described to build a complete, abstract representation of a function's code before any final addresses are known.

### **The Linker (The Central Hub)**

The Linker is the final step of the abstract pipeline. Its output is the "source of truth" that both emitters will use.

* **API:** std::vector\<Instruction\> process(const InstructionStream& stream, const LabelManager& manager);
* **Function:** It takes the abstract stream and produces a fully resolved std::vector\<Instruction\>, where every instruction has its final address and its binary encoding has been fully patched.

---

## **3\. The Final Emitters: Choosing Your Output**

After the Linker has done its work, the CodeGenerator chooses which emitter to use based on the command-line flag.

### **AssemblyWriter (for \--asm mode)**

This component writes a human-readable assembly file compatible with standard toolchains.

* **Design:** A simple utility that iterates over the Linker's output.
* **API:** void write\_to\_file(const std::string& path, const std::vector\<Instruction\>& instructions, ...);
* **Logic:**
  1. Prints file-level assembler directives (.section .text, .globl \_main).
  2. Iterates through the finalized instructions vector.
  3. Before printing each instruction, it checks if a label is defined at that instruction's address and prints the label name (e.g., .L1:).
  4. It then prints the assembly\_text field of the instruction, indented.

### **CodeBuffer (for \--run mode)**

This component places the binary code into memory for immediate JIT execution.

* **Design:** A utility that abstracts OS-specific memory allocation (mmap/VirtualAlloc).
* **API:** void\* commit(const std::vector\<uint8\_t\>& code);
* **Logic:**
  1. Extracts just the encoding field from each Instruction in the Linker's output.
  2. Packs these 32-bit integers into a std::vector\<uint8\_t\>.
  3. Allocates executable memory and copies the byte vector into it.
  4. Returns a function pointer to the new code block.

---

## **4\. The Complete Workflow**

C++

// 1\. Setup
LabelManager label\_manager;
InstructionStream stream(\&label\_manager);
Linker linker;

// 2\. Generate abstract instructions (common to both modes)
stream.add(Encoder::create\_load\_immediate(X0, 123));
// ...

// 3\. Finalize the abstract representation
std::vector\<Instruction\> finalized\_instructions \= linker.process(stream, label\_manager);

// 4\. CHOOSE THE EMITTER
if (mode \== "--asm") {
    AssemblyWriter writer;
    writer.write\_to\_file("output.s", finalized\_instructions, ...);
}
else if (mode \== "--run") {
    CodeBuffer code\_buffer;

    // Extract binary from the finalized instructions
    std::vector\<uint8\_t\> machine\_code;
    for (const auto& instr : finalized\_instructions) {
        // ... push bytes of instr.encoding into machine\_code ...
    }

    // Commit to executable memory and get a function pointer
    auto\* func\_ptr \= static\_cast\<void(\*)()\>(code\_buffer.commit(machine\_code));
    func\_ptr(); // Execute the code
}



## **RepeatUntilOptimizationPass**

The RepeatUntilOptimizationPass is an optimization pass focused on REPEAT UNTIL loop constructs. Its purpose is to perform constant folding on the loop's condition and, based on the constant value, transform the loop into a more efficient equivalent.

### **Purpose**

The main goals of this pass are:

* **Constant Folding on Loop Conditions**: Evaluate the UNTIL condition of REPEAT loops if it resolves to a constant value.
* **Specialized Loop Transformations**:
  * If REPEAT \<body\> UNTIL \<true\>: Transform the loop into just its \<body\>, as it will execute exactly once.
  * If REPEAT \<body\> UNTIL \<false\>: Transform the loop into a WHILE \<true\> DO \<body\>, effectively creating an infinite loop, which matches the semantics of REPEAT UNTIL false in BCPL (where false is 0).

### **Implementation Details**

The RepeatUntilOptimizationPass is implemented as an AST visitor. It traverses the program's AST, specifically targeting RepeatStatement nodes, and applies the aforementioned transformations.

#### **Constructor and Initialization**

* The constructor RepeatUntilOptimizationPass(std::unordered\_map\<std::string, int64\_t\>& manifests) initializes the pass with a reference to a map of manifests. These manifests likely contain compile-time constants that might be used in expressions, allowing the pass to potentially resolve conditions to constants.

#### **Core Logic (visit(RepeatStatement\* node))**

This is the central part of the optimization:

1. **Recursive Optimization of Children**: It first recursively calls visit on the node-\>body and node-\>condition (if present) to ensure that any nested expressions or statements within them are also optimized before the current loop is processed.
2. **REPEATUNTIL Check**: The optimization *only* applies if node-\>loopType is specifically RepeatStatement::LoopType::repeatuntil and a condition (new\_cond) exists. This prevents unintended transformations of REPEATWHILE or simple REPEAT loops.
3. **Constant Condition Check**: It attempts to dynamic\_cast the new\_cond (the optimized condition expression) to a NumberLiteral. This means the optimization only triggers if the condition expression has been successfully constant-folded by previous optimization passes (or is inherently a literal).
   * **If cond\_lit-\>value \!= 0 (True Condition)**:
     * In BCPL, any non-zero value is considered TRUE.
     * If the condition is constantly true, the REPEAT UNTIL loop executes its body exactly once and then exits.
     * The loop node is replaced entirely by its new\_body, effectively eliminating the loop construct.
   * **If cond\_lit-\>value \== 0 (False Condition)**:
     * In BCPL, 0 is considered FALSE.
     * If the condition is constantly false, the REPEAT UNTIL loop will never terminate, resulting in an infinite loop.
     * It transforms this into a WhileStatement with a condition that is always true (std::make\_unique\<NumberLiteral\>(-1) since BCPL true is often \-1). This explicitly represents the infinite loop behavior.
4. **No Optimization Fallback**: If the loopType is not repeatuntil, or if the condition is not a constant NumberLiteral, or if there's no condition, the RepeatStatement is simply reconstructed with its (potentially optimized) children and its original loopType, without applying any special transformation.

#### **Boilerplate Visitor Implementations (Pass-through)**

* **General visit dispatchers**: visit(Expression\* node), visit(Statement\* node), and visit(Declaration\* node) act as dispatchers, using dynamic\_cast to forward the call to the appropriate specialized visitor method for each AST node type. Unsupported types throw std::runtime\_error.
* **Recursive Traversal**: The majority of the visit methods (e.g., for BinaryOp, IfStatement, CompoundStatement, FunctionDeclaration, etc.) primarily serve to recursively traverse the AST. They optimize the children of the current node and then reconstruct the current node with its optimized children. This ensures that RepeatStatement nodes nested deeply within the AST are eventually encountered and processed.
* **visit(VariableAccess\* node)**: This specific visitor attempts to perform manifest constant folding. If a VariableAccess name matches a key in the manifests map, it replaces the VariableAccess node with a NumberLiteral containing the manifest's value. This is crucial for the RepeatStatement optimization to work if the loop condition involves manifest constants.

### **Key Design Points**

* **Specific Loop Optimization**: This pass targets a very specific loop construct (REPEAT UNTIL) and condition type (constant NumberLiteral), making it highly specialized.
* **Constant Folding Dependency**: Its effectiveness heavily relies on previous or concurrent constant folding passes that can resolve expressions in the loop condition to NumberLiteral values. The visit(VariableAccess\* node) method helps in this by folding manifest constants.
* **Semantics Preservation**: The transformations (single execution vs. infinite loop) strictly adhere to the semantics of REPEAT UNTIL based on BCPL's interpretation of truthiness.
* **AST Transformation**: The pass actively transforms the AST by replacing RepeatStatement nodes with CompoundStatement (containing just the body) or WhileStatement nodes.
* **No Transformation if No Opportunity**: The pass is designed to reconstruct the original RepeatStatement if no optimization opportunity (i.e., constant REPEAT UNTIL condition) is found, ensuring that the AST remains valid.

### **Potential Enhancements/Considerations for a Custom Version**

* **Pre-Requisite for Constant Folding**: Explicitly state or ensure that a robust Constant Folding/Propagation pass runs *before* this pass to maximize the chances of RepeatStatement conditions becoming NumberLiterals.
* **Support for Other Constant Types**: If the language has other literal types (e.g., FloatLiteral used in conditions), extend the NumberLiteral check to include them and define their truthiness.
* **Loop Unrolling for Small, Known Iterations**: For REPEATUNTIL loops that are determined to run a small, constant number of times (e.g., REPEAT ... UNTIL N where N is an integer count, and the loop body does not contain side effects that prevent unrolling), the loop could be unrolled by duplicating the body N times. This is a more complex optimization than the current pass.
* **Handling BREAK and LOOP (Continue)**: If the RepeatStatement body contains BREAK or LOOP (similar to continue) statements, the transformations need to correctly handle their target semantics in the new WhileStatement or inlined body.
* **Generalized Loop Transformations**: Consider a more general loop optimization framework that can transform different loop types (FOR, WHILE, REPEAT) into each other based on constant conditions or other properties.
* **Warning for Infinite Loops**: For REPEAT \<body\> UNTIL \<false\> scenarios, a compiler might issue a warning to the user about a detected infinite loop, as this is often an unintentional programming error.
* **Cost Model**: For very large loop bodies that execute only once (REPEAT \<body\> UNTIL \<true\>), simple inlining might lead to code bloat if the \<body\> is huge. While this is less critical for a single execution, a more advanced optimizer might consider the size of the \<body\> before performing this transformation, although for REPEAT UNTIL true, just replacing it with the body is usually a win.



## **LoopInvariantCodeMotionPass**

The LoopInvariantCodeMotionPass is an optimization pass designed to perform Loop Invariant Code Motion (LICM). Its primary goal is to identify computations within loops that produce the same result in every iteration (i.e., are "loop-invariant") and move them outside the loop. This reduces redundant computations, leading to more efficient code execution.

### **Purpose**

The main purpose of this pass is to enhance program performance by:

* **Reducing Redundant Computations**: Expressions whose values do not change during loop execution are computed only once before the loop begins, rather than repeatedly in each iteration.
* **Improving Execution Speed**: By moving invariant computations out of the loop, the total number of operations executed within the loop decreases, leading to faster execution times.

### **Implementation Details**

The LoopInvariantCodeMotionPass is implemented as a visitor over the Abstract Syntax Tree (AST) of the program. It inherits from OptimizationPass and overrides the apply method to initiate the transformation.

#### **Constructor and Initialization**

* The constructor LoopInvariantCodeMotionPass(std::unordered\_map\<std::string, int64\_t\>& manifests) initializes the pass with a reference to a map of manifests. These manifests likely contain compile-time constants or other global information that might be relevant for determining loop invariance.

#### **Core Logic (apply method)**

* The apply(ProgramPtr program) method is the entry entry point for the optimization. It calls the visit method with the root Program node, initiating the AST traversal and transformation.

#### **Visitor Pattern Implementation**

The pass uses the visitor pattern to traverse the AST. It defines visit methods for different types of AST nodes: Expression, Statement, Declaration, and Program.

* **Dispatchers**: The visit(Expression\* node), visit(Statement\* node), and visit(Declaration\* node) methods act as dispatchers, using dynamic\_cast to determine the concrete type of the node and then forwarding the call to the appropriate specialized visit method. If an unsupported node type is encountered, a std::runtime\_error is thrown.
* **Expression Visitors**: For most expression types (e.g., NumberLiteral, BinaryOp, FunctionCall), the visit methods simply reconstruct the expression node by recursively visiting its children. This implies that the current implementation primarily focuses on identifying loop-invariant *statements* or expressions within statements, rather than performing LICM on arbitrary expressions in isolation. The original node is effectively copied, with its children potentially optimized.
* **Declaration Visitors**:
  * visit(Program\* node): Iterates through declarations in the program, applies the visitor to each, and constructs a new Program with the potentially optimized declarations.
  * visit(LetDeclaration\* node): Visits the initializers of LetDeclaration nodes.
  * visit(FunctionDeclaration\* node): Visits the body (either statement or expression) of a function declaration.
  * GlobalDeclaration, ManifestDeclaration, GetDirective nodes are explicitly returned as nullptr from visit(Declaration\* node), implying they are not subject to this optimization or are handled elsewhere.
* **Statement Visitors**: This is where the core LICM logic is applied for loops.
  * visit(CompoundStatement\* node): Iterates through and visits each statement within a compound statement.
  * visit(ForStatement\* node): This is the crucial part of the LICM pass. Instead of implementing the LICM logic directly, it delegates to an external LoopOptimizer::process method. This method is called with the ForStatement node and a temporary Optimizer instance. The manifests from the LoopInvariantCodeMotionPass are set on this temporary Optimizer instance, suggesting that the LoopOptimizer::process method utilizes these manifests to determine loop invariance.
  * Other statement visitors (e.g., Assignment, IfStatement, WhileStatement, RepeatStatement, SwitchonStatement) primarily traverse their child nodes and reconstruct the statement, allowing nested loops and expressions within them to be processed.
  * DeclarationStatement nodes are visited to optimize their contained declarations. If a declaration is optimized away (returns nullptr), the DeclarationStatement itself will also be optimized away.

#### **Dependencies**

* OptimizationPass.h: Provides the base interface for optimization passes.
* AST.h: Defines the Abstract Syntax Tree nodes that the pass operates on.
* LoopOptimizer.h: Contains the core logic for loop optimization, specifically used by LoopInvariantCodeMotionPass for ForStatement nodes.
* Optimizer.h: An overarching optimizer class, an instance of which is temporarily created to utilize the LoopOptimizer::process method.
* \<unordered\_map\>, \<memory\>, \<vector\>, \<stdexcept\>: Standard C++ library includes for data structures and error handling.

#### **Key Design Points**

* **Delegation to LoopOptimizer**: The pass offloads the complex LICM analysis and transformation for ForStatement nodes to a separate LoopOptimizer utility. This promotes modularity and reusability of the core loop optimization logic.
* **Visitor Pattern**: Consistent use of the visitor pattern simplifies AST traversal and transformation.
* **Smart Pointers**: The use of ProgramPtr, ExprPtr, StmtPtr, and DeclPtr (likely std::unique\_ptr aliases) indicates modern C++ practices for memory management.
* **Manifests**: The manifests map is passed around, indicating that global constants or configuration values can influence the optimization decisions, particularly in determining loop invariance.

### **Potential Enhancements/Considerations**

* **Detailed LICM Logic**: The current code delegates to LoopOptimizer::process. Understanding the internal workings of LoopOptimizer (e.g., how it identifies loop-invariant expressions, handles side effects, and correctly hoists code) is crucial for a complete picture.
* **Side Effects**: A robust LICM implementation must carefully analyze expressions for side effects. Expressions with side effects (e.g., function calls that modify global state, I/O operations) generally cannot be hoisted out of a loop without changing program semantics.
* **Pre-header Blocks**: Typically, hoisted loop-invariant code is moved to a "pre-header" basic block that executes unconditionally before the loop entry. The LoopOptimizer should handle the creation and population of such blocks.
* **Loop Entry Dominance**: For an expression to be safely hoisted, it must dominate all loop exits. This ensures that the hoisted code is executed only if the loop is entered.
* **Variable Liveness**: After hoisting, the hoisted expression's result should be stored in a temporary variable, and subsequent uses within the loop should refer to this temporary.
* **More Granular LICM**: While the ForStatement is the explicit target, loop-invariant code could also exist in WhileStatement or RepeatStatement bodies, which are currently only traversed. Extending LICM to these loop types might be beneficial.
* **Error Handling**: The throw std::runtime\_error for unsupported nodes is a valid approach for development, but in a production compiler, more specific error messages or a fallback mechanism might be desired.
