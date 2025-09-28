// Standard BCPL Library Header
// This file contains standard declarations for the BCPL compiler

GLOBAL {
    // System I/O functions
    FWRITE: 1
    WRITES: 2
    WRITED: 3
    WRITEN: 4
    WRITEF: 5

    // Math and utility functions
    ABS:   10
    LEVEL: 11

    // String and vector handling
    VEC:   20
    VECAP: 21
    TABLE: 22
    
    // System control
    FINISH: 30
    RESULT: 31
}

MANIFEST {
    TRUE = 1
    FALSE = 0
    ENDSTREAMCH = -1
}

// Include utility functions
GET "utils.h"