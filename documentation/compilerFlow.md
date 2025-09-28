# **Compiler Flow and Architecture**

This document outlines the complete compilation process, detailing how each component of the modern BCPL compiler interacts to transform source code into executable AArch64 machine code. The architecture is designed to be modular, maintainable, and support both Ahead-of-Time (AOT) compilation to assembly files and Just-in-Time (JIT) compilation directly to memory.  
The compilation process can be broken down into three main phases: the **Frontend**, the **Middle-end (Optimizer)**, and the **Backend**.

## **1\. Frontend: Parsing**

The process begins with the **Parser**, which reads the BCPL source code (.b file).

* **Input**: BCPL source text.  
* **Action**: The Parser tokenizes the source code and builds a hierarchical representation of its structure.  
* **Output**: An **Abstract Syntax Tree (AST)**. The root of this tree is a Program node, and it contains a complete, structured representation of the source code, including all declarations, statements, and expressions. The AST is the central data structure for the rest of the compilation process.

## **2\. Middle-end: Optimization**

Once the initial AST is built, it's passed to the **Optimizer**. The Optimizer's goal is to transform the AST into a new, functionally equivalent AST that will result in more efficient machine code.

* **Input**: The initial AST from the Parser.  
* **Action**: A PassManager applies a sequence of optimization passes to the AST. Each pass traverses the tree and performs specific transformations:  
  1. **Constant Folding**: Evaluates constant expressions at compile time (e.g., 2 \+ 5 becomes 7).  
  2. **Common Subexpression Elimination**: Finds identical expressions and computes them only once.  
  3. **Loop Invariant Code Motion (LICM)**: Moves calculations that don't change inside a loop to outside the loop.  
* **Output**: A new, optimized AST.

## **3\. Backend: Code Generation**

The optimized AST is then handed to the **CodeGenerator**, which is the main orchestrator for the backend. The backend's process is a sophisticated collaboration between multiple components to handle instructions, registers, memory, and data.  
This process is performed **per function**. For each FunctionDeclaration or RoutineDeclaration in the AST, the following steps occur:

### **Step 3.1: Function Setup**

The CodeGenerator's DeclarationGenerator helper begins processing the function.

1. A new CallFrameManager is created for the function to manage its stack frame.  
2. A new InstructionStream is created to hold the sequence of generated instructions for this function.  
3. A new LabelManager is created to manage local labels for branches within the function.

### **Step 3.2: Body Traversal and Instruction Generation**

The CodeGenerator traverses the function's body (a tree of statements and expressions).

* **Delegation**: The CodeGenerator acts as a dispatcher. It delegates the actual work to its helpers:  
  * **StatementGenerator**: Handles control flow like IF, FOR, and WHILE loops. It creates labels via the LabelManager and adds branch instructions to the InstructionStream.  
  * **ExpressionGenerator**: Handles all expressions. Its goal is to compute the value of an expression and leave the result in a register. It interacts heavily with the RegisterManager to acquire and release scratch registers.  
* **Instruction Encoding**: Both helpers use the static Encoder class to create Instruction objects. Each Instruction contains both its **assembly text** ("ADD X0, X1, X2") and its **preliminary binary encoding** (0x8B020020). For branch instructions, the encoding is incomplete and marked with a RelocationType.  
* **Data Handling**: When the ExpressionGenerator encounters a literal that cannot be encoded directly into an instruction (like a string or a large float), it registers it with the **DataGenerator**. The DataGenerator stores the literal, assigns it a unique label (e.g., L\_str0), and returns the label. The ExpressionGenerator then generates code to load the address of this label into a register.

### **Step 3.3: Linking**

Once the traversal of the function's body is complete, the InstructionStream contains a complete, but unlinked, representation of the function's code.

* **Input**: The InstructionStream and the LabelManager for the function.  
* **Action**: The CodeGenerator calls the **Linker**. The Linker performs two key actions:  
  1. It resolves all label addresses. For every branch instruction, it looks up the target label's address in the LabelManager.  
  2. It calculates the correct PC-relative offset for the branch and **patches the binary encoding** of the instruction.  
* **Output**: A std::vector\<Instruction\> where every instruction is fully resolved and its binary encoding is final.

### **Step 3.4: Final Assembly and Emission**

The CodeGenerator now has everything it needs to produce the final output.

1. It generates the function's **prologue** and **epilogue** using the CallFrameManager. The prologue is prepended to the finalized instruction vector, and the epilogue is appended.  
2. The complete set of instructions for the function is now ready.  
3. The DataGenerator is called to generate the assembly directives for the .rodata and .data sections, which contain all the string literals, float constants, and static variables collected during the process.  
4. Finally, an **Emitter** is chosen based on the compiler's mode (AOT vs. JIT):  
   * **AOT (Assembly File)**: An AssemblyWriter iterates through the finalized instructions, printing the assembly\_text of each to a .s file. It then appends the data sections from the DataGenerator.  
   * **JIT (In-Memory Execution)**: The CodeBuffer class is used. It takes the finalized vector of instructions, extracts only the binary encoding from each, and copies these bytes into a block of writeable memory. It then changes the memory permissions to executable and returns a function pointer to the start of the code, ready to be called.