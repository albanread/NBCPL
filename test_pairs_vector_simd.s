.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _START
.globl _WRITEN
.globl _SDL2_DRAW_RECT
.globl _SLURP
.globl _SDL2_CREATE_WINDOW
.globl _WRITES
.globl _WRITEF
.globl _BCPL_LIST_APPEND_STRING
.globl _BCPL_LIST_APPEND_FLOAT
.globl _BCPL_LIST_GET_NTH
.globl _IGETVEC
.globl _RDCH
.globl _BCPL_ALLOC_WORDS
.globl _HEAPMANAGER_ISSAMMENABLED
.globl _SPLIT
.globl _SGETVEC
.globl _GET_FREE_LIST_HEAD_ADDR
.globl _PGETVEC
.globl _OBJECT_HEAP_FREE
.globl _DEEPCOPYLIST
.globl _BCPL_FREE_LIST_SAFE
.globl _BCPL_CHECK_AND_DISPLAY_ERRORS
.globl _WRITEC
.globl _FILE_READ
.globl _UNPACKSTRING
.globl _SDL2_DELAY
.globl _FREEVEC
.globl _BCPL_CONCAT_LISTS
.globl _SDL2_GET_EVENT_KEY
.globl _RUNTIME_METHOD_LOOKUP
.globl _BCPL_LIST_GET_HEAD_AS_INT
.globl _HEAPMANAGER_ENTER_SCOPE
.globl _WRITEF2
.globl _FILTER
.globl _BCPL_CLEAR_ERRORS
.globl _FLOG
.globl _GETVEC
.globl _BCPL_LIST_CREATE_EMPTY
.globl _FGETVEC
.globl _LPND
.globl _BCPL_LIST_APPEND_INT
.globl _RETURNNODETOFREELIST
.globl _FCOS
.globl _APND
.globl _MALLOC
.globl _JOIN
.globl _BCPL_FREE_CELLS
.globl _FRND
.globl _SPIT
.globl _FTAN
.globl _DEEPCOPYLITERALLIST
.globl _STRCOPY
.globl _WRITEF7
.globl _SDL2_DESTROY_WINDOW
.globl _SDL2_SET_WINDOW_TITLE
.globl _HEAPMANAGER_WAITFORSAMM
.globl _SDL2_CREATE_RENDERER_EX
.globl _SPND
.globl _SDL2_POLL_EVENT
.globl _RND
.globl _STRLEN
.globl _HEAPMANAGER_EXIT_SCOPE
.globl _SDL2_DRAW_POINT
.globl _WRITEF3
.globl _BCPL_GET_ATOM_TYPE
.globl _FILE_READS
.globl _SDL2_QUIT
.globl _FILE_WRITES
.globl _PIC_RUNTIME_HELPER
.globl _RAND
.globl _SDL2_FILL_RECT
.globl _FWRITE
.globl _SDL2_CLEAR
.globl _SDL2_DESTROY_RENDERER
.globl _BCPL_LIST_GET_REST
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
.globl _BCPL_ALLOC_CHARS
.globl _WRITEF5
.globl _FABS
.globl _FILE_OPEN_READ
.globl _SDL2_GET_CURRENT_VIDEO_DRIVER
.globl _SDL2_INIT_SUBSYSTEMS
.globl _BCPL_FREE_LIST
.globl _FILE_CLOSE
.globl _PACKSTRING
.globl _WRITEF4
.globl _FEXP
.globl _FPND
.globl _FIX
.globl _WRITEF1
.globl _FILE_WRITE
.globl _COPYLIST
.globl _SDL2_GET_DISPLAY_MODES
.globl _BCPL_GET_LAST_ERROR
.globl _SDL2_PRESENT
.globl _SDL2_GET_EVENT_MOUSE
.globl _SDL2_SET_DRAW_COLOR
.globl _WRITEF6
.globl _HEAPMANAGER_SETSAMMENABLED
.globl _FILE_OPEN_WRITE
.globl _SDL2_GET_TICKS
.globl _BCPL_LIST_GET_HEAD_AS_FLOAT
.globl _NEWLINE
.globl _SDL2_INIT
.globl _CONCAT
.globl _SDL2_DRAW_LINE
.globl _SDL2_CLEAR_ERROR
.globl _SDL2_CREATE_RENDERER
.globl _SDL2_SET_WINDOW_SIZE
.globl _SDL2_GET_EVENT_BUTTON
.globl _SETTYPE
.globl _FILE_SEEK
.globl _SDL2_GET_VERSION
.globl _SDL2_GET_ERROR
.globl _FINISH
.globl _QGETVEC
.globl _SDL2_GET_VIDEO_DRIVERS
.globl _FIND
.globl ___SDL2_STATIC_BUILD
.p2align 2
_start:
_START:
    B L_START
    
; --- Veneer Section ---
    ; --- End Veneer Section ---

L_START:
    STP X29, X30, [SP, #-112]!
    MOV X29, SP
    STP x19, x20, [x29, #56]
    STP x25, x26, [x29, #72]
    STP x27, x28, [x29, #88]
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
L_START_Entry_0:
    ADRP X9, L_str0@PAGE
    ADD X9, X9, L_str0@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    MOVZ X9, #4
    LSL X20, X9, #1
    MOV X0, X20
    BL _GETVEC
    MOV X20, X0
    MOV X27, X20
    MOVZ X10, #4
    LSL X20, X10, #1
    MOV X0, X20
    BL _GETVEC
    MOV X20, X0
    MOV X26, X20
    ADRP X11, L_pair0@PAGE
    ADD X11, X11, L_pair0@PAGEOFF
    LDR X11, [X11, #0]
    MOVZ X12, #0
    LSL X13, X12, #3
    ADD X14, X27, X13
    STR X11, [X14, #0]
    ADRP X11, L_pair1@PAGE
    ADD X11, X11, L_pair1@PAGEOFF
    LDR X11, [X11, #0]
    MOVZ X12, #1
    LSL X13, X12, #3
    ADD X14, X27, X13
    STR X11, [X14, #0]
    ADRP X11, L_pair2@PAGE
    ADD X11, X11, L_pair2@PAGEOFF
    LDR X11, [X11, #0]
    MOVZ X12, #2
    LSL X13, X12, #3
    ADD X14, X27, X13
    STR X11, [X14, #0]
    ADRP X11, L_pair3@PAGE
    ADD X11, X11, L_pair3@PAGEOFF
    LDR X11, [X11, #0]
    MOVZ X12, #3
    LSL X13, X12, #3
    ADD X14, X27, X13
    STR X11, [X14, #0]
    ADRP X11, L_pair4@PAGE
    ADD X11, X11, L_pair4@PAGEOFF
    LDR X11, [X11, #0]
    MOVZ X12, #0
    LSL X13, X12, #3
    ADD X14, X26, X13
    STR X11, [X14, #0]
    ADRP X11, L_pair5@PAGE
    ADD X11, X11, L_pair5@PAGEOFF
    LDR X11, [X11, #0]
    MOVZ X12, #1
    LSL X13, X12, #3
    ADD X14, X26, X13
    STR X11, [X14, #0]
    ADRP X11, L_pair6@PAGE
    ADD X11, X11, L_pair6@PAGEOFF
    LDR X11, [X11, #0]
    MOVZ X12, #2
    LSL X13, X12, #3
    ADD X14, X26, X13
    STR X11, [X14, #0]
    ADRP X11, L_pair7@PAGE
    ADD X11, X11, L_pair7@PAGEOFF
    LDR X11, [X11, #0]
    MOVZ X12, #3
    LSL X13, X12, #3
    ADD X14, X26, X13
    STR X11, [X14, #0]
    ADRP X11, L_str1@PAGE
    ADD X11, X11, L_str1@PAGEOFF
    ADD X11, X11, #8
    MOV X0, X11
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
    MOV X9, X27
    ADD X9, X9, X26
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
    MOV X9, X27
    SUB X9, X9, X26
    MOV X15, X9
    ADRP X9, L_str6@PAGE
    ADD X9, X9, L_str6@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str7@PAGE
    ADD X9, X9, L_str7@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    MOV X9, X27
    MUL X9, X9, X26
    MOV X25, X9
    ADRP X9, L_str8@PAGE
    ADD X9, X9, L_str8@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str9@PAGE
    ADD X9, X9, L_str9@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str10@PAGE
    ADD X9, X9, L_str10@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
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
    ADRP X9, L_str14@PAGE
    ADD X9, X9, L_str14@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    B L_START_Exit_1
L_START_Exit_1:
    B L_0
L_0:
    LDP x19, x20, [x29, #56]
    LDP x25, x26, [x29, #72]
    LDP x27, x28, [x29, #88]
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [SP, #0]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L___veneer_:
    movz x16, #31152
    movk x16, #741, lsl #16
    movk x16, #1, lsl #32
    movk x16, #0, lsl #48
    blr x16

.section __DATA,__const
.p2align 3
L_str0:
    .quad 0x25
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
    .long 0x53
    .long 0x49
    .long 0x4d
    .long 0x44
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
    .quad 0x3c
    ; (upper half)
    .long 0x76
    .long 0x65
    .long 0x63
    .long 0x31
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x5b
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x28
    .long 0x31
    .long 0x30
    .long 0x2c
    .long 0x32
    .long 0x30
    .long 0x29
    .long 0x2c
    .long 0x20
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x28
    .long 0x33
    .long 0x30
    .long 0x2c
    .long 0x34
    .long 0x30
    .long 0x29
    .long 0x2c
    .long 0x20
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x28
    .long 0x35
    .long 0x30
    .long 0x2c
    .long 0x36
    .long 0x30
    .long 0x29
    .long 0x2c
    .long 0x20
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x28
    .long 0x37
    .long 0x30
    .long 0x2c
    .long 0x38
    .long 0x30
    .long 0x29
    .long 0x5d
    .long 0xa
    .long 0x0
    .long 0x0
L_str2:
    .quad 0x34
    ; (upper half)
    .long 0x76
    .long 0x65
    .long 0x63
    .long 0x32
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x5b
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x28
    .long 0x31
    .long 0x2c
    .long 0x32
    .long 0x29
    .long 0x2c
    .long 0x20
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x28
    .long 0x33
    .long 0x2c
    .long 0x34
    .long 0x29
    .long 0x2c
    .long 0x20
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x28
    .long 0x35
    .long 0x2c
    .long 0x36
    .long 0x29
    .long 0x2c
    .long 0x20
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x28
    .long 0x37
    .long 0x2c
    .long 0x38
    .long 0x29
    .long 0x5d
    .long 0xa
    .long 0x0
    .long 0x0
L_str3:
    .quad 0x32
    ; (upper half)
    .long 0xa
    .long 0x50
    .long 0x65
    .long 0x72
    .long 0x66
    .long 0x6f
    .long 0x72
    .long 0x6d
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
    .long 0x76
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x6f
    .long 0x72
    .long 0x20
    .long 0x61
    .long 0x64
    .long 0x64
    .long 0x69
    .long 0x74
    .long 0x69
    .long 0x6f
    .long 0x6e
    .long 0x20
    .long 0x28
    .long 0x31
    .long 0x32
    .long 0x38
    .long 0x2d
    .long 0x62
    .long 0x69
    .long 0x74
    .long 0x20
    .long 0x53
    .long 0x49
    .long 0x4d
    .long 0x44
    .long 0x29
    .long 0x3a
    .long 0xa
    .long 0x0
    .long 0x0
L_str4:
    .quad 0x48
    ; (upper half)
    .long 0x45
    .long 0x78
    .long 0x70
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x65
    .long 0x64
    .long 0x20
    .long 0x72
    .long 0x65
    .long 0x73
    .long 0x75
    .long 0x6c
    .long 0x74
    .long 0x31
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x5b
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x28
    .long 0x31
    .long 0x31
    .long 0x2c
    .long 0x32
    .long 0x32
    .long 0x29
    .long 0x2c
    .long 0x20
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x28
    .long 0x33
    .long 0x33
    .long 0x2c
    .long 0x34
    .long 0x34
    .long 0x29
    .long 0x2c
    .long 0x20
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x28
    .long 0x35
    .long 0x35
    .long 0x2c
    .long 0x36
    .long 0x36
    .long 0x29
    .long 0x2c
    .long 0x20
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x28
    .long 0x37
    .long 0x37
    .long 0x2c
    .long 0x38
    .long 0x38
    .long 0x29
    .long 0x5d
    .long 0xa
    .long 0x0
    .long 0x0
L_str5:
    .quad 0x35
    ; (upper half)
    .long 0xa
    .long 0x50
    .long 0x65
    .long 0x72
    .long 0x66
    .long 0x6f
    .long 0x72
    .long 0x6d
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
    .long 0x76
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x6f
    .long 0x72
    .long 0x20
    .long 0x73
    .long 0x75
    .long 0x62
    .long 0x74
    .long 0x72
    .long 0x61
    .long 0x63
    .long 0x74
    .long 0x69
    .long 0x6f
    .long 0x6e
    .long 0x20
    .long 0x28
    .long 0x31
    .long 0x32
    .long 0x38
    .long 0x2d
    .long 0x62
    .long 0x69
    .long 0x74
    .long 0x20
    .long 0x53
    .long 0x49
    .long 0x4d
    .long 0x44
    .long 0x29
    .long 0x3a
    .long 0xa
    .long 0x0
    .long 0x0
L_str6:
    .quad 0x47
    ; (upper half)
    .long 0x45
    .long 0x78
    .long 0x70
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x65
    .long 0x64
    .long 0x20
    .long 0x72
    .long 0x65
    .long 0x73
    .long 0x75
    .long 0x6c
    .long 0x74
    .long 0x32
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x5b
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x28
    .long 0x39
    .long 0x2c
    .long 0x31
    .long 0x38
    .long 0x29
    .long 0x2c
    .long 0x20
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x28
    .long 0x32
    .long 0x37
    .long 0x2c
    .long 0x33
    .long 0x36
    .long 0x29
    .long 0x2c
    .long 0x20
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x28
    .long 0x34
    .long 0x35
    .long 0x2c
    .long 0x35
    .long 0x34
    .long 0x29
    .long 0x2c
    .long 0x20
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x28
    .long 0x36
    .long 0x33
    .long 0x2c
    .long 0x37
    .long 0x32
    .long 0x29
    .long 0x5d
    .long 0xa
    .long 0x0
    .long 0x0
L_str7:
    .quad 0x38
    ; (upper half)
    .long 0xa
    .long 0x50
    .long 0x65
    .long 0x72
    .long 0x66
    .long 0x6f
    .long 0x72
    .long 0x6d
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
    .long 0x76
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x6f
    .long 0x72
    .long 0x20
    .long 0x6d
    .long 0x75
    .long 0x6c
    .long 0x74
    .long 0x69
    .long 0x70
    .long 0x6c
    .long 0x69
    .long 0x63
    .long 0x61
    .long 0x74
    .long 0x69
    .long 0x6f
    .long 0x6e
    .long 0x20
    .long 0x28
    .long 0x31
    .long 0x32
    .long 0x38
    .long 0x2d
    .long 0x62
    .long 0x69
    .long 0x74
    .long 0x20
    .long 0x53
    .long 0x49
    .long 0x4d
    .long 0x44
    .long 0x29
    .long 0x3a
    .long 0xa
    .long 0x0
    .long 0x0
L_str8:
    .quad 0x4d
    ; (upper half)
    .long 0x45
    .long 0x78
    .long 0x70
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x65
    .long 0x64
    .long 0x20
    .long 0x72
    .long 0x65
    .long 0x73
    .long 0x75
    .long 0x6c
    .long 0x74
    .long 0x33
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x5b
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x28
    .long 0x31
    .long 0x30
    .long 0x2c
    .long 0x34
    .long 0x30
    .long 0x29
    .long 0x2c
    .long 0x20
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x28
    .long 0x39
    .long 0x30
    .long 0x2c
    .long 0x31
    .long 0x36
    .long 0x30
    .long 0x29
    .long 0x2c
    .long 0x20
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x28
    .long 0x32
    .long 0x35
    .long 0x30
    .long 0x2c
    .long 0x33
    .long 0x36
    .long 0x30
    .long 0x29
    .long 0x2c
    .long 0x20
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x28
    .long 0x34
    .long 0x39
    .long 0x30
    .long 0x2c
    .long 0x36
    .long 0x34
    .long 0x30
    .long 0x29
    .long 0x5d
    .long 0xa
    .long 0x0
    .long 0x0
L_str9:
    .quad 0x27
    ; (upper half)
    .long 0xa
    .long 0x2a
    .long 0x2a
    .long 0x20
    .long 0x31
    .long 0x32
    .long 0x38
    .long 0x2d
    .long 0x62
    .long 0x69
    .long 0x74
    .long 0x20
    .long 0x4e
    .long 0x45
    .long 0x4f
    .long 0x4e
    .long 0x20
    .long 0x53
    .long 0x49
    .long 0x4d
    .long 0x44
    .long 0x20
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x20
    .long 0x53
    .long 0x75
    .long 0x6d
    .long 0x6d
    .long 0x61
    .long 0x72
    .long 0x79
    .long 0x20
    .long 0x2a
    .long 0x2a
    .long 0x3a
    .long 0xa
    .long 0x0
    .long 0x0
L_str10:
    .quad 0x2a
    ; (upper half)
    .long 0x2d
    .long 0x20
    .long 0x50
    .long 0x72
    .long 0x6f
    .long 0x63
    .long 0x65
    .long 0x73
    .long 0x73
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x32
    .long 0x20
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x73
    .long 0x20
    .long 0x70
    .long 0x65
    .long 0x72
    .long 0x20
    .long 0x4e
    .long 0x45
    .long 0x4f
    .long 0x4e
    .long 0x20
    .long 0x69
    .long 0x6e
    .long 0x73
    .long 0x74
    .long 0x72
    .long 0x75
    .long 0x63
    .long 0x74
    .long 0x69
    .long 0x6f
    .long 0x6e
    .long 0xa
    .long 0x0
    .long 0x0
L_str11:
    .quad 0x29
    ; (upper half)
    .long 0x2d
    .long 0x20
    .long 0x55
    .long 0x73
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x51
    .long 0x20
    .long 0x72
    .long 0x65
    .long 0x67
    .long 0x69
    .long 0x73
    .long 0x74
    .long 0x65
    .long 0x72
    .long 0x73
    .long 0x20
    .long 0x77
    .long 0x69
    .long 0x74
    .long 0x68
    .long 0x20
    .long 0x2e
    .long 0x34
    .long 0x53
    .long 0x20
    .long 0x61
    .long 0x72
    .long 0x72
    .long 0x61
    .long 0x6e
    .long 0x67
    .long 0x65
    .long 0x6d
    .long 0x65
    .long 0x6e
    .long 0x74
    .long 0xa
    .long 0x0
    .long 0x0
L_str12:
    .quad 0x24
    ; (upper half)
    .long 0x2d
    .long 0x20
    .long 0x34
    .long 0x78
    .long 0x20
    .long 0x73
    .long 0x70
    .long 0x65
    .long 0x65
    .long 0x64
    .long 0x75
    .long 0x70
    .long 0x20
    .long 0x6f
    .long 0x76
    .long 0x65
    .long 0x72
    .long 0x20
    .long 0x73
    .long 0x63
    .long 0x61
    .long 0x6c
    .long 0x61
    .long 0x72
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
    .long 0xa
    .long 0x0
    .long 0x0
L_str13:
    .quad 0x3b
    ; (upper half)
    .long 0x2d
    .long 0x20
    .long 0x45
    .long 0x61
    .long 0x63
    .long 0x68
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
    .long 0x70
    .long 0x72
    .long 0x6f
    .long 0x63
    .long 0x65
    .long 0x73
    .long 0x73
    .long 0x65
    .long 0x73
    .long 0x20
    .long 0x34
    .long 0x20
    .long 0xd7
    .long 0x20
    .long 0x33
    .long 0x32
    .long 0x2d
    .long 0x62
    .long 0x69
    .long 0x74
    .long 0x20
    .long 0x69
    .long 0x6e
    .long 0x74
    .long 0x65
    .long 0x67
    .long 0x65
    .long 0x72
    .long 0x73
    .long 0x20
    .long 0x69
    .long 0x6e
    .long 0x20
    .long 0x70
    .long 0x61
    .long 0x72
    .long 0x61
    .long 0x6c
    .long 0x6c
    .long 0x65
    .long 0x6c
    .long 0xa
    .long 0x0
    .long 0x0
L_str14:
    .quad 0x22
    ; (upper half)
    .long 0xa
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
    .long 0x53
    .long 0x49
    .long 0x4d
    .long 0x44
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
    .quad 0x140000000a
    ; (upper half)
L_pair1:
    .quad 0x280000001e
    ; (upper half)
L_pair2:
    .quad 0x3c00000032
    ; (upper half)
L_pair3:
    .quad 0x5000000046
    ; (upper half)
L_pair4:
    .quad 0x200000001
    ; (upper half)
L_pair5:
    .quad 0x400000003
    ; (upper half)
L_pair6:
    .quad 0x600000005
    ; (upper half)
L_pair7:
    .quad 0x800000007
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
