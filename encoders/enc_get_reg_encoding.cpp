// This encoder is NOT present in the test schedule. Test will be added via wrapper and results updated here.
#include "Encoder.h"
#include <string>
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <execinfo.h>
#include <cstdlib>

/**
 * @brief Helper function to get the 5-bit integer encoding of a register name.
 * @details
 * This function translates a register name string (e.g., "X0", "w1", "sp", "D0", "V0") into
 * its 5-bit hardware encoding (0-31). It is case-insensitive and handles aliases
 * for the stack pointer (SP) and zero register (WZR/XZR), as well as floating-point
 * and vector registers (D0-D31, V0-V31).
 *
 * @param reg_name The register name as a string.
 * @return The 5-bit integer encoding of the register.
 * @throw std::invalid_argument if the register name is invalid.
 */
uint32_t Encoder::get_reg_encoding(const std::string& reg_name) {
    if (reg_name.empty()) {
        std::cerr << "ERROR: Empty register name detected in get_reg_encoding!" << std::endl;
        std::cerr << "This error occurred during encoder function call." << std::endl;
        
        // Print stack trace
        void *array[10];
        size_t size = backtrace(array, 10);
        char **strings = backtrace_symbols(array, size);
        
        std::cerr << "Stack trace:" << std::endl;
        for (size_t i = 0; i < size; i++) {
            std::cerr << "  " << strings[i] << std::endl;
        }
        free(strings);
        
        throw std::invalid_argument("Register name cannot be empty.");
    }

    // Convert to lowercase for case-insensitive matching.
    std::string lower_reg = reg_name;
    std::transform(lower_reg.begin(), lower_reg.end(), lower_reg.begin(), ::tolower);

    // Handle aliases for register 31 first.
    if (lower_reg == "wzr" || lower_reg == "xzr" || lower_reg == "wsp" || lower_reg == "sp") {
        return 31;
    }

    // Check for a valid alphabetic prefix.
    char prefix = lower_reg[0];
    if (!std::isalpha(prefix)) {
        throw std::invalid_argument("Invalid register prefix in '" + reg_name + "'. Must be a letter.");
    }

    try {
        // Handle different register types
        if (prefix == 'd' || prefix == 'v' || prefix == 's' || prefix == 'q') { // Added 'v' for vector registers, 's' for single precision FP, and 'q' for 128-bit vector registers
            uint32_t reg_num = std::stoul(lower_reg.substr(1));
            if (reg_num > 31) {
                throw std::out_of_range("FP/Vector register number " + std::to_string(reg_num) + 
                                      " is out of the valid range [0, 31].");
            }
            return reg_num;
        } else if (prefix == 'w' || prefix == 'x') {
            uint32_t reg_num = std::stoul(lower_reg.substr(1));
            if (reg_num > 31) {
                throw std::out_of_range("Register number " + std::to_string(reg_num) + 
                                      " is out of the valid range [0, 31].");
            }
            return reg_num;
        } else {
            throw std::invalid_argument("Invalid register prefix in '" + reg_name + 
                                     "'. Must be 'w', 'x', 'd', 'v', 's', or 'q'.");
        }
    } catch (const std::logic_error&) {
        throw std::invalid_argument("Invalid register format: " + reg_name);
    }
}
