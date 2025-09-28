/*
 * runtime_freelist.c
 * Canonical freelist implementation for BCPL runtime.
 * Provides thread-safe allocation and deallocation of ListAtom and ListHeader nodes,
 * tracks allocated chunks for cleanup, and exposes API for use in both standalone and JIT builds.
 */

#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include "BCPLError.h"

// Forward declaration for error tracking
extern void _BCPL_SET_ERROR(BCPLErrorCode code, const char* func, const char* msg);

// --- Types ---
typedef struct ListAtom {
    int type;
    int pad;
    union {
        int64_t int_value;
        double float_value;
        void* ptr_value;
    } value;
    struct ListAtom* next;
} ListAtom;

typedef struct ListHeader {
    int type;
    struct ListAtom* head;
    struct ListAtom* tail;
    size_t length;
} ListHeader;

// --- Freelist globals ---
static ListAtom* g_free_list_head = NULL;
static ListHeader* g_header_free_list_head = NULL;

// --- Freelist metrics ---
static size_t g_freelist_node_count = 0;
static size_t g_total_nodes_allocated_from_heap = 0;
static size_t g_nodes_reused_from_freelist = 0;
static size_t g_total_node_requests = 0;

// --- Constants ---
#define INITIAL_FREELIST_CHUNK_SIZE 1024
#define HEADER_FREELIST_CHUNK_SIZE 128
#define MAX_FREELIST_CHUNK_SIZE 131072

// --- Adaptive scaling globals ---
static size_t current_atom_chunk_size = INITIAL_FREELIST_CHUNK_SIZE;
static size_t current_header_chunk_size = HEADER_FREELIST_CHUNK_SIZE;
static time_t last_atom_replenish_time = 0;
static time_t last_header_replenish_time = 0;
static int replenish_frequency_counter = 0;

// --- Scaling metrics ---
static size_t total_atom_replenishments = 0;
static size_t total_header_replenishments = 0;
static size_t atom_scaling_events = 0;
static size_t header_scaling_events = 0;
static size_t max_atom_chunk_size_reached = INITIAL_FREELIST_CHUNK_SIZE;
static size_t max_header_chunk_size_reached = HEADER_FREELIST_CHUNK_SIZE;

// --- Thread safety ---
static pthread_mutex_t freelist_mutex = PTHREAD_MUTEX_INITIALIZER;
static int freelist_initialized = 0;
void initialize_freelist(void);

// --- Chunk tracking ---
typedef struct ChunkNode {
    void* chunk;
    struct ChunkNode* next;
} ChunkNode;

static ChunkNode* atom_chunk_list_head = NULL;
static ChunkNode* header_chunk_list_head = NULL;

// --- Internal helpers for chunk tracking ---
static void track_atom_chunk(void* chunk) {
    ChunkNode* node = (ChunkNode*)malloc(sizeof(ChunkNode));
    if (!node) {
        _BCPL_SET_ERROR(ERROR_OUT_OF_MEMORY, "track_atom_chunk", "Failed to allocate chunk tracking node");
        return; // Continue without tracking - not ideal but better than crashing
    }
    node->chunk = chunk;
    node->next = atom_chunk_list_head;
    atom_chunk_list_head = node;
}

static void track_header_chunk(void* chunk) {
    ChunkNode* node = (ChunkNode*)malloc(sizeof(ChunkNode));
    if (!node) {
        _BCPL_SET_ERROR(ERROR_OUT_OF_MEMORY, "track_header_chunk", "Failed to allocate header chunk tracking node");
        return; // Continue without tracking - not ideal but better than crashing
    }
    node->chunk = chunk;
    node->next = header_chunk_list_head;
    header_chunk_list_head = node;
}

// --- Freelist replenishment ---
static void replenishFreelist() {
    // Adaptive scaling: if we're replenishing frequently, scale up chunk size
    time_t now = time(NULL);
    size_t old_chunk_size = current_atom_chunk_size;
    
    if (now - last_atom_replenish_time <= 2) {  // If replenishing within 2 seconds
        replenish_frequency_counter++;
        if (replenish_frequency_counter >= 3) {  // After 3 rapid replenishments
            current_atom_chunk_size *= 4;  // Quadruple chunk size for faster scaling
            if (current_atom_chunk_size > MAX_FREELIST_CHUNK_SIZE) {
                current_atom_chunk_size = MAX_FREELIST_CHUNK_SIZE;  // Cap it
            }
            replenish_frequency_counter = 0;  // Reset counter
            
            // Track scaling event
            if (current_atom_chunk_size > old_chunk_size) {
                atom_scaling_events++;
                if (current_atom_chunk_size > max_atom_chunk_size_reached) {
                    max_atom_chunk_size_reached = current_atom_chunk_size;
                }
            }
        }
    } else {
        replenish_frequency_counter = 0;  // Reset if we haven't replenished recently
    }
    last_atom_replenish_time = now;
    total_atom_replenishments++;
    
    size_t nodes_to_alloc = current_atom_chunk_size;
    
    // Debug output for adaptive scaling
    if (nodes_to_alloc > INITIAL_FREELIST_CHUNK_SIZE) {
        printf("FREELIST: Scaled up atom chunk size to %zu (pressure detected)\n", nodes_to_alloc);
    }
    
    ListAtom* new_chunk = (ListAtom*)malloc(sizeof(ListAtom) * nodes_to_alloc);
    if (!new_chunk) {
        _BCPL_SET_ERROR(ERROR_OUT_OF_MEMORY, "replenishFreelist", "Failed to allocate freelist chunk");
        exit(1); // Still exit, but log the error first
    }
    track_atom_chunk(new_chunk);
    g_total_nodes_allocated_from_heap += nodes_to_alloc;

    // Link all the new nodes together in a chain.
    for (size_t i = 0; i < nodes_to_alloc - 1; ++i) {
        new_chunk[i].next = &new_chunk[i + 1];
    }
    new_chunk[nodes_to_alloc - 1].next = NULL;

    // Prepend this new chunk to the (potentially empty) existing freelist.
    new_chunk[nodes_to_alloc - 1].next = g_free_list_head;
    g_free_list_head = new_chunk;
    g_freelist_node_count += nodes_to_alloc;
}

static void replenishHeaderFreelist() {
    // Adaptive scaling for headers too
    time_t now = time(NULL);
    size_t old_chunk_size = current_header_chunk_size;
    
    if (now - last_header_replenish_time <= 2) {  // If replenishing within 2 seconds
        current_header_chunk_size *= 4;  // Quadruple chunk size for faster scaling
        if (current_header_chunk_size > (MAX_FREELIST_CHUNK_SIZE / 8)) {  // Headers don't need as much scaling
            current_header_chunk_size = (MAX_FREELIST_CHUNK_SIZE / 8);
        }
        
        // Track scaling event
        if (current_header_chunk_size > old_chunk_size) {
            header_scaling_events++;
            if (current_header_chunk_size > max_header_chunk_size_reached) {
                max_header_chunk_size_reached = current_header_chunk_size;
            }
        }
    }
    last_header_replenish_time = now;
    total_header_replenishments++;
    
    size_t headers_to_alloc = current_header_chunk_size;
    
    // Debug output for adaptive scaling
    if (headers_to_alloc > HEADER_FREELIST_CHUNK_SIZE) {
        printf("FREELIST: Scaled up header chunk size to %zu (pressure detected)\n", headers_to_alloc);
    }
    
    ListHeader* new_chunk = (ListHeader*)malloc(sizeof(ListHeader) * headers_to_alloc);
    if (!new_chunk) {
        _BCPL_SET_ERROR(ERROR_OUT_OF_MEMORY, "replenishHeaderFreelist", "Failed to allocate header freelist chunk");
        exit(1); // Still exit, but log the error first
    }
    track_header_chunk(new_chunk);

    // Link the new headers together in a chain using the 'head' pointer for the link.
    for (size_t i = 0; i < headers_to_alloc - 1; ++i) {
        new_chunk[i].head = (ListAtom*)&new_chunk[i + 1];
    }
    new_chunk[headers_to_alloc - 1].head = (ListAtom*)g_header_free_list_head;
    g_header_free_list_head = new_chunk;
}

// --- API: Get/Return ListAtom nodes ---
#ifdef __cplusplus
extern "C" {
#endif

ListAtom* getNodeFromFreelist() {
   if (!freelist_initialized) {
       initialize_freelist();
   }
   pthread_mutex_lock(&freelist_mutex);
   g_total_node_requests++;
   if (g_free_list_head == NULL) {
       replenishFreelist();
   } else {
       g_nodes_reused_from_freelist++;
   }
   ListAtom* node = g_free_list_head;
   g_free_list_head = g_free_list_head->next;
   g_freelist_node_count--;
   pthread_mutex_unlock(&freelist_mutex);
   
   // SAMM: Individual ListAtoms are not tracked in scope
   // Only ListHeaders are tracked since they own the entire list
   
   return node;
}

void returnNodeToFreelist(ListAtom* node) {
    if (!node) return;
    pthread_mutex_lock(&freelist_mutex);
    node->next = g_free_list_head;
    g_free_list_head = node;
    g_freelist_node_count++;
    pthread_mutex_unlock(&freelist_mutex);
}

// --- API: Get/Return ListHeader nodes ---
ListHeader* getHeaderFromFreelist() {
   if (!freelist_initialized) {
       initialize_freelist();
   }
   pthread_mutex_lock(&freelist_mutex);
   if (g_header_free_list_head == NULL) {
       replenishHeaderFreelist();
   }
   ListHeader* header = g_header_free_list_head;
   g_header_free_list_head = (ListHeader*)g_header_free_list_head->head;
   pthread_mutex_unlock(&freelist_mutex);
   
   // SAMM: Track freelist allocation in current scope if enabled
   extern int HeapManager_isSAMMEnabled(void);
   extern void HeapManager_trackFreelistAllocation(void* ptr);
   if (HeapManager_isSAMMEnabled() && header != NULL) {
       HeapManager_trackFreelistAllocation(header);
   }
   
   return header;
}

void returnHeaderToFreelist(ListHeader* header) {
    if (!header) return;
    pthread_mutex_lock(&freelist_mutex);
    header->head = (ListAtom*)g_header_free_list_head;
    g_header_free_list_head = header;
    pthread_mutex_unlock(&freelist_mutex);
}

// --- API: Freelist cleanup ---
void cleanup_freelists() {
   pthread_mutex_lock(&freelist_mutex);

   // Free all atom chunks
   ChunkNode* node = atom_chunk_list_head;
   while (node) {
       free(node->chunk);
       ChunkNode* next = node->next;
       free(node);
       node = next;
   }
   atom_chunk_list_head = NULL;

   // Free all header chunks
   node = header_chunk_list_head;
   while (node) {
       free(node->chunk);
       ChunkNode* next = node->next;
       free(node);
       node = next;
   }
   header_chunk_list_head = NULL;

   // Reset freelist pointers
   g_free_list_head = NULL;
   g_header_free_list_head = NULL;
   g_freelist_node_count = 0;
   g_total_nodes_allocated_from_heap = 0;
   g_nodes_reused_from_freelist = 0;
   g_total_node_requests = 0;
   freelist_initialized = 0;

   pthread_mutex_unlock(&freelist_mutex);
}

// --- Metrics API ---
void printFreelistMetrics() {
    printf("\n=== Freelist Adaptive Scaling Metrics ===\n");
    printf("Atom replenishments: %zu\n", total_atom_replenishments);
    printf("Header replenishments: %zu\n", total_header_replenishments);
    printf("Atom scaling events: %zu\n", atom_scaling_events);
    printf("Header scaling events: %zu\n", header_scaling_events);
    printf("Current atom chunk size: %zu (started at %d)\n", current_atom_chunk_size, INITIAL_FREELIST_CHUNK_SIZE);
    printf("Current header chunk size: %zu (started at %d)\n", current_header_chunk_size, HEADER_FREELIST_CHUNK_SIZE);
    printf("Max atom chunk size reached: %zu\n", max_atom_chunk_size_reached);
    printf("Max header chunk size reached: %zu\n", max_header_chunk_size_reached);
    printf("Total nodes allocated from heap: %zu\n", g_total_nodes_allocated_from_heap);
    printf("Total node requests: %zu\n", g_total_node_requests);
    printf("Nodes reused from freelist: %zu\n", g_nodes_reused_from_freelist);
    printf("Freelist reuse rate: %.2f%%\n", g_total_node_requests > 0 ? (double)g_nodes_reused_from_freelist / g_total_node_requests * 100.0 : 0.0);
    printf("Current freelist node count: %zu\n", g_freelist_node_count);
    printf("==========================================\n");
}

#ifdef __cplusplus
}
#endif

// --- Public: BCPL_RECYCLE ---
void BCPL_RECYCLE(void) {
   cleanup_freelists();
}

// --- Initialization helper ---
#ifdef __cplusplus
extern "C" {
#endif

void initialize_freelist(void) {
   pthread_mutex_lock(&freelist_mutex);
   if (!freelist_initialized) {
       replenishFreelist();
       replenishHeaderFreelist();
       freelist_initialized = 1;
   }
   pthread_mutex_unlock(&freelist_mutex);
}

#ifdef __cplusplus
}
#endif

// --- Optional: Metrics accessors ---
size_t freelist_node_count() {
    pthread_mutex_lock(&freelist_mutex);
    size_t count = g_freelist_node_count;
    pthread_mutex_unlock(&freelist_mutex);
    return count;
}

size_t total_nodes_allocated() {
    pthread_mutex_lock(&freelist_mutex);
    size_t total = g_total_nodes_allocated_from_heap;
    pthread_mutex_unlock(&freelist_mutex);
    return total;
}