.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _START
.globl _HeapManager_exit_scope
.globl _WRITEN
.globl _GETVEC
.globl _WRITEF
.globl _BCPL_LIST_GET_HEAD_AS_FOCT
.globl _BCPL_LIST_GET_HEAD_AS_QUAD
.globl _NEWLINE
.globl _SDL2_QUIT
.globl _RDCH
.globl _BCPL_LIST_GET_NTH
.globl _BCPL_ALLOC_WORDS
.globl _BCPL_LIST_GET_HEAD_AS_FPAIR
.globl _SPLIT
.globl _CAIRO_GET_VERSION
.globl _BCPL_CHECK_AND_DISPLAY_ERRORS
.globl _FIND
.globl _CAIRO_CLEAR
.globl _SDL2_GET_EVENT_BUTTON
.globl _LPND
.globl _FGETVEC
.globl _SLURP
.globl _SDL2_CREATE_WINDOW
.globl _TIMER_START
.globl _SDL2_SET_WINDOW_TITLE
.globl _CAIRO_DRAW_IMAGE
.globl _UNPACKSTRING
.globl _SDL2_DELAY
.globl _FREEVEC
.globl _SDL2_CLEAR_ERROR
.globl _LIST_APPEND_INT
.globl _WRITEF2
.globl _HEAPMANAGER_WAITFORSAMM
.globl _BCPL_BOUNDS_ERROR
.globl _REVERSE
.globl _STRCOPY
.globl _LIST_HEAD_INT
.globl _LIST_APPEND_STRING
.globl _LIST_HEAD_FLOAT
.globl _GET_FREE_LIST_HEAD_ADDR
.globl _LIST_TAIL
.globl _JOIN
.globl _MALLOC
.globl _BCPL_FREE_CELLS
.globl _SDL2_GET_EVENT_KEY
.globl _BCPL_CONCAT_LISTS
.globl _SDL2_DRAW_POINT
.globl _HEAPMANAGER_EXIT_SCOPE
.globl _BCPL_LIST_APPEND_FOCT
.globl _BCPL_LIST_APPEND_OCT
.globl _SDL2_GET_ERROR
.globl _FCOS
.globl _APND
.globl _FILE_OPEN_READ
.globl _LIST_APPEND_FLOAT
.globl _BCPL_LIST_CREATE_EMPTY
.globl _BCPL_LIST_APPEND_PAIR
.globl _BCPL_LIST_APPEND_FQUAD
.globl _FILE_WRITES
.globl _FILTER
.globl _FLOG
.globl _BCPL_CLEAR_ERRORS
.globl _BCPL_LIST_GET_HEAD_AS_FQUAD
.globl _HEAPMANAGER_ENTER_SCOPE
.globl _FPND
.globl _FEXP
.globl _WRITEF4
.globl _CAIRO_FILL_RECTS_VEC
.globl _RETURNNODETOFREELIST
.globl _RAND
.globl _SDL2_FILL_RECT
.globl _FWRITE
.globl _FILE_TELL
.globl _STRLEN
.globl _RND
.globl _FILE_OPEN_APPEND
.globl _SDL2_CREATE_WINDOW_EX
.globl _FSIN
.globl _WRITES
.globl _BCPL_ALLOC_CHARS
.globl _WRITEF5
.globl _BCPL_FREE_LIST_SAFE
.globl _DEEPCOPYLIST
.globl _BCPL_FREE_LIST
.globl _FILE_CLOSE
.globl _FIX
.globl _WRITEF3
.globl _BCPL_GET_ATOM_TYPE
.globl _LIST_FREE
.globl _FILE_READS
.globl _PIC_RUNTIME_HELPER
.globl _SDL2_GET_EVENT_MOUSE
.globl _CAIRO_DRAW_TEXT
.globl _STRCMP
.globl _SDL2_GET_VIDEO_DRIVERS
.globl _FINISH
.globl _CAIRO_FILL_RECT
.globl _HEAPMANAGER_ISSAMMENABLED
.globl _SDL2_DRAW_RECT
.globl _TIMER_CLEAR
.globl _DEBUG_PRINT_VTABLE_STORE
.globl _FILE_OPEN_WRITE
.globl _SDL2_GET_TICKS
.globl _WRITEF6
.globl _BCPL_LIST_APPEND_FPAIR
.globl _HEAPMANAGER_SETSAMMENABLED
.globl _SDL2_GET_DISPLAY_MODES
.globl _BCPL_LIST_GET_HEAD_AS_OCT
.globl _TIMER_END
.globl _SDL2_GET_VERSION
.globl _CAIRO_DRAW_LINE
.globl _OBJECT_HEAP_FREE
.globl _CONCAT
.globl _CAIRO_IS_AVAILABLE
.globl _BCPL_LIST_GET_HEAD_AS_PAIR
.globl _SDL2_DRAW_LINE
.globl _BCPL_LIST_GET_REST
.globl _CAIRO_SET_COLOR
.globl _RUNTIME_METHOD_LOOKUP
.globl _DEBUG_PRINT_FINAL_OBJECT
.globl _CAIRO_FILL_CIRCLE
.globl _SDL2_INIT
.globl _SDL2_CLEAR
.globl _SDL2_DESTROY_RENDERER
.globl _SDL2_SET_DRAW_COLOR
.globl _SETTYPE
.globl _FILE_SEEK
.globl _BCPL_LIST_APPEND_QUAD
.globl _CAIRO_SAVE_PNG
.globl _SDL2_CREATE_RENDERER_EX
.globl _SPND
.globl _SDL2_POLL_EVENT
.globl _CAIRO_FILL_CIRCLES_VEC
.globl _SDL2_CREATE_RENDERER
.globl _LIST_CREATE
.globl _CAIRO_DRAW_LINES_VEC
.globl _SDL2_SET_WINDOW_SIZE
.globl _SDL2_INIT_SUBSYSTEMS
.globl _SDL2_GET_CURRENT_VIDEO_DRIVER
.globl _OBJECT_HEAP_ALLOC
.globl _CAIRO_LOAD_PNG
.globl _SDL2_TEST_BASIC
.globl _WRITEF1
.globl _FILE_WRITE
.globl _BCPL_GET_LAST_ERROR
.globl _SDL2_PRESENT
.globl _TIMER_GET_CALL_COUNT
.globl _COPYLIST
.globl _FABS
.globl _CAIRO_LOAD_IMAGE
.globl _SPIT
.globl _FTAN
.globl _FRND
.globl _WRITEC
.globl _FILE_READ
.globl _TIMER_DISPLAY
.globl _FILE_EOF
.globl _TIMER_GET_TOTAL_NS
.globl _DEEPCOPYLITERALLIST
.globl _PACKSTRING
.globl _BCPL_LIST_APPEND_LIST
.globl _CAIRO_CREATE_SURFACE
.globl _WRITEF7
.globl _SDL2_DESTROY_WINDOW
.globl _CAIRO_SET_FONT
.p2align 2
_start:
_START:
    B L_START
    
; --- Veneer Section ---
    ; --- End Veneer Section ---

L_START:
    STP X29, X30, [SP, #-96]!
    MOV X29, SP
    STP x19, x25, [x29, #48]
    STP x26, x27, [x29, #64]
    STR X28, [X29, #80] ; Saved Reg: X28 @ FP+80
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
L_START_Entry_0:
    MOVZ X9, #0
    MOV X27, X9
    MOVZ X9, #200
    MOV X25, X9
    MOVZ X9, #150
    MOV X25, X9
    ADRP X9, L_str0_plus_8@PAGE
    ADD X9, X9, L_str0_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    BL _CAIRO_IS_AVAILABLE
    MOV X26, X0
    CMP x26, #0
    CSET X10, EQ
    CMP X10, XZR
    B.EQ L_START_Join_2
    B L_START_Then_1
L_START_FinishCleanup_3:
    MOVZ X9, #0
    MOV X0, X9
    MOVZ X9, #1
    MOVK X9, #512, LSL #16
    MOV X16, X9
    SVC #128
L_START_FinishCleanup_6:
    MOVZ X9, #0
    MOV X0, X9
    MOVZ X9, #1
    MOVK X9, #512, LSL #16
    MOV X16, X9
    SVC #128
L_START_FinishCleanup_7:
    MOVZ X9, #0
    MOV X0, X9
    MOVZ X9, #1
    MOVK X9, #512, LSL #16
    MOV X16, X9
    SVC #128
L_START_Join_2:
    ADRP X9, L_str1_plus_8@PAGE
    ADD X9, X9, L_str1_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str2_plus_8@PAGE
    ADD X9, X9, L_str2_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    SUB SP, SP, #16
    MOVZ X9, #150
    STR X9, [SP, #8]
    MOVZ X9, #200
    STR X9, [SP, #0]
    LDP x0, x1, [SP, #0]
    ADD SP, SP, #16
    BL _CAIRO_CREATE_SURFACE
    MOV X27, X0
    CMP x27, #0
    CSET X10, EQ
    CMP X10, XZR
    B.EQ L_START_Join_5
    B L_START_Then_4
L_START_Join_5:
    ADRP X9, L_str3_plus_8@PAGE
    ADD X9, X9, L_str3_plus_8@PAGEOFF
    MOV X0, X9
    MOV X1, X27
    BL _WRITEF1
    ADRP X9, L_str4_plus_8@PAGE
    ADD X9, X9, L_str4_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    SUB SP, SP, #16
    MOVZ X9, #65535
    MOVK X9, #65535, LSL #16
    STR X9, [SP, #8]
    STR X27, [SP, #0]
    LDP x0, x1, [SP, #0]
    ADD SP, SP, #16
    BL _CAIRO_CLEAR
    ADRP X9, L_str5_plus_8@PAGE
    ADD X9, X9, L_str5_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    SUB SP, SP, #16
    MOVZ X9, #255
    MOVK X9, #65280, LSL #16
    STR X9, [SP, #8]
    STR X27, [SP, #0]
    LDP x0, x1, [SP, #0]
    ADD SP, SP, #16
    BL _CAIRO_SET_COLOR
    ADRP X9, L_str6_plus_8@PAGE
    ADD X9, X9, L_str6_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    SUB SP, SP, #48
    ADRP X10, L_float0@PAGE
    ADD X10, X10, L_float0@PAGEOFF
    LDR D0, [X10, #0]
    STR D0, [SP, #32]
    ADRP X11, L_float1@PAGE
    ADD X11, X11, L_float1@PAGEOFF
    LDR D0, [X11, #0]
    STR D0, [SP, #24]
    ADRP X12, L_float2@PAGE
    ADD X12, X12, L_float2@PAGEOFF
    LDR D0, [X12, #0]
    STR D0, [SP, #16]
    ADRP X13, L_float2@PAGE
    ADD X13, X13, L_float2@PAGEOFF
    LDR D0, [X13, #0]
    STR D0, [SP, #8]
    STR X27, [SP, #0]
    LDP x0, x1, [SP, #0]
    LDP x2, x3, [SP, #16]
    LDR X4, [SP, #32]
    ADD SP, SP, #48
    SCVTF D0, X1
    SCVTF D1, X2
    SCVTF D2, X3
    SCVTF D3, X4
    BL _CAIRO_FILL_RECT
    ADRP X9, L_str7_plus_8@PAGE
    ADD X9, X9, L_str7_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    SUB SP, SP, #16
    ADRP X9, L_str8_plus_8@PAGE
    ADD X9, X9, L_str8_plus_8@PAGEOFF
    STR X9, [SP, #8]
    STR X27, [SP, #0]
    LDP x0, x1, [SP, #0]
    ADD SP, SP, #16
    BL _CAIRO_SAVE_PNG
    ADRP X9, L_str9_plus_8@PAGE
    ADD X9, X9, L_str9_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    BL _HeapManager_exit_scope
    B L_START_FinishCleanup_7
L_START_Then_1:
    ADRP X9, L_str10_plus_8@PAGE
    ADD X9, X9, L_str10_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    BL _HeapManager_exit_scope
    B L_START_FinishCleanup_3
L_START_Then_4:
    ADRP X9, L_str11_plus_8@PAGE
    ADD X9, X9, L_str11_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    BL _HeapManager_exit_scope
    B L_START_FinishCleanup_6
L_0:
    LDP x19, x25, [x29, #48]
    LDP x26, x27, [x29, #64]
    LDR X28, [X29, #80] ; Restored Reg: X28 @ FP+80
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [SP, #0]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET

.section __DATA,__const
.p2align 3
L_str0:
    .quad 0x14
    ; (upper half)
.p2align 2
L_str0_plus_8:
    .long 0x43
    .long 0x61
    .long 0x69
    .long 0x72
    .long 0x6f
    .long 0x20
    .long 0x66
    .long 0x6c
    .long 0x6f
    .long 0x61
    .long 0x74
    .long 0x20
    .long 0x74
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x2e
    .long 0x2e
    .long 0x2e
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str1:
    .quad 0x13
    ; (upper half)
.p2align 2
L_str1_plus_8:
    .long 0x43
    .long 0x61
    .long 0x69
    .long 0x72
    .long 0x6f
    .long 0x20
    .long 0x69
    .long 0x73
    .long 0x20
    .long 0x61
    .long 0x76
    .long 0x61
    .long 0x69
    .long 0x6c
    .long 0x61
    .long 0x62
    .long 0x6c
    .long 0x65
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str2:
    .quad 0x14
    ; (upper half)
.p2align 2
L_str2_plus_8:
    .long 0x43
    .long 0x72
    .long 0x65
    .long 0x61
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x73
    .long 0x75
    .long 0x72
    .long 0x66
    .long 0x61
    .long 0x63
    .long 0x65
    .long 0x2e
    .long 0x2e
    .long 0x2e
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str3:
    .quad 0x20
    ; (upper half)
.p2align 2
L_str3_plus_8:
    .long 0x53
    .long 0x75
    .long 0x72
    .long 0x66
    .long 0x61
    .long 0x63
    .long 0x65
    .long 0x20
    .long 0x63
    .long 0x72
    .long 0x65
    .long 0x61
    .long 0x74
    .long 0x65
    .long 0x64
    .long 0x20
    .long 0x77
    .long 0x69
    .long 0x74
    .long 0x68
    .long 0x20
    .long 0x68
    .long 0x61
    .long 0x6e
    .long 0x64
    .long 0x6c
    .long 0x65
    .long 0x3a
    .long 0x20
    .long 0x25
    .long 0x4e
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str4:
    .quad 0x14
    ; (upper half)
.p2align 2
L_str4_plus_8:
    .long 0x43
    .long 0x6c
    .long 0x65
    .long 0x61
    .long 0x72
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x73
    .long 0x75
    .long 0x72
    .long 0x66
    .long 0x61
    .long 0x63
    .long 0x65
    .long 0x2e
    .long 0x2e
    .long 0x2e
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str5:
    .quad 0x11
    ; (upper half)
.p2align 2
L_str5_plus_8:
    .long 0x53
    .long 0x65
    .long 0x74
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x63
    .long 0x6f
    .long 0x6c
    .long 0x6f
    .long 0x72
    .long 0x2e
    .long 0x2e
    .long 0x2e
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str6:
    .quad 0x29
    ; (upper half)
.p2align 2
L_str6_plus_8:
    .long 0x44
    .long 0x72
    .long 0x61
    .long 0x77
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x72
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x61
    .long 0x6e
    .long 0x67
    .long 0x6c
    .long 0x65
    .long 0x20
    .long 0x77
    .long 0x69
    .long 0x74
    .long 0x68
    .long 0x20
    .long 0x66
    .long 0x6c
    .long 0x6f
    .long 0x61
    .long 0x74
    .long 0x20
    .long 0x6c
    .long 0x69
    .long 0x74
    .long 0x65
    .long 0x72
    .long 0x61
    .long 0x6c
    .long 0x73
    .long 0x2e
    .long 0x2e
    .long 0x2e
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str7:
    .quad 0x10
    ; (upper half)
.p2align 2
L_str7_plus_8:
    .long 0x53
    .long 0x61
    .long 0x76
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x69
    .long 0x6d
    .long 0x61
    .long 0x67
    .long 0x65
    .long 0x2e
    .long 0x2e
    .long 0x2e
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str8:
    .quad 0x12
    ; (upper half)
.p2align 2
L_str8_plus_8:
    .long 0x66
    .long 0x6c
    .long 0x6f
    .long 0x61
    .long 0x74
    .long 0x5f
    .long 0x66
    .long 0x69
    .long 0x78
    .long 0x5f
    .long 0x74
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x2e
    .long 0x70
    .long 0x6e
    .long 0x67
    .long 0x0
    .long 0x0
.p2align 3
L_str9:
    .quad 0x1d
    ; (upper half)
.p2align 2
L_str9_plus_8:
    .long 0x54
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
    .long 0x21
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str10:
    .quad 0x1b
    ; (upper half)
.p2align 2
L_str10_plus_8:
    .long 0x45
    .long 0x52
    .long 0x52
    .long 0x4f
    .long 0x52
    .long 0x3a
    .long 0x20
    .long 0x43
    .long 0x61
    .long 0x69
    .long 0x72
    .long 0x6f
    .long 0x20
    .long 0x6e
    .long 0x6f
    .long 0x74
    .long 0x20
    .long 0x61
    .long 0x76
    .long 0x61
    .long 0x69
    .long 0x6c
    .long 0x61
    .long 0x62
    .long 0x6c
    .long 0x65
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str11:
    .quad 0x20
    ; (upper half)
.p2align 2
L_str11_plus_8:
    .long 0x45
    .long 0x52
    .long 0x52
    .long 0x4f
    .long 0x52
    .long 0x3a
    .long 0x20
    .long 0x46
    .long 0x61
    .long 0x69
    .long 0x6c
    .long 0x65
    .long 0x64
    .long 0x20
    .long 0x74
    .long 0x6f
    .long 0x20
    .long 0x63
    .long 0x72
    .long 0x65
    .long 0x61
    .long 0x74
    .long 0x65
    .long 0x20
    .long 0x73
    .long 0x75
    .long 0x72
    .long 0x66
    .long 0x61
    .long 0x63
    .long 0x65
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_float0:
    .quad 0x403e000000000000
    ; (upper half)
L_float1:
    .quad 0x4049000000000000
    ; (upper half)
L_float2:
    .quad 0x4024000000000000
    ; (upper half)

.section __DATA,__data
.p2align 3
    .long 0x0
    .long 0x0
    .long 0x0
    .long 0x0
    .long 0x0
.p2align 2
L__data_segment_base:
