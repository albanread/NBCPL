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
    STP X29, X30, [SP, #-144]!
    MOV X29, SP
    STP x19, x20, [x29, #56]
    STP x21, x22, [x29, #72]
    STP x23, x24, [x29, #88]
    STP x25, x26, [x29, #104]
    STP x27, x28, [x29, #120]
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
    MOVZ X9, #4
    MOV X20, X9
    LSL X21, X9, #1
    MOV X0, X21
    BL _GETVEC
    MOV X21, X0
    SUB X9, X21, #8
    STR X20, [X9, #0]
    MOV X25, X21
    MOVZ X9, #4
    MOV X20, X9
    LSL X21, X9, #1
    MOV X0, X21
    BL _GETVEC
    MOV X21, X0
    SUB X9, X21, #8
    STR X20, [X9, #0]
    MOV X24, X21
    MOVZ X9, #5
    MOVZ X10, #0
    LSL X11, X10, #3
    ADD X12, X25, X11
    STR X9, [X12, #0]
    MOVZ X9, #7
    MOVZ X10, #1
    LSL X11, X10, #3
    ADD X12, X25, X11
    STR X9, [X12, #0]
    MOVZ X9, #3
    MOVZ X10, #2
    LSL X11, X10, #3
    ADD X12, X25, X11
    STR X9, [X12, #0]
    MOVZ X9, #9
    MOVZ X10, #3
    LSL X11, X10, #3
    ADD X12, X25, X11
    STR X9, [X12, #0]
    MOVZ X9, #2
    MOVZ X10, #0
    LSL X11, X10, #3
    ADD X12, X24, X11
    STR X9, [X12, #0]
    MOVZ X9, #8
    MOVZ X10, #1
    LSL X11, X10, #3
    ADD X12, X24, X11
    STR X9, [X12, #0]
    MOVZ X9, #6
    MOVZ X10, #2
    LSL X11, X10, #3
    ADD X12, X24, X11
    STR X9, [X12, #0]
    MOVZ X9, #1
    MOVZ X10, #3
    LSL X11, X10, #3
    ADD X12, X24, X11
    STR X9, [X12, #0]
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
    ADRP X9, L_str6@PAGE
    ADD X9, X9, L_str6@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    MOV X20, X25
    MOV X21, X24
    MOVZ X9, #8
    MOV X10, X0
    MOV X0, X9
    BL _GETVEC
    MOV X9, X0
    MOV X0, X10
    ; DEBUG: Setting vector length to 4
    MOVZ X11, #4
    ; DEBUG: Subtracting 8 from address to get header location
    SUB X9, X9, #8
    ; DEBUG: Storing length at header location
    STR X11, [X9, #0]
    ; DEBUG: Restoring original address
    ADD X9, X9, #8
    LDR Q0, [X25, #0]
    LDR Q1, [X24, #0]
    SMIN V2.4S, V0.4S, V1.4S
    STR Q2, [X9, #0]
    LDR Q0, [X25, #16]
    LDR Q1, [X24, #16]
    SMIN V2.4S, V0.4S, V1.4S
    STR Q2, [X9, #16]
    MOV X26, X9
    ADRP X9, L_str7@PAGE
    ADD X9, X9, L_str7@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    MOVZ X9, #0
    SUB X11, X26, #8
    LDR X10, [X11, #0] ; Load vector length for bounds check
    CMP X9, X10
    B.HS L__bounds_error_handler_START
    MOV X10, X9
    LSL X10, X10, #3
    ADD X11, X26, X10
    LDR X9, [X11, #0]
    MOV X1, X9
    MOVZ X9, #1
    SUB X11, X26, #8
    LDR X10, [X11, #0] ; Load vector length for bounds check
    CMP X9, X10
    B.HS L__bounds_error_handler_START
    MOV X10, X9
    LSL X10, X10, #3
    ADD X11, X26, X10
    LDR X9, [X11, #0]
    MOV X2, X9
    MOVZ X9, #2
    SUB X11, X26, #8
    LDR X10, [X11, #0] ; Load vector length for bounds check
    CMP X9, X10
    B.HS L__bounds_error_handler_START
    MOV X10, X9
    LSL X10, X10, #3
    ADD X11, X26, X10
    LDR X9, [X11, #0]
    MOV X3, X9
    MOVZ X9, #3
    SUB X11, X26, #8
    LDR X10, [X11, #0] ; Load vector length for bounds check
    CMP X9, X10
    B.HS L__bounds_error_handler_START
    MOV X10, X9
    LSL X10, X10, #3
    ADD X11, X26, X10
    LDR X9, [X11, #0]
    MOV X4, X9
    BL _WRITEF4
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
    ADRP X9, L_str5@PAGE
    ADD X9, X9, L_str5@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str10@PAGE
    ADD X9, X9, L_str10@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    MOV X22, X25
    MOVZ X9, #8
    MOV X10, X0
    MOV X0, X9
    BL _GETVEC
    MOV X9, X0
    MOV X0, X10
    ; DEBUG: Setting vector length to 4
    MOVZ X11, #4
    ; DEBUG: Subtracting 8 from address to get header location
    SUB X9, X9, #8
    ; DEBUG: Storing length at header location
    STR X11, [X9, #0]
    ; DEBUG: Restoring original address
    ADD X9, X9, #8
    LDR Q0, [X25, #0]
    LDR Q1, [X24, #0]
    SMAX V2.4S, V0.4S, V1.4S
    STR Q2, [X9, #0]
    LDR Q0, [X25, #16]
    LDR Q1, [X24, #16]
    SMAX V2.4S, V0.4S, V1.4S
    STR Q2, [X9, #16]
    MOV X27, X9
    ADRP X9, L_str7@PAGE
    ADD X9, X9, L_str7@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    MOVZ X9, #0
    SUB X11, X27, #8
    LDR X10, [X11, #0] ; Load vector length for bounds check
    CMP X9, X10
    B.HS L__bounds_error_handler_START
    MOV X10, X9
    LSL X10, X10, #3
    ADD X11, X27, X10
    LDR X9, [X11, #0]
    MOV X1, X9
    MOVZ X9, #1
    SUB X11, X27, #8
    LDR X10, [X11, #0] ; Load vector length for bounds check
    CMP X9, X10
    B.HS L__bounds_error_handler_START
    MOV X10, X9
    LSL X10, X10, #3
    ADD X11, X27, X10
    LDR X9, [X11, #0]
    MOV X2, X9
    MOVZ X9, #2
    SUB X11, X27, #8
    LDR X10, [X11, #0] ; Load vector length for bounds check
    CMP X9, X10
    B.HS L__bounds_error_handler_START
    MOV X10, X9
    LSL X10, X10, #3
    ADD X11, X27, X10
    LDR X9, [X11, #0]
    MOV X3, X9
    MOVZ X9, #3
    SUB X11, X27, #8
    LDR X10, [X11, #0] ; Load vector length for bounds check
    CMP X9, X10
    B.HS L__bounds_error_handler_START
    MOV X10, X9
    LSL X10, X10, #3
    ADD X11, X27, X10
    LDR X9, [X11, #0]
    MOV X4, X9
    BL _WRITEF4
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
    ADRP X9, L_str5@PAGE
    ADD X9, X9, L_str5@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str13@PAGE
    ADD X9, X9, L_str13@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    MOV X26, X24
    MOVZ X9, #8
    MOV X10, X0
    MOV X0, X9
    BL _GETVEC
    MOV X9, X0
    MOV X0, X10
    ; DEBUG: Setting vector length to 4
    MOVZ X11, #4
    ; DEBUG: Subtracting 8 from address to get header location
    SUB X9, X9, #8
    ; DEBUG: Storing length at header location
    STR X11, [X9, #0]
    ; DEBUG: Restoring original address
    ADD X9, X9, #8
    LDR Q0, [X25, #0]
    LDR Q1, [X24, #0]
    ADD V2.4S, V0.4S, V1.4S
    STR Q2, [X9, #0]
    LDR Q0, [X25, #16]
    LDR Q1, [X24, #16]
    ADD V2.4S, V0.4S, V1.4S
    STR Q2, [X9, #16]
    MOV X23, X9
    ADRP X9, L_str7@PAGE
    ADD X9, X9, L_str7@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    MOVZ X9, #0
    SUB X11, X23, #8
    LDR X10, [X11, #0] ; Load vector length for bounds check
    CMP X9, X10
    B.HS L__bounds_error_handler_START
    MOV X10, X9
    LSL X10, X10, #3
    ADD X11, X23, X10
    LDR X9, [X11, #0]
    MOV X1, X9
    MOVZ X9, #1
    SUB X11, X23, #8
    LDR X10, [X11, #0] ; Load vector length for bounds check
    CMP X9, X10
    B.HS L__bounds_error_handler_START
    MOV X10, X9
    LSL X10, X10, #3
    ADD X11, X23, X10
    LDR X9, [X11, #0]
    MOV X2, X9
    MOVZ X9, #2
    SUB X11, X23, #8
    LDR X10, [X11, #0] ; Load vector length for bounds check
    CMP X9, X10
    B.HS L__bounds_error_handler_START
    MOV X10, X9
    LSL X10, X10, #3
    ADD X11, X23, X10
    LDR X9, [X11, #0]
    MOV X3, X9
    MOVZ X9, #3
    SUB X11, X23, #8
    LDR X10, [X11, #0] ; Load vector length for bounds check
    CMP X9, X10
    B.HS L__bounds_error_handler_START
    MOV X10, X9
    LSL X10, X10, #3
    ADD X11, X23, X10
    LDR X9, [X11, #0]
    MOV X4, X9
    BL _WRITEF4
    ADRP X9, L_str14@PAGE
    ADD X9, X9, L_str14@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str15@PAGE
    ADD X9, X9, L_str15@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str5@PAGE
    ADD X9, X9, L_str5@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str16@PAGE
    ADD X9, X9, L_str16@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    MOVZ X9, #0
    SUB X11, X26, #8
    LDR X10, [X11, #0] ; Load vector length for bounds check
    CMP X9, X10
    B.HS L__bounds_error_handler_START
    MOV X10, X9
    LSL X10, X10, #3
    ADD X11, X26, X10
    LDR X9, [X11, #0]
    CMP X9, #2
    CSET X10, EQ
    CMP X10, #0
    B.EQ L_1
    MOVZ X9, #1
    SUB X12, X26, #8
    LDR X11, [X12, #0] ; Load vector length for bounds check
    CMP X9, X11
    B.HS L__bounds_error_handler_START
    MOV X11, X9
    LSL X11, X11, #3
    ADD X12, X26, X11
    LDR X9, [X12, #0]
    CMP X9, #7
    CSET X11, EQ
    B L_2
L_1:
    MOVZ X11, #0
L_2:
    CMP X11, #0
    B.EQ L_3
    MOVZ X9, #2
    SUB X13, X26, #8
    LDR X12, [X13, #0] ; Load vector length for bounds check
    CMP X9, X12
    B.HS L__bounds_error_handler_START
    MOV X12, X9
    LSL X12, X12, #3
    ADD X13, X26, X12
    LDR X9, [X13, #0]
    CMP X9, #3
    CSET X12, EQ
    B L_4
L_3:
    MOVZ X12, #0
L_4:
    CMP X12, #0
    B.EQ L_5
    MOVZ X9, #3
    SUB X14, X26, #8
    LDR X13, [X14, #0] ; Load vector length for bounds check
    CMP X9, X13
    B.HS L__bounds_error_handler_START
    MOV X13, X9
    LSL X13, X13, #3
    ADD X14, X26, X13
    LDR X9, [X14, #0]
    CMP X9, #1
    CSET X13, EQ
    B L_6
L_5:
    MOVZ X13, #0
L_6:
    CMP X13, XZR
    B.EQ L_START_Else_2
    B L_START_Then_1
L_START_Else_2:
    ADRP X9, L_str17@PAGE
    ADD X9, X9, L_str17@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    B L_START_Join_3
L_START_Else_5:
    ADRP X9, L_str18@PAGE
    ADD X9, X9, L_str18@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    B L_START_Join_6
L_START_Else_8:
    ADRP X9, L_str19@PAGE
    ADD X9, X9, L_str19@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    B L_START_Join_9
L_START_Exit_10:
    B L_0
L_START_Join_3:
    MOVZ X9, #0
    SUB X11, X27, #8
    LDR X10, [X11, #0] ; Load vector length for bounds check
    CMP X9, X10
    B.HS L__bounds_error_handler_START
    MOV X10, X9
    LSL X10, X10, #3
    ADD X11, X27, X10
    LDR X9, [X11, #0]
    CMP X9, #5
    CSET X10, EQ
    CMP X10, #0
    B.EQ L_7
    MOVZ X9, #1
    SUB X12, X27, #8
    LDR X11, [X12, #0] ; Load vector length for bounds check
    CMP X9, X11
    B.HS L__bounds_error_handler_START
    MOV X11, X9
    LSL X11, X11, #3
    ADD X12, X27, X11
    LDR X9, [X12, #0]
    CMP X9, #8
    CSET X11, EQ
    B L_8
L_7:
    MOVZ X11, #0
L_8:
    CMP X11, #0
    B.EQ L_9
    MOVZ X9, #2
    SUB X13, X27, #8
    LDR X12, [X13, #0] ; Load vector length for bounds check
    CMP X9, X12
    B.HS L__bounds_error_handler_START
    MOV X12, X9
    LSL X12, X12, #3
    ADD X13, X27, X12
    LDR X9, [X13, #0]
    CMP X9, #6
    CSET X12, EQ
    B L_10
L_9:
    MOVZ X12, #0
L_10:
    CMP X12, #0
    B.EQ L_11
    MOVZ X9, #3
    SUB X14, X27, #8
    LDR X13, [X14, #0] ; Load vector length for bounds check
    CMP X9, X13
    B.HS L__bounds_error_handler_START
    MOV X13, X9
    LSL X13, X13, #3
    ADD X14, X27, X13
    LDR X9, [X14, #0]
    CMP X9, #9
    CSET X13, EQ
    B L_12
L_11:
    MOVZ X13, #0
L_12:
    CMP X13, XZR
    B.EQ L_START_Else_5
    B L_START_Then_4
L_START_Join_6:
    MOVZ X9, #0
    SUB X14, X23, #8
    LDR X13, [X14, #0] ; Load vector length for bounds check
    CMP X9, X13
    B.HS L__bounds_error_handler_START
    MOV X13, X9
    LSL X13, X13, #3
    ADD X14, X23, X13
    LDR X9, [X14, #0]
    CMP X9, #7
    CSET X13, EQ
    CMP X13, #0
    B.EQ L_13
    MOVZ X9, #1
    SUB X15, X23, #8
    LDR X14, [X15, #0] ; Load vector length for bounds check
    CMP X9, X14
    B.HS L__bounds_error_handler_START
    MOV X14, X9
    LSL X14, X14, #3
    ADD X15, X23, X14
    LDR X9, [X15, #0]
    CMP X9, #15
    CSET X14, EQ
    B L_14
L_13:
    MOVZ X14, #0
L_14:
    CMP X14, #0
    B.EQ L_15
    MOVZ X9, #2
    SUB X9, X23, #8
    LDR X15, [X9, #0] ; Load vector length for bounds check
    CMP X9, X15
    B.HS L__bounds_error_handler_START
    LSL X9, X9, #3
    ADD X10, X23, X9
    LDR X9, [X10, #0]
    CMP X9, #9
    CSET X10, EQ
    B L_16
L_15:
    MOVZ X10, #0
L_16:
    CMP X10, #0
    B.EQ L_17
    MOVZ X9, #3
    SUB X12, X23, #8
    LDR X11, [X12, #0] ; Load vector length for bounds check
    CMP X9, X11
    B.HS L__bounds_error_handler_START
    MOV X11, X9
    LSL X11, X11, #3
    ADD X12, X23, X11
    LDR X9, [X12, #0]
    CMP X9, #10
    CSET X11, EQ
    B L_18
L_17:
    MOVZ X11, #0
L_18:
    CMP X11, XZR
    B.EQ L_START_Else_8
    B L_START_Then_7
L_START_Join_9:
    ADRP X9, L_str5@PAGE
    ADD X9, X9, L_str5@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str20@PAGE
    ADD X9, X9, L_str20@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str21@PAGE
    ADD X9, X9, L_str21@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str22@PAGE
    ADD X9, X9, L_str22@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str23@PAGE
    ADD X9, X9, L_str23@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str24@PAGE
    ADD X9, X9, L_str24@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str5@PAGE
    ADD X9, X9, L_str5@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str25@PAGE
    ADD X9, X9, L_str25@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    B L_START_Exit_10
L_START_Then_1:
    ADRP X9, L_str26@PAGE
    ADD X9, X9, L_str26@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    B L_START_Join_3
L_START_Then_4:
    ADRP X9, L_str27@PAGE
    ADD X9, X9, L_str27@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    B L_START_Join_6
L_START_Then_7:
    ADRP X9, L_str28@PAGE
    ADD X9, X9, L_str28@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    B L_START_Join_9
L__bounds_error_handler_START:
    MOVZ X0, #0
    MOVZ X1, #0
    MOVZ X2, #0
    BL _BCPL_BOUNDS_ERROR
    BRK #0
L_0:
    LDP x19, x20, [x29, #56]
    LDP x21, x22, [x29, #72]
    LDP x23, x24, [x29, #88]
    LDP x25, x26, [x29, #104]
    LDP x27, x28, [x29, #120]
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [SP, #0]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L___veneer_:
    movz x16, #61152
    movk x16, #103, lsl #16
    movk x16, #1, lsl #32
    movk x16, #0, lsl #48
    blr x16

.section __DATA,__const
.p2align 3
L_str0:
    .quad 0x2e
    ; (upper half)
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x41
    .long 0x6c
    .long 0x6c
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
    .long 0x52
    .long 0x65
    .long 0x64
    .long 0x75
    .long 0x63
    .long 0x74
    .long 0x69
    .long 0x6f
    .long 0x6e
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
    .quad 0x2d
    ; (upper half)
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0xa
    .long 0x0
    .long 0x0
L_str2:
    .quad 0xf
    ; (upper half)
    .long 0x49
    .long 0x6e
    .long 0x70
    .long 0x75
    .long 0x74
    .long 0x20
    .long 0x76
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x6f
    .long 0x72
    .long 0x73
    .long 0x3a
    .long 0xa
    .long 0x0
    .long 0x0
L_str3:
    .quad 0x15
    ; (upper half)
    .long 0x20
    .long 0x20
    .long 0x70
    .long 0x61
    .long 0x69
    .long 0x72
    .long 0x73
    .long 0x31
    .long 0x3a
    .long 0x20
    .long 0x35
    .long 0x2c
    .long 0x20
    .long 0x37
    .long 0x2c
    .long 0x20
    .long 0x33
    .long 0x2c
    .long 0x20
    .long 0x39
    .long 0xa
    .long 0x0
    .long 0x0
L_str4:
    .quad 0x15
    ; (upper half)
    .long 0x20
    .long 0x20
    .long 0x70
    .long 0x61
    .long 0x69
    .long 0x72
    .long 0x73
    .long 0x32
    .long 0x3a
    .long 0x20
    .long 0x32
    .long 0x2c
    .long 0x20
    .long 0x38
    .long 0x2c
    .long 0x20
    .long 0x36
    .long 0x2c
    .long 0x20
    .long 0x31
    .long 0xa
    .long 0x0
    .long 0x0
L_str5:
    .quad 0x1
    ; (upper half)
    .long 0xa
    .long 0x0
    .long 0x0
L_str6:
    .quad 0x30
    ; (upper half)
    .long 0x31
    .long 0x2e
    .long 0x20
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x4d
    .long 0x49
    .long 0x4e
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
    .long 0x28
    .long 0x65
    .long 0x6c
    .long 0x65
    .long 0x6d
    .long 0x65
    .long 0x6e
    .long 0x74
    .long 0x2d
    .long 0x77
    .long 0x69
    .long 0x73
    .long 0x65
    .long 0x20
    .long 0x6d
    .long 0x69
    .long 0x6e
    .long 0x69
    .long 0x6d
    .long 0x75
    .long 0x6d
    .long 0x29
    .long 0xa
    .long 0x0
    .long 0x0
L_str7:
    .quad 0x1a
    ; (upper half)
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x52
    .long 0x65
    .long 0x73
    .long 0x75
    .long 0x6c
    .long 0x74
    .long 0x3a
    .long 0x20
    .long 0x25
    .long 0x4e
    .long 0x2c
    .long 0x20
    .long 0x25
    .long 0x4e
    .long 0x2c
    .long 0x20
    .long 0x25
    .long 0x4e
    .long 0x2c
    .long 0x20
    .long 0x25
    .long 0x4e
    .long 0xa
    .long 0x0
    .long 0x0
L_str8:
    .quad 0x18
    ; (upper half)
    .long 0x20
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
    .long 0x32
    .long 0x2c
    .long 0x20
    .long 0x37
    .long 0x2c
    .long 0x20
    .long 0x33
    .long 0x2c
    .long 0x20
    .long 0x31
    .long 0xa
    .long 0x0
    .long 0x0
L_str9:
    .quad 0x33
    ; (upper half)
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x2713
    .long 0x20
    .long 0x4d
    .long 0x49
    .long 0x4e
    .long 0x20
    .long 0x75
    .long 0x73
    .long 0x65
    .long 0x73
    .long 0x20
    .long 0x4e
    .long 0x45
    .long 0x4f
    .long 0x4e
    .long 0x20
    .long 0x53
    .long 0x4d
    .long 0x49
    .long 0x4e
    .long 0x2e
    .long 0x34
    .long 0x53
    .long 0x20
    .long 0x66
    .long 0x6f
    .long 0x72
    .long 0x20
    .long 0x34
    .long 0x78
    .long 0x20
    .long 0x70
    .long 0x61
    .long 0x72
    .long 0x61
    .long 0x6c
    .long 0x6c
    .long 0x65
    .long 0x6c
    .long 0x20
    .long 0x6d
    .long 0x69
    .long 0x6e
    .long 0x69
    .long 0x6d
    .long 0x75
    .long 0x6d
    .long 0xa
    .long 0x0
    .long 0x0
L_str10:
    .quad 0x30
    ; (upper half)
    .long 0x32
    .long 0x2e
    .long 0x20
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x4d
    .long 0x41
    .long 0x58
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
    .long 0x28
    .long 0x65
    .long 0x6c
    .long 0x65
    .long 0x6d
    .long 0x65
    .long 0x6e
    .long 0x74
    .long 0x2d
    .long 0x77
    .long 0x69
    .long 0x73
    .long 0x65
    .long 0x20
    .long 0x6d
    .long 0x61
    .long 0x78
    .long 0x69
    .long 0x6d
    .long 0x75
    .long 0x6d
    .long 0x29
    .long 0xa
    .long 0x0
    .long 0x0
L_str11:
    .quad 0x18
    ; (upper half)
    .long 0x20
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
    .long 0x35
    .long 0x2c
    .long 0x20
    .long 0x38
    .long 0x2c
    .long 0x20
    .long 0x36
    .long 0x2c
    .long 0x20
    .long 0x39
    .long 0xa
    .long 0x0
    .long 0x0
L_str12:
    .quad 0x33
    ; (upper half)
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x2713
    .long 0x20
    .long 0x4d
    .long 0x41
    .long 0x58
    .long 0x20
    .long 0x75
    .long 0x73
    .long 0x65
    .long 0x73
    .long 0x20
    .long 0x4e
    .long 0x45
    .long 0x4f
    .long 0x4e
    .long 0x20
    .long 0x53
    .long 0x4d
    .long 0x41
    .long 0x58
    .long 0x2e
    .long 0x34
    .long 0x53
    .long 0x20
    .long 0x66
    .long 0x6f
    .long 0x72
    .long 0x20
    .long 0x34
    .long 0x78
    .long 0x20
    .long 0x70
    .long 0x61
    .long 0x72
    .long 0x61
    .long 0x6c
    .long 0x6c
    .long 0x65
    .long 0x6c
    .long 0x20
    .long 0x6d
    .long 0x61
    .long 0x78
    .long 0x69
    .long 0x6d
    .long 0x75
    .long 0x6d
    .long 0xa
    .long 0x0
    .long 0x0
L_str13:
    .quad 0x31
    ; (upper half)
    .long 0x33
    .long 0x2e
    .long 0x20
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x53
    .long 0x55
    .long 0x4d
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
    .long 0x28
    .long 0x65
    .long 0x6c
    .long 0x65
    .long 0x6d
    .long 0x65
    .long 0x6e
    .long 0x74
    .long 0x2d
    .long 0x77
    .long 0x69
    .long 0x73
    .long 0x65
    .long 0x20
    .long 0x61
    .long 0x64
    .long 0x64
    .long 0x69
    .long 0x74
    .long 0x69
    .long 0x6f
    .long 0x6e
    .long 0x29
    .long 0xa
    .long 0x0
    .long 0x0
L_str14:
    .quad 0x1a
    ; (upper half)
    .long 0x20
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
    .long 0x37
    .long 0x2c
    .long 0x20
    .long 0x31
    .long 0x35
    .long 0x2c
    .long 0x20
    .long 0x39
    .long 0x2c
    .long 0x20
    .long 0x31
    .long 0x30
    .long 0xa
    .long 0x0
    .long 0x0
L_str15:
    .quad 0x33
    ; (upper half)
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x2713
    .long 0x20
    .long 0x53
    .long 0x55
    .long 0x4d
    .long 0x20
    .long 0x75
    .long 0x73
    .long 0x65
    .long 0x73
    .long 0x20
    .long 0x4e
    .long 0x45
    .long 0x4f
    .long 0x4e
    .long 0x20
    .long 0x41
    .long 0x44
    .long 0x44
    .long 0x2e
    .long 0x34
    .long 0x53
    .long 0x20
    .long 0x66
    .long 0x6f
    .long 0x72
    .long 0x20
    .long 0x34
    .long 0x78
    .long 0x20
    .long 0x70
    .long 0x61
    .long 0x72
    .long 0x61
    .long 0x6c
    .long 0x6c
    .long 0x65
    .long 0x6c
    .long 0x20
    .long 0x61
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
L_str16:
    .quad 0xe
    ; (upper half)
    .long 0x56
    .long 0x65
    .long 0x72
    .long 0x69
    .long 0x66
    .long 0x69
    .long 0x63
    .long 0x61
    .long 0x74
    .long 0x69
    .long 0x6f
    .long 0x6e
    .long 0x3a
    .long 0xa
    .long 0x0
    .long 0x0
L_str17:
    .quad 0x1b
    ; (upper half)
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x2717
    .long 0x20
    .long 0x4d
    .long 0x49
    .long 0x4e
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
    .long 0x3a
    .long 0x20
    .long 0x46
    .long 0x41
    .long 0x49
    .long 0x4c
    .long 0x45
    .long 0x44
    .long 0xa
    .long 0x0
    .long 0x0
L_str18:
    .quad 0x1b
    ; (upper half)
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x2717
    .long 0x20
    .long 0x4d
    .long 0x41
    .long 0x58
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
    .long 0x3a
    .long 0x20
    .long 0x46
    .long 0x41
    .long 0x49
    .long 0x4c
    .long 0x45
    .long 0x44
    .long 0xa
    .long 0x0
    .long 0x0
L_str19:
    .quad 0x1b
    ; (upper half)
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x2717
    .long 0x20
    .long 0x53
    .long 0x55
    .long 0x4d
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
    .long 0x3a
    .long 0x20
    .long 0x46
    .long 0x41
    .long 0x49
    .long 0x4c
    .long 0x45
    .long 0x44
    .long 0xa
    .long 0x0
    .long 0x0
L_str20:
    .quad 0x16
    ; (upper half)
    .long 0x50
    .long 0x65
    .long 0x72
    .long 0x66
    .long 0x6f
    .long 0x72
    .long 0x6d
    .long 0x61
    .long 0x6e
    .long 0x63
    .long 0x65
    .long 0x20
    .long 0x42
    .long 0x65
    .long 0x6e
    .long 0x65
    .long 0x66
    .long 0x69
    .long 0x74
    .long 0x73
    .long 0x3a
    .long 0xa
    .long 0x0
    .long 0x0
L_str21:
    .quad 0x33
    ; (upper half)
    .long 0x20
    .long 0x20
    .long 0x2d
    .long 0x20
    .long 0x53
    .long 0x49
    .long 0x4d
    .long 0x44
    .long 0x20
    .long 0x70
    .long 0x61
    .long 0x72
    .long 0x61
    .long 0x6c
    .long 0x6c
    .long 0x65
    .long 0x6c
    .long 0x69
    .long 0x73
    .long 0x6d
    .long 0x20
    .long 0x75
    .long 0x73
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x41
    .long 0x52
    .long 0x4d
    .long 0x36
    .long 0x34
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
    .long 0x73
    .long 0xa
    .long 0x0
    .long 0x0
L_str22:
    .quad 0x21
    ; (upper half)
    .long 0x20
    .long 0x20
    .long 0x2d
    .long 0x20
    .long 0x56
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x6f
    .long 0x72
    .long 0x20
    .long 0x72
    .long 0x65
    .long 0x67
    .long 0x69
    .long 0x73
    .long 0x74
    .long 0x65
    .long 0x72
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
    .long 0xa
    .long 0x0
    .long 0x0
L_str23:
    .quad 0x2b
    ; (upper half)
    .long 0x20
    .long 0x20
    .long 0x2d
    .long 0x20
    .long 0x45
    .long 0x66
    .long 0x66
    .long 0x69
    .long 0x63
    .long 0x69
    .long 0x65
    .long 0x6e
    .long 0x74
    .long 0x20
    .long 0x6d
    .long 0x65
    .long 0x6d
    .long 0x6f
    .long 0x72
    .long 0x79
    .long 0x20
    .long 0x62
    .long 0x61
    .long 0x6e
    .long 0x64
    .long 0x77
    .long 0x69
    .long 0x64
    .long 0x74
    .long 0x68
    .long 0x20
    .long 0x75
    .long 0x74
    .long 0x69
    .long 0x6c
    .long 0x69
    .long 0x7a
    .long 0x61
    .long 0x74
    .long 0x69
    .long 0x6f
    .long 0x6e
    .long 0xa
    .long 0x0
    .long 0x0
L_str24:
    .quad 0x36
    ; (upper half)
    .long 0x20
    .long 0x20
    .long 0x2d
    .long 0x20
    .long 0x4f
    .long 0x70
    .long 0x74
    .long 0x69
    .long 0x6d
    .long 0x61
    .long 0x6c
    .long 0x20
    .long 0x72
    .long 0x65
    .long 0x67
    .long 0x69
    .long 0x73
    .long 0x74
    .long 0x65
    .long 0x72
    .long 0x20
    .long 0x61
    .long 0x6c
    .long 0x6c
    .long 0x6f
    .long 0x63
    .long 0x61
    .long 0x74
    .long 0x69
    .long 0x6f
    .long 0x6e
    .long 0x20
    .long 0x66
    .long 0x6f
    .long 0x72
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
L_str25:
    .quad 0x37
    ; (upper half)
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
    .long 0x52
    .long 0x65
    .long 0x64
    .long 0x75
    .long 0x63
    .long 0x74
    .long 0x69
    .long 0x6f
    .long 0x6e
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
    .long 0x3a
    .long 0x20
    .long 0x41
    .long 0x4c
    .long 0x4c
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
    .long 0x21
    .long 0xa
    .long 0x0
    .long 0x0
L_str26:
    .quad 0x1b
    ; (upper half)
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x2713
    .long 0x20
    .long 0x4d
    .long 0x49
    .long 0x4e
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
    .long 0x3a
    .long 0x20
    .long 0x50
    .long 0x41
    .long 0x53
    .long 0x53
    .long 0x45
    .long 0x44
    .long 0xa
    .long 0x0
    .long 0x0
L_str27:
    .quad 0x1b
    ; (upper half)
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x2713
    .long 0x20
    .long 0x4d
    .long 0x41
    .long 0x58
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
    .long 0x3a
    .long 0x20
    .long 0x50
    .long 0x41
    .long 0x53
    .long 0x53
    .long 0x45
    .long 0x44
    .long 0xa
    .long 0x0
    .long 0x0
L_str28:
    .quad 0x1b
    ; (upper half)
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x2713
    .long 0x20
    .long 0x53
    .long 0x55
    .long 0x4d
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
    .long 0x3a
    .long 0x20
    .long 0x50
    .long 0x41
    .long 0x53
    .long 0x53
    .long 0x45
    .long 0x44
    .long 0xa
    .long 0x0
    .long 0x0

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
    .long 0x0
L__data_segment_base:
