.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _START
.globl _HeapManager_exit_scope
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
.globl _HeapManager_enter_scope
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

L_TestClass_CREATE:
    STP X29, X30, [SP, #-48]!
    MOV X29, SP
    STP x19, x27, [x29, #24]
    STR X28, [X29, #40] ; Saved Reg: X28 @ FP+40
    MOV X27, X0    // Move parameter '_this' from X0 to X27
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
L_TestClass_CREATE_Entry_0:
    MOVZ X9, #100
    STR X9, [X27, #24] ; Store to member initialized_value
    MOVZ X9, #10
    STR X9, [X27, #8] ; Store to member x
    MOV X27, X9
    MOVZ X9, #20
    STR X9, [X27, #16] ; Store to member y
    MOV X27, X9
    SUB SP, SP, #16
    ADRP X9, L_str0_plus_8@PAGE
    ADD X9, X9, L_str0_plus_8@PAGEOFF
    STR X9, [SP, #0]
    LDR X0, [SP, #0]
    ADD SP, SP, #16
    BL _WRITES
    B L_TestClass_CREATE_Exit_1
L_TestClass_CREATE_Exit_1:
    B L_0
L_0:
    LDP x19, x27, [x29, #24]
    LDR X28, [X29, #40] ; Restored Reg: X28 @ FP+40
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [SP, #0]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_TestClass_RELEASE:
    STP X29, X30, [SP, #-48]!
    MOV X29, SP
    STP x19, x27, [x29, #24]
    STR X28, [X29, #40] ; Saved Reg: X28 @ FP+40
    MOV X27, X0    // Move parameter '_this' from X0 to X27
L_TestClass_RELEASE_Entry_0:
    B L_TestClass_RELEASE_Exit_1
L_TestClass_RELEASE_Exit_1:
    B L_1
L_1:
    LDP x19, x27, [x29, #24]
    LDR X28, [X29, #40] ; Restored Reg: X28 @ FP+40
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [SP, #0]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_START:
    STP X29, X30, [SP, #-128]!
    MOV X29, SP
    STP x19, x20, [x29, #48]
    STP x21, x22, [x29, #64]
    STP x24, x25, [x29, #80]
    STP x26, x27, [x29, #96]
    STR X28, [X29, #112] ; Saved Reg: X28 @ FP+112
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
L_START_Entry_0:
    BL _HeapManager_enter_scope
    SUB SP, SP, #16
    ADRP X9, L_str1_plus_8@PAGE
    ADD X9, X9, L_str1_plus_8@PAGEOFF
    STR X9, [SP, #0]
    LDR X0, [SP, #0]
    ADD SP, SP, #16
    BL _WRITES
    MOVZ X0, #32
    BL _OBJECT_HEAP_ALLOC
    MOV X20, X0
    ADRP X9, L_TestClass_vtable@PAGE
    ADD X9, X9, L_TestClass_vtable@PAGEOFF
    STR X9, [X20, #0] ; store vtable ptr
    MOV X0, X20
    LDR X9, [X20, #0] ; Load vtable pointer for CREATE call
    LDR X10, [X9, #0] ; Load CREATE method address
    BLR X10
    MOV X27, X20
    LDR X9, [X27, #8] ; Load member x
    MOV X25, X9
    LDR X9, [X27, #16] ; Load member y
    MOV X24, X9
    LDR X9, [X27, #24] ; Load member initialized_value
    MOV X26, X9
    SUB SP, SP, #16
    ADRP X9, L_str2_plus_8@PAGE
    ADD X9, X9, L_str2_plus_8@PAGEOFF
    STR X9, [SP, #0]
    LDR X0, [SP, #0]
    ADD SP, SP, #16
    BL _WRITES
    SUB SP, SP, #16
    STR X25, [SP, #0]
    LDR X0, [SP, #0]
    ADD SP, SP, #16
    BL _WRITEN
    SUB SP, SP, #16
    ADRP X9, L_str3_plus_8@PAGE
    ADD X9, X9, L_str3_plus_8@PAGEOFF
    STR X9, [SP, #0]
    LDR X0, [SP, #0]
    ADD SP, SP, #16
    BL _WRITES
    SUB SP, SP, #16
    STR X24, [SP, #0]
    LDR X0, [SP, #0]
    ADD SP, SP, #16
    BL _WRITEN
    SUB SP, SP, #16
    ADRP X9, L_str4_plus_8@PAGE
    ADD X9, X9, L_str4_plus_8@PAGEOFF
    STR X9, [SP, #0]
    LDR X0, [SP, #0]
    ADD SP, SP, #16
    BL _WRITES
    SUB SP, SP, #16
    STR X26, [SP, #0]
    LDR X0, [SP, #0]
    ADD SP, SP, #16
    BL _WRITEN
    SUB SP, SP, #16
    MOVZ W9, #10
    STR X9, [SP, #0]
    LDR X0, [SP, #0]
    ADD SP, SP, #16
    BL _WRITEC
    MOVZ X9, #50
    STR X9, [X27, #8] ; Store to member x
    MOVZ X9, #75
    STR X9, [X27, #16] ; Store to member y
    SUB SP, SP, #16
    ADRP X9, L_str5_plus_8@PAGE
    ADD X9, X9, L_str5_plus_8@PAGEOFF
    STR X9, [SP, #0]
    LDR X0, [SP, #0]
    ADD SP, SP, #16
    BL _WRITES
    SUB SP, SP, #16
    LDR X9, [X27, #8] ; Load member x
    STR X9, [SP, #0]
    LDR X0, [SP, #0]
    ADD SP, SP, #16
    BL _WRITEN
    SUB SP, SP, #16
    ADRP X9, L_str3_plus_8@PAGE
    ADD X9, X9, L_str3_plus_8@PAGEOFF
    STR X9, [SP, #0]
    LDR X0, [SP, #0]
    ADD SP, SP, #16
    BL _WRITES
    SUB SP, SP, #16
    LDR X9, [X27, #16] ; Load member y
    STR X9, [SP, #0]
    LDR X0, [SP, #0]
    ADD SP, SP, #16
    BL _WRITEN
    SUB SP, SP, #16
    MOVZ W9, #10
    STR X9, [SP, #0]
    LDR X0, [SP, #0]
    ADD SP, SP, #16
    BL _WRITEC
    LDR X9, [X27, #8] ; Load member x
    CMP X9, #50
    CSET X10, EQ
    CMP X10, #0
    B.EQ L_3
    LDR X9, [X27, #16] ; Load member y
    CMP X9, #75
    CSET X11, EQ
    B L_4
L_3:
    MOVZ X11, #0
L_4:
    CMP X11, #0
    B.EQ L_5
    LDR X9, [X27, #24] ; Load member initialized_value
    CMP X9, #100
    CSET X12, EQ
    B L_6
L_5:
    MOVZ X12, #0
L_6:
    CMP X12, XZR
    B.EQ L_START_Else_2
    B L_START_Then_1
L_START_Else_2:
    BL _HeapManager_enter_scope
    SUB SP, SP, #16
    ADRP X9, L_str6_plus_8@PAGE
    ADD X9, X9, L_str6_plus_8@PAGEOFF
    STR X9, [SP, #0]
    LDR X0, [SP, #0]
    ADD SP, SP, #16
    BL _WRITES
    BL _HeapManager_exit_scope
    B L_START_Join_3
L_START_Exit_4:
    B L_2
L_START_Join_3:
    SUB SP, SP, #16
    ADRP X9, L_str7_plus_8@PAGE
    ADD X9, X9, L_str7_plus_8@PAGEOFF
    STR X9, [SP, #0]
    LDR X0, [SP, #0]
    ADD SP, SP, #16
    BL _WRITES
    BL _HeapManager_exit_scope
    B L_START_Exit_4
L_START_Then_1:
    BL _HeapManager_enter_scope
    SUB SP, SP, #16
    ADRP X9, L_str8_plus_8@PAGE
    ADD X9, X9, L_str8_plus_8@PAGEOFF
    STR X9, [SP, #0]
    LDR X0, [SP, #0]
    ADD SP, SP, #16
    BL _WRITES
    BL _HeapManager_exit_scope
    B L_START_Join_3
L_2:
    LDP x19, x20, [x29, #48]
    LDP x21, x22, [x29, #64]
    LDP x24, x25, [x29, #80]
    LDP x26, x27, [x29, #96]
    LDR X28, [X29, #112] ; Restored Reg: X28 @ FP+112
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [SP, #0]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L___veneer_:
    movz x16, #61732
    movk x16, #817, lsl #16
    movk x16, #1, lsl #32
    movk x16, #0, lsl #48
    blr x16

.section __DATA,__const
.p2align 3
L_str0:
    .quad 0x39
    ; (upper half)
.p2align 2
L_str0_plus_8:
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x43
    .long 0x6c
    .long 0x61
    .long 0x73
    .long 0x73
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
    .long 0x78
    .long 0x3d
    .long 0x31
    .long 0x30
    .long 0x2c
    .long 0x20
    .long 0x79
    .long 0x3d
    .long 0x32
    .long 0x30
    .long 0x2c
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
    .long 0x5f
    .long 0x76
    .long 0x61
    .long 0x6c
    .long 0x75
    .long 0x65
    .long 0x3d
    .long 0x31
    .long 0x30
    .long 0x30
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str1:
    .quad 0x27
    ; (upper half)
.p2align 2
L_str1_plus_8:
    .long 0x3d
    .long 0x3d
    .long 0x3d
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
    .long 0x65
    .long 0x6d
    .long 0x62
    .long 0x65
    .long 0x72
    .long 0x20
    .long 0x56
    .long 0x61
    .long 0x72
    .long 0x69
    .long 0x61
    .long 0x62
    .long 0x6c
    .long 0x65
    .long 0x20
    .long 0x41
    .long 0x63
    .long 0x63
    .long 0x65
    .long 0x73
    .long 0x73
    .long 0x20
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str2:
    .quad 0xf
    ; (upper half)
.p2align 2
L_str2_plus_8:
    .long 0x52
    .long 0x65
    .long 0x61
    .long 0x64
    .long 0x20
    .long 0x76
    .long 0x61
    .long 0x6c
    .long 0x75
    .long 0x65
    .long 0x73
    .long 0x3a
    .long 0x20
    .long 0x78
    .long 0x3d
    .long 0x0
    .long 0x0
.p2align 3
L_str3:
    .quad 0x4
    ; (upper half)
.p2align 2
L_str3_plus_8:
    .long 0x2c
    .long 0x20
    .long 0x79
    .long 0x3d
    .long 0x0
    .long 0x0
.p2align 3
L_str4:
    .quad 0x14
    ; (upper half)
.p2align 2
L_str4_plus_8:
    .long 0x2c
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
    .long 0x5f
    .long 0x76
    .long 0x61
    .long 0x6c
    .long 0x75
    .long 0x65
    .long 0x3d
    .long 0x0
    .long 0x0
.p2align 3
L_str5:
    .quad 0x16
    ; (upper half)
.p2align 2
L_str5_plus_8:
    .long 0x41
    .long 0x66
    .long 0x74
    .long 0x65
    .long 0x72
    .long 0x20
    .long 0x6d
    .long 0x6f
    .long 0x64
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
    .long 0x20
    .long 0x78
    .long 0x3d
    .long 0x0
    .long 0x0
.p2align 3
L_str6:
    .quad 0x26
    ; (upper half)
.p2align 2
L_str6_plus_8:
    .long 0x2717
    .long 0x20
    .long 0x46
    .long 0x41
    .long 0x49
    .long 0x4c
    .long 0x3a
    .long 0x20
    .long 0x4d
    .long 0x65
    .long 0x6d
    .long 0x62
    .long 0x65
    .long 0x72
    .long 0x20
    .long 0x76
    .long 0x61
    .long 0x72
    .long 0x69
    .long 0x61
    .long 0x62
    .long 0x6c
    .long 0x65
    .long 0x20
    .long 0x61
    .long 0x63
    .long 0x63
    .long 0x65
    .long 0x73
    .long 0x73
    .long 0x20
    .long 0x66
    .long 0x61
    .long 0x69
    .long 0x6c
    .long 0x65
    .long 0x64
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str7:
    .quad 0x2d
    ; (upper half)
.p2align 2
L_str7_plus_8:
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x20
    .long 0x4d
    .long 0x65
    .long 0x6d
    .long 0x62
    .long 0x65
    .long 0x72
    .long 0x20
    .long 0x56
    .long 0x61
    .long 0x72
    .long 0x69
    .long 0x61
    .long 0x62
    .long 0x6c
    .long 0x65
    .long 0x20
    .long 0x41
    .long 0x63
    .long 0x63
    .long 0x65
    .long 0x73
    .long 0x73
    .long 0x20
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x20
    .long 0x43
    .long 0x6f
    .long 0x6d
    .long 0x70
    .long 0x6c
    .long 0x65
    .long 0x74
    .long 0x65
    .long 0x20
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str8:
    .quad 0x3b
    ; (upper half)
.p2align 2
L_str8_plus_8:
    .long 0x2713
    .long 0x20
    .long 0x50
    .long 0x41
    .long 0x53
    .long 0x53
    .long 0x3a
    .long 0x20
    .long 0x4d
    .long 0x65
    .long 0x6d
    .long 0x62
    .long 0x65
    .long 0x72
    .long 0x20
    .long 0x76
    .long 0x61
    .long 0x72
    .long 0x69
    .long 0x61
    .long 0x62
    .long 0x6c
    .long 0x65
    .long 0x20
    .long 0x61
    .long 0x63
    .long 0x63
    .long 0x65
    .long 0x73
    .long 0x73
    .long 0x20
    .long 0x61
    .long 0x6e
    .long 0x64
    .long 0x20
    .long 0x6d
    .long 0x6f
    .long 0x64
    .long 0x69
    .long 0x66
    .long 0x69
    .long 0x63
    .long 0x61
    .long 0x74
    .long 0x69
    .long 0x6f
    .long 0x6e
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
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_TestClass_vtable:
    .quad L_TestClass_CREATE
    .quad L_TestClass_RELEASE

.section __DATA,__data
.p2align 3
    .long 0x0
    .long 0x0
    .long 0x0
    .long 0x0
.p2align 2
L__data_segment_base:
