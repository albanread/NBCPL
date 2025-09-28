This compiler processes BCPL source code and can produce two distinct outputs: a just-in-time (JIT) executable in memory or a standard, linkable ARM64 assembly file.

Its workflow is as follows:
Frontend Processing: The compiler reads a .bcl source file. A Lexer tokenizes the code, and a Parser builds an Abstract Syntax Tree (AST) representing the program's structure. An optional tracing feature allows for detailed output of this stage.
Analysis and Optimization: The ASTAnalyzer singleton traverses the AST to collect metrics, such as identifying all function calls and variable definitions, which informs the code generation process. Optional optimization passes can modify the AST to improve the code.
Intermediate Code Generation: The NewCodeGenerator walks the analyzed AST and produces a stream of machine-agnostic Instruction objects. For calls to C runtime functions like WRITES, it generates a sequence of MOVZ/MOVK instructions to load the function's absolute address into a register, followed by a BLR (Branch with Link to Register) instruction. This intermediate representation is designed for JIT compilation. String literals and other global data are handled by a DataGenerator.
Output Path Selection: The compiler's behavior diverges based on command-line flags (--run for JIT, --asm for assembly, --exec for a full build-and-run cycle).
JIT Execution (--run): A Linker resolves label addresses within the instruction stream. The finalized machine code is committed to an executable memory buffer by the CodeBuffer, and a function pointer to the entry point is executed directly.
Static Assembly (--asm or --exec): The AssemblyWriter is invoked. It performs a critical transformation:
It identifies the MOVZ/MOVK/BLR sequences used for runtime calls in the JIT-oriented intermediate code.
It replaces these sequences with standard, linker-friendly BL <_label> instructions (e.g., BL _WRITES).
It converts other intermediate forms (like data directives) into syntax compatible with the clang assembler (e.g., DCD becomes .word).
This transformed, standard assembly code is written to a .s file.
Execution (--exec): If the --exec flag is used, main.cpp orchestrates a command-line build process. It calls clang to assemble the generated .s file and link it against a pre-compiled bcpl_runtime.o object file, creating a native executable. This executable is then immediately run.
