#include "DataTypes.h"
#define _DARWIN_C_SOURCE // Required for ucontext.h on macOS
#include "analysis/LiveIntervalPass.h"
#include "analysis/LinearScanAllocator.h"
#include <iostream>
#include <unistd.h>
#include "AssemblerData.h"
#include "VeneerManager.h"

#include "ClassTable.h"
#include "ClassPass.h"
#include "analysis/SignatureAnalysisVisitor.h"

#include "RuntimeManager.h"
#include "runtime/RuntimeBridge.h"

#include "RuntimeSymbols.h"
#include <iostream>
#include "HeapManager/HeapManager.h"
#include <fstream>
#include <string>
#include <vector>
#include "HeapManager/heap_manager_defs.h"
#include <memory>
#include <stdexcept>
#include <cstring>
#include <csignal>
#include <cstdlib>
#include <sys/mman.h>
#include <unistd.h>
#include "CodeLister.h"
#include "StrengthReductionPass.h"
#include <sys/ucontext.h>
#include <sys/wait.h>
#include <execinfo.h>
#include <unistd.h>
#include "ClassTable.h"
#include "SymbolTable.h" // Added for stack canary control
#include "runtime/BCPLError.h"

// --- Project Headers ---
#include "Preprocessor.h"
#include "AST.h"
#include "SymbolLogger.h"


#include "AssemblyWriter.h"
#include "CodeBuffer.h"
#include "LocalOptimizationPass.h"
#include "ConstantFoldingPass.h"


#include "LoopInvariantCodeMotionPass.h"
// #include "passes/ShortCircuitPass.h"  // Temporarily disabled to test crash
#include "DataGenerator.h"
#include "DebugPrinter.h"
#include "StringTable.h"
#include "StringLiteralLiftingPass.h"
#include "InstructionStream.h"
#include "JITExecutor.h"
#include "LabelManager.h"
#include "Lexer.h"
#include "LexerDebug.h"
#include "Linker.h"
#include "NewCodeGenerator.h"
#include "Parser.h"
#include "RegisterManager.h"
#include "RuntimeManager.h"
#include "SignalSafeUtils.h"
#include "analysis/ASTAnalyzer.h"
#include "passes/ManifestResolutionPass.h"
#include "passes/GlobalInitializerPass.h"
#include "LivenessAnalysisPass.h"
#include "passes/MethodInliningPass.h"
#include "CFGBuilderPass.h"
#include "passes/CFGSimplificationPass.h"
#include "BoundsCheckingPass.h"  // Re-enabled bounds checking pass
#include "CreateMethodReorderPass.h"  // Fix call interval bug in CREATE methods
#include "HeapManager/HeapManager.h"
#include "analysis/SymbolDiscoveryPass.h"
#include "runtime.h"
#include "version.h"
#include "PeepholeOptimizer.h"


// --- Formatter ---
#include "format/CodeFormatter.h"

// --- Encoder Testing ---
#include "testing/encoder_validation/EncoderTester.h"

// Utility
//
void dump_class_table(const ClassTable& table) {
    std::cout << "\n--- Class Table Dump ---" << std::endl;
    if (table.entries().empty()) {
        std::cout << "(Table is empty)" << std::endl;
        std::cout << "--- End Class Table Dump ---\n" << std::endl;
        return;
    }

    // Iterate through all classes in the table
    for (const auto& pair : table.entries()) {
        const auto& entry = pair.second;
        if (!entry) continue;

        // Print Class Name and Inheritance
        std::cout << "\nClass: " << entry->name;
        if (!entry->parent_name.empty()) {
            std::cout << " (extends " << entry->parent_name << ")";
        }
        std::cout << "\n  - Instance Size: " << entry->instance_size << " bytes";

        // Print Member Variables
        std::cout << "\n  - Members (" << entry->member_variables.size() << "):";
        if (entry->member_variables.empty()) {
            std::cout << " (none)";
        } else {
            for (const auto& mem_pair : entry->member_variables) {
                const auto& member = mem_pair.second;
                std::cout << "\n    - " << member.name << " (Type: " << vartype_to_string(member.type)
                          << ", Offset: " << member.offset << ")";
            }
        }

        // Print VTable Blueprint
        std::cout << "\n  - VTable (" << entry->vtable_blueprint.size() << " entries):";
        if (entry->vtable_blueprint.empty()) {
            std::cout << " (none)";
        } else {
            for (size_t i = 0; i < entry->vtable_blueprint.size(); ++i) {
                const auto& method_name = entry->vtable_blueprint[i];
                ClassMethodInfo* method_info = entry->findMethod(method_name, false);
                if (method_info) {
                    std::cout << "\n    - Slot " << i << ": " << method_info->qualified_name;
                } else {
                    std::cout << "\n    - Slot " << i << ": [ERROR: method not found: " << method_name << "]";
                }
            }
        }
    }
    std::cout << "\n--- End Class Table Dump ---\n" << std::endl;
}


// NewBCPL pragmatic multipass compiler

// --- Global Variables ---
bool g_enable_lexer_trace = false;
bool g_enable_heap_trace = false;
bool g_enable_symbols_trace = false;
std::unique_ptr<CodeBuffer> g_jit_code_buffer;
std::unique_ptr<JITMemoryManager> g_jit_data_manager; // <-- ADD THIS LINE
std::unique_ptr<JITExecutor> g_jit_executor;
std::string g_jit_breakpoint_label;
int g_jit_breakpoint_offset = 0;
std::string g_source_code;

// --- Forward Declarations ---
#include "include/SignalHandler.h"
std::string read_file_content(const std::string& filepath);
bool parse_arguments(int argc, char* argv[], bool& run_jit, bool& generate_asm, bool& exec_mode,
                    bool& enable_opt, bool& enable_tracing,
                    bool& trace_lexer, bool& trace_parser, bool& trace_ast, bool& trace_cfg,
                    bool& trace_codegen, bool& trace_optimizer, bool& trace_liveness,
                    bool& trace_runtime, bool& trace_symbols, bool& trace_heap,
                    bool& trace_preprocessor, bool& enable_preprocessor,
                    bool& dump_jit_stack, bool& enable_peephole, bool& enable_stack_canaries,
                    bool& format_code, bool& trace_class_table, bool& trace_vtables,
                    bool& bounds_checking_enabled, bool& enable_samm, bool& enable_superdisc,
                    bool& use_neon, bool& generate_list, bool& test_encoders,
                    bool& test_encode, std::string& test_encode_name, bool& list_encoders,
                    std::string& input_filepath, std::string& call_entry_name, int& offset_instructions,
                    std::vector<std::string>& include_paths, std::string& runtime_mode);
void handle_static_compilation(bool exec_mode, const std::string& base_name, const InstructionStream& instruction_stream, const DataGenerator& data_generator, bool enable_debug_output, const std::string& runtime_mode, const VeneerManager& veneer_manager, bool generate_list);
void* handle_jit_compilation(void* jit_data_memory_base, InstructionStream& instruction_stream, int offset_instructions, bool enable_debug_output, std::vector<Instruction>* finalized_instructions = nullptr);
void handle_jit_execution(void* code_buffer_base, const std::string& call_entry_name, bool dump_jit_stack, bool enable_debug_output);

// =================================================================================
// Main Execution Pipeline
// =================================================================================
#include "analysis/RetainAnalysisPass.h"

int main(int argc, char* argv[]) {
    ASTAnalyzer& analyzer = ASTAnalyzer::getInstance();
    bool enable_tracing = false;
    if (enable_tracing) {
        std::cout << "Debug: Starting compiler, argc=" << argc << std::endl;
    }
    bool run_jit = false;
    bool generate_asm = false;
    bool exec_mode = false;
    bool enable_opt = true;
    bool dump_jit_stack = false;
    bool enable_preprocessor = true;  // Enabled by default
    bool trace_preprocessor = false;
    bool enable_stack_canaries = false;  // Disabled by default
    bool enable_samm = true;  // SAMM enabled by default to prevent memory leaks
    bool enable_superdisc = true; // CREATE Method Reordering Pass enabled by default

    // Granular tracing flags for different compiler passes
    bool trace_lexer = false;
    bool trace_parser = false;
    bool trace_ast = false;
    bool trace_cfg = false;
    bool trace_codegen = false;
    bool trace_optimizer = false;
    bool trace_liveness = false;
    bool trace_runtime = false;
    bool trace_symbols = false;
    bool trace_heap = false;
    bool enable_peephole = true; // Peephole optimizer enabled by default
    bool trace_class_table = false; // Trace class table flag
    bool trace_vtables = false; // Trace vtable generation
    std::string input_filepath;
    std::string call_entry_name = "START";
    int offset_instructions = 0;
    g_jit_breakpoint_offset = 0;
    std::vector<std::string> include_paths;
    std::string runtime_mode = "jit"; // Default runtime mode
    bool format_code = false; // Add this flag
    bool bounds_checking_enabled = true; // Runtime bounds checking enabled by default
    bool use_neon = true; // NEON SIMD instructions enabled by default
    bool generate_list = false; // Generate listing file with hex opcodes
    bool test_encoders = false; // Encoder validation testing mode
    bool test_encode = false; // Individual encoder testing mode
    std::string test_encode_name; // Name of specific encoder to test
    bool list_encoders = false; // List available encoders mode

    if (enable_tracing) {
        std::cout << "Debug: About to parse arguments\n";
    }
    try {
        if (!parse_arguments(argc, argv, run_jit, generate_asm, exec_mode, enable_opt, enable_tracing,
                            trace_lexer, trace_parser, trace_ast, trace_cfg, trace_codegen,
                            trace_optimizer, trace_liveness, trace_runtime, trace_symbols, trace_heap,
                            trace_preprocessor, enable_preprocessor, dump_jit_stack, enable_peephole,
                            enable_stack_canaries,
                            format_code,
                            trace_class_table,
                            trace_vtables,
                            bounds_checking_enabled, enable_samm,
                            enable_superdisc, use_neon, generate_list, test_encoders,
                            test_encode, test_encode_name, list_encoders,
                            input_filepath, call_entry_name, offset_instructions, include_paths, runtime_mode)) {
            if (enable_tracing) {
                std::cout << "Debug: parse_arguments returned false\n";
            }
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception in parse_arguments: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown exception in parse_arguments\n";
        return 1;
    }
    if (enable_tracing) {
        std::cout << "Debug: Arguments parsed successfully\n";
    }

    // Check if we should run encoder validation tests
    if (test_encoders) {
        EncoderTester tester;
        bool success = tester.run_all_tests();
        return success ? 0 : 1;
    }
    
    // Check if we should test a specific encoder
    if (test_encode) {
        EncoderTester tester;
        bool success;
        
        // Check if it's a pattern (contains *)
        if (test_encode_name.find('*') != std::string::npos) {
            success = tester.run_pattern_tests(test_encode_name);
        } else {
            success = tester.run_single_test(test_encode_name);
        }
        return success ? 0 : 1;
    }
    
    // Check if we should list available encoders
    if (list_encoders) {
        EncoderTester tester;
        std::vector<std::string> encoders = tester.list_available_encoders();
        
        std::cout << "\n=== Available NEON Encoders ===" << std::endl;
        std::cout << "Total: " << encoders.size() << " encoders\n" << std::endl;
        
        // Group by operation type for better display
        std::cout << "Floating Point Minimum Pairwise (FMINP):" << std::endl;
        for (const std::string& name : encoders) {
            if (name.find("fminp_") == 0) {
                std::cout << "  " << name << std::endl;
            }
        }
        
        std::cout << "\nFloating Point Maximum Pairwise (FMAXP):" << std::endl;
        for (const std::string& name : encoders) {
            if (name.find("fmaxp_") == 0) {
                std::cout << "  " << name << std::endl;
            }
        }
        
        std::cout << "\nFloating Point Addition Pairwise (FADDP):" << std::endl;
        for (const std::string& name : encoders) {
            if (name.find("faddp_") == 0) {
                std::cout << "  " << name << std::endl;
            }
        }
        
        std::cout << "\nInteger Addition Pairwise (ADDP):" << std::endl;
        for (const std::string& name : encoders) {
            if (name.find("addp_") == 0) {
                std::cout << "  " << name << std::endl;
            }
        }
        
        std::cout << "\nSigned Integer Minimum Pairwise (SMINP):" << std::endl;
        for (const std::string& name : encoders) {
            if (name.find("sminp_") == 0) {
                std::cout << "  " << name << std::endl;
            }
        }
        
        std::cout << "\nScalar Operations:" << std::endl;
        for (const std::string& name : encoders) {
            if (name.find("scalar_") == 0) {
                std::cout << "  " << name << std::endl;
            }
        }
        
        std::cout << "\nFloating Point Operations:" << std::endl;
        for (const std::string& name : encoders) {
            if (name.find("fp_") == 0) {
                std::cout << "  " << name << std::endl;
            }
        }
        
        std::cout << "\nVector Floating Point Operations:" << std::endl;
        for (const std::string& name : encoders) {
            if (name.find("vec_") == 0) {
                std::cout << "  " << name << std::endl;
            }
        }
        
        std::cout << "\nMemory Operations:" << std::endl;
        for (const std::string& name : encoders) {
            if (name.find("mem_") == 0) {
                std::cout << "  " << name << std::endl;
            }
        }
        
        std::cout << "\nBranch Operations:" << std::endl;
        for (const std::string& name : encoders) {
            if (name.find("branch_") == 0) {
                std::cout << "  " << name << std::endl;
            }
        }
        
        std::cout << "\nUsage Examples:" << std::endl;
        std::cout << "\nSigned Integer Maximum (SMAX):" << std::endl;
        for (const std::string& name : encoders) {
            if (name.find("smax_") == 0) {
                std::cout << "  " << name << std::endl;
            }
        }
        
        std::cout << "\nInteger Addition (ADD):" << std::endl;
        for (const std::string& name : encoders) {
            if (name.find("add_") == 0) {
                std::cout << "  " << name << std::endl;
            }
        }
        
        std::cout << "\nInteger Subtraction (SUB):" << std::endl;
        for (const std::string& name : encoders) {
            if (name.find("sub_") == 0) {
                std::cout << "  " << name << std::endl;
            }
        }
        
        std::cout << "\nFloating Point Minimum (FMIN):" << std::endl;
        for (const std::string& name : encoders) {
            if (name.find("fmin_") == 0) {
                std::cout << "  " << name << std::endl;
            }
        }
        
        std::cout << "\nUsage Examples:" << std::endl;
        std::cout << "  ./NewBCPL --test-encode fminp_4s        # Test single encoder" << std::endl;
        std::cout << "  ./NewBCPL --test-encode \"fminp_*\"       # Test FMINP family" << std::endl;
        std::cout << "  ./NewBCPL --test-encode \"*_4s\"         # Test all 4S layouts" << std::endl;
        std::cout << "  ./NewBCPL --test-encode \"scalar_*\"     # Test all scalar operations" << std::endl;
        std::cout << "  ./NewBCPL --test-encode \"vec_*\"        # Test all vector operations" << std::endl;
        std::cout << "  ./NewBCPL --test-encode \"mem_*\"        # Test all memory operations" << std::endl;
        std::cout << "  ./NewBCPL --test-encoders               # Test all encoders" << std::endl;
        
        return 0;
    }

    // Set individual trace flags based on global tracing or specific flags
    g_enable_lexer_trace = enable_tracing || trace_lexer;
    g_enable_heap_trace = enable_tracing || trace_heap;
    g_enable_symbols_trace = enable_tracing || trace_symbols;
    bool enable_debug_output = enable_tracing || trace_codegen; // <-- Declare as local variable
    HeapManager::getInstance().setTraceEnabled(enable_tracing || trace_heap);

    // SAMM (heap manager) is used by JIT code, not the compiler itself
    // Enable SAMM (Scope Aware Memory Management) - enabled by default
    HeapManager::getInstance().setSAMMEnabled(enable_samm);
    if (enable_tracing || trace_heap) {
        std::cout << "SAMM (Scope Aware Memory Management): " << (enable_samm ? "ENABLED" : "DISABLED") << std::endl;
    }

    // Register SAMM shutdown handler to ensure clean exit
    std::atexit([]() {
        HeapManager::getInstance().waitForSAMM();
        HeapManager::getInstance().shutdown();
    });
    if (enable_tracing || trace_runtime) {
        RuntimeManager::instance().enableTracing();
    }

    // Apply stack canary setting
    CallFrameManager::setStackCanariesEnabled(enable_stack_canaries);

    // Print version if any tracing is enabled
    if (enable_tracing || trace_lexer || trace_parser || trace_ast || trace_cfg ||
        trace_codegen || trace_optimizer || trace_liveness || trace_runtime ||
        trace_symbols || trace_heap || trace_vtables) {
        print_version();
    }

    SignalHandler::setup();

    try {
        if (enable_preprocessor) {
            Preprocessor preprocessor;
            preprocessor.enableDebug(trace_preprocessor);

            // Add any include paths specified on command line
            for (const auto& path : include_paths) {
                preprocessor.addIncludePath(path);
            }

            // Add current directory as default include path
            char current_dir[PATH_MAX];
            if (getcwd(current_dir, sizeof(current_dir)) != nullptr) {
                preprocessor.addIncludePath(std::string(current_dir));
            }

            // Process the file and handle GET directives
            g_source_code = preprocessor.process(input_filepath);
        } else {
            g_source_code = read_file_content(input_filepath);
        }

        if (enable_tracing) {
            std::cout << "Compiling this source Code:\n" << g_source_code << std::endl;
        }
        // Initialize and register runtime functions using the runtime bridge
        runtime::initialize_runtime();
        runtime::register_runtime_functions();

        if (enable_tracing) {
            std::cout << "Using " << runtime::get_runtime_version() << std::endl;
        }

        // --- Parsing -> initial AST Construction ---
        Lexer lexer(g_source_code, enable_tracing || trace_lexer);
        Parser parser(lexer, enable_tracing || trace_parser);
        ProgramPtr ast;
        try {
            ast = parser.parse_program();
        } catch (const std::exception& e) {
            std::cerr << "\nFatal parsing error: " << e.what() << std::endl;
            return 1;
        }

        // ** Parser fail fast - ERROR HANDLING BLOCK **
        if (!parser.getErrors().empty() || parser.hasFatalError() || !ast) {
            std::cerr << "\nCompilation failed due to syntax errors:" << std::endl;
            for (const auto& err : parser.getErrors()) {
                std::cerr << "  " << err << std::endl;
            }
            if (parser.getErrors().empty() && !ast) {
                std::cerr << "  Parser returned null AST (unknown error)" << std::endl;
            }
            if (parser.hasFatalError()) {
                std::cerr << "  Parser encountered a fatal error" << std::endl;
            }
            std::cerr << "\nCompilation terminated." << std::endl;
            return 1; // Exit before code generation
        }

        if (enable_tracing || trace_parser) {
            std::cout << "Parsing complete. AST built.\n";
        }


        // --- RETAIN Analysis Pass ---
        {
            RetainAnalysisPass retain_pass;
            SymbolTable* symbol_table = new SymbolTable();
            retain_pass.run(*ast, *symbol_table);
        }

        // --- Create tables once in main ---
        auto symbol_table = std::make_unique<SymbolTable>();
        auto class_table = std::make_unique<ClassTable>();

        // Run the ClassPass to discover and lay out all classes.
        // It takes references to the tables and populates them.
        ClassPass class_pass(*class_table, *symbol_table);
        class_pass.set_debug(trace_class_table);  // Enable debug output only if trace_class_table is set
        class_pass.run(*ast);

        if (trace_class_table) {
            dump_class_table(*class_table);
        }

        // --- Formatting Logic ---
        if (format_code) {
            CodeFormatter formatter;
            std::cout << formatter.format(*ast);
            return 0; // Exit after formatting
        }

        // display early ast before most passes
        if (enable_tracing || trace_ast) {
            std::cout << "\n--- Initial Abstract Syntax Tree ---\n";
            DebugPrinter printer;
            printer.print(*ast);
            std::cout << "----------------------------------\n\n";
        }

        // --- Semantic and Optimization Passes ---

        // Manifest constants
        static std::unordered_map<std::string, int64_t> g_global_manifest_constants;
        if (enable_tracing || trace_optimizer) std::cout << "Applying Manifest Resolution Pass...\n";
        ManifestResolutionPass manifest_pass(g_global_manifest_constants);
        ast = manifest_pass.apply(std::move(ast));

        // --- Apply Global Initializer Pass ---
        // this injects global initializers into a function, run by START
        if (enable_tracing || trace_optimizer) std::cout << "Applying Global Initializer Pass...\n";
        GlobalInitializerPass global_init_pass;
        global_init_pass.run(*ast);

        if (enable_tracing || trace_ast) {
            std::cout << "\n--- AST After Global Initializer Injection ---\n";
            DebugPrinter printer;
            printer.print(*ast);
            std::cout << "------------------------------------------\n\n";
        }

        // ---  Build Symbol Table ---
        if (enable_tracing || trace_symbols) std::cout << "Building symbol table...\n";
        SymbolDiscoveryPass symbol_discovery_pass(enable_tracing || trace_symbols);
        symbol_discovery_pass.build_into(*ast, *symbol_table, *class_table);

        // --- DEBUG: Dump symbol table after symbol discovery ---
        if (enable_tracing || trace_symbols) {
            std::cout << "\n=== SYMBOL TABLE DUMP AFTER SYMBOL DISCOVERY ===\n";
            std::cout << symbol_table->toString() << std::endl;
            std::cout << "================================================\n\n";
        }

        // Register runtime functions in symbol table
        if (enable_tracing || trace_symbols || trace_runtime) std::cout << "Registering runtime functions in symbol table...\n";
        RuntimeSymbols::registerAll(*symbol_table);

        // (CSE pass removed; now handled after CFG construction)
        // --- String Table Pass: Construct early and wire to pipeline ---
        StringTable string_table;

        // Get analyzer instance for use in optimization passes and analysis
        ASTAnalyzer& analyzer = ASTAnalyzer::getInstance();

        // --- Local Optimization Pass (CSE/LVN) using StringTable ---
        if (enable_opt) {
            if (enable_tracing || trace_optimizer) std::cout << "Optimization enabled. Applying passes...\n";
            ConstantFoldingPass constant_folding_pass(g_global_manifest_constants, symbol_table.get(), trace_optimizer);
            ast = constant_folding_pass.apply(std::move(ast));

            // Clear FOR loop state before StrengthReductionPass to prevent corruption
            ASTAnalyzer& analyzer_for_clearing = ASTAnalyzer::getInstance();
            while (!analyzer_for_clearing.active_for_loop_scopes_.empty()) {
                analyzer_for_clearing.active_for_loop_scopes_.pop();
            }
            analyzer_for_clearing.for_variable_unique_aliases_.clear();
            while (!analyzer_for_clearing.loop_context_stack_.empty()) {
                analyzer_for_clearing.loop_context_stack_.pop();
            }
            if (enable_tracing || trace_optimizer) {
                std::cout << "[FIX] Cleared FOR loop state before StrengthReductionPass to prevent corruption" << std::endl;
            }

            StrengthReductionPass strength_reduction_pass(trace_optimizer);
            strength_reduction_pass.run(*ast);

            // --- Method Inlining Pass: Inline trivial accessors/setters before codegen ---
            MethodInliningPass method_inliner(analyzer);
            method_inliner.run(*ast);
            // --- End Method Inlining Pass ---
        }

        // TEMPORARILY DISABLED: Boolean short-circuiting pass causing startup crash
        if (enable_tracing || trace_optimizer) std::cout << "SKIPPED: Boolean Short-Circuiting Pass (startup crash)...\n";

        // ShortCircuitPass short_circuit_pass(g_global_manifest_constants);
        // ast = short_circuit_pass.apply(std::move(ast));

        // if (enable_tracing || trace_ast) {
        //     std::cout << "\n--- AST After Short-Circuiting Pass ---\n";
        //     DebugPrinter printer;
        //     printer.print(*ast);
        //     std::cout << "----------------------------------\n\n";
        // }

// --- TWO-PASS ANALYSIS FOR PROPER SCOPING ---
// Pass 1: Signature analysis to establish all parameter types first
if (enable_tracing || trace_ast) std::cout << "Pass 1: Analyzing function signatures...\n";
SignatureAnalysisVisitor signature_visitor(symbol_table.get(), analyzer, enable_tracing || trace_ast);
signature_visitor.analyze_signatures(*ast);

// Loop-Invariant Code Motion Pass (LICM)
// - run after signature analysis so function metrics exist
if (enable_opt) {
    LoopInvariantCodeMotionPass licm_pass(
        g_global_manifest_constants,
        *symbol_table,
        analyzer
    );
    ast = licm_pass.apply(std::move(ast));
}

// check parameter types registration
if (enable_tracing || trace_ast) {
    std::cout << "Debug: Checking function metrics after signature analysis...\n";
    const auto& metrics = analyzer.get_function_metrics();
    for (const auto& [func_name, func_metrics] : metrics) {
        std::cout << "Function: " << func_name << std::endl;
        for (const auto& [param_name, param_type] : func_metrics.parameter_types) {
            std::cout << "  Parameter: " << param_name << " -> " << static_cast<int>(param_type) << std::endl;
        }
    }
}

// major pass 2
// Pass 2: Full AST analysis with parameter types already established
if (enable_tracing || trace_ast) std::cout << "Pass 2: Full AST analysis...\n";
analyzer.set_trace_enabled(enable_tracing || trace_ast);

// Clear FOR loop state before main analysis to prevent corruption
// This ensures that any stale state from previous optimization passes doesn't leak
while (!analyzer.active_for_loop_scopes_.empty()) {
    analyzer.active_for_loop_scopes_.pop();
}
analyzer.for_variable_unique_aliases_.clear();
while (!analyzer.loop_context_stack_.empty()) {
    analyzer.loop_context_stack_.pop();
}
if (enable_tracing || trace_ast) {
    std::cout << "Cleared FOR loop state before analysis to prevent corruption" << std::endl;
}

analyzer.analyze(*ast, symbol_table.get(), class_table.get());

// Check for semantic errors after analysis
// early exit, fail fast
const auto& semantic_errors = analyzer.getSemanticErrors();
if (!semantic_errors.empty()) {
    std::cerr << "\nCompilation failed due to the following semantic error(s):" << std::endl;
    for (const auto& error : semantic_errors) {
        std::cerr << "  " << error << std::endl;
    }
    return 1; // Exit before continuing compilation
}

// --- Synchronize improved type info from analyzer to symbol table ---

if (enable_tracing || trace_ast) {
   std::cout << "Initial AST analysis complete.\n";
   analyzer.print_report();
}

// --- String Literal Lifting Pass: run after symbol discovery/type analysis, before CFG/liveness ---
{
    if (enable_tracing || trace_optimizer) std::cout << "Running StringLiteralLiftingPass (string literal lifting)...\n";
    StringLiteralLiftingPass string_lifting_pass(&string_table);
    string_lifting_pass.run(*ast, *symbol_table, analyzer);
}

// --- Run Local Optimization Pass (CSE/LVN) after analyzer.analyze so function metrics are available ---
if (enable_opt) {
    if (enable_tracing || trace_optimizer) std::cout << "Optimization enabled. Applying passes...\n";
    LocalOptimizationPass local_opt_pass(&string_table, trace_optimizer);
    local_opt_pass.run(*ast, *symbol_table, analyzer);
}

if (trace_class_table) {
    // This function will be defined above main()
    dump_class_table(*class_table);
}

// Again clear FOR loop state before AST transformation to prevent corruption
while (!analyzer.active_for_loop_scopes_.empty()) {
    analyzer.active_for_loop_scopes_.pop();
}
analyzer.for_variable_unique_aliases_.clear();
while (!analyzer.loop_context_stack_.empty()) {
    analyzer.loop_context_stack_.pop();
}
if (enable_tracing || trace_ast) {
    std::cout << "Cleared FOR loop state before AST transformation to prevent corruption" << std::endl;
}

// transfor AST
analyzer.transform(*ast);
if (enable_tracing || trace_ast) std::cout << "AST transformation complete.\n";

        // --- CREATE METHOD REORDERING PASS ---
        // Kludge to avoid interval bug in super call
        // Fix call interval bug by moving SUPER.CREATE calls after _this member assignments
        if (enable_superdisc) {
            if (enable_tracing || trace_optimizer) std::cout << "Applying CREATE Method Reordering Pass...\n";
            CreateMethodReorderPass create_reorder_pass(class_table.get(), enable_tracing || trace_optimizer);
            ast = create_reorder_pass.transform(std::move(ast));
            if (enable_tracing || trace_optimizer) std::cout << "CREATE Method Reordering Pass complete.\n";
        } else {
            if (enable_tracing || trace_optimizer) std::cout << "SKIPPED: CREATE Method Reordering Pass (--no-superdisc)\n";
        }

        // --- COMPILE-TIME BOUNDS CHECKING ---
        // bounds checking pass
        if (bounds_checking_enabled) {
            if (enable_tracing) std::cout << "Running compile-time bounds checking...\n";
            BoundsCheckingPass bounds_checker(symbol_table.get(), true);
            bounds_checker.analyze(*ast);

            if (bounds_checker.has_errors()) {
                bounds_checker.print_errors();
                std::cerr << "Compilation failed due to bounds checking errors." << std::endl;
                return 1;
            }
            if (enable_tracing) std::cout << "Compile-time bounds checking complete.\n";
        }

        // AST -> CFG call flow graph builder
        if (enable_tracing || trace_cfg) std::cout << "Building Control Flow Graphs...\n";

        // Clear FOR loop state before CFGBuilderPass to prevent corruption
        while (!analyzer.active_for_loop_scopes_.empty()) {
            analyzer.active_for_loop_scopes_.pop();
        }
        analyzer.for_variable_unique_aliases_.clear();
        while (!analyzer.loop_context_stack_.empty()) {
            analyzer.loop_context_stack_.pop();
        }

        if (enable_tracing || trace_cfg) {
            std::cout << "Cleared FOR loop state before CFGBuilderPass to prevent corruption" << std::endl;
        }

        // Initial call flow graph
        CFGBuilderPass cfg_builder(symbol_table.get(), enable_tracing || trace_cfg);
        cfg_builder.build(*ast);

        // CFG optimizations
        // --- CFG Simplification Pass (Branch Chaining Elimination) ---
        if (enable_opt) {
            if (enable_tracing || trace_cfg) std::cout << "Applying CFG Simplification Pass (Branch Chaining)...\n";
            CFGSimplificationPass cfg_simplification_pass(enable_tracing || trace_cfg);
            cfg_simplification_pass.run(const_cast<std::unordered_map<std::string, std::unique_ptr<ControlFlowGraph>>&>(cfg_builder.get_cfgs()));
        }


        // --- SECOND LIVENESS ANALYSIS (after cleanup blocks) ---
        // Re-run Liveness Analysis and LiveIntervalPass on the MODIFIED CFG.
        // This is crucial because CFGBuilderPass may have added new blocks and statements
        // (like scope cleanup blocks) that the original analysis did not see.
        if (enable_tracing || trace_liveness) {
            std::cout << "\n[INFO] Re-running Liveness Analysis on modified CFG...\n";
        }

        // Liveness
        LivenessAnalysisPass final_liveness_analyzer(cfg_builder.get_cfgs(), symbol_table.get(), enable_tracing || trace_liveness);
        final_liveness_analyzer.run();

        if (enable_tracing || trace_liveness) {
            final_liveness_analyzer.print_results();
        }

        // Re-build the live intervals based on the new, correct liveness information.
        // Note: LiveIntervalPass will be created later in the upfront allocation section
        // The rest of the register allocation pipeline will now use these corrected intervals.

        if (enable_tracing || trace_cfg) {
            const auto& cfgs = cfg_builder.get_cfgs();
            for (const auto& pair : cfgs) {
                pair.second->print_cfg();
            }
        }

        // final pre-code gen AST

        if (enable_tracing || trace_ast) {
            std::cout << "\n--- AST Dump Immediately Before Code Generation ---\n";
            DebugPrinter printer;
            printer.print(*ast);
            std::cout << "----------------------------------\n\n";
        }

        // calculate register pressure
        if (enable_tracing || trace_liveness) std::cout << "Updating register pressure from liveness data...\n";
        auto pressure_results = final_liveness_analyzer.calculate_register_pressure();

        // Get a mutable reference to the analyzer's metrics
        auto& function_metrics = ASTAnalyzer::getInstance().get_function_metrics_mut();

        for (const auto& pair : pressure_results) {
            const std::string& func_name = pair.first;
            int pressure = pair.second;

            auto it = function_metrics.find(func_name);
            if (it != function_metrics.end()) {
                it->second.max_live_variables = pressure;
            } else {
                std::cerr << "Main Error: Function metrics not found for: " << func_name << std::endl;
            }
        }

        // --- UPFRONT ALLOCATION FOR ALL FUNCTIONS ---
        if (enable_tracing || trace_codegen) {
            std::cout << "\n[INFO] Building Live Intervals for all functions...\n";
        }
        LiveIntervalPass interval_pass(symbol_table.get(), enable_tracing || trace_liveness);
        for (const auto& pair : cfg_builder.get_cfgs()) {
            interval_pass.run(*pair.second, final_liveness_analyzer, pair.first);
        }

        // Create the register allocator and the master allocation map here, BEFORE code generation.
        if (enable_tracing || trace_codegen) {
            std::cout << "\n[INFO] Performing Linear Scan Register Allocation for ALL functions...\n";
        }
        LinearScanAllocator register_allocator(analyzer, enable_tracing || trace_codegen);
        std::map<std::string, std::map<std::string, LiveInterval>> all_allocations;

        // Iterate through all functions and allocate registers for each one.
        for (const auto& pair : cfg_builder.get_cfgs()) {
            const std::string& func_name = pair.first;
            const auto& intervals = interval_pass.getIntervalsFor(func_name);

            // CRITICAL FIX: Only use true variable registers for variable allocation
            // Scratch registers (X9-X15) must be reserved exclusively for scratch allocation
            std::vector<std::string> all_int_regs = RegisterManager::VARIABLE_REGS;  // Callee-saved (X19-X27) only

            std::vector<std::string> all_fp_regs = RegisterManager::FP_VARIABLE_REGS;  // Callee-saved (D8-D15) only

            // The result of each allocation is stored in the master map.
            all_allocations[func_name] = register_allocator.allocate(
                intervals, all_int_regs, all_fp_regs, func_name
            );
        }

        // --- SYNC REGISTER MANAGER WITH ALLOCATOR DECISIONS ---
        // This ensures RegisterManager knows which registers are reserved by LinearScanAllocator
        // and prevents scratch register allocation from trampling on variable registers
        if (enable_tracing || trace_codegen) {
            std::cout << "\n[INFO] Synchronizing RegisterManager with LinearScanAllocator decisions...\n";
        }
        // The RegisterManager will be synced per-function during code generation


        // --- ADD THIS DEBUG BLOCK ---
        if (enable_tracing || trace_codegen) {
            std::cout << "\n--- Final Metrics State Before Code Generation ---\n";
            ASTAnalyzer::getInstance().print_report();
            std::cout << "--------------------------------------------------\n\n";
        }


        // --- Prepare for Code Generation ---
        InstructionStream instruction_stream(LabelManager::instance(), enable_tracing || trace_codegen);
        DataGenerator data_generator(enable_tracing || trace_codegen, trace_vtables);
        data_generator.set_class_table(class_table.get());
        data_generator.set_string_table(&string_table);
        RegisterManager& register_manager = RegisterManager::getInstance();
        register_manager.set_debug_enabled(enable_tracing || trace_codegen);
        LabelManager& label_manager = LabelManager::instance();
        int debug_level = (enable_tracing || trace_codegen) ? 5 : 0;

        // --- Allocate JIT data pool before code generation ---
        const size_t JIT_DATA_POOL_SIZE = 1024 * 1024; // New 1MB size
        // supports 32k 64bit global variables.

        g_jit_data_manager = std::make_unique<JITMemoryManager>();
        g_jit_data_manager->allocate(JIT_DATA_POOL_SIZE);
        void* jit_data_memory_base = g_jit_data_manager->getMemoryPointer();
        if (!jit_data_memory_base) {
            std::cerr << "Failed to allocate JIT data pool." << std::endl;
            return 1;
        }

        // --- Initialize Code Buffer for JIT Mode (needed for veneer manager) ---
        void* code_buffer_base = nullptr;
        if (run_jit || trace_codegen) {
            if (!g_jit_code_buffer) {
                g_jit_code_buffer = std::make_unique<CodeBuffer>(32 * 1024 * 1024, enable_tracing || trace_codegen);
            }
            code_buffer_base = g_jit_code_buffer->getMemoryPointer();
        }

        // --- Code Generation ---
        NewCodeGenerator code_generator(
            instruction_stream,
            register_manager,
            label_manager,
            enable_tracing || trace_codegen,
            debug_level,
            data_generator,
            reinterpret_cast<uint64_t>(jit_data_memory_base),
            cfg_builder, // Pass the CFGBuilderPass object
            analyzer,    // Pass the analyzer instance
            std::move(symbol_table), // Pass the populated symbol table
            all_allocations, // Pass the new allocation map
            run_jit, // Pass is_jit_mode: true for JIT, false for static/exec
            class_table.get(),
            final_liveness_analyzer, // Pass the liveness analyzer
            bounds_checking_enabled, // Pass bounds checking flag
            use_neon // Pass NEON flag
        );

        // --- Initialize veneer manager
        // creates veneers for runtime calls before START
        if ((run_jit || trace_codegen) && code_buffer_base) {
            code_generator.initialize_veneer_manager(reinterpret_cast<uint64_t>(code_buffer_base));
        }

        // Generate code
        code_generator.generate_code(*ast);
        // Emit all interned strings after code generation
        data_generator.emit_interned_strings();
        if (enable_tracing || trace_codegen) std::cout << "Code generation complete.\n";

        // --- Print symbol table after code generation ---
        if (enable_tracing || trace_symbols || trace_codegen) {
            std::cout << "\n--- Symbol Table After Code Generation ---\n";
            code_generator.get_symbol_table()->dumpTable();
        }

        // Populate the JIT data segment with initial values for globals
          if (enable_tracing || trace_codegen) std::cout << "Data sections generated.\n";


        if (enable_peephole) {
            PeepholeOptimizer peephole_optimizer(enable_tracing || trace_codegen);
            peephole_optimizer.optimize(instruction_stream);
        }



        // --exec
        //
        // Generate CLANG compatible .s assembler file, compile and link it
        // Auto-enable assembly generation if listing file is requested
        if (generate_asm || exec_mode || generate_list) {
            std::string base_name = input_filepath.substr(0, input_filepath.find_last_of('.'));
            handle_static_compilation(exec_mode, base_name, instruction_stream, data_generator, enable_tracing || trace_codegen, runtime_mode, code_generator.get_veneer_manager(), generate_list);
        }

        // --- RESET THE LABEL MANAGER ---
        LabelManager::instance().reset();

        // --run
        // RUN generated code

        // For --trace-codegen, we need to run the linking process to get proper addresses
        void* final_code_buffer_base = nullptr;
        if (trace_codegen || run_jit) {
            // Code buffer was already allocated before code generation for veneer manager
            std::vector<Instruction> finalized_instructions;
            final_code_buffer_base = handle_jit_compilation(jit_data_memory_base, instruction_stream, g_jit_breakpoint_offset, enable_tracing || trace_codegen, &finalized_instructions);

            // --- Populate the runtime function pointer table before populating the data segment and executing code ---
            RuntimeManager::instance().populate_function_pointer_table(jit_data_memory_base);

            // Make the first 512KB (runtime table) read-only
            if (g_jit_data_manager) {
                g_jit_data_manager->makeReadOnly(512 * 1024, 512 * 1024);
                if (enable_debug_output) {
                    std::cout << "Set runtime function table memory to read-only.\n";
                }
            }

            data_generator.populate_data_segment(jit_data_memory_base, label_manager);

            if (run_jit) {
                handle_jit_execution(final_code_buffer_base, call_entry_name, dump_jit_stack, enable_tracing || trace_runtime);

                // --- NEW: Call the listing functions here for --run ---
                if (enable_tracing && !trace_codegen) {
                    std::cout << data_generator.generate_rodata_listing(label_manager);
                    std::cout << data_generator.generate_data_listing(label_manager, jit_data_memory_base);
                }
            }

            // Show assembly listing for --trace-codegen after all linking is complete
            if (trace_codegen) {
                CodeLister code_lister;
                std::string listing = code_lister.generate_code_listing(
                    finalized_instructions, 
                    label_manager.get_defined_labels(), 
                    reinterpret_cast<size_t>(final_code_buffer_base)
                );
                std::cout << "\n--- Generated Assembly Code (After Linking) ---\n" << listing << "-----------------------------------------------\n\n";
                
                std::cout << data_generator.generate_rodata_listing(label_manager);
                std::cout << data_generator.generate_data_listing(label_manager, jit_data_memory_base);
            }
        }
    } catch (const std::exception& ex) {
        std::cerr << "NewBCPL Compiler Error: " << ex.what() << std::endl;
        return 1;
    }

    if (enable_tracing || trace_runtime || trace_heap) {
        print_runtime_metrics();
    }

    return 0;
}

// =================================================================================
// Helper Function Implementations
// =================================================================================

/**
 * @brief Parses command-line arguments.
 * @return True if parsing is successful, false otherwise.
 */
bool parse_arguments(int argc, char* argv[], bool& run_jit, bool& generate_asm, bool& exec_mode,
                    bool& enable_opt, bool& enable_tracing,
                    bool& trace_lexer, bool& trace_parser, bool& trace_ast, bool& trace_cfg,
                    bool& trace_codegen, bool& trace_optimizer, bool& trace_liveness,
                    bool& trace_runtime, bool& trace_symbols, bool& trace_heap,
                    bool& trace_preprocessor, bool& enable_preprocessor,
                    bool& dump_jit_stack, bool& enable_peephole, bool& enable_stack_canaries,
                    bool& format_code, bool& trace_class_table, bool& trace_vtables,
                    bool& bounds_checking_enabled, bool& enable_samm,
                    bool& enable_superdisc, bool& use_neon, bool& generate_list, bool& test_encoders,
                    bool& test_encode, std::string& test_encode_name, bool& list_encoders,
                    std::string& input_filepath, std::string& call_entry_name, int& offset_instructions,
                    std::vector<std::string>& include_paths, std::string& runtime_mode) {
    if (enable_tracing) {
        std::cout << "Debug: Entering parse_arguments with argc=" << argc << std::endl;
        std::cout << "Debug: Iterating through " << argc << " arguments\n";
    }

    // Initialize bounds checking to enabled by default
    bounds_checking_enabled = true;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (enable_tracing) {
            std::cout << "Debug: Processing argument " << i << ": " << arg << std::endl;
        }
        if (arg == "--run" || arg == "-r") run_jit = true;
        else if (arg == "--asm" || arg == "-a") generate_asm = true;
        else if (arg == "--exec" || arg == "-e") exec_mode = true;
        else if (arg == "--opt" || arg == "-o") enable_opt = true;
        else if (arg == "--no-peep") enable_peephole = false;
        else if (arg == "--trace" || arg == "-T") enable_tracing = true;
        // duplicate line removed
        else if (arg == "--trace-lexer") trace_lexer = true;
        else if (arg == "--trace-parser") trace_parser = true;
        else if (arg == "--trace-ast") trace_ast = true;
        else if (arg == "--trace-cfg") trace_cfg = true;
        else if (arg == "--trace-codegen") trace_codegen = true;
        else if (arg == "--trace-optimizer") trace_optimizer = true;
        else if (arg == "--trace-liveness") trace_liveness = true;
        else if (arg == "--trace-runtime") trace_runtime = true;
        else if (arg == "--trace-symbols") trace_symbols = true;
        else if (arg == "--trace-heap") trace_heap = true;
        else if (arg == "--trace-class-table") trace_class_table = true;
        else if (arg == "--trace-vtable") trace_vtables = true;
        else if (arg == "--trace-preprocessor") trace_preprocessor = true;
        else if (arg == "--dump-jit-stack") dump_jit_stack = true;
        else if (arg == "--stack-canaries") enable_stack_canaries = true;
        else if (arg == "--format") format_code = true;
        else if (arg == "--no-bounds-check") bounds_checking_enabled = false;
        else if (arg == "--noSAMM") enable_samm = false;
        else if (arg == "--no-opt") enable_opt = false;
        else if (arg == "--no-superdisc") enable_superdisc = false;
        else if (arg == "--no-neon") use_neon = false;
        else if (arg == "--list" || arg == "-l") generate_list = true;
        else if (arg == "--test-encoders") test_encoders = true;
        else if (arg == "--test-encode") {
            if (i + 1 < argc) {
                test_encode = true;
                test_encode_name = argv[++i];
            } else {
                std::cerr << "Error: --test-encode requires an encoder name argument" << std::endl;
                return false;
            }
        }
        else if (arg == "--list-encoders") list_encoders = true;
        else if (arg.substr(0, 10) == "--runtime=") {
            runtime_mode = arg.substr(10);
            if (runtime_mode != "jit" && runtime_mode != "standalone" && runtime_mode != "unified") {
                std::cerr << "Error: Invalid runtime mode '" << runtime_mode << "'. Use jit, standalone, or unified." << std::endl;
                return false;
            }
        }
        else if (arg == "-I" || arg == "--include-path") {
            if (i + 1 < argc) include_paths.push_back(argv[++i]);
            else {
                std::cerr << "Error: -I/--include-path requires a directory path argument" << std::endl;
                return false;
            }
        }
        else if (arg == "--call" || arg == "-c") {
            if (i + 1 < argc) call_entry_name = argv[++i];
            else { std::cerr << "Error: --call option requires a name." << std::endl; return false; }
        } else if (arg == "--break" || arg == "-b") {
            if (i + 1 < argc) {
                g_jit_breakpoint_label = argv[++i];
                if (i + 1 < argc) {
                    std::string next_arg = argv[i+1];
                    if (next_arg.length() > 0 && (next_arg[0] == '+' || next_arg[0] == '-')) {
                        try { offset_instructions = std::stoi(next_arg); ++i; }
                        catch (const std::exception&) { std::cerr << "Error: Invalid offset for --break: " << next_arg << std::endl; return false; }
                    }
                }
            } else { std::cerr << "Error: --break option requires a label name." << std::endl; return false; }
        } else if (arg == "--help" || arg == "-h") {
            if (enable_tracing) {
                std::cout << "Debug: Displaying help\n";
            }
            std::cout << "Usage: " << argv[0] << " [options] <input_file.bcl>\n"
                      << "NewBCPL Compiler ";
            print_version();
            std::cout << "Options:\n"
                      << "  --run, -r              : JIT compile and execute the code.\n"
                      << "  --asm, -a              : Generate ARM64 assembly file.\n"
                      << "  --exec, -e             : Assemble, build with clang, and execute (uses unified runtime by default).\n"
                      << "  --runtime=MODE         : Select runtime mode (jit, standalone, unified). Default: jit for --run, unified for --exec.\n"
                      << "  --opt, -o              : Enable AST-to-AST optimization passes (default: ON).\n"
                      << "  --no-opt               : Disable all AST-to-AST optimization passes.\n"
                      << "  --popt                 : Enable peephole optimizer (enabled by default).\n"
                      << "  --no-peep              : Disable peephole optimizer.\n"
                      << "  --no-preprocessor      : Disable GET directive processing.\n"
                      << "  --stack-canaries       : Enable stack canaries for buffer overflow detection.\n"
                      << "  --no-bounds-check      : Disable runtime bounds checking for vector/string access (default: enabled).\n"
                      << "  --noSAMM               : Disable SAMM (Scope Aware Memory Management) - reduces automatic cleanup (default: enabled).\n"
                      << "  --no-superdisc         : Disable CREATE Method Reordering Pass (rewrite CREATE)\n"
                      << "  --no-neon              : Disable NEON SIMD instructions for vector operations (use scalar fallback).\n"
                      << "  --list, -l             : Generate listing file (.lst) with hex opcodes alongside assembly.\n"
                      << "\n"
                      << "Encoder Testing:\n"
                      << "  --test-encoders        : Run all encoder validation tests (53 total).\n"
                      << "  --test-encode <name>   : Run validation for specific encoder or pattern.\n"
                      << "  --list-encoders        : Show all available encoder names.\n"
                      << "  -I path, --include-path path : Add directory to include search path for GET directives.\n"
                      << "                          Multiple -I flags can be specified for additional paths.\n"
                      << "                          Search order: 1) Current file's directory 2) Specified include paths\n"
                      << "  --dump-jit-stack       : Dumps the JIT stack memory after execution.\n"
                      << "  --call name, -c name   : JIT-call the routine with the given label.\n"
                      << "  --break label[+/-off]  : Insert a BRK #0 instruction at the specified label, with optional offset.\n"
                      << "  --format               : Format BCPL source code and output to stdout.\n"
                      << "  --help, -h             : Display this help message.\n"
                      << "\n"
                      << "Tracing Options (for debugging and development):\n"
                      << "  --trace, -T            : Enable all detailed tracing (verbose).\n"
                      << "  --trace-lexer          : Enable lexer tracing.\n"
                      << "  --trace-parser         : Enable parser tracing.\n"
                      << "  --trace-ast            : Enable AST building and transformation tracing.\n"
                      << "  --trace-cfg            : Enable control flow graph construction tracing.\n"
                      << "  --trace-codegen        : Enable code generation tracing.\n"
                      << "  --trace-opt            : Enable optimizer tracing.\n"
                      << "  --trace-liveness       : Enable liveness analysis tracing.\n"
                      << "  --trace-runtime        : Enable runtime function tracing.\n"
                      << "  --trace-symbols        : Enable symbol table construction tracing.\n"
                      << "  --trace-heap           : Enable heap manager tracing.\n"
                      << "  --trace-preprocessor   : Enable preprocessor tracing.\n"
                      << "  --trace-class-table    : Print the class table after symbol discovery.\n"
                      << "  --trace-vtable         : Enable detailed vtable structure tracing.\n";
            return false;
        } else if (input_filepath.empty() && arg[0] != '-') {
            input_filepath = arg;
        } else {
            std::cerr << "Error: Multiple input files specified or unknown argument: " << arg << std::endl;
            return false;
        }
    }

    if (input_filepath.empty() && !test_encoders && !test_encode && !list_encoders) {
        if (enable_tracing) {
            std::cerr << "Debug: No input file specified.\n";
        }
        std::cerr << "Error: No input file specified. Use --help for usage.\n";
        return false;
    }
    if (enable_tracing) {
        std::cout << "Debug: parse_arguments successful, input_filepath=" << input_filepath << std::endl;
    }
    return true;
}

/**
 * @brief Handles static compilation to an assembly file and optionally builds and runs it.
 */
void handle_static_compilation(bool exec_mode, const std::string& base_name, const InstructionStream& instruction_stream, const DataGenerator& data_generator, bool enable_debug_output, const std::string& runtime_mode, const VeneerManager& veneer_manager, bool generate_list) {
    if (enable_debug_output) std::cout << "Performing static linking for assembly file generation...\n";
    Linker static_linker;
    std::vector<Instruction> static_instructions = static_linker.process(
        instruction_stream, LabelManager::instance(), RuntimeManager::instance(), 0, nullptr, nullptr, enable_debug_output);

    std::string asm_output_path = base_name + ".s";
    AssemblyWriter asm_writer;
    asm_writer.write_to_file(asm_output_path, static_instructions, LabelManager::instance(), data_generator, veneer_manager);

    if (generate_list) {
        // Generate object file first, then create listing with objdump
        std::string obj_path = base_name + ".o";
        std::string lst_path = base_name + ".lst";
        std::string compile_command = "clang -c " + asm_output_path + " -o " + obj_path;
        
        if (enable_debug_output) {
            std::cout << "Compiling to object file: " << compile_command << std::endl;
        }
        
        int compile_result = system(compile_command.c_str());
        if (compile_result == 0) {
            // Use objdump to create listing with hex opcodes
            std::string objdump_command = "objdump -d -S " + obj_path + " > " + lst_path;
            if (enable_debug_output) {
                std::cout << "Generating listing file: " << objdump_command << std::endl;
            }
            
            int objdump_result = system(objdump_command.c_str());
            if (objdump_result == 0) {
                std::cout << "Generated listing file: " << lst_path << std::endl;
                std::cout << "Generated object file: " << obj_path << std::endl;
            } else {
                std::cerr << "Warning: Failed to generate listing with objdump" << std::endl;
            }
        } else {
            std::cerr << "Warning: Failed to compile object file for listing" << std::endl;
        }
    }

    if (exec_mode) {
        if (enable_debug_output) std::cout << "\n--- Exec Mode (via clang) ---\n";
        std::string executable_output_path = "testrun";

        // Select runtime library and additional flags based on runtime mode
        std::string runtime_lib;
        std::string extra_flags;

        // Default to unified runtime for --exec mode if no specific runtime was specified
        std::string effective_runtime_mode = runtime_mode;
        if (runtime_mode == "jit" && exec_mode) {
            effective_runtime_mode = "unified";
            if (enable_debug_output) std::cout << "Note: Using unified runtime (default for --exec mode)\n";
        }

        if (effective_runtime_mode == "unified") {
            // Check for SDL2-enabled runtime libraries first
            if (enable_debug_output) std::cout << "Checking for SDL2 runtime libraries...\n";
            if (access("./libbcpl_runtime_sdl2_static.a", F_OK) == 0) {
                runtime_lib = "./libbcpl_runtime_sdl2_static.a";
                if (enable_debug_output) std::cout << "Using SDL2 static runtime library\n";
            } else if (access("./libbcpl_runtime_sdl2.a", F_OK) == 0) {
                runtime_lib = "./libbcpl_runtime_sdl2.a";
                if (enable_debug_output) std::cout << "Using SDL2 dynamic runtime library\n";
            } else {
                runtime_lib = "./libbcpl_runtime.a";
                if (enable_debug_output) std::cout << "Using basic unified runtime library\n";
            }
            if (enable_debug_output) std::cout << "Selected runtime library: " << runtime_lib << "\n";
            extra_flags = " -lstdc++";
        } else if (effective_runtime_mode == "standalone") {
            runtime_lib = "./libbcpl_runtime_c.a";
            extra_flags = "";
        } else { // jit mode (fallback to unified for static compilation)
            // Check for SDL2-enabled runtime libraries first for JIT fallback too
            if (enable_debug_output) std::cout << "Checking for SDL2 runtime libraries (JIT fallback)...\n";
            if (access("./libbcpl_runtime_sdl2_static.a", F_OK) == 0) {
                runtime_lib = "./libbcpl_runtime_sdl2_static.a";
                if (enable_debug_output) std::cout << "Note: JIT mode fallback to SDL2 static runtime for static compilation\n";
            } else if (access("./libbcpl_runtime_sdl2_dynamic.a", F_OK) == 0) {
                runtime_lib = "./libbcpl_runtime_sdl2_dynamic.a";
                if (enable_debug_output) std::cout << "Note: JIT mode fallback to SDL2 dynamic runtime for static compilation\n";
            } else {
                runtime_lib = "./libbcpl_runtime.a";
                if (enable_debug_output) std::cout << "Note: JIT mode fallback to unified runtime for static compilation\n";
            }
            if (enable_debug_output) std::cout << "Selected runtime library: " << runtime_lib << "\n";
            extra_flags = " -lstdc++";
        }

        // Add SDL2 linking flags if using SDL2 runtime
        std::string sdl2_flags = "";
        if (runtime_lib.find("sdl2") != std::string::npos) {
            if (runtime_lib.find("sdl2_static") != std::string::npos) {
                // Static SDL2 linking - add system frameworks
                sdl2_flags = " -lm -framework CoreAudio -framework AudioToolbox -framework CoreHaptics -framework GameController -framework ForceFeedback -lobjc -framework CoreVideo -framework Cocoa -framework Carbon -framework IOKit -framework QuartzCore -framework Metal -framework CoreFoundation -framework Foundation";
                if (enable_debug_output) std::cout << "Adding SDL2 static linking frameworks\n";
            } else {
                // Dynamic SDL2 linking
                sdl2_flags = " -L/opt/homebrew/lib -lSDL2";
                if (enable_debug_output) std::cout << "Adding SDL2 dynamic linking flags\n";
            }
        }

        std::string clang_command = "clang -g -o " + executable_output_path + " starter.o " + asm_output_path + " " + runtime_lib + extra_flags + sdl2_flags;
        if (enable_debug_output) std::cout << "Executing: " << clang_command << std::endl;

        int build_result = system(clang_command.c_str());
        if (build_result == 0) {
            if (enable_debug_output) std::cout << "Build successful." << std::endl;
            std::string run_command = "./" + executable_output_path;
            if (enable_debug_output) std::cout << "\n--- Running '" << run_command << "' ---\n";
            int run_result = system(run_command.c_str());
            if (enable_debug_output) std::cout << "--- Program finished with exit code: " << WEXITSTATUS(run_result) << " ---\n";
        } else {
            std::cerr << "Error: Build failed with code " << build_result << std::endl;
        }
    }
}

/**
 * @brief Handles the JIT compilation process: linking, memory population, and final code commit.
 * @return A pointer to the executable code buffer.
 */
void* handle_jit_compilation(void* jit_data_memory_base, InstructionStream& instruction_stream, int offset_instructions, bool enable_debug_output, std::vector<Instruction>* finalized_instructions) {
    // Code buffer should already be allocated by main() for veneer manager
    if (!g_jit_code_buffer) {
        throw std::runtime_error("Code buffer not initialized before JIT compilation");
    }
    void* code_buffer_base = g_jit_code_buffer->getMemoryPointer();
    Linker jit_linker;

    // Linker runs and assigns final virtual addresses to every instruction
    std::vector<Instruction> finalized_jit_instructions = jit_linker.process(
        instruction_stream, LabelManager::instance(), RuntimeManager::instance(),
        reinterpret_cast<size_t>(code_buffer_base),
        code_buffer_base, // rodata_base is unused by linker, but pass for consistency
        jit_data_memory_base, enable_debug_output);

    if (enable_debug_output) std::cout << "Populating JIT memory according to linker layout...\n";

    // This vector is now only for the CodeLister, not for memory population.
    std::vector<Instruction> code_and_rodata_for_listing;

    // Manually populate the JIT code and data buffers based on the linker's assigned addresses.
    for (size_t i = 0; i < finalized_jit_instructions.size(); ++i) {
        const auto& instr = finalized_jit_instructions[i];

        // Skip pseudo-instructions that are just for label definition
        if (instr.is_label_definition) {
            continue;
        }

        switch (instr.segment) {
            case SegmentType::CODE:
            case SegmentType::RODATA: {
                // --- HANDLES THE VTABLE IN .rodata ---
                size_t offset = instr.address - reinterpret_cast<size_t>(code_buffer_base);
                char* dest = static_cast<char*>(code_buffer_base) + offset;

                // Use the relocation type to identify the start of a 64-bit value.
                if (instr.relocation == RelocationType::ABSOLUTE_ADDRESS_LO32) {
                    if ((i + 1) < finalized_jit_instructions.size() &&
                        finalized_jit_instructions[i + 1].relocation == RelocationType::ABSOLUTE_ADDRESS_HI32)
                    {
                        const auto& upper_instr = finalized_jit_instructions[i + 1];
                        uint64_t value = (static_cast<uint64_t>(upper_instr.encoding) << 32) | instr.encoding;
                        if (enable_debug_output) {
                            std::cerr << "[RODATA] Writing 64-bit value 0x" << std::hex << value
                                    << std::dec << " at 0x" << std::hex << instr.address
                                    << std::dec << " for " << instr.target_label << std::endl;
                        }
                        memcpy(dest, &value, sizeof(uint64_t));
                        i++; // Manually advance past the upper_instr
                    } else {
                        // This case indicates a linker error (a lone LO32 without a HI32)
                        // For safety, write only the 4 bytes.
                        memcpy(dest, &instr.encoding, sizeof(uint32_t));
                    }
                } else if (instr.relocation != RelocationType::ABSOLUTE_ADDRESS_HI32) {
                    // For all other instructions (including regular code and HI32 parts that are skipped),
                    // write 4 bytes. The HI32 case is skipped because the LO32 case handles it.
                    
                    // Trace specific instructions before memory write
                    if (instr.trace_this_instruction) {
                        std::cerr << "[JIT MEMORY WRITE TRACE] About to write: " << instr.assembly_text 
                                  << " | Encoding: 0x" << std::hex << instr.encoding << std::dec 
                                  << " | To address: 0x" << std::hex << instr.address << std::dec << std::endl;
                    }
                    
                    memcpy(dest, &instr.encoding, sizeof(uint32_t));
                }

                if (instr.segment == SegmentType::CODE || instr.segment == SegmentType::RODATA) {
                    code_and_rodata_for_listing.push_back(instr);
                }
                break;
            }

            case SegmentType::DATA: {
                size_t offset = instr.address - reinterpret_cast<size_t>(jit_data_memory_base);
                char* dest = static_cast<char*>(jit_data_memory_base) + offset;

                // This logic was already correct and is preserved.
                if (instr.assembly_text.find(".quad") != std::string::npos && (i + 1) < finalized_jit_instructions.size()) {
                    const auto& upper_instr = finalized_jit_instructions[i + 1];
                    uint64_t value = (static_cast<uint64_t>(upper_instr.encoding) << 32) | instr.encoding;
                    memcpy(dest, &value, sizeof(uint64_t));
                    i++; // Skip the upper-half instruction.
                } else {
                    memcpy(dest, &instr.encoding, sizeof(uint32_t));
                }
                break;
            }
        }
    }

    // Set breakpoint if requested
    if (!g_jit_breakpoint_label.empty()) {
        try {
            size_t breakpoint_target_address = LabelManager::instance().get_label_address(g_jit_breakpoint_label) + (offset_instructions * 4);
            size_t offset = breakpoint_target_address - reinterpret_cast<size_t>(code_buffer_base);
            char* dest = static_cast<char*>(code_buffer_base) + offset;
            uint32_t brk_instruction = 0xD4200000; // BRK #0
            memcpy(dest, &brk_instruction, sizeof(uint32_t));
             if (enable_debug_output) std::cout << "DEBUG: Breakpoint set at 0x" << std::hex << breakpoint_target_address << std::dec << "\n";
        } catch (const std::runtime_error& e) {
            std::cerr << "Error setting breakpoint: " << e.what() << "\n";
        }
    }

    // Store finalized instructions for caller if requested
    if (finalized_instructions) {
        *finalized_instructions = finalized_jit_instructions;
    }

    // Now, commit the memory. Pass the instruction list for debug listing purposes.
    return g_jit_code_buffer->commit(code_and_rodata_for_listing);
}


/**
 * @brief Handles the execution of the JIT-compiled code.
 */
void handle_jit_execution(void* code_buffer_base, const std::string& call_entry_name, bool dump_jit_stack, bool enable_debug_output) {
    if (!code_buffer_base) {
        std::cerr << "Cannot execute JIT code: function pointer is null." << std::endl;
        return;
    }

    if (enable_debug_output) std::cout << "\n--- JIT Execution ---\n";

    size_t entry_offset = LabelManager::instance().get_label_address(call_entry_name) - reinterpret_cast<size_t>(code_buffer_base);
    void* entry_address = static_cast<char*>(code_buffer_base) + entry_offset;

    if (enable_debug_output) std::cout << "JIT execution enabled. Entry point '" << call_entry_name << "' at " << entry_address << std::endl;

    JITFunc jit_func = reinterpret_cast<JITFunc>(entry_address);
    g_jit_executor = std::make_unique<JITExecutor>(dump_jit_stack);

    if (RuntimeManager::instance().isTracingEnabled()) {
        std::cout << "[JITExecutor] Starting execution of JIT-compiled function at address: "
                  << entry_address << std::endl;
    }

    int64_t jit_result = g_jit_executor->execute(jit_func);

    if (RuntimeManager::instance().isTracingEnabled()) {
        std::cout << "[JITExecutor] Execution completed. Result: " << jit_result << std::endl;
    }

    // Check for runtime warnings/errors after execution
    extern volatile BCPLErrorInfo g_recent_errors[MAX_RECENT_ERRORS];
    extern volatile size_t g_recent_error_index;

    if (g_recent_error_index > 0) {
        std::cout << "\n--- Runtime Warnings/Errors ---\n";

        size_t start_index = (g_recent_error_index > MAX_RECENT_ERRORS) ?
                            (g_recent_error_index - MAX_RECENT_ERRORS) : 0;

        for (size_t i = start_index; i < g_recent_error_index; i++) {
            size_t idx = i % MAX_RECENT_ERRORS;
            const volatile auto& error = g_recent_errors[idx];

            if (error.code != RUNTIME_OK) {
                const char* error_type = (error.code == ERROR_INVALID_POINTER) ? "WARNING" : "ERROR";
                std::cout << "[" << error_type << "] " << (const char*)error.function_name
                         << ": " << (const char*)error.message << "\n";
            }
        }

        std::cout << "--------------------------------\n";
    }

    if (enable_debug_output) std::cout << "\n--- JIT returned with result: " << jit_result << " ---\n";
}

/**
 * @brief Reads the entire content of a file into a string.
 */
std::string read_file_content(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filepath);
    }
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}
