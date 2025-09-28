.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _START
.globl _WRITEN
.globl _SDL2_SET_DRAW_COLOR
.globl _WRITES
.globl _GET_FREE_LIST_HEAD_ADDR
.globl _BCPL_LIST_APPEND_STRING
.globl _WRITEF
.globl _CONCAT
.globl _WRITEF2
.globl _WRITEF6
.globl _SDL2_DESTROY_RENDERER
.globl _BCPL_LIST_GET_NTH
.globl _FILE_OPEN_READ
.globl _BCPL_CHECK_AND_DISPLAY_ERRORS
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
.globl _FPND
.globl _FEXP
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
.globl _BCPL_FREE_LIST_SAFE
.globl _DEEPCOPYLIST
.globl _BCPL_FREE_LIST
.globl _FILE_CLOSE
.globl _FIND
.globl _WRITEF3
.globl _BCPL_GET_ATOM_TYPE
.globl _FILE_READS
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
.globl _FTAN
.globl _SPIT
.globl _FRND
.globl _OBJECT_HEAP_ALLOC
.globl _SDL2_TEST_BASIC
.globl _BCPL_LIST_GET_TAIL
.globl _REVERSE
.globl _BCPL_BOUNDS_ERROR
.globl _FILE_EOF
.globl _STRCMP
.globl _SDL2_GET_VIDEO_DRIVERS
.globl _FINISH
.globl _RDCH
.globl _SDL2_GET_EVENT_MOUSE
.globl _SPLIT
.globl _FILE_WRITES
.globl _SDL2_QUIT
.globl _SDL2_INIT
.globl _SDL2_SET_WINDOW_TITLE
.globl _WRITEF7
.globl _SDL2_DESTROY_WINDOW
.globl _FABS
.globl _DEEPCOPYLITERALLIST
.globl _BCPL_LIST_GET_REST
.globl _SDL2_CLEAR
.globl _SDL2_CREATE_RENDERER
.globl _SDL2_CLEAR_ERROR
.globl _SDL2_SET_WINDOW_SIZE
.globl _SETTYPE
.globl _FILE_SEEK
.globl _SDL2_GET_VERSION
.globl _SDL2_GET_EVENT_BUTTON
.globl _STRCOPY
.globl _FIX
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
.globl _FILE_OPEN_WRITE
.globl _SDL2_GET_TICKS
.globl ___SDL2_STATIC_BUILD
.globl _STRLEN
.globl _SDL2_DRAW_POINT
.globl _SDL2_POLL_EVENT
.globl _SDL2_GET_ERROR
.p2align 2
_start:
_START:
    B L_START
L_init_fern:
    STP X29, X30, [SP, #-32]!
    MOV X29, SP
    STR X19, [X29, #16] ; Saved Reg: X19 @ FP+16
    STR X28, [X29, #24] ; Saved Reg: X28 @ FP+24
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
    MOVZ X9, #8, LSL #16
    ADD X19, X28, X9
L_init_fern_Entry_0:
    ADRP X10, L_float0@PAGE
    ADD X10, X10, L_float0@PAGEOFF
    LDR D0, [X10, #0]
    STR D0, [X28, #0]
    FMOV D31, D0
    ADRP X11, L_float0@PAGE
    ADD X11, X11, L_float0@PAGEOFF
    LDR D0, [X11, #0]
    STR D0, [X28, #8]
    FMOV D31, D0
    MOVZ X11, #0
    STR X11, [X28, #16] ; point_count
    MOV X15, X11
    B L_init_fern_Exit_1
L_init_fern_Exit_1:
    B L_0
L_0:
    LDR X19, [X29, #16] ; Restored Reg: X19 @ FP+16
    LDR X28, [X29, #24] ; Restored Reg: X28 @ FP+24
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_iterate_fern:
    STP X29, X30, [SP, #-128]!
    MOV X29, SP
    STR D13, [X29, #40] ; Saved Reg: D13 @ FP+40
    STR D14, [X29, #48] ; Saved Reg: D14 @ FP+48
    STR D15, [X29, #56] ; Saved Reg: D15 @ FP+56
    STR X19, [X29, #64] ; Saved Reg: X19 @ FP+64
    STR X20, [X29, #72] ; Saved Reg: X20 @ FP+72
    STR X21, [X29, #80] ; Saved Reg: X21 @ FP+80
    STR X22, [X29, #88] ; Saved Reg: X22 @ FP+88
    STR X23, [X29, #96] ; Saved Reg: X23 @ FP+96
    STR X27, [X29, #104] ; Saved Reg: X27 @ FP+104
    STR X28, [X29, #112] ; Saved Reg: X28 @ FP+112
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
    MOVZ X11, #8, LSL #16
    ADD X19, X28, X11
L_iterate_fern_Entry_0:
    ADRP X12, L_float0@PAGE
    ADD X12, X12, L_float0@PAGEOFF
    LDR D0, [X12, #0]
    FMOV D13, D0
    FMOV D13, D0
    ADRP X13, L_float0@PAGE
    ADD X13, X13, L_float0@PAGEOFF
    LDR D0, [X13, #0]
    FMOV D31, D0
    FMOV D31, D0
    LDR X13, [X19, #584]
    BLR X13
    FMOV D30, D0
    FMOV D30, D0
    ADRP X14, L_float1@PAGE
    ADD X14, X14, L_float1@PAGEOFF
    LDR D0, [X14, #0]
    FMOV D1, D30
    FCMP D1, D0
    CSET X14, LT
    CMP X14, XZR
    B.EQ L_iterate_fern_Else_2
    B L_iterate_fern_Then_1
L_iterate_fern_Else_2:
    ADRP X15, L_float2@PAGE
    ADD X15, X15, L_float2@PAGEOFF
    LDR D0, [X15, #0]
    FMOV D1, D30
    FCMP D1, D0
    CSET X15, LT
    CMP X15, XZR
    B.EQ L_iterate_fern_Else_5
    B L_iterate_fern_Then_4
L_iterate_fern_Else_5:
    ADRP X9, L_float3@PAGE
    ADD X9, X9, L_float3@PAGEOFF
    LDR D0, [X9, #0]
    FMOV D1, D30
    FCMP D1, D0
    CSET X9, LT
    CMP X9, XZR
    B.EQ L_iterate_fern_Else_8
    B L_iterate_fern_Then_7
L_iterate_fern_Else_8:
    ADRP X10, L_float4@PAGE
    ADD X10, X10, L_float4@PAGEOFF
    LDR D0, [X10, #0]
    FNEG D1, D0
    LDR D0, [X28, #0]
    FMUL D1, D1, D0
    ADRP X11, L_float5@PAGE
    ADD X11, X11, L_float5@PAGEOFF
    LDR D0, [X11, #0]
    LDR D2, [X28, #8]
    FMUL D0, D0, D2
    FADD D1, D1, D0
    FMOV D13, D1
    FMOV D13, D1
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
    FMOV D31, D0
    FMOV D31, D0
    B L_iterate_fern_Join_9
L_iterate_fern_Exit_10:
    B L_1
L_iterate_fern_Join_3:
    STR D13, [X28, #0]
    FMOV D15, D13
    STR D31, [X28, #8]
    FMOV D14, D31
    LDR X14, [X28, #16] ; point_count
    ADD X14, X14, #1
    STR X14, [X28, #16] ; point_count
    MOV X27, X14
    B L_iterate_fern_Exit_10
L_iterate_fern_Join_6:
    B L_iterate_fern_Join_3
L_iterate_fern_Join_9:
    B L_iterate_fern_Join_6
L_iterate_fern_Then_1:
    ADRP X15, L_float0@PAGE
    ADD X15, X15, L_float0@PAGEOFF
    LDR D0, [X15, #0]
    FMOV D13, D0
    FMOV D13, D0
    ADRP X9, L_float9@PAGE
    ADD X9, X9, L_float9@PAGEOFF
    LDR D0, [X9, #0]
    LDR D1, [X28, #8]
    FMUL D0, D0, D1
    FMOV D31, D0
    FMOV D31, D0
    B L_iterate_fern_Join_3
L_iterate_fern_Then_4:
    ADRP X10, L_float10@PAGE
    ADD X10, X10, L_float10@PAGEOFF
    LDR D0, [X10, #0]
    LDR D1, [X28, #0]
    FMUL D0, D0, D1
    ADRP X11, L_float11@PAGE
    ADD X11, X11, L_float11@PAGEOFF
    LDR D1, [X11, #0]
    LDR D2, [X28, #8]
    FMUL D1, D1, D2
    FADD D0, D0, D1
    FMOV D13, D0
    FMOV D13, D0
    ADRP X12, L_float11@PAGE
    ADD X12, X12, L_float11@PAGEOFF
    LDR D0, [X12, #0]
    FNEG D1, D0
    LDR D0, [X28, #0]
    FMUL D1, D1, D0
    ADRP X13, L_float10@PAGE
    ADD X13, X13, L_float10@PAGEOFF
    LDR D0, [X13, #0]
    LDR D2, [X28, #8]
    FMUL D0, D0, D2
    FADD D1, D1, D0
    ADRP X14, L_float12@PAGE
    ADD X14, X14, L_float12@PAGEOFF
    LDR D0, [X14, #0]
    FADD D1, D1, D0
    FMOV D31, D1
    FMOV D31, D1
    B L_iterate_fern_Join_6
L_iterate_fern_Then_7:
    ADRP X15, L_float13@PAGE
    ADD X15, X15, L_float13@PAGEOFF
    LDR D0, [X15, #0]
    LDR D1, [X28, #0]
    FMUL D0, D0, D1
    ADRP X9, L_float6@PAGE
    ADD X9, X9, L_float6@PAGEOFF
    LDR D1, [X9, #0]
    LDR D2, [X28, #8]
    FMUL D1, D1, D2
    FSUB D0, D0, D1
    FMOV D13, D0
    FMOV D13, D0
    ADRP X10, L_float14@PAGE
    ADD X10, X10, L_float14@PAGEOFF
    LDR D0, [X10, #0]
    LDR D1, [X28, #0]
    FMUL D0, D0, D1
    ADRP X11, L_float15@PAGE
    ADD X11, X11, L_float15@PAGEOFF
    LDR D1, [X11, #0]
    LDR D2, [X28, #8]
    FMUL D1, D1, D2
    FADD D0, D0, D1
    ADRP X12, L_float12@PAGE
    ADD X12, X12, L_float12@PAGEOFF
    LDR D1, [X12, #0]
    FADD D0, D0, D1
    FMOV D31, D0
    FMOV D31, D0
    B L_iterate_fern_Join_9
L_1:
    LDR D13, [X29, #40] ; Restored Reg: D13 @ FP+40
    LDR D14, [X29, #48] ; Restored Reg: D14 @ FP+48
    LDR D15, [X29, #56] ; Restored Reg: D15 @ FP+56
    LDR X19, [X29, #64] ; Restored Reg: X19 @ FP+64
    LDR X20, [X29, #72] ; Restored Reg: X20 @ FP+72
    LDR X21, [X29, #80] ; Restored Reg: X21 @ FP+80
    LDR X22, [X29, #88] ; Restored Reg: X22 @ FP+88
    LDR X23, [X29, #96] ; Restored Reg: X23 @ FP+96
    LDR X27, [X29, #104] ; Restored Reg: X27 @ FP+104
    LDR X28, [X29, #112] ; Restored Reg: X28 @ FP+112
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_fern_to_screen_x:
    STP X29, X30, [SP, #-64]!
    MOV X29, SP
    STR X19, [X29, #40] ; Saved Reg: X19 @ FP+40
    STR X20, [X29, #48] ; Saved Reg: X20 @ FP+48
    STR X28, [X29, #56] ; Saved Reg: X28 @ FP+56
    MOV X15, X0
L_fern_to_screen_x_Entry_0:
    ADRP X13, L_float16@PAGE
    ADD X13, X13, L_float16@PAGEOFF
    LDR D0, [X13, #0]
    SCVTF D1, X15
    FMUL D1, D1, D0
    FMOV D31, D1
    FMOV D31, D1
    FCVTZS X13, D31
    ADD X13, X13, #400
    MOV X15, X13
    B L_fern_to_screen_x_ResultisCleanup_1
L_fern_to_screen_x_Exit_2:
    B L_2
L_fern_to_screen_x_ResultisCleanup_1:
    MOV X0, X15
    B L_2
    B L_fern_to_screen_x_Exit_2
L_2:
    LDR X19, [X29, #40] ; Restored Reg: X19 @ FP+40
    LDR X20, [X29, #48] ; Restored Reg: X20 @ FP+48
    LDR X28, [X29, #56] ; Restored Reg: X28 @ FP+56
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_fern_to_screen_y:
    STP X29, X30, [SP, #-64]!
    MOV X29, SP
    STR X19, [X29, #40] ; Saved Reg: X19 @ FP+40
    STR X20, [X29, #48] ; Saved Reg: X20 @ FP+48
    STR X28, [X29, #56] ; Saved Reg: X28 @ FP+56
    MOV X15, X0
L_fern_to_screen_y_Entry_0:
    ADRP X14, L_float16@PAGE
    ADD X14, X14, L_float16@PAGEOFF
    LDR D0, [X14, #0]
    SCVTF D1, X15
    FMUL D1, D1, D0
    FMOV D31, D1
    FMOV D31, D1
    MOVZ X14, #500
    FCVTZS X15, D31
    SUB X14, X14, X15
    MOV X15, X14
    B L_fern_to_screen_y_ResultisCleanup_1
L_fern_to_screen_y_Exit_2:
    B L_3
L_fern_to_screen_y_ResultisCleanup_1:
    MOV X0, X15
    B L_3
    B L_fern_to_screen_y_Exit_2
L_3:
    LDR X19, [X29, #40] ; Restored Reg: X19 @ FP+40
    LDR X20, [X29, #48] ; Restored Reg: X20 @ FP+48
    LDR X28, [X29, #56] ; Restored Reg: X28 @ FP+56
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_Global:
    STP X29, X30, [SP, #-32]!
    MOV X29, SP
    STR X19, [X29, #16] ; Saved Reg: X19 @ FP+16
    STR X28, [X29, #24] ; Saved Reg: X28 @ FP+24
L_Global_Entry_0:
    ADRP X15, L_float0@PAGE
    ADD X15, X15, L_float0@PAGEOFF
    LDR D0, [X15, #0]
    STR D0, [X28, #0]
    FMOV D31, D0
    ADRP X9, L_float0@PAGE
    ADD X9, X9, L_float0@PAGEOFF
    LDR D0, [X9, #0]
    STR D0, [X28, #8]
    FMOV D31, D0
    MOVZ X9, #0
    STR X9, [X28, #16] ; point_count
    MOV X15, X9
    B L_Global_Exit_1
L_Global_Exit_1:
    B L_4
L_4:
    LDR X19, [X29, #16] ; Restored Reg: X19 @ FP+16
    LDR X28, [X29, #24] ; Restored Reg: X28 @ FP+24
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_START:
    STP X29, X30, [SP, #-160]!
    MOV X29, SP
    STR D14, [X29, #64] ; Saved Reg: D14 @ FP+64
    STR D15, [X29, #72] ; Saved Reg: D15 @ FP+72
    STR X19, [X29, #80] ; Saved Reg: X19 @ FP+80
    STR X20, [X29, #88] ; Saved Reg: X20 @ FP+88
    STR X21, [X29, #96] ; Saved Reg: X21 @ FP+96
    STR X22, [X29, #104] ; Saved Reg: X22 @ FP+104
    STR X23, [X29, #112] ; Saved Reg: X23 @ FP+112
    STR X24, [X29, #120] ; Saved Reg: X24 @ FP+120
    STR X25, [X29, #128] ; Saved Reg: X25 @ FP+128
    STR X26, [X29, #136] ; Saved Reg: X26 @ FP+136
    STR X27, [X29, #144] ; Saved Reg: X27 @ FP+144
    STR X28, [X29, #152] ; Saved Reg: X28 @ FP+152
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
    MOVZ X9, #8, LSL #16
    ADD X19, X28, X9
L_START_Entry_0:
    BL L_Global
    LDR X9, [X19, #672]
    BLR X9
    MOV X15, X0
    MOV X9, X15
    CMP X9, #0
    CSET X10, EQ
    CMP X10, XZR
    B.EQ L_START_Join_2
    B L_START_Then_1
L_START_Exit_15:
    B L_5
L_START_ForBody_8:
    BL L_iterate_fern
    LDR X9, [X28, #16] ; point_count
    CMP X9, #10
    CSET X10, GT
    CMP X10, XZR
    B.EQ L_START_Join_12
    B L_START_Then_11
L_START_ForExit_10:
    MOV X20, X25
    MOV X0, X20
    BL _SDL2_PRESENT
    BL _NEWLINE
    MOVZ X9, #2000
    MOV X20, X9
    MOV X0, X20
    BL _SDL2_DELAY
    B L_START_Join_6
L_START_ForHeader_7:
    MOV X9, X22
    CMP X9, #100
    B.GT L_START_ForExit_10
    B L_START_ForBody_8
L_START_ForIncrement_9:
    MOV X9, X22
    ADD X9, X9, #1
    MOV X22, X9
    B L_START_ForHeader_7
L_START_Join_12:
    B L_START_ForIncrement_9
L_START_Join_14:
    B L_START_Join_12
L_START_Join_2:
    ADRP X9, L_str0@PAGE
    ADD X9, X9, L_str0@PAGEOFF
    ADD X9, X9, #8
    MOV X20, X9
    MOV X0, X20
    BL _WRITES
    BL _NEWLINE
    B L_START_Exit_15
L_START_Join_4:
    B L_START_Join_2
L_START_Join_6:
    B L_START_Join_4
L_START_Then_1:
    ADRP X9, L_str1@PAGE
    ADD X9, X9, L_str1@PAGEOFF
    ADD X9, X9, #8
    MOV X20, X9
    MOV X0, X20
    BL _WRITES
    BL _NEWLINE
    ADRP X9, L_str2@PAGE
    ADD X9, X9, L_str2@PAGEOFF
    ADD X9, X9, #8
    MOV X20, X9
    MOV X0, X20
    LDR X10, [X19, #696]
    BLR X10
    MOV X22, X0
    MOV X10, X22
    CMP X10, #0
    CSET X11, GT
    CMP X11, XZR
    B.EQ L_START_Join_4
    B L_START_Then_3
L_START_Then_11:
    LDR D0, [X28, #0]
    FMOV D8, D0
    FCVTZS X0, D8
    BL L_fern_to_screen_x
    MOV X24, X0
    LDR D1, [X28, #8]
    FMOV D8, D1
    FCVTZS X0, D8
    BL L_fern_to_screen_y
    MOV X23, X0
    MOV X10, X24
    CMP X10, #0
    CSET X11, GE
    CMP X11, #0
    B.EQ L_6
    MOV X10, X24
    CMP X10, #800
    CSET X12, LT
    B L_7
L_6:
    MOVZ X12, #0
L_7:
    CMP X12, #0
    B.EQ L_8
    MOV X10, X23
    CMP X10, #0
    CSET X13, GE
    B L_9
L_8:
    MOVZ X13, #0
L_9:
    CMP X13, #0
    B.EQ L_10
    MOV X10, X23
    CMP X10, #600
    CSET X14, LT
    B L_11
L_10:
    MOVZ X14, #0
L_11:
    CMP X14, XZR
    B.EQ L_START_Join_14
    B L_START_Then_13
L_START_Then_13:
    MOV X20, X25
    MOV X21, X24
    MOV X22, X23
    MOV X0, X20
    MOV X1, X21
    MOV X2, X22
    BL _SDL2_DRAW_POINT
    B L_START_Join_14
L_START_Then_3:
    ADRP X9, L_str3@PAGE
    ADD X9, X9, L_str3@PAGEOFF
    ADD X9, X9, #8
    MOV X20, X9
    MOV X0, X20
    BL _WRITES
    BL _NEWLINE
    MOV X20, X22
    MOV X0, X20
    LDR X9, [X19, #736]
    BLR X9
    MOV X25, X0
    MOV X9, X25
    CMP X9, #0
    CSET X10, GT
    CMP X10, XZR
    B.EQ L_START_Join_6
    B L_START_Then_5
L_START_Then_5:
    ADRP X9, L_str4@PAGE
    ADD X9, X9, L_str4@PAGEOFF
    ADD X9, X9, #8
    MOV X20, X9
    MOV X0, X20
    BL _WRITES
    BL _NEWLINE
    BL L_init_fern
    MOV X20, X25
    MOVZ X9, #10
    MOV X21, X9
    MOVZ X9, #15
    MOV X22, X9
    MOVZ X9, #25
    MOV X23, X9
    MOVZ X9, #255
    MOV X24, X9
    MOV X0, X20
    MOV X1, X21
    MOV X2, X22
    MOV X3, X23
    MOV X4, X24
    BL _SDL2_SET_DRAW_COLOR
    MOV X20, X25
    MOV X0, X20
    BL _SDL2_CLEAR
    MOV X20, X25
    MOVZ X9, #0
    MOV X21, X9
    MOVZ X9, #255
    MOV X22, X9
    MOVZ X9, #100
    MOV X23, X9
    MOVZ X9, #255
    MOV X24, X9
    MOV X0, X20
    MOV X1, X21
    MOV X2, X22
    MOV X3, X23
    MOV X4, X24
    BL _SDL2_SET_DRAW_COLOR
    ADRP X9, L_str5@PAGE
    ADD X9, X9, L_str5@PAGEOFF
    ADD X9, X9, #8
    MOV X20, X9
    MOV X0, X20
    BL _WRITES
    BL _NEWLINE
    MOVZ X9, #1
    MOV X22, X9
    B L_START_ForHeader_7
L_5:
    LDR D14, [X29, #64] ; Restored Reg: D14 @ FP+64
    LDR D15, [X29, #72] ; Restored Reg: D15 @ FP+72
    LDR X19, [X29, #80] ; Restored Reg: X19 @ FP+80
    LDR X20, [X29, #88] ; Restored Reg: X20 @ FP+88
    LDR X21, [X29, #96] ; Restored Reg: X21 @ FP+96
    LDR X22, [X29, #104] ; Restored Reg: X22 @ FP+104
    LDR X23, [X29, #112] ; Restored Reg: X23 @ FP+112
    LDR X24, [X29, #120] ; Restored Reg: X24 @ FP+120
    LDR X25, [X29, #128] ; Restored Reg: X25 @ FP+128
    LDR X26, [X29, #136] ; Restored Reg: X26 @ FP+136
    LDR X27, [X29, #144] ; Restored Reg: X27 @ FP+144
    LDR X28, [X29, #152] ; Restored Reg: X28 @ FP+152
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET

.section __DATA,__const
.p2align 3
L_str0:
    .quad 0x10
    ; (upper half)
    .long 0x50
    .long 0x72
    .long 0x6f
    .long 0x67
    .long 0x72
    .long 0x61
    .long 0x6d
    .long 0x20
    .long 0x63
    .long 0x6f
    .long 0x6d
    .long 0x70
    .long 0x6c
    .long 0x65
    .long 0x74
    .long 0x65
    .long 0x0
    .long 0x0
L_str1:
    .quad 0x10
    ; (upper half)
    .long 0x53
    .long 0x44
    .long 0x4c
    .long 0x32
    .long 0x20
    .long 0x69
    .long 0x6e
    .long 0x69
    .long 0x74
    .long 0x69
    .long 0x61
    .long 0x6c
    .long 0x69
    .long 0x7a
    .long 0x65
    .long 0x64
    .long 0x0
    .long 0x0
L_str2:
    .quad 0x9
    ; (upper half)
    .long 0x42
    .long 0x43
    .long 0x50
    .long 0x4c
    .long 0x20
    .long 0x46
    .long 0x65
    .long 0x72
    .long 0x6e
    .long 0x0
    .long 0x0
L_str3:
    .quad 0xe
    ; (upper half)
    .long 0x57
    .long 0x69
    .long 0x6e
    .long 0x64
    .long 0x6f
    .long 0x77
    .long 0x20
    .long 0x63
    .long 0x72
    .long 0x65
    .long 0x61
    .long 0x74
    .long 0x65
    .long 0x64
    .long 0x0
    .long 0x0
L_str4:
    .quad 0x10
    ; (upper half)
    .long 0x52
    .long 0x65
    .long 0x6e
    .long 0x64
    .long 0x65
    .long 0x72
    .long 0x65
    .long 0x72
    .long 0x20
    .long 0x63
    .long 0x72
    .long 0x65
    .long 0x61
    .long 0x74
    .long 0x65
    .long 0x64
    .long 0x0
    .long 0x0
L_str5:
    .quad 0x19
    ; (upper half)
    .long 0x47
    .long 0x65
    .long 0x6e
    .long 0x65
    .long 0x72
    .long 0x61
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x66
    .long 0x65
    .long 0x72
    .long 0x6e
    .long 0x20
    .long 0x70
    .long 0x6f
    .long 0x69
    .long 0x6e
    .long 0x74
    .long 0x73
    .long 0x2e
    .long 0x2e
    .long 0x2e
    .long 0x0
    .long 0x0
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
    .quad 0x3fc3333333333333
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
    .quad 0x3ff999999999999a
    ; (upper half)
L_float13:
    .quad 0x3fc999999999999a
    ; (upper half)
L_float14:
    .quad 0x3fcd70a3d70a3d71
    ; (upper half)
L_float15:
    .quad 0x3fcc28f5c28f5c29
    ; (upper half)
L_float16:
    .quad 0x404e000000000000
    ; (upper half)

.section __DATA,__data
.p2align 3
    .long 0x0
    .long 0x0
    .long 0x0
L__data_segment_base:
    .quad 0x0
    ; (upper half)
    .quad 0x0
    ; (upper half)
    .quad 0x0
    ; (upper half)
