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
    STP X29, X30, [SP, #-144]!
    MOV X29, SP
    STP x19, x20, [x29, #56]
    STP x21, x22, [x29, #72]
    STP x23, x24, [x29, #88]
    STP x25, x26, [x29, #104]
    STP x27, x28, [x29, #120]
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
L_START_Entry_0:
    ADRP X9, L_str0@PAGE
    ADD X9, X9, L_str0@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    MOVZ X9, #3
    MOV X20, X9
    LSL X21, X9, #1
    MOV X0, X21
    BL _GETVEC
    MOV X21, X0
    SUB X9, X21, #8
    STR X20, [X9, #0]
    MOV X25, X21
    MOVZ X9, #3
    MOV X20, X9
    LSL X21, X9, #1
    MOV X0, X21
    BL _GETVEC
    MOV X21, X0
    SUB X9, X21, #8
    STR X20, [X9, #0]
    MOV X24, X21
    ADRP X9, L_pair0@PAGE
    ADD X9, X9, L_pair0@PAGEOFF
    LDR X9, [X9, #0]
    MOVZ X10, #0
    LSL X11, X10, #3
    ADD X12, X25, X11
    STR X9, [X12, #0]
    ADRP X9, L_pair1@PAGE
    ADD X9, X9, L_pair1@PAGEOFF
    LDR X9, [X9, #0]
    MOVZ X10, #1
    LSL X11, X10, #3
    ADD X12, X25, X11
    STR X9, [X12, #0]
    ADRP X9, L_pair2@PAGE
    ADD X9, X9, L_pair2@PAGEOFF
    LDR X9, [X9, #0]
    MOVZ X10, #2
    LSL X11, X10, #3
    ADD X12, X25, X11
    STR X9, [X12, #0]
    ADRP X9, L_pair3@PAGE
    ADD X9, X9, L_pair3@PAGEOFF
    LDR X9, [X9, #0]
    MOVZ X10, #3
    LSL X11, X10, #3
    ADD X12, X25, X11
    STR X9, [X12, #0]
    ADRP X9, L_pair4@PAGE
    ADD X9, X9, L_pair4@PAGEOFF
    LDR X9, [X9, #0]
    MOVZ X10, #0
    LSL X11, X10, #3
    ADD X12, X24, X11
    STR X9, [X12, #0]
    ADRP X9, L_pair5@PAGE
    ADD X9, X9, L_pair5@PAGEOFF
    LDR X9, [X9, #0]
    MOVZ X10, #1
    LSL X11, X10, #3
    ADD X12, X24, X11
    STR X9, [X12, #0]
    ADRP X9, L_pair6@PAGE
    ADD X9, X9, L_pair6@PAGEOFF
    LDR X9, [X9, #0]
    MOVZ X10, #2
    LSL X11, X10, #3
    ADD X12, X24, X11
    STR X9, [X12, #0]
    ADRP X9, L_pair7@PAGE
    ADD X9, X9, L_pair7@PAGEOFF
    LDR X9, [X9, #0]
    MOVZ X10, #3
    LSL X11, X10, #3
    ADD X12, X24, X11
    STR X9, [X12, #0]
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
    MOV X20, X25
    MOV X21, X24
    SUB X10, X25, #8
    LDR X9, [X10, #0]
    SUB X11, X25, #8
    LDR X10, [X11, #0]
    SUB X12, X24, #8
    LDR X11, [X12, #0]
    MOVZ X12, #1
    LSL X14, X10, #1
    MOV X13, X0
    MOV X0, X14
    BL _GETVEC
    MOV X14, X0
    MOV X0, X13
    MOV X11, X25
    MOV X13, X24
    MOV X15, X14
L_1:
    CMP X12, #0
    B.EQ L_2
    LDR Q0, [X11, #0]
    LDR Q1, [X13, #0]
    SMIN V2.4S, V0.4S, V1.4S
    STR Q2, [X15, #0]
    ADD X11, X11, #16
    ADD X13, X13, #16
    ADD X15, X15, #16
    SUB X12, X12, #1
    B L_1
L_2:
    MOV X26, X14
    ADRP X9, L_str4@PAGE
    ADD X9, X9, L_str4@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    MOVZ X9, #0
    SUB X12, X26, #8
    LDR X11, [X12, #0] ; Load vector length for bounds check
    CMP X9, X11
    B.HS L__bounds_error_handler_START
    MOV X11, X9
    LSL X11, X11, #3
    ADD X12, X26, X11
    LDR X9, [X12, #0]
    MOV X1, X9
    BL _WRITEF1
    ADRP X9, L_str5@PAGE
    ADD X9, X9, L_str5@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    MOVZ X9, #1
    SUB X11, X26, #8
    LDR X10, [X11, #0] ; Load vector length for bounds check
    CMP X9, X10
    B.HS L__bounds_error_handler_START
    MOV X10, X9
    LSL X10, X10, #3
    ADD X11, X26, X10
    LDR X9, [X11, #0]
    MOV X1, X9
    BL _WRITEF1
    ADRP X9, L_str6@PAGE
    ADD X9, X9, L_str6@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    MOVZ X9, #2
    SUB X11, X26, #8
    LDR X10, [X11, #0] ; Load vector length for bounds check
    CMP X9, X10
    B.HS L__bounds_error_handler_START
    MOV X10, X9
    LSL X10, X10, #3
    ADD X11, X26, X10
    LDR X9, [X11, #0]
    MOV X1, X9
    BL _WRITEF1
    ADRP X9, L_str7@PAGE
    ADD X9, X9, L_str7@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    MOVZ X9, #3
    SUB X11, X26, #8
    LDR X10, [X11, #0] ; Load vector length for bounds check
    CMP X9, X10
    B.HS L__bounds_error_handler_START
    MOV X10, X9
    LSL X10, X10, #3
    ADD X11, X26, X10
    LDR X9, [X11, #0]
    MOV X1, X9
    BL _WRITEF1
    ADRP X9, L_str8@PAGE
    ADD X9, X9, L_str8@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    MOV X22, X25
    SUB X10, X25, #8
    LDR X9, [X10, #0]
    SUB X11, X25, #8
    LDR X10, [X11, #0]
    SUB X12, X24, #8
    LDR X11, [X12, #0]
    MOVZ X12, #1
    LSL X14, X10, #1
    MOV X13, X0
    MOV X0, X14
    BL _GETVEC
    MOV X14, X0
    MOV X0, X13
    MOV X11, X25
    MOV X13, X24
    MOV X15, X14
L_3:
    CMP X12, #0
    B.EQ L_4
    LDR Q0, [X11, #0]
    LDR Q1, [X13, #0]
    SMAX V2.4S, V0.4S, V1.4S
    STR Q2, [X15, #0]
    ADD X11, X11, #16
    ADD X13, X13, #16
    ADD X15, X15, #16
    SUB X12, X12, #1
    B L_3
L_4:
    MOV X27, X14
    ADRP X9, L_str9@PAGE
    ADD X9, X9, L_str9@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    MOVZ X9, #0
    SUB X12, X27, #8
    LDR X11, [X12, #0] ; Load vector length for bounds check
    CMP X9, X11
    B.HS L__bounds_error_handler_START
    MOV X11, X9
    LSL X11, X11, #3
    ADD X12, X27, X11
    LDR X9, [X12, #0]
    MOV X1, X9
    BL _WRITEF1
    ADRP X9, L_str10@PAGE
    ADD X9, X9, L_str10@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    MOVZ X9, #1
    SUB X11, X27, #8
    LDR X10, [X11, #0] ; Load vector length for bounds check
    CMP X9, X10
    B.HS L__bounds_error_handler_START
    MOV X10, X9
    LSL X10, X10, #3
    ADD X11, X27, X10
    LDR X9, [X11, #0]
    MOV X1, X9
    BL _WRITEF1
    ADRP X9, L_str11@PAGE
    ADD X9, X9, L_str11@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    MOVZ X9, #2
    SUB X11, X27, #8
    LDR X10, [X11, #0] ; Load vector length for bounds check
    CMP X9, X10
    B.HS L__bounds_error_handler_START
    MOV X10, X9
    LSL X10, X10, #3
    ADD X11, X27, X10
    LDR X9, [X11, #0]
    MOV X1, X9
    BL _WRITEF1
    ADRP X9, L_str12@PAGE
    ADD X9, X9, L_str12@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    MOVZ X9, #3
    SUB X11, X27, #8
    LDR X10, [X11, #0] ; Load vector length for bounds check
    CMP X9, X10
    B.HS L__bounds_error_handler_START
    MOV X10, X9
    LSL X10, X10, #3
    ADD X11, X27, X10
    LDR X9, [X11, #0]
    MOV X1, X9
    BL _WRITEF1
    ADRP X9, L_str13@PAGE
    ADD X9, X9, L_str13@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    MOV X26, X24
    SUB X10, X25, #8
    LDR X9, [X10, #0]
    SUB X11, X25, #8
    LDR X10, [X11, #0]
    SUB X12, X24, #8
    LDR X11, [X12, #0]
    MOVZ X12, #1
    LSL X14, X10, #1
    MOV X13, X0
    MOV X0, X14
    BL _GETVEC
    MOV X14, X0
    MOV X0, X13
    MOV X11, X25
    MOV X13, X24
    MOV X15, X14
L_5:
    CMP X12, #0
    B.EQ L_6
    LDR Q0, [X11, #0]
    LDR Q1, [X13, #0]
    ADD V2.4S, V0.4S, V1.4S
    STR Q2, [X15, #0]
    ADD X11, X11, #16
    ADD X13, X13, #16
    ADD X15, X15, #16
    SUB X12, X12, #1
    B L_5
L_6:
    MOV X23, X14
    ADRP X9, L_str14@PAGE
    ADD X9, X9, L_str14@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    MOV X1, X23
    BL _WRITEF1
    ADRP X9, L_str15@PAGE
    ADD X9, X9, L_str15@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    B L_START_Exit_1
L_START_Exit_1:
    B L_0
L__bounds_error_handler_START:
    MOVZ X0, #0
    MOVZ X1, #0
    MOVZ X2, #0
    BL _BCPL_BOUNDS_ERROR
    BRK #0
L_0:
    LDP x19, x20, [x29, #56]
    LDP x21, x22, [x29, #72]
    LDP x23, x24, [x29, #88]
    LDP x25, x26, [x29, #104]
    LDP x27, x28, [x29, #120]
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [SP, #0]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L___veneer_:
    movz x16, #41320
    movk x16, #110, lsl #16
    movk x16, #1, lsl #32
    movk x16, #0, lsl #48
    blr x16

.section __DATA,__const
.p2align 3
L_str0:
    .quad 0x2a
    ; (upper half)
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x53
    .long 0x20
    .long 0x56
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x6f
    .long 0x72
    .long 0x20
    .long 0x52
    .long 0x65
    .long 0x64
    .long 0x75
    .long 0x63
    .long 0x74
    .long 0x69
    .long 0x6f
    .long 0x6e
    .long 0x20
    .long 0x4f
    .long 0x70
    .long 0x65
    .long 0x72
    .long 0x61
    .long 0x74
    .long 0x69
    .long 0x6f
    .long 0x6e
    .long 0x73
    .long 0xa
    .long 0x0
    .long 0x0
L_str1:
    .quad 0x23
    ; (upper half)
    .long 0x70
    .long 0x61
    .long 0x69
    .long 0x72
    .long 0x73
    .long 0x31
    .long 0x3a
    .long 0x20
    .long 0x28
    .long 0x31
    .long 0x2c
    .long 0x32
    .long 0x29
    .long 0x2c
    .long 0x20
    .long 0x28
    .long 0x33
    .long 0x2c
    .long 0x34
    .long 0x29
    .long 0x2c
    .long 0x20
    .long 0x28
    .long 0x35
    .long 0x2c
    .long 0x36
    .long 0x29
    .long 0x2c
    .long 0x20
    .long 0x28
    .long 0x37
    .long 0x2c
    .long 0x38
    .long 0x29
    .long 0xa
    .long 0x0
    .long 0x0
L_str2:
    .quad 0x23
    ; (upper half)
    .long 0x70
    .long 0x61
    .long 0x69
    .long 0x72
    .long 0x73
    .long 0x32
    .long 0x3a
    .long 0x20
    .long 0x28
    .long 0x32
    .long 0x2c
    .long 0x31
    .long 0x29
    .long 0x2c
    .long 0x20
    .long 0x28
    .long 0x36
    .long 0x2c
    .long 0x33
    .long 0x29
    .long 0x2c
    .long 0x20
    .long 0x28
    .long 0x34
    .long 0x2c
    .long 0x39
    .long 0x29
    .long 0x2c
    .long 0x20
    .long 0x28
    .long 0x38
    .long 0x2c
    .long 0x35
    .long 0x29
    .long 0xa
    .long 0x0
    .long 0x0
L_str3:
    .quad 0x35
    ; (upper half)
    .long 0xa
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
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
    .long 0x28
    .long 0x61
    .long 0x75
    .long 0x74
    .long 0x6f
    .long 0x6d
    .long 0x61
    .long 0x74
    .long 0x69
    .long 0x63
    .long 0x20
    .long 0x74
    .long 0x79
    .long 0x70
    .long 0x65
    .long 0x20
    .long 0x69
    .long 0x6e
    .long 0x66
    .long 0x65
    .long 0x72
    .long 0x65
    .long 0x6e
    .long 0x63
    .long 0x65
    .long 0x29
    .long 0x2e
    .long 0x2e
    .long 0x2e
    .long 0xa
    .long 0x0
    .long 0x0
L_str4:
    .quad 0x16
    ; (upper half)
    .long 0x4d
    .long 0x49
    .long 0x4e
    .long 0x20
    .long 0x72
    .long 0x65
    .long 0x73
    .long 0x75
    .long 0x6c
    .long 0x74
    .long 0x20
    .long 0x70
    .long 0x61
    .long 0x69
    .long 0x72
    .long 0x20
    .long 0x30
    .long 0x3a
    .long 0x20
    .long 0x25
    .long 0x50
    .long 0xa
    .long 0x0
    .long 0x0
L_str5:
    .quad 0x16
    ; (upper half)
    .long 0x4d
    .long 0x49
    .long 0x4e
    .long 0x20
    .long 0x72
    .long 0x65
    .long 0x73
    .long 0x75
    .long 0x6c
    .long 0x74
    .long 0x20
    .long 0x70
    .long 0x61
    .long 0x69
    .long 0x72
    .long 0x20
    .long 0x31
    .long 0x3a
    .long 0x20
    .long 0x25
    .long 0x50
    .long 0xa
    .long 0x0
    .long 0x0
L_str6:
    .quad 0x16
    ; (upper half)
    .long 0x4d
    .long 0x49
    .long 0x4e
    .long 0x20
    .long 0x72
    .long 0x65
    .long 0x73
    .long 0x75
    .long 0x6c
    .long 0x74
    .long 0x20
    .long 0x70
    .long 0x61
    .long 0x69
    .long 0x72
    .long 0x20
    .long 0x32
    .long 0x3a
    .long 0x20
    .long 0x25
    .long 0x50
    .long 0xa
    .long 0x0
    .long 0x0
L_str7:
    .quad 0x16
    ; (upper half)
    .long 0x4d
    .long 0x49
    .long 0x4e
    .long 0x20
    .long 0x72
    .long 0x65
    .long 0x73
    .long 0x75
    .long 0x6c
    .long 0x74
    .long 0x20
    .long 0x70
    .long 0x61
    .long 0x69
    .long 0x72
    .long 0x20
    .long 0x33
    .long 0x3a
    .long 0x20
    .long 0x25
    .long 0x50
    .long 0xa
    .long 0x0
    .long 0x0
L_str8:
    .quad 0x35
    ; (upper half)
    .long 0xa
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x4d
    .long 0x41
    .long 0x58
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
    .long 0x28
    .long 0x61
    .long 0x75
    .long 0x74
    .long 0x6f
    .long 0x6d
    .long 0x61
    .long 0x74
    .long 0x69
    .long 0x63
    .long 0x20
    .long 0x74
    .long 0x79
    .long 0x70
    .long 0x65
    .long 0x20
    .long 0x69
    .long 0x6e
    .long 0x66
    .long 0x65
    .long 0x72
    .long 0x65
    .long 0x6e
    .long 0x63
    .long 0x65
    .long 0x29
    .long 0x2e
    .long 0x2e
    .long 0x2e
    .long 0xa
    .long 0x0
    .long 0x0
L_str9:
    .quad 0x16
    ; (upper half)
    .long 0x4d
    .long 0x41
    .long 0x58
    .long 0x20
    .long 0x72
    .long 0x65
    .long 0x73
    .long 0x75
    .long 0x6c
    .long 0x74
    .long 0x20
    .long 0x70
    .long 0x61
    .long 0x69
    .long 0x72
    .long 0x20
    .long 0x30
    .long 0x3a
    .long 0x20
    .long 0x25
    .long 0x50
    .long 0xa
    .long 0x0
    .long 0x0
L_str10:
    .quad 0x16
    ; (upper half)
    .long 0x4d
    .long 0x41
    .long 0x58
    .long 0x20
    .long 0x72
    .long 0x65
    .long 0x73
    .long 0x75
    .long 0x6c
    .long 0x74
    .long 0x20
    .long 0x70
    .long 0x61
    .long 0x69
    .long 0x72
    .long 0x20
    .long 0x31
    .long 0x3a
    .long 0x20
    .long 0x25
    .long 0x50
    .long 0xa
    .long 0x0
    .long 0x0
L_str11:
    .quad 0x16
    ; (upper half)
    .long 0x4d
    .long 0x41
    .long 0x58
    .long 0x20
    .long 0x72
    .long 0x65
    .long 0x73
    .long 0x75
    .long 0x6c
    .long 0x74
    .long 0x20
    .long 0x70
    .long 0x61
    .long 0x69
    .long 0x72
    .long 0x20
    .long 0x32
    .long 0x3a
    .long 0x20
    .long 0x25
    .long 0x50
    .long 0xa
    .long 0x0
    .long 0x0
L_str12:
    .quad 0x16
    ; (upper half)
    .long 0x4d
    .long 0x41
    .long 0x58
    .long 0x20
    .long 0x72
    .long 0x65
    .long 0x73
    .long 0x75
    .long 0x6c
    .long 0x74
    .long 0x20
    .long 0x70
    .long 0x61
    .long 0x69
    .long 0x72
    .long 0x20
    .long 0x33
    .long 0x3a
    .long 0x20
    .long 0x25
    .long 0x50
    .long 0xa
    .long 0x0
    .long 0x0
L_str13:
    .quad 0x35
    ; (upper half)
    .long 0xa
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x53
    .long 0x55
    .long 0x4d
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
    .long 0x28
    .long 0x61
    .long 0x75
    .long 0x74
    .long 0x6f
    .long 0x6d
    .long 0x61
    .long 0x74
    .long 0x69
    .long 0x63
    .long 0x20
    .long 0x74
    .long 0x79
    .long 0x70
    .long 0x65
    .long 0x20
    .long 0x69
    .long 0x6e
    .long 0x66
    .long 0x65
    .long 0x72
    .long 0x65
    .long 0x6e
    .long 0x63
    .long 0x65
    .long 0x29
    .long 0x2e
    .long 0x2e
    .long 0x2e
    .long 0xa
    .long 0x0
    .long 0x0
L_str14:
    .quad 0xf
    ; (upper half)
    .long 0x53
    .long 0x55
    .long 0x4d
    .long 0x20
    .long 0x72
    .long 0x65
    .long 0x73
    .long 0x75
    .long 0x6c
    .long 0x74
    .long 0x3a
    .long 0x20
    .long 0x25
    .long 0x50
    .long 0xa
    .long 0x0
    .long 0x0
L_str15:
    .quad 0x2b
    ; (upper half)
    .long 0xa
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x53
    .long 0x20
    .long 0x72
    .long 0x65
    .long 0x64
    .long 0x75
    .long 0x63
    .long 0x74
    .long 0x69
    .long 0x6f
    .long 0x6e
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
    .long 0x73
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
    .long 0x21
    .long 0xa
    .long 0x0
    .long 0x0
L_pair0:
    .quad 0x200000001
    ; (upper half)
L_pair1:
    .quad 0x400000003
    ; (upper half)
L_pair2:
    .quad 0x600000005
    ; (upper half)
L_pair3:
    .quad 0x800000007
    ; (upper half)
L_pair4:
    .quad 0x100000002
    ; (upper half)
L_pair5:
    .quad 0x300000006
    ; (upper half)
L_pair6:
    .quad 0x900000004
    ; (upper half)
L_pair7:
    .quad 0x500000008
    ; (upper half)

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
L__data_segment_base:
