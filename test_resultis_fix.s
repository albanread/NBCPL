.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _START
.globl _WRITEN
.globl _WRITES
.globl _BCPL_LIST_APPEND_STRING
.globl _WRITEF
.globl _BCPL_LIST_GET_NTH
.globl _FILE_OPEN_READ
.globl _BCPL_CHECK_AND_DISPLAY_ERRORS
.globl _RDCH
.globl _BCPL_ALLOC_WORDS
.globl _FILE_READ
.globl _WRITEC
.globl _HEAPMANAGER_ISSAMMENABLED
.globl _SPLIT
.globl _BCPL_LIST_CREATE_EMPTY
.globl _GET_FREE_LIST_HEAD_ADDR
.globl _UNPACKSTRING
.globl _SDL2_DELAY
.globl _FREEVEC
.globl _BCPL_LIST_APPEND_FLOAT
.globl _BCPL_CONCAT_LISTS
.globl _SDL2_GET_EVENT_KEY
.globl _RUNTIME_METHOD_LOOKUP
.globl _BCPL_LIST_GET_HEAD_AS_INT
.globl _NEWLINE
.globl _BCPL_LIST_GET_HEAD_AS_FLOAT
.globl _OBJECT_HEAP_FREE
.globl _FPND
.globl _FEXP
.globl _WRITEF4
.globl _HEAPMANAGER_ENTER_SCOPE
.globl _FCOS
.globl _APND
.globl _WRITEF2
.globl _BCPL_CLEAR_ERRORS
.globl _FILTER
.globl _FLOG
.globl _GETVEC
.globl _LPND
.globl _BCPL_LIST_APPEND_INT
.globl _RETURNNODETOFREELIST
.globl _MALLOC
.globl _JOIN
.globl _BCPL_FREE_CELLS
.globl _STRCOPY
.globl _PACKSTRING
.globl _BCPL_FREE_LIST_SAFE
.globl _DEEPCOPYLIST
.globl _BCPL_FREE_LIST
.globl _FILE_CLOSE
.globl _FIND
.globl _WRITEF3
.globl _BCPL_GET_ATOM_TYPE
.globl _FILE_READS
.globl _FTAN
.globl _FRND
.globl _SPIT
.globl _SDL2_QUIT
.globl _FILE_WRITES
.globl _PIC_RUNTIME_HELPER
.globl _SDL2_CREATE_WINDOW
.globl _SLURP
.globl _RAND
.globl _WRITEF5
.globl _BCPL_ALLOC_CHARS
.globl _OBJECT_HEAP_ALLOC
.globl _SDL2_TEST_BASIC
.globl _BCPL_LIST_GET_TAIL
.globl _REVERSE
.globl _BCPL_BOUNDS_ERROR
.globl _FILE_EOF
.globl _STRCMP
.globl _FILE_TELL
.globl _FILE_OPEN_APPEND
.globl _SDL2_CREATE_WINDOW_EX
.globl _FSIN
.globl _SDL2_GET_CURRENT_VIDEO_DRIVER
.globl _SDL2_INIT_SUBSYSTEMS
.globl _SDL2_GET_VIDEO_DRIVERS
.globl _FINISH
.globl _SDL2_GET_EVENT_MOUSE
.globl _SDL2_SET_WINDOW_TITLE
.globl _WRITEF7
.globl _SDL2_DESTROY_WINDOW
.globl _FABS
.globl _DEEPCOPYLITERALLIST
.globl _FIX
.globl _WRITEF1
.globl _FILE_WRITE
.globl _COPYLIST
.globl _SDL2_DRAW_RECT
.globl _SDL2_GET_DISPLAY_MODES
.globl _BCPL_GET_LAST_ERROR
.globl _SDL2_PRESENT
.globl _SPND
.globl _SDL2_CREATE_RENDERER_EX
.globl _BCPL_LIST_GET_REST
.globl _SDL2_CLEAR
.globl _SDL2_DESTROY_RENDERER
.globl _SDL2_SET_DRAW_COLOR
.globl _SDL2_INIT
.globl _SDL2_FILL_RECT
.globl _FWRITE
.globl _CONCAT
.globl _SDL2_DRAW_LINE
.globl _SDL2_CREATE_RENDERER
.globl _SDL2_CLEAR_ERROR
.globl _SDL2_SET_WINDOW_SIZE
.globl _SETTYPE
.globl _FILE_SEEK
.globl _SDL2_GET_VERSION
.globl _SDL2_DRAW_POINT
.globl _HEAPMANAGER_EXIT_SCOPE
.globl _SDL2_GET_EVENT_BUTTON
.globl _WRITEF6
.globl _HEAPMANAGER_SETSAMMENABLED
.globl _FILE_OPEN_WRITE
.globl _SDL2_GET_TICKS
.globl ___SDL2_STATIC_BUILD
.globl _HEAPMANAGER_WAITFORSAMM
.globl _RND
.globl _STRLEN
.globl _SDL2_POLL_EVENT
.globl _SDL2_GET_ERROR
.p2align 2
_start:
_START:
    B L_START
    
; --- Veneer Section ---
    ; --- End Veneer Section ---

L_FACTORIAL_RECURSIVE:
    STP X29, X30, [SP, #-48]!
    MOV X29, SP
    STP x19, x27, [x29, #24]
    STR X28, [X29, #40] ; Saved Reg: X28 @ FP+40
    MOV X27, X0    // Move parameter 'N' from X0 to X27
L_FACTORIAL_RECURSIVE_Entry_0:
    CMP x27, #1
    CSET X10, LE
    CMP X10, XZR
    B.EQ L_FACTORIAL_RECURSIVE_Join_2
    B L_FACTORIAL_RECURSIVE_Then_1
L_FACTORIAL_RECURSIVE_Exit_3:
    B L_0
L_FACTORIAL_RECURSIVE_Join_2:
    MOV X20, X27
    SUB x21, x27, #1
    MOV X0, X21
    BL L_FACTORIAL_RECURSIVE
    MUL X20, X20, X0
    MOV X0, X20
    B L_0
    B L_FACTORIAL_RECURSIVE_Exit_3
L_FACTORIAL_RECURSIVE_Then_1:
    MOVZ X10, #1
    MOV X0, X10
    B L_0
    B L_FACTORIAL_RECURSIVE_Exit_3
L_0:
    LDP x19, x27, [x29, #24]
    LDR X28, [X29, #40] ; Restored Reg: X28 @ FP+40
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [x31]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_START:
    STP X29, X30, [SP, #-128]!
    MOV X29, SP
    STP x19, x20, [x29, #48]
    STP x21, x22, [x29, #64]
    STP x24, x25, [x29, #80]
    STP x26, x27, [x29, #96]
    STR X28, [X29, #112] ; Saved Reg: X28 @ FP+112
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
L_START_Entry_0:
    ADRP X10, L_str0@PAGE
    ADD X10, X10, L_str0@PAGEOFF
    ADD X10, X10, #8
    MOV X0, X10
    BL _WRITES
    MOVZ x20, #3
    MOV X0, X20
    BL L_FACTORIAL_RECURSIVE
    MOV X24, X0
    MOV X0, X24
    BL _WRITEN
    ADRP X9, L_str1@PAGE
    ADD X9, X9, L_str1@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    ADRP X9, L_str2@PAGE
    ADD X9, X9, L_str2@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    MOVZ x20, #2
    MOV X0, X20
    BL L_FACTORIAL_RECURSIVE
    MOV X25, X0
    MOV X0, X25
    BL _WRITEN
    ADRP X9, L_str1@PAGE
    ADD X9, X9, L_str1@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    ADRP X9, L_str3@PAGE
    ADD X9, X9, L_str3@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    MOVZ x20, #1
    MOV X0, X20
    BL L_FACTORIAL_RECURSIVE
    MOV X26, X0
    MOV X0, X26
    BL _WRITEN
    ADRP X9, L_str1@PAGE
    ADD X9, X9, L_str1@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    ADRP X9, L_str4@PAGE
    ADD X9, X9, L_str4@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    MOVZ x20, #0
    MOV X0, X20
    BL L_FACTORIAL_RECURSIVE
    MOV X27, X0
    MOV X0, X27
    BL _WRITEN
    ADRP X9, L_str1@PAGE
    ADD X9, X9, L_str1@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    B L_START_ResultisCleanup_1
L_START_Exit_2:
    B L_1
L_START_ResultisCleanup_1:
    MOV X0, X24
    B L_1
    B L_START_Exit_2
L_1:
    LDP x19, x20, [x29, #48]
    LDP x21, x22, [x29, #64]
    LDP x24, x25, [x29, #80]
    LDP x26, x27, [x29, #96]
    LDR X28, [X29, #112] ; Restored Reg: X28 @ FP+112
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [x31]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET

.section __DATA,__const
.p2align 3
L_str0:
    .quad 0x18
    ; (upper half)
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x66
    .long 0x61
    .long 0x63
    .long 0x74
    .long 0x6f
    .long 0x72
    .long 0x69
    .long 0x61
    .long 0x6c
    .long 0x20
    .long 0x6f
    .long 0x66
    .long 0x20
    .long 0x33
    .long 0x3a
    .long 0x20
    .long 0x0
    .long 0x0
L_str1:
    .quad 0x2
    ; (upper half)
    .long 0x2a
    .long 0x4e
    .long 0x0
    .long 0x0
L_str2:
    .quad 0x18
    ; (upper half)
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x66
    .long 0x61
    .long 0x63
    .long 0x74
    .long 0x6f
    .long 0x72
    .long 0x69
    .long 0x61
    .long 0x6c
    .long 0x20
    .long 0x6f
    .long 0x66
    .long 0x20
    .long 0x32
    .long 0x3a
    .long 0x20
    .long 0x0
    .long 0x0
L_str3:
    .quad 0x18
    ; (upper half)
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x66
    .long 0x61
    .long 0x63
    .long 0x74
    .long 0x6f
    .long 0x72
    .long 0x69
    .long 0x61
    .long 0x6c
    .long 0x20
    .long 0x6f
    .long 0x66
    .long 0x20
    .long 0x31
    .long 0x3a
    .long 0x20
    .long 0x0
    .long 0x0
L_str4:
    .quad 0x18
    ; (upper half)
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x66
    .long 0x61
    .long 0x63
    .long 0x74
    .long 0x6f
    .long 0x72
    .long 0x69
    .long 0x61
    .long 0x6c
    .long 0x20
    .long 0x6f
    .long 0x66
    .long 0x20
    .long 0x30
    .long 0x3a
    .long 0x20
    .long 0x0
    .long 0x0

.section __DATA,__data
.p2align 3
    .long 0x0
    .long 0x0
    .long 0x0
    .long 0x0
    .long 0x0
L__data_segment_base:
