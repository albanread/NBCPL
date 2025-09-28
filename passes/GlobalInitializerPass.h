#ifndef GLOBALINITIALIZERPASS_H
#define GLOBALINITIALIZERPASS_H

#include "../AST.h"
#include "../ASTVisitor.h"
#include <vector>
#include <memory>

/**
 * @brief Pass that transforms GLOBALS block declarations into a Global routine.
 * 
 * This pass:
 * 1. Finds all GlobalVariableDeclaration nodes with initializers
 * 2. Extracts the initializers and creates assignment statements
 * 3. Creates a Global routine containing these assignments
 * 4. Injects a call to Global at the start of the START routine
 * 5. Clears the initializers from the original declarations
 * 
 * Example transformation:
 * GLOBALS $(
 *   LET global_int = 123
 *   FLET global_float = 3.14
 * $)
 * 
 * Becomes:
 * LET Global() BE $(
 *   global_int := 123
 *   global_float := 3.14
 * $)
 * 
 * And START gets modified to call Global first.
 */
class GlobalInitializerPass : public ASTVisitor {
public:
    GlobalInitializerPass() = default;
    ~GlobalInitializerPass() = default;

    /**
     * @brief Main entry point for the pass
     * @param program The root program AST node to transform
     */
    void run(Program& program);

    // ASTVisitor interface
    void visit(Program& node) override;
    void visit(GlobalVariableDeclaration& node) override;

private:
    /**
     * @brief Collection of assignment statements created from global initializers
     */
    std::vector<StmtPtr> collected_initializers_;

    /**
     * @brief Helper to find the START routine in the program
     * @param program The program to search
     * @return Pointer to START routine, or nullptr if not found
     */
    RoutineDeclaration* find_start_routine(Program& program);

    /**
     * @brief Helper to inject a call to Global at the start of a routine
     * @param routine The routine to modify
     */
    void inject_global_init_call(RoutineDeclaration* routine);
};

#endif // GLOBALINITIALIZERPASS_H