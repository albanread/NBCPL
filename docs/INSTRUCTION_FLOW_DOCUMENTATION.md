# Instruction Flow Documentation for NewBCPL Compiler

This document traces the complete flow of instructions through the NewBCPL compiler, from AST generation to final JIT execution, with special focus on identifying where NEON FPAIR instruction encodings might get corrupted.

## Overview of the Compilation Pipeline

The NewBCPL compiler follows this high-level flow:

```
Source Code (.bcl) → Lexer → Parser → AST → CodeGen → InstructionStream → Linker → JIT Memory → Execution
```

## Detailed Instruction Flow

### 1. Source Code Parsing (Parser.cpp)
- **Input**: BCPL source code
- **Output**: Abstract Syntax Tree (AST)
- **Key Files**: `Parser.cpp`, `AST.h`
- **Transformation**: Text → AST nodes
- **Corruption Risk**: None (no instruction encoding yet)

### 2. Code Generation (NewCodeGenerator.cpp)
- **Input**: AST nodes
- **Output**: Raw instructions via emit() calls
- **Key Files**: `NewCodeGenerator.cpp`, `VectorCodeGen.cpp`
- **Transformation**: AST nodes → Instruction objects
- **NEON Flow**: 
  - Binary operations with FPAIR types are detected
  - `VectorCodeGen::generateNeonBinaryOp()` is called
  - For FSUB: `VectorCodeGen::fsub_vector_2s()` creates the instruction
- **Corruption Risk**: **MEDIUM** - Encoding created here via BitPatcher

#### NEON FSUB Instruction Creation (VectorCodeGen.cpp:1501-1535)
```cpp
Instruction VectorCodeGen::fsub_vector_2s(const std::string& vd, const std::string& vn, const std::string& vm) {
    // Base FSUB vector opcode for .2S: 0x0ea1d400 (exact match to clang output)
    BitPatcher patcher(0x0ea1d400);
    
    // Clear existing register fields
    patcher.patch(0, 0, 5);   // Clear Rd (bits 4:0)
    patcher.patch(0, 5, 5);   // Clear Rn (bits 9:5)
    patcher.patch(0, 16, 5);  // Clear Rm (bits 20:16)
    
    patcher.patch(vd_num, 0, 5);   // Rd (bits 4:0)
    patcher.patch(vn_num, 5, 5);   // Rn (bits 9:5)
    patcher.patch(vm_num, 16, 5);  // Rm (bits 20:16)

    return Instruction(patcher.get_value(), assembly_text);
}
```

### 3. BitPatcher Processing (BitPatcher.cpp)
- **Input**: Base opcode + register numbers
- **Output**: Final 32-bit encoding
- **Key Files**: `BitPatcher.cpp`, `BitPatcher.h`
- **Transformation**: Base opcode + bit field patches → Final encoding
- **Corruption Risk**: **HIGH** - Complex inline assembly manipulation
- **Critical Code**: ARM64 inline assembly in `BitPatcher::patch()`

#### BitPatcher ARM64 Assembly
The BitPatcher uses inline ARM64 assembly to manipulate bits:
```cpp
__asm__ volatile (
    // Create mask, shift to position, clear target field, patch value
    // Multiple register operations and bit shifts
    : [data] "+r" (this->data), [mask] "=&r" (temp_mask), [val] "+r" (value_to_patch)
    : [start_bit] "r" (start_bit), [num_bits] "r" (num_bits)
    : "w1", "w2", "cc"
);
```

### 4. Instruction Stream (InstructionStream.cpp)
- **Input**: Instruction objects from CodeGen
- **Output**: Vector of instructions
- **Key Files**: `InstructionStream.cpp`, `InstructionStream.h`
- **Transformation**: Individual instructions → Ordered instruction vector
- **Corruption Risk**: **LOW** - Simple container, no encoding modification
- **Method**: `InstructionStream::add(const Instruction& instr)`

### 5. Linker Processing (Linker.cpp)
- **Input**: InstructionStream + LabelManager
- **Output**: Finalized instructions with resolved addresses
- **Key Files**: `Linker.cpp`, `Linker.h`
- **Transformation**: Relative addresses → Absolute addresses, relocations applied
- **Corruption Risk**: **MEDIUM** - For instructions with relocations only
- **NEON Impact**: NEON instructions typically don't require relocation, so linker should pass them through unchanged

#### Two-Pass Linker Process
1. **Pass 1**: `assignAddressesAndResolveLabels()` - Assigns addresses
2. **Pass 2**: `performRelocations()` - Applies relocations

**Critical**: NEON FSUB instructions should NOT be modified by the linker since they don't have relocations.

### 6. JIT Memory Writing (main.cpp:1175-1281)
- **Input**: Finalized instruction vector from linker
- **Output**: Instructions written to executable memory
- **Key Files**: `main.cpp` (handle_jit_compilation), `JITMemoryManager.cpp`
- **Transformation**: Instruction objects → Raw bytes in memory
- **Corruption Risk**: **HIGH** - Direct memory writing with memcpy
- **Critical Code**:

```cpp
// Memory writing loop in handle_jit_compilation()
for (size_t i = 0; i < finalized_jit_instructions.size(); ++i) {
    const auto& instr = finalized_jit_instructions[i];
    
    switch (instr.segment) {
        case SegmentType::CODE:
            size_t offset = instr.address - reinterpret_cast<size_t>(code_buffer_base);
            char* dest = static_cast<char*>(code_buffer_base) + offset;
            memcpy(dest, &instr.encoding, sizeof(uint32_t));  // <-- CRITICAL POINT
            break;
    }
}
```

### 7. Code Listing Generation (CodeLister.cpp)
- **Input**: Finalized instructions
- **Output**: Assembly listing (.lst file)
- **Key Files**: `CodeLister.cpp`, `CodeLister.h`
- **Transformation**: Instruction objects → Human-readable assembly + hex
- **Corruption Risk**: **LOW** - Display only, but could show corrupted values
- **Purpose**: Debugging and verification

### 8. JIT Execution (JITExecutor.cpp)
- **Input**: Executable memory containing machine code
- **Output**: Program execution
- **Key Files**: `JITExecutor.cpp`, `JITMemoryManager.cpp`
- **Transformation**: Memory → CPU execution
- **Corruption Risk**: None (execution phase)

## Critical Corruption Points

Based on the conversation summary and code analysis, the most likely corruption points are:

### 1. BitPatcher Inline Assembly (HIGH RISK)
- **Location**: `BitPatcher.cpp:patch()`
- **Issue**: Complex ARM64 inline assembly manipulating 32-bit values
- **Symptom**: Correct base opcode (0x0ea1d400) could be corrupted during bit field manipulation
- **Debug**: Add debug output before/after each patch() call

### 2. JIT Memory Writing (HIGH RISK)
- **Location**: `main.cpp:handle_jit_compilation()`
- **Issue**: memcpy of 32-bit instruction encoding to memory
- **Symptom**: Encoding corruption during memory write (0x0ea1d400 → 0xea1d4000)
- **Debug**: Add debug output showing encoding value before memcpy

### 3. Endian Issues (MEDIUM RISK)
- **Location**: Any 32-bit value handling
- **Issue**: Incorrect endian interpretation during encoding/writing
- **Symptom**: Byte order reversal or bit shifting
- **Debug**: Check if encoding values are being interpreted as different endianness

## Debugging Strategy

To identify where the corruption occurs, add debug output at these key points:

### 1. VectorCodeGen Output
```cpp
// In VectorCodeGen::fsub_vector_2s()
uint32_t final_encoding = patcher.get_value();
std::cerr << "[DEBUG] FSUB encoding after BitPatcher: 0x" << std::hex << final_encoding << std::dec << std::endl;
```

### 2. Linker Input/Output
```cpp
// In Linker::process()
for (const auto& instr : instructions) {
    if (instr.opcode == InstructionDecoder::OpType::FSUB) {
        std::cerr << "[DEBUG] FSUB encoding in linker: 0x" << std::hex << instr.encoding << std::dec << std::endl;
    }
}
```

### 3. JIT Memory Writing
```cpp
// In handle_jit_compilation() before memcpy
if (instr.opcode == InstructionDecoder::OpType::FSUB) {
    std::cerr << "[DEBUG] FSUB encoding before memcpy: 0x" << std::hex << instr.encoding << std::dec << std::endl;
    std::cerr << "[DEBUG] Writing to address: 0x" << std::hex << instr.address << std::dec << std::endl;
}
```

### 4. CodeLister Display
```cpp
// In CodeLister to verify what's actually in memory
// Read back the memory contents and compare with expected encoding
```

## Next Steps

1. **Add minimal debug output** at the three critical points above
2. **Use test_fsub_only.bcl** as a simple test case
3. **Trace a single FSUB instruction** through the entire pipeline
4. **Compare expected vs. actual encoding** at each stage
5. **Focus on the shift pattern**: 0x0ea1d400 → 0xea1d4000 suggests a 4-bit shift

## File Dependencies

Key files involved in instruction flow:
- `VectorCodeGen.cpp/h` - NEON instruction generation
- `BitPatcher.cpp/h` - Bit-level instruction encoding
- `InstructionStream.cpp/h` - Instruction container
- `Linker.cpp/h` - Address resolution and relocation
- `main.cpp` - JIT memory writing
- `CodeLister.cpp/h` - Debug output generation
- `JITMemoryManager.cpp/h` - Memory management
- `Encoder.h` - Instruction data structure

This comprehensive trace should help identify exactly where the NEON FPAIR FSUB encoding corruption occurs.