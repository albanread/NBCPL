#pragma once

#include <functional>
#include <string>
#include <map>
#include <vector>

// Forward declarations to avoid circular includes
class NewCodeGenerator;
class PairwiseReductionLoopStatement;
class Symbol;

// Define a function pointer type for our encoders
using NeonReducerEncoder = std::function<void(NewCodeGenerator&, const PairwiseReductionLoopStatement&)>;

/**
 * @brief Central registry for NEON reduction instruction encoders
 * 
 * This registry provides a clean separation between dispatch logic and 
 * encoding logic by maintaining a map of validated, single-purpose encoders.
 * Each encoder is responsible for generating ARM64 NEON instructions for
 * a specific reduction operation on a specific vector layout.
 */
class NeonReducerRegistry {
public:
    /**
     * @brief Constructs registry and registers all available encoders
     */
    NeonReducerRegistry();
    
    /**
     * @brief Find an encoder for the given intrinsic name and vector type
     * @param intrinsic_name The NEON intrinsic name from PairwiseReductionLoopStatement
     * @param vector_type The vector type string (e.g., "FPAIR", "FQUAD")
     * @return Function pointer to encoder, or nullptr if not found
     */
    NeonReducerEncoder findEncoder(const std::string& intrinsic_name, const std::string& vector_type);
    
    /**
     * @brief Find encoder with fallback detection
     * @param intrinsic_name The NEON intrinsic name
     * @param vector_type The vector type string
     * @param used_fallback Output parameter indicating if fallback was used
     * @return Function pointer to encoder, or nullptr if not found
     */
    NeonReducerEncoder findEncoderWithFallback(
        const std::string& intrinsic_name, 
        const std::string& vector_type,
        bool& used_fallback
    );
    
    /**
     * @brief Get list of all registered encoders (for debugging/testing)
     * @return Vector of registry keys for all registered encoders
     */
    std::vector<std::string> getRegisteredEncoders() const;
    
    /**
     * @brief Check if a specific encoder exists
     * @param intrinsic_name The NEON intrinsic name
     * @param vector_type The vector type string
     * @return True if encoder is registered
     */
    bool hasEncoder(const std::string& intrinsic_name, const std::string& vector_type) const;
    
    /**
     * @brief Get human-readable information about an encoder
     * @param intrinsic_name The NEON intrinsic name
     * @param vector_type The vector type string
     * @return Description string, or empty if not found
     */
    std::string getEncoderInfo(const std::string& intrinsic_name, const std::string& vector_type) const;

private:
    // The registry maps keys to encoder functions
    // Key format: "INTRINSIC_NAME:VECTOR_TYPE:LAYOUT"
    std::map<std::string, NeonReducerEncoder> encoder_map_;
    
    // Additional metadata for each encoder (for debugging/info)
    std::map<std::string, std::string> encoder_info_map_;
    
    /**
     * @brief Helper to create standardized registry keys
     * @param intrinsic_name The NEON intrinsic name
     * @param vector_type The vector type string  
     * @return Formatted key string
     */
    std::string makeKey(const std::string& intrinsic_name, const std::string& vector_type) const;
    
    /**
     * @brief Map vector type to NEON arrangement specifier
     * @param vector_type The vector type string (e.g., "FQUAD", "FPAIR")
     * @return NEON arrangement (e.g., "4H", "2S", "4S")
     */
    std::string getArrangement(const std::string& vector_type) const;
    
    /**
     * @brief Register all available encoders
     * Called from constructor to populate the registry
     */
    void registerEncoders();
    
    /**
     * @brief Register a single encoder with metadata
     * @param intrinsic_name The NEON intrinsic name
     * @param vector_type The vector type string
     * @param encoder The encoder function
     * @param info Human-readable description
     */
    void registerEncoder(
        const std::string& intrinsic_name,
        const std::string& vector_type,
        NeonReducerEncoder encoder,
        const std::string& info
    );
};

// Forward declarations for encoder functions that will be implemented
// These are declared here to avoid circular dependencies

// Float pairwise minimum encoders
void gen_neon_fminp_4s(NewCodeGenerator& cg, const PairwiseReductionLoopStatement& node);
void gen_neon_fminp_2s(NewCodeGenerator& cg, const PairwiseReductionLoopStatement& node);
void gen_neon_fminp_4h(NewCodeGenerator& cg, const PairwiseReductionLoopStatement& node); // For FQUAD

// Integer pairwise minimum encoders
void gen_neon_sminp_4s(NewCodeGenerator& cg, const PairwiseReductionLoopStatement& node);
void gen_neon_sminp_2s(NewCodeGenerator& cg, const PairwiseReductionLoopStatement& node);

// Float pairwise maximum encoders
void gen_neon_fmaxp_4s(NewCodeGenerator& cg, const PairwiseReductionLoopStatement& node);
void gen_neon_fmaxp_2s(NewCodeGenerator& cg, const PairwiseReductionLoopStatement& node);
void gen_neon_fmaxp_4h(NewCodeGenerator& cg, const PairwiseReductionLoopStatement& node); // For FQUAD

// Float pairwise addition encoders
void gen_neon_faddp_4s(NewCodeGenerator& cg, const PairwiseReductionLoopStatement& node);
void gen_neon_faddp_2s(NewCodeGenerator& cg, const PairwiseReductionLoopStatement& node);
void gen_neon_faddp_4h(NewCodeGenerator& cg, const PairwiseReductionLoopStatement& node); // For FQUAD

// Integer pairwise addition encoders
void gen_neon_addp_4s(NewCodeGenerator& cg, const PairwiseReductionLoopStatement& node);
void gen_neon_addp_2s(NewCodeGenerator& cg, const PairwiseReductionLoopStatement& node);