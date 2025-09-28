## Plan for Control Flow Graph (CFG) Builder Pass

**Goal:** To build a Control Flow Graph (CFG) from the Abstract Syntax Tree (AST) for each function and routine in the BCPL program. The CFG will represent the program's execution paths as a collection of basic blocks and the edges between them.

**Key Concepts:**

*   **Basic Block:** A sequence of consecutive statements in which flow of control enters at the beginning and leaves at the end without halt or possibility of branching except at the end.
*   **Edge:** Represents a transfer of control from one basic block to another.
*   **Entry Block:** The first basic block of a function/routine.
*   **Exit Block:** A basic block from which control leaves the function/routine (e.g., `RETURN`, `FINISH`).

**Data Structures:**

1.  **`BasicBlock` Class:**
    *   `std::string id;`: Unique identifier for the basic block (e.g., "BB_0", "BB_1").
    *   `std::vector<StmtPtr> statements;`: A list of AST statements contained within this basic block.
    *   `std::vector<BasicBlock*> successors;`: Pointers to basic blocks that can be executed immediately after this block.
    *   `std::vector<BasicBlock*> predecessors;`: Pointers to basic blocks that can transfer control to this block.
    *   `bool is_entry = false;`: True if this is the entry block of a function/routine.
    *   `bool is_exit = false;`: True if this is an exit block of a function/routine.
    *   `std::string label_name = "";`: If this block starts with a label, its name.

2.  **`ControlFlowGraph` Class:**
    *   `std::unordered_map<std::string, std::unique_ptr<BasicBlock>> blocks;`: Owns all basic blocks in the graph.
    *   `BasicBlock* entry_block = nullptr;`: Pointer to the entry basic block.
    *   `BasicBlock* exit_block = nullptr;`: Pointer to a conceptual exit block (useful for analysis, though not all functions have a single explicit exit).
    *   `std::string function_name;`: The name of the function/routine this CFG belongs to.
    *   `BasicBlock* create_block(const std::string& id_prefix = "BB_");`: Factory method to create and add a new basic block.
    *   `void add_edge(BasicBlock* from, BasicBlock* to);`: Adds a control flow edge between two blocks.
    *   `BasicBlock* get_block(const std::string& id) const;`: Retrieves a block by its ID.

**CFG Builder Pass (`CFGBuilderPass`) Class Structure:**

*   **Inherits from `ASTVisitor`**: This pass will traverse the AST. It will *not* inherit from `Optimizer` directly, as its primary role is to *build* a representation, not to *optimize* the AST in place. It will be a separate `ASTPass` or similar.
*   **Private Members:**
    *   `std::unordered_map<std::string, std::unique_ptr<ControlFlowGraph>> function_cfgs;`: Stores a CFG for each function/routine.
    *   `ControlFlowGraph* current_cfg = nullptr;`: Pointer to the CFG currently being built.
    *   `BasicBlock* current_basic_block = nullptr;`: Pointer to the basic block currently being populated.
    *   `std::unordered_map<std::string, BasicBlock*> label_targets;`: Maps label names to their corresponding basic blocks (for `GOTO` resolution).
    *   `int block_id_counter = 0;`: Counter for generating unique basic block IDs.
    *   `std::vector<BasicBlock*> break_targets;`: Stack to keep track of `BREAK` targets (block after loop/switch).
    *   `std::vector<BasicBlock*> loop_targets;`: Stack to keep track of `LOOP` targets (start of loop body).
    *   `std::vector<BasicBlock*> endcase_targets;`: Stack to keep track of `ENDCASE` targets (block after `SWITCHON`).

*   **Public Methods:**
    *   `CFGBuilderPass();`
    *   `std::string getName() const override { return "CFG Builder Pass"; }`
    *   `void build(Program& program);`: Main entry point to start building CFGs for the entire program.
    *   `const std::unordered_map<std::string, std::unique_ptr<ControlFlowGraph>>& get_cfgs() const { return function_cfgs; }`

*   **Overridden `ASTVisitor` Methods (Key Logic):**

    *   **`visit(Program& node)`:**
        *   Iterate through declarations and statements.
        *   For `FunctionDeclaration` and `RoutineDeclaration`, initiate a new CFG build.
        *   For top-level statements, they might form a "main" CFG or be ignored for now if we focus on functions.

    *   **`visit(FunctionDeclaration& node)` / `visit(RoutineDeclaration& node)`:**
        *   Create a new `ControlFlowGraph` for this function/routine.
        *   Set `current_cfg` to this new CFG.
        *   Create the `entry_block` for the function.
        *   Set `current_basic_block` to the `entry_block`.
        *   Recursively visit the function/routine body (`node.body`).
        *   After visiting the body, ensure all paths lead to an exit block or are terminated.

    *   **`visit(LabelDeclaration& node)`:**
        *   If `current_basic_block` is not empty, end it and add an edge to the new block.
        *   Create a new `BasicBlock` and associate it with `node.name`.
        *   Add this block to `label_targets`.
        *   Set `current_basic_block` to this new block.
        *   Recursively visit `node.command`.

    *   **`visit(IfStatement& node)` / `visit(UnlessStatement& node)` / `visit(TestStatement& node)`:**
        *   Process the condition (no new block, just expression evaluation).
        *   End `current_basic_block`.
        *   Create new basic blocks for the `then_branch` and `else_branch` (if `TestStatement`).
        *   Add edges from the old `current_basic_block` to these new blocks.
        *   Recursively visit the branches.
        *   Create a "join" block after the conditional, and add edges from the end of each branch to this join block. Set `current_basic_block` to this join block.

    *   **`visit(WhileStatement& node)` / `visit(UntilStatement& node)` / `visit(RepeatStatement& node)` / `visit(ForStatement& node)`:**
        *   Create a "loop header" block.
        *   Add an edge from `current_basic_block` to the loop header.
        *   Set `current_basic_block` to the loop header.
        *   Push appropriate `BREAK` and `LOOP` targets onto their stacks.
        *   Recursively visit the loop condition and body.
        *   Add edges from the end of the loop body back to the loop header (for iteration).
        *   Add edges from the loop header (or condition evaluation) to the block *after* the loop (for exit).
        *   Pop `BREAK` and `LOOP` targets.

    *   **`visit(GotoStatement& node)`:**
        *   End `current_basic_block`.
        *   Add an edge from `current_basic_block` to the block targeted by `node.label_expr` (resolved via `label_targets`).
        *   Set `current_basic_block` to `nullptr` (no fall-through).

    *   **`visit(ReturnStatement& node)` / `visit(FinishStatement& node)`:**
        *   End `current_basic_block`.
        *   Add an edge from `current_basic_block` to the function's conceptual `exit_block`.
        *   Set `current_basic_block` to `nullptr`.

    *   **`visit(BreakStatement& node)` / `visit(LoopStatement& node)` / `visit(EndcaseStatement& node)`:**
        *   End `current_basic_block`.
        *   Add an edge to the appropriate target from the respective stack (`break_targets`, `loop_targets`, `endcase_targets`).
        *   Set `current_basic_block` to `nullptr`.

    *   **`visit(CompoundStatement& node)` / `visit(BlockStatement& node)`:**
        *   Iterate through children. Each statement within a compound/block statement is added to `current_basic_block` unless it's a control flow statement that ends the block.

    *   **All other `visit` methods (e.g., `AssignmentStatement`, `BinaryOp`, `NumberLiteral`):**
        *   These are "straight-line" statements/expressions. They simply add themselves (or their relevant parts) to the `current_basic_block`'s statement list and recursively visit their children. They do not create new basic blocks or edges.

**Implementation Details & Challenges:**

*   **`ASTNode::replace_with` vs. CFG building:** The CFG pass will *not* modify the AST. It will only read from it to build the graph.
*   **Handling `VALOF` and `RESULTIS`:** `VALOF` expressions contain a command block, and `RESULTIS` acts like a return from this block. This requires special handling to create a sub-CFG for the `VALOF` block and connect `RESULTIS` statements to the end of that sub-CFG.
*   **Implicit Fall-through:** Many BCPL statements implicitly fall through to the next statement. The CFG builder must correctly model this by adding edges from the end of one basic block to the start of the next.
*   **Temporary Basic Blocks:** When a control flow statement is encountered, the current basic block ends. A new basic block is created for the "fall-through" path. If there's no fall-through (e.g., `GOTO`), the `current_basic_block` becomes `nullptr`.
*   **Label Resolution:** `GOTO` statements refer to labels. The CFG builder needs to make a first pass to identify all labels and their corresponding basic blocks, and then a second pass (or a deferred resolution mechanism) to connect the `GOTO` edges.
*   **Error Handling:** Detect and report unreachable code or undefined labels.
