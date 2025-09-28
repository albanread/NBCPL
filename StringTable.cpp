#include "StringTable.h"
#include <sstream>

// Constructor
StringTable::StringTable()
    : next_id_(0)
{}

// Returns the label for the string, creating it if needed
std::string StringTable::get_or_create_label(const std::string& value) {
    auto it = string_to_label_.find(value);
    if (it != string_to_label_.end()) {
        return it->second;
    }
    // Create new label
    std::ostringstream oss;
    oss << "L_str" << next_id_++;
    std::string label = oss.str();
    string_to_label_[value] = label;
    label_to_string_[label] = value;
    return label;
}

// Returns all label->string mappings (for emission)
const std::unordered_map<std::string, std::string>& StringTable::get_all_labels() const {
    return label_to_string_;
}

// Optional: clear all interned strings (for reuse/testing)
void StringTable::clear() {
    string_to_label_.clear();
    label_to_string_.clear();
    next_id_ = 0;
}