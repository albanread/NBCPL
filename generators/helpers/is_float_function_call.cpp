#include "../../NewCodeGenerator.h"
#include "../../analysis/ASTAnalyzer.h"
#include "../RuntimeManager.h"
#include <string>

bool NewCodeGenerator::is_float_function_call(FunctionCall& node) {
    if (auto* var_access = dynamic_cast<VariableAccess*>(node.function_expr.get())) {
        const std::string& func_name = var_access->name;
        auto& return_types = ASTAnalyzer::getInstance().get_function_return_types();
        
        // Debug output
        // std::cerr << "[DEBUG] is_float_function_call checking: " << func_name << std::endl;
        // std::cerr << "[DEBUG] return_types map size: " << return_types.size() << std::endl;
        // for (const auto& pair : return_types) {
        //     std::cerr << "[DEBUG]   " << pair.first << " -> " << (pair.second == VarType::FLOAT ? "FLOAT" : "INTEGER") << std::endl;
        // }
        
        auto it = return_types.find(func_name);
        if (it != return_types.end()) {
            bool is_float = it->second == VarType::FLOAT;
            // std::cerr << "[DEBUG] Found " << func_name << " in return_types, is_float=" << is_float << std::endl;
            return is_float;
        }
        // std::cerr << "[DEBUG] " << func_name << " NOT found in return_types" << std::endl;
        
        if (RuntimeManager::instance().is_function_registered(func_name)) {
            bool is_float = RuntimeManager::instance().get_function(func_name).type == FunctionType::FLOAT;
            // std::cerr << "[DEBUG] Found " << func_name << " in RuntimeManager, is_float=" << is_float << std::endl;
            return is_float;
        }
        // std::cerr << "[DEBUG] " << func_name << " NOT found in RuntimeManager" << std::endl;
    }
    return false;
}