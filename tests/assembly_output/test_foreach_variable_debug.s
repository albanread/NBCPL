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
.globl _WRITEF6
.globl _SDL2_DESTROY_RENDERER
.globl _BCPL_LIST_GET_NTH
.globl _FILE_OPEN_READ
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
.globl _SDL2_GET_CURRENT_VIDEO_DRIVER
.globl _SDL2_INIT_SUBSYSTEMS
.globl _REVERSE
.globl _BCPL_BOUNDS_ERROR
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
    STP X29, X30, [SP, #-208]!
    MOV X29, SP
    STR X19, [X29, #128] ; Saved Reg: X19 @ FP+128
    STR X20, [X29, #136] ; Saved Reg: X20 @ FP+136
    STR X21, [X29, #144] ; Saved Reg: X21 @ FP+144
    STR X22, [X29, #152] ; Saved Reg: X22 @ FP+152
    STR X24, [X29, #160] ; Saved Reg: X24 @ FP+160
    STR X25, [X29, #168] ; Saved Reg: X25 @ FP+168
    STR X26, [X29, #176] ; Saved Reg: X26 @ FP+176
    STR X27, [X29, #184] ; Saved Reg: X27 @ FP+184
    STR X28, [X29, #192] ; Saved Reg: X28 @ FP+192
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
    MOV X27, X9
    ADRP X9, L_str3@PAGE
    ADD X9, X9, L_str3@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str4@PAGE
    ADD X9, X9, L_str4@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    SUB X10, X27, #8
    LDR X9, [X10, #0] ; Load vector/table/string length
    MOV X26, X9
    MOVZ X9, #0
    MOV X25, X9
    B L_START_ForEachHeader_1
L_START_BlockCleanup_17:
    B L_START_Exit_18
L_START_Exit_18:
    B L_0
L_START_ForEachBody_10:
    SUB X10, X26, #8
    LDR X9, [X10, #0] ; Load string length for bounds check
    CMP X27, X9
    B.HS L__bounds_error_handler_START
    MOV X9, X27
    LSL X9, X9, #2
    ADD X10, X26, X9
    LDR W9, [X10]
    MOV X24, X9
    ADRP X9, L_str5@PAGE
    ADD X9, X9, L_str5@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    MOV X1, X24
    BL _WRITEF1
    B L_START_ForEachIncrement_11
L_START_ForEachBody_14:
    SUB X10, X27, #8
    LDR X9, [X10, #0] ; Load string length for bounds check
    CMP X26, X9
    B.HS L__bounds_error_handler_START
    MOV X9, X26
    LSL X9, X9, #2
    ADD X10, X27, X9
    LDR W9, [X10]
    MOV X24, X9
    ADRP X9, L_str5@PAGE
    ADD X9, X9, L_str5@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    MOV X1, X24
    BL _WRITEF1
    B L_START_ForEachIncrement_15
L_START_ForEachBody_2:
    SUB X10, X27, #8
    LDR X9, [X10, #0] ; Load string length for bounds check
    CMP X25, X9
    B.HS L__bounds_error_handler_START
    MOV X9, X25
    LSL X9, X9, #2
    ADD X10, X27, X9
    LDR W9, [X10]
    MOV X24, X9
    ADRP X9, L_str5@PAGE
    ADD X9, X9, L_str5@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    MOV X1, X24
    BL _WRITEF1
    B L_START_ForEachIncrement_3
L_START_ForEachBody_6:
    SUB X10, X25, #8
    LDR X9, [X10, #0] ; Load vector length for bounds check
    CMP X26, X9
    B.HS L__bounds_error_handler_START
    MOV X9, X26
    LSL X9, X9, #3
    ADD X10, X25, X9
    LDR X9, [X10, #0]
    MOV X27, X9
    ADRP X9, L_str6@PAGE
    ADD X9, X9, L_str6@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    MOV X1, X27
    BL _WRITEF1
    B L_START_ForEachIncrement_7
L_START_ForEachExit_12:
    ADRP X9, L_str7@PAGE
    ADD X9, X9, L_str7@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str8@PAGE
    ADD X9, X9, L_str8@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str9@PAGE
    ADD X9, X9, L_str9@PAGEOFF
    ADD X9, X9, #8
    MOV X27, X9
    ADRP X9, L_str10@PAGE
    ADD X9, X9, L_str10@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str4@PAGE
    ADD X9, X9, L_str4@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    SUB X10, X27, #8
    LDR X9, [X10, #0] ; Load vector/table/string length
    MOV X25, X9
    MOVZ X9, #0
    MOV X26, X9
    B L_START_ForEachHeader_13
L_START_ForEachExit_16:
    ADRP X9, L_str11@PAGE
    ADD X9, X9, L_str11@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str12@PAGE
    ADD X9, X9, L_str12@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str13@PAGE
    ADD X9, X9, L_str13@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    B L_START_BlockCleanup_17
L_START_ForEachExit_4:
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
    MOVZ X0, #3
    BL _BCPL_ALLOC_WORDS
    MOV X9, X0
    MOVZ X10, #1
    MOVZ X11, #0
    ADD X12, X9, X11
    STR X10, [X12, #0]
    MOVZ X10, #2
    MOVZ X11, #8
    ADD X12, X9, X11
    STR X10, [X12, #0]
    MOVZ X10, #3
    MOVZ X11, #16
    ADD X12, X9, X11
    STR X10, [X12, #0]
    MOV X25, X9
    ADRP X9, L_str16@PAGE
    ADD X9, X9, L_str16@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str4@PAGE
    ADD X9, X9, L_str4@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    MOVZ X9, #0
    MOV X26, X9
    B L_START_ForEachHeader_5
L_START_ForEachExit_8:
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
    MOV X26, X9
    ADRP X9, L_str20@PAGE
    ADD X9, X9, L_str20@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str4@PAGE
    ADD X9, X9, L_str4@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    SUB X10, X26, #8
    LDR X9, [X10, #0] ; Load vector/table/string length
    MOV X25, X9
    MOVZ X9, #0
    MOV X27, X9
    B L_START_ForEachHeader_9
L_START_ForEachHeader_1:
    MOV X9, X25
    CMP X9, X26
    CSET X10, LT
    CMP X10, XZR
    B.NE L_START_ForEachBody_2
    B L_START_ForEachExit_4
L_START_ForEachHeader_13:
    MOV X9, X26
    CMP X9, X25
    CSET X10, LT
    CMP X10, XZR
    B.NE L_START_ForEachBody_14
    B L_START_ForEachExit_16
L_START_ForEachHeader_5:
    MOVZ X9, #3
    MOV X10, X26
    CMP X10, X9
    CSET X11, LT
    CMP X11, XZR
    B.NE L_START_ForEachBody_6
    B L_START_ForEachExit_8
L_START_ForEachHeader_9:
    MOV X9, X27
    CMP X9, X25
    CSET X10, LT
    CMP X10, XZR
    B.NE L_START_ForEachBody_10
    B L_START_ForEachExit_12
L_START_ForEachIncrement_11:
    MOVZ X9, #1
    MOV X10, X27
    ADD X10, X10, X9
    MOV X27, X10
    B L_START_ForEachHeader_9
L_START_ForEachIncrement_15:
    MOVZ X9, #1
    MOV X10, X26
    ADD X10, X10, X9
    MOV X26, X10
    B L_START_ForEachHeader_13
L_START_ForEachIncrement_3:
    MOVZ X9, #1
    MOV X10, X25
    ADD X10, X10, X9
    MOV X25, X10
    B L_START_ForEachHeader_1
L_START_ForEachIncrement_7:
    MOVZ X9, #1
    MOV X10, X26
    ADD X10, X10, X9
    MOV X26, X10
    B L_START_ForEachHeader_5
L__bounds_error_handler_START:
    MOVZ X0, #0
    MOVZ X1, #0
    MOVZ X2, #0
    BL _BCPL_BOUNDS_ERROR
    BRK #0
L_0:
    LDR X19, [X29, #128] ; Restored Reg: X19 @ FP+128
    LDR X20, [X29, #136] ; Restored Reg: X20 @ FP+136
    LDR X21, [X29, #144] ; Restored Reg: X21 @ FP+144
    LDR X22, [X29, #152] ; Restored Reg: X22 @ FP+152
    LDR X24, [X29, #160] ; Restored Reg: X24 @ FP+160
    LDR X25, [X29, #168] ; Restored Reg: X25 @ FP+168
    LDR X26, [X29, #176] ; Restored Reg: X26 @ FP+176
    LDR X27, [X29, #184] ; Restored Reg: X27 @ FP+184
    LDR X28, [X29, #192] ; Restored Reg: X28 @ FP+192
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L___veneer_:
    movz x16, #21696
    movk x16, #625, lsl #16
    movk x16, #1, lsl #32
    movk x16, #0, lsl #48
    blr x16

.section __DATA,__const
.p2align 3
L_str0:
    .quad 0x25
    ; (upper half)
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x20
    .long 0x46
    .long 0x4f
    .long 0x52
    .long 0x45
    .long 0x41
    .long 0x43
    .long 0x48
    .long 0x20
    .long 0x56
    .long 0x41
    .long 0x52
    .long 0x49
    .long 0x41
    .long 0x42
    .long 0x4c
    .long 0x45
    .long 0x20
    .long 0x44
    .long 0x45
    .long 0x42
    .long 0x55
    .long 0x47
    .long 0x20
    .long 0x54
    .long 0x45
    .long 0x53
    .long 0x54
    .long 0x20
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x5c
    .long 0x6e
    .long 0x0
    .long 0x0
L_str1:
    .quad 0x31
    ; (upper half)
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x20
    .long 0x31
    .long 0x3a
    .long 0x20
    .long 0x53
    .long 0x74
    .long 0x72
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x76
    .long 0x61
    .long 0x72
    .long 0x69
    .long 0x61
    .long 0x62
    .long 0x6c
    .long 0x65
    .long 0x20
    .long 0x27
    .long 0x61
    .long 0x62
    .long 0x63
    .long 0x27
    .long 0x20
    .long 0x28
    .long 0x73
    .long 0x68
    .long 0x6f
    .long 0x75
    .long 0x6c
    .long 0x64
    .long 0x20
    .long 0x63
    .long 0x61
    .long 0x6c
    .long 0x6c
    .long 0x20
    .long 0x4c
    .long 0x45
    .long 0x4e
    .long 0x29
    .long 0x5c
    .long 0x6e
    .long 0x0
    .long 0x0
L_str2:
    .quad 0x3
    ; (upper half)
    .long 0x61
    .long 0x62
    .long 0x63
    .long 0x0
    .long 0x0
L_str3:
    .quad 0x22
    ; (upper half)
    .long 0x45
    .long 0x78
    .long 0x70
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x65
    .long 0x64
    .long 0x3a
    .long 0x20
    .long 0x61
    .long 0x2c
    .long 0x20
    .long 0x62
    .long 0x2c
    .long 0x20
    .long 0x63
    .long 0x20
    .long 0x28
    .long 0x33
    .long 0x20
    .long 0x63
    .long 0x68
    .long 0x61
    .long 0x72
    .long 0x61
    .long 0x63
    .long 0x74
    .long 0x65
    .long 0x72
    .long 0x73
    .long 0x29
    .long 0x5c
    .long 0x6e
    .long 0x0
    .long 0x0
L_str4:
    .quad 0x10
    ; (upper half)
    .long 0x41
    .long 0x63
    .long 0x74
    .long 0x75
    .long 0x61
    .long 0x6c
    .long 0x20
    .long 0x6f
    .long 0x75
    .long 0x74
    .long 0x70
    .long 0x75
    .long 0x74
    .long 0x3a
    .long 0x5c
    .long 0x6e
    .long 0x0
    .long 0x0
L_str5:
    .quad 0x6
    ; (upper half)
    .long 0x20
    .long 0x20
    .long 0x25
    .long 0x63
    .long 0x5c
    .long 0x6e
    .long 0x0
    .long 0x0
L_str6:
    .quad 0x6
    ; (upper half)
    .long 0x20
    .long 0x20
    .long 0x25
    .long 0x64
    .long 0x5c
    .long 0x6e
    .long 0x0
    .long 0x0
L_str7:
    .quad 0x1a
    ; (upper half)
    .long 0x53
    .long 0x68
    .long 0x6f
    .long 0x72
    .long 0x74
    .long 0x20
    .long 0x73
    .long 0x74
    .long 0x72
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x74
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x20
    .long 0x64
    .long 0x6f
    .long 0x6e
    .long 0x65
    .long 0x5c
    .long 0x6e
    .long 0x5c
    .long 0x6e
    .long 0x0
    .long 0x0
L_str8:
    .quad 0x2c
    ; (upper half)
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x20
    .long 0x34
    .long 0x3a
    .long 0x20
    .long 0x53
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x6c
    .long 0x65
    .long 0x2d
    .long 0x65
    .long 0x6c
    .long 0x65
    .long 0x6d
    .long 0x65
    .long 0x6e
    .long 0x74
    .long 0x20
    .long 0x73
    .long 0x74
    .long 0x72
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x76
    .long 0x61
    .long 0x72
    .long 0x69
    .long 0x61
    .long 0x62
    .long 0x6c
    .long 0x65
    .long 0x20
    .long 0x27
    .long 0x78
    .long 0x27
    .long 0x5c
    .long 0x6e
    .long 0x0
    .long 0x0
L_str9:
    .quad 0x1
    ; (upper half)
    .long 0x78
    .long 0x0
    .long 0x0
L_str10:
    .quad 0x1b
    ; (upper half)
    .long 0x45
    .long 0x78
    .long 0x70
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x65
    .long 0x64
    .long 0x3a
    .long 0x20
    .long 0x78
    .long 0x20
    .long 0x28
    .long 0x31
    .long 0x20
    .long 0x63
    .long 0x68
    .long 0x61
    .long 0x72
    .long 0x61
    .long 0x63
    .long 0x74
    .long 0x65
    .long 0x72
    .long 0x29
    .long 0x5c
    .long 0x6e
    .long 0x0
    .long 0x0
L_str11:
    .quad 0x19
    ; (upper half)
    .long 0x53
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x6c
    .long 0x65
    .long 0x20
    .long 0x63
    .long 0x68
    .long 0x61
    .long 0x72
    .long 0x20
    .long 0x74
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x20
    .long 0x64
    .long 0x6f
    .long 0x6e
    .long 0x65
    .long 0x5c
    .long 0x6e
    .long 0x5c
    .long 0x6e
    .long 0x0
    .long 0x0
L_str12:
    .quad 0x26
    ; (upper half)
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x20
    .long 0x56
    .long 0x41
    .long 0x52
    .long 0x49
    .long 0x41
    .long 0x42
    .long 0x4c
    .long 0x45
    .long 0x20
    .long 0x44
    .long 0x45
    .long 0x42
    .long 0x55
    .long 0x47
    .long 0x20
    .long 0x54
    .long 0x45
    .long 0x53
    .long 0x54
    .long 0x20
    .long 0x43
    .long 0x4f
    .long 0x4d
    .long 0x50
    .long 0x4c
    .long 0x45
    .long 0x54
    .long 0x45
    .long 0x20
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x5c
    .long 0x6e
    .long 0x0
    .long 0x0
L_str13:
    .quad 0x37
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
    .long 0x73
    .long 0x20
    .long 0x74
    .long 0x68
    .long 0x65
    .long 0x20
    .long 0x6e
    .long 0x6f
    .long 0x6e
    .long 0x2d
    .long 0x6f
    .long 0x70
    .long 0x74
    .long 0x69
    .long 0x6d
    .long 0x69
    .long 0x7a
    .long 0x65
    .long 0x64
    .long 0x20
    .long 0x70
    .long 0x61
    .long 0x74
    .long 0x68
    .long 0x20
    .long 0x74
    .long 0x6f
    .long 0x20
    .long 0x69
    .long 0x73
    .long 0x6f
    .long 0x6c
    .long 0x61
    .long 0x74
    .long 0x65
    .long 0x20
    .long 0x74
    .long 0x68
    .long 0x65
    .long 0x20
    .long 0x62
    .long 0x75
    .long 0x67
    .long 0x21
    .long 0x5c
    .long 0x6e
    .long 0x0
    .long 0x0
L_str14:
    .quad 0x1d
    ; (upper half)
    .long 0x53
    .long 0x74
    .long 0x72
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x76
    .long 0x61
    .long 0x72
    .long 0x69
    .long 0x61
    .long 0x62
    .long 0x6c
    .long 0x65
    .long 0x20
    .long 0x74
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x20
    .long 0x64
    .long 0x6f
    .long 0x6e
    .long 0x65
    .long 0x5c
    .long 0x6e
    .long 0x5c
    .long 0x6e
    .long 0x0
    .long 0x0
L_str15:
    .quad 0x38
    ; (upper half)
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x20
    .long 0x32
    .long 0x3a
    .long 0x20
    .long 0x56
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x6f
    .long 0x72
    .long 0x20
    .long 0x76
    .long 0x61
    .long 0x72
    .long 0x69
    .long 0x61
    .long 0x62
    .long 0x6c
    .long 0x65
    .long 0x20
    .long 0x56
    .long 0x45
    .long 0x43
    .long 0x28
    .long 0x31
    .long 0x2c
    .long 0x20
    .long 0x32
    .long 0x2c
    .long 0x20
    .long 0x33
    .long 0x29
    .long 0x20
    .long 0x28
    .long 0x73
    .long 0x68
    .long 0x6f
    .long 0x75
    .long 0x6c
    .long 0x64
    .long 0x20
    .long 0x63
    .long 0x61
    .long 0x6c
    .long 0x6c
    .long 0x20
    .long 0x4c
    .long 0x45
    .long 0x4e
    .long 0x29
    .long 0x5c
    .long 0x6e
    .long 0x0
    .long 0x0
L_str16:
    .quad 0x20
    ; (upper half)
    .long 0x45
    .long 0x78
    .long 0x70
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x65
    .long 0x64
    .long 0x3a
    .long 0x20
    .long 0x31
    .long 0x2c
    .long 0x20
    .long 0x32
    .long 0x2c
    .long 0x20
    .long 0x33
    .long 0x20
    .long 0x28
    .long 0x33
    .long 0x20
    .long 0x65
    .long 0x6c
    .long 0x65
    .long 0x6d
    .long 0x65
    .long 0x6e
    .long 0x74
    .long 0x73
    .long 0x29
    .long 0x5c
    .long 0x6e
    .long 0x0
    .long 0x0
L_str17:
    .quad 0x1d
    ; (upper half)
    .long 0x56
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x6f
    .long 0x72
    .long 0x20
    .long 0x76
    .long 0x61
    .long 0x72
    .long 0x69
    .long 0x61
    .long 0x62
    .long 0x6c
    .long 0x65
    .long 0x20
    .long 0x74
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x20
    .long 0x64
    .long 0x6f
    .long 0x6e
    .long 0x65
    .long 0x5c
    .long 0x6e
    .long 0x5c
    .long 0x6e
    .long 0x0
    .long 0x0
L_str18:
    .quad 0x2a
    ; (upper half)
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x20
    .long 0x33
    .long 0x3a
    .long 0x20
    .long 0x54
    .long 0x77
    .long 0x6f
    .long 0x2d
    .long 0x65
    .long 0x6c
    .long 0x65
    .long 0x6d
    .long 0x65
    .long 0x6e
    .long 0x74
    .long 0x20
    .long 0x73
    .long 0x74
    .long 0x72
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x76
    .long 0x61
    .long 0x72
    .long 0x69
    .long 0x61
    .long 0x62
    .long 0x6c
    .long 0x65
    .long 0x20
    .long 0x27
    .long 0x68
    .long 0x69
    .long 0x27
    .long 0x5c
    .long 0x6e
    .long 0x0
    .long 0x0
L_str19:
    .quad 0x2
    ; (upper half)
    .long 0x68
    .long 0x69
    .long 0x0
    .long 0x0
L_str20:
    .quad 0x1f
    ; (upper half)
    .long 0x45
    .long 0x78
    .long 0x70
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x65
    .long 0x64
    .long 0x3a
    .long 0x20
    .long 0x68
    .long 0x2c
    .long 0x20
    .long 0x69
    .long 0x20
    .long 0x28
    .long 0x32
    .long 0x20
    .long 0x63
    .long 0x68
    .long 0x61
    .long 0x72
    .long 0x61
    .long 0x63
    .long 0x74
    .long 0x65
    .long 0x72
    .long 0x73
    .long 0x29
    .long 0x5c
    .long 0x6e
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
L__data_segment_base:
