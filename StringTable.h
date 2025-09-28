#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>

/**
 * StringTable
 * 
 * Responsible for interning string literals, assigning unique labels,
 * and providing access to all interned strings for code generation.
 */
class StringTable {
public:
    StringTable();

    // Returns the label for the string, creating it if needed
    std::string get_or_create_label(const std::string& value);

    // Returns the label for a string, or empty string if not present
    std::string get_label(const std::string& value) const;

    // Returns all interned strings and their labels (label -> value)
    const std::unordered_map<std::string, std::string>& get_all_labels() const;

    // Returns all interned strings and their labels (value -> label)
    const std::unordered_map<std::string, std::string>& get_all_string_to_label() const;

    // For testing or codegen: get all labels in insertion order
    std::vector<std::pair<std::string, std::string>> get_labels_in_order() const;

    void clear();

private:
    uint64_t next_id_;
    std::unordered_map<std::string, std::string> string_to_label_; // value -> label
    std::unordered_map<std::string, std::string> label_to_string_; // label -> value
};
