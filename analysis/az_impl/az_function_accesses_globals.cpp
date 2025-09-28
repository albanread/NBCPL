#include "../ASTAnalyzer.h"

/**
 * @brief Returns true if the given function accesses global variables.
 */
bool ASTAnalyzer::function_accesses_globals(const std::string& function_name) const {
    auto it = function_metrics_.find(function_name);
    if (it != function_metrics_.end()) {
        return it->second.accesses_globals;
    }
    return false;
}
