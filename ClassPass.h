#ifndef CLASSPASS_H
#define CLASSPASS_H

#include "AST.h"
#include "ClassTable.h"
#include "SymbolTable.h"
#include "ASTVisitor.h"
#include "HeapManager/HeapManager.h"
#include <string>
#include <unordered_map>
#include <unordered_set>

/**
 * @class ClassPass
 * @brief Performs a two-phase pass over the AST to populate and finalize the ClassTable.
 *
 * Phase 1 (Discovery): Traverses the AST to find all ClassDeclaration nodes,
 * creating basic entries in the ClassTable and mapping class names to AST nodes.
 * Phase 2 (Finalization): Resolves inheritance, calculates memory layouts, vtable layouts,
 * and links special methods (CREATE/RELEASE).
 * * Debug capabilities can be enabled to help diagnose layout and inheritance issues.
 */
class ClassPass : public ASTVisitor {
public:
    /**
     * @brief Constructs the ClassPass with references to the tables it will populate.
     * @param class_table Reference to the ClassTable to populate.
     * @param symbol_table Reference to the SymbolTable (for future use).
     */
    ClassPass(ClassTable& class_table, SymbolTable& symbol_table);

    /**
     * @brief Main entry point to execute the pass.
     * @param program The root Program node of the AST.
     */
    void run(Program& program);

    /**
     * @brief Enable debug tracing for the class pass
     * @param enable True to enable debugging, false to disable
     */
    void set_debug(bool enable) { trace_enabled_ = enable; }

    /**
     * @brief Validate the memory layout of all finalized classes
     * @return True if all classes have valid memory layouts, false otherwise
     */
    bool validate_all_class_layouts() const;
    
    /**
     * @brief Process a class method and update the vtable entries accordingly
     * @param entry The class table entry to update
     * @param method_name The name of the method
     * @param is_routine Whether the method is a routine (void return)
     * @param is_synthetic Whether this is a synthetic method (default: false)
     * @param is_virtual Whether the method is virtual
     * @param is_final Whether the method is final
     */
    void process_method(ClassTableEntry* entry, const std::string& method_name, bool is_routine, bool is_synthetic = false, bool is_virtual = false, bool is_final = false, Visibility visibility = Visibility::Public);

private:
    void inject_superclass_release_calls(ClassDeclaration* class_node, ClassTableEntry* entry);
    
    /**
     * @brief Inject initializers from let declarations into the CREATE method
     * @param class_node The class AST node
     * @param class_name The name of the class
     */
    void inject_initializers(ClassDeclaration* class_node, const std::string& class_name);

    // Visitor methods for AST traversal (Discovery phase)
    void visit(Program& node) override;
    void visit(ClassDeclaration& node) override;
    
    // Visitor methods for AST traversal (Finalization phase - methods only)
    void visit(FunctionDeclaration& node) override;
    void visit(RoutineDeclaration& node) override;

private:
    // Phase 1: Discover all classes in the AST.
    void discover_classes(Program& program);

    // Phase 2: Resolve inheritance and calculate memory/vtable layouts.
    void finalize_layouts();

    // Helper: Recursively finalize a single class and its parents.
    void finalize_class_layout(const std::string& class_name, std::unordered_set<std::string>& resolution_path);

    // Debug helper: Validate a single class's memory layout
    bool validate_class_layout(const std::string& class_name) const;
    
    // Debug helper: Log memory layout for a class
    void log_class_memory_layout(const std::string& class_name) const;

    ClassTable& class_table_;
    SymbolTable& symbol_table_;
    bool trace_enabled_ = false;
    std::string current_class_name_; // Track current class during traversal

    // Map from class name to its AST node for use in finalization.
    std::unordered_map<std::string, ClassDeclaration*> class_ast_nodes_;
};

#endif // CLASSPASS_H
