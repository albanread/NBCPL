#include "SymbolTable.h"
#include "SymbolLogger.h"
#include "DataTypes.h"
#include <iostream>
#include <sstream>
#include <algorithm>

SymbolTable::SymbolTable()
    : current_scope_level_(0), current_function_name_("Global") {}

void SymbolTable::enterScope() {
    ++current_scope_level_;
}

void SymbolTable::exitScope() {
    if (current_scope_level_ > 0) {
        --current_scope_level_;
    } else {
        std::cerr << "Warning: Attempting to exit global scope" << std::endl;
    }
}

void SymbolTable::setCurrentFunction(const std::string& function_name) {
    current_function_name_ = function_name;
}

bool SymbolTable::addSymbol(const Symbol& symbol) {
    // Check for redefinition in the current scope
    for (auto it = all_symbols_.rbegin(); it != all_symbols_.rend(); ++it) {
        if (it->name == symbol.name && it->scope_level == current_scope_level_ && it->function_name == current_function_name_) {
            // Redefinition in the same scope and function
            return false;
        }
    }
    all_symbols_.push_back(symbol);
    SymbolLogger::getInstance().logSymbol(symbol);
    return true;
}

bool SymbolTable::lookup(const std::string& name, Symbol& symbol) const {
    // Search from innermost to outermost scope
    for (auto it = all_symbols_.rbegin(); it != all_symbols_.rend(); ++it) {
        if (it->name == name) {
            symbol = *it;
            return true;
        }
    }
    // Add trace message on failure
    if (g_enable_symbols_trace) {
        std::cout << "[SYMBOL TABLE TRACE] Lookup FAILED for symbol: '" << name << "'" << std::endl;
    }
    return false;
}

bool SymbolTable::lookup(const std::string& name, const std::string& function_name, Symbol& symbol) const {
    // CORRECTED FIX: Prioritize the requested function context first
    // This ensures we find the correct symbol in the proper scope
    
    // 1. First, search in the requested function context
    for (auto it = all_symbols_.rbegin(); it != all_symbols_.rend(); ++it) {
        if (it->name == name && it->function_name == function_name) {
            symbol = *it;
            return true;
        }
    }
    
    // 2. If not found in the requested context, search in Global scope
    for (auto it = all_symbols_.rbegin(); it != all_symbols_.rend(); ++it) {
        if (it->name == name && it->function_name == "Global") {
            symbol = *it;
            return true;
        }
    }
    
    // 3. Only as a last resort, search in other local function contexts
    for (auto it = all_symbols_.rbegin(); it != all_symbols_.rend(); ++it) {
        if (it->name == name && it->function_name != "Global" && it->function_name != function_name) {
            symbol = *it;
            if (g_enable_symbols_trace) {
                std::cout << "[SYMBOL TABLE TRACE] Warning: Found symbol '" << name << "' in different context '" 
                         << it->function_name << "' (requested context was '" << function_name << "')" << std::endl;
            }
            return true;
        }
    }
    
    // Add trace message on failure
    if (g_enable_symbols_trace) {
        std::cout << "[SYMBOL TABLE TRACE] Lookup FAILED for symbol: '" << name << "' in function context '" << function_name << "'" << std::endl;
    }
    
    // Debug: Show what symbols DO exist for this name in other contexts
    bool found_in_other_context = false;
    if (g_enable_symbols_trace) {
        for (auto it = all_symbols_.rbegin(); it != all_symbols_.rend(); ++it) {
            if (it->name == name) {
                std::cout << "[SYMBOL TABLE TRACE]   Found '" << name << "' in function context '" << it->function_name << "' (scope level " << it->scope_level << ")" << std::endl;
                found_in_other_context = true;
            }
        }
        if (!found_in_other_context) {
            std::cout << "[SYMBOL TABLE TRACE]   Symbol '" << name << "' not found in ANY context" << std::endl;
        }
    }
    
    return false;
}

/**
 * @brief Retrieves a list of all symbols defined at a specific scope level.
 * This is used by the code generator to identify local variables for cleanup
 * when a scope is exited.
 * @param scope_level The integer level of the scope to query.
 * @return A vector of Symbol objects defined at that level.
 */
std::vector<Symbol> SymbolTable::getSymbolsInScope(int scope_level) const {
    std::vector<Symbol> symbols;
    for (const auto& symbol : all_symbols_) {
        if (symbol.scope_level == scope_level) {
            symbols.push_back(symbol);
        }
    }
    return symbols;
}

bool SymbolTable::updateSymbol(const std::string& name, const Symbol& new_symbol_data) {
    for (auto it = all_symbols_.rbegin(); it != all_symbols_.rend(); ++it) {
        if (it->name == name) {
            // Check type priority before updating
            if (should_update_type(it->type, new_symbol_data.type)) {
                *it = new_symbol_data;
                SymbolLogger::getInstance().logSymbol(*it);
                return true;
            } else {
                return false;
            }
        }
    }
    return false;
}

bool SymbolTable::updateSymbolType(const std::string& name, VarType type) {
    for (auto it = all_symbols_.rbegin(); it != all_symbols_.rend(); ++it) {
        if (it->name == name) {
            // Check type priority before updating
            if (should_update_type(it->type, type)) {
                it->type = type;
                SymbolLogger::getInstance().logSymbol(*it);
                return true;
            } else {
                return false;
            }
        }
    }
    return false;
}

bool SymbolTable::updateFunctionParameterType(const std::string& function_name, size_t param_index, VarType type) {
    for (auto& symbol : all_symbols_) {
        if (symbol.name == function_name && symbol.is_function_like()) {
            if (param_index < symbol.parameters.size()) {
                symbol.parameters[param_index].type = type;
                SymbolLogger::getInstance().logSymbol(symbol);
                return true;
            }
        }
    }
    return false;
}

void SymbolTable::setSymbolType(const std::string& name, VarType type) {
    Symbol symbol;
    if (lookup(name, symbol)) {
        // Check type priority before updating
        if (should_update_type(symbol.type, type)) {
            symbol.type = type;
            updateSymbol(name, symbol);
        }
    }
}

void SymbolTable::setSymbolStackLocation(const std::string& name, int offset) {
    for (auto it = all_symbols_.rbegin(); it != all_symbols_.rend(); ++it) {
        if (it->name == name) {
            it->location = SymbolLocation::stack(offset);
            break;
        }
    }
}

void SymbolTable::setSymbolDataLocation(const std::string& name, size_t offset) {
    for (auto it = all_symbols_.rbegin(); it != all_symbols_.rend(); ++it) {
        if (it->name == name) {
            it->location = SymbolLocation::data(offset);
            break;
        }
    }
}

void SymbolTable::setSymbolAbsoluteValue(const std::string& name, int64_t value) {
    for (auto it = all_symbols_.rbegin(); it != all_symbols_.rend(); ++it) {
        if (it->name == name) {
            it->location = SymbolLocation::absolute(value);
            break;
        }
    }
}

void SymbolTable::dumpTable() const {
    std::cout << toString() << std::endl;
}

std::string SymbolTable::toString() const {
    std::ostringstream oss;
    oss << "Symbol Table (Persistent, All Symbols)\n";
    oss << "==================================================\n";
    for (const auto& symbol : all_symbols_) {
        oss << symbol.to_string() << "\n";
    }
    return oss.str();
}

std::vector<Symbol> SymbolTable::getAllSymbols() const {
    return all_symbols_;
}