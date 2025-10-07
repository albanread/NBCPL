#pragma once

#include "../AST.h"
#include "../Reducer.h"
#include <string>
#include <memory>
#include <vector>

/**
 * PAIR Reducer Module - Testing and validation for NewBCPL's existing PAIR functionality
 * 
 * This module provides testing infrastructure for NewBCPL's PAIR type integration
 * with the reducer architecture. It validates existing functionality rather than
 * implementing new code generation.
 * 
 * PAIR Type (Existing in NewBCPL):
 * - Storage: Single 64-bit value containing two 32-bit integers
 * - Syntax: pair(first, second) creates a PAIR value
 * - Access: pair_var.first and pair_var.second access components
 * - Operations: MIN, MAX, SUM work with PAIR types
 * 
 * Testing Focus:
 * - Validate existing PAIR expression handling
 * - Test reducer integration with PAIR types
 * - Verify NEON registry has PAIR support
 * - Validate performance characteristics
 */

namespace PairReducerModule {

    /**
     * PAIR Type Testing Utilities
     */
    class PairTypeHandler {
    public:
        /**
         * Validate that an expression represents a PAIR type
         */
        static bool isPairExpression(const Expression* expr);
        
        /**
         * Extract PAIR values from a literal PAIR expression
         */
        static bool extractPairLiterals(const PairExpression* pair_expr, 
                                      int32_t& first_val, int32_t& second_val);
        
        /**
         * Check if two PAIR expressions are type-compatible
         */
        static bool arePairsCompatible(const Expression* left_expr, const Expression* right_expr);
        
        /**
         * Get the NEON arrangement string for PAIR operations
         */
        static std::string getNeonArrangement() { return ".2S"; }
        
        /**
         * Get the register width required for PAIR values
         */
        static int getRegisterWidth() { return 64; }
    };



    /**
     * PAIR Validation Utilities
     */
    class PairValidator {
    public:
        static void validatePairReduction(const std::string& operation_name,
                                        const Expression* left_expr,
                                        const Expression* right_expr);
        
        static bool isPairOperationSupported(const std::string& operation_name);
        
        static bool validatePairLiterals(int32_t first_val, int32_t second_val);
        
        static std::string formatPairError(const std::string& operation_name,
                                         const std::string& error_type,
                                         const std::string& context = "");
    };

    /**
     * PAIR Performance Testing
     */
    class PairOptimizer {
    public:
        static std::vector<std::string> optimizePairOperationSequence(
            const std::vector<std::string>& operations);
        
        static std::string chooseRegisterStrategy(int operation_count);
        
        struct PairPerformanceMetrics {
            int estimated_cycles;
            float throughput_ops_per_cycle;
            int register_pressure;
            bool can_pipeline;
        };
        
        static PairPerformanceMetrics estimatePerformance(const std::string& operation_name);
    };

    /**
     * Module Testing Functions
     */
    namespace Init {
        /**
         * Validate PAIR module functionality
         */
        bool validatePairModule();
    }

} // namespace PairReducerModule