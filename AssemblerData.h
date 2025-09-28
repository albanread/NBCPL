#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>

class AssemblerData {
public:
    struct DataEntry {
        enum Type { LABEL, LONG, UTF32_STRING, BYTE_ARRAY };
        Type type;
        std::string label_name; // For LABEL type
        uint64_t long_value;    // For LONG type
        std::u32string utf32_string_value; // For UTF32_STRING type
        std::vector<uint8_t> byte_array_value; // For BYTE_ARRAY type
        size_t offset; // Offset within the segment
        size_t size;   // Size of the data
    };

    explicit AssemblerData(const std::string& name, const std::string& directive = "");

    void add_label(const std::string& label);
    void add_long(uint64_t value);
    void add_utf32_string(const std::u32string& value);
    void add_byte_array(const std::vector<uint8_t>& bytes);

    const std::string& get_name() const { return name_; }
    const std::string& get_directive() const { return directive_; }
    const std::vector<DataEntry>& get_data_entries() const { return data_entries_; }
    size_t get_current_offset() const { return current_offset_; }

    // Get the offset of a label within this segment
    size_t get_label_offset(const std::string& label) const;

private:
    std::string name_;
    std::string directive_;
    std::vector<DataEntry> data_entries_;
    std::unordered_map<std::string, size_t> label_offsets_;
    size_t current_offset_;
};
