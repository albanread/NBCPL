/**
 * Pure C implementation of BCPL_SPLIT_STRING and BCPL_JOIN_LIST
 * for BCPLRuntime standalone build (no C++ standard library dependency).
 *
 * This file replaces runtime_string_ops.cpp for static builds.
 */

#include "runtime.h"         // For runtime function signatures and types
#include "ListDataTypes.h"   // For ListHeader, ListAtom, etc.
#include "heap_interface.h"  // For BCPL_LIST_CREATE_EMPTY, BCPL_LIST_APPEND_STRING, bcpl_alloc_chars
#include <string.h>          // For memcpy
#include <stdint.h>
#include <stddef.h>

/**
 * @brief Calculate the length of a BCPL string payload.
 * @param s Pointer to the start of the character data.
 * @return The number of characters before the null terminator.
 */
static size_t bcpl_strlen(const uint32_t* s) {
    if (!s) return 0;
    size_t len = 0;
    while (s[len] != 0) {
        len++;
    }
    return len;
}

/**
 * @brief Compare the start of a string with a delimiter.
 * @param s The string to check.
 * @param delimiter The delimiter to look for.
 * @param delimiter_len The length of the delimiter.
 * @return 1 if it matches, 0 otherwise.
 */
static int bcpl_strncmp(const uint32_t* s, const uint32_t* delimiter, size_t delimiter_len) {
    for (size_t i = 0; i < delimiter_len; ++i) {
        if (s[i] != delimiter[i]) {
            return 0; // No match
        }
        if (s[i] == 0) {
            return 0; // Source is shorter than delimiter
        }
    }
    return 1; // Match
}

/**
 * @brief Joins a list of BCPL strings into a single string using a delimiter.
 * This implementation relies on bcpl_alloc_chars handling 16-byte alignment.
 */
uint32_t* BCPL_JOIN_LIST(struct ListHeader* list_header, uint32_t* delimiter_payload) {
    if (!list_header || !list_header->head) {
        return (uint32_t*)bcpl_alloc_chars(0); // Return a new empty string
    }

    size_t delimiter_len = bcpl_strlen(delimiter_payload);

    // --- Pass 1: Calculate the total length required for the new string ---
    size_t total_char_len = 0;
    size_t element_count = 0;
    struct ListAtom* current = list_header->head;

    while (current) {
        if (current->type == ATOM_STRING && current->value.ptr_value != NULL) {
            uint64_t* base_ptr = (uint64_t*)current->value.ptr_value;
            total_char_len += base_ptr[0]; // Add the length from the prefix
            element_count++;
        }
        current = current->next;
    }

    if (element_count > 1) {
        total_char_len += delimiter_len * (element_count - 1);
    }

    // --- Pass 2: Allocate memory and build the final string ---
    uint32_t* result_payload = (uint32_t*)bcpl_alloc_chars(total_char_len);
    if (!result_payload) return NULL;

    uint32_t* cursor = result_payload;
    current = list_header->head;
    size_t i = 0;

    while (current) {
        if (current->type == ATOM_STRING && current->value.ptr_value != NULL) {
            uint64_t* base_ptr = (uint64_t*)current->value.ptr_value;
            uint64_t element_len = base_ptr[0];
            uint32_t* element_payload = (uint32_t*)(base_ptr + 1);

            memcpy(cursor, element_payload, element_len * sizeof(uint32_t));
            cursor += element_len;

            if (i < element_count - 1 && delimiter_len > 0) {
                memcpy(cursor, delimiter_payload, delimiter_len * sizeof(uint32_t));
                cursor += delimiter_len;
            }
            i++;
        }
        current = current->next;
    }

    // The null terminator is already set by bcpl_alloc_chars.
    return result_payload;
}

/**
 * @brief Splits a BCPL string by a delimiter into a list of new BCPL strings.
 * This implementation relies on bcpl_alloc_chars handling 16-byte alignment.
 */
struct ListHeader* BCPL_SPLIT_STRING(uint32_t* source_payload, uint32_t* delimiter_payload) {
    struct ListHeader* result_list = BCPL_LIST_CREATE_EMPTY();
    if (!source_payload || !delimiter_payload || !result_list) {
        return result_list; // Return empty list on invalid input
    }

    size_t delimiter_len = bcpl_strlen(delimiter_payload);

    // Edge case: empty delimiter splits into single characters
    if (delimiter_len == 0) {
        for (const uint32_t* p = source_payload; *p != 0; ++p) {
            uint32_t* char_payload = (uint32_t*)bcpl_alloc_chars(1);
            if (char_payload) {
                char_payload[0] = *p;
                char_payload[1] = 0;
                void* base_ptr = (uint64_t*)char_payload - 1;
                BCPL_LIST_APPEND_STRING(result_list, (uint32_t*)base_ptr);
            }
        }
        return result_list;
    }

    const uint32_t* start = source_payload;
    const uint32_t* end = source_payload;

    while (*end != 0) {
        if (bcpl_strncmp(end, delimiter_payload, delimiter_len)) {
            size_t token_len = end - start;
            uint32_t* token_payload = (uint32_t*)bcpl_alloc_chars(token_len);
            if (token_payload) {
                memcpy(token_payload, start, token_len * sizeof(uint32_t));
                void* base_ptr = (uint64_t*)token_payload - 1;
                BCPL_LIST_APPEND_STRING(result_list, (uint32_t*)base_ptr);
            }
            start = end + delimiter_len;
            end = start;
        } else {
            end++;
        }
    }

    size_t last_token_len = end - start;
    uint32_t* last_token_payload = (uint32_t*)bcpl_alloc_chars(last_token_len);
    if (last_token_payload) {
        memcpy(last_token_payload, start, last_token_len * sizeof(uint32_t));
        void* base_ptr = (uint64_t*)last_token_payload - 1;
        BCPL_LIST_APPEND_STRING(result_list, (uint32_t*)base_ptr);
    }

    return result_list;
}