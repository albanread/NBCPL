#include "AssemblerData.h"
#include <stdexcept>

AssemblerData::AssemblerData(const std::string& name, const std::string& directive)
    : name_(name), directive_(directive), current_offset_(0) {}

void AssemblerData::add_label(const std::string& label) {
    if (label_offsets_.count(label)) {
        throw std::runtime_error("Duplicate label: " + label);
    }
    DataEntry entry;
    entry.type = DataEntry::LABEL;
    entry.label_name = label;
    entry.offset = current_offset_;
    entry.size = 0; // Labels don't consume space directly
    data_entries_.push_back(entry);
    label_offsets_[label] = current_offset_;
}

void AssemblerData::add_long(uint64_t value) {
    DataEntry entry;
    entry.type = DataEntry::LONG;
    entry.long_value = value;
    entry.offset = current_offset_;
    entry.size = sizeof(uint64_t);
    data_entries_.push_back(entry);
    current_offset_ += entry.size;
}

void AssemblerData::add_utf32_string(const std::u32string& value) {
    DataEntry entry;
    entry.type = DataEntry::UTF32_STRING;
    entry.utf32_string_value = value;
    entry.offset = current_offset_;
    // Each char32_t is 4 bytes, plus a null terminator
    entry.size = (value.length() + 1) * sizeof(char32_t);
    data_entries_.push_back(entry);
    current_offset_ += entry.size;
}

void AssemblerData::add_byte_array(const std::vector<uint8_t>& bytes) {
    DataEntry entry;
    entry.type = DataEntry::BYTE_ARRAY;
    entry.byte_array_value = bytes;
    entry.offset = current_offset_;
    entry.size = bytes.size();
    data_entries_.push_back(entry);
    current_offset_ += entry.size;
}

size_t AssemblerData::get_label_offset(const std::string& label) const {
    auto it = label_offsets_.find(label);
    if (it != label_offsets_.end()) {
        return it->second;
    }
    throw std::runtime_error("Label not found: " + label);
}
