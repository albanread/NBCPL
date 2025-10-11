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

L_Point_CREATE:
    STP X29, X30, [SP, #-128]!
    MOV X29, SP
    STP x19, x20, [x29, #40]
    STP x21, x22, [x29, #56]
    STP x23, x24, [x29, #72]
    STP x25, x26, [x29, #88]
    STP x27, x28, [x29, #104]
    MOV X27, X0    // Move parameter '_this' from X0 to X27
    MOV X26, X1    // Move parameter 'initialX' from X1 to X26
    MOV X25, X2    // Move parameter 'initialY' from X2 to X25
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
L_Point_CREATE_Entry_0:
    STR X26, [X27, #8] ; Store to member x
    MOV X24, X26
    STR X25, [X27, #16] ; Store to member y
    MOV X23, X25
    ADRP X9, L_str0_plus_8@PAGE
    ADD X9, X9, L_str0_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITES
    LDR X9, [X27, #8] ; x
    MOV X0, X9
    BL _WRITEN
    ADRP X9, L_str1_plus_8@PAGE
    ADD X9, X9, L_str1_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITES
    LDR X9, [X27, #16] ; y
    MOV X0, X9
    BL _WRITEN
    ADRP X9, L_str2_plus_8@PAGE
    ADD X9, X9, L_str2_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITES
    B L_Point_CREATE_Exit_1
L_Point_CREATE_Exit_1:
    B L_0
L_0:
    LDP x19, x20, [x29, #40]
    LDP x21, x22, [x29, #56]
    LDP x23, x24, [x29, #72]
    LDP x25, x26, [x29, #88]
    LDP x27, x28, [x29, #104]
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [SP, #0]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_Point_getX:
    STP X29, X30, [SP, #-64]!
    MOV X29, SP
    STP x19, x26, [x29, #24]
    STP x27, x28, [x29, #40]
    MOV X27, X0    // Move parameter '_this' from X0 to X27
L_Point_getX_Entry_0:
    LDR X9, [X27, #8] ; x
    MOV X0, X9
    B L_1
    B L_Point_getX_Exit_1
L_Point_getX_Exit_1:
    B L_1
L_1:
    LDP x19, x26, [x29, #24]
    LDP x27, x28, [x29, #40]
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [SP, #0]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_Point_getY:
    STP X29, X30, [SP, #-64]!
    MOV X29, SP
    STP x19, x26, [x29, #24]
    STP x27, x28, [x29, #40]
    MOV X27, X0    // Move parameter '_this' from X0 to X27
L_Point_getY_Entry_0:
    LDR X9, [X27, #16] ; y
    MOV X0, X9
    B L_2
    B L_Point_getY_Exit_1
L_Point_getY_Exit_1:
    B L_2
L_2:
    LDP x19, x26, [x29, #24]
    LDP x27, x28, [x29, #40]
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [SP, #0]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_Point_set:
    STP X29, X30, [SP, #-128]!
    MOV X29, SP
    STP x19, x20, [x29, #40]
    STP x21, x22, [x29, #56]
    STP x23, x24, [x29, #72]
    STP x25, x26, [x29, #88]
    STP x27, x28, [x29, #104]
    MOV X27, X0    // Move parameter '_this' from X0 to X27
    MOV X26, X1    // Move parameter 'newX' from X1 to X26
    MOV X25, X2    // Move parameter 'newY' from X2 to X25
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
L_Point_set_Entry_0:
    STR X26, [X27, #8] ; Store to member x
    MOV X24, X26
    STR X25, [X27, #16] ; Store to member y
    MOV X23, X25
    ADRP X9, L_str3_plus_8@PAGE
    ADD X9, X9, L_str3_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITES
    LDR X9, [X27, #8] ; x
    MOV X0, X9
    BL _WRITEN
    ADRP X9, L_str1_plus_8@PAGE
    ADD X9, X9, L_str1_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITES
    LDR X9, [X27, #16] ; y
    MOV X0, X9
    BL _WRITEN
    ADRP X9, L_str2_plus_8@PAGE
    ADD X9, X9, L_str2_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITES
    B L_Point_set_Exit_1
L_Point_set_Exit_1:
    B L_3
L_3:
    LDP x19, x20, [x29, #40]
    LDP x21, x22, [x29, #56]
    LDP x23, x24, [x29, #72]
    LDP x25, x26, [x29, #88]
    LDP x27, x28, [x29, #104]
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [SP, #0]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_Point_RELEASE:
    STP X29, X30, [SP, #-48]!
    MOV X29, SP
    STP x19, x27, [x29, #24]
    STR X28, [X29, #40] ; Saved Reg: X28 @ FP+40
    MOV X27, X0    // Move parameter '_this' from X0 to X27
L_Point_RELEASE_Entry_0:
    B L_Point_RELEASE_Exit_1
L_Point_RELEASE_Exit_1:
    B L_4
L_4:
    LDP x19, x27, [x29, #24]
    LDR X28, [X29, #40] ; Restored Reg: X28 @ FP+40
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [SP, #0]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_START:
    STP X29, X30, [SP, #-48]!
    MOV X29, SP
    STP x19, x27, [x29, #24]
    STR X28, [X29, #40] ; Saved Reg: X28 @ FP+40
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
L_START_Entry_0:
    BL _HeapManager_enter_scope
    ADRP X9, L_str4_plus_8@PAGE
    ADD X9, X9, L_str4_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITES
    MOVZ X0, #24
    BL _OBJECT_HEAP_ALLOC
    MOV X20, X0
    ADRP X9, L_Point_vtable@PAGE
    ADD X9, X9, L_Point_vtable@PAGEOFF
    STR X9, [X20, #0] ; store vtable ptr
    MOV X0, X20
    MOVZ X9, #50
    MOV X1, X9
    MOVZ X9, #75
    MOV X2, X9
    LDR X9, [X20, #0] ; Load vtable pointer for CREATE call
    LDR X10, [X9, #0] ; Load CREATE method address
    BLR X10
    MOV X27, X20
    ADRP X9, L_str5_plus_8@PAGE
    ADD X9, X9, L_str5_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITES
    LDR X9, [X27, #0] ; Load vtable pointer
    LDR X10, [X9, #16] ; Load method address
    MOV X0, X27
    BLR X10
    BL _WRITEN
    ADRP X9, L_str6_plus_8@PAGE
    ADD X9, X9, L_str6_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITES
    LDR X9, [X27, #0] ; Load vtable pointer
    LDR X10, [X9, #24] ; Load method address
    MOV X0, X27
    BLR X10
    BL _WRITEN
    ADRP X9, L_str7_plus_8@PAGE
    ADD X9, X9, L_str7_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITES
    ADRP X9, L_str8_plus_8@PAGE
    ADD X9, X9, L_str8_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITES
    MOVZ x20, #100
    MOVZ x20, #200
    MOV X0, X27
    MOV X1, X20
    MOV X2, X20
    BL L_Point_set
    ADRP X9, L_str9_plus_8@PAGE
    ADD X9, X9, L_str9_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITES
    LDR X9, [X27, #0] ; Load vtable pointer
    LDR X10, [X9, #16] ; Load method address
    MOV X0, X27
    BLR X10
    BL _WRITEN
    ADRP X9, L_str6_plus_8@PAGE
    ADD X9, X9, L_str6_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITES
    LDR X9, [X27, #0] ; Load vtable pointer
    LDR X10, [X9, #24] ; Load method address
    MOV X0, X27
    BLR X10
    BL _WRITEN
    ADRP X9, L_str10_plus_8@PAGE
    ADD X9, X9, L_str10_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITES
    BL _HeapManager_exit_scope
    B L_START_Exit_1
L_START_Exit_1:
    B L_5
L_5:
    LDP x19, x27, [x29, #24]
    LDR X28, [X29, #40] ; Restored Reg: X28 @ FP+40
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [SP, #0]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L___veneer_:
    movz x16, #60612
    movk x16, #304, lsl #16
    movk x16, #1, lsl #32
    movk x16, #0, lsl #48
    blr x16

.section __DATA,__const
.p2align 3
L_str0:
    .quad 0x12
    ; (upper half)
.p2align 2
L_str0_plus_8:
    .long 0x50
    .long 0x6f
    .long 0x69
    .long 0x6e
    .long 0x74
    .long 0x20
    .long 0x63
    .long 0x72
    .long 0x65
    .long 0x61
    .long 0x74
    .long 0x65
    .long 0x64
    .long 0x20
    .long 0x61
    .long 0x74
    .long 0x20
    .long 0x28
    .long 0x0
    .long 0x0
.p2align 3
L_str1:
    .quad 0x2
    ; (upper half)
.p2align 2
L_str1_plus_8:
    .long 0x2c
    .long 0x20
    .long 0x0
    .long 0x0
.p2align 3
L_str2:
    .quad 0x2
    ; (upper half)
.p2align 2
L_str2_plus_8:
    .long 0x29
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str3:
    .quad 0x10
    ; (upper half)
.p2align 2
L_str3_plus_8:
    .long 0x50
    .long 0x6f
    .long 0x69
    .long 0x6e
    .long 0x74
    .long 0x20
    .long 0x6d
    .long 0x6f
    .long 0x76
    .long 0x65
    .long 0x64
    .long 0x20
    .long 0x74
    .long 0x6f
    .long 0x20
    .long 0x28
    .long 0x0
    .long 0x0
.p2align 3
L_str4:
    .quad 0x12
    ; (upper half)
.p2align 2
L_str4_plus_8:
    .long 0x43
    .long 0x72
    .long 0x65
    .long 0x61
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x70
    .long 0x6f
    .long 0x69
    .long 0x6e
    .long 0x74
    .long 0x2e
    .long 0x2e
    .long 0x2e
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str5:
    .quad 0x12
    ; (upper half)
.p2align 2
L_str5_plus_8:
    .long 0x52
    .long 0x65
    .long 0x61
    .long 0x64
    .long 0x69
    .long 0x6e
    .long 0x67
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
L_str6:
    .quad 0x4
    ; (upper half)
.p2align 2
L_str6_plus_8:
    .long 0x2c
    .long 0x20
    .long 0x79
    .long 0x3d
    .long 0x0
    .long 0x0
.p2align 3
L_str7:
    .quad 0x2
    ; (upper half)
.p2align 2
L_str7_plus_8:
    .long 0xa
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str8:
    .quad 0x16
    ; (upper half)
.p2align 2
L_str8_plus_8:
    .long 0x53
    .long 0x65
    .long 0x74
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x6e
    .long 0x65
    .long 0x77
    .long 0x20
    .long 0x76
    .long 0x61
    .long 0x6c
    .long 0x75
    .long 0x65
    .long 0x73
    .long 0x2e
    .long 0x2e
    .long 0x2e
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str9:
    .quad 0x16
    ; (upper half)
.p2align 2
L_str9_plus_8:
    .long 0x52
    .long 0x65
    .long 0x61
    .long 0x64
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x6e
    .long 0x65
    .long 0x77
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
L_str10:
    .quad 0x1
    ; (upper half)
.p2align 2
L_str10_plus_8:
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_Point_vtable:
    .quad L_Point_CREATE
    .quad L_Point_RELEASE
    .quad L_Point_getX
    .quad L_Point_getY
    .quad L_Point_set

.section __DATA,__data
.p2align 3
    .long 0x0
.p2align 2
L__data_segment_base:
