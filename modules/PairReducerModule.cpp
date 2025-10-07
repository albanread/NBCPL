#include "PairReducerModule.h"
#include "../Reducer.h"
#include "../AST.h"
#include <stdexcept>
#include <iostream>

namespace PairReducerModule {

    // =====================================================
    // PairTypeHandler Implementation
    // =====================================================

    bool PairTypeHandler::isPairExpression(const Expression* expr) {
        return dynamic_cast<const PairExpression*>(expr) != nullptr;
    }

    bool PairTypeHandler::extractPairLiterals(const PairExpression* pair_expr, 
                                            int32_t& first_val, int32_t& second_val) {
        if (!pair_expr || !pair_expr->is_literal()) {
            return false;
        }

        // Extract first value
        if (auto* first_lit = dynamic_cast<const NumberLiteral*>(pair_expr->first_expr.get())) {
            if (first_lit->literal_type == NumberLiteral::LiteralType::Integer) {
                first_val = static_cast<int32_t>(first_lit->int_value);
            } else {
                return false; // Float in PAIR is invalid
            }
        } else {
            return false;
        }

        // Extract second value
        if (auto* second_lit = dynamic_cast<const NumberLiteral*>(pair_expr->second_expr.get())) {
            if (second_lit->literal_type == NumberLiteral::LiteralType::Integer) {
                second_val = static_cast<int32_t>(second_lit->int_value);
            } else {
                return false; // Float in PAIR is invalid
            }
        } else {
            return false;
        }

        return true;
    }

    bool PairTypeHandler::arePairsCompatible(const Expression* left_expr, const Expression* right_expr) {
        return isPairExpression(left_expr) && isPairExpression(right_expr);
    }

    // =====================================================
    // PairValidator Implementation
    // =====================================================

    void PairValidator::validatePairReduction(const std::string& operation_name,
                                            const Expression* left_expr,
                                            const Expression* right_expr) {
        // Basic validation for testing - in real implementation would check actual expressions
        if (!isPairOperationSupported(operation_name)) {
            throw std::runtime_error("PAIR operation not supported: " + operation_name);
        }
    }

    bool PairValidator::isPairOperationSupported(const std::string& operation_name) {
        return operation_name == "MIN" || operation_name == "MAX" || operation_name == "SUM" ||
               operation_name == "PRODUCT" || operation_name == "BITWISE_AND" || operation_name == "BITWISE_OR";
    }

    bool PairValidator::validatePairLiterals(int32_t first_val, int32_t second_val) {
        // All int32_t values are valid for PAIR
        return true;
    }

    std::string PairValidator::formatPairError(const std::string& operation_name,
                                             const std::string& error_type,
                                             const std::string& context) {
        std::string error_msg = "PAIR " + operation_name + " Error [" + error_type + "]";
        if (!context.empty()) {
            error_msg += ": " + context;
        }
        return error_msg;
    }

    // =====================================================
    // PairOptimizer Implementation (Simplified)
    // =====================================================

    std::vector<std::string> PairOptimizer::optimizePairOperationSequence(
        const std::vector<std::string>& operations) {
        // For testing, just return the operations as-is
        return operations;
    }

    std::string PairOptimizer::chooseRegisterStrategy(int operation_count) {
        if (operation_count <= 4) {
            return "MINIMAL_REGISTERS";
        } else if (operation_count <= 16) {
            return "BALANCED_REGISTERS";
        } else {
            return "STREAMING_REGISTERS";
        }
    }

    PairReducerModule::PairOptimizer::PairPerformanceMetrics PairOptimizer::estimatePerformance(const std::string& operation_name) {
        PairPerformanceMetrics metrics;
        
        if (operation_name == "MIN" || operation_name == "MAX" || operation_name == "SUM") {
            metrics.estimated_cycles = 2;
            metrics.throughput_ops_per_cycle = 1.0f;
            metrics.register_pressure = 3;
            metrics.can_pipeline = true;
        } else if (operation_name == "PRODUCT") {
            metrics.estimated_cycles = 3;
            metrics.throughput_ops_per_cycle = 0.5f;
            metrics.register_pressure = 3;
            metrics.can_pipeline = true;
        } else {
            metrics.estimated_cycles = 4;
            metrics.throughput_ops_per_cycle = 0.25f;
            metrics.register_pressure = 4;
            metrics.can_pipeline = false;
        }
        
        return metrics;
    }

    // =====================================================
    // Module Initialization Implementation (Simplified)
    // =====================================================

    namespace Init {
        
        bool validatePairModule() {
            try {
                // Test reducer creation
                auto min_reducer = createReducer("MIN");
                auto max_reducer = createReducer("MAX");
                auto sum_reducer = createReducer("SUM");
                
                if (!min_reducer || !max_reducer || !sum_reducer) {
                    return false;
                }
                
                // Test type handler functions
                if (PairTypeHandler::getNeonArrangement() != ".2S") {
                    return false;
                }
                
                if (PairTypeHandler::getRegisterWidth() != 64) {
                    return false;
                }
                
                // Test validator functions
                if (!PairValidator::isPairOperationSupported("MIN") ||
                    !PairValidator::isPairOperationSupported("MAX") ||
                    !PairValidator::isPairOperationSupported("SUM")) {
                    return false;
                }
                
                return true;
                
            } catch (...) {
                return false;
            }
        }
        
    } // namespace Init

} // namespace PairReducerModule