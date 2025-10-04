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
    STP X29, X30, [SP, #-80]!
    MOV X29, SP
    STP x19, x20, [x29, #40]
    STP x26, x27, [x29, #56]
    STR X28, [X29, #72] ; Saved Reg: X28 @ FP+72
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
L_START_Entry_0:
    ADRP X9, L_str0@PAGE
    ADD X9, X9, L_str0@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    MOVZ X9, #2
    LSL X20, X9, #1
    MOV X0, X20
    BL _GETVEC
    MOV X20, X0
    MOV X27, X20
    MOVZ X10, #2
    LSL X20, X10, #1
    MOV X0, X20
    BL _GETVEC
    MOV X20, X0
    MOV X26, X20
    ADRP X11, L_str1@PAGE
    ADD X11, X11, L_str1@PAGEOFF
    ADD X11, X11, #8
    MOV X0, X11
    BL _WRITEF
    ADRP X9, L_pair0@PAGE
    ADD X9, X9, L_pair0@PAGEOFF
    LDR X9, [X9, #0]
    MOVZ X10, #0
    LSL X11, X10, #3
    ADD X12, X27, X11
    STR X9, [X12, #0]
    ADRP X9, L_pair1@PAGE
    ADD X9, X9, L_pair1@PAGEOFF
    LDR X9, [X9, #0]
    MOVZ X10, #1
    LSL X11, X10, #3
    ADD X12, X27, X11
    STR X9, [X12, #0]
    ADRP X9, L_pair2@PAGE
    ADD X9, X9, L_pair2@PAGEOFF
    LDR X9, [X9, #0]
    MOVZ X10, #0
    LSL X11, X10, #3
    ADD X12, X26, X11
    STR X9, [X12, #0]
    ADRP X9, L_pair3@PAGE
    ADD X9, X9, L_pair3@PAGEOFF
    LDR X9, [X9, #0]
    MOVZ X10, #1
    LSL X11, X10, #3
    ADD X12, X26, X11
    STR X9, [X12, #0]
    ADRP X9, L_str2@PAGE
    ADD X9, X9, L_str2@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str3@PAGE
    ADD X9, X9, L_str3@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    MOVZ X9, #16
    MOV X10, X0
    MOV X0, X9
    BL _GETVEC
    MOV X9, X0
    MOV X0, X10
    LDR Q0, [X27, #0]
    LDR Q1, [X26, #0]
    ADD V2.4S, V0.4S, V1.4S
    STR Q2, [X9, #0]
    LDR Q3, [X27, #16]
    LDR Q4, [X26, #16]
    ADD V5.4S, V3.4S, V4.4S
    STR Q5, [X9, #16]
    LDR Q6, [X27, #32]
    LDR Q7, [X26, #32]
    ADD V16.4S, V6.4S, V7.4S
    STR Q16, [X9, #32]
    LDR Q17, [X27, #48]
    LDR Q18, [X26, #48]
    ADD V19.4S, V17.4S, V18.4S
    STR Q19, [X9, #48]
    MOV X15, X9
    ADRP X9, L_str4@PAGE
    ADD X9, X9, L_str4@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str5@PAGE
    ADD X9, X9, L_str5@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    B L_START_Exit_1
L_START_Exit_1:
    B L_0
L_0:
    LDP x19, x20, [x29, #40]
    LDP x26, x27, [x29, #56]
    LDR X28, [X29, #72] ; Restored Reg: X28 @ FP+72
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [SP, #0]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L___veneer_:
    movz x16, #1172
    movk x16, #1080, lsl #16
    movk x16, #1, lsl #32
    movk x16, #0, lsl #48
    blr x16

.section __DATA,__const
.p2align 3
L_str0:
    .quad 0x1f
    ; (upper half)
    .long 0x44
    .long 0x65
    .long 0x62
    .long 0x75
    .long 0x67
    .long 0x3a
    .long 0x20
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x53
    .long 0x20
    .long 0x54
    .long 0x79
    .long 0x70
    .long 0x65
    .long 0x20
    .long 0x52
    .long 0x6f
    .long 0x75
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0xa
    .long 0x0
    .long 0x0
L_str1:
    .quad 0x1e
    ; (upper half)
    .long 0x43
    .long 0x72
    .long 0x65
    .long 0x61
    .long 0x74
    .long 0x65
    .long 0x64
    .long 0x20
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x53
    .long 0x20
    .long 0x76
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x6f
    .long 0x72
    .long 0x73
    .long 0x20
    .long 0x61
    .long 0x20
    .long 0x61
    .long 0x6e
    .long 0x64
    .long 0x20
    .long 0x62
    .long 0xa
    .long 0x0
    .long 0x0
L_str2:
    .quad 0x14
    ; (upper half)
    .long 0x49
    .long 0x6e
    .long 0x69
    .long 0x74
    .long 0x69
    .long 0x61
    .long 0x6c
    .long 0x69
    .long 0x7a
    .long 0x65
    .long 0x64
    .long 0x20
    .long 0x76
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x6f
    .long 0x72
    .long 0x73
    .long 0xa
    .long 0x0
    .long 0x0
L_str3:
    .quad 0x1c
    ; (upper half)
    .long 0x41
    .long 0x62
    .long 0x6f
    .long 0x75
    .long 0x74
    .long 0x20
    .long 0x74
    .long 0x6f
    .long 0x20
    .long 0x70
    .long 0x65
    .long 0x72
    .long 0x66
    .long 0x6f
    .long 0x72
    .long 0x6d
    .long 0x3a
    .long 0x20
    .long 0x63
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x61
    .long 0x20
    .long 0x2b
    .long 0x20
    .long 0x62
    .long 0xa
    .long 0x0
    .long 0x0
L_str4:
    .quad 0x27
    ; (upper half)
    .long 0x41
    .long 0x64
    .long 0x64
    .long 0x69
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
    .long 0x20
    .long 0x2d
    .long 0x20
    .long 0x72
    .long 0x65
    .long 0x73
    .long 0x75
    .long 0x6c
    .long 0x74
    .long 0x20
    .long 0x73
    .long 0x74
    .long 0x6f
    .long 0x72
    .long 0x65
    .long 0x64
    .long 0x20
    .long 0x69
    .long 0x6e
    .long 0x20
    .long 0x63
    .long 0xa
    .long 0x0
    .long 0x0
L_str5:
    .quad 0x1c
    ; (upper half)
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x53
    .long 0x20
    .long 0x72
    .long 0x6f
    .long 0x75
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x74
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x20
    .long 0x63
    .long 0x6f
    .long 0x6d
    .long 0x70
    .long 0x6c
    .long 0x65
    .long 0x74
    .long 0x65
    .long 0xa
    .long 0x0
    .long 0x0
L_pair0:
    .quad 0x140000000a
    ; (upper half)
L_pair1:
    .quad 0x280000001e
    ; (upper half)
L_pair2:
    .quad 0x200000001
    ; (upper half)
L_pair3:
    .quad 0x400000003
    ; (upper half)

.section __DATA,__data
.p2align 3
    .long 0x0
    .long 0x0
    .long 0x0
    .long 0x0
    .long 0x0
L__data_segment_base:
