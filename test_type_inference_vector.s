.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _START
.globl _HeapManager_exit_scope
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
.globl _HeapManager_enter_scope
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
    STP X29, X30, [SP, #-192]!
    MOV X29, SP
    STP x19, x20, [x29, #120]
    STP x21, x24, [x29, #136]
    STP x25, x26, [x29, #152]
    STP x27, x28, [x29, #168]
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
L_START_Entry_0:
    BL _HeapManager_enter_scope
    MOVZ x20, #100
    MOV X0, X20
    BL _GETVEC
    MOV X26, X0
    MOVZ X10, #100
    MOV X15, X10
    ADRP X10, L_str0@PAGE
    ADD X10, X10, L_str0@PAGEOFF
    ADD X10, X10, #8
    MOV X0, X10
    MOVZ X10, #100
    MOV X1, X10
    BL _WRITEF1
    MOVZ x20, #50
    MOV X0, X20
    BL _FGETVEC
    MOV X27, X0
    MOVZ X10, #50
    MOV X15, X10
    ADRP X10, L_str1@PAGE
    ADD X10, X10, L_str1@PAGEOFF
    ADD X10, X10, #8
    MOV X0, X10
    MOVZ X10, #50
    MOV X1, X10
    BL _WRITEF1
    MOVZ X9, #25
    MOV X0, X9
    BL _GETVEC
    MOV X9, X0
    MOV X15, X9
    MOVZ X9, #25
    MOV X15, X9
    ADRP X9, L_str2@PAGE
    ADD X9, X9, L_str2@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    MOVZ X9, #25
    MOV X1, X9
    BL _WRITEF1
    MOVZ X9, #1
    MOV X25, X9
    B L_START_ForHeader_1
L_START_Exit_9:
    B L_0
L_START_ForBody_2:
    BL _HeapManager_enter_scope
    LSL X9, X25, #3
    ADD X10, X26, X9
    STR X25, [X10, #0]
    BL _HeapManager_exit_scope
    B L_START_ForIncrement_3
L_START_ForBody_6:
    BL _HeapManager_enter_scope
    ADRP X10, L_float0@PAGE
    ADD X10, X10, L_float0@PAGEOFF
    LDR D0, [X10, #0]
    SCVTF D1, X25
    FMUL D1, D1, D0
    LSL X10, X25, #3
    ADD X11, X27, X10
    STR D1, [X11, #0]
    BL _HeapManager_exit_scope
    B L_START_ForIncrement_7
L_START_ForExit_4:
    ADRP X9, L_str3@PAGE
    ADD X9, X9, L_str3@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    MOVZ X9, #50
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
    MOVZ X9, #1
    MOV X25, X9
    B L_START_ForHeader_5
L_START_ForExit_8:
    ADRP X9, L_str4@PAGE
    ADD X9, X9, L_str4@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    MOVZ X9, #25
    SUB X11, X27, #8
    LDR X10, [X11, #0] ; Load vector length for bounds check
    CMP X9, X10
    B.HS L__bounds_error_handler_START
    MOV X10, X9
    LSL X10, X10, #3
    ADD X11, X27, X10
    LDR D0, [X11, #0]
    FMOV X9, D0
    MOV X1, X9
    BL _WRITEF1
    MOVZ x20, #200
    MOV X0, X20
    BL _GETVEC
    MOV X24, X0
    MOVZ X10, #200
    MOV X24, X10
    ADRP X10, L_str5@PAGE
    ADD X10, X10, L_str5@PAGEOFF
    ADD X10, X10, #8
    MOV X0, X10
    MOVZ X10, #200
    MOV X1, X10
    BL _WRITEF1
    ADRP X9, L_str6@PAGE
    ADD X9, X9, L_str6@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    BL _HeapManager_exit_scope
    B L_START_Exit_9
L_START_ForHeader_1:
    MOV X9, X25
    MOVZ X10, #100
    CMP X9, X10
    B.GT L_START_ForExit_4
    B L_START_ForBody_2
L_START_ForHeader_5:
    MOV X9, X25
    MOVZ X11, #50
    CMP X9, X11
    B.GT L_START_ForExit_8
    B L_START_ForBody_6
L_START_ForIncrement_3:
    MOV X9, X25
    ADD X9, X9, #1
    MOV X25, X9
    B L_START_ForHeader_1
L_START_ForIncrement_7:
    MOV X9, X25
    ADD X9, X9, #1
    MOV X25, X9
    B L_START_ForHeader_5
L__bounds_error_handler_START:
    MOVZ X0, #0
    MOVZ X1, #0
    MOVZ X2, #0
    BL _BCPL_BOUNDS_ERROR
    BRK #0
L_0:
    LDP x19, x20, [x29, #120]
    LDP x21, x24, [x29, #136]
    LDP x25, x26, [x29, #152]
    LDP x27, x28, [x29, #168]
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [SP, #0]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L___veneer_:
    movz x16, #53348
    movk x16, #703, lsl #16
    movk x16, #1, lsl #32
    movk x16, #0, lsl #48
    blr x16

.section __DATA,__const
.p2align 3
L_str0:
    .quad 0x1e
    ; (upper half)
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x20
    .long 0x31
    .long 0x3a
    .long 0x20
    .long 0x4c
    .long 0x45
    .long 0x4e
    .long 0x28
    .long 0x47
    .long 0x45
    .long 0x54
    .long 0x56
    .long 0x45
    .long 0x43
    .long 0x28
    .long 0x31
    .long 0x30
    .long 0x30
    .long 0x29
    .long 0x29
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x64
    .long 0xa
    .long 0x0
    .long 0x0
L_str1:
    .quad 0x1e
    ; (upper half)
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x20
    .long 0x32
    .long 0x3a
    .long 0x20
    .long 0x4c
    .long 0x45
    .long 0x4e
    .long 0x28
    .long 0x46
    .long 0x47
    .long 0x45
    .long 0x54
    .long 0x56
    .long 0x45
    .long 0x43
    .long 0x28
    .long 0x35
    .long 0x30
    .long 0x29
    .long 0x29
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x64
    .long 0xa
    .long 0x0
    .long 0x0
L_str2:
    .quad 0x19
    ; (upper half)
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x20
    .long 0x33
    .long 0x3a
    .long 0x20
    .long 0x4c
    .long 0x45
    .long 0x4e
    .long 0x28
    .long 0x56
    .long 0x45
    .long 0x43
    .long 0x20
    .long 0x32
    .long 0x35
    .long 0x29
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x64
    .long 0xa
    .long 0x0
    .long 0x0
L_str3:
    .quad 0x12
    ; (upper half)
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x20
    .long 0x34
    .long 0x3a
    .long 0x20
    .long 0x56
    .long 0x21
    .long 0x35
    .long 0x30
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x64
    .long 0xa
    .long 0x0
    .long 0x0
L_str4:
    .quad 0x13
    ; (upper half)
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x20
    .long 0x35
    .long 0x3a
    .long 0x20
    .long 0x46
    .long 0x56
    .long 0x21
    .long 0x32
    .long 0x35
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x66
    .long 0xa
    .long 0x0
    .long 0x0
L_str5:
    .quad 0x26
    ; (upper half)
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x20
    .long 0x36
    .long 0x3a
    .long 0x20
    .long 0x4c
    .long 0x45
    .long 0x4e
    .long 0x28
    .long 0x47
    .long 0x45
    .long 0x54
    .long 0x56
    .long 0x45
    .long 0x43
    .long 0x28
    .long 0x42
    .long 0x55
    .long 0x46
    .long 0x46
    .long 0x45
    .long 0x52
    .long 0x5f
    .long 0x53
    .long 0x49
    .long 0x5a
    .long 0x45
    .long 0x29
    .long 0x29
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x64
    .long 0xa
    .long 0x0
    .long 0x0
L_str6:
    .quad 0x2a
    ; (upper half)
    .long 0x41
    .long 0x6c
    .long 0x6c
    .long 0x20
    .long 0x76
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x6f
    .long 0x72
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
    .long 0x20
    .long 0x74
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x73
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
L_float0:
    .quad 0x3ff8000000000000
    ; (upper half)

.section __DATA,__data
.p2align 3
    .long 0x0
    .long 0x0
    .long 0x0
    .long 0x0
    .long 0x0
L__data_segment_base:
