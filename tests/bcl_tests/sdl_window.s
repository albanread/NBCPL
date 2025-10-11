.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _START
.globl _HeapManager_exit_scope
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
.globl _SLURP
.globl _UNPACKSTRING
.globl _FREEVEC
.globl _SDL2_DELAY
.globl _SDL2_CREATE_WINDOW
.globl _DEEPCOPYLIST
.globl _BCPL_FREE_LIST_SAFE
.globl _STRCOPY
.globl _LIST_APPEND_STRING
.globl _LIST_HEAD_INT
.globl _WRITEC
.globl _FILE_READ
.globl _LPND
.globl _FGETVEC
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
.globl _SDL2_GET_EVENT_BUTTON
.globl _SDL2_DRAW_POINT
.globl _HEAPMANAGER_EXIT_SCOPE
.globl _FILE_OPEN_WRITE
.globl _SDL2_GET_TICKS
.globl _WRITEF6
.globl _HEAPMANAGER_SETSAMMENABLED
.globl _BCPL_LIST_GET_REST
.globl _SDL2_DESTROY_RENDERER
.globl _SDL2_CLEAR
.globl _SDL2_CLEAR_ERROR
.globl _SDL2_CREATE_RENDERER
.globl _SDL2_SET_WINDOW_SIZE
.globl _SETTYPE
.globl _FILE_SEEK
.globl _SDL2_TEST_BASIC
.globl _OBJECT_HEAP_ALLOC
.globl _SDL2_GET_VERSION
.globl _WRITEF7
.globl _SDL2_DESTROY_WINDOW
.globl _SDL2_SET_WINDOW_TITLE
.globl _FILE_WRITES
.globl _PIC_RUNTIME_HELPER
.globl _WRITEF4
.globl _FEXP
.globl _FPND
.globl _FIX
.globl _WRITEF1
.globl _FILE_WRITE
.globl _SDL2_DRAW_RECT
.globl _COPYLIST
.globl _SDL2_GET_DISPLAY_MODES
.globl _BCPL_GET_LAST_ERROR
.globl _SDL2_PRESENT
.globl _SPND
.globl _SDL2_CREATE_RENDERER_EX
.globl _SDL2_SET_DRAW_COLOR
.globl _OBJECT_HEAP_FREE
.globl _CONCAT
.globl _SDL2_DRAW_LINE
.globl _LIST_TAIL
.globl _SDL2_GET_ERROR
.globl _FIND
.globl _FILE_EOF
.globl _RUNTIME_METHOD_LOOKUP
.globl _HEAPMANAGER_WAITFORSAMM
.globl _SDL2_POLL_EVENT
.p2align 2
_start:
_START:
    B L_START
    
; --- Veneer Section ---
    ; --- End Veneer Section ---

L_START:
    STP X29, X30, [SP, #-160]!
    MOV X29, SP
    STP x19, x20, [x29, #72]
    STP x21, x22, [x29, #88]
    STP x23, x24, [x29, #104]
    STP x25, x26, [x29, #120]
    STP x27, x28, [x29, #136]
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
L_START_Entry_0:
    ADRP X9, L_str0_plus_8@PAGE
    ADD X9, X9, L_str0_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str1_plus_8@PAGE
    ADD X9, X9, L_str1_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    MOVZ x21, #32
    MOV X0, X21
    BL _SDL2_INIT_SUBSYSTEMS
    MOV X20, X0
    CMP x20, #0
    CSET X11, EQ
    CMP X11, XZR
    B.EQ L_START_Else_2
    B L_START_Then_1
L_START_Else_2:
    ADRP X10, L_str2_plus_8@PAGE
    ADD X10, X10, L_str2_plus_8@PAGEOFF
    MOV X0, X10
    BL _WRITEF
    BL _HeapManager_exit_scope
    B L_START_FinishCleanup_4
L_START_Else_6:
    ADRP X9, L_str3_plus_8@PAGE
    ADD X9, X9, L_str3_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    B L_START_Join_7
L_START_Else_9:
    ADRP X9, L_str4_plus_8@PAGE
    ADD X9, X9, L_str4_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    B L_START_Join_7
L_START_Exit_20:
    B L_0
L_START_FinishCleanup_4:
    MOVZ X9, #0
    MOV X0, X9
    MOVZ X9, #1
    MOVK X9, #512, LSL #16
    MOV X16, X9
    SVC #128
L_START_ForBody_17:
    MOVZ X9, #2
    MOVZ X10, #2
    MOV X0, X27
    MOV X1, X23
    MOV X2, X23
    MOV X3, X9
    MOV X4, X10
    BL _SDL2_DRAW_RECT
    B L_START_ForIncrement_18
L_START_ForExit_19:
    MOV X0, X27
    BL _SDL2_PRESENT
    MOVZ X9, #16
    MOV X0, X9
    BL _SDL2_DELAY
    B L_START_WhileHeader_11
L_START_ForHeader_16:
    MOV X9, X23
    MOVZ X10, #200
    CMP X9, X10
    B.GT L_START_ForExit_19
    B L_START_ForBody_17
L_START_ForIncrement_18:
    ADD X23, X23, #1
    B L_START_ForHeader_16
L_START_Join_15:
    MOVZ X9, #30
    MOVZ X11, #30
    MOVZ X12, #50
    MOVZ X13, #255
    MOV X0, X27
    MOV X1, X9
    MOV X2, X11
    MOV X3, X12
    MOV X4, X13
    BL _SDL2_SET_DRAW_COLOR
    MOV X0, X27
    BL _SDL2_CLEAR
    MOVZ X9, #20
    MOVZ X10, #220
    MOVZ X11, #20
    MOVZ X12, #255
    MOV X0, X27
    MOV X1, X9
    MOV X2, X10
    MOV X3, X11
    MOV X4, X12
    BL _SDL2_SET_DRAW_COLOR
    MOVZ X9, #100
    MOV X23, X9
    B L_START_ForHeader_16
L_START_Join_3:
    ADRP X9, L_str5_plus_8@PAGE
    ADD X9, X9, L_str5_plus_8@PAGEOFF
    MOV X20, X9
    MOV X0, X20
    BL _SDL2_CREATE_WINDOW
    MOV X25, X0
    CMP x25, #0
    CSET X11, GT
    CMP X11, XZR
    B.EQ L_START_Else_6
    B L_START_Then_5
L_START_Join_7:
    BL _SDL2_QUIT
    ADRP X9, L_str6_plus_8@PAGE
    ADD X9, X9, L_str6_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    B L_START_Exit_20
L_START_Then_1:
    ADRP X9, L_str7_plus_8@PAGE
    ADD X9, X9, L_str7_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    B L_START_Join_3
L_START_Then_14:
    MOVZ X9, #0
    MOV X26, X9
    ADRP X9, L_str8_plus_8@PAGE
    ADD X9, X9, L_str8_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITES
    B L_START_Join_15
L_START_Then_5:
    MOV X20, X25
    MOV X0, X20
    BL _SDL2_CREATE_RENDERER
    MOV X27, X0
    CMP x27, #0
    CSET X10, GT
    CMP X10, XZR
    B.EQ L_START_Else_9
    B L_START_Then_8
L_START_Then_8:
    MOVZ X9, #1
    MOV X26, X9
    ADRP X9, L_str9_plus_8@PAGE
    ADD X9, X9, L_str9_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITES
    B L_START_WhileHeader_11
L_START_WhileBody_12:
    BL _SDL2_POLL_EVENT
    MOV X22, X0
    MOV X21, X22
    CMP x21, #256
    CSET X10, EQ
    CMP X10, XZR
    B.EQ L_START_Join_15
    B L_START_Then_14
L_START_WhileExit_13:
    ADRP X9, L_str10_plus_8@PAGE
    ADD X9, X9, L_str10_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    MOV X0, X27
    BL _SDL2_DESTROY_RENDERER
    MOV X0, X25
    BL _SDL2_DESTROY_WINDOW
    B L_START_Join_7
L_START_WhileHeader_11:
    CMP X26, XZR
    B.EQ L_START_WhileExit_13
    B L_START_WhileBody_12
L_0:
    LDP x19, x20, [x29, #72]
    LDP x21, x22, [x29, #88]
    LDP x23, x24, [x29, #104]
    LDP x25, x26, [x29, #120]
    LDP x27, x28, [x29, #136]
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
    .long 0x53
    .long 0x44
    .long 0x4c
    .long 0x32
    .long 0x20
    .long 0x57
    .long 0x69
    .long 0x6e
    .long 0x64
    .long 0x6f
    .long 0x77
    .long 0x20
    .long 0x45
    .long 0x78
    .long 0x61
    .long 0x6d
    .long 0x70
    .long 0x6c
    .long 0x65
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str1:
    .quad 0x14
    ; (upper half)
.p2align 2
L_str1_plus_8:
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str2:
    .quad 0x20
    ; (upper half)
.p2align 2
L_str2_plus_8:
    .long 0x274c
    .long 0x20
    .long 0x53
    .long 0x44
    .long 0x4c
    .long 0x32
    .long 0x20
    .long 0x69
    .long 0x6e
    .long 0x69
    .long 0x74
    .long 0x69
    .long 0x61
    .long 0x6c
    .long 0x69
    .long 0x7a
    .long 0x61
    .long 0x74
    .long 0x69
    .long 0x6f
    .long 0x6e
    .long 0x20
    .long 0x66
    .long 0x61
    .long 0x69
    .long 0x6c
    .long 0x65
    .long 0x64
    .long 0x2e
    .long 0x2e
    .long 0x2e
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str3:
    .quad 0x1b
    ; (upper half)
.p2align 2
L_str3_plus_8:
    .long 0x274c
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
    .long 0x77
    .long 0x69
    .long 0x6e
    .long 0x64
    .long 0x6f
    .long 0x77
    .long 0x21
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str4:
    .quad 0x1d
    ; (upper half)
.p2align 2
L_str4_plus_8:
    .long 0x274c
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
    .long 0x72
    .long 0x65
    .long 0x6e
    .long 0x64
    .long 0x65
    .long 0x72
    .long 0x65
    .long 0x72
    .long 0x21
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str5:
    .quad 0x10
    ; (upper half)
.p2align 2
L_str5_plus_8:
    .long 0x20
    .long 0x54
    .long 0x68
    .long 0x65
    .long 0x20
    .long 0x42
    .long 0x43
    .long 0x50
    .long 0x4c
    .long 0x20
    .long 0x57
    .long 0x69
    .long 0x6e
    .long 0x64
    .long 0x6f
    .long 0x77
    .long 0x0
    .long 0x0
.p2align 3
L_str6:
    .quad 0x1a
    ; (upper half)
.p2align 2
L_str6_plus_8:
    .long 0x2705
    .long 0x20
    .long 0x53
    .long 0x44
    .long 0x4c
    .long 0x32
    .long 0x20
    .long 0x73
    .long 0x68
    .long 0x75
    .long 0x74
    .long 0x64
    .long 0x6f
    .long 0x77
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
    .long 0x2e
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str7:
    .quad 0x21
    ; (upper half)
.p2align 2
L_str7_plus_8:
    .long 0x2705
    .long 0x20
    .long 0x53
    .long 0x44
    .long 0x4c
    .long 0x32
    .long 0x20
    .long 0x69
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
L_str8:
    .quad 0xc
    ; (upper half)
.p2align 2
L_str8_plus_8:
    .long 0x51
    .long 0x55
    .long 0x49
    .long 0x54
    .long 0x20
    .long 0x70
    .long 0x72
    .long 0x65
    .long 0x73
    .long 0x73
    .long 0x65
    .long 0x64
    .long 0x0
    .long 0x0
.p2align 3
L_str9:
    .quad 0x9
    ; (upper half)
.p2align 2
L_str9_plus_8:
    .long 0x52
    .long 0x75
    .long 0x6e
    .long 0x6e
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str10:
    .quad 0x23
    ; (upper half)
.p2align 2
L_str10_plus_8:
    .long 0x43
    .long 0x6c
    .long 0x65
    .long 0x61
    .long 0x6e
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x75
    .long 0x70
    .long 0x20
    .long 0x72
    .long 0x65
    .long 0x6e
    .long 0x64
    .long 0x65
    .long 0x72
    .long 0x65
    .long 0x72
    .long 0x20
    .long 0x61
    .long 0x6e
    .long 0x64
    .long 0x20
    .long 0x77
    .long 0x69
    .long 0x6e
    .long 0x64
    .long 0x6f
    .long 0x77
    .long 0x2e
    .long 0x2e
    .long 0x2e
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
    .long 0x0
.p2align 2
L__data_segment_base:
