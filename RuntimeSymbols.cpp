#include "RuntimeSymbols.h"
#include <iostream>
#include <optional>

// Conditionally include SDL2 registration if available
#ifdef SDL2_RUNTIME_ENABLED
#include "runtime/SDL2/sdl2_registration.h"
#endif



void RuntimeSymbols::registerAll(SymbolTable& symbol_table) {
    // Standard runtime integer-returning functions
    registerRuntimeFunction(symbol_table, "READN");
    registerRuntimeFloatFunction(symbol_table, "RND", {
        {VarType::INTEGER, false} // max_val parameter
    });
    registerRuntimeFunction(symbol_table, "RAND", {
        {VarType::INTEGER, false} // max_val parameter
    });
    registerRuntimeFunction(symbol_table, "LENGTH", {
        {VarType::INTEGER, false} // string pointer
    });
    registerRuntimeFunction(symbol_table, "GETBYTE", {
        {VarType::INTEGER, false}, // pointer
        {VarType::INTEGER, false}  // offset
    });
    registerRuntimeFunction(symbol_table, "GETWORD", {
        {VarType::INTEGER, false}, // pointer
        {VarType::INTEGER, false}  // offset
    });
    
    // Floating-point runtime functions
    registerRuntimeFloatFunction(symbol_table, "READF");
    registerRuntimeFloatFunction(symbol_table, "FLTOFX", {
        {VarType::INTEGER, false}  // integer to convert
    });
    registerRuntimeFloatFunction(symbol_table, "FSIN", {
        {VarType::FLOAT, false}    // angle in radians
    });
    registerRuntimeFloatFunction(symbol_table, "FCOS", {
        {VarType::FLOAT, false}    // angle in radians
    });

    registerRuntimeFloatFunction(symbol_table, "FTAN", {
        {VarType::FLOAT, false}    // angle in radians
    });
    registerRuntimeFloatFunction(symbol_table, "FABS", {
        {VarType::FLOAT, false}    // value
    });
    registerRuntimeFloatFunction(symbol_table, "FLOG", {
        {VarType::FLOAT, false}    // value
    });
    registerRuntimeFloatFunction(symbol_table, "FEXP", {
        {VarType::FLOAT, false}    // value
    });
    registerRuntimeFloatFunction(symbol_table, "FRND");
    
    // Type conversion functions
    // Note: FIX is now a compiler intrinsic (UnaryOp::IntegerConvert), not a runtime function
    
    // Void-returning runtime routines
    registerRuntimeRoutine(symbol_table, "WRITES", {
        {VarType::INTEGER, false}  // string pointer
    });
    registerRuntimeRoutine(symbol_table, "WRITEN", {
        {VarType::INTEGER, false}  // integer value
    });
    registerRuntimeFloatRoutine(symbol_table, "FWRITE", {
        {VarType::FLOAT, false}    // float value
    });
    
    // WRITEF is a special variadic-style function handled by the code generator.
    // We register the base case here. The compiler's RoutineCallStatement handler
    // treats it as a special case that can have many arguments and automatically
    // generates calls to WRITEF1, WRITEF2, etc. based on argument count.
    registerRuntimeRoutine(symbol_table, "WRITEF", {
        {VarType::STRING, false}   // format string (minimum requirement)
    });
    registerRuntimeRoutine(symbol_table, "PUTBYTE", {
        {VarType::INTEGER, false}, // pointer
        {VarType::INTEGER, false}, // offset
        {VarType::INTEGER, false}  // value
    });
    registerRuntimeRoutine(symbol_table, "PUTWORD", {
        {VarType::INTEGER, false}, // pointer
        {VarType::INTEGER, false}, // offset
        {VarType::INTEGER, false}  // value
    });
    registerRuntimeRoutine(symbol_table, "EXIT", {
        {VarType::INTEGER, false}  // exit code
    });
    registerRuntimeRoutine(symbol_table, "NEWLINE");
    registerRuntimeRoutine(symbol_table, "NEWPAGE");
    
    // FILE_ API functions
    registerRuntimeFunction(symbol_table, "FILE_OPEN_READ", {
        {VarType::STRING, false}   // filename
    });
    registerRuntimeFunction(symbol_table, "FILE_OPEN_WRITE", {
        {VarType::STRING, false}   // filename
    });
    registerRuntimeFunction(symbol_table, "FILE_OPEN_APPEND", {
        {VarType::STRING, false}   // filename
    });
    registerRuntimeFunction(symbol_table, "FILE_CLOSE", {
        {VarType::INTEGER, false}  // file handle
    });
    registerRuntimeFunction(symbol_table, "FILE_WRITES", {
        {VarType::INTEGER, false}, // file handle
        {VarType::STRING, false}   // string to write
    });
    registerRuntimeFunction(symbol_table, "FILE_READS", {
        {VarType::INTEGER, false}  // file handle
    });
    registerRuntimeFunction(symbol_table, "FILE_READ", {
        {VarType::INTEGER, false}, // file handle
        {VarType::INTEGER, false}, // buffer
        {VarType::INTEGER, false}  // size
    });
    registerRuntimeFunction(symbol_table, "FILE_WRITE", {
        {VarType::INTEGER, false}, // file handle
        {VarType::INTEGER, false}, // buffer
        {VarType::INTEGER, false}  // size
    });
    registerRuntimeFunction(symbol_table, "FILE_SEEK", {
        {VarType::INTEGER, false}, // file handle
        {VarType::INTEGER, false}, // offset
        {VarType::INTEGER, false}  // origin
    });
    registerRuntimeFunction(symbol_table, "FILE_TELL", {
        {VarType::INTEGER, false}  // file handle
    });
    registerRuntimeFunction(symbol_table, "FILE_EOF", {
        {VarType::INTEGER, false}  // file handle
    });
    
    // Conditionally register SDL2 functions if available
#ifdef SDL2_RUNTIME_ENABLED
    try {
        runtime::register_sdl2_runtime_functions();
        
        // Register SDL2 functions in the symbol table for compilation-time recognition
        registerSDL2Symbols(symbol_table);
        
        // std::cout << "SDL2 runtime functions registered successfully." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Warning: Failed to register SDL2 runtime functions: " << e.what() << std::endl;
    }
#endif
}

void RuntimeSymbols::registerRuntimeFunction(
    SymbolTable& symbol_table, 
    const std::string& name,
    const std::vector<Symbol::ParameterInfo>& params
) {
    // Create a new symbol for this runtime function
    Symbol symbol(name, SymbolKind::RUNTIME_FUNCTION, VarType::INTEGER, 0, "");
    
    // Add parameter information
    symbol.parameters = params;
    
    // Register in the global scope
    if (!symbol_table.addSymbol(symbol)) {
        std::cerr << "Warning: Could not register runtime function " << name 
                  << " (duplicate symbol)" << std::endl;
    }
}

void RuntimeSymbols::registerRuntimeFloatFunction(
    SymbolTable& symbol_table, 
    const std::string& name,
    const std::vector<Symbol::ParameterInfo>& params
) {
    // Create a new symbol for this float runtime function
    Symbol symbol(name, SymbolKind::RUNTIME_FLOAT_FUNCTION, VarType::FLOAT, 0, "");
    
    // Add parameter information
    symbol.parameters = params;
    
    // Register in the global scope
    if (!symbol_table.addSymbol(symbol)) {
        std::cerr << "Warning: Could not register runtime float function " << name 
                  << " (duplicate symbol)" << std::endl;
    }
}

void RuntimeSymbols::registerRuntimeRoutine(
    SymbolTable& symbol_table, 
    const std::string& name,
    const std::vector<Symbol::ParameterInfo>& params
) {
    // Create a new symbol for this runtime routine
    Symbol symbol(name, SymbolKind::RUNTIME_ROUTINE, VarType::INTEGER, 0, "");
    
    // Add parameter information
    symbol.parameters = params;
    
    // Register in the global scope
    if (!symbol_table.addSymbol(symbol)) {
        std::cerr << "Warning: Could not register runtime routine " << name 
                  << " (duplicate symbol)" << std::endl;
    }
}

void RuntimeSymbols::registerRuntimeFloatRoutine(
    SymbolTable& symbol_table, 
    const std::string& name,
    const std::vector<Symbol::ParameterInfo>& params
) {
    // Create a new symbol for this float-handling runtime routine
    Symbol symbol(name, SymbolKind::RUNTIME_FLOAT_ROUTINE, VarType::FLOAT, 0, "");
    
    // Add parameter information
    symbol.parameters = params;
    
    // Register in the global scope
    if (!symbol_table.addSymbol(symbol)) {
        std::cerr << "Warning: Could not register runtime float routine " << name 
                  << " (duplicate symbol)" << std::endl;
    }
}

#ifdef SDL2_RUNTIME_ENABLED
void RuntimeSymbols::registerSDL2Symbols(SymbolTable& symbol_table) {
    // =============================================================================
    // INITIALIZATION AND CLEANUP
    // =============================================================================
    
    registerRuntimeFunction(symbol_table, "SDL2_INIT");
    registerRuntimeFunction(symbol_table, "SDL2_INIT_SUBSYSTEMS", {
        {VarType::INTEGER, false} // subsystems flags
    });
    registerRuntimeRoutine(symbol_table, "SDL2_QUIT");
    
    // =============================================================================
    // WINDOW MANAGEMENT
    // =============================================================================
    
    registerRuntimeFunction(symbol_table, "SDL2_CREATE_WINDOW", {
        {VarType::STRING, false} // title
    });
    registerRuntimeFunction(symbol_table, "SDL2_CREATE_WINDOW_EX", {
        {VarType::STRING, false}, // title
        {VarType::INTEGER, false}, // x
        {VarType::INTEGER, false}, // y
        {VarType::INTEGER, false}, // width
        {VarType::INTEGER, false}, // height
        {VarType::INTEGER, false}  // flags
    });
    registerRuntimeRoutine(symbol_table, "SDL2_DESTROY_WINDOW", {
        {VarType::INTEGER, false} // window pointer
    });
    registerRuntimeRoutine(symbol_table, "SDL2_SET_WINDOW_TITLE", {
        {VarType::INTEGER, false}, // window pointer
        {VarType::STRING, false}   // title
    });
    registerRuntimeRoutine(symbol_table, "SDL2_SET_WINDOW_SIZE", {
        {VarType::INTEGER, false}, // window pointer
        {VarType::INTEGER, false}, // width
        {VarType::INTEGER, false}  // height
    });
    
    // =============================================================================
    // RENDERING
    // =============================================================================
    
    registerRuntimeFunction(symbol_table, "SDL2_CREATE_RENDERER", {
        {VarType::INTEGER, false} // window pointer
    });
    registerRuntimeFunction(symbol_table, "SDL2_CREATE_RENDERER_EX", {
        {VarType::INTEGER, false}, // window pointer
        {VarType::INTEGER, false}  // flags
    });
    registerRuntimeRoutine(symbol_table, "SDL2_DESTROY_RENDERER", {
        {VarType::INTEGER, false} // renderer pointer
    });
    registerRuntimeRoutine(symbol_table, "SDL2_SET_DRAW_COLOR", {
        {VarType::INTEGER, false}, // renderer pointer
        {VarType::INTEGER, false}, // red
        {VarType::INTEGER, false}, // green
        {VarType::INTEGER, false}, // blue
        {VarType::INTEGER, false}  // alpha
    });
    registerRuntimeRoutine(symbol_table, "SDL2_CLEAR", {
        {VarType::INTEGER, false} // renderer pointer
    });
    registerRuntimeRoutine(symbol_table, "SDL2_PRESENT", {
        {VarType::INTEGER, false} // renderer pointer
    });
    registerRuntimeRoutine(symbol_table, "SDL2_DRAW_POINT", {
        {VarType::INTEGER, false}, // renderer pointer
        {VarType::INTEGER, false}, // x
        {VarType::INTEGER, false}  // y
    });
    registerRuntimeRoutine(symbol_table, "SDL2_DRAW_LINE", {
        {VarType::INTEGER, false}, // renderer pointer
        {VarType::INTEGER, false}, // x1
        {VarType::INTEGER, false}, // y1
        {VarType::INTEGER, false}, // x2
        {VarType::INTEGER, false}  // y2
    });
    registerRuntimeRoutine(symbol_table, "SDL2_DRAW_RECT", {
        {VarType::INTEGER, false}, // renderer pointer
        {VarType::INTEGER, false}, // x
        {VarType::INTEGER, false}, // y
        {VarType::INTEGER, false}, // width
        {VarType::INTEGER, false}  // height
    });
    registerRuntimeRoutine(symbol_table, "SDL2_FILL_RECT", {
        {VarType::INTEGER, false}, // renderer pointer
        {VarType::INTEGER, false}, // x
        {VarType::INTEGER, false}, // y
        {VarType::INTEGER, false}, // width
        {VarType::INTEGER, false}  // height
    });
    
    // =============================================================================
    // EVENT HANDLING
    // =============================================================================
    
    registerRuntimeFunction(symbol_table, "SDL2_POLL_EVENT");
    registerRuntimeFunction(symbol_table, "SDL2_GET_EVENT_KEY");
    registerRuntimeFunction(symbol_table, "SDL2_GET_EVENT_MOUSE", {
        {VarType::INTEGER, false}, // x pointer
        {VarType::INTEGER, false}  // y pointer
    });
    registerRuntimeFunction(symbol_table, "SDL2_GET_EVENT_BUTTON");
    
    // =============================================================================
    // TIMING
    // =============================================================================
    
    registerRuntimeRoutine(symbol_table, "SDL2_DELAY", {
        {VarType::INTEGER, false} // milliseconds
    });
    registerRuntimeFunction(symbol_table, "SDL2_GET_TICKS");
    
    // =============================================================================
    // UTILITY FUNCTIONS
    // =============================================================================
    
    registerRuntimeFunction(symbol_table, "SDL2_GET_ERROR");
    registerRuntimeRoutine(symbol_table, "SDL2_CLEAR_ERROR");
    
    // =============================================================================
    // DIAGNOSTIC FUNCTIONS
    // =============================================================================
    
    registerRuntimeFunction(symbol_table, "SDL2_GET_VERSION");
    registerRuntimeFunction(symbol_table, "SDL2_GET_VIDEO_DRIVERS");
    registerRuntimeFunction(symbol_table, "SDL2_GET_CURRENT_VIDEO_DRIVER");
    registerRuntimeFunction(symbol_table, "SDL2_GET_DISPLAY_MODES");
    registerRuntimeFunction(symbol_table, "SDL2_TEST_BASIC");
}
#endif
