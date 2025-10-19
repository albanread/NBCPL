.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _START
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
.globl _FIND
.globl _SDL2_GET_EVENT_BUTTON
.globl _LPND
.globl _FGETVEC
.globl _SLURP
.globl _DEEPCOPYLIST
.globl _BCPL_FREE_LIST_SAFE
.globl _WRITEC
.globl _TIMER_DISPLAY
.globl _FILE_READ
.globl _UNPACKSTRING
.globl _SDL2_DELAY
.globl _FREEVEC
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
.globl _LIST_HEAD_INT
.globl _STRCOPY
.globl _LIST_APPEND_STRING
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
.globl _SDL2_DRAW_POINT
.globl _HEAPMANAGER_EXIT_SCOPE
.globl _FILE_OPEN_WRITE
.globl _SDL2_GET_TICKS
.globl _WRITEF6
.globl _HEAPMANAGER_SETSAMMENABLED
.globl _SETTYPE
.globl _FILE_SEEK
.globl _SDL2_GET_VERSION
.globl _SDL2_TEST_BASIC
.globl _OBJECT_HEAP_ALLOC
.globl _FILE_WRITES
.globl _PIC_RUNTIME_HELPER
.globl _SDL2_DRAW_RECT
.globl _TIMER_CLEAR
.globl _SDL2_CLEAR_ERROR
.globl _SDL2_SET_WINDOW_SIZE
.globl _SDL2_CREATE_RENDERER
.globl _WRITEF4
.globl _FEXP
.globl _FPND
.globl _FIX
.globl _WRITEF1
.globl _FILE_WRITE
.globl _TIMER_GET_CALL_COUNT
.globl _COPYLIST
.globl _SDL2_GET_DISPLAY_MODES
.globl _BCPL_GET_LAST_ERROR
.globl _SDL2_PRESENT
.globl _SPND
.globl _SDL2_CREATE_RENDERER_EX
.globl _BCPL_LIST_GET_REST
.globl _SDL2_DESTROY_RENDERER
.globl _SDL2_CLEAR
.globl _WRITEF7
.globl _SDL2_DESTROY_WINDOW
.globl _SDL2_SET_WINDOW_TITLE
.globl _HEAPMANAGER_WAITFORSAMM
.globl _SDL2_POLL_EVENT
.globl _LIST_TAIL
.globl _SDL2_GET_ERROR
.globl _FILE_EOF
.globl _SDL2_CREATE_WINDOW
.globl _TIMER_GET_TOTAL_NS
.globl _RUNTIME_METHOD_LOOKUP
.globl _TIMER_START
.globl _SDL2_SET_DRAW_COLOR
.globl _CONCAT
.globl _SDL2_DRAW_LINE
.globl _OBJECT_HEAP_FREE
.globl _TIMER_END
.p2align 2
_start:
_START:
    B L_START
    
; --- Veneer Section ---
    ; --- End Veneer Section ---

L_start:
    STP X29, X30, [SP, #-64]!
    MOV X29, SP
    STR D15, [X29, #32] ; Saved Reg: D15 @ FP+32
    STP x19, x27, [x29, #40]
    STR X28, [X29, #56] ; Saved Reg: X28 @ FP+56
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
L_start_Entry_0:
    MOVZ X9, #42
    MOV X27, X9
    ADRP X10, L_float0@PAGE
    ADD X10, X10, L_float0@PAGEOFF
    LDR D0, [X10, #0]
    FMOV D15, D0
    FMOV D15, D0
    SUB SP, SP, #32
    MOVZ X10, #52
    STR X10, [SP, #16]
    STR D15, [SP, #8]
    MOVZ X10, #42
    STR X10, [SP, #0]
    LDP x11, x12, [SP, #8]
    MOV X0, X10
    SCVTF D0, X11
    MOV X1, X12
    ADD SP, SP, #32
    BL L_test_mixed_args
    SUB SP, SP, #16
    MOVZ X9, #42
    STR X9, [SP, #8]
    STR D15, [SP, #0]
    LDP x9, x10, [SP, #0]
    SCVTF D0, X9
    MOV X0, X10
    ADD SP, SP, #16
    BL L_test_mixed_routine
    ADRP X9, L_str0_plus_8@PAGE
    ADD X9, X9, L_str0_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    B L_start_Exit_1
L_start_Exit_1:
    B L_0
L_0:
    LDR D15, [X29, #32] ; Restored Reg: D15 @ FP+32
    LDP x19, x27, [x29, #40]
    LDR X28, [X29, #56] ; Restored Reg: X28 @ FP+56
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [SP, #0]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_test_mixed_args:
    STP X29, X30, [SP, #-96]!
    MOV X29, SP
    STP x19, x20, [x29, #40]
    STP x21, x25, [x29, #56]
    STP x26, x27, [x29, #72]
    STR X28, [X29, #88] ; Saved Reg: X28 @ FP+88
    MOV X25, X0    // Move parameter 'int_arg' from X0 to X25
    MOV X26, X1    // Move parameter 'float_arg' from X1 to X26
    MOV X27, X2    // Move parameter 'another_int' from X2 to X27
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
L_test_mixed_args_Entry_0:
    ADRP X9, L_str1_plus_8@PAGE
    ADD X9, X9, L_str1_plus_8@PAGEOFF
    MOV X0, X9
    MOV X1, X25
    MOV X2, X26
    MOV X3, X27
    BL _WRITEF3
    B L_test_mixed_args_Exit_1
L_test_mixed_args_Exit_1:
    B L_1
L_1:
    LDP x19, x20, [x29, #40]
    LDP x21, x25, [x29, #56]
    LDP x26, x27, [x29, #72]
    LDR X28, [X29, #88] ; Restored Reg: X28 @ FP+88
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [SP, #0]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_test_mixed_routine:
    STP X29, X30, [SP, #-80]!
    MOV X29, SP
    STP x19, x20, [x29, #32]
    STP x26, x27, [x29, #48]
    STR X28, [X29, #64] ; Saved Reg: X28 @ FP+64
    MOV X27, X0    // Move parameter 'f_arg' from X0 to X27
    MOV X26, X1    // Move parameter 'i_arg' from X1 to X26
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
L_test_mixed_routine_Entry_0:
    ADRP X9, L_str2_plus_8@PAGE
    ADD X9, X9, L_str2_plus_8@PAGEOFF
    MOV X0, X9
    MOV X1, X27
    MOV X2, X26
    BL _WRITEF2
    B L_test_mixed_routine_Exit_1
L_test_mixed_routine_Exit_1:
    B L_2
L_2:
    LDP x19, x20, [x29, #32]
    LDP x26, x27, [x29, #48]
    LDR X28, [X29, #64] ; Restored Reg: X28 @ FP+64
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [SP, #0]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET

.section __DATA,__const
.p2align 3
L_str0:
    .quad 0x22
    ; (upper half)
.p2align 2
L_str0_plus_8:
    .long 0x54
    .long 0x79
    .long 0x70
    .long 0x65
    .long 0x2d
    .long 0x61
    .long 0x77
    .long 0x61
    .long 0x72
    .long 0x65
    .long 0x20
    .long 0x6c
    .long 0x6f
    .long 0x61
    .long 0x64
    .long 0x69
    .long 0x6e
    .long 0x67
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
    .long 0x64
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str1:
    .quad 0x24
    ; (upper half)
.p2align 2
L_str1_plus_8:
    .long 0x52
    .long 0x65
    .long 0x63
    .long 0x65
    .long 0x69
    .long 0x76
    .long 0x65
    .long 0x64
    .long 0x3a
    .long 0x20
    .long 0x69
    .long 0x6e
    .long 0x74
    .long 0x3d
    .long 0x25
    .long 0x4e
    .long 0x2c
    .long 0x20
    .long 0x66
    .long 0x6c
    .long 0x6f
    .long 0x61
    .long 0x74
    .long 0x3d
    .long 0x25
    .long 0x46
    .long 0x2c
    .long 0x20
    .long 0x69
    .long 0x6e
    .long 0x74
    .long 0x32
    .long 0x3d
    .long 0x25
    .long 0x4e
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str2:
    .quad 0x23
    ; (upper half)
.p2align 2
L_str2_plus_8:
    .long 0x52
    .long 0x6f
    .long 0x75
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x65
    .long 0x20
    .long 0x72
    .long 0x65
    .long 0x63
    .long 0x65
    .long 0x69
    .long 0x76
    .long 0x65
    .long 0x64
    .long 0x3a
    .long 0x20
    .long 0x66
    .long 0x6c
    .long 0x6f
    .long 0x61
    .long 0x74
    .long 0x3d
    .long 0x25
    .long 0x46
    .long 0x2c
    .long 0x20
    .long 0x69
    .long 0x6e
    .long 0x74
    .long 0x3d
    .long 0x25
    .long 0x4e
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_float0:
    .quad 0x40091eb851eb851f
    ; (upper half)

.section __DATA,__data
.p2align 3
    .long 0x0
.p2align 2
L__data_segment_base:
