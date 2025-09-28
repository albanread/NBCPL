// runtime/heap_interface.cpp
//
// This file provides the C++ implementation for the BCPL runtime's list
// and memory management functions, specifically for JIT mode. It acts as a
// bridge between the C++ compiler components (like RuntimeManager) and the
// underlying C-style freelist and allocation logic.
// All functions are exposed with C linkage to be callable from the JIT'd code
// and registered with the RuntimeManager.

#include "runtime.h"
#include "BCPLError.h"
#include "ListDataTypes.h"
#include "../HeapManager/HeapManager.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <iostream>

// --- Forward declarations for internal C-style freelist functions ---
// These are implemented in jit_heap_bridge.cpp (which is included by jit_runtime.cpp)
// and provide the raw nodes for list construction.
#ifdef __cplusplus
extern "C" {
#endif

ListAtom* getNodeFromFreelist();
void returnNodeToFreelist(ListAtom* node);
ListHeader* getHeaderFromFreelist();
void returnHeaderToFreelist(ListHeader* header);

#ifdef __cplusplus
}
#endif

extern "C" {

// ============================================================================
// List Creation and Manipulation
// ============================================================================

/**
 * @brief Creates a new, empty list header.
 * @return A pointer to the newly allocated ListHeader.
 */
ListHeader* BCPL_LIST_CREATE_EMPTY(void) {
    ListHeader* header = (ListHeader*)HeapManager::getInstance().allocList();
    if (header) {
        header->type = ATOM_SENTINEL;
        header->contains_literals = 0; // Default to safe (no literals)
        header->length = 0;
        header->head = nullptr;
        header->tail = nullptr;
    }
    return header;
}

/**
 * @brief Appends an integer to a list (O(1) operation).
 */
void BCPL_LIST_APPEND_INT(ListHeader* header, int64_t value) {
    if (!header || header->type != ATOM_SENTINEL) return;

    ListAtom* new_node = getNodeFromFreelist();
    new_node->type = ATOM_INT;
    new_node->pad = 0;
    new_node->value.int_value = value;
    new_node->next = nullptr;

    if (header->head == nullptr) {
        header->head = new_node;
        header->tail = new_node;
    } else {
        header->tail->next = new_node;
        header->tail = new_node;
    }
    header->length++;
}

/**
 * @brief Appends a float to a list (O(1) operation).
 */
void BCPL_LIST_APPEND_FLOAT(ListHeader* header, double value) {
    if (!header || header->type != ATOM_SENTINEL) return;

    ListAtom* new_node = getNodeFromFreelist();
    new_node->type = ATOM_FLOAT;
    new_node->pad = 0;
    new_node->value.float_value = value;
    new_node->next = nullptr;

    if (header->head == nullptr) {
        header->head = new_node;
        header->tail = new_node;
    } else {
        header->tail->next = new_node;
        header->tail = new_node;
    }
    header->length++;
}

/**
 * @brief Appends a string pointer to a list (O(1) operation).
 */
void BCPL_LIST_APPEND_STRING(ListHeader* header, uint32_t* value) {
    if (!header || header->type != ATOM_SENTINEL) return;

    ListAtom* new_node = getNodeFromFreelist();
    new_node->type = ATOM_STRING;
    new_node->pad = 0;
    new_node->value.ptr_value = value;
    new_node->next = nullptr;

    if (header->head == nullptr) {
        header->head = new_node;
        header->tail = new_node;
    } else {
        header->tail->next = new_node;
        header->tail = new_node;
    }
    header->length++;
}

/**
 * @brief Appends a nested list pointer to a list (O(1) operation).
 */
void BCPL_LIST_APPEND_LIST(ListHeader* header, ListHeader* list_to_append) {
    if (!header || header->type != ATOM_SENTINEL) return;

    ListAtom* new_node = getNodeFromFreelist();
    new_node->type = ATOM_LIST_POINTER;
    new_node->pad = 0;
    new_node->value.ptr_value = list_to_append;
    new_node->next = nullptr;

    if (header->head == nullptr) {
        header->head = new_node;
        header->tail = new_node;
    } else {
        header->tail->next = new_node;
        header->tail = new_node;
    }
    header->length++;
}

/**
 * @brief Appends an object pointer to a list (O(1) operation).
 */
void BCPL_LIST_APPEND_OBJECT(ListHeader* header, void* object_ptr) {
    if (!header || header->type != ATOM_SENTINEL) return;

    ListAtom* new_node = getNodeFromFreelist();
    new_node->type = ATOM_OBJECT;
    new_node->pad = 0;
    new_node->value.ptr_value = object_ptr;
    new_node->next = nullptr;

    if (header->head == nullptr) {
        header->head = new_node;
        header->tail = new_node;
    } else {
        header->tail->next = new_node;
        header->tail = new_node;
    }
    header->length++;
}


// ============================================================================
// List Accessors
// ============================================================================

int64_t BCPL_LIST_GET_HEAD_AS_INT(void* header_ptr) {
    ListHeader* header = (ListHeader*)header_ptr;
    if (!header || header->type != ATOM_SENTINEL || !header->head) return 0;
    return header->head->value.int_value;
}

double BCPL_LIST_GET_HEAD_AS_FLOAT(void* header_ptr) {
    ListHeader* header = (ListHeader*)header_ptr;
    if (!header || header->type != ATOM_SENTINEL || !header->head) return 0.0;
    return header->head->value.float_value;
}

void* BCPL_LIST_GET_TAIL(void* header_ptr) {
    ListHeader* header = (ListHeader*)header_ptr;
    if (!header || header->type != ATOM_SENTINEL) return nullptr;
    return header->tail;
}

void* BCPL_LIST_GET_REST(void* header_ptr) {
    ListHeader* header = (ListHeader*)header_ptr;
    if (!header || header->type != ATOM_SENTINEL || !header->head) return nullptr;
    return header->head->next;
}

int64_t BCPL_GET_ATOM_TYPE(void* header_ptr) {
    ListHeader* header = (ListHeader*)header_ptr;
    if (!header || header->type != ATOM_SENTINEL || !header->head) return -1;
    return (int64_t)header->head->type;
}

void* BCPL_LIST_GET_NTH(void* header_ptr, int64_t n) {
    ListHeader* header = (ListHeader*)header_ptr;
    if (!header || header->type != ATOM_SENTINEL || n < 0) return nullptr;
    ListAtom* current = header->head;
    for (int64_t i = 0; i < n && current; ++i) {
        current = current->next;
    }
    return current;
}


// ============================================================================
// List Utilities (Copy, Concat, etc.)
// ============================================================================

ListHeader* BCPL_SHALLOW_COPY_LIST(ListHeader* original_header) {
    if (!original_header) return nullptr;
    ListHeader* new_header = BCPL_LIST_CREATE_EMPTY();
    ListAtom* current_original = original_header->head;
    while (current_original) {
        ListAtom* new_node = getNodeFromFreelist();
        new_node->type = current_original->type;
        new_node->value = current_original->value;
        new_node->next = nullptr;
        if (new_header->head == nullptr) {
            new_header->head = new_node;
            new_header->tail = new_node;
        } else {
            new_header->tail->next = new_node;
            new_header->tail = new_node;
        }
        new_header->length++;
        current_original = current_original->next;
    }
    return new_header;
}

ListHeader* BCPL_DEEP_COPY_LIST(ListHeader* original_header) {
    if (!original_header) return nullptr;
    ListHeader* new_header = BCPL_LIST_CREATE_EMPTY();
    ListAtom* current_original = original_header->head;
    while (current_original) {
        ListAtom* new_node = getNodeFromFreelist();
        new_node->type = current_original->type;
        new_node->next = nullptr;

        switch (current_original->type) {
            case ATOM_STRING: {
                uint64_t* base_ptr = (uint64_t*)current_original->value.ptr_value;
                size_t len = base_ptr[0];
                uint32_t* new_str_payload = (uint32_t*)bcpl_alloc_chars(len);
                memcpy(new_str_payload, (uint32_t*)(base_ptr + 1), (len + 1) * sizeof(uint32_t));
                new_node->value.ptr_value = (uint64_t*)new_str_payload - 1;
                break;
            }
            case ATOM_LIST_POINTER:
                new_node->value.ptr_value = BCPL_DEEP_COPY_LIST((ListHeader*)current_original->value.ptr_value);
                break;
            default:
                new_node->value = current_original->value;
                break;
        }

        if (new_header->head == nullptr) {
            new_header->head = new_node;
            new_header->tail = new_node;
        } else {
            new_header->tail->next = new_node;
            new_header->tail = new_node;
        }
        new_header->length++;
        current_original = current_original->next;
    }
    return new_header;
}

ListHeader* BCPL_DEEP_COPY_LITERAL_LIST(ListLiteralHeader* literal_header) {
    if (!literal_header) return nullptr;
    ListHeader* new_header = BCPL_LIST_CREATE_EMPTY();
    new_header->contains_literals = 0; // Mark as safe to free - all strings are deep copied
    ListAtom* current_original = literal_header->head;
    while (current_original) {
        ListAtom* new_node = getNodeFromFreelist();
        new_node->type = current_original->type;
        new_node->next = nullptr;
        // Deep copy logic for strings/nested lists
        switch (current_original->type) {
            case ATOM_STRING: {
                uint64_t* base_ptr = (uint64_t*)current_original->value.ptr_value;
                size_t len = base_ptr[0];
                uint32_t* new_str_payload = (uint32_t*)bcpl_alloc_chars(len);
                memcpy(new_str_payload, (uint32_t*)(base_ptr + 1), (len + 1) * sizeof(uint32_t));
                new_node->value.ptr_value = (uint64_t*)new_str_payload - 1;
                break;
            }
            case ATOM_LIST_POINTER:
                new_node->value.ptr_value = BCPL_DEEP_COPY_LITERAL_LIST((ListLiteralHeader*)current_original->value.ptr_value);
                break;
            default:
                new_node->value = current_original->value;
                break;
        }
        if (new_header->head == nullptr) {
            new_header->head = new_node;
            new_header->tail = new_node;
        } else {
            new_header->tail->next = new_node;
            new_header->tail = new_node;
        }
        new_header->length++;
        current_original = current_original->next;
    }
    return new_header;
}


ListHeader* BCPL_CONCAT_LISTS(ListHeader* list1, ListHeader* list2) {
    ListHeader* new_list = BCPL_SHALLOW_COPY_LIST(list1);
    if (!new_list) return BCPL_SHALLOW_COPY_LIST(list2);
    if (list2 && list2->head) {
        ListAtom* current_original = list2->head;
        while (current_original) {
            // This is a shallow copy append
            if (new_list->head == nullptr) {
                new_list->head = current_original;
                new_list->tail = current_original;
            } else {
                new_list->tail->next = current_original;
                new_list->tail = current_original;
            }
            new_list->length++;
            current_original = current_original->next;
        }
    }
    return new_list;
}

ListHeader* BCPL_REVERSE_LIST(ListHeader* original_header) {
    if (!original_header) return nullptr;
    ListHeader* new_header = BCPL_LIST_CREATE_EMPTY();
    ListAtom* current_original = original_header->head;
    while (current_original) {
        ListAtom* new_node = getNodeFromFreelist();
        new_node->type = current_original->type;
        new_node->value = current_original->value;
        new_node->next = new_header->head; // Prepend
        new_header->head = new_node;
        if (new_header->tail == nullptr) new_header->tail = new_node;
        new_header->length++;
        current_original = current_original->next;
    }
    return new_header;
}

ListAtom* BCPL_FIND_IN_LIST(ListHeader* header, int64_t value_bits, int64_t type_tag) {
    if (!header) return nullptr;
    ListAtom* current = header->head;
    while (current) {
        if (current->type == type_tag) {
            if (type_tag == ATOM_INT && current->value.int_value == value_bits) return current;
            if (type_tag == ATOM_FLOAT) {
                double target_float;
                memcpy(&target_float, &value_bits, sizeof(double));
                if (current->value.float_value == target_float) return current;
            }
        }
        current = current->next;
    }
    return nullptr;
}

ListHeader* BCPL_LIST_FILTER(ListHeader* header, int64_t (*predicate)(int64_t)) {
    if (!header || !predicate) return nullptr;
    ListHeader* new_header = BCPL_LIST_CREATE_EMPTY();
    ListAtom* current = header->head;
    while (current) {
        if (predicate(current->value.int_value) != 0) {
            BCPL_LIST_APPEND_INT(new_header, current->value.int_value); // Simplified for int
        }
        current = current->next;
    }
    return new_header;
}


// ============================================================================
// Freelist Management
// ============================================================================

void bcpl_free_list(void* header_ptr) {
    ListHeader* header = (ListHeader*)header_ptr;
    if (!header) return;
    
    ListAtom* current = header->head;
    while (current) {
        ListAtom* next = current->next;
        
        // Only free string data if this list doesn't contain literals
        if (current->type == ATOM_STRING && current->value.ptr_value && !header->contains_literals) {
            // For strings, adjust the pointer back to the original allocation
            // The string pointer was adjusted by -1 during allocation to account for length prefix
            uint64_t* original_alloc_ptr = (uint64_t*)current->value.ptr_value + 1;
            bcpl_free(original_alloc_ptr);
        } else if (current->type == ATOM_LIST_POINTER && current->value.ptr_value) {
            // For nested lists, free recursively
            bcpl_free_list(current->value.ptr_value);
        }
        // Always free the list node itself
        returnNodeToFreelist(current);
        current = next;
    }
    HeapManager::getInstance().free(header);
}

// Safe version that handles lists with mixed literal/heap data
void bcpl_free_list_safe(void* header_ptr) {
    ListHeader* header = (ListHeader*)header_ptr;
    if (!header) return;
    
    // Check if header looks valid
    if ((uintptr_t)header < 0x1000) {
        _BCPL_SET_ERROR(ERROR_INVALID_POINTER, "bcpl_free_list_safe", "Skipping cleanup of invalid list pointer (likely corrupted during FOREACH)");
        return; // Skip obviously invalid pointers
    }
    
    ListAtom* current = header->head;
    int node_count = 0;
    while (current && node_count < 1000) { // Limit iterations to prevent infinite loops
        ListAtom* next = current->next;
        
        // Only try to free nested lists, skip strings to avoid literal data issues
        if (current->type == ATOM_LIST_POINTER && current->value.ptr_value) {
            try {
                // Try to free nested list recursively
                bcpl_free_list_safe(current->value.ptr_value);
            } catch (...) {
                _BCPL_SET_ERROR(ERROR_INVALID_POINTER, "bcpl_free_list_safe", "Warning: Failed to free nested list during cleanup");
            }
        }
        // Note: We skip freeing ATOM_STRING data to avoid literal string issues
        
        returnNodeToFreelist(current);
        current = next;
        node_count++;
    }
    
    if (node_count < 1000) {
        try {
            HeapManager::getInstance().free(header);
        } catch (...) {
            _BCPL_SET_ERROR(ERROR_INVALID_POINTER, "bcpl_free_list_safe", "Warning: Failed to free list header during cleanup");
        }
    } else {
        _BCPL_SET_ERROR(ERROR_INVALID_POINTER, "bcpl_free_list_safe", "Warning: List cleanup stopped - possible circular reference");
    }
}

void BCPL_FREE_CELLS(void) {
    // This function's implementation would depend on how the global freelist is managed
    // and might involve iterating and freeing large chunks of memory.
    // For now, it's a placeholder.
}

void* get_g_free_list_head_address(void) {
    // This is unsafe and for debugging only.
    return (void*)&getNodeFromFreelist; // Placeholder
}

void returnNodeToFreelist_runtime(void* node) {
    returnNodeToFreelist((ListAtom*)node);
}

// Alias for bcpl_free_list -> BCPL_FREE_LIST
void BCPL_FREE_LIST(void* header_ptr) {
    bcpl_free_list(header_ptr);
}

// Note: BCPL_GET_LAST_ERROR and BCPL_CLEAR_ERRORS are now defined in BCPLError.c
// to avoid duplicate symbols. They are declared in BCPLError.h and registered
// in RuntimeBridge.cpp for runtime access.

// Function to check and display any runtime warnings/errors
void BCPL_CHECK_AND_DISPLAY_ERRORS(void) {
    if (g_recent_error_index == 0) return; // No errors to display
    
    std::cout << "\n--- Runtime Warnings/Errors ---\n";
    
    size_t start_index = (g_recent_error_index > MAX_RECENT_ERRORS) ? 
                        (g_recent_error_index - MAX_RECENT_ERRORS) : 0;
    
    for (size_t i = start_index; i < g_recent_error_index; i++) {
        size_t idx = i % MAX_RECENT_ERRORS;
        const volatile auto& error = g_recent_errors[idx];
        
        if (error.code != RUNTIME_OK) {
            const char* error_type = (error.code == ERROR_INVALID_POINTER) ? "WARNING" : "ERROR";
            std::cout << "[" << error_type << "] " << (const char*)error.function_name << ": " << (const char*)error.message << "\n";
        }
    }
    
    std::cout << "--------------------------------\n";
}

// Alias for BCPL_DEEP_COPY_LITERAL_LIST -> DEEPCOPYLITERALLIST
ListHeader* DEEPCOPYLITERALLIST(ListLiteralHeader* literal_header) {
    return BCPL_DEEP_COPY_LITERAL_LIST(literal_header);
}

} // extern "C"
