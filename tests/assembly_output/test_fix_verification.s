.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _START
.globl _WRITEN
.globl _SDL2_SET_DRAW_COLOR
.globl _WRITES
.globl _GET_FREE_LIST_HEAD_ADDR
.globl _BCPL_LIST_APPEND_STRING
.globl _WRITEF
.globl _BCPL_LIST_GET_TAIL
.globl _CONCAT
.globl _WRITEF2
.globl _REVERSE
.globl _WRITEF6
.globl _SDL2_DESTROY_RENDERER
.globl _BCPL_LIST_GET_NTH
.globl _BCPL_CHECK_AND_DISPLAY_ERRORS
.globl _DEEPCOPYLIST
.globl _BCPL_FREE_LIST_SAFE
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
.globl _FEXP
.globl _FPND
.globl _WRITEF4
.globl _NEWLINE
.globl _BCPL_LIST_GET_HEAD_AS_FLOAT
.globl _FWRITE
.globl _SDL2_FILL_RECT
.globl _OBJECT_HEAP_FREE
.globl _BCPL_LIST_GET_HEAD_AS_INT
.globl _RUNTIME_METHOD_LOOKUP
.globl _RETURNNODETOFREELIST
.globl _APND
.globl _FCOS
.globl _PACKSTRING
.globl _BCPL_FREE_LIST
.globl _FILE_CLOSE
.globl _WRITEF3
.globl _BCPL_GET_ATOM_TYPE
.globl _FILE_READS
.globl _FTAN
.globl _FRND
.globl _SPIT
.globl _SPLIT
.globl _SDL2_QUIT
.globl _FILE_WRITES
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
.globl _FILE_OPEN_READ
.globl _SDL2_GET_CURRENT_VIDEO_DRIVER
.globl _SDL2_INIT_SUBSYSTEMS
.globl _FILE_EOF
.globl _STRCMP
.globl _SDL2_GET_VIDEO_DRIVERS
.globl _FINISH
.globl _RDCH
.globl _SDL2_GET_EVENT_MOUSE
.globl _SDL2_INIT
.globl _SDL2_SET_WINDOW_TITLE
.globl _WRITEF7
.globl _SDL2_DESTROY_WINDOW
.globl _FABS
.globl _DEEPCOPYLITERALLIST
.globl _BCPL_LIST_GET_REST
.globl _SDL2_CLEAR
.globl _SDL2_GET_TICKS
.globl _FILE_OPEN_WRITE
.globl _SDL2_CREATE_RENDERER
.globl _SDL2_CLEAR_ERROR
.globl _SDL2_SET_WINDOW_SIZE
.globl _SETTYPE
.globl _FILE_SEEK
.globl _SDL2_TEST_BASIC
.globl _OBJECT_HEAP_ALLOC
.globl _SDL2_GET_VERSION
.globl _SDL2_GET_EVENT_BUTTON
.globl _STRCOPY
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
.globl ___SDL2_STATIC_BUILD
.globl _STRLEN
.globl _SDL2_DRAW_POINT
.globl _SDL2_POLL_EVENT
.globl _FIND
.globl _SDL2_GET_ERROR
.p2align 2
_start:
_START:
    B L_START
L_START:
    STP X29, X30, [SP, #-192]!
    MOV X29, SP
    STR X19, [X29, #112] ; Saved Reg: X19 @ FP+112
    STR X20, [X29, #120] ; Saved Reg: X20 @ FP+120
    STR X21, [X29, #128] ; Saved Reg: X21 @ FP+128
    STR X22, [X29, #136] ; Saved Reg: X22 @ FP+136
    STR X23, [X29, #144] ; Saved Reg: X23 @ FP+144
    STR X24, [X29, #152] ; Saved Reg: X24 @ FP+152
    STR X25, [X29, #160] ; Saved Reg: X25 @ FP+160
    STR X26, [X29, #168] ; Saved Reg: X26 @ FP+168
    STR X27, [X29, #176] ; Saved Reg: X27 @ FP+176
    STR X28, [X29, #184] ; Saved Reg: X28 @ FP+184
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
    MOVZ X9, #29233
    MOV X20, X9
    MOV X0, X20
    LDR X10, [X19, #664]
    BLR X10
    MOV X27, X0
    ADRP X10, L_str4@PAGE
    ADD X10, X10, L_str4@PAGEOFF
    ADD X10, X10, #8
    MOV X21, X10
    MOV X0, X21
    LDR X11, [X19, #680]
    BLR X11
    MOV X27, X0
    MOV X22, X27
    MOV X0, X22
    LDR X11, [X19, #720]
    BLR X11
    MOV X26, X0
    ADRP X11, L_str5@PAGE
    ADD X11, X11, L_str5@PAGEOFF
    ADD X11, X11, #8
    MOV X0, X11
    BL _WRITEF
    MOVZ X9, #0
    MOV X25, X9
    MOVZ X9, #1
    MOV X24, X9
    MOVZ X9, #42
    MOV X23, X9
    ADD X25, X25, X24
    ADD X25, X25, X23
    MOV X22, X25
    ADRP X9, L_str6@PAGE
    ADD X9, X9, L_str6@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    MOV X1, X25
    MOV X2, X24
    MOV X3, X23
    BL _WRITEF3
    ADRP X9, L_str7@PAGE
    ADD X9, X9, L_str7@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    MOV X1, X22
    BL _WRITEF1
    ADRP X9, L_str8@PAGE
    ADD X9, X9, L_str8@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    MOVZ X9, #0
    CMP X26, X9
    CSET X10, GT
    CMP X10, XZR
    B.EQ L_START_Else_2
    B L_START_Then_1
L_START_BlockCleanup_10:
    B L_START_Exit_11
L_START_BlockCleanup_4:
    B L_START_Join_3
L_START_BlockCleanup_9:
    B L_START_ForIncrement_7
L_START_Else_2:
    ADRP X9, L_str9@PAGE
    ADD X9, X9, L_str9@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    B L_START_Join_3
L_START_Exit_11:
    B L_0
L_START_ForBody_6:
    MOV X0, X26
    MOVZ X9, #50
    MUL X23, X23, X9
    MOV X1, X23
    MOVZ X9, #0
    MOV X2, X9
    MOVZ X9, #0
    MOV X3, X9
    MOVZ X9, #255
    MOV X4, X9
    BL _SDL2_SET_DRAW_COLOR
    MOV X0, X26
    BL _SDL2_CLEAR
    MOV X22, X23
    ADRP X9, L_str10@PAGE
    ADD X9, X9, L_str10@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    MOV X1, X22
    BL _WRITEF1
    B L_START_BlockCleanup_9
L_START_ForExit_8:
    ADRP X9, L_str11@PAGE
    ADD X9, X9, L_str11@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    MOV X0, X26
    BL _SDL2_DESTROY_RENDERER
    MOV X0, X27
    BL _SDL2_DESTROY_WINDOW
    BL _SDL2_QUIT
    MOVZ X9, #1
    MOV X24, X9
    ADRP X9, L_str12@PAGE
    ADD X9, X9, L_str12@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    MOV X1, X24
    BL _WRITEF1
    ADRP X9, L_str13@PAGE
    ADD X9, X9, L_str13@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str14@PAGE
    ADD X9, X9, L_str14@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str15@PAGE
    ADD X9, X9, L_str15@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str16@PAGE
    ADD X9, X9, L_str16@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str17@PAGE
    ADD X9, X9, L_str17@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str18@PAGE
    ADD X9, X9, L_str18@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str19@PAGE
    ADD X9, X9, L_str19@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str20@PAGE
    ADD X9, X9, L_str20@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str21@PAGE
    ADD X9, X9, L_str21@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str22@PAGE
    ADD X9, X9, L_str22@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str23@PAGE
    ADD X9, X9, L_str23@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str24@PAGE
    ADD X9, X9, L_str24@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str25@PAGE
    ADD X9, X9, L_str25@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str26@PAGE
    ADD X9, X9, L_str26@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str27@PAGE
    ADD X9, X9, L_str27@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str28@PAGE
    ADD X9, X9, L_str28@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str29@PAGE
    ADD X9, X9, L_str29@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str30@PAGE
    ADD X9, X9, L_str30@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str31@PAGE
    ADD X9, X9, L_str31@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    B L_START_BlockCleanup_10
L_START_ForHeader_5:
    MOV X9, X23
    MOVZ X10, #3
    CMP X9, X10
    B.GT L_START_ForExit_8
    B L_START_ForBody_6
L_START_ForIncrement_7:
    MOVZ X9, #1
    ADD X23, X23, X9
    B L_START_ForHeader_5
L_START_Join_3:
    ADRP X9, L_str32@PAGE
    ADD X9, X9, L_str32@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    MOVZ X9, #1
    MOV X23, X9
    B L_START_ForHeader_5
L_START_Then_1:
    MOV X0, X26
    MOVZ X9, #255
    MOV X1, X9
    MOVZ X9, #0
    MOV X2, X9
    MOVZ X9, #0
    MOV X3, X9
    MOVZ X9, #255
    MOV X4, X9
    BL _SDL2_SET_DRAW_COLOR
    MOV X0, X26
    BL _SDL2_CLEAR
    MOV X0, X26
    BL _SDL2_PRESENT
    MOVZ X9, #255
    MOV X23, X9
    MOVZ X9, #1
    MOV X22, X9
    ADRP X9, L_str33@PAGE
    ADD X9, X9, L_str33@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    MOV X1, X23
    MOV X2, X22
    BL _WRITEF2
    B L_START_BlockCleanup_4
L_0:
    LDR X19, [X29, #112] ; Restored Reg: X19 @ FP+112
    LDR X20, [X29, #120] ; Restored Reg: X20 @ FP+120
    LDR X21, [X29, #128] ; Restored Reg: X21 @ FP+128
    LDR X22, [X29, #136] ; Restored Reg: X22 @ FP+136
    LDR X23, [X29, #144] ; Restored Reg: X23 @ FP+144
    LDR X24, [X29, #152] ; Restored Reg: X24 @ FP+152
    LDR X25, [X29, #160] ; Restored Reg: X25 @ FP+160
    LDR X26, [X29, #168] ; Restored Reg: X26 @ FP+168
    LDR X27, [X29, #176] ; Restored Reg: X27 @ FP+176
    LDR X28, [X29, #184] ; Restored Reg: X28 @ FP+184
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET

.section __DATA,__const
.p2align 3
L_str0:
    .quad 0x2c
    ; (upper half)
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x20
    .long 0x4c
    .long 0x69
    .long 0x76
    .long 0x65
    .long 0x6e
    .long 0x65
    .long 0x73
    .long 0x73
    .long 0x20
    .long 0x41
    .long 0x6e
    .long 0x61
    .long 0x6c
    .long 0x79
    .long 0x73
    .long 0x69
    .long 0x73
    .long 0x20
    .long 0x46
    .long 0x69
    .long 0x78
    .long 0x20
    .long 0x56
    .long 0x65
    .long 0x72
    .long 0x69
    .long 0x66
    .long 0x69
    .long 0x63
    .long 0x61
    .long 0x74
    .long 0x69
    .long 0x6f
    .long 0x6e
    .long 0x20
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0
L_str1:
    .quad 0x31
    ; (upper half)
    .long 0x54
    .long 0x68
    .long 0x69
    .long 0x73
    .long 0x20
    .long 0x74
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x20
    .long 0x64
    .long 0x65
    .long 0x6d
    .long 0x6f
    .long 0x6e
    .long 0x73
    .long 0x74
    .long 0x72
    .long 0x61
    .long 0x74
    .long 0x65
    .long 0x73
    .long 0x20
    .long 0x74
    .long 0x68
    .long 0x61
    .long 0x74
    .long 0x20
    .long 0x53
    .long 0x44
    .long 0x4c
    .long 0x32
    .long 0x20
    .long 0x66
    .long 0x75
    .long 0x6e
    .long 0x63
    .long 0x74
    .long 0x69
    .long 0x6f
    .long 0x6e
    .long 0x20
    .long 0x6e
    .long 0x61
    .long 0x6d
    .long 0x65
    .long 0x73
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0
L_str2:
    .quad 0x2c
    ; (upper half)
    .long 0x61
    .long 0x72
    .long 0x65
    .long 0x20
    .long 0x6e
    .long 0x6f
    .long 0x20
    .long 0x6c
    .long 0x6f
    .long 0x6e
    .long 0x67
    .long 0x65
    .long 0x72
    .long 0x20
    .long 0x74
    .long 0x72
    .long 0x65
    .long 0x61
    .long 0x74
    .long 0x65
    .long 0x64
    .long 0x20
    .long 0x61
    .long 0x73
    .long 0x20
    .long 0x6c
    .long 0x69
    .long 0x76
    .long 0x65
    .long 0x20
    .long 0x76
    .long 0x61
    .long 0x72
    .long 0x69
    .long 0x61
    .long 0x62
    .long 0x6c
    .long 0x65
    .long 0x73
    .long 0x2e
    .long 0x25
    .long 0x4e
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0
L_str3:
    .quad 0x29
    ; (upper half)
    .long 0x31
    .long 0x2e
    .long 0x20
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x62
    .long 0x61
    .long 0x73
    .long 0x69
    .long 0x63
    .long 0x20
    .long 0x53
    .long 0x44
    .long 0x4c
    .long 0x32
    .long 0x20
    .long 0x66
    .long 0x75
    .long 0x6e
    .long 0x63
    .long 0x74
    .long 0x69
    .long 0x6f
    .long 0x6e
    .long 0x20
    .long 0x63
    .long 0x61
    .long 0x6c
    .long 0x6c
    .long 0x73
    .long 0x2e
    .long 0x2e
    .long 0x2e
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0
L_str4:
    .quad 0xb
    ; (upper half)
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x20
    .long 0x57
    .long 0x69
    .long 0x6e
    .long 0x64
    .long 0x6f
    .long 0x77
    .long 0x0
    .long 0x0
L_str5:
    .quad 0x28
    ; (upper half)
    .long 0x32
    .long 0x2e
    .long 0x20
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x61
    .long 0x63
    .long 0x74
    .long 0x75
    .long 0x61
    .long 0x6c
    .long 0x20
    .long 0x70
    .long 0x72
    .long 0x6f
    .long 0x67
    .long 0x72
    .long 0x61
    .long 0x6d
    .long 0x20
    .long 0x76
    .long 0x61
    .long 0x72
    .long 0x69
    .long 0x61
    .long 0x62
    .long 0x6c
    .long 0x65
    .long 0x73
    .long 0x2e
    .long 0x2e
    .long 0x2e
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0
L_str6:
    .quad 0x33
    ; (upper half)
    .long 0x56
    .long 0x61
    .long 0x72
    .long 0x69
    .long 0x61
    .long 0x62
    .long 0x6c
    .long 0x65
    .long 0x73
    .long 0x3a
    .long 0x20
    .long 0x63
    .long 0x6f
    .long 0x75
    .long 0x6e
    .long 0x74
    .long 0x65
    .long 0x72
    .long 0x3d
    .long 0x25
    .long 0x64
    .long 0x2c
    .long 0x20
    .long 0x6c
    .long 0x6f
    .long 0x6f
    .long 0x70
    .long 0x5f
    .long 0x76
    .long 0x61
    .long 0x72
    .long 0x3d
    .long 0x25
    .long 0x64
    .long 0x2c
    .long 0x20
    .long 0x74
    .long 0x65
    .long 0x6d
    .long 0x70
    .long 0x5f
    .long 0x76
    .long 0x61
    .long 0x6c
    .long 0x75
    .long 0x65
    .long 0x3d
    .long 0x25
    .long 0x64
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0
L_str7:
    .quad 0x9
    ; (upper half)
    .long 0x53
    .long 0x75
    .long 0x6d
    .long 0x3a
    .long 0x20
    .long 0x25
    .long 0x64
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0
L_str8:
    .quad 0x30
    ; (upper half)
    .long 0x33
    .long 0x2e
    .long 0x20
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x53
    .long 0x44
    .long 0x4c
    .long 0x32
    .long 0x20
    .long 0x63
    .long 0x61
    .long 0x6c
    .long 0x6c
    .long 0x73
    .long 0x20
    .long 0x69
    .long 0x6e
    .long 0x20
    .long 0x63
    .long 0x6f
    .long 0x6e
    .long 0x74
    .long 0x72
    .long 0x6f
    .long 0x6c
    .long 0x20
    .long 0x73
    .long 0x74
    .long 0x72
    .long 0x75
    .long 0x63
    .long 0x74
    .long 0x75
    .long 0x72
    .long 0x65
    .long 0x73
    .long 0x2e
    .long 0x2e
    .long 0x2e
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0
L_str9:
    .quad 0x1b
    ; (upper half)
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
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0
L_str10:
    .quad 0x13
    ; (upper half)
    .long 0x46
    .long 0x72
    .long 0x61
    .long 0x6d
    .long 0x65
    .long 0x20
    .long 0x25
    .long 0x64
    .long 0x20
    .long 0x72
    .long 0x65
    .long 0x6e
    .long 0x64
    .long 0x65
    .long 0x72
    .long 0x65
    .long 0x64
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0
L_str11:
    .quad 0x1d
    ; (upper half)
    .long 0x35
    .long 0x2e
    .long 0x20
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
    .long 0x73
    .long 0x6f
    .long 0x75
    .long 0x72
    .long 0x63
    .long 0x65
    .long 0x73
    .long 0x2e
    .long 0x2e
    .long 0x2e
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0
L_str12:
    .quad 0x22
    ; (upper half)
    .long 0x43
    .long 0x6c
    .long 0x65
    .long 0x61
    .long 0x6e
    .long 0x75
    .long 0x70
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
    .long 0x77
    .long 0x69
    .long 0x74
    .long 0x68
    .long 0x20
    .long 0x73
    .long 0x74
    .long 0x61
    .long 0x74
    .long 0x75
    .long 0x73
    .long 0x3a
    .long 0x20
    .long 0x25
    .long 0x64
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0
L_str13:
    .quad 0x2e
    ; (upper half)
    .long 0x25
    .long 0x4e
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x20
    .long 0x45
    .long 0x78
    .long 0x70
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x65
    .long 0x64
    .long 0x20
    .long 0x4c
    .long 0x69
    .long 0x76
    .long 0x65
    .long 0x6e
    .long 0x65
    .long 0x73
    .long 0x73
    .long 0x20
    .long 0x41
    .long 0x6e
    .long 0x61
    .long 0x6c
    .long 0x79
    .long 0x73
    .long 0x69
    .long 0x73
    .long 0x20
    .long 0x52
    .long 0x65
    .long 0x73
    .long 0x75
    .long 0x6c
    .long 0x74
    .long 0x73
    .long 0x20
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0
L_str14:
    .quad 0x22
    ; (upper half)
    .long 0x53
    .long 0x48
    .long 0x4f
    .long 0x55
    .long 0x4c
    .long 0x44
    .long 0x20
    .long 0x61
    .long 0x70
    .long 0x70
    .long 0x65
    .long 0x61
    .long 0x72
    .long 0x20
    .long 0x69
    .long 0x6e
    .long 0x20
    .long 0x6c
    .long 0x69
    .long 0x76
    .long 0x65
    .long 0x20
    .long 0x69
    .long 0x6e
    .long 0x74
    .long 0x65
    .long 0x72
    .long 0x76
    .long 0x61
    .long 0x6c
    .long 0x73
    .long 0x3a
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0
L_str15:
    .quad 0x21
    ; (upper half)
    .long 0x2d
    .long 0x20
    .long 0x69
    .long 0x6e
    .long 0x69
    .long 0x74
    .long 0x5f
    .long 0x72
    .long 0x65
    .long 0x73
    .long 0x75
    .long 0x6c
    .long 0x74
    .long 0x2c
    .long 0x20
    .long 0x77
    .long 0x69
    .long 0x6e
    .long 0x64
    .long 0x6f
    .long 0x77
    .long 0x2c
    .long 0x20
    .long 0x72
    .long 0x65
    .long 0x6e
    .long 0x64
    .long 0x65
    .long 0x72
    .long 0x65
    .long 0x72
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0
L_str16:
    .quad 0x2d
    ; (upper half)
    .long 0x2d
    .long 0x20
    .long 0x63
    .long 0x6f
    .long 0x75
    .long 0x6e
    .long 0x74
    .long 0x65
    .long 0x72
    .long 0x2c
    .long 0x20
    .long 0x6c
    .long 0x6f
    .long 0x6f
    .long 0x70
    .long 0x5f
    .long 0x76
    .long 0x61
    .long 0x72
    .long 0x2c
    .long 0x20
    .long 0x74
    .long 0x65
    .long 0x6d
    .long 0x70
    .long 0x5f
    .long 0x76
    .long 0x61
    .long 0x6c
    .long 0x75
    .long 0x65
    .long 0x2c
    .long 0x20
    .long 0x72
    .long 0x65
    .long 0x73
    .long 0x75
    .long 0x6c
    .long 0x74
    .long 0x5f
    .long 0x73
    .long 0x75
    .long 0x6d
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0
L_str17:
    .quad 0x23
    ; (upper half)
    .long 0x2d
    .long 0x20
    .long 0x72
    .long 0x65
    .long 0x64
    .long 0x5f
    .long 0x63
    .long 0x6f
    .long 0x6d
    .long 0x70
    .long 0x6f
    .long 0x6e
    .long 0x65
    .long 0x6e
    .long 0x74
    .long 0x2c
    .long 0x20
    .long 0x64
    .long 0x72
    .long 0x61
    .long 0x77
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x5f
    .long 0x63
    .long 0x6f
    .long 0x6d
    .long 0x70
    .long 0x6c
    .long 0x65
    .long 0x74
    .long 0x65
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0
L_str18:
    .quad 0x13
    ; (upper half)
    .long 0x2d
    .long 0x20
    .long 0x69
    .long 0x2c
    .long 0x20
    .long 0x66
    .long 0x72
    .long 0x61
    .long 0x6d
    .long 0x65
    .long 0x5f
    .long 0x6e
    .long 0x75
    .long 0x6d
    .long 0x62
    .long 0x65
    .long 0x72
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0
L_str19:
    .quad 0x12
    ; (upper half)
    .long 0x2d
    .long 0x20
    .long 0x63
    .long 0x6c
    .long 0x65
    .long 0x61
    .long 0x6e
    .long 0x75
    .long 0x70
    .long 0x5f
    .long 0x73
    .long 0x74
    .long 0x61
    .long 0x74
    .long 0x75
    .long 0x73
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0
L_str20:
    .quad 0x2
    ; (upper half)
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0
L_str21:
    .quad 0x26
    ; (upper half)
    .long 0x73
    .long 0x68
    .long 0x6f
    .long 0x75
    .long 0x6c
    .long 0x64
    .long 0x20
    .long 0x4e
    .long 0x4f
    .long 0x54
    .long 0x20
    .long 0x61
    .long 0x70
    .long 0x70
    .long 0x65
    .long 0x61
    .long 0x72
    .long 0x20
    .long 0x69
    .long 0x6e
    .long 0x20
    .long 0x6c
    .long 0x69
    .long 0x76
    .long 0x65
    .long 0x20
    .long 0x69
    .long 0x6e
    .long 0x74
    .long 0x65
    .long 0x72
    .long 0x76
    .long 0x61
    .long 0x6c
    .long 0x73
    .long 0x3a
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0
L_str22:
    .quad 0x18
    ; (upper half)
    .long 0x2d
    .long 0x20
    .long 0x53
    .long 0x44
    .long 0x4c
    .long 0x32
    .long 0x5f
    .long 0x49
    .long 0x4e
    .long 0x49
    .long 0x54
    .long 0x5f
    .long 0x53
    .long 0x55
    .long 0x42
    .long 0x53
    .long 0x59
    .long 0x53
    .long 0x54
    .long 0x45
    .long 0x4d
    .long 0x53
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0
L_str23:
    .quad 0x16
    ; (upper half)
    .long 0x2d
    .long 0x20
    .long 0x53
    .long 0x44
    .long 0x4c
    .long 0x32
    .long 0x5f
    .long 0x43
    .long 0x52
    .long 0x45
    .long 0x41
    .long 0x54
    .long 0x45
    .long 0x5f
    .long 0x57
    .long 0x49
    .long 0x4e
    .long 0x44
    .long 0x4f
    .long 0x57
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0
L_str24:
    .quad 0x18
    ; (upper half)
    .long 0x2d
    .long 0x20
    .long 0x53
    .long 0x44
    .long 0x4c
    .long 0x32
    .long 0x5f
    .long 0x43
    .long 0x52
    .long 0x45
    .long 0x41
    .long 0x54
    .long 0x45
    .long 0x5f
    .long 0x52
    .long 0x45
    .long 0x4e
    .long 0x44
    .long 0x45
    .long 0x52
    .long 0x45
    .long 0x52
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0
L_str25:
    .quad 0x17
    ; (upper half)
    .long 0x2d
    .long 0x20
    .long 0x53
    .long 0x44
    .long 0x4c
    .long 0x32
    .long 0x5f
    .long 0x53
    .long 0x45
    .long 0x54
    .long 0x5f
    .long 0x44
    .long 0x52
    .long 0x41
    .long 0x57
    .long 0x5f
    .long 0x43
    .long 0x4f
    .long 0x4c
    .long 0x4f
    .long 0x52
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0
L_str26:
    .quad 0xe
    ; (upper half)
    .long 0x2d
    .long 0x20
    .long 0x53
    .long 0x44
    .long 0x4c
    .long 0x32
    .long 0x5f
    .long 0x43
    .long 0x4c
    .long 0x45
    .long 0x41
    .long 0x52
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0
L_str27:
    .quad 0x10
    ; (upper half)
    .long 0x2d
    .long 0x20
    .long 0x53
    .long 0x44
    .long 0x4c
    .long 0x32
    .long 0x5f
    .long 0x50
    .long 0x52
    .long 0x45
    .long 0x53
    .long 0x45
    .long 0x4e
    .long 0x54
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0
L_str28:
    .quad 0x19
    ; (upper half)
    .long 0x2d
    .long 0x20
    .long 0x53
    .long 0x44
    .long 0x4c
    .long 0x32
    .long 0x5f
    .long 0x44
    .long 0x45
    .long 0x53
    .long 0x54
    .long 0x52
    .long 0x4f
    .long 0x59
    .long 0x5f
    .long 0x52
    .long 0x45
    .long 0x4e
    .long 0x44
    .long 0x45
    .long 0x52
    .long 0x45
    .long 0x52
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0
L_str29:
    .quad 0x17
    ; (upper half)
    .long 0x2d
    .long 0x20
    .long 0x53
    .long 0x44
    .long 0x4c
    .long 0x32
    .long 0x5f
    .long 0x44
    .long 0x45
    .long 0x53
    .long 0x54
    .long 0x52
    .long 0x4f
    .long 0x59
    .long 0x5f
    .long 0x57
    .long 0x49
    .long 0x4e
    .long 0x44
    .long 0x4f
    .long 0x57
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0
L_str30:
    .quad 0xd
    ; (upper half)
    .long 0x2d
    .long 0x20
    .long 0x53
    .long 0x44
    .long 0x4c
    .long 0x32
    .long 0x5f
    .long 0x51
    .long 0x55
    .long 0x49
    .long 0x54
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0
L_str31:
    .quad 0x19
    ; (upper half)
    .long 0x25
    .long 0x4e
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x20
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x20
    .long 0x43
    .long 0x6f
    .long 0x6d
    .long 0x70
    .long 0x6c
    .long 0x65
    .long 0x74
    .long 0x65
    .long 0x20
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0
L_str32:
    .quad 0x23
    ; (upper half)
    .long 0x34
    .long 0x2e
    .long 0x20
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x53
    .long 0x44
    .long 0x4c
    .long 0x32
    .long 0x20
    .long 0x63
    .long 0x61
    .long 0x6c
    .long 0x6c
    .long 0x73
    .long 0x20
    .long 0x69
    .long 0x6e
    .long 0x20
    .long 0x6c
    .long 0x6f
    .long 0x6f
    .long 0x70
    .long 0x73
    .long 0x2e
    .long 0x2e
    .long 0x2e
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0
L_str33:
    .quad 0x22
    ; (upper half)
    .long 0x44
    .long 0x72
    .long 0x61
    .long 0x77
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x77
    .long 0x69
    .long 0x74
    .long 0x68
    .long 0x20
    .long 0x72
    .long 0x65
    .long 0x64
    .long 0x3d
    .long 0x25
    .long 0x64
    .long 0x2c
    .long 0x20
    .long 0x63
    .long 0x6f
    .long 0x6d
    .long 0x70
    .long 0x6c
    .long 0x65
    .long 0x74
    .long 0x65
    .long 0x3d
    .long 0x25
    .long 0x64
    .long 0x25
    .long 0x4e
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
    .long 0x0
    .long 0x0
    .long 0x0
    .long 0x0
    .long 0x0
    .long 0x0
    .long 0x0
    .long 0x0
    .long 0x0
L__data_segment_base:
