.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _START
.globl _WRITEN
.globl _GETVEC
.globl _WRITEF
.globl _NEWLINE
.globl _SDL2_QUIT
.globl _SDL2_INIT
.globl _RDCH
.globl _BCPL_LIST_GET_NTH
.globl _BCPL_ALLOC_WORDS
.globl _SPLIT
.globl _BCPL_CHECK_AND_DISPLAY_ERRORS
.globl _FIND
.globl _SDL2_GET_EVENT_BUTTON
.globl _LPND
.globl _FGETVEC
.globl _SLURP
.globl _DEEPCOPYLIST
.globl _BCPL_FREE_LIST_SAFE
.globl _WRITEC
.globl _TIMER_DISPLAY
.globl _FILE_READ
.globl _UNPACKSTRING
.globl _SDL2_DELAY
.globl _FREEVEC
.globl _BCPL_CONCAT_LISTS
.globl _SDL2_GET_EVENT_KEY
.globl _LIST_CREATE
.globl _LIST_APPEND_INT
.globl _FILTER
.globl _BCPL_CLEAR_ERRORS
.globl _FLOG
.globl _BCPL_BOUNDS_ERROR
.globl _REVERSE
.globl _GET_FREE_LIST_HEAD_ADDR
.globl _SPIT
.globl _FRND
.globl _FTAN
.globl _DEEPCOPYLITERALLIST
.globl _FCOS
.globl _APND
.globl _LIST_HEAD_INT
.globl _STRCOPY
.globl _LIST_APPEND_STRING
.globl _FILE_OPEN_READ
.globl _LIST_APPEND_FLOAT
.globl _JOIN
.globl _BCPL_FREE_CELLS
.globl _MALLOC
.globl _RETURNNODETOFREELIST
.globl _RAND
.globl _SDL2_FILL_RECT
.globl _FWRITE
.globl _FILE_TELL
.globl _WRITEF2
.globl _STRLEN
.globl _RND
.globl _PACKSTRING
.globl _BCPL_LIST_APPEND_LIST
.globl _HEAPMANAGER_ISSAMMENABLED
.globl _FILE_OPEN_APPEND
.globl _SDL2_CREATE_WINDOW_EX
.globl _FSIN
.globl _WRITES
.globl _BCPL_ALLOC_CHARS
.globl _WRITEF5
.globl _FABS
.globl _SDL2_GET_CURRENT_VIDEO_DRIVER
.globl _SDL2_INIT_SUBSYSTEMS
.globl _LIST_FREE
.globl _FILE_READS
.globl _WRITEF3
.globl _BCPL_GET_ATOM_TYPE
.globl _BCPL_FREE_LIST
.globl _FILE_CLOSE
.globl _SDL2_GET_EVENT_MOUSE
.globl _STRCMP
.globl _SDL2_GET_VIDEO_DRIVERS
.globl _FINISH
.globl _LIST_HEAD_FLOAT
.globl _HEAPMANAGER_ENTER_SCOPE
.globl _SDL2_DRAW_POINT
.globl _HEAPMANAGER_EXIT_SCOPE
.globl _FILE_OPEN_WRITE
.globl _SDL2_GET_TICKS
.globl _WRITEF6
.globl _HEAPMANAGER_SETSAMMENABLED
.globl _SETTYPE
.globl _FILE_SEEK
.globl _SDL2_GET_VERSION
.globl _SDL2_TEST_BASIC
.globl _OBJECT_HEAP_ALLOC
.globl _FILE_WRITES
.globl _PIC_RUNTIME_HELPER
.globl _SDL2_DRAW_RECT
.globl _TIMER_CLEAR
.globl _SDL2_CLEAR_ERROR
.globl _SDL2_SET_WINDOW_SIZE
.globl _SDL2_CREATE_RENDERER
.globl _WRITEF4
.globl _FEXP
.globl _FPND
.globl _FIX
.globl _WRITEF1
.globl _FILE_WRITE
.globl _TIMER_GET_CALL_COUNT
.globl _COPYLIST
.globl _SDL2_GET_DISPLAY_MODES
.globl _BCPL_GET_LAST_ERROR
.globl _SDL2_PRESENT
.globl _SPND
.globl _SDL2_CREATE_RENDERER_EX
.globl _BCPL_LIST_GET_REST
.globl _SDL2_DESTROY_RENDERER
.globl _SDL2_CLEAR
.globl _WRITEF7
.globl _SDL2_DESTROY_WINDOW
.globl _SDL2_SET_WINDOW_TITLE
.globl _HEAPMANAGER_WAITFORSAMM
.globl _SDL2_POLL_EVENT
.globl _LIST_TAIL
.globl _SDL2_GET_ERROR
.globl _FILE_EOF
.globl _SDL2_CREATE_WINDOW
.globl _TIMER_GET_TOTAL_NS
.globl _RUNTIME_METHOD_LOOKUP
.globl _TIMER_START
.globl _SDL2_SET_DRAW_COLOR
.globl _CONCAT
.globl _SDL2_DRAW_LINE
.globl _OBJECT_HEAP_FREE
.globl _TIMER_END
.p2align 2
_start:
_START:
    B L_START
    
; --- Veneer Section ---
    ; --- End Veneer Section ---

L_START:
    STP X29, X30, [SP, #-144]!
    MOV X29, SP
    STP x19, x20, [x29, #64]
    STP x21, x22, [x29, #80]
    STP x23, x24, [x29, #96]
    STP x25, x26, [x29, #112]
    STP x27, x28, [x29, #128]
L_START_Entry_0:
    MOVZ X9, #10
    MOV X20, X9
    LSL X20, X9, #1
    MOV X0, X20
    BL _GETVEC
    MOV X20, X0
    SUB X9, X20, #8
    STR X20, [X9, #0]
    MOV X27, X20
    MOVZ X9, #10
    MOV X20, X9
    LSL X20, X9, #1
    MOV X0, X20
    BL _GETVEC
    MOV X20, X0
    SUB X9, X20, #8
    STR X20, [X9, #0]
    MOV X26, X20
    MOVZ X9, #0
    MOV X25, X9
    B L_START_ForHeader_1
L_START_Exit_9:
    B L_0
L_START_ForBody_2:
    ADRP X9, L_pair0@PAGE
    ADD X9, X9, L_pair0@PAGEOFF
    LDR X9, [X9, #0]
    MOV X23, X9
    ADRP X9, L_pair1@PAGE
    ADD X9, X9, L_pair1@PAGEOFF
    LDR X9, [X9, #0]
    MOV X22, X9
    LSL X9, X25, #3
    ADD X10, X27, X9
    STR X23, [X10, #0]
    LSL X9, X25, #3
    ADD X10, X26, X9
    STR X22, [X10, #0]
    B L_START_ForIncrement_3
L_START_ForBody_6:
    SUB X10, X27, #8
    LDR X9, [X10, #0] ; Load vector length for bounds check
    CMP X20, X9
    B.HS L__bounds_error_handler_START
    MOV X9, X20
    LSL X9, X9, #3
    ADD X10, X27, X9
    LDR X9, [X10, #0]
    SUB X11, X26, #8
    LDR X10, [X11, #0] ; Load vector length for bounds check
    CMP X20, X10
    B.HS L__bounds_error_handler_START
    MOV X10, X20
    LSL X10, X10, #3
    ADD X11, X26, X10
    LDR X10, [X11, #0]
    ADD X9, X9, X10
    LSL X10, X20, #3
    ADD X11, X21, X10
    STR X9, [X11, #0]
    B L_START_ForIncrement_7
L_START_ForExit_4:
    SUB X10, X27, #8
    LDR X9, [X10, #0] ; Load vector/table/string length
    MOV X24, X9
    MOV X20, X24
    LSL X20, X24, #1
    MOV X0, X20
    BL _GETVEC
    MOV X20, X0
    SUB X9, X20, #8
    STR X20, [X9, #0]
    MOV X21, X20
    MOVZ X9, #0
    MOV X20, X9
    B L_START_ForHeader_5
L_START_ForHeader_1:
    MOV X9, X25
    MOVZ X10, #9
    CMP X9, X10
    B.GT L_START_ForExit_4
    B L_START_ForBody_2
L_START_ForHeader_5:
    MOV X9, X20
    MOV X11, X24
    SUB X11, X11, #1
    CMP X9, X11
    B.GT L_START_Exit_9
    B L_START_ForBody_6
L_START_ForIncrement_3:
    ADD X25, X25, #1
    B L_START_ForHeader_1
L_START_ForIncrement_7:
    ADD X20, X20, #1
    B L_START_ForHeader_5
L__bounds_error_handler_START:
    MOVZ X0, #0
    MOVZ X1, #65535
    MOVZ X2, #0
    BL _BCPL_BOUNDS_ERROR
    BRK #0
L_0:
    LDP x19, x20, [x29, #64]
    LDP x21, x22, [x29, #80]
    LDP x23, x24, [x29, #96]
    LDP x25, x26, [x29, #112]
    LDP x27, x28, [x29, #128]
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [SP, #0]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L___veneer_:
    movz x16, #30664
    movk x16, #1135, lsl #16
    movk x16, #1, lsl #32
    movk x16, #0, lsl #48
    blr x16

.section __DATA,__const
.p2align 3
L_pair0:
    .quad 0xa0000000a
    ; (upper half)
L_pair1:
    .quad 0x400000003
    ; (upper half)

.section __DATA,__data
.p2align 3
.p2align 2
L__data_segment_base:
