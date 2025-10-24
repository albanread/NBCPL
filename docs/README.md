# NewBCPL Documentation Index

This directory contains technical documentation for the NewBCPL compiler and runtime system.

## Core Technical Notes

### Language Implementation
- **[tech_note_lists.md](tech_note_lists.md)** - Comprehensive analysis of the lists implementation, covering AST representation, dual code generation, runtime data structures, and performance characteristics
- **[manifestlist.md](manifestlist.md)** - Documentation on read-only MANIFESTLIST functionality
- **[enhancelists.md](enhancelists.md)** - List enhancement proposals and implementation notes

### Compiler Architecture
- **[SymbolTable.md](SymbolTable.md)** - Symbol table implementation and scoping rules
- **[Preprocessor.md](Preprocessor.md)** - Preprocessor functionality and macro system
- **[VENEER_SYSTEM_GUIDE.md](VENEER_SYSTEM_GUIDE.md)** - Veneer system for external function integration

### Code Generation & Optimization
- **[foreach_optimization.md](foreach_optimization.md)** - FOREACH loop optimizations and control flow analysis
- **[modular_reduction_system.md](modular_reduction_system.md)** - Modular reduction system architecture
- **[NeonReducer_Architecture.md](NeonReducer_Architecture.md)** - NEON SIMD reducer implementation
- **[Vector_Type_Reducer_Integration.md](Vector_Type_Reducer_Integration.md)** - Vector type system integration

### Runtime System
- **[runtime.md](runtime.md)** - Runtime system overview and API documentation
- **[RuntimeFunctionProcess.md](RuntimeFunctionProcess.md)** - Runtime function integration process

### Platform-Specific
- **[ARM64_CALLING_CONVENTION_BUG_ANALYSIS.md](ARM64_CALLING_CONVENTION_BUG_ANALYSIS.md)** - ARM64 calling convention implementation details and bug analysis

### Legacy & Cleanup
- **[Legacy_Reducer_Cleanup_Summary.md](Legacy_Reducer_Cleanup_Summary.md)** - Summary of legacy reducer cleanup and modernization

## Documentation Organization

### By Topic

**Memory Management:**
- Lists implementation (tech_note_lists.md)
- Runtime system (runtime.md)
- SAMM integration (see runtime.md)

**Type System:**
- Symbol table (SymbolTable.md)
- Vector types (Vector_Type_Reducer_Integration.md)
- Lists type inference (tech_note_lists.md)

**Code Generation:**
- FOREACH optimization (foreach_optimization.md)
- NEON SIMD (NeonReducer_Architecture.md)
- Lists dual generation (tech_note_lists.md)

**Language Features:**
- Lists and MANIFESTLIST (tech_note_lists.md, manifestlist.md)
- Preprocessor (Preprocessor.md)
- External functions (VENEER_SYSTEM_GUIDE.md)

### By Development Phase

**Core Language:**
1. Symbol table and scoping
2. Type system and inference
3. Lists implementation
4. Runtime integration

**Optimizations:**
1. FOREACH loop optimization
2. Vector/SIMD code generation
3. Modular reduction system
4. Platform-specific optimizations

**Integration:**
1. External function veneers
2. Runtime function process
3. ARM64 calling conventions
4. Legacy system cleanup

## Quick Reference

### For New Contributors
Start with:
1. [SymbolTable.md](SymbolTable.md) - Understanding scoping and symbol resolution
2. [tech_note_lists.md](tech_note_lists.md) - Major language feature implementation
3. [runtime.md](runtime.md) - Runtime system overview

### For Language Users
Reference:
1. [manifestlist.md](manifestlist.md) - List usage patterns
2. [Preprocessor.md](Preprocessor.md) - Macro and preprocessing features
3. [runtime.md](runtime.md) - Available runtime functions

### For Performance Analysis
Focus on:
1. [tech_note_lists.md](tech_note_lists.md) - Lists performance characteristics
2. [foreach_optimization.md](foreach_optimization.md) - Loop optimization details
3. [NeonReducer_Architecture.md](NeonReducer_Architecture.md) - SIMD performance
4. [ARM64_CALLING_CONVENTION_BUG_ANALYSIS.md](ARM64_CALLING_CONVENTION_BUG_ANALYSIS.md) - Platform performance

## Contributing to Documentation

When adding new documentation:

1. **Technical Notes**: Use the `tech_note_*.md` naming convention for comprehensive implementation analyses
2. **Feature Documentation**: Use descriptive names for specific feature documentation
3. **Architecture Guides**: Include diagrams and code examples where helpful
4. **Update This Index**: Add new documents to the appropriate sections above

## Related Documentation

- **Main README**: [../README.md](../README.md) - Project overview and getting started
- **Examples**: [../examples/](../examples/) - Code examples and test cases
- **Tests**: [../tests/](../tests/) - Test suite and validation examples

---

*This documentation covers the NewBCPL compiler as of December 2024. For the most current information, please check the git commit history and recent changes.*