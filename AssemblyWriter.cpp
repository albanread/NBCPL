#include "AssemblyWriter.h"
#include "DataGenerator.h"
#include "Encoder.h"
#include "LabelManager.h"
#include "RuntimeManager.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Helper to check if a character can be part of a label name.
static bool is_label_char(char c) {
    return isalnum(c) || c == '_' || c == '.';
}

// Helper to safely replace all whole-word occurrences of a label.
static void replace_label_in_text(std::string& text, const std::string& old_label, const std::string& new_label) {
    size_t pos = 0;
    while ((pos = text.find(old_label, pos)) != std::string::npos) {
        bool left_ok = (pos == 0) || !is_label_char(text[pos - 1]);
        size_t end_pos = pos + old_label.length();
        bool right_ok = (end_pos == text.length()) || !is_label_char(text[end_pos]);

        if (left_ok && right_ok) {
            text.replace(pos, old_label.length(), new_label);
            pos += new_label.length();
        } else {
            pos += 1;
        }
    }
}

// Helper to extract function name and register from a JIT address-loading comment.
static std::string extract_load_addr_info(const std::string& text, std::string& out_reg) {
    const std::string prefix = "; load ";
    const std::string suffix = " addr";
    size_t comment_pos = text.find(prefix);
    if (comment_pos == std::string::npos) return "";

    size_t func_start = comment_pos + prefix.length();
    size_t func_end = text.find(suffix, func_start);
    if (func_end == std::string::npos) return "";

    size_t instr_end = text.find(' ');
    if (instr_end == std::string::npos) return "";
    size_t reg_start = text.find("X", instr_end);
    if (reg_start == std::string::npos) return "";
    size_t reg_end = text.find(",", reg_start);
    if (reg_end == std::string::npos) return "";

    out_reg = text.substr(reg_start, reg_end - reg_start);
    size_t last_char = out_reg.find_last_not_of(" \t\n\r");
    if (std::string::npos != last_char) {
        out_reg.erase(last_char + 1);
    }
    return text.substr(func_start, func_end - func_start);
}

// Convert :: syntax to clang-compatible label names
static std::string make_clang_compatible_label(const std::string& label) {
    std::string result = label;
    size_t pos = 0;
    while ((pos = result.find("::", pos)) != std::string::npos) {
        result.replace(pos, 2, "_");
        pos += 1;
    }
    return result;
}

AssemblyWriter::AssemblyWriter() {}

void AssemblyWriter::write_to_file(const std::string& path,
                                   const std::vector<Instruction>& instructions,
                                   const LabelManager& label_manager,
                                   const DataGenerator& data_generator,
                                   const VeneerManager& veneer_manager) {
    std::ofstream ofs(path);
    if (!ofs.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + path);
    }

    std::vector<Instruction> code_instructions;
    std::vector<Instruction> data_instructions;
    std::vector<Instruction> rodata_instructions;
    for (const auto& instr : instructions) {
        switch (instr.segment) {
            case SegmentType::RODATA:
                rodata_instructions.push_back(instr);
                break;
            case SegmentType::DATA:
                data_instructions.push_back(instr);
                break;
            case SegmentType::CODE:
            default:
                code_instructions.push_back(instr);
                break;
        }
    }

    // --- Create Veneer Lookups for Static Assembly Generation ---
    // Create a set of all veneer labels for filtering
    std::unordered_set<std::string> veneer_labels;
    // Create a map from veneer label back to original function name
    std::unordered_map<std::string, std::string> veneer_to_function_map;
    
    const auto& veneer_label_map = veneer_manager.get_veneer_labels();
    for (const auto& pair : veneer_label_map) {
        const std::string& function_name = pair.first;
        const std::string& veneer_label = pair.second;
        veneer_labels.insert(veneer_label);
        veneer_to_function_map[veneer_label] = function_name;
    }

    std::unordered_map<size_t, std::vector<std::string>> address_to_labels;
    const auto& defined_labels = label_manager.get_defined_labels();
    for (const auto& pair : defined_labels) {
        if (!label_manager.is_runtime_label(pair.first)) {
            address_to_labels[pair.second].push_back(pair.first);
        }
    }

    // --- Corrected Label Renaming ---
    std::unordered_map<std::string, std::string> label_rename_map;
    for (const auto& pair : address_to_labels) {
        for (const auto& label : pair.second) {
            if (label.rfind(".L", 0) == 0) {
                label_rename_map[label] = "L_" + label.substr(2);
            } else if (label.rfind("L_", 0) != 0 && label != "_start") {
                label_rename_map[label] = "L_" + label;
            } else {
                label_rename_map[label] = label;
            }
        
            // Special handling for vtable labels to ensure they get L_ prefix
            if (label.find("_vtable") != std::string::npos && label.rfind("L_", 0) != 0) {
                label_rename_map[label] = "L_" + label;
            }
        }
    }

    // --- START OF NEW FIX ---
    // Pre-scan to create a map of every label to its correct segment type.
    std::unordered_map<std::string, SegmentType> label_to_segment;
    for (const auto& instr : instructions) {
        if (instr.is_label_definition && !instr.target_label.empty()) {
            label_to_segment[instr.target_label] = instr.segment;
        }
    }
    // --- END OF NEW FIX ---

    // REMOVE bulk label replacement loops.

    // --- Pre-pass to find ALL external function calls ---
    std::unordered_set<std::string> external_symbols;
    for (const auto& instr : code_instructions) {
        if (instr.relocation == RelocationType::PC_RELATIVE_26_BIT_OFFSET &&
            RuntimeManager::instance().is_function_registered(instr.target_label)) {
            external_symbols.insert("_" + instr.target_label);
        }
        
        // Also add external functions that are called via veneers
        if (instr.relocation == RelocationType::PC_RELATIVE_26_BIT_OFFSET &&
            !instr.target_label.empty() &&
            veneer_to_function_map.find(instr.target_label) != veneer_to_function_map.end()) {
            const std::string& function_name = veneer_to_function_map[instr.target_label];
            external_symbols.insert("_" + function_name);
        }

        // --- JitCall handling removed - now handled by veneer system ---

        // Ensure all registered runtime routines are declared as .globl
        const auto& registered_functions = RuntimeManager::instance().get_registered_functions();
        for (const auto& func_pair : registered_functions) {
            external_symbols.insert("_" + func_pair.first); // Use the key (function name) from the map
        }
        // --- Legacy JIT_CALL_START handling removed - now handled by veneer system ---
    }

    // --- Write .text section ---
    ofs << ".section __TEXT,__text,regular,pure_instructions\n";
    ofs << ".globl _start\n";
    ofs << ".globl _START\n";
    for (const auto& sym : external_symbols) {
        ofs << ".globl " << sym << "\n";
    }
    ofs << ".p2align 2\n";

    std::string start_label_name;
    auto start_it = label_rename_map.find("START");
    if (start_it != label_rename_map.end()) {
        start_label_name = start_it->second;
    } else {
        start_label_name = "L_START";
    }
    ofs << "_start:\n";
    ofs << "_START:\n";
    ofs << "    B " << start_label_name << "\n";
    std::unordered_set<std::string> written_labels{"_start"};

    // --- Main Instruction Processing Loop ---
    for (size_t i = 0; i < code_instructions.size(); ++i) {
        const auto& instr = code_instructions[i];
        
        // Filter empty placeholder instructions that would generate DCD 0
        if (instr.encoding == 0x0 && instr.assembly_text.empty()) {
            continue; // Skip empty placeholder instructions
        }
        
        // Filter Veneer Code: Skip any instruction tagged as JitAddress
        if (instr.jit_attribute == JITAttribute::JitAddress) {
            continue; // Skip veneer instructions completely
        }
        
        auto addr_it = address_to_labels.find(code_instructions[i].address);
        if (addr_it != address_to_labels.end()) {
            for (const auto& original_label : addr_it->second) {

                // Only print labels that belong in the CODE segment.
                auto seg_it = label_to_segment.find(original_label);
                if (seg_it != label_to_segment.end() && seg_it->second != SegmentType::CODE) {
                    continue; // This is a DATA or RODATA label, skip it.
                }

                // Filter Veneer Labels: Skip veneer labels
                if (veneer_labels.find(original_label) != veneer_labels.end()) {
                    continue; // This is a veneer label, skip it
                }

                if (written_labels.find(original_label) == written_labels.end()) {
                    auto rename_it = label_rename_map.find(original_label);
                    if (rename_it != label_rename_map.end()) {
                        ofs << make_clang_compatible_label(rename_it->second) << ":\n";
                    } else {
                        ofs << make_clang_compatible_label(original_label) << ":\n";
                    }
                    written_labels.insert(original_label);
                }
            }
        }
        std::string final_assembly_text = instr.assembly_text;


        // Remap BL Calls: Check if this is a BL instruction targeting a veneer
        if (instr.relocation == RelocationType::PC_RELATIVE_26_BIT_OFFSET && 
            !instr.target_label.empty() &&
            veneer_to_function_map.find(instr.target_label) != veneer_to_function_map.end()) {
            // This is a BL instruction targeting a veneer, remap to direct function call
            const std::string& function_name = veneer_to_function_map[instr.target_label];
            ofs << "    BL _" << function_name << "\n";
            continue;
        }

        // Only perform label replacement on instructions that actually use labels.
        if (!instr.target_label.empty()) {
            auto rename_it = label_rename_map.find(instr.target_label);
            if (rename_it != label_rename_map.end()) {
                // Replace the original target label with the renamed one in the assembly text.
                replace_label_in_text(final_assembly_text, instr.target_label, make_clang_compatible_label(rename_it->second));
            } else {
                // Make the original label clang-compatible
                replace_label_in_text(final_assembly_text, instr.target_label, make_clang_compatible_label(instr.target_label));
            }
        }


        if (instr.jit_attribute == JITAttribute::JitAddress) {
            // This attribute is used for JIT-specific address loading that
            // should be omitted or handled differently in static assembly.

            // Case 1: The 4-instruction MOVZ/MOVK sequence for absolute addresses.
            if (instr.assembly_text.rfind("MOVZ ", 0) == 0) {
                i += 3; // Skip this MOVZ and the next three MOVKs.
                continue;
            }
            // Fallback for any other unexpected JitAddress instruction.
            continue;
        } else if (instr.jit_attribute == JITAttribute::JitStore || instr.jit_attribute == JITAttribute::JitRestore) {
            continue;
        } else if (instr.relocation == RelocationType::PC_RELATIVE_26_BIT_OFFSET && RuntimeManager::instance().is_function_registered(instr.target_label)) {
            ofs << "    BL _" << instr.target_label << "\n";
            continue;
        } else if (i + 1 < code_instructions.size() &&
                   instr.assembly_text.rfind("ADRP ", 0) == 0 &&
                   code_instructions[i+1].assembly_text.find(", #:lo12:") != std::string::npos) {
            const auto& next_instr = code_instructions[i+1];
            size_t comma_pos = instr.assembly_text.find(',');
            std::string reg = instr.assembly_text.substr(5, comma_pos - 5);
            reg.erase(reg.find_last_not_of(" \t") + 1);
            std::string label;
            auto label_it = label_rename_map.find(instr.target_label);
            if (label_it != label_rename_map.end()) {
                label = label_it->second;
            } else {
                label = instr.target_label;
            }

            ofs << "    ADRP " << reg << ", " << label << "@PAGE\n";
            ofs << "    ADD " << reg << ", " << reg << ", " << label << "@PAGEOFF\n";
            i++;
            continue;
        }

        if (!final_assembly_text.empty()) {
            ofs << "    " << final_assembly_text << "\n";
        }
    }


    // --- Write .rodata section (Mach-O Clang-compatible) ---
    if (!rodata_instructions.empty()) {
        ofs << "\n.section __DATA,__const\n";
        ofs << ".p2align 3\n";
        for (size_t i = 0; i < rodata_instructions.size(); ++i) {
            const auto& instr = rodata_instructions[i];

            // If this is the high 32 bits of an address, skip it.
            if (instr.relocation == RelocationType::ABSOLUTE_ADDRESS_HI32) {
                continue;
            }

            if (instr.is_label_definition && !instr.target_label.empty()) {
                auto rename_it = label_rename_map.find(instr.target_label);
                if (rename_it != label_rename_map.end()) {
                    ofs << make_clang_compatible_label(rename_it->second) << ":\n";
                } else {
                    ofs << make_clang_compatible_label(instr.target_label) << ":\n";
                }
            }

            std::string asm_line = instr.assembly_text;

            // Explicitly handle our new relocation type for clean output.
            if (instr.relocation == RelocationType::ABSOLUTE_ADDRESS_LO32) {
                auto rename_it = label_rename_map.find(instr.target_label);
                if (rename_it != label_rename_map.end()) {
                    asm_line = ".quad " + make_clang_compatible_label(rename_it->second);
                } else {
                    asm_line = ".quad " + make_clang_compatible_label(instr.target_label);
                }
            } else if (asm_line.rfind("DCD ", 0) == 0) {
                asm_line.replace(0, 3, ".long");
            }

            // Only write if there's something to write (handles empty placeholders).
            if (!asm_line.empty() && asm_line.find_first_not_of(" \t;") != std::string::npos) {
                ofs << "    " << asm_line << "\n";
            }
        }
    }

    // --- Write .data section (Mach-O Clang-compatible) ---
    if (!data_instructions.empty()) {
        ofs << "\n.section __DATA,__data\n";
        ofs << ".p2align 3\n";

        bool skipping_runtime_table = false;
        for (size_t i = 0; i < data_instructions.size(); ++i) {
            const auto& instr = data_instructions[i];

            // This is the only place labels for the .data section should be written.
            // The check instr.is_label_definition is key.
            if (instr.is_label_definition && !instr.target_label.empty()) {
                // Check if this is the start of the runtime function table.
                if (instr.target_label == "L__runtime_function_table") {
                    skipping_runtime_table = true;
                    continue; // Skip writing the label itself.
                }
                // A new label definition marks the end of the runtime table.
                if (skipping_runtime_table) {
                    skipping_runtime_table = false;
                }
                ofs << make_clang_compatible_label(instr.target_label) << ":\n"; // Write the label here.
            }

            // If we are in the runtime table section, skip the data entries.
            if (skipping_runtime_table) {
                continue;
            }

            // If this is the high 32 bits of an address, skip it.
            if (instr.relocation == RelocationType::ABSOLUTE_ADDRESS_HI32) {
                continue;
            }

            // If the instruction has assembly text (like .quad or .long), print it.
            if (!instr.assembly_text.empty()) {
                std::string asm_line = instr.assembly_text;

                if (instr.relocation == RelocationType::ABSOLUTE_ADDRESS_LO32) {
                    auto rename_it = label_rename_map.find(instr.target_label);
                    if (rename_it != label_rename_map.end()) {
                        asm_line = ".quad " + make_clang_compatible_label(rename_it->second);
                    } else {
                        asm_line = ".quad " + make_clang_compatible_label(instr.target_label);
                    }
                } else if (asm_line.rfind("DCQ ", 0) == 0) {
                    asm_line.replace(0, 3, ".quad");
                } else if (asm_line.rfind("DCD ", 0) == 0) {
                    asm_line.replace(0, 3, ".long");
                }

                if (!asm_line.empty() && asm_line.find_first_not_of(" \t;") != std::string::npos) {
                    ofs << "    " << asm_line << "\n";
                }
            }
        }
    }

    ofs.close();
    std::cout << "Assembly written to: " << path << std::endl;
}
