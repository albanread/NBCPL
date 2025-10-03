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

L_Global:
    STP X29, X30, [SP, #-32]!
    MOV X29, SP
    STP x19, x28, [x29, #16]
L_Global_Entry_0:
    MOVZ X9, #256
    STR X9, [X28, #0] ; G
    MOV X15, X9
    MOVZ X9, #2
    STR X9, [X28, #8] ; H
    MOV X15, X9
    B L_Global_Exit_1
L_Global_Exit_1:
    B L_0
L_0:
    LDP x19, x28, [x29, #16]
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [SP, #0]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_START:
    STP X29, X30, [SP, #-176]!
    MOV X29, SP
    STP x19, x20, [x29, #88]
    STP x21, x22, [x29, #104]
    STP x23, x24, [x29, #120]
    STP x25, x26, [x29, #136]
    STP x27, x28, [x29, #152]
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
L_START_Entry_0:
    BL _HeapManager_enter_scope
    BL L_Global
    ADRP X9, L_str0@PAGE
    ADD X9, X9, L_str0@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    LDP x9, x10, [x28, #0]
    MUL X9, X9, X10
    MOV X23, X9
    MOV X0, X23
    BL _WRITEN
    ADRP X9, L_str1@PAGE
    ADD X9, X9, L_str1@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    MOVZ X9, #10
    MOV X0, X9
    BL _WRITEC
    MOVZ X9, #256
    STR X9, [X28, #0] ; G
    MOV X27, X9
    MOVZ X9, #2
    STR X9, [X28, #8] ; H
    MOV X26, X9
    ADRP X9, L_str0@PAGE
    ADD X9, X9, L_str0@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    MOV X0, X23
    BL _WRITEN
    ADRP X9, L_str1@PAGE
    ADD X9, X9, L_str1@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    ADRP X9, L_str2@PAGE
    ADD X9, X9, L_str2@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    MOVZ X9, #10
    MOV X0, X9
    BL _GETVEC
    MOV X9, X0
    MOV X24, X9
    MOVZ X9, #10
    MOV X0, X9
    BL _BCPL_ALLOC_CHARS
    MOV X15, X0
    ADRP X9, L_str3@PAGE
    ADD X9, X9, L_str3@PAGEOFF
    ADD X9, X9, #8
    MOV X23, X9
    MOV X0, X23
    BL _WRITES
    CMP x24, #0
    CSET X10, EQ
    CMP X10, XZR
    B.EQ L_START_Join_2
    B L_START_Then_1
L_START_Exit_20:
    B L_1
L_START_FinishCleanup_3:
    MOVZ X9, #0
    MOV X0, X9
    MOVZ X9, #1
    MOVK X9, #512, LSL #16
    MOV X16, X9
    SVC #128
L_START_ForBody_13:
    BL _HeapManager_enter_scope
    MOV X9, X23
    MOVZ X10, #120
    MUL X9, X9, X10
    MOVZ X10, #4
    SDIV X9, X9, X10
    MOV X10, X23
    MOVZ X11, #3
    MUL X10, X10, X11
    ADD X9, X9, X10
    SUB X9, X9, #7
    LSL X10, X23, #3
    ADD X11, X24, X10
    STR X9, [X11, #0]
    ADRP X9, L_str4@PAGE
    ADD X9, X9, L_str4@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    MOV X0, X23
    BL _WRITEN
    ADRP X9, L_str1@PAGE
    ADD X9, X9, L_str1@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    SUB X10, X24, #8
    LDR X9, [X10, #0] ; Load vector length for bounds check
    CMP X23, X9
    B.HS L__bounds_error_handler_START
    MOV X9, X23
    LSL X9, X9, #3
    ADD X10, X24, X9
    LDR X9, [X10, #0]
    MOV X0, X9
    BL _WRITEN
    BL _HeapManager_exit_scope
    B L_START_ForIncrement_14
L_START_ForBody_5:
    BL _HeapManager_enter_scope
    MOV X9, X23
    MOVZ X10, #5
    MUL X9, X9, X10
    LSL X10, X23, #3
    ADD X11, X24, X10
    STR X9, [X11, #0]
    BL _HeapManager_exit_scope
    B L_START_ForIncrement_6
L_START_ForBody_9:
    BL _HeapManager_enter_scope
    ADRP X9, L_str4@PAGE
    ADD X9, X9, L_str4@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    MOV X0, X23
    BL _WRITEN
    ADRP X9, L_str1@PAGE
    ADD X9, X9, L_str1@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    SUB X10, X24, #8
    LDR X9, [X10, #0] ; Load vector length for bounds check
    CMP X23, X9
    B.HS L__bounds_error_handler_START
    MOV X9, X23
    LSL X9, X9, #3
    ADD X10, X24, X9
    LDR X9, [X10, #0]
    MOV X0, X9
    BL _WRITEN
    BL _HeapManager_exit_scope
    B L_START_ForIncrement_10
L_START_ForEachBody_17:
    SUB X10, X24, #8
    LDR X9, [X10, #0] ; Load vector length for bounds check
    CMP X22, X9
    B.HS L__bounds_error_handler_START
    MOV X9, X22
    LSL X9, X9, #3
    ADD X10, X24, X9
    LDR X9, [X10, #0]
    MOV X20, X9
    BL _HeapManager_enter_scope
    MOV X0, X20
    BL _WRITEN
    MOVZ X9, #32
    MOV X0, X9
    BL _WRITEC
    BL _HeapManager_exit_scope
    B L_START_ForEachIncrement_18
L_START_ForEachExit_19:
    BL _HeapManager_exit_scope
    B L_START_Exit_20
L_START_ForEachHeader_16:
    CMP x22, x21
    CSET X10, LT
    CMP X10, XZR
    B.NE L_START_ForEachBody_17
    B L_START_ForEachExit_19
L_START_ForEachIncrement_18:
    MOV X9, X22
    ADD X9, X9, #1
    MOV X22, X9
    B L_START_ForEachHeader_16
L_START_ForExit_11:
    MOVZ X9, #0
    MOV X23, X9
    B L_START_ForHeader_12
L_START_ForExit_15:
    ADRP X9, L_str5@PAGE
    ADD X9, X9, L_str5@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    SUB X10, X24, #8
    LDR X9, [X10, #0] ; Load vector/table/string length
    MOV X21, X9
    MOVZ X9, #0
    MOV X22, X9
    B L_START_ForEachHeader_16
L_START_ForExit_7:
    ADRP X9, L_str4@PAGE
    ADD X9, X9, L_str4@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    MOV X0, X25
    BL _WRITEN
    ADRP X9, L_str1@PAGE
    ADD X9, X9, L_str1@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    MOVZ X9, #0
    MOV X23, X9
    B L_START_ForHeader_8
L_START_ForHeader_12:
    MOV X9, X23
    SUB X11, X24, #8
    LDR X10, [X11, #0] ; Load vector/table/string length
    SUB X10, X10, #1
    CMP X9, X10
    B.GT L_START_ForExit_15
    B L_START_ForBody_13
L_START_ForHeader_4:
    MOV X9, X23
    SUB X12, X24, #8
    LDR X11, [X12, #0] ; Load vector/table/string length
    SUB X11, X11, #1
    CMP X9, X11
    B.GT L_START_ForExit_7
    B L_START_ForBody_5
L_START_ForHeader_8:
    MOV X9, X23
    SUB X13, X24, #8
    LDR X12, [X13, #0] ; Load vector/table/string length
    SUB X12, X12, #1
    CMP X9, X12
    B.GT L_START_ForExit_11
    B L_START_ForBody_9
L_START_ForIncrement_10:
    MOV X9, X23
    ADD X9, X9, #1
    MOV X23, X9
    B L_START_ForHeader_8
L_START_ForIncrement_14:
    MOV X9, X23
    ADD X9, X9, #1
    MOV X23, X9
    B L_START_ForHeader_12
L_START_ForIncrement_6:
    MOV X9, X23
    ADD X9, X9, #1
    MOV X23, X9
    B L_START_ForHeader_4
L_START_Join_2:
    MOVZ X9, #0
    MOV X23, X9
    B L_START_ForHeader_4
L_START_Then_1:
    BL _HeapManager_enter_scope
    ADRP X9, L_str6@PAGE
    ADD X9, X9, L_str6@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    BL _HeapManager_exit_scope
    B L_START_FinishCleanup_3
L__bounds_error_handler_START:
    MOVZ X0, #0
    MOVZ X1, #0
    MOVZ X2, #0
    BL _BCPL_BOUNDS_ERROR
    BRK #0
L_1:
    LDP x19, x20, [x29, #88]
    LDP x21, x22, [x29, #104]
    LDP x23, x24, [x29, #120]
    LDP x25, x26, [x29, #136]
    LDP x27, x28, [x29, #152]
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [SP, #0]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L___veneer_:
    movz x16, #684
    movk x16, #254, lsl #16
    movk x16, #1, lsl #32
    movk x16, #0, lsl #48
    blr x16

.section __DATA,__const
.p2align 3
L_str0:
    .quad 0x4
    ; (upper half)
    .long 0x47
    .long 0x2a
    .long 0x48
    .long 0x3d
    .long 0x0
    .long 0x0
L_str1:
    .quad 0x1
    ; (upper half)
    .long 0xa
    .long 0x0
    .long 0x0
L_str2:
    .quad 0x5
    ; (upper half)
    .long 0x74
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0xa
    .long 0x0
    .long 0x0
L_str3:
    .quad 0x7
    ; (upper half)
    .long 0x48
    .long 0x65
    .long 0x6c
    .long 0x6c
    .long 0x6f
    .long 0x21
    .long 0xa
    .long 0x0
    .long 0x0
L_str4:
    .quad 0x2
    ; (upper half)
    .long 0x49
    .long 0x3d
    .long 0x0
    .long 0x0
L_str5:
    .quad 0xd
    ; (upper half)
    .long 0x46
    .long 0x6f
    .long 0x72
    .long 0x65
    .long 0x61
    .long 0x63
    .long 0x68
    .long 0x20
    .long 0x74
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0xa
    .long 0x0
    .long 0x0
L_str6:
    .quad 0x5
    ; (upper half)
    .long 0x46
    .long 0x41
    .long 0x49
    .long 0x4c
    .long 0xa
    .long 0x0
    .long 0x0

.section __DATA,__data
.p2align 3
    .long 0x0
L__data_segment_base:
    .quad 0x0
    ; (upper half)
    .quad 0x0
    ; (upper half)
