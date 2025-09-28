#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "Symbol.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <stack>
#include <memory>

// Global symbols trace flag (set early in main.cpp)
extern bool g_enable_symbols_trace;

// The SymbolTable class manages all symbols persistently.
// It holds a single vector of all symbols ever declared.
class SymbolTable {
public:
    SymbolTable();
    ~SymbolTable() = default;

    // Scope management
    void enterScope();
    void exitScope();
    int currentScopeLevel() const { return current_scope_level_; }
    int getCurrentScopeLevel() const { return current_scope_level_; }

    /**
     * @brief Retrieves a list of all symbols defined at a specific scope level.
     * This is used by the code generator to identify local variables for cleanup
     * when a scope is exited.
     * @param scope_level The integer level of the scope to query.
     * @return A vector of Symbol objects defined at that level.
     */
    std::vector<Symbol> getSymbolsInScope(int scope_level) const;

    // Symbol manipulation
    bool addSymbol(const Symbol& symbol);
    bool updateSymbolType(const std::string& name, VarType type);
    bool updateFunctionParameterType(const std::string& function_name, size_t param_index, VarType type);

    // Symbol lookup
    bool lookup(const std::string& name, Symbol& symbol) const;
    // Lookup by name and function context
    bool lookup(const std::string& name, const std::string& function_name, Symbol& symbol) const;

    // Context management
    void setCurrentFunction(const std::string& function_name);
    std::string getCurrentFunction() const { return current_function_name_; }

    // Legacy wrappers for location/type updates (persistent model)
    void setSymbolDataLocation(const std::string& name, size_t offset);
    void setSymbolAbsoluteValue(const std::string& name, int64_t value);
    void setSymbolStackLocation(const std::string& name, int offset);
    void setSymbolType(const std::string& name, VarType type);

    // Iteration and utility
    std::vector<Symbol> getAllSymbols() const;
    void dumpTable() const;
    std::string toString() const;
    bool updateSymbol(const std::string& name, const Symbol& new_symbol_data);

private:
    // Persistent list of all symbols ever declared
    std::vector<Symbol> all_symbols_;

    // Current scope level (0 = global)
    int current_scope_level_ = 0;

    // Track the current function for context
    std::string current_function_name_ = "Global";
};


#endif // SYMBOL_TABLE_H