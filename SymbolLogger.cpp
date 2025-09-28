#include "SymbolLogger.h"
#include <iostream>

SymbolLogger& SymbolLogger::getInstance() {
    static SymbolLogger instance;
    return instance;
}

void SymbolLogger::logSymbol(const Symbol& symbol) {
    symbol_log_.push_back(symbol);
}

void SymbolLogger::clearLog() {
    symbol_log_.clear();
}

void SymbolLogger::dumpLog() const {
    std::cout << "\n--- Persistent Symbol Log ---\n";
    std::cout << "=============================\n";
    if (symbol_log_.empty()) {
        std::cout << "(No symbols were logged)\n";
    } else {
        for (const auto& symbol : symbol_log_) {
            std::cout << "Logged: " << symbol.to_string() << std::endl;
        }
    }
    std::cout << "=============================\n";
}
