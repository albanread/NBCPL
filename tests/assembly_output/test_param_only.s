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
L_IntRoutine:
    STP X29, X30, [SP, #-80]!
    MOV X29, SP
    STR X19, [X29, #32] ; Saved Reg: X19 @ FP+32
    STR X20, [X29, #40] ; Saved Reg: X20 @ FP+40
    STR X21, [X29, #48] ; Saved Reg: X21 @ FP+48
    STR X26, [X29, #56] ; Saved Reg: X26 @ FP+56
    STR X27, [X29, #64] ; Saved Reg: X27 @ FP+64
    STR X28, [X29, #72] ; Saved Reg: X28 @ FP+72
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
    MOVZ X9, #8, LSL #16
    ADD X19, X28, X9
L_IntRoutine_Entry_0:
    MOV X9, X27
    ADD X9, X9, X26
    MOV X20, X9
    MOV X0, X20
    BL _WRITEN
    B L_IntRoutine_Exit_1
L_IntRoutine_Exit_1:
    B L_0
L_0:
    LDR X19, [X29, #32] ; Restored Reg: X19 @ FP+32
    LDR X20, [X29, #40] ; Restored Reg: X20 @ FP+40
    LDR X21, [X29, #48] ; Restored Reg: X21 @ FP+48
    LDR X26, [X29, #56] ; Restored Reg: X26 @ FP+56
    LDR X27, [X29, #64] ; Restored Reg: X27 @ FP+64
    LDR X28, [X29, #72] ; Restored Reg: X28 @ FP+72
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_FloatRoutine:
    STP X29, X30, [SP, #-80]!
    MOV X29, SP
    STR D14, [X29, #32] ; Saved Reg: D14 @ FP+32
    STR D15, [X29, #40] ; Saved Reg: D15 @ FP+40
    STR X19, [X29, #48] ; Saved Reg: X19 @ FP+48
    STR X20, [X29, #56] ; Saved Reg: X20 @ FP+56
    STR X21, [X29, #64] ; Saved Reg: X21 @ FP+64
    STR X28, [X29, #72] ; Saved Reg: X28 @ FP+72
    FMOV D15, D0
    FMOV D14, D1
L_FloatRoutine_Entry_0:
    FMOV D0, D15
    FADD D0, D0, D14
    ADRP X10, L_float0@PAGE
    ADD X10, X10, L_float0@PAGEOFF
    LDR D1, [X10, #0]
    FADD D0, D0, D1
    B L_1
    B L_FloatRoutine_Exit_1
L_FloatRoutine_Exit_1:
    B L_1
L_1:
    LDR D14, [X29, #32] ; Restored Reg: D14 @ FP+32
    LDR D15, [X29, #40] ; Restored Reg: D15 @ FP+40
    LDR X19, [X29, #48] ; Restored Reg: X19 @ FP+48
    LDR X20, [X29, #56] ; Restored Reg: X20 @ FP+56
    LDR X21, [X29, #64] ; Restored Reg: X21 @ FP+64
    LDR X28, [X29, #72] ; Restored Reg: X28 @ FP+72
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_MixedRoutine:
    STP X29, X30, [SP, #-64]!
    MOV X29, SP
    STR X19, [X29, #32] ; Saved Reg: X19 @ FP+32
    STR X20, [X29, #40] ; Saved Reg: X20 @ FP+40
    STR X27, [X29, #48] ; Saved Reg: X27 @ FP+48
    STR X28, [X29, #56] ; Saved Reg: X28 @ FP+56
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
    MOVZ X10, #8, LSL #16
    ADD X19, X28, X10
L_MixedRoutine_Entry_0:
    MOVZ X10, #10
    MOV X11, X27
    ADD X11, X11, X10
    MOV X20, X11
    MOV X0, X20
    BL _WRITEN
    B L_MixedRoutine_Exit_1
L_MixedRoutine_Exit_1:
    B L_2
L_2:
    LDR X19, [X29, #32] ; Restored Reg: X19 @ FP+32
    LDR X20, [X29, #40] ; Restored Reg: X20 @ FP+40
    LDR X27, [X29, #48] ; Restored Reg: X27 @ FP+48
    LDR X28, [X29, #56] ; Restored Reg: X28 @ FP+56
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_START:
    STP X29, X30, [SP, #-64]!
    MOV X29, SP
    STR D15, [X29, #32] ; Saved Reg: D15 @ FP+32
    STR X19, [X29, #40] ; Saved Reg: X19 @ FP+40
    STR X27, [X29, #48] ; Saved Reg: X27 @ FP+48
    STR X28, [X29, #56] ; Saved Reg: X28 @ FP+56
L_START_Entry_0:
    MOVZ X9, #42
    MOV X27, X9
    ADRP X10, L_float1@PAGE
    ADD X10, X10, L_float1@PAGEOFF
    LDR D0, [X10, #0]
    FMOV D15, D0
    FMOV D15, D0
    MOVZ X10, #10
    MOV X20, X10
    MOVZ X10, #20
    MOV X21, X10
    MOV X0, X20
    MOV X1, X21
    BL L_IntRoutine
    ADRP X10, L_float0@PAGE
    ADD X10, X10, L_float0@PAGEOFF
    LDR D0, [X10, #0]
    FMOV D8, D0
    ADRP X11, L_float2@PAGE
    ADD X11, X11, L_float2@PAGEOFF
    LDR D0, [X11, #0]
    FMOV D9, D0
    FMOV D0, D8
    FMOV D1, D9
    BL L_FloatRoutine
    MOVZ X9, #100
    MOV X20, X9
    ADRP X10, L_float3@PAGE
    ADD X10, X10, L_float3@PAGEOFF
    LDR D0, [X10, #0]
    FMOV D8, D0
    MOV X0, X20
    FMOV D0, D8
    BL L_MixedRoutine
    MOV X20, X27
    FMOV D8, D15
    MOV X0, X20
    FMOV D0, D8
    BL L_IntRoutine
    MOV X20, X27
    FMOV D8, D15
    MOV X0, X20
    FMOV D0, D8
    BL L_FloatRoutine
    B L_START_BlockCleanup_1
L_START_BlockCleanup_1:
    B L_START_Exit_2
L_START_Exit_2:
    B L_3
L_3:
    LDR D15, [X29, #32] ; Restored Reg: D15 @ FP+32
    LDR X19, [X29, #40] ; Restored Reg: X19 @ FP+40
    LDR X27, [X29, #48] ; Restored Reg: X27 @ FP+48
    LDR X28, [X29, #56] ; Restored Reg: X28 @ FP+56
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET

.section __DATA,__const
.p2align 3
L_float0:
    .quad 0x3ff8000000000000
    ; (upper half)
L_float1:
    .quad 0x40091eb851eb851f
    ; (upper half)
L_float2:
    .quad 0x4004000000000000
    ; (upper half)
L_float3:
    .quad 0x4058f9999999999a
    ; (upper half)

.section __DATA,__data
.p2align 3
L__data_segment_base:
