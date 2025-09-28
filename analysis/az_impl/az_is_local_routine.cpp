#include "../ASTAnalyzer.h"
#include "../../NameMangler.h"

/**
 * @brief Checks if a given name corresponds to a known user-defined routine.
 * @param name The name to check.
 * @return True if the name is in the set of discovered local routine names.
 */
bool ASTAnalyzer::is_local_routine(const std::string& name) const {
    // First check if the name is directly in the set (for global routines or already mangled names)
    if (local_routine_names_.count(name) > 0) {
        return true;
    }
    
    // If we're in a class context and the name is not already qualified, check the mangled version
    if (!current_class_name_.empty() && !NameMangler::isQualifiedName(name)) {
        std::string mangled_name = NameMangler::mangleMethod(current_class_name_, name);
        return local_routine_names_.count(mangled_name) > 0;
    }
    
    return false;
}
