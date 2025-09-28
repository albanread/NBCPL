.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _START
.globl _WRITEN
.globl _SDL2_SET_DRAW_COLOR
.globl _WRITES
.globl _GET_FREE_LIST_HEAD_ADDR
.globl _BCPL_LIST_APPEND_STRING
.globl _WRITEF
.globl _CONCAT
.globl _WRITEF2
.globl _WRITEF6
.globl _SDL2_DESTROY_RENDERER
.globl _BCPL_LIST_GET_NTH
.globl _FILE_OPEN_READ
.globl _BCPL_CHECK_AND_DISPLAY_ERRORS
.globl _SDL2_DELAY
.globl _FREEVEC
.globl _UNPACKSTRING
.globl _SDL2_PRESENT
.globl _BCPL_GET_LAST_ERROR
.globl _BCPL_LIST_APPEND_INT
.globl _LPND
.globl _MALLOC
.globl _BCPL_FREE_CELLS
.globl _JOIN
.globl _BCPL_LIST_CREATE_EMPTY
.globl _WRITEC
.globl _FILE_READ
.globl _BCPL_CONCAT_LISTS
.globl _SDL2_GET_EVENT_KEY
.globl _BCPL_LIST_APPEND_FLOAT
.globl _FPND
.globl _FEXP
.globl _WRITEF4
.globl _RUNTIME_METHOD_LOOKUP
.globl _BCPL_LIST_GET_HEAD_AS_INT
.globl _NEWLINE
.globl _BCPL_LIST_GET_HEAD_AS_FLOAT
.globl _FWRITE
.globl _SDL2_FILL_RECT
.globl _OBJECT_HEAP_FREE
.globl _RETURNNODETOFREELIST
.globl _APND
.globl _FCOS
.globl _PACKSTRING
.globl _BCPL_FREE_LIST_SAFE
.globl _DEEPCOPYLIST
.globl _BCPL_FREE_LIST
.globl _FILE_CLOSE
.globl _FIND
.globl _WRITEF3
.globl _BCPL_GET_ATOM_TYPE
.globl _FILE_READS
.globl _PIC_RUNTIME_HELPER
.globl _SDL2_CREATE_WINDOW
.globl _SLURP
.globl _RAND
.globl _FILE_TELL
.globl _FILE_OPEN_APPEND
.globl _SDL2_CREATE_WINDOW_EX
.globl _FSIN
.globl _BCPL_ALLOC_WORDS
.globl _BCPL_ALLOC_CHARS
.globl _WRITEF5
.globl _SDL2_GET_CURRENT_VIDEO_DRIVER
.globl _SDL2_INIT_SUBSYSTEMS
.globl _FTAN
.globl _SPIT
.globl _FRND
.globl _OBJECT_HEAP_ALLOC
.globl _SDL2_TEST_BASIC
.globl _BCPL_LIST_GET_TAIL
.globl _REVERSE
.globl _BCPL_BOUNDS_ERROR
.globl _FILE_EOF
.globl _STRCMP
.globl _SDL2_GET_VIDEO_DRIVERS
.globl _FINISH
.globl _RDCH
.globl _SDL2_GET_EVENT_MOUSE
.globl _SPLIT
.globl _FILE_WRITES
.globl _SDL2_QUIT
.globl _SDL2_INIT
.globl _SDL2_SET_WINDOW_TITLE
.globl _WRITEF7
.globl _SDL2_DESTROY_WINDOW
.globl _FABS
.globl _DEEPCOPYLITERALLIST
.globl _BCPL_LIST_GET_REST
.globl _SDL2_CLEAR
.globl _SDL2_CREATE_RENDERER
.globl _SDL2_CLEAR_ERROR
.globl _SDL2_SET_WINDOW_SIZE
.globl _SETTYPE
.globl _FILE_SEEK
.globl _SDL2_GET_VERSION
.globl _SDL2_GET_EVENT_BUTTON
.globl _STRCOPY
.globl _FIX
.globl _WRITEF1
.globl _FILE_WRITE
.globl _COPYLIST
.globl _SDL2_DRAW_RECT
.globl _SDL2_GET_DISPLAY_MODES
.globl _SPND
.globl _SDL2_CREATE_RENDERER_EX
.globl _SDL2_DRAW_LINE
.globl _BCPL_CLEAR_ERRORS
.globl _FLOG
.globl _FILTER
.globl _FILE_OPEN_WRITE
.globl _SDL2_GET_TICKS
.globl ___SDL2_STATIC_BUILD
.globl _STRLEN
.globl _SDL2_DRAW_POINT
.globl _SDL2_POLL_EVENT
.globl _SDL2_GET_ERROR
.p2align 2
_start:
_START:
    B L_START
L_START:
    STP X29, X30, [SP, #-144]!
    MOV X29, SP
    STR X19, [X29, #64] ; Saved Reg: X19 @ FP+64
    STR X20, [X29, #72] ; Saved Reg: X20 @ FP+72
    STR X21, [X29, #80] ; Saved Reg: X21 @ FP+80
    STR X22, [X29, #88] ; Saved Reg: X22 @ FP+88
    STR X23, [X29, #96] ; Saved Reg: X23 @ FP+96
    STR X24, [X29, #104] ; Saved Reg: X24 @ FP+104
    STR X25, [X29, #112] ; Saved Reg: X25 @ FP+112
    STR X26, [X29, #120] ; Saved Reg: X26 @ FP+120
    STR X27, [X29, #128] ; Saved Reg: X27 @ FP+128
    STR X28, [X29, #136] ; Saved Reg: X28 @ FP+136
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
    MOVZ X9, #8, LSL #16
    ADD X19, X28, X9
L_START_Entry_0:
    ADRP X9, L_str0@PAGE
    ADD X9, X9, L_str0@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str1@PAGE
    ADD X9, X9, L_str1@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    MOVZ X9, #29233
    MOV X20, X9
    MOV X0, X20
    LDR X10, [X19, #680]
    BLR X10
    MOV X15, X0
    MOV X10, X15
    CMP X10, #0
    CSET X11, EQ
    CMP X11, XZR
    B.EQ L_START_Else_2
    B L_START_Then_1
L_START_Else_2:
    ADRP X10, L_str2@PAGE
    ADD X10, X10, L_str2@PAGEOFF
    ADD X10, X10, #8
    MOV X0, X10
    BL _WRITEF
    B L_START_FinishCleanup_4
L_START_Else_6:
    ADRP X9, L_str3@PAGE
    ADD X9, X9, L_str3@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    B L_START_Join_7
L_START_Else_9:
    ADRP X9, L_str4@PAGE
    ADD X9, X9, L_str4@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    B L_START_Join_10
L_START_Exit_16:
    B L_0
L_START_FinishCleanup_4:
    MOVZ X9, #0
    MOV X0, X9
    MOVZ X9, #1
    MOVK X9, #512, LSL #16
    MOV X16, X9
    SVC #128
L_START_Join_10:
    B L_START_Join_7
L_START_Join_15:
    MOV X20, X24
    MOVZ X9, #30
    MOV X21, X9
    MOVZ X9, #30
    MOV X22, X9
    MOVZ X9, #50
    MOV X23, X9
    MOVZ X9, #255
    MOV X24, X9
    MOV X0, X20
    MOV X1, X21
    MOV X2, X22
    MOV X3, X23
    MOV X4, X24
    BL _SDL2_SET_DRAW_COLOR
    MOV X20, X24
    MOV X0, X20
    BL _SDL2_CLEAR
    MOV X20, X24
    MOV X0, X20
    BL _SDL2_PRESENT
    MOVZ X9, #16
    MOV X20, X9
    MOV X0, X20
    BL _SDL2_DELAY
    B L_START_WhileHeader_11
L_START_Join_3:
    ADRP X9, L_str5@PAGE
    ADD X9, X9, L_str5@PAGEOFF
    ADD X9, X9, #8
    MOV X20, X9
    MOV X0, X20
    LDR X10, [X19, #696]
    BLR X10
    MOV X27, X0
    MOV X10, X27
    CMP X10, #0
    CSET X11, GT
    CMP X11, XZR
    B.EQ L_START_Else_6
    B L_START_Then_5
L_START_Join_7:
    BL _SDL2_QUIT
    ADRP X9, L_str6@PAGE
    ADD X9, X9, L_str6@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    B L_START_Exit_16
L_START_Then_1:
    ADRP X9, L_str7@PAGE
    ADD X9, X9, L_str7@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    B L_START_Join_3
L_START_Then_14:
    MOVZ X9, #0
    MOV X23, X9
    B L_START_Join_15
L_START_Then_5:
    MOV X20, X27
    MOV X0, X20
    LDR X9, [X19, #736]
    BLR X9
    MOV X24, X0
    MOV X9, X24
    CMP X9, #0
    CSET X10, GT
    CMP X10, XZR
    B.EQ L_START_Else_9
    B L_START_Then_8
L_START_Then_8:
    MOVZ X9, #1
    MOV X23, X9
    ADRP X9, L_str8@PAGE
    ADD X9, X9, L_str8@PAGEOFF
    ADD X9, X9, #8
    MOV X20, X9
    MOV X0, X20
    BL _WRITES
    B L_START_WhileHeader_11
L_START_WhileBody_12:
    LDR X9, [X19, #816]
    BLR X9
    MOV X26, X0
    MOV X25, X26
    MOV X9, X25
    CMP X9, #256
    CSET X10, EQ
    CMP X10, XZR
    B.EQ L_START_Join_15
    B L_START_Then_14
L_START_WhileExit_13:
    ADRP X9, L_str9@PAGE
    ADD X9, X9, L_str9@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    MOV X20, X24
    MOV X0, X20
    BL _SDL2_DESTROY_RENDERER
    MOV X20, X27
    MOV X0, X20
    BL _SDL2_DESTROY_WINDOW
    B L_START_Join_10
L_START_WhileHeader_11:
    CMP X23, XZR
    B.EQ L_START_WhileExit_13
    B L_START_WhileBody_12
L_0:
    LDR X19, [X29, #64] ; Restored Reg: X19 @ FP+64
    LDR X20, [X29, #72] ; Restored Reg: X20 @ FP+72
    LDR X21, [X29, #80] ; Restored Reg: X21 @ FP+80
    LDR X22, [X29, #88] ; Restored Reg: X22 @ FP+88
    LDR X23, [X29, #96] ; Restored Reg: X23 @ FP+96
    LDR X24, [X29, #104] ; Restored Reg: X24 @ FP+104
    LDR X25, [X29, #112] ; Restored Reg: X25 @ FP+112
    LDR X26, [X29, #120] ; Restored Reg: X26 @ FP+120
    LDR X27, [X29, #128] ; Restored Reg: X27 @ FP+128
    LDR X28, [X29, #136] ; Restored Reg: X28 @ FP+136
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET

.section __DATA,__const
.p2align 3
L_str0:
    .quad 0x14
    ; (upper half)
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
L_str1:
    .quad 0x14
    ; (upper half)
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
L_str2:
    .quad 0x20
    ; (upper half)
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
L_str3:
    .quad 0x1b
    ; (upper half)
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
L_str4:
    .quad 0x1d
    ; (upper half)
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
L_str5:
    .quad 0x10
    ; (upper half)
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
L_str6:
    .quad 0x1a
    ; (upper half)
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
L_str7:
    .quad 0x21
    ; (upper half)
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
L_str8:
    .quad 0x9
    ; (upper half)
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
L_str9:
    .quad 0x23
    ; (upper half)
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
L__data_segment_base:
