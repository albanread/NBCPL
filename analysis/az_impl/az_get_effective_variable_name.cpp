#include "../ASTAnalyzer.h"
#include <stack>
#include <map>
#include <string>

/**
 * @brief Resolves the effective variable name, handling FOR loop variable renaming.
 * @param original_name The original variable name.
 * @return The effective (possibly renamed) variable name.
 */
std::string ASTAnalyzer::get_effective_variable_name(const std::string& original_name) const {
    // DEBUG: Add extensive logging to track corruption
    if (trace_enabled_) {
        std::cout << "[DEBUG get_effective_variable_name] Called with: '" << original_name << "'" << std::endl;
        std::cout << "[DEBUG get_effective_variable_name] Loop context stack size: " << loop_context_stack_.size() << std::endl;
        std::cout << "[DEBUG get_effective_variable_name] Active FOR loop scopes size: " << active_for_loop_scopes_.size() << std::endl;
        std::cout << "[DEBUG get_effective_variable_name] FOR variable aliases size: " << for_variable_unique_aliases_.size() << std::endl;
    }
    
    // Only apply FOR loop variable renaming if we're currently in a FOR loop context
    bool in_for_loop_context = false;
    if (!loop_context_stack_.empty()) {
        // Check if we're currently in a FOR loop or nested within one
        std::stack<LoopContext> temp_context_stack = loop_context_stack_;
        while (!temp_context_stack.empty()) {
            if (temp_context_stack.top() == LoopContext::FOR_LOOP) {
                in_for_loop_context = true;
                break;
            }
            temp_context_stack.pop();
        }
    }

    if (trace_enabled_) {
        std::cout << "[DEBUG get_effective_variable_name] In FOR loop context: " << (in_for_loop_context ? "YES" : "NO") << std::endl;
    }

    // If not in FOR loop context, don't apply FOR loop renaming
    if (!in_for_loop_context) {
        if (trace_enabled_) {
            std::cout << "[DEBUG get_effective_variable_name] Not in FOR loop context, returning original: '" << original_name << "'" << std::endl;
        }
        return original_name;
    }

    // 1. First, check the active loop scopes for nested loops.
    // This ensures that an inner `FOR I` correctly shadows an outer `FOR I`.
    std::stack<std::map<std::string, std::string>> temp_stack = active_for_loop_scopes_;
    while (!temp_stack.empty()) {
        const auto& scope_map = temp_stack.top();
        auto it = scope_map.find(original_name);
        if (it != scope_map.end()) {
            if (trace_enabled_) {
                std::cout << "[DEBUG get_effective_variable_name] Found in active scope: '" << original_name << "' -> '" << it->second << "'" << std::endl;
            }
            return it->second; // Found in an active scope
        }
        temp_stack.pop();
    }

    // 2. If not in an active loop, check the persistent aliases map.
    // This allows access to the variable after the loop has finished.
    auto alias_it = for_variable_unique_aliases_.find(original_name);
    if (alias_it != for_variable_unique_aliases_.end()) {
        if (trace_enabled_) {
            std::cout << "[DEBUG get_effective_variable_name] Found in persistent aliases: '" << original_name << "' -> '" << alias_it->second << "'" << std::endl;
        }
        return alias_it->second; // Return the last known unique name for this variable
    }

    // 3. If it was never a loop variable, return its original name.
    if (trace_enabled_) {
        std::cout << "[DEBUG get_effective_variable_name] No renaming needed, returning: '" << original_name << "'" << std::endl;
    }
    return original_name;
}
