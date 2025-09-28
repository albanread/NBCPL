## How to Add a New Runtime Function to the BCPL Compiler

This guide outlines the essential steps to add a new C/C++ function to the BCPL runtime and make it callable from your BCPL code. Following these steps ensures that the function is correctly registered with the compiler's JIT and static compilation pipelines.

Step 1: Implement the Function
First, implement your function in C or C++. The function must adhere to the C calling convention (extern "C") to be visible to the runtime linker.

Location: Place your new function's implementation in a relevant file within the runtime/ directory. For example, a new string utility would go well in runtime/runtime_string_utils.inc.

Signature: The function signature must use C-compatible types like int64_t, double, void*, and pointers.

Example: A new function ADD_NUMBERS

// In a file like runtime/runtime_core.inc

#ifdef __cplusplus
extern "C" {
#endif

int64_t ADD_NUMBERS(int64_t a, int64_t b) {
    return a + b;
}

#ifdef __cplusplus
}
#endif

Step 2: Declare the Function in a Header
Provide a forward declaration for your new function in a C-compatible header file. This allows other parts of the runtime to see it.

Location: A good place for this is runtime/heap_interface.h or a new, more specific header if needed.

Example:

// In runtime/heap_interface.h

#ifdef __cplusplus
extern "C" {
#endif

int64_t ADD_NUMBERS(int64_t a, int64_t b);

#ifdef __cplusplus
}
#endif

Step 3: Register the Function with the Runtime Manager
The RuntimeManager is the compiler's central registry for runtime functions. You must add your new function to it so the code generator knows its name, address, and the number of arguments it takes.

Location: Edit the register_runtime_functions function in runtime/RuntimeBridge.cpp.

Example:

// In runtime/RuntimeBridge.cpp, inside register_runtime_functions()

void register_runtime_functions() {
    auto& manager = RuntimeManager::instance();

    // ... other function registrations ...

    // Add the new function here
    register_runtime_function("ADD_NUMBERS", 2, reinterpret_cast<void*>(ADD_NUMBERS));

    // ... other function registrations ...
}

The first argument is the uppercase name that will be used in BCPL code.

The second argument is the number of parameters the function takes.

The third argument is the function pointer, cast to void*.

For functions that return a floating-point number, add FunctionType::FLOAT as a fourth argument.

Step 4: Add the Symbol to the Symbol Table (Optional but Recommended)
For better type checking and clarity, add the runtime function to the RuntimeSymbols. This helps the compiler understand the function's signature.

Location: Edit the registerAll function in RuntimeSymbols.cpp.

Example:

// In RuntimeSymbols.cpp

void RuntimeSymbols::registerAll(SymbolTable& symbol_table) {
    // ... other symbol registrations ...

    registerRuntimeFunction(symbol_table, "ADD_NUMBERS", {
        {VarType::INTEGER, false}, // Parameter 'a'
        {VarType::INTEGER, false}  // Parameter 'b'
    });
}

Step 5: Rebuild the Runtime
After making these code changes, you must rebuild the runtime libraries. Run the build script from the project's root directory:

./buildruntime --jit
./buildruntime --standalone

This will compile your new function into libbcpl_runtime_jit.a and libbcpl_runtime_c.a, making it available to the compiler. Your function is now ready to be called from BCPL!
