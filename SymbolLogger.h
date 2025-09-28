#ifndef SYMBOLLOGGER_H
#define SYMBOLLOGGER_H

#include "Symbol.h"
#include <vector>
#include <string>

class SymbolLogger {
public:
    // Singleton access
    static SymbolLogger& getInstance();

    // Log a symbol (append to the persistent log)
    void logSymbol(const Symbol& symbol);

    // Print the entire persistent log to stdout
    void dumpLog() const;

    // Clear the persistent log (for a fresh compilation run)
    void clearLog();

private:
    SymbolLogger() = default;
    SymbolLogger(const SymbolLogger&) = delete;
    SymbolLogger& operator=(const SymbolLogger&) = delete;

    std::vector<Symbol> symbol_log_;
};

#endif // SYMBOLLOGGER_H