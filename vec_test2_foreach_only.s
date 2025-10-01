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

L_START:
    STP X29, X30, [SP, #-144]!
    MOV X29, SP
    STP x19, x20, [x29, #64]
    STP x21, x22, [x29, #80]
    STP x23, x24, [x29, #96]
    STP x25, x26, [x29, #112]
    STP x27, x28, [x29, #128]
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
L_START_Entry_0:
    ADRP X9, L_str0@PAGE
    ADD X9, X9, L_str0@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    MOVZ x20, #10
    MOV X0, X20
    BL _GETVEC
    MOV X27, X0
    MOVZ x24, #0
    B L_START_ForHeader_1
L_START_Exit_9:
    B L_0
L_START_ForBody_2:
    MOV X10, X24
    MOVZ X11, #5
    MUL X10, X10, X11
    LSL X11, X24, #3
    ADD X12, X27, X11
    STR X10, [X12, #0]
    B L_START_ForIncrement_3
L_START_ForEachBody_6:
    SUB X11, X27, #8
    LDR X10, [X11, #0] ; Load vector length for bounds check
    CMP X26, X10
    B.HS L__bounds_error_handler_START
    MOV X10, X26
    LSL X10, X10, #3
    ADD X11, X27, X10
    LDR X10, [X11, #0]
    MOV X23, X10
    ADRP X10, L_str1@PAGE
    ADD X10, X10, L_str1@PAGEOFF
    ADD X10, X10, #8
    MOV X0, X10
    MOV X1, X23
    BL _WRITEF1
    B L_START_ForEachIncrement_7
L_START_ForEachExit_8:
    ADRP X9, L_str2@PAGE
    ADD X9, X9, L_str2@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    B L_START_Exit_9
L_START_ForEachHeader_5:
    CMP x26, x25
    CSET X10, LT
    CMP X10, XZR
    B.NE L_START_ForEachBody_6
    B L_START_ForEachExit_8
L_START_ForEachIncrement_7:
    ADD x26, x26, #1
    B L_START_ForEachHeader_5
L_START_ForExit_4:
    ADRP X9, L_str3@PAGE
    ADD X9, X9, L_str3@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    ADRP X9, L_str4@PAGE
    ADD X9, X9, L_str4@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    SUB X10, X27, #8
    LDR X9, [X10, #0] ; Load vector/table/string length
    MOV X1, X9
    BL _WRITEF1
    ADRP X9, L_str5@PAGE
    ADD X9, X9, L_str5@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    SUB X10, X27, #8
    LDR X9, [X10, #0] ; Load vector/table/string length
    MOV X25, X9
    MOVZ x26, #0
    B L_START_ForEachHeader_5
L_START_ForHeader_1:
    MOV X9, X24
    SUB X11, X27, #8
    LDR X10, [X11, #0] ; Load vector/table/string length
    SUB X10, X10, #1
    CMP X9, X10
    B.GT L_START_ForExit_4
    B L_START_ForBody_2
L_START_ForIncrement_3:
    ADD x24, x24, #1
    B L_START_ForHeader_1
L__bounds_error_handler_START:
    MOVZ X0, #0
    MOVZ X1, #0
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
    LDP x29, x30, [x31]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L___veneer_:
    movz x16, #13924
    movk x16, #629, lsl #16
    movk x16, #1, lsl #32
    movk x16, #0, lsl #48
    blr x16

.section __DATA,__const
.p2align 3
L_str0:
    .quad 0x1f
    ; (upper half)
    .long 0x43
    .long 0x72
    .long 0x65
    .long 0x61
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x61
    .long 0x6e
    .long 0x64
    .long 0x20
    .long 0x70
    .long 0x6f
    .long 0x70
    .long 0x75
    .long 0x6c
    .long 0x61
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x76
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x6f
    .long 0x72
    .long 0xa
    .long 0x0
    .long 0x0
L_str1:
    .quad 0xc
    ; (upper half)
    .long 0x45
    .long 0x6c
    .long 0x65
    .long 0x6d
    .long 0x65
    .long 0x6e
    .long 0x74
    .long 0x3a
    .long 0x20
    .long 0x25
    .long 0x64
    .long 0xa
    .long 0x0
    .long 0x0
L_str2:
    .quad 0x12
    ; (upper half)
    .long 0x46
    .long 0x4f
    .long 0x52
    .long 0x45
    .long 0x41
    .long 0x43
    .long 0x48
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
L_str3:
    .quad 0x1e
    ; (upper half)
    .long 0x56
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x6f
    .long 0x72
    .long 0x20
    .long 0x70
    .long 0x6f
    .long 0x70
    .long 0x75
    .long 0x6c
    .long 0x61
    .long 0x74
    .long 0x65
    .long 0x64
    .long 0x20
    .long 0x73
    .long 0x75
    .long 0x63
    .long 0x63
    .long 0x65
    .long 0x73
    .long 0x73
    .long 0x66
    .long 0x75
    .long 0x6c
    .long 0x6c
    .long 0x79
    .long 0xa
    .long 0x0
    .long 0x0
L_str4:
    .quad 0x12
    ; (upper half)
    .long 0x56
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x6f
    .long 0x72
    .long 0x20
    .long 0x6c
    .long 0x65
    .long 0x6e
    .long 0x67
    .long 0x74
    .long 0x68
    .long 0x3a
    .long 0x20
    .long 0x25
    .long 0x64
    .long 0xa
    .long 0x0
    .long 0x0
L_str5:
    .quad 0x16
    ; (upper half)
    .long 0x53
    .long 0x74
    .long 0x61
    .long 0x72
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x46
    .long 0x4f
    .long 0x52
    .long 0x45
    .long 0x41
    .long 0x43
    .long 0x48
    .long 0x20
    .long 0x74
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0xa
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
