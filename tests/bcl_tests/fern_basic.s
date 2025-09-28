.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _START
.globl _HeapManager_exit_scope
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
    STP X29, X30, [SP, #-192]!
    MOV X29, SP
    STR X19, [X29, #96] ; Saved Reg: X19 @ FP+96
    STR X20, [X29, #104] ; Saved Reg: X20 @ FP+104
    STR X21, [X29, #112] ; Saved Reg: X21 @ FP+112
    STR X22, [X29, #120] ; Saved Reg: X22 @ FP+120
    STR X23, [X29, #128] ; Saved Reg: X23 @ FP+128
    STR X24, [X29, #136] ; Saved Reg: X24 @ FP+136
    STR X25, [X29, #144] ; Saved Reg: X25 @ FP+144
    STR X26, [X29, #152] ; Saved Reg: X26 @ FP+152
    STR X27, [X29, #160] ; Saved Reg: X27 @ FP+160
    STR X28, [X29, #168] ; Saved Reg: X28 @ FP+168
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
L_START_Entry_0:
    MOVZ X9, #500
    MOV X26, X9
    MOVZ X9, #32
    MOV X22, X9
    MOV X0, X22
    BL _SDL2_INIT_SUBSYSTEMS
    MOV X15, X0
    MOV X10, X15
    CMP X10, #0
    CSET X11, EQ
    CMP X11, XZR
    B.EQ L_START_Join_2
    B L_START_Then_1
L_START_FinishCleanup_18:
    MOVZ X10, #0
    MOV X0, X10
    MOVZ X10, #1
    MOVK X10, #512, LSL #16
    MOV X16, X10
    SVC #128
L_START_ForBody_11:
    MOVZ X10, #800
    MOV X22, X10
    MOV X0, X22
    BL _RAND
    MOV X20, X0
    MOVZ X11, #480
    MOV X22, X11
    MOV X0, X22
    BL _RAND
    MOV X19, X0
    MOV X0, X25
    MOV X1, X20
    MOV X2, X19
    BL _SDL2_DRAW_POINT
    B L_START_ForIncrement_12
L_START_ForExit_13:
    MOV X0, X25
    BL _SDL2_PRESENT
    MOVZ X9, #10
    MOV X0, X9
    BL _SDL2_DELAY
    LDR X9, [X29, #24] ; _opt_temp_0
    MOV X22, X9
    MOV X9, X22
    CMP X9, #256
    CSET X10, EQ
    CMP X10, XZR
    B.EQ L_START_Join_15
    B L_START_Then_14
L_START_ForHeader_10:
    MOV X9, X27
    CMP X9, #4000
    B.GT L_START_ForExit_13
    B L_START_ForBody_11
L_START_ForIncrement_12:
    MOV X9, X27
    ADD X9, X9, #1
    MOV X27, X9
    B L_START_ForHeader_10
L_START_Join_15:
    MOV X9, X26
    CMP X9, #1
    CSET X10, LT
    CMP X10, XZR
    B.EQ L_START_Join_17
    B L_START_Then_16
L_START_Join_17:
    B L_START_WhileHeader_7
L_START_Join_2:
    ADRP X9, L_str0@PAGE
    ADD X9, X9, L_str0@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    BL _NEWLINE
    BL _HeapManager_exit_scope
    B L_START_FinishCleanup_18
L_START_Join_4:
    MOV X0, X25
    BL _SDL2_DESTROY_RENDERER
    MOV X0, X23
    BL _SDL2_DESTROY_WINDOW
    BL _SDL2_QUIT
    B L_START_Join_2
L_START_Join_6:
    B L_START_Join_4
L_START_Then_1:
    ADRP X9, L_str1@PAGE
    ADD X9, X9, L_str1@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    BL _NEWLINE
    ADRP X9, L_str2@PAGE
    ADD X9, X9, L_str2@PAGEOFF
    ADD X9, X9, #8
    MOV X19, X9
    MOV X0, X19
    BL _SDL2_CREATE_WINDOW
    MOV X23, X0
    MOV X10, X23
    CMP X10, #0
    CSET X11, GT
    CMP X11, XZR
    B.EQ L_START_Join_4
    B L_START_Then_3
L_START_Then_14:
    MOVZ X10, #0
    MOV X24, X10
    B L_START_Join_15
L_START_Then_16:
    MOVZ X10, #0
    MOV X24, X10
    ADRP X10, L_str3@PAGE
    ADD X10, X10, L_str3@PAGEOFF
    ADD X10, X10, #8
    MOV X0, X10
    BL _WRITES
    B L_START_Join_17
L_START_Then_3:
    ADRP X9, L_str4@PAGE
    ADD X9, X9, L_str4@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    BL _NEWLINE
    MOV X19, X23
    MOV X0, X19
    BL _SDL2_CREATE_RENDERER
    MOV X25, X0
    MOV X9, X25
    CMP X9, #0
    CSET X10, GT
    CMP X10, XZR
    B.EQ L_START_Join_6
    B L_START_Then_5
L_START_Then_5:
    MOVZ X9, #255
    MOVZ X10, #0
    MOVZ X11, #100
    MOVZ X12, #255
    MOV X0, X25
    MOV X1, X9
    MOV X2, X10
    MOV X3, X11
    MOV X4, X12
    BL _SDL2_SET_DRAW_COLOR
    ADRP X9, L_str5@PAGE
    ADD X9, X9, L_str5@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    BL _NEWLINE
    MOVZ X9, #1
    MOV X24, X9
    MOVZ X9, #0
    MOV X20, X9
    MOVZ X9, #0
    MOV X19, X9
    B L_START_WhileHeader_7
L_START_WhileBody_8:
    BL _SDL2_POLL_EVENT
    STR X0, [X29, #24] ; _opt_temp_0
    MOV X9, X26
    SUB X9, X9, #1
    MOV X26, X9
    MOVZ X9, #1
    MOV X27, X9
    B L_START_ForHeader_10
L_START_WhileExit_9:
    B L_START_Join_6
L_START_WhileHeader_7:
    CMP X24, XZR
    B.EQ L_START_WhileExit_9
    B L_START_WhileBody_8
L_0:
    LDR X19, [X29, #96] ; Restored Reg: X19 @ FP+96
    LDR X20, [X29, #104] ; Restored Reg: X20 @ FP+104
    LDR X21, [X29, #112] ; Restored Reg: X21 @ FP+112
    LDR X22, [X29, #120] ; Restored Reg: X22 @ FP+120
    LDR X23, [X29, #128] ; Restored Reg: X23 @ FP+128
    LDR X24, [X29, #136] ; Restored Reg: X24 @ FP+136
    LDR X25, [X29, #144] ; Restored Reg: X25 @ FP+144
    LDR X26, [X29, #152] ; Restored Reg: X26 @ FP+152
    LDR X27, [X29, #160] ; Restored Reg: X27 @ FP+160
    LDR X28, [X29, #168] ; Restored Reg: X28 @ FP+168
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET

.section __DATA,__const
.p2align 3
L_str0:
    .quad 0x10
    ; (upper half)
    .long 0x50
    .long 0x72
    .long 0x6f
    .long 0x67
    .long 0x72
    .long 0x61
    .long 0x6d
    .long 0x20
    .long 0x63
    .long 0x6f
    .long 0x6d
    .long 0x70
    .long 0x6c
    .long 0x65
    .long 0x74
    .long 0x65
    .long 0x0
    .long 0x0
L_str1:
    .quad 0x10
    ; (upper half)
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
    .long 0x0
    .long 0x0
L_str2:
    .quad 0x9
    ; (upper half)
    .long 0x42
    .long 0x43
    .long 0x50
    .long 0x4c
    .long 0x20
    .long 0x46
    .long 0x65
    .long 0x72
    .long 0x6e
    .long 0x0
    .long 0x0
L_str3:
    .quad 0xc
    ; (upper half)
    .long 0x6f
    .long 0x75
    .long 0x74
    .long 0x20
    .long 0x6f
    .long 0x66
    .long 0x20
    .long 0x66
    .long 0x75
    .long 0x65
    .long 0x6c
    .long 0xa
    .long 0x0
    .long 0x0
L_str4:
    .quad 0xe
    ; (upper half)
    .long 0x57
    .long 0x69
    .long 0x6e
    .long 0x64
    .long 0x6f
    .long 0x77
    .long 0x20
    .long 0x63
    .long 0x72
    .long 0x65
    .long 0x61
    .long 0x74
    .long 0x65
    .long 0x64
    .long 0x0
    .long 0x0
L_str5:
    .quad 0x10
    ; (upper half)
    .long 0x52
    .long 0x65
    .long 0x6e
    .long 0x64
    .long 0x65
    .long 0x72
    .long 0x65
    .long 0x72
    .long 0x20
    .long 0x63
    .long 0x72
    .long 0x65
    .long 0x61
    .long 0x74
    .long 0x65
    .long 0x64
    .long 0x0
    .long 0x0

.section __DATA,__data
.p2align 3
    .long 0x0
    .long 0x0
    .long 0x0
    .long 0x0
L__data_segment_base:
