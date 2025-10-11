#include "runtime_api.h"
#include <cstring>

/**
 * @file runtime_api.cpp
 * @brief Runtime API Implementation - The Publisher
 * 
 * This file contains the master manifest of all runtime functions.
 * It serves as the single source of truth that the compiler imports.
 * 
 * To add a new runtime function:
 * 1. Add forward declaration below
 * 2. Add entry to g_runtime_manifest array
 * 3. Implement the function in appropriate runtime/ file
 */

// =============================================================================
// FORWARD DECLARATIONS - All Runtime Function Implementations
// =============================================================================

extern "C" {
    // Core I/O functions
    void WRITES(int string_ptr);
    void WRITEN(int value);
    void FWRITE(float value);
    void WRITEF(int format_ptr);
    void WRITEF1(int format_ptr, int arg1);
    void WRITEF2(int format_ptr, int arg1, int arg2);
    void WRITEF3(int format_ptr, int arg1, int arg2, int arg3);
    void WRITEF4(int format_ptr, int arg1, int arg2, int arg3, int arg4);
    void WRITEF5(int format_ptr, int arg1, int arg2, int arg3, int arg4, int arg5);
    void WRITEF6(int format_ptr, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6);
    void WRITEF7(int format_ptr, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7);
    void WRITEC(int char_value);
    int RDCH();
    void NEWLINE();
    
    // String/List operations
    int SPLIT(int string_ptr, int delimiter_ptr);
    int JOIN(int list_ptr, int delimiter_ptr);
    int STRLEN(int string_ptr);
    int STRCMP(int str1_ptr, int str2_ptr);
    void STRCOPY(int dest_ptr, int src_ptr);
    int PACKSTRING(int string_ptr);
    int UNPACKSTRING(int packed_ptr);
    int SLURP(int filename_ptr);
    int SPIT(int filename_ptr, int content_ptr);
    
    // Memory management
    int bcpl_alloc_words(int count, int size, int type);
    int bcpl_alloc_chars(int count);
    int bcpl_getvec(int size);
    int bcpl_fgetvec(int size);
    void bcpl_free(int ptr);
    void bcpl_free_list(int list_ptr);
    void bcpl_free_list_safe(int list_ptr);
    int BCPL_GET_LAST_ERROR(int buffer_ptr);
    void BCPL_CLEAR_ERRORS();
    void BCPL_CHECK_AND_DISPLAY_ERRORS();
    void BCPL_BOUNDS_ERROR(int index, int size, int context);
    void BCPL_FREE_CELLS();
    int get_g_free_list_head_address();
    
    // List operations
    int BCPL_LIST_CREATE_EMPTY();
    int BCPL_LIST_APPEND_INT(int list_ptr, int value);
    int BCPL_LIST_APPEND_FLOAT(int list_ptr, float value);
    int BCPL_LIST_APPEND_STRING(int list_ptr, int string_ptr);
    int BCPL_LIST_APPEND_LIST(int list_ptr, int other_list);
    int BCPL_LIST_GET_HEAD_AS_INT(int list_ptr);
    float BCPL_LIST_GET_HEAD_AS_FLOAT(int list_ptr);
    int BCPL_LIST_GET_TAIL(int list_ptr);
    int BCPL_LIST_GET_REST(int list_ptr);
    int BCPL_GET_ATOM_TYPE(int atom_ptr);
    int BCPL_LIST_GET_NTH(int list_ptr, int index);
    int BCPL_CONCAT_LISTS(int list1_ptr, int list2_ptr);
    int BCPL_SHALLOW_COPY_LIST(int list_ptr);
    int BCPL_DEEP_COPY_LIST(int list_ptr);
    int BCPL_DEEP_COPY_LITERAL_LIST(int list_ptr);
    int BCPL_REVERSE_LIST(int list_ptr);
    int BCPL_FIND_IN_LIST(int list_ptr, int value, int compare_func);
    int BCPL_LIST_FILTER(int list_ptr, int filter_func);
    void returnNodeToFreelist_runtime(int node_ptr);
    
    // Math functions
    int RAND(int max_val);
    float RND(int max_val);
    float FRND();
    float FSIN(float angle);
    float FCOS(float angle);
    float FTAN(float angle);
    float FABS(float value);
    float FLOG(float value);
    float FEXP(float value);
    int FIX(float value);
    
    // File I/O
    int FILE_OPEN_READ(int filename_ptr);
    int FILE_OPEN_WRITE(int filename_ptr);
    int FILE_OPEN_APPEND(int filename_ptr);
    int FILE_CLOSE(int handle);
    int FILE_WRITES(int handle, int string_ptr);
    int FILE_READS(int handle);
    int FILE_READ(int handle, int buffer_ptr, int size);
    int FILE_WRITE(int handle, int buffer_ptr, int size);
    int FILE_SEEK(int handle, int offset, int origin);
    int FILE_TELL(int handle);
    int FILE_EOF(int handle);
    
    // System functions
    void finish();
    
    // Heap Manager functions
    void HeapManager_enterScope();
    void HeapManager_exitScope();
    int HeapManager_isSAMMEnabled();
    void HeapManager_setSAMMEnabled(int enabled);
    void HeapManager_waitForSAMM();
    int HeapManager_OBJECT_HEAP_ALLOC(int size);
    void HeapManager_OBJECT_HEAP_FREE(int ptr);
    int HeapManager_RUNTIME_METHOD_LOOKUP(int class_id, int method_id);
    int HeapManager_PIC_RUNTIME_HELPER();
}

// =============================================================================
// RUNTIME FUNCTION MANIFEST - SINGLE SOURCE OF TRUTH
// =============================================================================

/**
 * Master manifest of all runtime functions published by the runtime library.
 * This is the authoritative list that the compiler imports.
 * 
 * IMPORTANT: Every runtime function callable from BCPL must have an entry here.
 * The compiler will ONLY know about functions listed in this manifest.
 */
static const RuntimeFunctionDescriptor g_runtime_manifest[] = {
    
    // -------------------------------------------------------------------------
    // CORE I/O FUNCTIONS
    // -------------------------------------------------------------------------
    {
        "WRITES", "_WRITES", reinterpret_cast<RuntimeFunctionPtr>(WRITES), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Write a string to standard output", "I/O"
    },
    {
        "WRITEN", "_WRITEN", reinterpret_cast<RuntimeFunctionPtr>(WRITEN), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Write an integer to standard output", "I/O"
    },
    {
        "FWRITE", "_FWRITE", reinterpret_cast<RuntimeFunctionPtr>(FWRITE), 1,
        RuntimeFunctionType::FLOAT_ROUTINE, RuntimeReturnType::VOID,
        "Write a float to standard output", "I/O"
    },
    {
        "WRITEF", "_WRITEF", reinterpret_cast<RuntimeFunctionPtr>(WRITEF), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Write formatted output (base case)", "I/O"
    },
    {
        "WRITEF1", "_WRITEF1", reinterpret_cast<RuntimeFunctionPtr>(WRITEF1), 2,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Write formatted output with 1 argument", "I/O"
    },
    {
        "WRITEF2", "_WRITEF2", reinterpret_cast<RuntimeFunctionPtr>(WRITEF2), 3,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Write formatted output with 2 arguments", "I/O"
    },
    {
        "WRITEF3", "_WRITEF3", reinterpret_cast<RuntimeFunctionPtr>(WRITEF3), 4,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Write formatted output with 3 arguments", "I/O"
    },
    {
        "WRITEF4", "_WRITEF4", reinterpret_cast<RuntimeFunctionPtr>(WRITEF4), 5,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Write formatted output with 4 arguments", "I/O"
    },
    {
        "WRITEF5", "_WRITEF5", reinterpret_cast<RuntimeFunctionPtr>(WRITEF5), 6,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Write formatted output with 5 arguments", "I/O"
    },
    {
        "WRITEF6", "_WRITEF6", reinterpret_cast<RuntimeFunctionPtr>(WRITEF6), 7,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Write formatted output with 6 arguments", "I/O"
    },
    {
        "WRITEF7", "_WRITEF7", reinterpret_cast<RuntimeFunctionPtr>(WRITEF7), 8,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Write formatted output with 7 arguments", "I/O"
    },
    {
        "WRITEC", "_WRITEC", reinterpret_cast<RuntimeFunctionPtr>(WRITEC), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Write a character to standard output", "I/O"
    },
    {
        "RDCH", "_RDCH", reinterpret_cast<RuntimeFunctionPtr>(RDCH), 0,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Read a character from standard input", "I/O"
    },
    {
        "NEWLINE", "_NEWLINE", reinterpret_cast<RuntimeFunctionPtr>(NEWLINE), 0,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Output a newline character", "I/O"
    },

    // -------------------------------------------------------------------------
    // STRING AND LIST OPERATIONS
    // -------------------------------------------------------------------------
    {
        "SPLIT", "_SPLIT", reinterpret_cast<RuntimeFunctionPtr>(SPLIT), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Split a string into a list using delimiter", "String"
    },
    {
        "JOIN", "_JOIN", reinterpret_cast<RuntimeFunctionPtr>(JOIN), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING,
        "Join a list of strings using delimiter", "String"
    },
    {
        "STRLEN", "_STRLEN", reinterpret_cast<RuntimeFunctionPtr>(STRLEN), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Get length of a string", "String"
    },
    {
        "STRCMP", "_STRCMP", reinterpret_cast<RuntimeFunctionPtr>(STRCMP), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Compare two strings", "String"
    },
    {
        "STRCOPY", "_STRCOPY", reinterpret_cast<RuntimeFunctionPtr>(STRCOPY), 2,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Copy one string to another", "String"
    },
    {
        "PACKSTRING", "_PACKSTRING", reinterpret_cast<RuntimeFunctionPtr>(PACKSTRING), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING,
        "Pack a string into memory", "String"
    },
    {
        "UNPACKSTRING", "_UNPACKSTRING", reinterpret_cast<RuntimeFunctionPtr>(UNPACKSTRING), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING,
        "Unpack a string from memory", "String"
    },
    {
        "SLURP", "_SLURP", reinterpret_cast<RuntimeFunctionPtr>(SLURP), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING,
        "Read entire file into string", "String"
    },
    {
        "SPIT", "_SPIT", reinterpret_cast<RuntimeFunctionPtr>(SPIT), 2,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Write string to file", "String"
    },

    // -------------------------------------------------------------------------
    // MEMORY MANAGEMENT
    // -------------------------------------------------------------------------
    {
        "GETVEC", "_GETVEC", reinterpret_cast<RuntimeFunctionPtr>(bcpl_getvec), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INT_VECTOR,
        "Allocate integer vector", "Memory"
    },
    {
        "FGETVEC", "_FGETVEC", reinterpret_cast<RuntimeFunctionPtr>(bcpl_fgetvec), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::FLOAT_VECTOR,
        "Allocate float vector", "Memory"
    },
    {
        "FREEVEC", "_FREEVEC", reinterpret_cast<RuntimeFunctionPtr>(bcpl_free), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Free allocated vector", "Memory"
    },
    {
        "BCPL_ALLOC_WORDS", "_BCPL_ALLOC_WORDS", reinterpret_cast<RuntimeFunctionPtr>(bcpl_alloc_words), 3,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Allocate words with type info", "Memory"
    },
    {
        "BCPL_ALLOC_CHARS", "_BCPL_ALLOC_CHARS", reinterpret_cast<RuntimeFunctionPtr>(bcpl_alloc_chars), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Allocate character buffer", "Memory"
    },
    {
        "MALLOC", "_MALLOC", reinterpret_cast<RuntimeFunctionPtr>(bcpl_alloc_words), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Allocate memory (alias for BCPL_ALLOC_WORDS)", "Memory"
    },
    {
        "BCPL_FREE_LIST", "_BCPL_FREE_LIST", reinterpret_cast<RuntimeFunctionPtr>(bcpl_free_list), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Free a list structure", "Memory"
    },
    {
        "BCPL_FREE_LIST_SAFE", "_BCPL_FREE_LIST_SAFE", reinterpret_cast<RuntimeFunctionPtr>(bcpl_free_list_safe), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Safely free a list structure", "Memory"
    },
    {
        "BCPL_GET_LAST_ERROR", "_BCPL_GET_LAST_ERROR", reinterpret_cast<RuntimeFunctionPtr>(BCPL_GET_LAST_ERROR), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Get last error message", "Memory"
    },
    {
        "BCPL_CLEAR_ERRORS", "_BCPL_CLEAR_ERRORS", reinterpret_cast<RuntimeFunctionPtr>(BCPL_CLEAR_ERRORS), 0,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Clear error state", "Memory"
    },
    {
        "BCPL_CHECK_AND_DISPLAY_ERRORS", "_BCPL_CHECK_AND_DISPLAY_ERRORS", reinterpret_cast<RuntimeFunctionPtr>(BCPL_CHECK_AND_DISPLAY_ERRORS), 0,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Check and display any errors", "Memory"
    },
    {
        "BCPL_BOUNDS_ERROR", "_BCPL_BOUNDS_ERROR", reinterpret_cast<RuntimeFunctionPtr>(BCPL_BOUNDS_ERROR), 3,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Report bounds checking error", "Memory"
    },
    {
        "BCPL_FREE_CELLS", "_BCPL_FREE_CELLS", reinterpret_cast<RuntimeFunctionPtr>(BCPL_FREE_CELLS), 0,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Free memory cells", "Memory"
    },
    {
        "GET_FREE_LIST_HEAD_ADDR", "_GET_FREE_LIST_HEAD_ADDR", reinterpret_cast<RuntimeFunctionPtr>(get_g_free_list_head_address), 0,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Get address of free list head", "Memory"
    },

    // -------------------------------------------------------------------------
    // LIST OPERATIONS
    // -------------------------------------------------------------------------
    {
        "LIST_CREATE", "_LIST_CREATE", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_CREATE_EMPTY), 0,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Create empty list", "List"
    },
    {
        "LIST_APPEND_INT", "_LIST_APPEND_INT", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_APPEND_INT), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Append integer to list", "List"
    },
    {
        "LIST_APPEND_FLOAT", "_LIST_APPEND_FLOAT", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_APPEND_FLOAT), 2,
        RuntimeFunctionType::FLOAT, RuntimeReturnType::STRING_LIST,
        "Append float to list", "List"
    },
    {
        "LIST_APPEND_STRING", "_LIST_APPEND_STRING", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_APPEND_STRING), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Append string to list", "List"
    },
    {
        "LIST_HEAD_INT", "_LIST_HEAD_INT", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_GET_HEAD_AS_INT), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Get first element of list as integer", "List"
    },
    {
        "LIST_HEAD_FLOAT", "_LIST_HEAD_FLOAT", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_GET_HEAD_AS_FLOAT), 1,
        RuntimeFunctionType::FLOAT, RuntimeReturnType::FLOAT,
        "Get first element of list as float", "List"
    },
    {
        "LIST_TAIL", "_LIST_TAIL", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_GET_TAIL), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Get tail (rest) of list", "List"
    },
    {
        "LIST_FREE", "_LIST_FREE", reinterpret_cast<RuntimeFunctionPtr>(bcpl_free_list), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Free a list and all its elements", "List"
    },
    {
        "BCPL_LIST_APPEND_LIST", "_BCPL_LIST_APPEND_LIST", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_APPEND_LIST), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Append list to another list", "List"
    },
    {
        "BCPL_LIST_GET_REST", "_BCPL_LIST_GET_REST", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_GET_REST), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Get rest of list (alias for tail)", "List"
    },
    {
        "BCPL_GET_ATOM_TYPE", "_BCPL_GET_ATOM_TYPE", reinterpret_cast<RuntimeFunctionPtr>(BCPL_GET_ATOM_TYPE), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Get type of atomic value", "List"
    },
    {
        "BCPL_LIST_GET_NTH", "_BCPL_LIST_GET_NTH", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_GET_NTH), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Get nth element of list", "List"
    },
    {
        "BCPL_CONCAT_LISTS", "_BCPL_CONCAT_LISTS", reinterpret_cast<RuntimeFunctionPtr>(BCPL_CONCAT_LISTS), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Concatenate two lists", "List"
    },
    {
        "CONCAT", "_CONCAT", reinterpret_cast<RuntimeFunctionPtr>(BCPL_CONCAT_LISTS), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Concatenate two lists (alias)", "List"
    },
    {
        "COPYLIST", "_COPYLIST", reinterpret_cast<RuntimeFunctionPtr>(BCPL_SHALLOW_COPY_LIST), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Create shallow copy of list", "List"
    },
    {
        "DEEPCOPYLIST", "_DEEPCOPYLIST", reinterpret_cast<RuntimeFunctionPtr>(BCPL_DEEP_COPY_LIST), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Create deep copy of list", "List"
    },
    {
        "DEEPCOPYLITERALLIST", "_DEEPCOPYLITERALLIST", reinterpret_cast<RuntimeFunctionPtr>(BCPL_DEEP_COPY_LITERAL_LIST), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Create deep copy of literal list", "List"
    },
    {
        "REVERSE", "_REVERSE", reinterpret_cast<RuntimeFunctionPtr>(BCPL_REVERSE_LIST), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Reverse a list", "List"
    },
    {
        "FIND", "_FIND", reinterpret_cast<RuntimeFunctionPtr>(BCPL_FIND_IN_LIST), 3,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Find element in list", "List"
    },
    {
        "FILTER", "_FILTER", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_FILTER), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Filter list elements", "List"
    },
    {
        "APND", "_APND", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_APPEND_INT), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Append integer to list (alias)", "List"
    },
    {
        "FPND", "_FPND", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_APPEND_FLOAT), 2,
        RuntimeFunctionType::FLOAT, RuntimeReturnType::STRING_LIST,
        "Append float to list (alias)", "List"
    },
    {
        "SPND", "_SPND", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_APPEND_STRING), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Append string to list (alias)", "List"
    },
    {
        "LPND", "_LPND", reinterpret_cast<RuntimeFunctionPtr>(BCPL_LIST_APPEND_LIST), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
        "Append list to list (alias)", "List"
    },
    {
        "returnNodeToFreelist", "_returnNodeToFreelist", reinterpret_cast<RuntimeFunctionPtr>(returnNodeToFreelist_runtime), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Return node to freelist", "List"
    },

    // -------------------------------------------------------------------------
    // MATHEMATICAL FUNCTIONS
    // -------------------------------------------------------------------------
    {
        "RAND", "_RAND", reinterpret_cast<RuntimeFunctionPtr>(RAND), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Generate random integer (0 to max_val-1)", "Math"
    },
    {
        "RND", "_RND", reinterpret_cast<RuntimeFunctionPtr>(RND), 1,
        RuntimeFunctionType::FLOAT, RuntimeReturnType::FLOAT,
        "Generate random float (0.0 to max_val)", "Math"
    },
    {
        "FRND", "_FRND", reinterpret_cast<RuntimeFunctionPtr>(FRND), 0,
        RuntimeFunctionType::FLOAT, RuntimeReturnType::FLOAT,
        "Generate random float (0.0 to 1.0)", "Math"
    },
    {
        "FSIN", "_FSIN", reinterpret_cast<RuntimeFunctionPtr>(FSIN), 1,
        RuntimeFunctionType::FLOAT, RuntimeReturnType::FLOAT,
        "Sine function (radians)", "Math"
    },
    {
        "FCOS", "_FCOS", reinterpret_cast<RuntimeFunctionPtr>(FCOS), 1,
        RuntimeFunctionType::FLOAT, RuntimeReturnType::FLOAT,
        "Cosine function (radians)", "Math"
    },
    {
        "FTAN", "_FTAN", reinterpret_cast<RuntimeFunctionPtr>(FTAN), 1,
        RuntimeFunctionType::FLOAT, RuntimeReturnType::FLOAT,
        "Tangent function (radians)", "Math"
    },
    {
        "FABS", "_FABS", reinterpret_cast<RuntimeFunctionPtr>(FABS), 1,
        RuntimeFunctionType::FLOAT, RuntimeReturnType::FLOAT,
        "Absolute value (float)", "Math"
    },
    {
        "FLOG", "_FLOG", reinterpret_cast<RuntimeFunctionPtr>(FLOG), 1,
        RuntimeFunctionType::FLOAT, RuntimeReturnType::FLOAT,
        "Natural logarithm", "Math"
    },
    {
        "FEXP", "_FEXP", reinterpret_cast<RuntimeFunctionPtr>(FEXP), 1,
        RuntimeFunctionType::FLOAT, RuntimeReturnType::FLOAT,
        "Exponential function", "Math"
    },
    {
        "FIX", "_FIX", reinterpret_cast<RuntimeFunctionPtr>(FIX), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Convert float to integer", "Math"
    },

    // -------------------------------------------------------------------------
    // FILE I/O OPERATIONS
    // -------------------------------------------------------------------------
    {
        "FILE_OPEN_READ", "_FILE_OPEN_READ", reinterpret_cast<RuntimeFunctionPtr>(FILE_OPEN_READ), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Open file for reading", "File"
    },
    {
        "FILE_OPEN_WRITE", "_FILE_OPEN_WRITE", reinterpret_cast<RuntimeFunctionPtr>(FILE_OPEN_WRITE), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Open file for writing", "File"
    },
    {
        "FILE_OPEN_APPEND", "_FILE_OPEN_APPEND", reinterpret_cast<RuntimeFunctionPtr>(FILE_OPEN_APPEND), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Open file for appending", "File"
    },
    {
        "FILE_CLOSE", "_FILE_CLOSE", reinterpret_cast<RuntimeFunctionPtr>(FILE_CLOSE), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Close file handle", "File"
    },
    {
        "FILE_WRITES", "_FILE_WRITES", reinterpret_cast<RuntimeFunctionPtr>(FILE_WRITES), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Write string to file", "File"
    },
    {
        "FILE_READS", "_FILE_READS", reinterpret_cast<RuntimeFunctionPtr>(FILE_READS), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING,
        "Read string from file", "File"
    },
    {
        "FILE_READ", "_FILE_READ", reinterpret_cast<RuntimeFunctionPtr>(FILE_READ), 3,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Read bytes from file", "File"
    },
    {
        "FILE_WRITE", "_FILE_WRITE", reinterpret_cast<RuntimeFunctionPtr>(FILE_WRITE), 3,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Write bytes to file", "File"
    },
    {
        "FILE_SEEK", "_FILE_SEEK", reinterpret_cast<RuntimeFunctionPtr>(FILE_SEEK), 3,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Seek to position in file", "File"
    },
    {
        "FILE_TELL", "_FILE_TELL", reinterpret_cast<RuntimeFunctionPtr>(FILE_TELL), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Get current position in file", "File"
    },
    {
        "FILE_EOF", "_FILE_EOF", reinterpret_cast<RuntimeFunctionPtr>(FILE_EOF), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Check if at end of file", "File"
    },

    // -------------------------------------------------------------------------
    // SYSTEM FUNCTIONS
    // -------------------------------------------------------------------------
    {
        "FINISH", "_FINISH", reinterpret_cast<RuntimeFunctionPtr>(finish), 0,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Terminate program execution", "System"
    },
    {
        "SETTYPE", "_SETTYPE", nullptr, 2,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Set type information (compiler intrinsic)", "System"
    },



    // -------------------------------------------------------------------------
    // HEAP MANAGER / MEMORY MANAGEMENT
    // -------------------------------------------------------------------------
    {
        "HeapManager_enter_scope", "_HeapManager_enter_scope", reinterpret_cast<RuntimeFunctionPtr>(HeapManager_enterScope), 0,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Enter new memory management scope", "Memory"
    },
    {
        "HeapManager_exit_scope", "_HeapManager_exit_scope", reinterpret_cast<RuntimeFunctionPtr>(HeapManager_exitScope), 0,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Exit current memory management scope", "Memory"
    },
    {
        "HEAPMANAGER_ISSAMMENABLED", "_HEAPMANAGER_ISSAMMENABLED", reinterpret_cast<RuntimeFunctionPtr>(HeapManager_isSAMMEnabled), 0,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Check if SAMM (Scope Aware Memory Management) is enabled", "Memory"
    },
    {
        "HEAPMANAGER_SETSAMMENABLED", "_HEAPMANAGER_SETSAMMENABLED", reinterpret_cast<RuntimeFunctionPtr>(HeapManager_setSAMMEnabled), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Enable or disable SAMM", "Memory"
    },
    {
        "HEAPMANAGER_WAITFORSAMM", "_HEAPMANAGER_WAITFORSAMM", reinterpret_cast<RuntimeFunctionPtr>(HeapManager_waitForSAMM), 0,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Wait for SAMM operations to complete", "Memory"
    },
    {
        "OBJECT_HEAP_ALLOC", "_OBJECT_HEAP_ALLOC", reinterpret_cast<RuntimeFunctionPtr>(HeapManager_OBJECT_HEAP_ALLOC), 1,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Allocate object on heap", "Memory"
    },
    {
        "OBJECT_HEAP_FREE", "_OBJECT_HEAP_FREE", reinterpret_cast<RuntimeFunctionPtr>(HeapManager_OBJECT_HEAP_FREE), 1,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Free object from heap", "Memory"
    },
    {
        "RUNTIME_METHOD_LOOKUP", "_RUNTIME_METHOD_LOOKUP", reinterpret_cast<RuntimeFunctionPtr>(HeapManager_RUNTIME_METHOD_LOOKUP), 2,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Look up method at runtime", "Memory"
    },
    {
        "PIC_RUNTIME_HELPER", "_PIC_RUNTIME_HELPER", reinterpret_cast<RuntimeFunctionPtr>(HeapManager_PIC_RUNTIME_HELPER), 0,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Polymorphic inline cache helper", "Memory"
    }
};

// Calculate manifest size
static const int g_manifest_count = sizeof(g_runtime_manifest) / sizeof(g_runtime_manifest[0]);

// =============================================================================
// API IMPLEMENTATION FUNCTIONS
// =============================================================================

const RuntimeFunctionDescriptor* get_runtime_manifest(int& count) {
    count = g_manifest_count;
    return g_runtime_manifest;
}

const char* get_runtime_api_version() {
    return "1.0.0";
}

void initialize_runtime_system() {
    // Initialize any global runtime state here
    // This replaces scattered initialization calls
}

void shutdown_runtime_system() {
    // Clean shutdown of runtime resources
}

bool verify_runtime_manifest() {
    // Verify all function pointers are non-null (except SETTYPE which is a compiler intrinsic)
    for (int i = 0; i < g_manifest_count; ++i) {
        const auto& desc = g_runtime_manifest[i];
        // SETTYPE is special - it's a compiler intrinsic with null function pointer
        if (!desc.function_pointer && strcmp(desc.veneer_name, "SETTYPE") != 0) {
            return false;
        }
        if (!desc.veneer_name || !desc.linker_symbol || !desc.description) {
            return false;
        }
        if (desc.arg_count < 0) {
            return false;
        }
    }
    return true;
}

const RuntimeFunctionDescriptor* lookup_runtime_function(const char* veneer_name) {
    if (!veneer_name) return nullptr;
    
    for (int i = 0; i < g_manifest_count; ++i) {
        if (strcmp(g_runtime_manifest[i].veneer_name, veneer_name) == 0) {
            return &g_runtime_manifest[i];
        }
    }
    return nullptr;
}

const RuntimeFunctionDescriptor** get_functions_by_category(const char* category, int& count) {
    static const RuntimeFunctionDescriptor* category_functions[256]; // Static buffer
    count = 0;
    
    if (!category) return nullptr;
    
    for (int i = 0; i < g_manifest_count && count < 255; ++i) {
        if (strcmp(g_runtime_manifest[i].category, category) == 0) {
            category_functions[count++] = &g_runtime_manifest[i];
        }
    }
    
    return category_functions;
}