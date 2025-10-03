#ifndef DATA_GENERATOR_H
#define DATA_GENERATOR_H

#include "StringTable.h"

#include "SymbolTable.h"
#include "AST.h"
#include "DataTypes.h"
#include "ClassTable.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <memory>

// Forward declaration for AssemblerData
class AssemblerData;

class InstructionStream;
class LabelManager; // Forward declaration

class DataGenerator {
public:
    void set_string_table(StringTable* table) { string_table_ = table; }
    // --- Struct Definitions for Literals ---

    void set_class_table(ClassTable* ct);

    struct StaticVariableInfo {
        std::string label;
        ExprPtr initializer;
    };

    struct TableInfo {
        std::string label;
        std::vector<int64_t> values;
    };

    struct StringLiteralInfo {
        std::string label;
        std::u32string value;
    };

    // Hash function for std::tuple to use in unordered_map for quads
    struct QuadHash {
        std::size_t operator()(const std::tuple<int64_t, int64_t, int64_t, int64_t>& q) const {
            auto h1 = std::hash<int64_t>{}(std::get<0>(q));
            auto h2 = std::hash<int64_t>{}(std::get<1>(q));
            auto h3 = std::hash<int64_t>{}(std::get<2>(q));
            auto h4 = std::hash<int64_t>{}(std::get<3>(q));
            return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
        }
    };

    // Hash function for std::pair to use in unordered_map
    struct PairHash {
        std::size_t operator()(const std::pair<int64_t, int64_t>& p) const {
            return std::hash<int64_t>()(p.first) ^ (std::hash<int64_t>()(p.second) << 1);
        }
    };

    struct FloatLiteralInfo {
        std::string label;
        double value;
    };

    struct FloatTableLiteralInfo {
        std::string label;
        std::vector<double> values;
    };

    // A node within a list literal template
    struct ListLiteralNode {
        std::string node_label;
        int32_t type_tag;
        uint64_t value_bits;      // For integers or float bits
        std::string value_ptr_label; // For strings or nested lists
        bool value_is_ptr;
        std::string next_node_label; // Label of the next node, or empty if null
    };

    // Holds all info for a complete list literal template
    struct ListLiteralInfo {
        std::string header_label;
        std::vector<ListLiteralNode> nodes;
        size_t length;
    };

    struct PairLiteralInfo {
        std::string label;
        int64_t first_value;
        int64_t second_value;
    };

    struct QuadLiteralInfo {
        std::string label;
        int64_t first_value;
        int64_t second_value;
        int64_t third_value;
        int64_t fourth_value;
    };

    // Memoization: map canonical string key to generated header label
    std::unordered_map<std::string, std::string> list_literal_label_map;


    DataGenerator(bool enable_tracing = false, bool trace_vtables = false);

    void set_symbol_table(SymbolTable* table) { symbol_table_ = table; }

    // --- Public Methods for Adding Data ---

    std::string add_string_literal(const std::string& value);
    std::string add_float_literal(double value);
    std::string add_pair_literal(int64_t first_value, int64_t second_value);
    std::string add_quad_literal(int64_t first_value, int64_t second_value, int64_t third_value, int64_t fourth_value);

    // Emit all interned strings from the string table
    void emit_interned_strings();
    void add_global_variable(const std::string& name, ExprPtr initializer);
    std::string add_table_literal(const std::vector<ExprPtr>& initializers);
    std::string add_float_table_literal(const std::vector<ExprPtr>& initializers);
    std::string add_list_literal(const ListExpression* node);

    // --- Public Methods for Generating Sections ---

    void generate_rodata_section(InstructionStream& stream);
    void generate_data_section(InstructionStream& stream);

    // --- Public Methods for Analysis and Code Generation Support ---

    void calculate_global_offsets();
    size_t get_global_word_offset(const std::string& name) const;
    bool is_global_variable(const std::string& name) const;

    // --- Public Methods for Debug Listings ---

    std::string generate_rodata_listing(const LabelManager& label_manager);
    std::string generate_data_listing(const LabelManager& label_manager, void* data_base_address);
    void populate_data_segment(void* data_base_address, const LabelManager& label_manager);

    // Display a single literal list in human-readable form
    std::string display_literal_list(const ListLiteralInfo& list_info) const;

private:
    StringTable* string_table_ = nullptr;
    SymbolTable* symbol_table_ = nullptr;
    ClassTable* class_table_ = nullptr; // New: ClassTable reference
    bool enable_tracing_;
    bool trace_vtables_ = false;

    // --- Member Variables for Storing Literals ---

    size_t next_string_id_;
    std::unordered_map<std::string, std::string> string_literal_map_;
    std::vector<StringLiteralInfo> string_literals_;

    size_t next_float_id_;
    std::unordered_map<double, std::string> float_literal_map_;
    std::vector<FloatLiteralInfo> float_literals_;

    size_t next_table_id_ = 0;
    std::vector<TableInfo> table_literals_;

    size_t next_float_table_id_ = 0;
    std::vector<FloatTableLiteralInfo> float_table_literals_;

    size_t next_list_id_ = 0;
    std::vector<ListLiteralInfo> list_literals_;

    size_t next_pair_id_ = 0;
    std::unordered_map<std::pair<int64_t, int64_t>, std::string, PairHash> pair_literal_map_;
    std::vector<PairLiteralInfo> pair_literals_;

    size_t next_quad_id_ = 0;
    std::unordered_map<std::tuple<int64_t, int64_t, int64_t, int64_t>, std::string, QuadHash> quad_literal_map_;
    std::vector<QuadLiteralInfo> quad_literals_;

    // String interning: map string content to generated label
    std::map<std::string, std::string> interned_strings_;

    // Data and RO data segments
    std::unique_ptr<AssemblerData> data_segment_;
    std::unique_ptr<AssemblerData> ro_data_segment_;

    std::vector<StaticVariableInfo> static_variables_;
    std::unordered_map<std::string, size_t> global_word_offsets_;

    void add_class_data(ClassDeclaration& node);
};

#endif // DATA_GENERATOR_H
