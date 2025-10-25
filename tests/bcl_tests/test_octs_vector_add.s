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

L_START:
    STP X29, X30, [SP, #-176]!
    MOV X29, SP
    STP x19, x20, [x29, #72]
    STP x21, x22, [x29, #88]
    STP x23, x24, [x29, #104]
    STP x25, x26, [x29, #120]
    STP x27, x28, [x29, #136]
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
L_START_Entry_0:
    ADRP X9, L_str0_plus_8@PAGE
    ADD X9, X9, L_str0_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    MOVZ X9, #6
    MOV X19, X9
    MOV X0, X19
    BL _GETVEC
    MOV X19, X0
    MOV X21, X19
    MOV X0, X19
    BL _GETVEC
    MOV X19, X0
    MOV X20, X19
    ADRP X9, L_str1_plus_8@PAGE
    ADD X9, X9, L_str1_plus_8@PAGEOFF
    MOV X0, X9
    MOVZ X9, #6
    MOV X1, X9
    BL _WRITEF1
    MOVZ X9, #1
    ins v0.b[0], w9
    MOVZ X9, #2
    ins v0.b[1], w9
    MOVZ X9, #3
    ins v0.b[2], w9
    MOVZ X9, #4
    ins v0.b[3], w9
    MOVZ X9, #5
    ins v0.b[4], w9
    MOVZ X9, #6
    ins v0.b[5], w9
    MOVZ X9, #7
    ins v0.b[6], w9
    MOVZ X9, #8
    ins v0.b[7], w9
    fmov X19, D0
    MOVZ X9, #0
    LSL X10, X9, #3
    ADD X11, X21, X10
    STR X19, [X11, #0]
    MOVZ X9, #10
    ins v0.b[0], w9
    MOVZ X9, #11
    ins v0.b[1], w9
    MOVZ X9, #12
    ins v0.b[2], w9
    MOVZ X9, #13
    ins v0.b[3], w9
    MOVZ X9, #14
    ins v0.b[4], w9
    MOVZ X9, #15
    ins v0.b[5], w9
    MOVZ X9, #16
    ins v0.b[6], w9
    MOVZ X9, #17
    ins v0.b[7], w9
    fmov X19, D0
    MOVZ X9, #1
    LSL X10, X9, #3
    ADD X11, X21, X10
    STR X19, [X11, #0]
    MOVZ X9, #20
    ins v0.b[0], w9
    MOVZ X9, #21
    ins v0.b[1], w9
    MOVZ X9, #22
    ins v0.b[2], w9
    MOVZ X9, #23
    ins v0.b[3], w9
    MOVZ X9, #24
    ins v0.b[4], w9
    MOVZ X9, #25
    ins v0.b[5], w9
    MOVZ X9, #26
    ins v0.b[6], w9
    MOVZ X9, #27
    ins v0.b[7], w9
    fmov X19, D0
    MOVZ X9, #2
    LSL X10, X9, #3
    ADD X11, X21, X10
    STR X19, [X11, #0]
    MOVZ X9, #30
    ins v0.b[0], w9
    MOVZ X9, #31
    ins v0.b[1], w9
    MOVZ X9, #32
    ins v0.b[2], w9
    MOVZ X9, #33
    ins v0.b[3], w9
    MOVZ X9, #34
    ins v0.b[4], w9
    MOVZ X9, #35
    ins v0.b[5], w9
    MOVZ X9, #36
    ins v0.b[6], w9
    MOVZ X9, #37
    ins v0.b[7], w9
    fmov X19, D0
    MOVZ X9, #3
    LSL X10, X9, #3
    ADD X11, X21, X10
    STR X19, [X11, #0]
    MOVZ X9, #40
    ins v0.b[0], w9
    MOVZ X9, #41
    ins v0.b[1], w9
    MOVZ X9, #42
    ins v0.b[2], w9
    MOVZ X9, #43
    ins v0.b[3], w9
    MOVZ X9, #44
    ins v0.b[4], w9
    MOVZ X9, #45
    ins v0.b[5], w9
    MOVZ X9, #46
    ins v0.b[6], w9
    MOVZ X9, #47
    ins v0.b[7], w9
    fmov X19, D0
    MOVZ X9, #4
    LSL X10, X9, #3
    ADD X11, X21, X10
    STR X19, [X11, #0]
    MOVZ X9, #50
    ins v0.b[0], w9
    MOVZ X9, #51
    ins v0.b[1], w9
    MOVZ X9, #52
    ins v0.b[2], w9
    MOVZ X9, #53
    ins v0.b[3], w9
    MOVZ X9, #54
    ins v0.b[4], w9
    MOVZ X9, #55
    ins v0.b[5], w9
    MOVZ X9, #56
    ins v0.b[6], w9
    MOVZ X9, #57
    ins v0.b[7], w9
    fmov X19, D0
    MOVZ X9, #5
    LSL X10, X9, #3
    ADD X11, X21, X10
    STR X19, [X11, #0]
    MOVZ X9, #1
    ins v0.b[0], w9
    MOVZ X9, #1
    ins v0.b[1], w9
    MOVZ X9, #1
    ins v0.b[2], w9
    MOVZ X9, #1
    ins v0.b[3], w9
    MOVZ X9, #1
    ins v0.b[4], w9
    MOVZ X9, #1
    ins v0.b[5], w9
    MOVZ X9, #1
    ins v0.b[6], w9
    MOVZ X9, #1
    ins v0.b[7], w9
    fmov X19, D0
    MOVZ X9, #0
    LSL X10, X9, #3
    ADD X11, X20, X10
    STR X19, [X11, #0]
    MOVZ X9, #2
    ins v0.b[0], w9
    MOVZ X9, #2
    ins v0.b[1], w9
    MOVZ X9, #2
    ins v0.b[2], w9
    MOVZ X9, #2
    ins v0.b[3], w9
    MOVZ X9, #2
    ins v0.b[4], w9
    MOVZ X9, #2
    ins v0.b[5], w9
    MOVZ X9, #2
    ins v0.b[6], w9
    MOVZ X9, #2
    ins v0.b[7], w9
    fmov X19, D0
    MOVZ X9, #1
    LSL X10, X9, #3
    ADD X11, X20, X10
    STR X19, [X11, #0]
    MOVZ X9, #3
    ins v0.b[0], w9
    MOVZ X9, #3
    ins v0.b[1], w9
    MOVZ X9, #3
    ins v0.b[2], w9
    MOVZ X9, #3
    ins v0.b[3], w9
    MOVZ X9, #3
    ins v0.b[4], w9
    MOVZ X9, #3
    ins v0.b[5], w9
    MOVZ X9, #3
    ins v0.b[6], w9
    MOVZ X9, #3
    ins v0.b[7], w9
    fmov X19, D0
    MOVZ X9, #2
    LSL X10, X9, #3
    ADD X11, X20, X10
    STR X19, [X11, #0]
    MOVZ X9, #4
    ins v0.b[0], w9
    MOVZ X9, #4
    ins v0.b[1], w9
    MOVZ X9, #4
    ins v0.b[2], w9
    MOVZ X9, #4
    ins v0.b[3], w9
    MOVZ X9, #4
    ins v0.b[4], w9
    MOVZ X9, #4
    ins v0.b[5], w9
    MOVZ X9, #4
    ins v0.b[6], w9
    MOVZ X9, #4
    ins v0.b[7], w9
    fmov X19, D0
    MOVZ X9, #3
    LSL X10, X9, #3
    ADD X11, X20, X10
    STR X19, [X11, #0]
    MOVZ X9, #5
    ins v0.b[0], w9
    MOVZ X9, #5
    ins v0.b[1], w9
    MOVZ X9, #5
    ins v0.b[2], w9
    MOVZ X9, #5
    ins v0.b[3], w9
    MOVZ X9, #5
    ins v0.b[4], w9
    MOVZ X9, #5
    ins v0.b[5], w9
    MOVZ X9, #5
    ins v0.b[6], w9
    MOVZ X9, #5
    ins v0.b[7], w9
    fmov X19, D0
    MOVZ X9, #4
    LSL X10, X9, #3
    ADD X11, X20, X10
    STR X19, [X11, #0]
    MOVZ X9, #6
    ins v0.b[0], w9
    MOVZ X9, #6
    ins v0.b[1], w9
    MOVZ X9, #6
    ins v0.b[2], w9
    MOVZ X9, #6
    ins v0.b[3], w9
    MOVZ X9, #6
    ins v0.b[4], w9
    MOVZ X9, #6
    ins v0.b[5], w9
    MOVZ X9, #6
    ins v0.b[6], w9
    MOVZ X9, #6
    ins v0.b[7], w9
    fmov X19, D0
    MOVZ X9, #5
    LSL X10, X9, #3
    ADD X11, X20, X10
    STR X19, [X11, #0]
    ADRP X9, L_str2_plus_8@PAGE
    ADD X9, X9, L_str2_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str3_plus_8@PAGE
    ADD X9, X9, L_str3_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    MOV X0, X19
    BL _GETVEC
    MOV X19, X0
    MOV X22, X19
    SUB SP, SP, #16
    STR X21, [SP, #0]
    LDR X0, [SP, #0]
    ADD SP, SP, #16
    SUB X10, X21, #8
    LDR X9, [X10, #0] ; Load vector/string length
    STR X9, [X29, #72] ; __vec_len_1
    MOV X0, X9
    BL _GETVEC
    MOV X19, X0
    MOV X22, X19
    MOVZ X9, #0
    MOV X19, X9
    B L_START_ForHeader_1
L_START_Exit_9:
    B L_0
L_START_ForBody_2:
    SUB X10, X21, #8
    LDR X9, [X10, #0] ; Load vector length for bounds check
    CMP X19, X9
    B.HS L__bounds_error_handler_START
    MOV X9, X19
    LSL X9, X9, #3
    ADD X10, X21, X9
    LDR X9, [X10, #0]
    SUB X11, X20, #8
    LDR X10, [X11, #0] ; Load vector length for bounds check
    CMP X19, X10
    B.HS L__bounds_error_handler_START
    MOV X10, X19
    LSL X10, X10, #3
    ADD X11, X20, X10
    LDR X10, [X11, #0]
    ADD X9, X9, X10
    LSL X10, X19, #3
    ADD X11, X22, X10
    STR X9, [X11, #0]
    B L_START_ForIncrement_3
L_START_ForBody_6:
    SUB X10, X25, #8
    LDR X9, [X10, #0] ; Load vector length for bounds check
    CMP X27, X9
    B.HS L__bounds_error_handler_START
    MOV X9, X27
    LSL X9, X9, #3
    ADD X10, X25, X9
    LDR X9, [X10, #0]
    SUB X11, X24, #8
    LDR X10, [X11, #0] ; Load vector length for bounds check
    CMP X27, X10
    B.HS L__bounds_error_handler_START
    MOV X10, X27
    LSL X10, X10, #3
    ADD X11, X24, X10
    LDR X10, [X11, #0]
    MUL X9, X9, X10
    LSL X10, X27, #3
    ADD X11, X23, X10
    STR X9, [X11, #0]
    B L_START_ForIncrement_7
L_START_ForExit_4:
    ADRP X9, L_str4_plus_8@PAGE
    ADD X9, X9, L_str4_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str5_plus_8@PAGE
    ADD X9, X9, L_str5_plus_8@PAGEOFF
    MOV X0, X9
    SUB X10, X22, #8
    LDR X9, [X10, #0] ; Load vector/table/string length
    MOV X1, X9
    BL _WRITEF1
    ADRP X9, L_str6_plus_8@PAGE
    ADD X9, X9, L_str6_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    MOVZ X9, #4
    MOV X0, X9
    BL _FGETVEC
    MOV X26, X0
    MOV X25, X26
    MOVZ X9, #4
    MOV X0, X9
    BL _FGETVEC
    MOV X26, X0
    MOV X24, X26
    ADRP X10, L_float0@PAGE
    ADD X10, X10, L_float0@PAGEOFF
    LDR D1, [X10, #0]
    fmov Q0, D1
    ins v0.s[0], V0.s[0]
    ADRP X11, L_float1@PAGE
    ADD X11, X11, L_float1@PAGEOFF
    LDR D1, [X11, #0]
    fmov Q0, D1
    ins v0.s[1], V0.s[0]
    ADRP X12, L_float2@PAGE
    ADD X12, X12, L_float2@PAGEOFF
    LDR D1, [X12, #0]
    fmov Q0, D1
    ins v0.s[2], V0.s[0]
    ADRP X13, L_float3@PAGE
    ADD X13, X13, L_float3@PAGEOFF
    LDR D1, [X13, #0]
    fmov Q0, D1
    ins v0.s[3], V0.s[0]
    ADRP X14, L_float4@PAGE
    ADD X14, X14, L_float4@PAGEOFF
    LDR D1, [X14, #0]
    fmov Q0, D1
    ins v0.s[4], V0.s[0]
    ADRP X15, L_float5@PAGE
    ADD X15, X15, L_float5@PAGEOFF
    LDR D1, [X15, #0]
    fmov Q0, D1
    ins v0.s[5], V0.s[0]
    ADRP X9, L_float6@PAGE
    ADD X9, X9, L_float6@PAGEOFF
    LDR D1, [X9, #0]
    fmov Q0, D1
    ins v0.s[6], V0.s[0]
    ADRP X10, L_float7@PAGE
    ADD X10, X10, L_float7@PAGEOFF
    LDR D1, [X10, #0]
    fmov Q0, D1
    ins v0.s[7], V0.s[0]
    str D0, [X26]
    MOVZ X10, #0
    LSL X11, X10, #3
    ADD X12, X25, X11
    STR X26, [X12, #0]
    ADRP X11, L_float8@PAGE
    ADD X11, X11, L_float8@PAGEOFF
    LDR D1, [X11, #0]
    fmov Q0, D1
    ins v0.s[0], V0.s[0]
    ADRP X12, L_float9@PAGE
    ADD X12, X12, L_float9@PAGEOFF
    LDR D1, [X12, #0]
    fmov Q0, D1
    ins v0.s[1], V0.s[0]
    ADRP X13, L_float10@PAGE
    ADD X13, X13, L_float10@PAGEOFF
    LDR D1, [X13, #0]
    fmov Q0, D1
    ins v0.s[2], V0.s[0]
    ADRP X14, L_float11@PAGE
    ADD X14, X14, L_float11@PAGEOFF
    LDR D1, [X14, #0]
    fmov Q0, D1
    ins v0.s[3], V0.s[0]
    ADRP X15, L_float12@PAGE
    ADD X15, X15, L_float12@PAGEOFF
    LDR D1, [X15, #0]
    fmov Q0, D1
    ins v0.s[4], V0.s[0]
    ADRP X9, L_float13@PAGE
    ADD X9, X9, L_float13@PAGEOFF
    LDR D1, [X9, #0]
    fmov Q0, D1
    ins v0.s[5], V0.s[0]
    ADRP X10, L_float14@PAGE
    ADD X10, X10, L_float14@PAGEOFF
    LDR D1, [X10, #0]
    fmov Q0, D1
    ins v0.s[6], V0.s[0]
    ADRP X11, L_float15@PAGE
    ADD X11, X11, L_float15@PAGEOFF
    LDR D1, [X11, #0]
    fmov Q0, D1
    ins v0.s[7], V0.s[0]
    str D0, [X26]
    MOVZ X11, #1
    LSL X12, X11, #3
    ADD X13, X25, X12
    STR X26, [X13, #0]
    ADRP X12, L_float16@PAGE
    ADD X12, X12, L_float16@PAGEOFF
    LDR D1, [X12, #0]
    fmov Q0, D1
    ins v0.s[0], V0.s[0]
    ADRP X13, L_float17@PAGE
    ADD X13, X13, L_float17@PAGEOFF
    LDR D1, [X13, #0]
    fmov Q0, D1
    ins v0.s[1], V0.s[0]
    ADRP X14, L_float18@PAGE
    ADD X14, X14, L_float18@PAGEOFF
    LDR D1, [X14, #0]
    fmov Q0, D1
    ins v0.s[2], V0.s[0]
    ADRP X15, L_float19@PAGE
    ADD X15, X15, L_float19@PAGEOFF
    LDR D1, [X15, #0]
    fmov Q0, D1
    ins v0.s[3], V0.s[0]
    ADRP X9, L_float20@PAGE
    ADD X9, X9, L_float20@PAGEOFF
    LDR D1, [X9, #0]
    fmov Q0, D1
    ins v0.s[4], V0.s[0]
    ADRP X10, L_float21@PAGE
    ADD X10, X10, L_float21@PAGEOFF
    LDR D1, [X10, #0]
    fmov Q0, D1
    ins v0.s[5], V0.s[0]
    ADRP X11, L_float22@PAGE
    ADD X11, X11, L_float22@PAGEOFF
    LDR D1, [X11, #0]
    fmov Q0, D1
    ins v0.s[6], V0.s[0]
    ADRP X12, L_float23@PAGE
    ADD X12, X12, L_float23@PAGEOFF
    LDR D1, [X12, #0]
    fmov Q0, D1
    ins v0.s[7], V0.s[0]
    str D0, [X26]
    MOVZ X12, #2
    LSL X13, X12, #3
    ADD X14, X25, X13
    STR X26, [X14, #0]
    ADRP X13, L_float24@PAGE
    ADD X13, X13, L_float24@PAGEOFF
    LDR D1, [X13, #0]
    fmov Q0, D1
    ins v0.s[0], V0.s[0]
    ADRP X14, L_float25@PAGE
    ADD X14, X14, L_float25@PAGEOFF
    LDR D1, [X14, #0]
    fmov Q0, D1
    ins v0.s[1], V0.s[0]
    ADRP X15, L_float26@PAGE
    ADD X15, X15, L_float26@PAGEOFF
    LDR D1, [X15, #0]
    fmov Q0, D1
    ins v0.s[2], V0.s[0]
    ADRP X9, L_float27@PAGE
    ADD X9, X9, L_float27@PAGEOFF
    LDR D1, [X9, #0]
    fmov Q0, D1
    ins v0.s[3], V0.s[0]
    ADRP X10, L_float28@PAGE
    ADD X10, X10, L_float28@PAGEOFF
    LDR D1, [X10, #0]
    fmov Q0, D1
    ins v0.s[4], V0.s[0]
    ADRP X11, L_float29@PAGE
    ADD X11, X11, L_float29@PAGEOFF
    LDR D1, [X11, #0]
    fmov Q0, D1
    ins v0.s[5], V0.s[0]
    ADRP X12, L_float30@PAGE
    ADD X12, X12, L_float30@PAGEOFF
    LDR D1, [X12, #0]
    fmov Q0, D1
    ins v0.s[6], V0.s[0]
    ADRP X13, L_float31@PAGE
    ADD X13, X13, L_float31@PAGEOFF
    LDR D1, [X13, #0]
    fmov Q0, D1
    ins v0.s[7], V0.s[0]
    str D0, [X26]
    MOVZ X13, #3
    LSL X14, X13, #3
    ADD X15, X25, X14
    STR X26, [X15, #0]
    ADRP X14, L_float1@PAGE
    ADD X14, X14, L_float1@PAGEOFF
    LDR D1, [X14, #0]
    fmov Q0, D1
    ins v0.s[0], V0.s[0]
    ADRP X15, L_float1@PAGE
    ADD X15, X15, L_float1@PAGEOFF
    LDR D1, [X15, #0]
    fmov Q0, D1
    ins v0.s[1], V0.s[0]
    ADRP X9, L_float1@PAGE
    ADD X9, X9, L_float1@PAGEOFF
    LDR D1, [X9, #0]
    fmov Q0, D1
    ins v0.s[2], V0.s[0]
    ADRP X10, L_float1@PAGE
    ADD X10, X10, L_float1@PAGEOFF
    LDR D1, [X10, #0]
    fmov Q0, D1
    ins v0.s[3], V0.s[0]
    ADRP X11, L_float1@PAGE
    ADD X11, X11, L_float1@PAGEOFF
    LDR D1, [X11, #0]
    fmov Q0, D1
    ins v0.s[4], V0.s[0]
    ADRP X12, L_float1@PAGE
    ADD X12, X12, L_float1@PAGEOFF
    LDR D1, [X12, #0]
    fmov Q0, D1
    ins v0.s[5], V0.s[0]
    ADRP X13, L_float1@PAGE
    ADD X13, X13, L_float1@PAGEOFF
    LDR D1, [X13, #0]
    fmov Q0, D1
    ins v0.s[6], V0.s[0]
    ADRP X14, L_float1@PAGE
    ADD X14, X14, L_float1@PAGEOFF
    LDR D1, [X14, #0]
    fmov Q0, D1
    ins v0.s[7], V0.s[0]
    str D0, [X26]
    MOVZ X14, #0
    LSL X15, X14, #3
    ADD X9, X24, X15
    STR X26, [X9, #0]
    ADRP X10, L_float16@PAGE
    ADD X10, X10, L_float16@PAGEOFF
    LDR D1, [X10, #0]
    fmov Q0, D1
    ins v0.s[0], V0.s[0]
    ADRP X11, L_float16@PAGE
    ADD X11, X11, L_float16@PAGEOFF
    LDR D1, [X11, #0]
    fmov Q0, D1
    ins v0.s[1], V0.s[0]
    ADRP X12, L_float16@PAGE
    ADD X12, X12, L_float16@PAGEOFF
    LDR D1, [X12, #0]
    fmov Q0, D1
    ins v0.s[2], V0.s[0]
    ADRP X13, L_float16@PAGE
    ADD X13, X13, L_float16@PAGEOFF
    LDR D1, [X13, #0]
    fmov Q0, D1
    ins v0.s[3], V0.s[0]
    ADRP X14, L_float16@PAGE
    ADD X14, X14, L_float16@PAGEOFF
    LDR D1, [X14, #0]
    fmov Q0, D1
    ins v0.s[4], V0.s[0]
    ADRP X15, L_float16@PAGE
    ADD X15, X15, L_float16@PAGEOFF
    LDR D1, [X15, #0]
    fmov Q0, D1
    ins v0.s[5], V0.s[0]
    ADRP X9, L_float16@PAGE
    ADD X9, X9, L_float16@PAGEOFF
    LDR D1, [X9, #0]
    fmov Q0, D1
    ins v0.s[6], V0.s[0]
    ADRP X10, L_float16@PAGE
    ADD X10, X10, L_float16@PAGEOFF
    LDR D1, [X10, #0]
    fmov Q0, D1
    ins v0.s[7], V0.s[0]
    str D0, [X26]
    MOVZ X10, #1
    LSL X11, X10, #3
    ADD X12, X24, X11
    STR X26, [X12, #0]
    ADRP X11, L_float12@PAGE
    ADD X11, X11, L_float12@PAGEOFF
    LDR D1, [X11, #0]
    fmov Q0, D1
    ins v0.s[0], V0.s[0]
    ADRP X12, L_float12@PAGE
    ADD X12, X12, L_float12@PAGEOFF
    LDR D1, [X12, #0]
    fmov Q0, D1
    ins v0.s[1], V0.s[0]
    ADRP X13, L_float12@PAGE
    ADD X13, X13, L_float12@PAGEOFF
    LDR D1, [X13, #0]
    fmov Q0, D1
    ins v0.s[2], V0.s[0]
    ADRP X14, L_float12@PAGE
    ADD X14, X14, L_float12@PAGEOFF
    LDR D1, [X14, #0]
    fmov Q0, D1
    ins v0.s[3], V0.s[0]
    ADRP X15, L_float12@PAGE
    ADD X15, X15, L_float12@PAGEOFF
    LDR D1, [X15, #0]
    fmov Q0, D1
    ins v0.s[4], V0.s[0]
    ADRP X9, L_float12@PAGE
    ADD X9, X9, L_float12@PAGEOFF
    LDR D1, [X9, #0]
    fmov Q0, D1
    ins v0.s[5], V0.s[0]
    ADRP X10, L_float12@PAGE
    ADD X10, X10, L_float12@PAGEOFF
    LDR D1, [X10, #0]
    fmov Q0, D1
    ins v0.s[6], V0.s[0]
    ADRP X11, L_float12@PAGE
    ADD X11, X11, L_float12@PAGEOFF
    LDR D1, [X11, #0]
    fmov Q0, D1
    ins v0.s[7], V0.s[0]
    str D0, [X26]
    MOVZ X11, #2
    LSL X12, X11, #3
    ADD X13, X24, X12
    STR X26, [X13, #0]
    ADRP X12, L_float32@PAGE
    ADD X12, X12, L_float32@PAGEOFF
    LDR D1, [X12, #0]
    fmov Q0, D1
    ins v0.s[0], V0.s[0]
    ADRP X13, L_float32@PAGE
    ADD X13, X13, L_float32@PAGEOFF
    LDR D1, [X13, #0]
    fmov Q0, D1
    ins v0.s[1], V0.s[0]
    ADRP X14, L_float32@PAGE
    ADD X14, X14, L_float32@PAGEOFF
    LDR D1, [X14, #0]
    fmov Q0, D1
    ins v0.s[2], V0.s[0]
    ADRP X15, L_float32@PAGE
    ADD X15, X15, L_float32@PAGEOFF
    LDR D1, [X15, #0]
    fmov Q0, D1
    ins v0.s[3], V0.s[0]
    ADRP X9, L_float32@PAGE
    ADD X9, X9, L_float32@PAGEOFF
    LDR D1, [X9, #0]
    fmov Q0, D1
    ins v0.s[4], V0.s[0]
    ADRP X10, L_float32@PAGE
    ADD X10, X10, L_float32@PAGEOFF
    LDR D1, [X10, #0]
    fmov Q0, D1
    ins v0.s[5], V0.s[0]
    ADRP X11, L_float32@PAGE
    ADD X11, X11, L_float32@PAGEOFF
    LDR D1, [X11, #0]
    fmov Q0, D1
    ins v0.s[6], V0.s[0]
    ADRP X12, L_float32@PAGE
    ADD X12, X12, L_float32@PAGEOFF
    LDR D1, [X12, #0]
    fmov Q0, D1
    ins v0.s[7], V0.s[0]
    str D0, [X26]
    MOVZ X12, #3
    LSL X13, X12, #3
    ADD X14, X24, X13
    STR X26, [X14, #0]
    MOVZ X12, #4
    MOV X0, X12
    BL _FGETVEC
    MOV X26, X0
    MOV X23, X26
    SUB SP, SP, #16
    STR X25, [SP, #0]
    LDR X0, [SP, #0]
    ADD SP, SP, #16
    SUB X13, X25, #8
    LDR X12, [X13, #0] ; Load vector/string length
    STR X12, [X29, #80] ; __vec_len_3
    MOV X0, X12
    BL _FGETVEC
    MOV X26, X0
    MOV X23, X26
    MOVZ X12, #0
    MOV X27, X12
    B L_START_ForHeader_5
L_START_ForExit_8:
    ADRP X12, L_str7_plus_8@PAGE
    ADD X12, X12, L_str7_plus_8@PAGEOFF
    MOV X0, X12
    BL _WRITEF
    ADRP X9, L_str8_plus_8@PAGE
    ADD X9, X9, L_str8_plus_8@PAGEOFF
    MOV X0, X9
    SUB X10, X23, #8
    LDR X9, [X10, #0] ; Load vector/table/string length
    MOV X1, X9
    BL _WRITEF1
    ADRP X9, L_str9_plus_8@PAGE
    ADD X9, X9, L_str9_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str10_plus_8@PAGE
    ADD X9, X9, L_str10_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    B L_START_Exit_9
L_START_ForHeader_1:
    MOV X9, X19
    LDR X10, [X29, #72] ; __vec_len_1
    SUB X10, X10, #1
    CMP X9, X10
    B.GT L_START_ForExit_4
    B L_START_ForBody_2
L_START_ForHeader_5:
    MOV X9, X27
    LDR X11, [X29, #80] ; __vec_len_3
    SUB X11, X11, #1
    CMP X9, X11
    B.GT L_START_ForExit_8
    B L_START_ForBody_6
L_START_ForIncrement_3:
    ADD X19, X19, #1
    B L_START_ForHeader_1
L_START_ForIncrement_7:
    ADD X27, X27, #1
    B L_START_ForHeader_5
L__bounds_error_handler_START:
    MOVZ X0, #0
    MOVZ X1, #65535
    MOVZ X2, #0
    BL _BCPL_BOUNDS_ERROR
    BRK #0
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
L___veneer_:
    movz x16, #50776
    movk x16, #718, lsl #16
    movk x16, #1, lsl #32
    movk x16, #0, lsl #48
    blr x16

.section __DATA,__const
.p2align 3
L_str0:
    .quad 0x35
    ; (upper half)
.p2align 2
L_str0_plus_8:
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x4f
    .long 0x43
    .long 0x54
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
    .long 0x77
    .long 0x69
    .long 0x74
    .long 0x68
    .long 0x20
    .long 0x4e
    .long 0x45
    .long 0x4f
    .long 0x4e
    .long 0x20
    .long 0x6f
    .long 0x70
    .long 0x74
    .long 0x69
    .long 0x6d
    .long 0x69
    .long 0x7a
    .long 0x61
    .long 0x74
    .long 0x69
    .long 0x6f
    .long 0x6e
    .long 0x5c
    .long 0x6e
    .long 0x0
    .long 0x0
.p2align 3
L_str1:
    .quad 0x23
    ; (upper half)
.p2align 2
L_str1_plus_8:
    .long 0x41
    .long 0x6c
    .long 0x6c
    .long 0x6f
    .long 0x63
    .long 0x61
    .long 0x74
    .long 0x65
    .long 0x64
    .long 0x20
    .long 0x4f
    .long 0x43
    .long 0x54
    .long 0x53
    .long 0x20
    .long 0x76
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x6f
    .long 0x72
    .long 0x73
    .long 0x20
    .long 0x6f
    .long 0x66
    .long 0x20
    .long 0x73
    .long 0x69
    .long 0x7a
    .long 0x65
    .long 0x20
    .long 0x25
    .long 0x64
    .long 0x5c
    .long 0x6e
    .long 0x0
    .long 0x0
.p2align 3
L_str2:
    .quad 0x19
    ; (upper half)
.p2align 2
L_str2_plus_8:
    .long 0x56
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x6f
    .long 0x72
    .long 0x20
    .long 0x64
    .long 0x61
    .long 0x74
    .long 0x61
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
    .long 0x5c
    .long 0x6e
    .long 0x0
    .long 0x0
.p2align 3
L_str3:
    .quad 0x24
    ; (upper half)
.p2align 2
L_str3_plus_8:
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
    .long 0x4f
    .long 0x43
    .long 0x54
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
    .long 0x2e
    .long 0x2e
    .long 0x2e
    .long 0x5c
    .long 0x6e
    .long 0x0
    .long 0x0
.p2align 3
L_str4:
    .quad 0x2e
    ; (upper half)
.p2align 2
L_str4_plus_8:
    .long 0x4f
    .long 0x43
    .long 0x54
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
    .long 0x63
    .long 0x6f
    .long 0x6d
    .long 0x70
    .long 0x6c
    .long 0x65
    .long 0x74
    .long 0x65
    .long 0x64
    .long 0x20
    .long 0x73
    .long 0x75
    .long 0x63
    .long 0x63
    .long 0x65
    .long 0x73
    .long 0x73
    .long 0x66
    .long 0x75
    .long 0x6c
    .long 0x6c
    .long 0x79
    .long 0x21
    .long 0x5c
    .long 0x6e
    .long 0x0
    .long 0x0
.p2align 3
L_str5:
    .quad 0x1a
    ; (upper half)
.p2align 2
L_str5_plus_8:
    .long 0x52
    .long 0x65
    .long 0x73
    .long 0x75
    .long 0x6c
    .long 0x74
    .long 0x20
    .long 0x76
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x6f
    .long 0x72
    .long 0x20
    .long 0x6c
    .long 0x65
    .long 0x6e
    .long 0x67
    .long 0x74
    .long 0x68
    .long 0x3a
    .long 0x20
    .long 0x25
    .long 0x64
    .long 0x5c
    .long 0x6e
    .long 0x0
    .long 0x0
.p2align 3
L_str6:
    .quad 0x27
    ; (upper half)
.p2align 2
L_str6_plus_8:
    .long 0x5c
    .long 0x6e
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x46
    .long 0x4f
    .long 0x43
    .long 0x54
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
    .long 0x5c
    .long 0x6e
    .long 0x0
    .long 0x0
.p2align 3
L_str7:
    .quad 0x28
    ; (upper half)
.p2align 2
L_str7_plus_8:
    .long 0x46
    .long 0x4f
    .long 0x43
    .long 0x54
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
    .long 0x63
    .long 0x6f
    .long 0x6d
    .long 0x70
    .long 0x6c
    .long 0x65
    .long 0x74
    .long 0x65
    .long 0x64
    .long 0x21
    .long 0x5c
    .long 0x6e
    .long 0x0
    .long 0x0
.p2align 3
L_str8:
    .quad 0x20
    ; (upper half)
.p2align 2
L_str8_plus_8:
    .long 0x46
    .long 0x4f
    .long 0x43
    .long 0x54
    .long 0x53
    .long 0x20
    .long 0x72
    .long 0x65
    .long 0x73
    .long 0x75
    .long 0x6c
    .long 0x74
    .long 0x20
    .long 0x76
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x6f
    .long 0x72
    .long 0x20
    .long 0x6c
    .long 0x65
    .long 0x6e
    .long 0x67
    .long 0x74
    .long 0x68
    .long 0x3a
    .long 0x20
    .long 0x25
    .long 0x64
    .long 0x5c
    .long 0x6e
    .long 0x0
    .long 0x0
.p2align 3
L_str9:
    .quad 0x42
    ; (upper half)
.p2align 2
L_str9_plus_8:
    .long 0x5c
    .long 0x6e
    .long 0x2705
    .long 0x20
    .long 0x41
    .long 0x6c
    .long 0x6c
    .long 0x20
    .long 0x4f
    .long 0x43
    .long 0x54
    .long 0x53
    .long 0x20
    .long 0x61
    .long 0x6e
    .long 0x64
    .long 0x20
    .long 0x46
    .long 0x4f
    .long 0x43
    .long 0x54
    .long 0x53
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
    .long 0x20
    .long 0x73
    .long 0x75
    .long 0x63
    .long 0x63
    .long 0x65
    .long 0x73
    .long 0x73
    .long 0x66
    .long 0x75
    .long 0x6c
    .long 0x6c
    .long 0x79
    .long 0x21
    .long 0x5c
    .long 0x6e
    .long 0x0
    .long 0x0
.p2align 3
L_str10:
    .quad 0x47
    ; (upper half)
.p2align 2
L_str10_plus_8:
    .long 0x4e
    .long 0x45
    .long 0x4f
    .long 0x4e
    .long 0x20
    .long 0x31
    .long 0x32
    .long 0x38
    .long 0x2d
    .long 0x62
    .long 0x69
    .long 0x74
    .long 0x20
    .long 0x6f
    .long 0x70
    .long 0x74
    .long 0x69
    .long 0x6d
    .long 0x69
    .long 0x7a
    .long 0x61
    .long 0x74
    .long 0x69
    .long 0x6f
    .long 0x6e
    .long 0x73
    .long 0x20
    .long 0x73
    .long 0x68
    .long 0x6f
    .long 0x75
    .long 0x6c
    .long 0x64
    .long 0x20
    .long 0x68
    .long 0x61
    .long 0x76
    .long 0x65
    .long 0x20
    .long 0x62
    .long 0x65
    .long 0x65
    .long 0x6e
    .long 0x20
    .long 0x61
    .long 0x70
    .long 0x70
    .long 0x6c
    .long 0x69
    .long 0x65
    .long 0x64
    .long 0x20
    .long 0x66
    .long 0x6f
    .long 0x72
    .long 0x20
    .long 0x6c
    .long 0x61
    .long 0x72
    .long 0x67
    .long 0x65
    .long 0x20
    .long 0x76
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x6f
    .long 0x72
    .long 0x73
    .long 0x5c
    .long 0x6e
    .long 0x0
    .long 0x0
.p2align 3
L_float0:
    .quad 0x3ff0000000000000
    ; (upper half)
L_float1:
    .quad 0x4000000000000000
    ; (upper half)
L_float2:
    .quad 0x4008000000000000
    ; (upper half)
L_float3:
    .quad 0x4010000000000000
    ; (upper half)
L_float4:
    .quad 0x4014000000000000
    ; (upper half)
L_float5:
    .quad 0x4018000000000000
    ; (upper half)
L_float6:
    .quad 0x401c000000000000
    ; (upper half)
L_float7:
    .quad 0x4020000000000000
    ; (upper half)
L_float8:
    .quad 0x3fb999999999999a
    ; (upper half)
L_float9:
    .quad 0x3fc999999999999a
    ; (upper half)
L_float10:
    .quad 0x3fd3333333333333
    ; (upper half)
L_float11:
    .quad 0x3fd999999999999a
    ; (upper half)
L_float12:
    .quad 0x3fe0000000000000
    ; (upper half)
L_float13:
    .quad 0x3fe3333333333333
    ; (upper half)
L_float14:
    .quad 0x3fe6666666666666
    ; (upper half)
L_float15:
    .quad 0x3fe999999999999a
    ; (upper half)
L_float16:
    .quad 0x4024000000000000
    ; (upper half)
L_float17:
    .quad 0x4026000000000000
    ; (upper half)
L_float18:
    .quad 0x4028000000000000
    ; (upper half)
L_float19:
    .quad 0x402a000000000000
    ; (upper half)
L_float20:
    .quad 0x402c000000000000
    ; (upper half)
L_float21:
    .quad 0x402e000000000000
    ; (upper half)
L_float22:
    .quad 0x4030000000000000
    ; (upper half)
L_float23:
    .quad 0x4031000000000000
    ; (upper half)
L_float24:
    .quad 0x4004000000000000
    ; (upper half)
L_float25:
    .quad 0x400c000000000000
    ; (upper half)
L_float26:
    .quad 0x4012000000000000
    ; (upper half)
L_float27:
    .quad 0x4016000000000000
    ; (upper half)
L_float28:
    .quad 0x401a000000000000
    ; (upper half)
L_float29:
    .quad 0x401e000000000000
    ; (upper half)
L_float30:
    .quad 0x4021000000000000
    ; (upper half)
L_float31:
    .quad 0x4023000000000000
    ; (upper half)
L_float32:
    .quad 0x3ff8000000000000
    ; (upper half)

.section __DATA,__data
.p2align 3
    .long 0x0
    .long 0x0
    .long 0x0
    .long 0x0
    .long 0x0
.p2align 2
L__data_segment_base:
