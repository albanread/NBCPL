#include "../HeapManager/HeapManager.h"
#include "../HeapManager/heap_c_wrappers.h"
#include <cstdint>
// --- C-style wrappers to bridge to the C++ HeapManager singleton ---
extern "C" void* HeapManager_OBJECT_HEAP_ALLOC(void* class_ptr);
extern "C" void HeapManager_OBJECT_HEAP_FREE(void* object_ptr);
extern "C" void* HeapManager_RUNTIME_METHOD_LOOKUP(void* class_ptr, uint64_t method_hash);
extern "C" void HeapManager_PIC_RUNTIME_HELPER();

extern "C" {
    void* HeapManager_OBJECT_HEAP_ALLOC(void* class_ptr) {
        return OBJECT_HEAP_ALLOC(class_ptr);
    }

    void HeapManager_OBJECT_HEAP_FREE(void* object_ptr) {
        OBJECT_HEAP_FREE(object_ptr);
    }

    void* HeapManager_RUNTIME_METHOD_LOOKUP(void* class_ptr, uint64_t method_hash) {
        // return HeapManager::getInstance().RUNTIME_METHOD_LOOKUP(class_ptr, method_hash);
        return nullptr; // Temporarily return nullptr
    }

    void HeapManager_PIC_RUNTIME_HELPER() {
        // HeapManager::getInstance().PIC_RUNTIME_HELPER();
    }
}

// RuntimeBridge.cpp
// Implementation of the runtime bridge between new C-compatible runtime and RuntimeManager

#include <cstdint>

#include "RuntimeBridge.h"
#include "runtime.h"
#include "../RuntimeManager.h"
#include "runtime.h"
#include <iostream>
#include <string>
#include <cmath>
#include "heap_interface.h"
#include "ListDataTypes.h"

#include <cstdint>

// Forward declarations for runtime functions
#include "runtime_freelist.h" // Include this for initialize_freelist declaration

extern "C" {
    void bcpl_free_list(void*);
    void bcpl_free_list_safe(void*);
    void* bcpl_getvec(int64_t num_words);
    void* bcpl_fgetvec(int64_t num_floats);
    void BCPL_CHECK_AND_DISPLAY_ERRORS(void);
    void BCPL_GET_LAST_ERROR(void*);
    void BCPL_CLEAR_ERRORS(void);
    void BCPL_BOUNDS_ERROR(uint32_t*, int64_t, int64_t);
    int64_t BCPL_LIST_GET_HEAD_AS_INT(void*);
    double BCPL_LIST_GET_HEAD_AS_FLOAT(void*);
    void* BCPL_LIST_GET_TAIL(void*);
    void* BCPL_LIST_GET_REST(void*);
    int64_t BCPL_GET_ATOM_TYPE(void*);
    void BCPL_LIST_APPEND_LIST(void*, void*);
    void* BCPL_LIST_GET_NTH(void*, int64_t);

    // Random number functions
    int64_t RAND(int64_t max_val);
    double FRND(void);
    double RND(int64_t max_val);
    
    // Float math functions
    double FSIN(double x);
    double FCOS(double x);
    double FTAN(double x);
    double FABS(double x);
    double FLOG(double x);
    double FEXP(double x);
    int64_t FIX(double x);

    // New list creation/append ABI
    ListHeader* BCPL_LIST_CREATE_EMPTY(void);
    void BCPL_LIST_APPEND_INT(void*, int64_t);
    void BCPL_LIST_APPEND_FLOAT(void*, double);
    void BCPL_LIST_APPEND_STRING(ListHeader* header, uint32_t* value);
    // Add more as needed for other types

    // List copy functions
    ListHeader* BCPL_SHALLOW_COPY_LIST(ListHeader* original_header);
    ListHeader* BCPL_DEEP_COPY_LIST(ListHeader* original_header);
    ListHeader* BCPL_DEEP_COPY_LITERAL_LIST(struct ListLiteralHeader*);
    ListHeader* BCPL_REVERSE_LIST(ListHeader* original_header);
    ListAtom* BCPL_FIND_IN_LIST(ListHeader* header, int64_t value_bits, int64_t type_tag);
    ListHeader* BCPL_LIST_FILTER(ListHeader* original_header, PredicateFunc predicate);
    
    // List concatenation and string operations
    ListHeader* BCPL_CONCAT_LISTS(struct ListHeader* list1_header, struct ListHeader* list2_header);
    ListHeader* BCPL_SPLIT_STRING(uint32_t* source_payload, uint32_t* delimiter_payload);
    uint32_t* BCPL_JOIN_LIST(struct ListHeader* list_header, uint32_t* delimiter_payload);

    // String functions
    uint32_t* STRCOPY(uint32_t* dst, const uint32_t* src);
    int64_t STRCMP(const uint32_t* s1, const uint32_t* s2);
    int64_t STRLEN(const uint32_t* s);
    void* PACKSTRING(uint32_t* bcpl_string);
    uint32_t* UNPACKSTRING(const uint8_t* byte_vector);
    
    // File I/O functions
    uint32_t* SLURP(uint32_t* filename_str);
    void SPIT(uint32_t* bcpl_string, uint32_t* filename_str);
    
    // New File API functions
    uintptr_t FILE_OPEN_READ(uint32_t* filename_str);
    uintptr_t FILE_OPEN_WRITE(uint32_t* filename_str);
    uintptr_t FILE_OPEN_APPEND(uint32_t* filename_str);
    uint32_t FILE_CLOSE(uintptr_t handle);
    uint32_t FILE_WRITES(uintptr_t handle, uint32_t* string_buffer);
    uint32_t* FILE_READS(uintptr_t handle);
    uint32_t FILE_READ(uintptr_t handle, uint32_t* buffer, uint32_t size);
    uint32_t FILE_WRITE(uintptr_t handle, uint32_t* buffer, uint32_t size);
    uint32_t FILE_SEEK(uintptr_t handle, int32_t offset, uint32_t origin);
    int32_t FILE_TELL(uintptr_t handle);
    uint32_t FILE_EOF(uintptr_t handle);
    
    // System functions
    void finish(void);

    // Free list helpers
    void BCPL_FREE_CELLS(void);
    void* get_g_free_list_head_address(void);
    void returnNodeToFreelist_runtime(void*);
    void NEWLINE(void);
    
    // Aliases for BCPL compatibility
    void* GETVEC(int64_t num_words) { return bcpl_getvec(num_words); }
    void* FGETVEC(int64_t num_floats) { return bcpl_fgetvec(num_floats); }
    void FREEVEC(void* ptr) { return bcpl_free(ptr); }
}

// Include the file API implementation
#include "runtime_file_api.inc"



namespace runtime {

void register_runtime_functions() {
    auto& manager = RuntimeManager::instance();
    
    // String optimizations are embedded and auto-initialize
    
    // Core I/O functions
    register_runtime_function("WRITES", 1, reinterpret_cast<void*>(WRITES));
    register_runtime_function("WRITEN", 1, reinterpret_cast<void*>(WRITEN));
    register_runtime_function("FWRITE", 1, reinterpret_cast<void*>(FWRITE), FunctionType::FLOAT);
    register_runtime_function("WRITEF", 1, reinterpret_cast<void*>(WRITEF));
    register_runtime_function("WRITEF1", 2, reinterpret_cast<void*>(WRITEF1));
    register_runtime_function("WRITEF2", 3, reinterpret_cast<void*>(WRITEF2));
    register_runtime_function("WRITEF3", 4, reinterpret_cast<void*>(WRITEF3));
    register_runtime_function("WRITEF4", 5, reinterpret_cast<void*>(WRITEF4));
    register_runtime_function("WRITEF5", 6, reinterpret_cast<void*>(WRITEF5));
    register_runtime_function("WRITEF6", 7, reinterpret_cast<void*>(WRITEF6));
    register_runtime_function("WRITEF7", 8, reinterpret_cast<void*>(WRITEF7));
    register_runtime_function("WRITEC", 1, reinterpret_cast<void*>(WRITEC));
    register_runtime_function("RDCH", 0, reinterpret_cast<void*>(RDCH));
    
    // Memory management functions
    register_runtime_function("BCPL_ALLOC_WORDS", 3, reinterpret_cast<void*>(bcpl_alloc_words));
    register_runtime_function("BCPL_ALLOC_CHARS", 1, reinterpret_cast<void*>(bcpl_alloc_chars));
    register_runtime_function("MALLOC", 1, reinterpret_cast<void*>(bcpl_alloc_words)); // Alias for compatibility
    register_runtime_function("GETVEC", 1, reinterpret_cast<void*>(bcpl_getvec)); // Traditional BCPL vector allocation
    register_runtime_function("FGETVEC", 1, reinterpret_cast<void*>(bcpl_fgetvec)); // Float vector allocation
    if (RuntimeManager::instance().isTracingEnabled()) {
        printf("DEBUG: Registering FREEVEC with bcpl_free at address %p\n", reinterpret_cast<void*>(bcpl_free));
    }
    register_runtime_function("FREEVEC", 1, reinterpret_cast<void*>(bcpl_free));
    register_runtime_function("BCPL_FREE_LIST", 1, reinterpret_cast<void*>(bcpl_free_list));
    register_runtime_function("BCPL_FREE_LIST_SAFE", 1, reinterpret_cast<void*>(bcpl_free_list_safe));
    register_runtime_function("BCPL_GET_LAST_ERROR", 1, reinterpret_cast<void*>(BCPL_GET_LAST_ERROR));
    register_runtime_function("BCPL_CLEAR_ERRORS", 0, reinterpret_cast<void*>(BCPL_CLEAR_ERRORS));
    register_runtime_function("BCPL_CHECK_AND_DISPLAY_ERRORS", 0, reinterpret_cast<void*>(BCPL_CHECK_AND_DISPLAY_ERRORS));
    register_runtime_function("BCPL_BOUNDS_ERROR", 3, reinterpret_cast<void*>(BCPL_BOUNDS_ERROR));
    register_runtime_function("BCPL_LIST_GET_HEAD_AS_INT", 1, reinterpret_cast<void*>(BCPL_LIST_GET_HEAD_AS_INT));
    register_runtime_function("BCPL_LIST_GET_HEAD_AS_FLOAT", 1, reinterpret_cast<void*>(BCPL_LIST_GET_HEAD_AS_FLOAT), FunctionType::FLOAT);
    register_runtime_function("BCPL_LIST_GET_TAIL", 1, reinterpret_cast<void*>(BCPL_LIST_GET_TAIL));
    register_runtime_function("BCPL_LIST_GET_REST", 1, reinterpret_cast<void*>(BCPL_LIST_GET_REST));
    register_runtime_function("BCPL_GET_ATOM_TYPE", 1, reinterpret_cast<void*>(BCPL_GET_ATOM_TYPE));
    register_runtime_function("BCPL_LIST_GET_NTH", 2, reinterpret_cast<void*>(BCPL_LIST_GET_NTH));

    // --- Register SETTYPE as a pseudo-runtime function ---
    // The address is null because the ASTAnalyzer will handle it and prevent any code from being generated.
    register_runtime_function("SETTYPE", 2, nullptr);

    // --- OO Object Heap/Runtime ---
    register_runtime_function("OBJECT_HEAP_ALLOC", 1, reinterpret_cast<void*>(HeapManager_OBJECT_HEAP_ALLOC));
    register_runtime_function("OBJECT_HEAP_FREE", 1, reinterpret_cast<void*>(HeapManager_OBJECT_HEAP_FREE));
    register_runtime_function("RUNTIME_METHOD_LOOKUP", 2, reinterpret_cast<void*>(HeapManager_RUNTIME_METHOD_LOOKUP));
    register_runtime_function("PIC_RUNTIME_HELPER", 0, reinterpret_cast<void*>(HeapManager_PIC_RUNTIME_HELPER));

    // --- SAMM: Scope Aware Memory Management ---
    register_runtime_function("HeapManager_enter_scope", 0, reinterpret_cast<void*>(HeapManager_enterScope));
    register_runtime_function("HeapManager_exit_scope", 0, reinterpret_cast<void*>(HeapManager_exitScope));
    register_runtime_function("HEAPMANAGER_SETSAMMENABLED", 1, reinterpret_cast<void*>(HeapManager_setSAMMEnabled));
    register_runtime_function("HEAPMANAGER_ISSAMMENABLED", 0, reinterpret_cast<void*>(HeapManager_isSAMMEnabled));
    register_runtime_function("HEAPMANAGER_WAITFORSAMM", 0, reinterpret_cast<void*>(HeapManager_waitForSAMM));

    // New free list helpers
    register_runtime_function("BCPL_FREE_CELLS", 0, reinterpret_cast<void*>(BCPL_FREE_CELLS));
    register_runtime_function("GET_FREE_LIST_HEAD_ADDR", 0, reinterpret_cast<void*>(get_g_free_list_head_address));

    // New list creation/append ABI
    register_runtime_function("BCPL_LIST_CREATE_EMPTY", 0, reinterpret_cast<void*>(BCPL_LIST_CREATE_EMPTY));
    register_runtime_function("BCPL_LIST_APPEND_INT", 2, reinterpret_cast<void*>(BCPL_LIST_APPEND_INT));
    register_runtime_function("BCPL_LIST_APPEND_FLOAT", 2, reinterpret_cast<void*>(BCPL_LIST_APPEND_FLOAT), FunctionType::FLOAT);
    register_runtime_function("BCPL_LIST_APPEND_STRING", 2, reinterpret_cast<void*>(BCPL_LIST_APPEND_STRING));
    // Add more as needed for other types

    // Aliases for BCPL list append for BCPL source-level calls
    register_runtime_function("APND", 2, reinterpret_cast<void*>(BCPL_LIST_APPEND_INT));
    register_runtime_function("FPND", 2, reinterpret_cast<void*>(BCPL_LIST_APPEND_FLOAT), FunctionType::FLOAT);
    register_runtime_function("SPND", 2, reinterpret_cast<void*>(BCPL_LIST_APPEND_STRING));
    // Add this new line for appending lists
    register_runtime_function("LPND", 2, reinterpret_cast<void*>(BCPL_LIST_APPEND_LIST));
    register_runtime_function("BCPL_CONCAT_LISTS", 2, reinterpret_cast<void*>(BCPL_CONCAT_LISTS));
    register_runtime_function("CONCAT", 2, reinterpret_cast<void*>(BCPL_CONCAT_LISTS));

    // List copy functions
    register_runtime_function("COPYLIST", 1, reinterpret_cast<void*>(BCPL_SHALLOW_COPY_LIST));
    register_runtime_function("DEEPCOPYLIST", 1, reinterpret_cast<void*>(BCPL_DEEP_COPY_LIST));
    // Register the new function for handling list literals
    register_runtime_function("DEEPCOPYLITERALLIST", 1, reinterpret_cast<void*>(BCPL_DEEP_COPY_LITERAL_LIST));
    register_runtime_function("REVERSE", 1, reinterpret_cast<void*>(BCPL_REVERSE_LIST));
    register_runtime_function("FIND", 3, reinterpret_cast<void*>(BCPL_FIND_IN_LIST));
    register_runtime_function("FILTER", 2, reinterpret_cast<void*>(BCPL_LIST_FILTER));

    // --- Register SPLIT and JOIN string/list functions ---
    register_runtime_function("APND", 2, reinterpret_cast<void*>(BCPL_LIST_APPEND_INT));
    register_runtime_function("SPLIT", 2, reinterpret_cast<void*>(BCPL_SPLIT_STRING), FunctionType::STANDARD, VarType::POINTER_TO_STRING_LIST);
    register_runtime_function("JOIN", 2, reinterpret_cast<void*>(BCPL_JOIN_LIST), FunctionType::STANDARD, VarType::POINTER_TO_STRING);
    
    // String functions
    register_runtime_function("STRCOPY", 2, reinterpret_cast<void*>(STRCOPY));
    register_runtime_function("STRCMP", 2, reinterpret_cast<void*>(STRCMP));
    register_runtime_function("STRLEN", 1, reinterpret_cast<void*>(STRLEN));
    register_runtime_function("PACKSTRING", 1, reinterpret_cast<void*>(PACKSTRING));
    register_runtime_function("UNPACKSTRING", 1, reinterpret_cast<void*>(UNPACKSTRING));
    
    // File I/O functions
    register_runtime_function("SLURP", 1, reinterpret_cast<void*>(SLURP));
    register_runtime_function("SPIT", 2, reinterpret_cast<void*>(SPIT));
    
    // New File API functions
    register_runtime_function("FILE_OPEN_READ", 1, reinterpret_cast<void*>(FILE_OPEN_READ));
    register_runtime_function("FILE_OPEN_WRITE", 1, reinterpret_cast<void*>(FILE_OPEN_WRITE));
    register_runtime_function("FILE_OPEN_APPEND", 1, reinterpret_cast<void*>(FILE_OPEN_APPEND));
    register_runtime_function("FILE_CLOSE", 1, reinterpret_cast<void*>(FILE_CLOSE));
    register_runtime_function("FILE_WRITES", 2, reinterpret_cast<void*>(FILE_WRITES));
    register_runtime_function("FILE_READS", 1, reinterpret_cast<void*>(FILE_READS));
    register_runtime_function("FILE_READ", 3, reinterpret_cast<void*>(FILE_READ));
    register_runtime_function("FILE_WRITE", 3, reinterpret_cast<void*>(FILE_WRITE));
    register_runtime_function("FILE_SEEK", 3, reinterpret_cast<void*>(FILE_SEEK));
    register_runtime_function("FILE_TELL", 1, reinterpret_cast<void*>(FILE_TELL));
    register_runtime_function("FILE_EOF", 1, reinterpret_cast<void*>(FILE_EOF));
    
    // Random number functions
    register_runtime_function("RAND", 1, reinterpret_cast<void*>(RAND));
    register_runtime_function("RND", 1, reinterpret_cast<void*>(RND), FunctionType::FLOAT);
    register_runtime_function("FRND", 0, reinterpret_cast<void*>(FRND), FunctionType::FLOAT);
    
    // Float math functions
    register_runtime_function("FSIN", 1, reinterpret_cast<void*>(FSIN), FunctionType::FLOAT);
    register_runtime_function("FCOS", 1, reinterpret_cast<void*>(FCOS), FunctionType::FLOAT);
    register_runtime_function("FTAN", 1, reinterpret_cast<void*>(FTAN), FunctionType::FLOAT);
    register_runtime_function("FABS", 1, reinterpret_cast<void*>(FABS), FunctionType::FLOAT);
    register_runtime_function("FLOG", 1, reinterpret_cast<void*>(FLOG), FunctionType::FLOAT);
    register_runtime_function("FEXP", 1, reinterpret_cast<void*>(FEXP), FunctionType::FLOAT);
    
    // Type conversion functions
    register_runtime_function("FIX", 1, reinterpret_cast<void*>(FIX));
    
    // System functions
    register_runtime_function("FINISH", 0, reinterpret_cast<void*>(finish));
    register_runtime_function("NEWLINE", 0, reinterpret_cast<void*>(NEWLINE));
    
    // Register fast freelist return for TL
    register_runtime_function("returnNodeToFreelist", 1, reinterpret_cast<void*>(returnNodeToFreelist_runtime));

    if (manager.isTracingEnabled()) {
        std::cout << "Registered " << manager.get_registered_functions().size() 
                  << " runtime functions" << std::endl;
        manager.print_registered_functions();
    }
    
    // Register SDL2 functions if available and announce capabilities
#ifdef SDL2_RUNTIME_ENABLED
    if (RuntimeManager::instance().isTracingEnabled()) {
        std::cout << "Core runtime registered. SDL2 registration will follow..." << std::endl;
    }
    try {
        // Actually call the SDL2 registration function
        extern void register_sdl2_runtime_functions();
        register_sdl2_runtime_functions();
        if (RuntimeManager::instance().isTracingEnabled()) {
            std::cout << "SDL2 runtime functions registered in unified runtime" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "Warning: Failed to register SDL2 runtime functions: " << e.what() << std::endl;
        manager.announce_runtime_capabilities();
    }
#else
    // No SDL2 available, announce now
    manager.announce_runtime_capabilities();
#endif
}

void register_runtime_function(
    const std::string& name, 
    int num_args, 
    void* address, 
    FunctionType type,
    VarType return_type) {
    
    try {
        RuntimeManager::instance().register_function(name, num_args, address, type, return_type);
    } catch (const std::exception& e) {
        // Function might already be registered - this is often fine during development
        if (RuntimeManager::instance().isTracingEnabled()) {
            std::cerr << "Warning: " << e.what() << std::endl;
        }
    }
}

// initialize_freelist is declared in runtime_freelist.h

void initialize_runtime() {
#ifdef JIT_MODE
    printf("BCPL runtime initialized in JIT_MODE (HeapManager enabled)\n");
#endif
    initialize_freelist(); // Pre-allocate freelist nodes at startup
    if (RuntimeManager::instance().isTracingEnabled()) {
        std::cout << "BCPL Runtime v" << BCPL_RUNTIME_VERSION << " initialized" << std::endl;
    }
    
    // Note: Runtime capabilities announcement happens in register_runtime_functions()
    // after all functions are registered to avoid duplicate/premature announcements
}

void cleanup_runtime() {
    // Orderly shutdown of SAMM and runtime
    HeapManager::getInstance().shutdown();
    if (RuntimeManager::instance().isTracingEnabled()) {
        std::cout << "BCPL Runtime shutdown complete" << std::endl;
    }
}

std::string get_runtime_version() {
    return "BCPL Runtime v" + std::string(BCPL_RUNTIME_VERSION);
}

} // namespace runtime

// --- Add this code to the end of runtime/RuntimeBridge.cpp ---

// Create C-style wrappers to expose the namespaced C++ functions to the linker.
extern "C" {
    void initialize_runtime() {
        runtime::initialize_runtime();
    }

    void register_runtime_functions() {
        runtime::register_runtime_functions();
    }
    
    // This wrapper handles the C++ string to C-style string conversion.
    const char* get_runtime_version() {
        static std::string version_str;
        version_str = runtime::get_runtime_version();
        return version_str.c_str();
    }
}
