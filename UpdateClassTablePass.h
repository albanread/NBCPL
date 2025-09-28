#ifndef UPDATE_CLASS_TABLE_PASS_H
#define UPDATE_CLASS_TABLE_PASS_H

#include <string>

class ClassTable;
class ASTAnalyzer;
struct ClassMethodInfo;

/**
 * @brief Pass that updates ClassTable with inferred parameter types from ASTAnalyzer
 * 
 * This pass runs after ASTAnalyzer has completed type inference but before code generation.
 * It takes the rich type information from ASTAnalyzer's function_metrics_ and uses it to
 * update the ClassTable's ClassMethodInfo entries with accurate parameter types.
 * 
 * This solves the problem where:
 * 1. ClassPass builds ClassTable early with UNKNOWN parameter types
 * 2. ASTAnalyzer later infers actual types but doesn't update ClassTable
 * 3. SUPER call validation and code generation use stale ClassTable data
 */
class UpdateClassTablePass {
public:
    /**
     * @brief Updates ClassTable with inferred parameter types from ASTAnalyzer
     * @param class_table The ClassTable to update
     * @param analyzer The ASTAnalyzer containing inferred type information
     */
    void run(ClassTable& class_table, const ASTAnalyzer& analyzer);

private:
    /**
     * @brief Updates a single method's parameter information
     * @param method_info The ClassMethodInfo to update
     * @param mangled_name The mangled name to look up in analyzer metrics
     * @param analyzer The ASTAnalyzer containing type information
     * @return true if the method was successfully updated
     */
    bool update_method_parameters(ClassMethodInfo& method_info, 
                                 const std::string& mangled_name,
                                 const ASTAnalyzer& analyzer);
};

#endif // UPDATE_CLASS_TABLE_PASS_H