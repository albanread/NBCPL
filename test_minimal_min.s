.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _START
.globl _WRITEN
.globl _SDL2_DRAW_RECT
.globl _WRITES
.globl _BCPL_LIST_APPEND_STRING
.globl _WRITEF
.globl _BCPL_LIST_GET_NTH
.globl _RDCH
.globl _BCPL_ALLOC_WORDS
.globl _FILE_READ
.globl _WRITEC
.globl _HEAPMANAGER_ISSAMMENABLED
.globl _SPLIT
.globl _BCPL_LIST_CREATE_EMPTY
.globl _GET_FREE_LIST_HEAD_ADDR
.globl _BCPL_CHECK_AND_DISPLAY_ERRORS
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
.globl _FEXP
.globl _FPND
.globl _WRITEF4
.globl _HEAPMANAGER_ENTER_SCOPE
.globl _WRITEF2
.globl _BCPL_CLEAR_ERRORS
.globl _FILTER
.globl _FLOG
.globl _GETVEC
.globl _FGETVEC
.globl _LPND
.globl _BCPL_LIST_APPEND_INT
.globl _RETURNNODETOFREELIST
.globl _FCOS
.globl _APND
.globl _MALLOC
.globl _JOIN
.globl _BCPL_FREE_CELLS
.globl _DEEPCOPYLITERALLIST
.globl _STRCOPY
.globl _WRITEF7
.globl _SDL2_DESTROY_WINDOW
.globl _SDL2_SET_WINDOW_TITLE
.globl _PACKSTRING
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
.globl _SDL2_CLEAR
.globl _SDL2_DESTROY_RENDERER
.globl _BCPL_LIST_GET_REST
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
.globl _FABS
.globl _FILE_OPEN_READ
.globl _SDL2_GET_CURRENT_VIDEO_DRIVER
.globl _SDL2_INIT_SUBSYSTEMS
.globl _BCPL_FREE_LIST_SAFE
.globl _DEEPCOPYLIST
.globl _BCPL_FREE_LIST
.globl _FILE_CLOSE
.globl _FIX
.globl _WRITEF1
.globl _FILE_WRITE
.globl _COPYLIST
.globl _SDL2_GET_DISPLAY_MODES
.globl _BCPL_GET_LAST_ERROR
.globl _SDL2_PRESENT
.globl _SPND
.globl _SDL2_CREATE_RENDERER_EX
.globl _SDL2_GET_EVENT_MOUSE
.globl _SDL2_SET_DRAW_COLOR
.globl _WRITEF6
.globl _HEAPMANAGER_SETSAMMENABLED
.globl _FILE_OPEN_WRITE
.globl _SDL2_GET_TICKS
.globl _SDL2_INIT
.globl _SDL2_FILL_RECT
.globl _FWRITE
.globl _CONCAT
.globl _SDL2_DRAW_LINE
.globl _SDL2_DRAW_POINT
.globl _HEAPMANAGER_EXIT_SCOPE
.globl _SDL2_GET_EVENT_BUTTON
.globl _SETTYPE
.globl _FILE_SEEK
.globl _SDL2_GET_VERSION
.globl _SDL2_CREATE_RENDERER
.globl _SDL2_CLEAR_ERROR
.globl _SDL2_SET_WINDOW_SIZE
.globl _HEAPMANAGER_WAITFORSAMM
.globl _RND
.globl _STRLEN
.globl _SDL2_POLL_EVENT
.globl _FINISH
.globl _SDL2_GET_VIDEO_DRIVERS
.globl _FIND
.globl _SDL2_GET_ERROR
.globl ___SDL2_STATIC_BUILD
.p2align 2
_start:
_START:
    B L_START
    
; --- Veneer Section ---
    ; --- End Veneer Section ---

L_START:
    STP X29, X30, [SP, #-112]!
    MOV X29, SP
    STP x19, x20, [x29, #32]
    STP x21, x22, [x29, #48]
    STP x24, x25, [x29, #64]
    STP x26, x27, [x29, #80]
    STR X28, [X29, #96] ; Saved Reg: X28 @ FP+96
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
L_START_Entry_0:
    MOVZ X9, #1
    MOV X20, X9
    LSL X21, X9, #1
    MOV X0, X21
    BL _GETVEC
    MOV X21, X0
    SUB X9, X21, #8
    STR X20, [X9, #0]
    MOV X24, X21
    MOVZ X9, #1
    MOV X20, X9
    LSL X21, X9, #1
    MOV X0, X21
    BL _GETVEC
    MOV X21, X0
    SUB X9, X21, #8
    STR X20, [X9, #0]
    MOV X15, X21
    ADRP X9, L_pair0@PAGE
    ADD X9, X9, L_pair0@PAGEOFF
    LDR X9, [X9, #0]
    MOVZ X10, #0
    LSL X11, X10, #3
    ADD X12, X24, X11
    STR X9, [X12, #0]
    ADRP X9, L_pair1@PAGE
    ADD X9, X9, L_pair1@PAGEOFF
    LDR X9, [X9, #0]
    MOVZ X10, #0
    LSL X11, X10, #3
    ADD X12, X15, X11
    STR X9, [X12, #0]
    SUB X10, X24, #8
    LDR X9, [X10, #0] ; Load vector/table/string length
    MOV X25, X9
    MOV X9, X25
    ADD X9, X9, #1
    MOVZ X10, #2
    SDIV X9, X9, X10
    MOV X27, X9
    MOV X9, X25
    MOVZ X10, #2
    MUL X9, X9, X10
    MOV X20, X9
    MOV X0, X20
    BL _GETVEC
    MOV X24, X0
    MOVZ X10, #0
    MOV X26, X10
    B L_START_ReductionHeader_1
L_START_Exit_5:
    B L_0
L_START_ReductionBody_2:
    MOV X10, X26
    MOV X26, X10
    B L_START_ReductionIncrement_3
L_START_ReductionExit_4:
    ADRP X10, L_str0@PAGE
    ADD X10, X10, L_str0@PAGEOFF
    ADD X10, X10, #8
    MOV X0, X10
    BL _WRITEF
    B L_START_Exit_5
L_START_ReductionHeader_1:
    CMP x26, x27
    CSET X10, LT
    CMP X10, XZR
    B.EQ L_START_ReductionExit_4
    B L_START_ReductionBody_2
L_START_ReductionIncrement_3:
    MOV X9, X26
    ADD X9, X9, #1
    MOV X26, X9
    B L_START_ReductionHeader_1
L_0:
    LDP x19, x20, [x29, #32]
    LDP x21, x22, [x29, #48]
    LDP x24, x25, [x29, #64]
    LDP x26, x27, [x29, #80]
    LDR X28, [X29, #96] ; Restored Reg: X28 @ FP+96
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [SP, #0]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L___veneer_:
    movz x16, #22416
    movk x16, #1110, lsl #16
    movk x16, #1, lsl #32
    movk x16, #0, lsl #48
    blr x16

.section __DATA,__const
.p2align 3
L_str0:
    .quad 0x18
    ; (upper half)
    .long 0x4d
    .long 0x49
    .long 0x4e
    .long 0x20
    .long 0x6f
    .long 0x70
    .long 0x65
    .long 0x72
    .long 0x61
    .long 0x74
    .long 0x69
    .long 0x6f
    .long 0x6e
    .long 0x20
    .long 0x63
    .long 0x6f
    .long 0x6d
    .long 0x70
    .long 0x6c
    .long 0x65
    .long 0x74
    .long 0x65
    .long 0x64
    .long 0xa
    .long 0x0
    .long 0x0
L_pair0:
    .quad 0x200000001
    ; (upper half)
L_pair1:
    .quad 0x400000003
    ; (upper half)

.section __DATA,__data
.p2align 3
L__data_segment_base:
