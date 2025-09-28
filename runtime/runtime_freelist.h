#pragma once

#include <stddef.h>
#include <stdint.h>

#include "ListDataTypes.h"

// Freelist API

#ifdef __cplusplus
extern "C" {
#endif

// Recycle and cleanup all freelist memory (call at shutdown)
void BCPL_RECYCLE(void);

// Initialize the freelist (called internally)
void initialize_freelist(void);

// Allocate a ListAtom from the freelist
ListAtom* getNodeFromFreelist(void);

// Return a ListAtom to the freelist
void returnNodeToFreelist(ListAtom* node);

// Allocate a ListHeader from the freelist
ListHeader* getHeaderFromFreelist(void);

// Return a ListHeader to the freelist
void returnHeaderToFreelist(ListHeader* header);

// Cleanup all freelist memory (call at shutdown)
void cleanup_freelists(void);

#ifdef __cplusplus
}
#endif