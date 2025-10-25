.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _START
.globl _WRITEN
.globl _GETVEC
.globl _WRITEF
.globl _BCPL_LIST_GET_HEAD_AS_FOCT
.globl _BCPL_LIST_GET_HEAD_AS_QUAD
.globl _SDL2_SET_WINDOW_TITLE
.globl _NEWLINE
.globl _SDL2_QUIT
.globl _RDCH
.globl _BCPL_LIST_GET_NTH
.globl _BCPL_ALLOC_WORDS
.globl _BCPL_LIST_GET_HEAD_AS_FPAIR
.globl _SPLIT
.globl _BCPL_CHECK_AND_DISPLAY_ERRORS
.globl _FIND
.globl _SDL2_GET_EVENT_BUTTON
.globl _LPND
.globl _FGETVEC
.globl _SLURP
.globl _SDL2_CREATE_WINDOW
.globl _TIMER_START
.globl _UNPACKSTRING
.globl _SDL2_DELAY
.globl _FREEVEC
.globl _LIST_CREATE
.globl _LIST_APPEND_INT
.globl _FILTER
.globl _BCPL_LIST_GET_HEAD_AS_FQUAD
.globl _BCPL_CLEAR_ERRORS
.globl _FLOG
.globl _BCPL_BOUNDS_ERROR
.globl _REVERSE
.globl _GET_FREE_LIST_HEAD_ADDR
.globl _LIST_TAIL
.globl _JOIN
.globl _MALLOC
.globl _BCPL_FREE_CELLS
.globl _SDL2_GET_EVENT_KEY
.globl _BCPL_CONCAT_LISTS
.globl _BCPL_LIST_APPEND_OCT
.globl _BCPL_LIST_APPEND_FOCT
.globl _SDL2_DRAW_POINT
.globl _HEAPMANAGER_EXIT_SCOPE
.globl _SDL2_GET_ERROR
.globl _WRITEF7
.globl _SDL2_DESTROY_WINDOW
.globl _FCOS
.globl _APND
.globl _STRCOPY
.globl _LIST_HEAD_INT
.globl _LIST_APPEND_STRING
.globl _FILE_OPEN_READ
.globl _LIST_APPEND_FLOAT
.globl _BCPL_LIST_CREATE_EMPTY
.globl _BCPL_LIST_APPEND_PAIR
.globl _BCPL_LIST_APPEND_QUAD
.globl _BCPL_LIST_APPEND_FQUAD
.globl _DEEPCOPYLITERALLIST
.globl _FILE_WRITES
.globl _LIST_HEAD_FLOAT
.globl _HEAPMANAGER_ENTER_SCOPE
.globl _FEXP
.globl _FPND
.globl _WRITEF4
.globl _RETURNNODETOFREELIST
.globl _RAND
.globl _SDL2_FILL_RECT
.globl _FWRITE
.globl _FILE_TELL
.globl _PACKSTRING
.globl _BCPL_LIST_APPEND_LIST
.globl _FILE_OPEN_APPEND
.globl _SDL2_CREATE_WINDOW_EX
.globl _FSIN
.globl _WRITEF6
.globl _HEAPMANAGER_SETSAMMENABLED
.globl _BCPL_LIST_APPEND_FPAIR
.globl _WRITES
.globl _BCPL_ALLOC_CHARS
.globl _WRITEF5
.globl _FABS
.globl _SDL2_GET_CURRENT_VIDEO_DRIVER
.globl _SDL2_INIT_SUBSYSTEMS
.globl _SPIT
.globl _FTAN
.globl _FRND
.globl _WRITEF3
.globl _LIST_FREE
.globl _FILE_READS
.globl _BCPL_GET_ATOM_TYPE
.globl _BCPL_FREE_LIST_SAFE
.globl _DEEPCOPYLIST
.globl _BCPL_FREE_LIST
.globl _FILE_CLOSE
.globl _PIC_RUNTIME_HELPER
.globl _SDL2_GET_EVENT_MOUSE
.globl _STRCMP
.globl _SETTYPE
.globl _FILE_SEEK
.globl _SDL2_GET_VERSION
.globl _SDL2_GET_VIDEO_DRIVERS
.globl _FINISH
.globl _HEAPMANAGER_ISSAMMENABLED
.globl _SDL2_DRAW_RECT
.globl _TIMER_CLEAR
.globl _DEBUG_PRINT_VTABLE_STORE
.globl _FILE_OPEN_WRITE
.globl _SDL2_GET_TICKS
.globl _FIX
.globl _WRITEF1
.globl _FILE_WRITE
.globl _TIMER_GET_CALL_COUNT
.globl _COPYLIST
.globl _BCPL_LIST_GET_HEAD_AS_OCT
.globl _SDL2_GET_DISPLAY_MODES
.globl _BCPL_GET_LAST_ERROR
.globl _SDL2_PRESENT
.globl _SPND
.globl _SDL2_CREATE_RENDERER_EX
.globl _BCPL_LIST_GET_REST
.globl _SDL2_DESTROY_RENDERER
.globl _SDL2_CLEAR
.globl _SDL2_CLEAR_ERROR
.globl _SDL2_CREATE_RENDERER
.globl _SDL2_SET_WINDOW_SIZE
.globl _FILE_EOF
.globl _TIMER_GET_TOTAL_NS
.globl _DEBUG_PRINT_FINAL_OBJECT
.globl _RUNTIME_METHOD_LOOKUP
.globl _WRITEC
.globl _FILE_READ
.globl _TIMER_DISPLAY
.globl _SDL2_SET_DRAW_COLOR
.globl _WRITEF2
.globl _STRLEN
.globl _RND
.globl _TIMER_END
.globl _OBJECT_HEAP_FREE
.globl _SDL2_INIT
.globl _OBJECT_HEAP_ALLOC
.globl _SDL2_TEST_BASIC
.globl _CONCAT
.globl _BCPL_LIST_GET_HEAD_AS_PAIR
.globl _SDL2_DRAW_LINE
.globl _HEAPMANAGER_WAITFORSAMM
.globl _SDL2_POLL_EVENT
.p2align 2
_start:
_START:
    B L_START
    
; --- Veneer Section ---
    ; --- End Veneer Section ---

L_START:
    SUB SP, SP, #512
    STP X29, X30, [SP, #0]
    MOV X29, SP
    STP x19, x20, [x29, #216]
    STP x21, x22, [x29, #232]
    STP x23, x24, [x29, #248]
    STP x25, x26, [x29, #264]
    STP x27, x28, [x29, #280]
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
L_START_Entry_0:
    ADRP X9, L_str0_plus_8@PAGE
    ADD X9, X9, L_str0_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str1_plus_8@PAGE
    ADD X9, X9, L_str1_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    MOVZ X9, #8
    MOV X0, X9
    BL _FGETVEC
    MOV X20, X0
    STR X20, [X29, #24] ; V1_ADD
    MOVZ X9, #8
    MOV X0, X9
    BL _FGETVEC
    MOV X20, X0
    STR X20, [X29, #64] ; V2_ADD
    MOVZ X9, #0
    STR X9, [X29, #16] ; I_for_var_0
    B L_START_ForHeader_1
L_START_Exit_62:
    B L_0
L_START_ForBody_11:
    LDR X9, [X29, #104] ; V3_ADD
    LDR X10, [X29, #16] ; I_for_var_0
    SUB X12, X9, #8
    LDR X11, [X12, #0] ; Load vector length for bounds check
    CMP X10, X11
    B.HS L__bounds_error_handler_START
    MOV X11, X10
    LSL X11, X11, #3
    ADD X12, X9, X11
    LDR X9, [X12, #0]
    MOV X23, X9
    ADRP X9, L_str2_plus_8@PAGE
    ADD X9, X9, L_str2_plus_8@PAGEOFF
    MOV X0, X9
    LDR X9, [X29, #16] ; I_for_var_0
    MOV X1, X9
    MOV X2, X23
    BL _WRITEF2
    B L_START_ForIncrement_12
L_START_ForBody_15:
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
    MOV X22, X9
    MOVZ X9, #0
    ADRP X13, L_float2@PAGE
    ADD X13, X13, L_float2@PAGEOFF
    LDR D0, [X13, #0]
    FCVT S1, D0 ;encoder.
    FMOV W13, S1
    BFXIL X9, X13, #0, #32
    ADRP X14, L_float3@PAGE
    ADD X14, X14, L_float3@PAGEOFF
    LDR D0, [X14, #0]
    FCVT S1, D0 ;encoder.
    FMOV W14, S1
    BFI X9, X14, #32, #32
    MOV X21, X9
    LDR X9, [X29, #56] ; V1_SUB
    LDR X14, [X29, #16] ; I_for_var_0
    LSL X15, X14, #3
    ADD X9, X9, X15
    STR X22, [X9, #0]
    LDR X9, [X29, #96] ; V2_SUB
    LDR X10, [X29, #16] ; I_for_var_0
    LSL X11, X10, #3
    ADD X12, X9, X11
    STR X21, [X12, #0]
    B L_START_ForIncrement_16
L_START_ForBody_2:
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
    MOV X25, X9
    MOVZ X9, #0
    ADRP X13, L_float6@PAGE
    ADD X13, X13, L_float6@PAGEOFF
    LDR D0, [X13, #0]
    FCVT S1, D0 ;encoder.
    FMOV W13, S1
    BFXIL X9, X13, #0, #32
    ADRP X14, L_float7@PAGE
    ADD X14, X14, L_float7@PAGEOFF
    LDR D0, [X14, #0]
    FCVT S1, D0 ;encoder.
    FMOV W14, S1
    BFI X9, X14, #32, #32
    STR X9, [X29, #152] ; _opt_temp_1
    LDR X9, [X29, #24] ; V1_ADD
    LDR X14, [X29, #16] ; I_for_var_0
    LSL X15, X14, #3
    ADD X9, X9, X15
    STR X25, [X9, #0]
    LDR X9, [X29, #152] ; _opt_temp_1
    LDR X10, [X29, #64] ; V2_ADD
    LDR X11, [X29, #16] ; I_for_var_0
    LSL X12, X11, #3
    ADD X13, X10, X12
    STR X9, [X13, #0]
    B L_START_ForIncrement_3
L_START_ForBody_24:
    LDR X9, [X29, #136] ; V3_SUB
    LDR X10, [X29, #16] ; I_for_var_0
    SUB X12, X9, #8
    LDR X11, [X12, #0] ; Load vector length for bounds check
    CMP X10, X11
    B.HS L__bounds_error_handler_START
    MOV X11, X10
    LSL X11, X11, #3
    ADD X12, X9, X11
    LDR X9, [X12, #0]
    MOV X23, X9
    ADRP X9, L_str3_plus_8@PAGE
    ADD X9, X9, L_str3_plus_8@PAGEOFF
    MOV X0, X9
    LDR X9, [X29, #16] ; I_for_var_0
    MOV X1, X9
    MOV X2, X23
    BL _WRITEF2
    B L_START_ForIncrement_25
L_START_ForBody_28:
    MOVZ X9, #0
    ADRP X11, L_float8@PAGE
    ADD X11, X11, L_float8@PAGEOFF
    LDR D0, [X11, #0]
    FCVT S1, D0 ;encoder.
    FMOV W11, S1
    BFXIL X9, X11, #0, #32
    ADRP X12, L_float9@PAGE
    ADD X12, X12, L_float9@PAGEOFF
    LDR D0, [X12, #0]
    FCVT S1, D0 ;encoder.
    FMOV W12, S1
    BFI X9, X12, #32, #32
    MOV X20, X9
    MOVZ X9, #0
    ADRP X13, L_float10@PAGE
    ADD X13, X13, L_float10@PAGEOFF
    LDR D0, [X13, #0]
    FCVT S1, D0 ;encoder.
    FMOV W13, S1
    BFXIL X9, X13, #0, #32
    ADRP X14, L_float11@PAGE
    ADD X14, X14, L_float11@PAGEOFF
    LDR D0, [X14, #0]
    FCVT S1, D0 ;encoder.
    FMOV W14, S1
    BFI X9, X14, #32, #32
    MOV X19, X9
    LDR X9, [X29, #40] ; V1_MUL
    LDR X14, [X29, #16] ; I_for_var_0
    LSL X15, X14, #3
    ADD X9, X9, X15
    STR X20, [X9, #0]
    LDR X9, [X29, #80] ; V2_MUL
    LDR X10, [X29, #16] ; I_for_var_0
    LSL X11, X10, #3
    ADD X12, X9, X11
    STR X19, [X12, #0]
    B L_START_ForIncrement_29
L_START_ForBody_37:
    LDR X9, [X29, #120] ; V3_MUL
    LDR X10, [X29, #16] ; I_for_var_0
    SUB X12, X9, #8
    LDR X11, [X12, #0] ; Load vector length for bounds check
    CMP X10, X11
    B.HS L__bounds_error_handler_START
    MOV X11, X10
    LSL X11, X11, #3
    ADD X12, X9, X11
    LDR X9, [X12, #0]
    MOV X23, X9
    ADRP X9, L_str4_plus_8@PAGE
    ADD X9, X9, L_str4_plus_8@PAGEOFF
    MOV X0, X9
    LDR X9, [X29, #16] ; I_for_var_0
    MOV X1, X9
    MOV X2, X23
    BL _WRITEF2
    B L_START_ForIncrement_38
L_START_ForBody_41:
    MOVZ X9, #0
    ADRP X11, L_float12@PAGE
    ADD X11, X11, L_float12@PAGEOFF
    LDR D0, [X11, #0]
    FCVT S1, D0 ;encoder.
    FMOV W11, S1
    BFXIL X9, X11, #0, #32
    ADRP X12, L_float13@PAGE
    ADD X12, X12, L_float13@PAGEOFF
    LDR D0, [X12, #0]
    FCVT S1, D0 ;encoder.
    FMOV W12, S1
    BFI X9, X12, #32, #32
    MOV X27, X9
    MOVZ X9, #0
    ADRP X13, L_float14@PAGE
    ADD X13, X13, L_float14@PAGEOFF
    LDR D0, [X13, #0]
    FCVT S1, D0 ;encoder.
    FMOV W13, S1
    BFXIL X9, X13, #0, #32
    ADRP X14, L_float11@PAGE
    ADD X14, X14, L_float11@PAGEOFF
    LDR D0, [X14, #0]
    FCVT S1, D0 ;encoder.
    FMOV W14, S1
    BFI X9, X14, #32, #32
    MOV X24, X9
    LDR X9, [X29, #48] ; V1_ODD
    LDR X14, [X29, #16] ; I_for_var_0
    LSL X15, X14, #3
    ADD X9, X9, X15
    STR X27, [X9, #0]
    LDR X9, [X29, #88] ; V2_ODD
    LDR X10, [X29, #16] ; I_for_var_0
    LSL X11, X10, #3
    ADD X12, X9, X11
    STR X24, [X12, #0]
    B L_START_ForIncrement_42
L_START_ForBody_50:
    LDR X9, [X29, #128] ; V3_ODD
    LDR X10, [X29, #16] ; I_for_var_0
    SUB X12, X9, #8
    LDR X11, [X12, #0] ; Load vector length for bounds check
    CMP X10, X11
    B.HS L__bounds_error_handler_START
    MOV X11, X10
    LSL X11, X11, #3
    ADD X12, X9, X11
    LDR X9, [X12, #0]
    MOV X23, X9
    ADRP X9, L_str5_plus_8@PAGE
    ADD X9, X9, L_str5_plus_8@PAGEOFF
    MOV X0, X9
    LDR X9, [X29, #16] ; I_for_var_0
    MOV X1, X9
    MOV X2, X23
    BL _WRITEF2
    B L_START_ForIncrement_51
L_START_ForBody_59:
    LDR X9, [X29, #112] ; V3_MIX
    LDR X10, [X29, #16] ; I_for_var_0
    SUB X12, X9, #8
    LDR X11, [X12, #0] ; Load vector length for bounds check
    CMP X10, X11
    B.HS L__bounds_error_handler_START
    MOV X11, X10
    LSL X11, X11, #3
    ADD X12, X9, X11
    LDR X9, [X12, #0]
    MOV X23, X9
    ADRP X9, L_str6_plus_8@PAGE
    ADD X9, X9, L_str6_plus_8@PAGEOFF
    MOV X0, X9
    LDR X9, [X29, #16] ; I_for_var_0
    MOV X1, X9
    MOV X2, X23
    BL _WRITEF2
    B L_START_ForIncrement_60
L_START_ForExit_13:
    ADRP X9, L_str7_plus_8@PAGE
    ADD X9, X9, L_str7_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    MOVZ X9, #6
    MOV X0, X9
    BL _FGETVEC
    MOV X20, X0
    STR X20, [X29, #56] ; V1_SUB
    MOVZ X9, #6
    MOV X0, X9
    BL _FGETVEC
    MOV X20, X0
    STR X20, [X29, #96] ; V2_SUB
    MOVZ X9, #0
    STR X9, [X29, #16] ; I_for_var_0
    B L_START_ForHeader_14
L_START_ForExit_17:
    SUB SP, SP, #16
    LDR X9, [X29, #56] ; V1_SUB
    STR X9, [SP, #0]
    LDR X0, [SP, #0]
    ADD SP, SP, #16
    LDR X9, [X29, #56] ; V1_SUB
    SUB X11, X9, #8
    LDR X10, [X11, #0] ; Load vector/string length
    STR X10, [X29, #264] ; __vec_len_3
    LDR X9, [X29, #264] ; __vec_len_3
    BL _FGETVEC
    MOV X20, X0
    STR X20, [X29, #136] ; V3_SUB
    MOVZ X9, #0
    STR X9, [X29, #224] ; __vec_i_2
    B L_START_WhileHeader_18
L_START_ForExit_26:
    ADRP X9, L_str8_plus_8@PAGE
    ADD X9, X9, L_str8_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    MOVZ X9, #4
    MOV X0, X9
    BL _FGETVEC
    MOV X20, X0
    STR X20, [X29, #40] ; V1_MUL
    MOVZ X9, #4
    MOV X0, X9
    BL _FGETVEC
    MOV X20, X0
    STR X20, [X29, #80] ; V2_MUL
    MOVZ X9, #0
    STR X9, [X29, #16] ; I_for_var_0
    B L_START_ForHeader_27
L_START_ForExit_30:
    SUB SP, SP, #16
    LDR X9, [X29, #40] ; V1_MUL
    STR X9, [SP, #0]
    LDR X0, [SP, #0]
    ADD SP, SP, #16
    LDR X9, [X29, #40] ; V1_MUL
    SUB X11, X9, #8
    LDR X10, [X11, #0] ; Load vector/string length
    STR X10, [X29, #272] ; __vec_len_5
    LDR X9, [X29, #272] ; __vec_len_5
    BL _FGETVEC
    MOV X20, X0
    STR X20, [X29, #120] ; V3_MUL
    MOVZ X9, #0
    STR X9, [X29, #232] ; __vec_i_4
    B L_START_WhileHeader_31
L_START_ForExit_39:
    ADRP X9, L_str9_plus_8@PAGE
    ADD X9, X9, L_str9_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    MOVZ X9, #7
    MOV X0, X9
    BL _FGETVEC
    MOV X20, X0
    STR X20, [X29, #48] ; V1_ODD
    MOVZ X9, #7
    MOV X0, X9
    BL _FGETVEC
    MOV X20, X0
    STR X20, [X29, #88] ; V2_ODD
    MOVZ X9, #0
    STR X9, [X29, #16] ; I_for_var_0
    B L_START_ForHeader_40
L_START_ForExit_4:
    SUB SP, SP, #16
    LDR X9, [X29, #24] ; V1_ADD
    STR X9, [SP, #0]
    LDR X0, [SP, #0]
    ADD SP, SP, #16
    LDR X9, [X29, #24] ; V1_ADD
    SUB X11, X9, #8
    LDR X10, [X11, #0] ; Load vector/string length
    STR X10, [X29, #256] ; __vec_len_1
    LDR X9, [X29, #256] ; __vec_len_1
    BL _FGETVEC
    MOV X20, X0
    STR X20, [X29, #104] ; V3_ADD
    MOVZ X9, #0
    STR X9, [X29, #216] ; __vec_i_0
    B L_START_WhileHeader_5
L_START_ForExit_43:
    SUB SP, SP, #16
    LDR X9, [X29, #48] ; V1_ODD
    STR X9, [SP, #0]
    LDR X0, [SP, #0]
    ADD SP, SP, #16
    LDR X9, [X29, #48] ; V1_ODD
    SUB X11, X9, #8
    LDR X10, [X11, #0] ; Load vector/string length
    STR X10, [X29, #280] ; __vec_len_7
    LDR X9, [X29, #280] ; __vec_len_7
    BL _FGETVEC
    MOV X24, X0
    STR X24, [X29, #128] ; V3_ODD
    MOVZ X9, #0
    STR X9, [X29, #240] ; __vec_i_6
    B L_START_WhileHeader_44
L_START_ForExit_52:
    ADRP X9, L_str10_plus_8@PAGE
    ADD X9, X9, L_str10_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    MOVZ X9, #3
    MOV X0, X9
    BL _FGETVEC
    MOV X24, X0
    STR X24, [X29, #32] ; V1_MIX
    MOVZ X9, #3
    MOV X0, X9
    BL _FGETVEC
    MOV X24, X0
    STR X24, [X29, #72] ; V2_MIX
    MOVZ X9, #0
    ADRP X11, L_float15@PAGE
    ADD X11, X11, L_float15@PAGEOFF
    LDR D0, [X11, #0]
    FCVT S1, D0 ;encoder.
    FMOV W11, S1
    BFXIL X9, X11, #0, #32
    ADRP X12, L_float16@PAGE
    ADD X12, X12, L_float16@PAGEOFF
    LDR D0, [X12, #0]
    FCVT S1, D0 ;encoder.
    FMOV W12, S1
    BFI X9, X12, #32, #32
    LDR X12, [X29, #32] ; V1_MIX
    MOVZ X13, #0
    LSL X14, X13, #3
    ADD X15, X12, X14
    STR X9, [X15, #0]
    MOVZ X9, #0
    ADRP X13, L_float17@PAGE
    ADD X13, X13, L_float17@PAGEOFF
    LDR D0, [X13, #0]
    FCVT S1, D0 ;encoder.
    FMOV W13, S1
    BFXIL X9, X13, #0, #32
    ADRP X14, L_float18@PAGE
    ADD X14, X14, L_float18@PAGEOFF
    LDR D0, [X14, #0]
    FCVT S1, D0 ;encoder.
    FMOV W14, S1
    BFI X9, X14, #32, #32
    LDR X14, [X29, #32] ; V1_MIX
    MOVZ X15, #1
    LSL X9, X15, #3
    ADD X10, X14, X9
    STR X9, [X10, #0]
    MOVZ X9, #0
    ADRP X11, L_float19@PAGE
    ADD X11, X11, L_float19@PAGEOFF
    LDR D0, [X11, #0]
    FCVT S1, D0 ;encoder.
    FMOV W11, S1
    BFXIL X9, X11, #0, #32
    ADRP X12, L_float20@PAGE
    ADD X12, X12, L_float20@PAGEOFF
    LDR D0, [X12, #0]
    FCVT S1, D0 ;encoder.
    FMOV W12, S1
    BFI X9, X12, #32, #32
    LDR X12, [X29, #32] ; V1_MIX
    MOVZ X13, #2
    LSL X14, X13, #3
    ADD X15, X12, X14
    STR X9, [X15, #0]
    MOVZ X9, #0
    ADRP X13, L_float21@PAGE
    ADD X13, X13, L_float21@PAGEOFF
    LDR D0, [X13, #0]
    FCVT S1, D0 ;encoder.
    FMOV W13, S1
    BFXIL X9, X13, #0, #32
    ADRP X14, L_float22@PAGE
    ADD X14, X14, L_float22@PAGEOFF
    LDR D0, [X14, #0]
    FCVT S1, D0 ;encoder.
    FMOV W14, S1
    BFI X9, X14, #32, #32
    LDR X14, [X29, #72] ; V2_MIX
    MOVZ X15, #0
    LSL X9, X15, #3
    ADD X10, X14, X9
    STR X9, [X10, #0]
    MOVZ X9, #0
    ADRP X11, L_float23@PAGE
    ADD X11, X11, L_float23@PAGEOFF
    LDR D0, [X11, #0]
    FCVT S1, D0 ;encoder.
    FMOV W11, S1
    BFXIL X9, X11, #0, #32
    ADRP X12, L_float24@PAGE
    ADD X12, X12, L_float24@PAGEOFF
    LDR D0, [X12, #0]
    FCVT S1, D0 ;encoder.
    FMOV W12, S1
    BFI X9, X12, #32, #32
    LDR X12, [X29, #72] ; V2_MIX
    MOVZ X13, #1
    LSL X14, X13, #3
    ADD X15, X12, X14
    STR X9, [X15, #0]
    MOVZ X9, #0
    ADRP X13, L_float25@PAGE
    ADD X13, X13, L_float25@PAGEOFF
    LDR D0, [X13, #0]
    FCVT S1, D0 ;encoder.
    FMOV W13, S1
    BFXIL X9, X13, #0, #32
    ADRP X14, L_float26@PAGE
    ADD X14, X14, L_float26@PAGEOFF
    LDR D0, [X14, #0]
    FCVT S1, D0 ;encoder.
    FMOV W14, S1
    BFI X9, X14, #32, #32
    LDR X14, [X29, #72] ; V2_MIX
    MOVZ X15, #2
    LSL X9, X15, #3
    ADD X10, X14, X9
    STR X9, [X10, #0]
    SUB SP, SP, #16
    LDR X9, [X29, #32] ; V1_MIX
    STR X9, [SP, #0]
    LDR X0, [SP, #0]
    ADD SP, SP, #16
    LDR X9, [X29, #32] ; V1_MIX
    SUB X11, X9, #8
    LDR X10, [X11, #0] ; Load vector/string length
    STR X10, [X29, #288] ; __vec_len_9
    LDR X9, [X29, #288] ; __vec_len_9
    BL _FGETVEC
    MOV X24, X0
    STR X24, [X29, #112] ; V3_MIX
    MOVZ X9, #0
    STR X9, [X29, #248] ; __vec_i_8
    B L_START_WhileHeader_53
L_START_ForExit_61:
    ADRP X9, L_str11_plus_8@PAGE
    ADD X9, X9, L_str11_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    B L_START_Exit_62
L_START_ForHeader_1:
    LDR X9, [X29, #16] ; I_for_var_0
    MOVZ X10, #7
    CMP X9, X10
    B.GT L_START_ForExit_4
    B L_START_ForBody_2
L_START_ForHeader_10:
    LDR X9, [X29, #16] ; I_for_var_0
    MOVZ X11, #7
    CMP X9, X11
    B.GT L_START_ForExit_13
    B L_START_ForBody_11
L_START_ForHeader_14:
    LDR X9, [X29, #16] ; I_for_var_0
    MOVZ X12, #5
    CMP X9, X12
    B.GT L_START_ForExit_17
    B L_START_ForBody_15
L_START_ForHeader_23:
    LDR X9, [X29, #16] ; I_for_var_0
    MOVZ X13, #5
    CMP X9, X13
    B.GT L_START_ForExit_26
    B L_START_ForBody_24
L_START_ForHeader_27:
    LDR X9, [X29, #16] ; I_for_var_0
    MOVZ X14, #3
    CMP X9, X14
    B.GT L_START_ForExit_30
    B L_START_ForBody_28
L_START_ForHeader_36:
    LDR X9, [X29, #16] ; I_for_var_0
    MOVZ X15, #3
    CMP X9, X15
    B.GT L_START_ForExit_39
    B L_START_ForBody_37
L_START_ForHeader_40:
    LDR X9, [X29, #16] ; I_for_var_0
    MOVZ X9, #6
    CMP X9, X9
    B.GT L_START_ForExit_43
    B L_START_ForBody_41
L_START_ForHeader_49:
    LDR X9, [X29, #16] ; I_for_var_0
    MOVZ X10, #6
    CMP X9, X10
    B.GT L_START_ForExit_52
    B L_START_ForBody_50
L_START_ForHeader_58:
    LDR X9, [X29, #16] ; I_for_var_0
    MOVZ X11, #2
    CMP X9, X11
    B.GT L_START_ForExit_61
    B L_START_ForBody_59
L_START_ForIncrement_12:
    LDR X9, [X29, #16] ; I_for_var_0
    ADD X9, X9, #1
    B L_START_ForHeader_10
L_START_ForIncrement_16:
    LDR X12, [X29, #16] ; I_for_var_0
    ADD X12, X12, #1
    B L_START_ForHeader_14
L_START_ForIncrement_25:
    LDR X13, [X29, #16] ; I_for_var_0
    ADD X13, X13, #1
    B L_START_ForHeader_23
L_START_ForIncrement_29:
    LDR X14, [X29, #16] ; I_for_var_0
    ADD X14, X14, #1
    B L_START_ForHeader_27
L_START_ForIncrement_3:
    LDR X15, [X29, #16] ; I_for_var_0
    ADD X15, X15, #1
    B L_START_ForHeader_1
L_START_ForIncrement_38:
    LDR X9, [X29, #16] ; I_for_var_0
    ADD X9, X9, #1
    B L_START_ForHeader_36
L_START_ForIncrement_42:
    LDR X10, [X29, #16] ; I_for_var_0
    ADD X10, X10, #1
    B L_START_ForHeader_40
L_START_ForIncrement_51:
    LDR X11, [X29, #16] ; I_for_var_0
    ADD X11, X11, #1
    B L_START_ForHeader_49
L_START_ForIncrement_60:
    LDR X12, [X29, #16] ; I_for_var_0
    ADD X12, X12, #1
    B L_START_ForHeader_58
L_START_Join_22:
    ADRP X13, L_str12_plus_8@PAGE
    ADD X13, X13, L_str12_plus_8@PAGEOFF
    MOV X0, X13
    BL _WRITEF
    ADRP X9, L_str13_plus_8@PAGE
    ADD X9, X9, L_str13_plus_8@PAGEOFF
    MOV X0, X9
    LDR X9, [X29, #136] ; V3_SUB
    SUB X11, X9, #8
    LDR X10, [X11, #0] ; Load vector/table/string length
    MOV X1, X10
    BL _WRITEF1
    MOVZ X9, #0
    STR X9, [X29, #16] ; I_for_var_0
    B L_START_ForHeader_23
L_START_Join_35:
    ADRP X9, L_str14_plus_8@PAGE
    ADD X9, X9, L_str14_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str13_plus_8@PAGE
    ADD X9, X9, L_str13_plus_8@PAGEOFF
    MOV X0, X9
    LDR X9, [X29, #120] ; V3_MUL
    SUB X11, X9, #8
    LDR X10, [X11, #0] ; Load vector/table/string length
    MOV X1, X10
    BL _WRITEF1
    MOVZ X9, #0
    STR X9, [X29, #16] ; I_for_var_0
    B L_START_ForHeader_36
L_START_Join_48:
    ADRP X9, L_str15_plus_8@PAGE
    ADD X9, X9, L_str15_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str13_plus_8@PAGE
    ADD X9, X9, L_str13_plus_8@PAGEOFF
    MOV X0, X9
    LDR X9, [X29, #128] ; V3_ODD
    SUB X11, X9, #8
    LDR X10, [X11, #0] ; Load vector/table/string length
    MOV X1, X10
    BL _WRITEF1
    MOVZ X9, #0
    STR X9, [X29, #16] ; I_for_var_0
    B L_START_ForHeader_49
L_START_Join_57:
    ADRP X9, L_str16_plus_8@PAGE
    ADD X9, X9, L_str16_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    MOVZ X9, #0
    STR X9, [X29, #16] ; I_for_var_0
    B L_START_ForHeader_58
L_START_Join_9:
    ADRP X9, L_str17_plus_8@PAGE
    ADD X9, X9, L_str17_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str13_plus_8@PAGE
    ADD X9, X9, L_str13_plus_8@PAGEOFF
    MOV X0, X9
    LDR X9, [X29, #104] ; V3_ADD
    SUB X11, X9, #8
    LDR X10, [X11, #0] ; Load vector/table/string length
    MOV X1, X10
    BL _WRITEF1
    MOVZ X9, #0
    STR X9, [X29, #16] ; I_for_var_0
    B L_START_ForHeader_10
L_START_Then_21:
    LDR X9, [X29, #56] ; V1_SUB
    LDR X10, [X29, #264] ; __vec_len_3
    SUB X10, X10, #1
    SUB X12, X9, #8
    LDR X11, [X12, #0] ; Load vector length for bounds check
    CMP X10, X11
    B.HS L__bounds_error_handler_START
    MOV X11, X10
    LSL X11, X11, #3
    ADD X12, X9, X11
    LDR X9, [X12, #0]
    LDR X10, [X29, #96] ; V2_SUB
    LDR X11, [X29, #264] ; __vec_len_3
    SUB X11, X11, #1
    SUB X13, X10, #8
    LDR X12, [X13, #0] ; Load vector length for bounds check
    CMP X11, X12
    B.HS L__bounds_error_handler_START
    MOV X12, X11
    LSL X12, X12, #3
    ADD X13, X10, X12
    LDR X10, [X13, #0]
    fmov D0, X9
    fmov D1, X10
    fsub v0.2s, v0.2s, v1.2s    ; fsub 2s
    fmov X20, D0
    LDR X10, [X29, #136] ; V3_SUB
    LDR X11, [X29, #264] ; __vec_len_3
    SUB X11, X11, #1
    LSL X12, X11, #3
    ADD X13, X10, X12
    STR X20, [X13, #0]
    B L_START_Join_22
L_START_Then_34:
    LDR X10, [X29, #40] ; V1_MUL
    LDR X11, [X29, #272] ; __vec_len_5
    SUB X11, X11, #1
    SUB X13, X10, #8
    LDR X12, [X13, #0] ; Load vector length for bounds check
    CMP X11, X12
    B.HS L__bounds_error_handler_START
    MOV X12, X11
    LSL X12, X12, #3
    ADD X13, X10, X12
    LDR X10, [X13, #0]
    LDR X11, [X29, #80] ; V2_MUL
    LDR X12, [X29, #272] ; __vec_len_5
    SUB X12, X12, #1
    SUB X14, X11, #8
    LDR X13, [X14, #0] ; Load vector length for bounds check
    CMP X12, X13
    B.HS L__bounds_error_handler_START
    MOV X13, X12
    LSL X13, X13, #3
    ADD X14, X11, X13
    LDR X11, [X14, #0]
    fmov D0, X10
    fmov D1, X11
    fmul v0.2s, v0.2s, v1.2s    ; dedicated 2s encoder
    fmov X24, D0
    LDR X11, [X29, #120] ; V3_MUL
    LDR X12, [X29, #272] ; __vec_len_5
    SUB X12, X12, #1
    LSL X13, X12, #3
    ADD X14, X11, X13
    STR X24, [X14, #0]
    B L_START_Join_35
L_START_Then_47:
    LDR X11, [X29, #48] ; V1_ODD
    LDR X12, [X29, #280] ; __vec_len_7
    SUB X12, X12, #1
    SUB X14, X11, #8
    LDR X13, [X14, #0] ; Load vector length for bounds check
    CMP X12, X13
    B.HS L__bounds_error_handler_START
    MOV X13, X12
    LSL X13, X13, #3
    ADD X14, X11, X13
    LDR X11, [X14, #0]
    LDR X12, [X29, #88] ; V2_ODD
    LDR X13, [X29, #280] ; __vec_len_7
    SUB X13, X13, #1
    SUB X15, X12, #8
    LDR X14, [X15, #0] ; Load vector length for bounds check
    CMP X13, X14
    B.HS L__bounds_error_handler_START
    MOV X14, X13
    LSL X14, X14, #3
    ADD X15, X12, X14
    LDR X12, [X15, #0]
    fmov D0, X11
    fmov D1, X12
    fadd v0.2s, v0.2s, v1.2s    ; FPAIR 2s encoder
    fmov X20, D0
    LDR X12, [X29, #128] ; V3_ODD
    LDR X13, [X29, #280] ; __vec_len_7
    SUB X13, X13, #1
    LSL X14, X13, #3
    ADD X15, X12, X14
    STR X20, [X15, #0]
    B L_START_Join_48
L_START_Then_56:
    LDR X12, [X29, #32] ; V1_MIX
    LDR X13, [X29, #288] ; __vec_len_9
    SUB X13, X13, #1
    SUB X15, X12, #8
    LDR X14, [X15, #0] ; Load vector length for bounds check
    CMP X13, X14
    B.HS L__bounds_error_handler_START
    MOV X14, X13
    LSL X14, X14, #3
    ADD X15, X12, X14
    LDR X12, [X15, #0]
    LDR X13, [X29, #72] ; V2_MIX
    LDR X14, [X29, #288] ; __vec_len_9
    SUB X14, X14, #1
    SUB X9, X13, #8
    LDR X15, [X9, #0] ; Load vector length for bounds check
    CMP X14, X15
    B.HS L__bounds_error_handler_START
    MOV X9, X14
    LSL X9, X9, #3
    ADD X10, X13, X9
    LDR X9, [X10, #0]
    fmov D0, X12
    fmov D1, X9
    fadd v0.2s, v0.2s, v1.2s    ; FPAIR 2s encoder
    fmov X24, D0
    LDR X9, [X29, #112] ; V3_MIX
    LDR X10, [X29, #288] ; __vec_len_9
    SUB X10, X10, #1
    LSL X11, X10, #3
    ADD X12, X9, X11
    STR X24, [X12, #0]
    B L_START_Join_57
L_START_Then_8:
    LDR X9, [X29, #24] ; V1_ADD
    LDR X10, [X29, #256] ; __vec_len_1
    SUB X10, X10, #1
    SUB X12, X9, #8
    LDR X11, [X12, #0] ; Load vector length for bounds check
    CMP X10, X11
    B.HS L__bounds_error_handler_START
    MOV X11, X10
    LSL X11, X11, #3
    ADD X12, X9, X11
    LDR X9, [X12, #0]
    LDR X10, [X29, #64] ; V2_ADD
    LDR X11, [X29, #256] ; __vec_len_1
    SUB X11, X11, #1
    SUB X13, X10, #8
    LDR X12, [X13, #0] ; Load vector length for bounds check
    CMP X11, X12
    B.HS L__bounds_error_handler_START
    MOV X12, X11
    LSL X12, X12, #3
    ADD X13, X10, X12
    LDR X10, [X13, #0]
    fmov D0, X9
    fmov D1, X10
    fadd v0.2s, v0.2s, v1.2s    ; FPAIR 2s encoder
    fmov X20, D0
    LDR X10, [X29, #104] ; V3_ADD
    LDR X11, [X29, #256] ; __vec_len_1
    SUB X11, X11, #1
    LSL X12, X11, #3
    ADD X13, X10, X12
    STR X20, [X13, #0]
    B L_START_Join_9
L_START_WhileBody_19:
    LDR X10, [X29, #136] ; V3_SUB
    LDR X11, [X29, #56] ; V1_SUB
    LDR X12, [X29, #96] ; V2_SUB
    LDR X13, [X29, #224] ; __vec_i_2
    LSL X24, X13, #3
    ADD X11, X11, X24
    LDR Q0, [X11, #0]
    SUB X11, X11, X24
    ADD X12, X12, X24
    LDR Q1, [X12, #0]
    SUB X12, X12, X24
    FSUB V2.4S, V0.4S, V1.4S
    ADD X10, X10, X24
    STR Q2, [X10, #0]
    SUB X10, X10, X24
    LDR X14, [X29, #224] ; __vec_i_2
    ADD X14, X14, #2
    B L_START_WhileHeader_18
L_START_WhileBody_32:
    LDR X15, [X29, #120] ; V3_MUL
    LDR X9, [X29, #40] ; V1_MUL
    LDR X10, [X29, #80] ; V2_MUL
    LDR X11, [X29, #232] ; __vec_i_4
    LSL X20, X11, #3
    ADD X9, X9, X20
    LDR Q3, [X9, #0]
    SUB X9, X9, X20
    ADD X10, X10, X20
    LDR Q4, [X10, #0]
    SUB X10, X10, X20
    FMUL V5.4S, V3.4S, V4.4S
    ADD X15, X15, X20
    STR Q5, [X15, #0]
    SUB X15, X15, X20
    LDR X12, [X29, #232] ; __vec_i_4
    ADD X12, X12, #2
    B L_START_WhileHeader_31
L_START_WhileBody_45:
    LDR X13, [X29, #128] ; V3_ODD
    LDR X14, [X29, #48] ; V1_ODD
    LDR X15, [X29, #88] ; V2_ODD
    LDR X9, [X29, #240] ; __vec_i_6
    LSL X24, X9, #3
    ADD X14, X14, X24
    LDR Q6, [X14, #0]
    SUB X14, X14, X24
    ADD X15, X15, X24
    LDR Q7, [X15, #0]
    SUB X15, X15, X24
    FADD V16.4S, V6.4S, V7.4S
    ADD X13, X13, X24
    STR Q16, [X13, #0]
    SUB X13, X13, X24
    LDR X10, [X29, #240] ; __vec_i_6
    ADD X10, X10, #2
    B L_START_WhileHeader_44
L_START_WhileBody_54:
    LDR X11, [X29, #112] ; V3_MIX
    LDR X12, [X29, #32] ; V1_MIX
    LDR X13, [X29, #72] ; V2_MIX
    LDR X14, [X29, #248] ; __vec_i_8
    LSL X20, X14, #3
    ADD X12, X12, X20
    LDR Q17, [X12, #0]
    SUB X12, X12, X20
    ADD X13, X13, X20
    LDR Q18, [X13, #0]
    SUB X13, X13, X20
    FADD V19.4S, V17.4S, V18.4S
    ADD X11, X11, X20
    STR Q19, [X11, #0]
    SUB X11, X11, X20
    LDR X15, [X29, #248] ; __vec_i_8
    ADD X15, X15, #2
    B L_START_WhileHeader_53
L_START_WhileBody_6:
    LDR X9, [X29, #104] ; V3_ADD
    LDR X10, [X29, #24] ; V1_ADD
    LDR X11, [X29, #64] ; V2_ADD
    LDR X12, [X29, #216] ; __vec_i_0
    LSL X24, X12, #3
    ADD X10, X10, X24
    LDR Q20, [X10, #0]
    SUB X10, X10, X24
    ADD X11, X11, X24
    LDR Q21, [X11, #0]
    SUB X11, X11, X24
    FADD V22.4S, V20.4S, V21.4S
    ADD X9, X9, X24
    STR Q22, [X9, #0]
    SUB X9, X9, X24
    LDR X13, [X29, #216] ; __vec_i_0
    ADD X13, X13, #2
    B L_START_WhileHeader_5
L_START_WhileExit_20:
    LDR X14, [X29, #264] ; __vec_len_3
    MOVZ X15, #1
    AND X14, X14, X15
    CMP X14, #1
    CSET X15, EQ
    CMP X15, XZR
    B.EQ L_START_Join_22
    B L_START_Then_21
L_START_WhileExit_33:
    LDR X14, [X29, #272] ; __vec_len_5
    MOVZ X15, #1
    AND X14, X14, X15
    CMP X14, #1
    CSET X15, EQ
    CMP X15, XZR
    B.EQ L_START_Join_35
    B L_START_Then_34
L_START_WhileExit_46:
    LDR X14, [X29, #280] ; __vec_len_7
    MOVZ X15, #1
    AND X14, X14, X15
    CMP X14, #1
    CSET X15, EQ
    CMP X15, XZR
    B.EQ L_START_Join_48
    B L_START_Then_47
L_START_WhileExit_55:
    LDR X14, [X29, #288] ; __vec_len_9
    MOVZ X15, #1
    AND X14, X14, X15
    CMP X14, #1
    CSET X15, EQ
    CMP X15, XZR
    B.EQ L_START_Join_57
    B L_START_Then_56
L_START_WhileExit_7:
    LDR X14, [X29, #256] ; __vec_len_1
    MOVZ X15, #1
    AND X14, X14, X15
    CMP X14, #1
    CSET X15, EQ
    CMP X15, XZR
    B.EQ L_START_Join_9
    B L_START_Then_8
L_START_WhileHeader_18:
    LDR X14, [X29, #224] ; __vec_i_2
    LDR X15, [X29, #264] ; __vec_len_3
    SUB X15, X15, #2
    CMP X14, X15
    CSET X9, LE
    CMP X9, XZR
    B.EQ L_START_WhileExit_20
    B L_START_WhileBody_19
L_START_WhileHeader_31:
    LDR X9, [X29, #232] ; __vec_i_4
    LDR X10, [X29, #272] ; __vec_len_5
    SUB X10, X10, #2
    CMP X9, X10
    CSET X11, LE
    CMP X11, XZR
    B.EQ L_START_WhileExit_33
    B L_START_WhileBody_32
L_START_WhileHeader_44:
    LDR X9, [X29, #240] ; __vec_i_6
    LDR X10, [X29, #280] ; __vec_len_7
    SUB X10, X10, #2
    CMP X9, X10
    CSET X11, LE
    CMP X11, XZR
    B.EQ L_START_WhileExit_46
    B L_START_WhileBody_45
L_START_WhileHeader_5:
    LDR X9, [X29, #216] ; __vec_i_0
    LDR X10, [X29, #256] ; __vec_len_1
    SUB X10, X10, #2
    CMP X9, X10
    CSET X11, LE
    CMP X11, XZR
    B.EQ L_START_WhileExit_7
    B L_START_WhileBody_6
L_START_WhileHeader_53:
    LDR X9, [X29, #248] ; __vec_i_8
    LDR X10, [X29, #288] ; __vec_len_9
    SUB X10, X10, #2
    CMP X9, X10
    CSET X11, LE
    CMP X11, XZR
    B.EQ L_START_WhileExit_55
    B L_START_WhileBody_54
L__bounds_error_handler_START:
    MOVZ X0, #0
    MOVZ X1, #65535
    MOVZ X2, #0
    BL _BCPL_BOUNDS_ERROR
    BRK #0
L_0:
    LDP x19, x20, [x29, #216]
    LDP x21, x22, [x29, #232]
    LDP x23, x24, [x29, #248]
    LDP x25, x26, [x29, #264]
    LDP x27, x28, [x29, #280]
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [SP, #0]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L___veneer_:
    movz x16, #45292
    movk x16, #109, lsl #16
    movk x16, #1, lsl #32
    movk x16, #0, lsl #48
    blr x16

.section __DATA,__const
.p2align 3
L_str0:
    .quad 0x34
    ; (upper half)
.p2align 2
L_str0_plus_8:
    .long 0xa
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x20
    .long 0x43
    .long 0x4f
    .long 0x4d
    .long 0x50
    .long 0x52
    .long 0x45
    .long 0x48
    .long 0x45
    .long 0x4e
    .long 0x53
    .long 0x49
    .long 0x56
    .long 0x45
    .long 0x20
    .long 0x46
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x20
    .long 0x56
    .long 0x45
    .long 0x43
    .long 0x54
    .long 0x4f
    .long 0x52
    .long 0x20
    .long 0x41
    .long 0x52
    .long 0x49
    .long 0x54
    .long 0x48
    .long 0x4d
    .long 0x45
    .long 0x54
    .long 0x49
    .long 0x43
    .long 0x20
    .long 0x54
    .long 0x45
    .long 0x53
    .long 0x54
    .long 0x20
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str1:
    .quad 0x1e
    ; (upper half)
.p2align 2
L_str1_plus_8:
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x20
    .long 0x31
    .long 0x3a
    .long 0x20
    .long 0x46
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x20
    .long 0x56
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x6f
    .long 0x72
    .long 0x20
    .long 0x41
    .long 0x64
    .long 0x64
    .long 0x69
    .long 0x74
    .long 0x69
    .long 0x6f
    .long 0x6e
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str2:
    .quad 0x12
    ; (upper half)
.p2align 2
L_str2_plus_8:
    .long 0x20
    .long 0x20
    .long 0x56
    .long 0x33
    .long 0x5f
    .long 0x41
    .long 0x44
    .long 0x44
    .long 0x5b
    .long 0x25
    .long 0x4e
    .long 0x5d
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x51
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str3:
    .quad 0x12
    ; (upper half)
.p2align 2
L_str3_plus_8:
    .long 0x20
    .long 0x20
    .long 0x56
    .long 0x33
    .long 0x5f
    .long 0x53
    .long 0x55
    .long 0x42
    .long 0x5b
    .long 0x25
    .long 0x4e
    .long 0x5d
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x51
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str4:
    .quad 0x12
    ; (upper half)
.p2align 2
L_str4_plus_8:
    .long 0x20
    .long 0x20
    .long 0x56
    .long 0x33
    .long 0x5f
    .long 0x4d
    .long 0x55
    .long 0x4c
    .long 0x5b
    .long 0x25
    .long 0x4e
    .long 0x5d
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x51
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str5:
    .quad 0x12
    ; (upper half)
.p2align 2
L_str5_plus_8:
    .long 0x20
    .long 0x20
    .long 0x56
    .long 0x33
    .long 0x5f
    .long 0x4f
    .long 0x44
    .long 0x44
    .long 0x5b
    .long 0x25
    .long 0x4e
    .long 0x5d
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x51
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str6:
    .quad 0x12
    ; (upper half)
.p2align 2
L_str6_plus_8:
    .long 0x20
    .long 0x20
    .long 0x56
    .long 0x33
    .long 0x5f
    .long 0x4d
    .long 0x49
    .long 0x58
    .long 0x5b
    .long 0x25
    .long 0x4e
    .long 0x5d
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x51
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str7:
    .quad 0x22
    ; (upper half)
.p2align 2
L_str7_plus_8:
    .long 0xa
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x20
    .long 0x32
    .long 0x3a
    .long 0x20
    .long 0x46
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x20
    .long 0x56
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x6f
    .long 0x72
    .long 0x20
    .long 0x53
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
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str8:
    .quad 0x25
    ; (upper half)
.p2align 2
L_str8_plus_8:
    .long 0xa
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x20
    .long 0x33
    .long 0x3a
    .long 0x20
    .long 0x46
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x20
    .long 0x56
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x6f
    .long 0x72
    .long 0x20
    .long 0x4d
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
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str9:
    .quad 0x2e
    ; (upper half)
.p2align 2
L_str9_plus_8:
    .long 0xa
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x20
    .long 0x34
    .long 0x3a
    .long 0x20
    .long 0x4f
    .long 0x64
    .long 0x64
    .long 0x2d
    .long 0x6c
    .long 0x65
    .long 0x6e
    .long 0x67
    .long 0x74
    .long 0x68
    .long 0x20
    .long 0x46
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x20
    .long 0x56
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x6f
    .long 0x72
    .long 0x20
    .long 0x28
    .long 0x37
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
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str10:
    .quad 0x1e
    ; (upper half)
.p2align 2
L_str10_plus_8:
    .long 0xa
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x20
    .long 0x35
    .long 0x3a
    .long 0x20
    .long 0x4d
    .long 0x69
    .long 0x78
    .long 0x65
    .long 0x64
    .long 0x20
    .long 0x50
    .long 0x72
    .long 0x65
    .long 0x63
    .long 0x69
    .long 0x73
    .long 0x69
    .long 0x6f
    .long 0x6e
    .long 0x20
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str11:
    .quad 0x30
    ; (upper half)
.p2align 2
L_str11_plus_8:
    .long 0xa
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x20
    .long 0x46
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x20
    .long 0x56
    .long 0x45
    .long 0x43
    .long 0x54
    .long 0x4f
    .long 0x52
    .long 0x20
    .long 0x41
    .long 0x52
    .long 0x49
    .long 0x54
    .long 0x48
    .long 0x4d
    .long 0x45
    .long 0x54
    .long 0x49
    .long 0x43
    .long 0x20
    .long 0x54
    .long 0x45
    .long 0x53
    .long 0x54
    .long 0x53
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
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str12:
    .quad 0x34
    ; (upper half)
.p2align 2
L_str12_plus_8:
    .long 0x20
    .long 0x20
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
    .long 0x28
    .long 0x32
    .long 0x35
    .long 0x2e
    .long 0x37
    .long 0x35
    .long 0x2c
    .long 0x33
    .long 0x30
    .long 0x2e
    .long 0x35
    .long 0x29
    .long 0x20
    .long 0x2d
    .long 0x20
    .long 0x28
    .long 0x35
    .long 0x2e
    .long 0x32
    .long 0x35
    .long 0x2c
    .long 0x38
    .long 0x2e
    .long 0x30
    .long 0x29
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x28
    .long 0x32
    .long 0x30
    .long 0x2e
    .long 0x35
    .long 0x2c
    .long 0x32
    .long 0x32
    .long 0x2e
    .long 0x35
    .long 0x29
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str13:
    .quad 0x17
    ; (upper half)
.p2align 2
L_str13_plus_8:
    .long 0x20
    .long 0x20
    .long 0x4c
    .long 0x65
    .long 0x6e
    .long 0x67
    .long 0x74
    .long 0x68
    .long 0x20
    .long 0x6f
    .long 0x66
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
    .long 0x4e
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str14:
    .quad 0x2e
    ; (upper half)
.p2align 2
L_str14_plus_8:
    .long 0x20
    .long 0x20
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
    .long 0x28
    .long 0x32
    .long 0x2e
    .long 0x35
    .long 0x2c
    .long 0x34
    .long 0x2e
    .long 0x30
    .long 0x29
    .long 0x20
    .long 0x2a
    .long 0x20
    .long 0x28
    .long 0x33
    .long 0x2e
    .long 0x30
    .long 0x2c
    .long 0x31
    .long 0x2e
    .long 0x35
    .long 0x29
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x28
    .long 0x37
    .long 0x2e
    .long 0x35
    .long 0x2c
    .long 0x36
    .long 0x2e
    .long 0x30
    .long 0x29
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str15:
    .quad 0x2e
    ; (upper half)
.p2align 2
L_str15_plus_8:
    .long 0x20
    .long 0x20
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
    .long 0x28
    .long 0x31
    .long 0x2e
    .long 0x30
    .long 0x2c
    .long 0x32
    .long 0x2e
    .long 0x30
    .long 0x29
    .long 0x20
    .long 0x2b
    .long 0x20
    .long 0x28
    .long 0x30
    .long 0x2e
    .long 0x35
    .long 0x2c
    .long 0x31
    .long 0x2e
    .long 0x35
    .long 0x29
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x28
    .long 0x31
    .long 0x2e
    .long 0x35
    .long 0x2c
    .long 0x33
    .long 0x2e
    .long 0x35
    .long 0x29
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str16:
    .quad 0x24
    ; (upper half)
.p2align 2
L_str16_plus_8:
    .long 0x20
    .long 0x20
    .long 0x4d
    .long 0x69
    .long 0x78
    .long 0x65
    .long 0x64
    .long 0x20
    .long 0x70
    .long 0x72
    .long 0x65
    .long 0x63
    .long 0x69
    .long 0x73
    .long 0x69
    .long 0x6f
    .long 0x6e
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
    .long 0x72
    .long 0x65
    .long 0x73
    .long 0x75
    .long 0x6c
    .long 0x74
    .long 0x73
    .long 0x3a
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str17:
    .quad 0x36
    ; (upper half)
.p2align 2
L_str17_plus_8:
    .long 0x20
    .long 0x20
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
    .long 0x28
    .long 0x31
    .long 0x30
    .long 0x2e
    .long 0x35
    .long 0x2c
    .long 0x32
    .long 0x30
    .long 0x2e
    .long 0x32
    .long 0x35
    .long 0x29
    .long 0x20
    .long 0x2b
    .long 0x20
    .long 0x28
    .long 0x33
    .long 0x2e
    .long 0x37
    .long 0x35
    .long 0x2c
    .long 0x34
    .long 0x2e
    .long 0x35
    .long 0x29
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x28
    .long 0x31
    .long 0x34
    .long 0x2e
    .long 0x32
    .long 0x35
    .long 0x2c
    .long 0x32
    .long 0x34
    .long 0x2e
    .long 0x37
    .long 0x35
    .long 0x29
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_float0:
    .quad 0x4039c00000000000
    ; (upper half)
L_float1:
    .quad 0x403e800000000000
    ; (upper half)
L_float2:
    .quad 0x4015000000000000
    ; (upper half)
L_float3:
    .quad 0x4020000000000000
    ; (upper half)
L_float4:
    .quad 0x4025000000000000
    ; (upper half)
L_float5:
    .quad 0x4034400000000000
    ; (upper half)
L_float6:
    .quad 0x400e000000000000
    ; (upper half)
L_float7:
    .quad 0x4012000000000000
    ; (upper half)
L_float8:
    .quad 0x4004000000000000
    ; (upper half)
L_float9:
    .quad 0x4010000000000000
    ; (upper half)
L_float10:
    .quad 0x4008000000000000
    ; (upper half)
L_float11:
    .quad 0x3ff8000000000000
    ; (upper half)
L_float12:
    .quad 0x3ff0000000000000
    ; (upper half)
L_float13:
    .quad 0x4000000000000000
    ; (upper half)
L_float14:
    .quad 0x3fe0000000000000
    ; (upper half)
L_float15:
    .quad 0x3ff1c6a7ef9db22d
    ; (upper half)
L_float16:
    .quad 0x4001c6a7ef9db22d
    ; (upper half)
L_float17:
    .quad 0x400aa9fbe76c8b44
    ; (upper half)
L_float18:
    .quad 0x4011c6a7ef9db22d
    ; (upper half)
L_float19:
    .quad 0x40163851eb851eb8
    ; (upper half)
L_float20:
    .quad 0x401aa9fbe76c8b44
    ; (upper half)
L_float21:
    .quad 0x3f50624dd2f1a9fc
    ; (upper half)
L_float22:
    .quad 0x3f60624dd2f1a9fc
    ; (upper half)
L_float23:
    .quad 0x3f689374bc6a7efa
    ; (upper half)
L_float24:
    .quad 0x3f70624dd2f1a9fc
    ; (upper half)
L_float25:
    .quad 0x3f747ae147ae147b
    ; (upper half)
L_float26:
    .quad 0x3f789374bc6a7efa
    ; (upper half)

.section __DATA,__data
.p2align 3
    .long 0x0
    .long 0x0
.p2align 2
L__data_segment_base:
