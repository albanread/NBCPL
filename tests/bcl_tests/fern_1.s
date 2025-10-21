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

L_init_fern:
    STP X29, X30, [SP, #-48]!
    MOV X29, SP
    STR D15, [X29, #16] ; Saved Reg: D15 @ FP+16
    STP x19, x27, [x29, #24]
    STR X28, [X29, #40] ; Saved Reg: X28 @ FP+40
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
L_init_fern_Entry_0:
    ADRP X10, L_float0@PAGE
    ADD X10, X10, L_float0@PAGEOFF
    LDR D0, [X10, #0]
    STR D0, [X28, #0]
    FMOV D15, D0
    ADRP X11, L_float0@PAGE
    ADD X11, X11, L_float0@PAGEOFF
    LDR D0, [X11, #0]
    STR D0, [X28, #8]
    FMOV D15, D0
    MOVZ X11, #0
    STR X11, [X28, #16] ; point_count
    MOV X27, X11
    B L_init_fern_Exit_1
L_init_fern_Exit_1:
    B L_0
L_0:
    LDR D15, [X29, #16] ; Restored Reg: D15 @ FP+16
    LDP x19, x27, [x29, #24]
    LDR X28, [X29, #40] ; Restored Reg: X28 @ FP+40
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [SP, #0]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_iterate_fern:
    STP X29, X30, [SP, #-128]!
    MOV X29, SP
    STR D11, [X29, #40] ; Saved Reg: D11 @ FP+40
    STR D12, [X29, #48] ; Saved Reg: D12 @ FP+48
    STR D13, [X29, #56] ; Saved Reg: D13 @ FP+56
    STR D14, [X29, #64] ; Saved Reg: D14 @ FP+64
    STR D15, [X29, #72] ; Saved Reg: D15 @ FP+72
    STP x19, x20, [x29, #80]
    STP x21, x22, [x29, #96]
    STP x27, x28, [x29, #112]
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
L_iterate_fern_Entry_0:
    ADRP X12, L_float0@PAGE
    ADD X12, X12, L_float0@PAGEOFF
    LDR D0, [X12, #0]
    FMOV D13, D0
    FMOV D13, D0
    ADRP X13, L_float0@PAGE
    ADD X13, X13, L_float0@PAGEOFF
    LDR D0, [X13, #0]
    FMOV D11, D0
    FMOV D11, D0
    BL _FRND
    FMOV D12, D0
    FMOV D12, D0
    ADRP X14, L_float1@PAGE
    ADD X14, X14, L_float1@PAGEOFF
    LDR D0, [X14, #0]
    FMOV D1, D12
    FCMP D1, D0
    CSET X14, LT
    CMP X14, XZR
    B.EQ L_iterate_fern_Else_2
    B L_iterate_fern_Then_1
L_iterate_fern_Else_2:
    ADRP X15, L_float2@PAGE
    ADD X15, X15, L_float2@PAGEOFF
    LDR D0, [X15, #0]
    FMOV D1, D12
    FCMP D1, D0
    CSET X15, LT
    CMP X15, XZR
    B.EQ L_iterate_fern_Else_5
    B L_iterate_fern_Then_4
L_iterate_fern_Else_5:
    ADRP X9, L_float3@PAGE
    ADD X9, X9, L_float3@PAGEOFF
    LDR D0, [X9, #0]
    FMOV D1, D12
    FCMP D1, D0
    CSET X9, LT
    CMP X9, XZR
    B.EQ L_iterate_fern_Else_8
    B L_iterate_fern_Then_7
L_iterate_fern_Else_8:
    ADRP X10, L_float4@PAGE
    ADD X10, X10, L_float4@PAGEOFF
    LDR D0, [X10, #0]
    LDR D1, [X28, #0]
    FMUL D0, D0, D1
    ADRP X11, L_float5@PAGE
    ADD X11, X11, L_float5@PAGEOFF
    LDR D1, [X11, #0]
    LDR D2, [X28, #8]
    FMUL D1, D1, D2
    FADD D0, D0, D1
    FMOV D13, D0
    FMOV D13, D0
    ADRP X12, L_float6@PAGE
    ADD X12, X12, L_float6@PAGEOFF
    LDR D0, [X12, #0]
    LDR D1, [X28, #0]
    FMUL D0, D0, D1
    ADRP X13, L_float7@PAGE
    ADD X13, X13, L_float7@PAGEOFF
    LDR D1, [X13, #0]
    LDR D2, [X28, #8]
    FMUL D1, D1, D2
    FADD D0, D0, D1
    ADRP X14, L_float8@PAGE
    ADD X14, X14, L_float8@PAGEOFF
    LDR D1, [X14, #0]
    FADD D0, D0, D1
    FMOV D11, D0
    FMOV D11, D0
    B L_iterate_fern_Join_3
L_iterate_fern_Exit_10:
    B L_1
L_iterate_fern_Join_3:
    ADRP X14, L_str0_plus_8@PAGE
    ADD X14, X14, L_str0_plus_8@PAGEOFF
    MOV X0, X14
    FMOV X14, D13
    MOV X1, X14
    FMOV X14, D11
    MOV X2, X14
    LDR X14, [X28, #16] ; point_count
    MOV X3, X14
    BL _WRITEF3
    STR D13, [X28, #0]
    FMOV D15, D13
    STR D11, [X28, #8]
    FMOV D14, D11
    LDR X9, [X28, #16] ; point_count
    ADD X9, X9, #1
    B L_iterate_fern_Exit_10
L_iterate_fern_Then_1:
    ADRP X11, L_float0@PAGE
    ADD X11, X11, L_float0@PAGEOFF
    LDR D0, [X11, #0]
    FMOV D13, D0
    FMOV D13, D0
    ADRP X12, L_float9@PAGE
    ADD X12, X12, L_float9@PAGEOFF
    LDR D0, [X12, #0]
    LDR D1, [X28, #8]
    FMUL D0, D0, D1
    FMOV D11, D0
    FMOV D11, D0
    B L_iterate_fern_Join_3
L_iterate_fern_Then_4:
    ADRP X13, L_float10@PAGE
    ADD X13, X13, L_float10@PAGEOFF
    LDR D0, [X13, #0]
    LDR D1, [X28, #0]
    FMUL D0, D0, D1
    ADRP X14, L_float11@PAGE
    ADD X14, X14, L_float11@PAGEOFF
    LDR D1, [X14, #0]
    LDR D2, [X28, #8]
    FMUL D1, D1, D2
    FADD D0, D0, D1
    FMOV D13, D0
    FMOV D13, D0
    ADRP X15, L_float12@PAGE
    ADD X15, X15, L_float12@PAGEOFF
    LDR D0, [X15, #0]
    LDR D1, [X28, #0]
    FMUL D0, D0, D1
    ADRP X9, L_float10@PAGE
    ADD X9, X9, L_float10@PAGEOFF
    LDR D1, [X9, #0]
    LDR D2, [X28, #8]
    FMUL D1, D1, D2
    FADD D0, D0, D1
    ADRP X10, L_float13@PAGE
    ADD X10, X10, L_float13@PAGEOFF
    LDR D1, [X10, #0]
    FADD D0, D0, D1
    FMOV D11, D0
    FMOV D11, D0
    B L_iterate_fern_Join_3
L_iterate_fern_Then_7:
    ADRP X11, L_float14@PAGE
    ADD X11, X11, L_float14@PAGEOFF
    LDR D0, [X11, #0]
    LDR D1, [X28, #0]
    FMUL D0, D0, D1
    ADRP X12, L_float6@PAGE
    ADD X12, X12, L_float6@PAGEOFF
    LDR D1, [X12, #0]
    LDR D2, [X28, #8]
    FMUL D1, D1, D2
    FSUB D0, D0, D1
    FMOV D13, D0
    FMOV D13, D0
    ADRP X13, L_float15@PAGE
    ADD X13, X13, L_float15@PAGEOFF
    LDR D0, [X13, #0]
    LDR D1, [X28, #0]
    FMUL D0, D0, D1
    ADRP X14, L_float16@PAGE
    ADD X14, X14, L_float16@PAGEOFF
    LDR D1, [X14, #0]
    LDR D2, [X28, #8]
    FMUL D1, D1, D2
    FADD D0, D0, D1
    ADRP X15, L_float13@PAGE
    ADD X15, X15, L_float13@PAGEOFF
    LDR D1, [X15, #0]
    FADD D0, D0, D1
    FMOV D11, D0
    FMOV D11, D0
    B L_iterate_fern_Join_3
L_1:
    LDR D11, [X29, #40] ; Restored Reg: D11 @ FP+40
    LDR D12, [X29, #48] ; Restored Reg: D12 @ FP+48
    LDR D13, [X29, #56] ; Restored Reg: D13 @ FP+56
    LDR D14, [X29, #64] ; Restored Reg: D14 @ FP+64
    LDR D15, [X29, #72] ; Restored Reg: D15 @ FP+72
    LDP x19, x20, [x29, #80]
    LDP x21, x22, [x29, #96]
    LDP x27, x28, [x29, #112]
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [SP, #0]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_Global:
    STP X29, X30, [SP, #-48]!
    MOV X29, SP
    STR D15, [X29, #16] ; Saved Reg: D15 @ FP+16
    STP x19, x27, [x29, #24]
    STR X28, [X29, #40] ; Saved Reg: X28 @ FP+40
L_Global_Entry_0:
    ADRP X9, L_float0@PAGE
    ADD X9, X9, L_float0@PAGEOFF
    LDR D0, [X9, #0]
    STR D0, [X28, #0]
    FMOV D15, D0
    ADRP X10, L_float0@PAGE
    ADD X10, X10, L_float0@PAGEOFF
    LDR D0, [X10, #0]
    STR D0, [X28, #8]
    FMOV D15, D0
    MOVZ X10, #0
    STR X10, [X28, #16] ; point_count
    MOV X27, X10
    B L_Global_Exit_1
L_Global_Exit_1:
    B L_2
L_2:
    LDR D15, [X29, #16] ; Restored Reg: D15 @ FP+16
    LDP x19, x27, [x29, #24]
    LDR X28, [X29, #40] ; Restored Reg: X28 @ FP+40
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [SP, #0]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_START:
    STP X29, X30, [SP, #-64]!
    MOV X29, SP
    STP x19, x26, [x29, #24]
    STP x27, x28, [x29, #40]
L_START_Entry_0:
    BL L_Global
    BL L_init_fern
    MOVZ X9, #1
    MOV X27, X9
    B L_START_ForHeader_1
L_START_Exit_5:
    B L_3
L_START_ForBody_2:
    BL L_iterate_fern
    B L_START_ForIncrement_3
L_START_ForHeader_1:
    MOV X9, X27
    MOVZ X10, #10000
    CMP X9, X10
    B.GT L_START_Exit_5
    B L_START_ForBody_2
L_START_ForIncrement_3:
    ADD X27, X27, #1
    B L_START_ForHeader_1
L_3:
    LDP x19, x26, [x29, #24]
    LDP x27, x28, [x29, #40]
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [SP, #0]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET

.section __DATA,__const
.p2align 3
L_str0:
    .quad 0x16
    ; (upper half)
.p2align 2
L_str0_plus_8:
    .long 0x78
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x66
    .long 0x20
    .long 0x79
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x66
    .long 0x20
    .long 0x63
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x64
    .long 0x20
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_float0:
    .quad 0x0
    ; (upper half)
L_float1:
    .quad 0x3f847ae147ae147b
    ; (upper half)
L_float2:
    .quad 0x3feb851eb851eb85
    ; (upper half)
L_float3:
    .quad 0x3fedc28f5c28f5c3
    ; (upper half)
L_float4:
    .quad 0xbfc3333333333333
    ; (upper half)
L_float5:
    .quad 0x3fd1eb851eb851ec
    ; (upper half)
L_float6:
    .quad 0x3fd0a3d70a3d70a4
    ; (upper half)
L_float7:
    .quad 0x3fceb851eb851eb8
    ; (upper half)
L_float8:
    .quad 0x3fdc28f5c28f5c29
    ; (upper half)
L_float9:
    .quad 0x3fc47ae147ae147b
    ; (upper half)
L_float10:
    .quad 0x3feb333333333333
    ; (upper half)
L_float11:
    .quad 0x3fa47ae147ae147b
    ; (upper half)
L_float12:
    .quad 0xbfa47ae147ae147b
    ; (upper half)
L_float13:
    .quad 0x3ff999999999999a
    ; (upper half)
L_float14:
    .quad 0x3fc999999999999a
    ; (upper half)
L_float15:
    .quad 0x3fcd70a3d70a3d71
    ; (upper half)
L_float16:
    .quad 0x3fcc28f5c28f5c29
    ; (upper half)

.section __DATA,__data
.p2align 3
L__data_segment_base:
    .quad 0x0
    ; (upper half)
    .quad 0x0
    ; (upper half)
    .quad 0x0
    ; (upper half)
