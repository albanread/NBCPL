#pragma once
#include "../PeepholeOptimizer.h"
#include <memory>

namespace PeepholePatterns {

    // Constant Folding and Simplification
    std::unique_ptr<InstructionPattern> createConstantFoldingPattern();
    std::unique_ptr<InstructionPattern> createIdentityOperationEliminationPattern();
    std::unique_ptr<InstructionPattern> createRedundantMovePattern();
    std::unique_ptr<InstructionPattern> createSelfMoveEliminationPattern();

    // Address Calculation and Fusion
    std::unique_ptr<InstructionPattern> createAdrFusionPattern();
    std::unique_ptr<InstructionPattern> createAdrAddAddFusionPattern();

    // Strength Reduction
    std::unique_ptr<InstructionPattern> createStrengthReductionPattern();
    std::unique_ptr<InstructionPattern> createMultiplyByPowerOfTwoPattern();
    std::unique_ptr<InstructionPattern> createDivideByPowerOfTwoPattern();

    // Memory Optimization
    std::unique_ptr<InstructionPattern> createLoadAfterStorePattern();
    std::unique_ptr<InstructionPattern> createRedundantLoadEliminationPattern();
    std::unique_ptr<InstructionPattern> createDeadStorePattern();
    std::unique_ptr<InstructionPattern> createRedundantStorePattern();
    std::unique_ptr<InstructionPattern> createLoadThroughScratchRegisterPattern();
    std::unique_ptr<InstructionPattern> createConservativeMovzScratchPattern();
    std::unique_ptr<InstructionPattern> createMovSubMovScratchPattern();
    std::unique_ptr<InstructionPattern> createLdrToLdpXPattern();
    std::unique_ptr<InstructionPattern> createStrToStpXPattern();
  
     // Control Flow Optimization
     std::unique_ptr<InstructionPattern> createCompareZeroBranchPattern();
    std::unique_ptr<InstructionPattern> createBranchChainingPattern();

    // ...add more pattern declarations as needed...

    // Identical sequential MOV elimination
    std::unique_ptr<InstructionPattern> createIdenticalMovePattern();

    // In-place operation optimization patterns
    std::unique_ptr<InstructionPattern> createInPlaceComparisonPattern();
    std::unique_ptr<InstructionPattern> createInPlaceArithmeticPattern();
}