#include "../ASTAnalyzer.h"

/**
 * @brief Returns true if the analyzer determined the function is a leaf (makes no calls).
 */
bool ASTAnalyzer::is_leaf_function(const std::string& function_name) const {
    auto it = function_metrics_.find(function_name);
    if (it != function_metrics_.end()) {
        return it->second.is_leaf;
    }
    // Default to false (non-leaf) if metrics are not found, as this is the safe assumption.
    return false;
}
