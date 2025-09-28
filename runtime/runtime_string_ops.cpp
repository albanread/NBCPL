/**
 * Unicode-safe implementation for BCPL_SPLIT_STRING and BCPL_JOIN_LIST.
 * All string logic operates on uint32_t* arrays representing Unicode codepoints.
 * No ASCII or std::string assumptions.
 */

#include "runtime.h"
#include "ListDataTypes.h"
#include "heap_interface.h"
#include <cstring>
#include <cstdint>

/**
 * @brief Calculate the length of a BCPL string payload (number of codepoints).
 */
static size_t bcpl_strlen(const uint32_t* s) {
    size_t len = 0;
    while (s && s[len] != 0) ++len;
    return len;
}

/**
 * @brief Compare two codepoint arrays for delimiter matching.
 */
static bool bcpl_codepoint_match(const uint32_t* s, const uint32_t* delimiter, size_t delimiter_len) {
    for (size_t i = 0; i < delimiter_len; ++i) {
        if (s[i] != delimiter[i]) return false;
        if (s[i] == 0) return false;
    }
    return true;
}

/**
 * @brief Split a BCPL string by a delimiter into a list of new BCPL strings.
 * This implementation is fully Unicode-safe.
 */
extern "C" ListHeader* BCPL_SPLIT_STRING(uint32_t* source_payload, uint32_t* delimiter_payload) {
    ListHeader* result_list = BCPL_LIST_CREATE_EMPTY();
    if (!source_payload || !delimiter_payload) return result_list;

    size_t delimiter_len = bcpl_strlen(delimiter_payload);

    // Edge case: empty delimiter splits into single codepoints
    if (delimiter_len == 0) {
        for (const uint32_t* p = source_payload; *p != 0; ++p) {
            uint32_t* char_payload = (uint32_t*)bcpl_alloc_chars(1);
            char_payload[0] = *p;
            char_payload[1] = 0;
            void* base_ptr = (uint64_t*)char_payload - 1;
            BCPL_LIST_APPEND_STRING(result_list, (uint32_t*)base_ptr);
        }
        return result_list;
    }

    const uint32_t* start = source_payload;
    const uint32_t* end = source_payload;

    while (*end != 0) {
        if (bcpl_codepoint_match(end, delimiter_payload, delimiter_len)) {
            size_t token_len = end - start;
            uint32_t* token_payload = (uint32_t*)bcpl_alloc_chars(token_len);
            if (token_len > 0) {
                memcpy(token_payload, start, token_len * sizeof(uint32_t));
            }
            token_payload[token_len] = 0;
            void* base_ptr = (uint64_t*)token_payload - 1;
            BCPL_LIST_APPEND_STRING(result_list, (uint32_t*)base_ptr);

            start = end + delimiter_len;
            end = start;
        } else {
            ++end;
        }
    }

    // Add final token
    size_t last_token_len = end - start;
    uint32_t* last_token_payload = (uint32_t*)bcpl_alloc_chars(last_token_len);
    if (last_token_len > 0) {
        memcpy(last_token_payload, start, last_token_len * sizeof(uint32_t));
    }
    last_token_payload[last_token_len] = 0;
    void* base_ptr = (uint64_t*)last_token_payload - 1;
    BCPL_LIST_APPEND_STRING(result_list, (uint32_t*)base_ptr);

    return result_list;
}

/**
 * @brief Join a list of BCPL strings into a single string using a delimiter.
 * This implementation is fully Unicode-safe.
 */
extern "C" uint32_t* BCPL_JOIN_LIST(ListHeader* list_header, uint32_t* delimiter_payload) {
    if (!list_header || !list_header->head) return (uint32_t*)bcpl_alloc_chars(0);

    size_t delimiter_len = bcpl_strlen(delimiter_payload);

    // Pass 1: Calculate total length
    size_t total_len = 0, element_count = 0;
    ListAtom* current = list_header->head;
    while (current) {
        if (current->type == ATOM_STRING && current->value.ptr_value) {
            uint64_t* base_ptr = (uint64_t*)current->value.ptr_value;
            total_len += base_ptr[0];
            ++element_count;
        }
        current = current->next;
    }
    if (element_count > 1) total_len += delimiter_len * (element_count - 1);

    // Pass 2: Allocate and build result
    uint32_t* result_payload = (uint32_t*)bcpl_alloc_chars(total_len);
    uint32_t* cursor = result_payload;
    current = list_header->head;
    size_t i = 0;
    while (current) {
        if (current->type == ATOM_STRING && current->value.ptr_value) {
            uint64_t* base_ptr = (uint64_t*)current->value.ptr_value;
            size_t element_len = base_ptr[0];
            uint32_t* element_payload = (uint32_t*)(base_ptr + 1);
            memcpy(cursor, element_payload, element_len * sizeof(uint32_t));
            cursor += element_len;
            if (i < element_count - 1 && delimiter_len > 0) {
                memcpy(cursor, delimiter_payload, delimiter_len * sizeof(uint32_t));
                cursor += delimiter_len;
            }
            ++i;
        }
        current = current->next;
    }
    result_payload[total_len] = 0;
    return result_payload;
}