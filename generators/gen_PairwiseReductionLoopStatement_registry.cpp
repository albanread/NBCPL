// generators/gen_PairwiseReductionLoopStatement_registry.cpp
#include "../NewCodeGenerator.h"
#include "../codegen/neon_reducers/NeonReducerRegistry.h"
#include "../AST.h"
#include "../Symbol.h"

// Static instance of the registry - initialized once
static NeonReducerRegistry neon_reducer_registry;

/**
 * @brief Registry-based visitor for PairwiseReductionLoopStatement
 * 
 * This function integrates the new modular NEON encoder registry with the
 * existing PairwiseReductionLoopStatement processing. It tries to find a
 * validated encoder first, then falls back to the existing monolithic
 * implementation to ensure continuity during the transition.
 */
void NewCodeGenerator::visit_pairwise_reduction_with_registry(PairwiseReductionLoopStatement& node) {
    debug_print("=== Registry-based PairwiseReductionLoopStatement processing ===");
    debug_print("Intrinsic: " + node.intrinsic_name);
    debug_print("Vector A: " + node.vector_a_name);
    debug_print("Vector B: " + node.vector_b_name);
    debug_print("Result: " + node.result_vector_name);

    // Step 1: Determine the vector type from the first input vector
    Symbol vector_a_symbol;
    if (!lookup_symbol(node.vector_a_name, vector_a_symbol)) {
        debug_print("ERROR: Cannot find symbol for vector: " + node.vector_a_name);
        // Fall back to legacy implementation
        visit_pairwise_reduction_legacy(node);
        return;
    }

    // Step 2: Map VarType to string for registry lookup
    std::string vector_type_str = varTypeToString(vector_a_symbol.type);
    
    // Step 3: Try to find a validated encoder in the registry
    bool used_fallback = false;
    NeonReducerEncoder encoder = neon_reducer_registry.findEncoderWithFallback(
        node.intrinsic_name, 
        vector_type_str,
        used_fallback
    );

    if (encoder && !used_fallback) {
        // === NEW PATH: Use validated encoder ===
        debug_print("Using validated NEON encoder for " + node.intrinsic_name + 
                   " on " + vector_type_str);
        
        try {
            encoder(*this, node);
            debug_print("Successfully generated code using registry encoder");
        } catch (const std::exception& e) {
            debug_print("ERROR in registry encoder: " + std::string(e.what()));
            debug_print("Falling back to legacy implementation");
            visit_pairwise_reduction_legacy(node);
        }
        
    } else {
        // === FALLBACK PATH: Use existing implementation ===
        if (used_fallback) {
            debug_print("WARN: Using fallback encoder for " + node.intrinsic_name + 
                       " on " + vector_type_str);
        } else {
            debug_print("WARN: No registry encoder found for " + node.intrinsic_name + 
                       " on " + vector_type_str);
        }
        
        // Call the existing implementation
        visit_pairwise_reduction_legacy(node);
    }
    
    debug_print("=== Registry-based processing complete ===");
}

/**
 * @brief Legacy implementation wrapper
 * 
 * This wraps the existing PairwiseReductionLoopStatement logic to maintain
 * compatibility during the transition. Once all encoders are validated,
 * this can be removed.
 */
void NewCodeGenerator::visit_pairwise_reduction_legacy(PairwiseReductionLoopStatement& node) {
    debug_print("Using legacy PairwiseReductionLoopStatement implementation");
    
    // Get the type of the input vectors from the symbol table
    Symbol vector_a_symbol;
    if (!lookup_symbol(node.vector_a_name, vector_a_symbol)) {
        debug_print("Error: Cannot find symbol for vector: " + node.vector_a_name);
        return;
    }

    // Route to existing type-specific generators based on vector type
    switch (vector_a_symbol.type) {
        case VarType::FPAIR:
            if (node.intrinsic_name.find("vpmin") != std::string::npos) {
                generateFPairPairwiseMin(node);
            } else {
                debug_print("Unsupported FPAIR intrinsic: " + node.intrinsic_name);
            }
            break;
            
        case VarType::FQUAD:
            if (node.intrinsic_name.find("vpmin") != std::string::npos) {
                generateFQuadPairwiseMin(node);
            } else {
                debug_print("Unsupported FQUAD intrinsic: " + node.intrinsic_name);
            }
            break;
            
        case VarType::FOCT:
            if (node.intrinsic_name.find("vpmin") != std::string::npos) {
                generateFOctPairwiseMin(node);
            } else {
                debug_print("Unsupported FOCT intrinsic: " + node.intrinsic_name);
            }
            break;
            
        case VarType::PAIR:
            if (node.intrinsic_name.find("vpmin") != std::string::npos) {
                generatePairPairwiseMin(node);
            } else {
                debug_print("Unsupported PAIR intrinsic: " + node.intrinsic_name);
            }
            break;
            
        case VarType::QUAD:
            if (node.intrinsic_name.find("vpmin") != std::string::npos) {
                generateQuadPairwiseMin(node);
            } else {
                debug_print("Unsupported QUAD intrinsic: " + node.intrinsic_name);
            }
            break;
            
        case VarType::OCT:
            if (node.intrinsic_name.find("vpmin") != std::string::npos) {
                generateOctPairwiseMin(node);
            } else {
                debug_print("Unsupported OCT intrinsic: " + node.intrinsic_name);
            }
            break;
            
        case VarType::VEC:
            if (node.intrinsic_name.find("vpmin") != std::string::npos) {
                generateIntegerVectorPairwiseMin(node);
            } else {
                debug_print("Unsupported VEC intrinsic: " + node.intrinsic_name);
            }
            break;
            
        default:
            debug_print("Unsupported vector type for pairwise reduction: " + 
                       varTypeToString(vector_a_symbol.type));
            break;
    }
}

/**
 * @brief Helper function to convert VarType to string
 * 
 * Maps NewBCPL VarType enum values to string representations
 * used by the NEON reducer registry.
 */
std::string NewCodeGenerator::varTypeToString(VarType type) const {
    switch (type) {
        case VarType::FPAIR: return "FPAIR";
        case VarType::FQUAD: return "FQUAD";
        case VarType::FOCT: return "FOCT";
        case VarType::PAIR: return "PAIR";
        case VarType::QUAD: return "QUAD";
        case VarType::OCT: return "OCT";
        case VarType::VEC: return "VEC8";    // Assuming 8-element vectors
        default: return "UNKNOWN";
    }
}

/**
 * @brief Get registry information for debugging
 * 
 * This function can be called during debugging to see what encoders
 * are available in the registry.
 */
void NewCodeGenerator::debug_print_registry_info() const {
    auto encoders = neon_reducer_registry.getRegisteredEncoders();
    debug_print("=== NEON Registry Information ===");
    debug_print("Total registered encoders: " + std::to_string(encoders.size()));
    
    for (const auto& encoder_key : encoders) {
        debug_print("  " + encoder_key);
    }
    debug_print("=== End Registry Information ===");
}