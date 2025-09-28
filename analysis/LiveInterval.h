#pragma once

#include <string>
#include "../DataTypes.h"

// Represents the lifetime of a variable from its first definition to its last use.
struct LiveInterval {
    std::string var_name;
    int start_point = -1; // Instruction number of the first appearance
    int end_point = -1;   // Instruction number of the last use
    VarType var_type = VarType::INTEGER; // Variable type to prevent pool corruption

    // --- Allocation Result ---
    bool is_spilled = false;
    std::string assigned_register; // Will be empty if spilled
    int stack_offset = -1;         // Spill location, relative to the frame pointer

    LiveInterval() = default;
    LiveInterval(const std::string& name, int start, int end)
        : var_name(name), start_point(start), end_point(end) {}
    LiveInterval(const std::string& name, int start, int end, VarType type)
        : var_name(name), start_point(start), end_point(end), var_type(type) {}
    
    // Equality operator for std::list::remove compatibility
    bool operator==(const LiveInterval& other) const {
        return var_name == other.var_name && 
               start_point == other.start_point && 
               end_point == other.end_point;
    }
};