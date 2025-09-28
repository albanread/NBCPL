#ifndef COMPILER_INTERFACE_H
#define COMPILER_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

// Error handling interface
void BCPL_SET_ERROR(int code, const char* message, const char* context);

// Freelist management interface
void initialize_freelist(void);
typedef struct ListAtom ListAtom;
ListAtom* getNodeFromFreelist(void);
void returnNodeToFreelist(ListAtom* node);

// Freelist metrics
void printFreelistMetrics(void);

// Add additional runtime-exposed functions here as needed
// Example:
// int some_runtime_function(int arg);

#ifdef __cplusplus
}
#endif

#endif // COMPILER_INTERFACE_H