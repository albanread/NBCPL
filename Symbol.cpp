#include "Symbol.h"
#include "DataTypes.h"
#include <sstream>
#include <iostream>
 

// Smart constructor: parses class_name from qualified name and warns if missing for class methods
Symbol::Symbol(const std::string& name, SymbolKind kind, VarType type, int scope_level, const std::string& function_name)
    : name(name), kind(kind), type(type), scope_level(scope_level), block_id(0), function_name(function_name), has_size(false), size(0) {
    size_t separator_pos = name.find("::");
    if (separator_pos != std::string::npos) {
        this->class_name = name.substr(0, separator_pos);
        if (this->class_name.empty()) {
            std::cerr << "[Compiler Warning] Symbol '" << name 
                      << "' appears to be a class method but failed to parse a class name." << std::endl;
        }
    }
}

// Constructor with block_id
Symbol::Symbol(const std::string& name, SymbolKind kind, VarType type, int scope_level, int block_id, const std::string& function_name)
    : name(name), kind(kind), type(type), scope_level(scope_level), block_id(block_id), function_name(function_name), has_size(false), size(0) {
    size_t separator_pos = name.find("::");
    if (separator_pos != std::string::npos) {
        this->class_name = name.substr(0, separator_pos);
        if (this->class_name.empty()) {
            std::cerr << "[Compiler Warning] Symbol '" << name 
                      << "' appears to be a class method but failed to parse a class name." << std::endl;
        }
    }
}

std::string Symbol::to_string() const {
   std::ostringstream oss;
    
    // Symbol name and kind
    oss << "Symbol '" << name << "' (";
    
    // Add class name if it exists
    if (!class_name.empty()) {
        oss << "class: " << class_name << ", ";
    }
    
    switch (kind) {
        case SymbolKind::LOCAL_VAR:
            oss << "LOCAL_VAR";
            break;
        case SymbolKind::STATIC_VAR:
            oss << "STATIC_VAR";
            break;
        case SymbolKind::GLOBAL_VAR:
            oss << "GLOBAL_VAR";
            break;
        case SymbolKind::MEMBER_VAR:
            oss << "MEMBER_VAR";
            break;
        case SymbolKind::PARAMETER:
            oss << "PARAMETER";
            break;
        case SymbolKind::FUNCTION:
            oss << "FUNCTION";
            break;
        case SymbolKind::FLOAT_FUNCTION:
            oss << "FLOAT_FUNCTION";
            break;
        case SymbolKind::ROUTINE:
            oss << "ROUTINE";
            break;
        case SymbolKind::LABEL:
            oss << "LABEL";
            break;
        case SymbolKind::MANIFEST:
            oss << "MANIFEST";
            break;
        case SymbolKind::RUNTIME_FUNCTION:
            oss << "RUNTIME_FUNCTION";
            break;
        case SymbolKind::RUNTIME_FLOAT_FUNCTION:
            oss << "RUNTIME_FLOAT_FUNCTION";
            break;
        case SymbolKind::RUNTIME_ROUTINE:
            oss << "RUNTIME_ROUTINE";
            break;
        case SymbolKind::RUNTIME_FLOAT_ROUTINE:
            oss << "RUNTIME_FLOAT_ROUTINE";
            break;
    }
    
    // Add type information
    oss << ", ";
    oss << vartype_to_string(type);
    
    // Add scope level and block ID
    oss << ", scope=" << scope_level << ", block=" << block_id;
    
    // Add location information
    if (location.type != SymbolLocation::LocationType::UNKNOWN) {
        oss << ", location=";
        switch (location.type) {
            case SymbolLocation::LocationType::STACK:
                oss << "STACK[FP+" << location.stack_offset << "]";
                break;
            case SymbolLocation::LocationType::DATA:
                oss << "DATA[" << location.data_offset << "]";
                break;
            case SymbolLocation::LocationType::ABSOLUTE:
                oss << "ABSOLUTE[" << location.absolute_value << "]";
                break;
            case SymbolLocation::LocationType::LABEL:
                oss << "LABEL";
                break;
            case SymbolLocation::LocationType::UNKNOWN:
                oss << "UNKNOWN";
                break;
        }
    }
    
    // Add size information for arrays if available
    if (has_size) {
        oss << ", size=" << size;
    }

    // --- ALWAYS SHOW CLASS NAME ---
    oss << ", class_name='" << class_name << "'";
    // --- END ADDITION ---

    // --- ALWAYS SHOW FUNCTION NAME ---
    oss << ", function_name='" << function_name << "'";
    // --- END ADDITION ---

    // --- SHOW PARAMETERS FOR FUNCTION-LIKE SYMBOLS ---
    if (is_function_like() && !parameters.empty()) {
        oss << ", parameters=[";
        for (size_t i = 0; i < parameters.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << vartype_to_string(parameters[i].type);
            if (parameters[i].is_optional) oss << "?";
        }
        oss << "]";
    }
    // --- END PARAMETERS ---

    oss << ")";
    return oss.str();
}
