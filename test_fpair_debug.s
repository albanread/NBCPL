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
    STP X29, X30, [SP, #-80]!
    MOV X29, SP
    STP x19, x27, [x29, #48]
    STR X28, [X29, #64] ; Saved Reg: X28 @ FP+64
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
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
    MOVZ X9, #0
    ADRP X11, L_float0@PAGE
    ADD X11, X11, L_float0@PAGEOFF
    LDR D0, [X11, #0]
    FCVT S1, D0 ;encoder.
    FMOV W11, S1
    BFXIL X9, X11, #0, #32
    ADRP X12, L_float1@PAGE
    ADD X12, X12, L_float1@PAGEOFF
    LDR D0, [X12, #0]
    FCVT S1, D0 ;encoder.
    FMOV W12, S1
    BFI X9, X12, #32, #32
    MOV X15, X9
    ADRP X9, L_str2@PAGE
    ADD X9, X9, L_str2@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str3@PAGE
    ADD X9, X9, L_str3@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    fmov D1, X15
    UBFX X20, X15, #0, #32
    fmov S2, W20
    fcvt D0, S2
    FMOV X9, D0
    MOV X1, X9
    BL _WRITEF1
    ADRP X9, L_str4@PAGE
    ADD X9, X9, L_str4@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    fmov D1, X15
    UBFX X20, X15, #32, #32
    fmov S2, W20
    fcvt D0, S2
    FMOV X9, D0
    MOV X1, X9
    BL _WRITEF1
    ADRP X9, L_str5@PAGE
    ADD X9, X9, L_str5@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    MOVZ X9, #0
    ADRP X11, L_float2@PAGE
    ADD X11, X11, L_float2@PAGEOFF
    LDR D0, [X11, #0]
    FCVT S1, D0 ;encoder.
    FMOV W11, S1
    BFXIL X9, X11, #0, #32
    ADRP X12, L_float3@PAGE
    ADD X12, X12, L_float3@PAGEOFF
    LDR D0, [X12, #0]
    FCVT S1, D0 ;encoder.
    FMOV W12, S1
    BFI X9, X12, #32, #32
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
    fmov D1, X15
    UBFX X20, X15, #0, #32
    fmov S2, W20
    fcvt D0, S2
    FMOV X9, D0
    MOV X1, X9
    BL _WRITEF1
    ADRP X9, L_str8@PAGE
    ADD X9, X9, L_str8@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    fmov D1, X15
    UBFX X20, X15, #32, #32
    fmov S2, W20
    fcvt D0, S2
    FMOV X9, D0
    MOV X1, X9
    BL _WRITEF1
    ADRP X9, L_str9@PAGE
    ADD X9, X9, L_str9@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    MOVZ X9, #0
    ADRP X11, L_float4@PAGE
    ADD X11, X11, L_float4@PAGEOFF
    LDR D0, [X11, #0]
    FCVT S1, D0 ;encoder.
    FMOV W11, S1
    BFXIL X9, X11, #0, #32
    ADRP X12, L_float5@PAGE
    ADD X12, X12, L_float5@PAGEOFF
    LDR D0, [X12, #0]
    FCVT S1, D0 ;encoder.
    FMOV W12, S1
    BFI X9, X12, #32, #32
    MOV X27, X9
    ADRP X9, L_str10@PAGE
    ADD X9, X9, L_str10@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str11@PAGE
    ADD X9, X9, L_str11@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    fmov D1, X27
    UBFX X20, X27, #0, #32
    fmov S2, W20
    fcvt D0, S2
    FMOV X9, D0
    MOV X1, X9
    BL _WRITEF1
    ADRP X9, L_str12@PAGE
    ADD X9, X9, L_str12@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    fmov D1, X27
    UBFX X20, X27, #32, #32
    fmov S2, W20
    fcvt D0, S2
    FMOV X9, D0
    MOV X1, X9
    BL _WRITEF1
    ADRP X9, L_str13@PAGE
    ADD X9, X9, L_str13@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    MOVZ X9, #0
    ADRP X11, L_float0@PAGE
    ADD X11, X11, L_float0@PAGEOFF
    LDR D0, [X11, #0]
    FNEG D1, D0
    FCVT S0, D1 ;encoder.
    FMOV W11, S0
    BFXIL X9, X11, #0, #32
    ADRP X12, L_float1@PAGE
    ADD X12, X12, L_float1@PAGEOFF
    LDR D0, [X12, #0]
    FNEG D1, D0
    FCVT S0, D1 ;encoder.
    FMOV W12, S0
    BFI X9, X12, #32, #32
    MOV X27, X9
    ADRP X9, L_str14@PAGE
    ADD X9, X9, L_str14@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str15@PAGE
    ADD X9, X9, L_str15@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    fmov D1, X27
    UBFX X20, X27, #0, #32
    fmov S2, W20
    fcvt D0, S2
    FMOV X9, D0
    MOV X1, X9
    BL _WRITEF1
    ADRP X9, L_str16@PAGE
    ADD X9, X9, L_str16@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    fmov D1, X27
    UBFX X20, X27, #32, #32
    fmov S2, W20
    fcvt D0, S2
    FMOV X9, D0
    MOV X1, X9
    BL _WRITEF1
    ADRP X9, L_str17@PAGE
    ADD X9, X9, L_str17@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    B L_START_Exit_1
L_START_Exit_1:
    B L_0
L_0:
    LDP x19, x27, [x29, #48]
    LDR X28, [X29, #64] ; Restored Reg: X28 @ FP+64
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [SP, #0]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET

.section __DATA,__const
.p2align 3
L_str0:
    .quad 0x29
    ; (upper half)
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x46
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x20
    .long 0x62
    .long 0x69
    .long 0x74
    .long 0x20
    .long 0x70
    .long 0x61
    .long 0x74
    .long 0x74
    .long 0x65
    .long 0x72
    .long 0x6e
    .long 0x73
    .long 0x20
    .long 0x61
    .long 0x6e
    .long 0x64
    .long 0x20
    .long 0x64
    .long 0x65
    .long 0x62
    .long 0x75
    .long 0x67
    .long 0x67
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0xa
    .long 0x0
    .long 0x0
L_str1:
    .quad 0x1c
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
    .long 0x69
    .long 0x6d
    .long 0x70
    .long 0x6c
    .long 0x65
    .long 0x20
    .long 0x66
    .long 0x6c
    .long 0x6f
    .long 0x61
    .long 0x74
    .long 0x20
    .long 0x76
    .long 0x61
    .long 0x6c
    .long 0x75
    .long 0x65
    .long 0x73
    .long 0xa
    .long 0x0
    .long 0x0
L_str2:
    .quad 0x1b
    ; (upper half)
    .long 0x66
    .long 0x31
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
    .long 0x28
    .long 0x31
    .long 0x2e
    .long 0x30
    .long 0x2c
    .long 0x20
    .long 0x32
    .long 0x2e
    .long 0x30
    .long 0x29
    .long 0xa
    .long 0x0
    .long 0x0
L_str3:
    .quad 0xf
    ; (upper half)
    .long 0x66
    .long 0x31
    .long 0x20
    .long 0x6c
    .long 0x61
    .long 0x6e
    .long 0x65
    .long 0x20
    .long 0x30
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x46
    .long 0xa
    .long 0x0
    .long 0x0
L_str4:
    .quad 0xf
    ; (upper half)
    .long 0x66
    .long 0x31
    .long 0x20
    .long 0x6c
    .long 0x61
    .long 0x6e
    .long 0x65
    .long 0x20
    .long 0x31
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x46
    .long 0xa
    .long 0x0
    .long 0x0
L_str5:
    .quad 0x1d
    ; (upper half)
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x20
    .long 0x32
    .long 0x3a
    .long 0x20
    .long 0x4f
    .long 0x72
    .long 0x69
    .long 0x67
    .long 0x69
    .long 0x6e
    .long 0x61
    .long 0x6c
    .long 0x20
    .long 0x74
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x20
    .long 0x76
    .long 0x61
    .long 0x6c
    .long 0x75
    .long 0x65
    .long 0x73
    .long 0xa
    .long 0x0
    .long 0x0
L_str6:
    .quad 0x1d
    ; (upper half)
    .long 0x66
    .long 0x32
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
    .long 0x28
    .long 0x31
    .long 0x30
    .long 0x2e
    .long 0x30
    .long 0x2c
    .long 0x20
    .long 0x32
    .long 0x30
    .long 0x2e
    .long 0x30
    .long 0x29
    .long 0xa
    .long 0x0
    .long 0x0
L_str7:
    .quad 0xf
    ; (upper half)
    .long 0x66
    .long 0x32
    .long 0x20
    .long 0x6c
    .long 0x61
    .long 0x6e
    .long 0x65
    .long 0x20
    .long 0x30
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x46
    .long 0xa
    .long 0x0
    .long 0x0
L_str8:
    .quad 0xf
    ; (upper half)
    .long 0x66
    .long 0x32
    .long 0x20
    .long 0x6c
    .long 0x61
    .long 0x6e
    .long 0x65
    .long 0x20
    .long 0x31
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x46
    .long 0xa
    .long 0x0
    .long 0x0
L_str9:
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
    .long 0x44
    .long 0x69
    .long 0x66
    .long 0x66
    .long 0x65
    .long 0x72
    .long 0x65
    .long 0x6e
    .long 0x74
    .long 0x20
    .long 0x76
    .long 0x61
    .long 0x6c
    .long 0x75
    .long 0x65
    .long 0x73
    .long 0xa
    .long 0x0
    .long 0x0
L_str10:
    .quad 0x23
    ; (upper half)
    .long 0x66
    .long 0x33
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
    .long 0x28
    .long 0x33
    .long 0x2e
    .long 0x31
    .long 0x34
    .long 0x31
    .long 0x35
    .long 0x39
    .long 0x2c
    .long 0x20
    .long 0x32
    .long 0x2e
    .long 0x37
    .long 0x31
    .long 0x38
    .long 0x32
    .long 0x38
    .long 0x29
    .long 0xa
    .long 0x0
    .long 0x0
L_str11:
    .quad 0xf
    ; (upper half)
    .long 0x66
    .long 0x33
    .long 0x20
    .long 0x6c
    .long 0x61
    .long 0x6e
    .long 0x65
    .long 0x20
    .long 0x30
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x46
    .long 0xa
    .long 0x0
    .long 0x0
L_str12:
    .quad 0xf
    ; (upper half)
    .long 0x66
    .long 0x33
    .long 0x20
    .long 0x6c
    .long 0x61
    .long 0x6e
    .long 0x65
    .long 0x20
    .long 0x31
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x46
    .long 0xa
    .long 0x0
    .long 0x0
L_str13:
    .quad 0x18
    ; (upper half)
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x20
    .long 0x34
    .long 0x3a
    .long 0x20
    .long 0x4e
    .long 0x65
    .long 0x67
    .long 0x61
    .long 0x74
    .long 0x69
    .long 0x76
    .long 0x65
    .long 0x20
    .long 0x76
    .long 0x61
    .long 0x6c
    .long 0x75
    .long 0x65
    .long 0x73
    .long 0xa
    .long 0x0
    .long 0x0
L_str14:
    .quad 0x1d
    ; (upper half)
    .long 0x66
    .long 0x34
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
    .long 0x28
    .long 0x2d
    .long 0x31
    .long 0x2e
    .long 0x30
    .long 0x2c
    .long 0x20
    .long 0x2d
    .long 0x32
    .long 0x2e
    .long 0x30
    .long 0x29
    .long 0xa
    .long 0x0
    .long 0x0
L_str15:
    .quad 0xf
    ; (upper half)
    .long 0x66
    .long 0x34
    .long 0x20
    .long 0x6c
    .long 0x61
    .long 0x6e
    .long 0x65
    .long 0x20
    .long 0x30
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x46
    .long 0xa
    .long 0x0
    .long 0x0
L_str16:
    .quad 0xf
    ; (upper half)
    .long 0x66
    .long 0x34
    .long 0x20
    .long 0x6c
    .long 0x61
    .long 0x6e
    .long 0x65
    .long 0x20
    .long 0x31
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x46
    .long 0xa
    .long 0x0
    .long 0x0
L_str17:
    .quad 0x16
    ; (upper half)
    .long 0x45
    .long 0x6e
    .long 0x64
    .long 0x20
    .long 0x6f
    .long 0x66
    .long 0x20
    .long 0x64
    .long 0x65
    .long 0x62
    .long 0x75
    .long 0x67
    .long 0x67
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x74
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0xa
    .long 0x0
    .long 0x0
L_float0:
    .quad 0x3ff0000000000000
    ; (upper half)
L_float1:
    .quad 0x4000000000000000
    ; (upper half)
L_float2:
    .quad 0x4024000000000000
    ; (upper half)
L_float3:
    .quad 0x4034000000000000
    ; (upper half)
L_float4:
    .quad 0x400921f9f01b866e
    ; (upper half)
L_float5:
    .quad 0x4005bf0995aaf790
    ; (upper half)

.section __DATA,__data
.p2align 3
    .long 0x0
    .long 0x0
    .long 0x0
    .long 0x0
L__data_segment_base:
