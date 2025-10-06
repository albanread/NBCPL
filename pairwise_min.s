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

L_START:
    STP X29, X30, [SP, #-128]!
    MOV X29, SP
    STP x19, x25, [x29, #88]
    STP x26, x27, [x29, #104]
    STR X28, [X29, #120] ; Saved Reg: X28 @ FP+120
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
L_START_Entry_0:
    BL _HeapManager_enter_scope
    ADRP X9, L_str0@PAGE
    ADD X9, X9, L_str0@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    MOVZ X9, #8
    MOV X0, X9
    BL _FGETVEC
    MOV X9, X0
    MOV X27, X9
    MOVZ X9, #8
    MOV X0, X9
    BL _FGETVEC
    MOV X9, X0
    MOV X26, X9
    MOVZ X9, #4
    MOV X0, X9
    BL _FGETVEC
    MOV X9, X0
    MOV X25, X9
    MOVZ X9, #4
    MOV X0, X9
    BL _FGETVEC
    MOV X9, X0
    MOV X25, X9
    ADRP X10, L_float0@PAGE
    ADD X10, X10, L_float0@PAGEOFF
    LDR D0, [X10, #0]
    MOVZ X10, #0
    LSL X11, X10, #3
    ADD X12, X27, X11
    STR D0, [X12, #0]
    ADRP X11, L_float1@PAGE
    ADD X11, X11, L_float1@PAGEOFF
    LDR D0, [X11, #0]
    MOVZ X11, #1
    LSL X12, X11, #3
    ADD X13, X27, X12
    STR D0, [X13, #0]
    ADRP X12, L_float2@PAGE
    ADD X12, X12, L_float2@PAGEOFF
    LDR D0, [X12, #0]
    MOVZ X12, #2
    LSL X13, X12, #3
    ADD X14, X27, X13
    STR D0, [X14, #0]
    ADRP X13, L_float3@PAGE
    ADD X13, X13, L_float3@PAGEOFF
    LDR D0, [X13, #0]
    MOVZ X13, #3
    LSL X14, X13, #3
    ADD X15, X27, X14
    STR D0, [X15, #0]
    ADRP X14, L_float4@PAGE
    ADD X14, X14, L_float4@PAGEOFF
    LDR D0, [X14, #0]
    MOVZ X14, #4
    LSL X15, X14, #3
    ADD X9, X27, X15
    STR D0, [X9, #0]
    ADRP X10, L_float5@PAGE
    ADD X10, X10, L_float5@PAGEOFF
    LDR D0, [X10, #0]
    MOVZ X10, #5
    LSL X11, X10, #3
    ADD X12, X27, X11
    STR D0, [X12, #0]
    ADRP X11, L_float6@PAGE
    ADD X11, X11, L_float6@PAGEOFF
    LDR D0, [X11, #0]
    MOVZ X11, #6
    LSL X12, X11, #3
    ADD X13, X27, X12
    STR D0, [X13, #0]
    ADRP X12, L_float7@PAGE
    ADD X12, X12, L_float7@PAGEOFF
    LDR D0, [X12, #0]
    MOVZ X12, #7
    LSL X13, X12, #3
    ADD X14, X27, X13
    STR D0, [X14, #0]
    ADRP X13, L_float8@PAGE
    ADD X13, X13, L_float8@PAGEOFF
    LDR D0, [X13, #0]
    MOVZ X13, #0
    LSL X14, X13, #3
    ADD X15, X26, X14
    STR D0, [X15, #0]
    ADRP X14, L_float9@PAGE
    ADD X14, X14, L_float9@PAGEOFF
    LDR D0, [X14, #0]
    MOVZ X14, #1
    LSL X15, X14, #3
    ADD X9, X26, X15
    STR D0, [X9, #0]
    ADRP X10, L_float10@PAGE
    ADD X10, X10, L_float10@PAGEOFF
    LDR D0, [X10, #0]
    MOVZ X10, #2
    LSL X11, X10, #3
    ADD X12, X26, X11
    STR D0, [X12, #0]
    ADRP X11, L_float11@PAGE
    ADD X11, X11, L_float11@PAGEOFF
    LDR D0, [X11, #0]
    MOVZ X11, #3
    LSL X12, X11, #3
    ADD X13, X26, X12
    STR D0, [X13, #0]
    ADRP X12, L_float12@PAGE
    ADD X12, X12, L_float12@PAGEOFF
    LDR D0, [X12, #0]
    MOVZ X12, #4
    LSL X13, X12, #3
    ADD X14, X26, X13
    STR D0, [X14, #0]
    ADRP X13, L_float13@PAGE
    ADD X13, X13, L_float13@PAGEOFF
    LDR D0, [X13, #0]
    MOVZ X13, #5
    LSL X14, X13, #3
    ADD X15, X26, X14
    STR D0, [X15, #0]
    ADRP X14, L_float14@PAGE
    ADD X14, X14, L_float14@PAGEOFF
    LDR D0, [X14, #0]
    MOVZ X14, #6
    LSL X15, X14, #3
    ADD X9, X26, X15
    STR D0, [X9, #0]
    ADRP X10, L_float4@PAGE
    ADD X10, X10, L_float4@PAGEOFF
    LDR D0, [X10, #0]
    MOVZ X10, #7
    LSL X11, X10, #3
    ADD X12, X26, X11
    STR D0, [X12, #0]
    MOVZ X10, #0
    MOV X26, X10
    SUB X11, X27, #8
    LDR X10, [X11, #0] ; Load vector/table/string length
    MOVZ X11, #2
    SDIV X10, X10, X11
    MOV X20, X10
    MOV X0, X20
    LDR X11, [X19, #128]
    BLR X11
    MOV X26, X0
    LD1 {V0.4S}, [X27]
    LD1 {V1.4S}, [X26]
    FMINP V2.4S, V0.4S, V1.4S
    ST1 {V2.4S}, [X26]
    BL _HeapManager_exit_scope
    B L_START_Exit_1
L_START_Exit_1:
    B L_0
L_0:
    LDP x19, x25, [x29, #88]
    LDP x26, x27, [x29, #104]
    LDR X28, [X29, #120] ; Restored Reg: X28 @ FP+120
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [SP, #0]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L___veneer_:
    movz x16, #39536
    movk x16, #97, lsl #16
    movk x16, #1, lsl #32
    movk x16, #0, lsl #48
    blr x16

.section __DATA,__const
.p2align 3
L_str0:
    .quad 0x28
    ; (upper half)
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x4e
    .long 0x45
    .long 0x4f
    .long 0x4e
    .long 0x20
    .long 0x50
    .long 0x61
    .long 0x69
    .long 0x72
    .long 0x77
    .long 0x69
    .long 0x73
    .long 0x65
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
    .long 0x73
    .long 0xa
    .long 0x0
    .long 0x0
L_float0:
    .quad 0x3ff8000000000000
    ; (upper half)
L_float1:
    .quad 0x4004000000000000
    ; (upper half)
L_float2:
    .quad 0x4020000000000000
    ; (upper half)
L_float3:
    .quad 0x4008000000000000
    ; (upper half)
L_float4:
    .quad 0x400199999999999a
    ; (upper half)
L_float5:
    .quad 0x401ecccccccccccd
    ; (upper half)
L_float6:
    .quad 0x4022333333333333
    ; (upper half)
L_float7:
    .quad 0x3ff199999999999a
    ; (upper half)
L_float8:
    .quad 0x4012000000000000
    ; (upper half)
L_float9:
    .quad 0x3ff0000000000000
    ; (upper half)
L_float10:
    .quad 0x4018000000000000
    ; (upper half)
L_float11:
    .quad 0x4022000000000000
    ; (upper half)
L_float12:
    .quad 0x400a666666666666
    ; (upper half)
L_float13:
    .quad 0x401a666666666666
    ; (upper half)
L_float14:
    .quad 0x402199999999999a
    ; (upper half)

.section __DATA,__data
.p2align 3
    .long 0x0
L__data_segment_base:
