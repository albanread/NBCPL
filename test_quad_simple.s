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
    ADRP X9, L_str0@PAGE
    ADD X9, X9, L_str0@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_quad0@PAGE
    ADD X9, X9, L_quad0@PAGEOFF
    LDR X9, [X9, #0]
    MOV X23, X9
    ADRP X9, L_str1@PAGE
    ADD X9, X9, L_str1@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    MOV X1, X23
    BL _WRITEF1
    ADRP X9, L_quad1@PAGE
    ADD X9, X9, L_quad1@PAGEOFF
    LDR X9, [X9, #0]
    MOV X22, X9
    ADRP X9, L_str2@PAGE
    ADD X9, X9, L_str2@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    MOV X1, X22
    BL _WRITEF1
    ADRP X9, L_str3@PAGE
    ADD X9, X9, L_str3@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    fmov D0, X23
    umov w20, v0.h[0]
    MOV X27, X20
    fmov D0, X23
    umov w20, v0.h[1]
    MOV X26, X20
    fmov D0, X23
    umov w20, v0.h[2]
    MOV X25, X20
    fmov D0, X23
    umov w20, v0.h[3]
    MOV X24, X20
    ADRP X9, L_str4@PAGE
    ADD X9, X9, L_str4@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    MOV X1, X27
    MOV X2, X26
    MOV X3, X25
    MOV X4, X24
    BL _WRITEF4
    ADRP X9, L_str5@PAGE
    ADD X9, X9, L_str5@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_quad2@PAGE
    ADD X9, X9, L_quad2@PAGEOFF
    LDR X9, [X9, #0]
    MOV X21, X9
    ADRP X9, L_str6@PAGE
    ADD X9, X9, L_str6@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    MOV X1, X21
    BL _WRITEF1
    MOVZ X9, #111
    MOVZ X10, #111
    fmov D0, X21
    ins v0.h[0], w10
    fmov X21, D0
    MOVZ X10, #222
    MOVZ X11, #222
    fmov D0, X21
    ins v0.h[1], w11
    fmov X21, D0
    MOVZ X11, #333
    MOVZ X12, #333
    fmov D0, X21
    ins v0.h[2], w12
    fmov X21, D0
    MOVZ X12, #444
    MOVZ X13, #444
    fmov D0, X21
    ins v0.h[3], w13
    fmov X21, D0
    ADRP X13, L_str7@PAGE
    ADD X13, X13, L_str7@PAGEOFF
    ADD X13, X13, #8
    MOV X0, X13
    MOV X1, X21
    BL _WRITEF1
    ADRP X9, L_str8@PAGE
    ADD X9, X9, L_str8@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    B L_START_Exit_1
L_START_Exit_1:
    B L_0
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
    .quad 0x1f
    ; (upper half)
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x51
    .long 0x55
    .long 0x41
    .long 0x44
    .long 0x20
    .long 0x76
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x6f
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
L_str1:
    .quad 0x8
    ; (upper half)
    .long 0x71
    .long 0x31
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x58
    .long 0xa
    .long 0x0
    .long 0x0
L_str2:
    .quad 0x8
    ; (upper half)
    .long 0x71
    .long 0x32
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x58
    .long 0xa
    .long 0x0
    .long 0x0
L_str3:
    .quad 0x1f
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
    .long 0x6c
    .long 0x61
    .long 0x6e
    .long 0x65
    .long 0x20
    .long 0x72
    .long 0x65
    .long 0x61
    .long 0x64
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
    .long 0x3a
    .long 0xa
    .long 0x0
    .long 0x0
L_str4:
    .quad 0x33
    ; (upper half)
    .long 0x71
    .long 0x31
    .long 0x2e
    .long 0x7c
    .long 0x30
    .long 0x7c
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x4e
    .long 0x2c
    .long 0x20
    .long 0x71
    .long 0x31
    .long 0x2e
    .long 0x7c
    .long 0x31
    .long 0x7c
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x4e
    .long 0x2c
    .long 0x20
    .long 0x71
    .long 0x31
    .long 0x2e
    .long 0x7c
    .long 0x32
    .long 0x7c
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x4e
    .long 0x2c
    .long 0x20
    .long 0x71
    .long 0x31
    .long 0x2e
    .long 0x7c
    .long 0x33
    .long 0x7c
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x4e
    .long 0xa
    .long 0x0
    .long 0x0
L_str5:
    .quad 0x20
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
    .long 0x6c
    .long 0x61
    .long 0x6e
    .long 0x65
    .long 0x20
    .long 0x77
    .long 0x72
    .long 0x69
    .long 0x74
    .long 0x65
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
    .long 0x3a
    .long 0xa
    .long 0x0
    .long 0x0
L_str6:
    .quad 0x10
    ; (upper half)
    .long 0x42
    .long 0x65
    .long 0x66
    .long 0x6f
    .long 0x72
    .long 0x65
    .long 0x3a
    .long 0x20
    .long 0x71
    .long 0x33
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x58
    .long 0xa
    .long 0x0
    .long 0x0
L_str7:
    .quad 0x1b
    ; (upper half)
    .long 0x41
    .long 0x66
    .long 0x74
    .long 0x65
    .long 0x72
    .long 0x20
    .long 0x6c
    .long 0x61
    .long 0x6e
    .long 0x65
    .long 0x20
    .long 0x77
    .long 0x72
    .long 0x69
    .long 0x74
    .long 0x65
    .long 0x73
    .long 0x3a
    .long 0x20
    .long 0x71
    .long 0x33
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x58
    .long 0xa
    .long 0x0
    .long 0x0
L_str8:
    .quad 0xf
    ; (upper half)
    .long 0xa
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
    .long 0xa
    .long 0x0
    .long 0x0
L_quad0:
    .quad 0x28001e0014000a
    ; (upper half)
L_quad1:
    .quad 0x230019000f0005
    ; (upper half)
L_quad2:
    .quad 0x190012c00c80064
    ; (upper half)

.section __DATA,__data
.p2align 3
    .long 0x0
    .long 0x0
    .long 0x0
    .long 0x0
L__data_segment_base:
