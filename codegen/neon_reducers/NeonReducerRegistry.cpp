#include "NeonReducerRegistry.h"
#include "../../NewCodeGenerator.h"
#include "../../AST.h"
#include "../../Symbol.h"
#include <iostream>
#include <sstream>

NeonReducerRegistry::NeonReducerRegistry() {
    registerEncoders();
}

NeonReducerEncoder NeonReducerRegistry::findEncoder(const std::string& intrinsic_name, const std::string& vector_type) {
    std::string key = makeKey(intrinsic_name, vector_type);
    
    auto it = encoder_map_.find(key);
    if (it != encoder_map_.end()) {
        return it->second;
    }
    
    return nullptr;
}

NeonReducerEncoder NeonReducerRegistry::findEncoderWithFallback(
    const std::string& intrinsic_name, 
    const std::string& vector_type,
    bool& used_fallback
) {
    // Try to find exact match first
    auto encoder = findEncoder(intrinsic_name, vector_type);
    if (encoder) {
        used_fallback = false;
        return encoder;
    }
    
    // For now, no fallback logic - this can be extended later
    // to handle compatible arrangements (e.g., 4S -> 2S conversion)
    used_fallback = true;
    
    // Log that we couldn't find an encoder
    std::cerr << "DEBUG: No NEON encoder found for: " 
              << makeKey(intrinsic_name, vector_type) << std::endl;
    
    return nullptr;
}

std::vector<std::string> NeonReducerRegistry::getRegisteredEncoders() const {
    std::vector<std::string> encoders;
    for (const auto& pair : encoder_map_) {
        encoders.push_back(pair.first);
    }
    return encoders;
}

bool NeonReducerRegistry::hasEncoder(const std::string& intrinsic_name, const std::string& vector_type) const {
    std::string key = makeKey(intrinsic_name, vector_type);
    return encoder_map_.find(key) != encoder_map_.end();
}

std::string NeonReducerRegistry::getEncoderInfo(const std::string& intrinsic_name, const std::string& vector_type) const {
    std::string key = makeKey(intrinsic_name, vector_type);
    auto it = encoder_info_map_.find(key);
    if (it != encoder_info_map_.end()) {
        return it->second;
    }
    return "";
}

std::string NeonReducerRegistry::makeKey(const std::string& intrinsic_name, const std::string& vector_type) const {
    std::string arrangement = getArrangement(vector_type);
    return intrinsic_name + ":" + vector_type + ":" + arrangement;
}

std::string NeonReducerRegistry::getArrangement(const std::string& vector_type) const {
    // Map NewBCPL vector types to ARM64 NEON arrangement specifiers
    if (vector_type == "FQUAD") {
        return "4H";  // 4x16-bit floats
    } else if (vector_type == "FQUADS") {
        return "4S";  // Vector of FQUADs - use 4S for now
    } else if (vector_type == "FPAIR") {
        return "2S";  // 2x32-bit floats
    } else if (vector_type == "PAIR") {
        return "2S";  // 2x32-bit integers
    } else if (vector_type == "QUAD") {
        return "4S";  // 4x32-bit integers
    } else if (vector_type == "FVEC8" || vector_type == "VEC8") {
        return "4S";  // 8-element vectors processed in 4S chunks
    } else if (vector_type == "OCT") {
        return "4S";  // 8x32-bit integers
    } else if (vector_type == "FOCT") {
        return "4S";  // 8x32-bit floats
    }
    
    // Default fallback
    return "4S";
}

void NeonReducerRegistry::registerEncoders() {
    // Register float pairwise minimum encoders
    registerEncoder("llvm.arm.neon.vpmin.v4f32", "FVEC8", gen_neon_fminp_4s,
                   "FMINP.4S for 8-element float vectors");
    registerEncoder("llvm.arm.neon.vpmin.v2f32", "FPAIR", gen_neon_fminp_2s,
                   "FMINP.2S for 2-element float pairs");
    registerEncoder("llvm.arm.neon.vpmin.v4f16", "FQUAD", gen_neon_fminp_4h,
                   "FMINP.4H for 4x16-bit float quads (FQUAD)");
    
    // Register integer pairwise minimum encoders  
    registerEncoder("llvm.arm.neon.vpmin.v4i32", "VEC8", gen_neon_sminp_4s,
                   "SMINP.4S for 8-element integer vectors");
    registerEncoder("llvm.arm.neon.vpmin.v2i32", "PAIR", gen_neon_sminp_2s,
                   "SMINP.2S for 2-element integer pairs");
    
    // Register float pairwise maximum encoders
    registerEncoder("llvm.arm.neon.vpmax.v4f32", "FVEC8", gen_neon_fmaxp_4s,
                   "FMAXP.4S for 8-element float vectors");
    registerEncoder("llvm.arm.neon.vpmax.v2f32", "FPAIR", gen_neon_fmaxp_2s,
                   "FMAXP.2S for 2-element float pairs");
    registerEncoder("llvm.arm.neon.vpmax.v4f16", "FQUAD", gen_neon_fmaxp_4h,
                   "FMAXP.4H for 4x16-bit float quads (FQUAD)");
    
    // Register float pairwise addition encoders
    registerEncoder("llvm.arm.neon.vpadd.v4f32", "FVEC8", gen_neon_faddp_4s,
                   "FADDP.4S for 8-element float vectors");
    registerEncoder("llvm.arm.neon.vpadd.v2f32", "FPAIR", gen_neon_faddp_2s,
                   "FADDP.2S for 2-element float pairs");
    registerEncoder("llvm.arm.neon.vpadd.v4f16", "FQUAD", gen_neon_faddp_4h,
                   "FADDP.4H for 4x16-bit float quads (FQUAD)");
    
    // Register integer pairwise addition encoders
    registerEncoder("llvm.arm.neon.vpadd.v4i32", "VEC8", gen_neon_addp_4s,
                   "ADDP.4S for 8-element integer vectors");
    registerEncoder("llvm.arm.neon.vpadd.v2i32", "PAIR", gen_neon_addp_2s,
                   "ADDP.2S for 2-element integer pairs");
    
    // Log registration summary
    std::cout << "NeonReducerRegistry: Registered " << encoder_map_.size() 
              << " NEON reduction encoders" << std::endl;
    
    // Debug: Print all registered encoders
    if (false) { // Enable for debugging
        std::cout << "Registered encoders:" << std::endl;
        for (const auto& pair : encoder_map_) {
            std::cout << "  " << pair.first << " - " 
                      << encoder_info_map_[pair.first] << std::endl;
        }
    }
}

void NeonReducerRegistry::registerEncoder(
    const std::string& intrinsic_name,
    const std::string& vector_type,
    NeonReducerEncoder encoder,
    const std::string& info
) {
    std::string key = makeKey(intrinsic_name, vector_type);
    encoder_map_[key] = encoder;
    encoder_info_map_[key] = info;
}