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
.globl _HeapManager_enter_scope
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

L_Animal_Speak:
    STP X29, X30, [SP, #-48]!
    MOV X29, SP
    STP x19, x27, [x29, #24]
    STR X28, [X29, #40] ; Saved Reg: X28 @ FP+40
    MOV X27, X0    // Move parameter '_this' from X0 to X27
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
L_Animal_Speak_Entry_0:
    ADRP X9, L_str0_plus_8@PAGE
    ADD X9, X9, L_str0_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    B L_Animal_Speak_Exit_1
L_Animal_Speak_Exit_1:
    B L_0
L_0:
    LDP x19, x27, [x29, #24]
    LDR X28, [X29, #40] ; Restored Reg: X28 @ FP+40
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [SP, #0]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_Animal_CREATE:
    STP X29, X30, [SP, #-48]!
    MOV X29, SP
    STP x19, x27, [x29, #24]
    STR X28, [X29, #40] ; Saved Reg: X28 @ FP+40
    MOV X27, X0    // Move parameter '_this' from X0 to X27
L_Animal_CREATE_Entry_0:
    B L_Animal_CREATE_Exit_1
L_Animal_CREATE_Exit_1:
    B L_1
L_1:
    LDP x19, x27, [x29, #24]
    LDR X28, [X29, #40] ; Restored Reg: X28 @ FP+40
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [SP, #0]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_Animal_RELEASE:
    STP X29, X30, [SP, #-48]!
    MOV X29, SP
    STP x19, x27, [x29, #24]
    STR X28, [X29, #40] ; Saved Reg: X28 @ FP+40
    MOV X27, X0    // Move parameter '_this' from X0 to X27
L_Animal_RELEASE_Entry_0:
    B L_Animal_RELEASE_Exit_1
L_Animal_RELEASE_Exit_1:
    B L_2
L_2:
    LDP x19, x27, [x29, #24]
    LDR X28, [X29, #40] ; Restored Reg: X28 @ FP+40
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [SP, #0]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_Dog_Speak:
    STP X29, X30, [SP, #-48]!
    MOV X29, SP
    STP x19, x27, [x29, #24]
    STR X28, [X29, #40] ; Saved Reg: X28 @ FP+40
    MOV X27, X0    // Move parameter '_this' from X0 to X27
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
L_Dog_Speak_Entry_0:
    ADRP X9, L_str1_plus_8@PAGE
    ADD X9, X9, L_str1_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    B L_Dog_Speak_Exit_1
L_Dog_Speak_Exit_1:
    B L_3
L_3:
    LDP x19, x27, [x29, #24]
    LDR X28, [X29, #40] ; Restored Reg: X28 @ FP+40
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [SP, #0]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_START:
    STP X29, X30, [SP, #-112]!
    MOV X29, SP
    STP x19, x20, [x29, #48]
    STP x24, x25, [x29, #64]
    STP x26, x27, [x29, #80]
    STR X28, [X29, #96] ; Saved Reg: X28 @ FP+96
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
L_START_Entry_0:
    BL _HeapManager_enter_scope
    ADRP X9, L_str2_plus_8@PAGE
    ADD X9, X9, L_str2_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str3_plus_8@PAGE
    ADD X9, X9, L_str3_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    MOVZ X0, #8
    BL _OBJECT_HEAP_ALLOC
    MOV X20, X0
    ADRP X9, L_Animal_vtable@PAGE
    ADD X9, X9, L_Animal_vtable@PAGEOFF
    STR X9, [X20, #0] ; store vtable ptr
    LDR X10, [X20, #0] ; Read back stored vtable ptr
    // DEBUG VTABLE STORE for Animal
    MOV X0, X20
    MOV X1, X9
    MOV X2, X10
    BL _DEBUG_PRINT_VTABLE_STORE
    MOV X0, X20
    LDR X9, [X20, #0] ; Load vtable pointer for CREATE call
    LDR X10, [X9, #0] ; Load CREATE method address
    BLR X10
    // DEBUG FINAL OBJECT POINTER for Animal
    MOV X0, X20
    LDR X9, [X20, #0] ; Load final vtable ptr
    BL _DEBUG_PRINT_FINAL_OBJECT
    MOV X27, X20
    MOVZ X0, #8
    BL _OBJECT_HEAP_ALLOC
    MOV X20, X0
    ADRP X9, L_Dog_vtable@PAGE
    ADD X9, X9, L_Dog_vtable@PAGEOFF
    STR X9, [X20, #0] ; store vtable ptr
    LDR X10, [X20, #0] ; Read back stored vtable ptr
    // DEBUG VTABLE STORE for Dog
    MOV X0, X20
    MOV X1, X9
    MOV X2, X10
    BL _DEBUG_PRINT_VTABLE_STORE
    // DEBUG FINAL OBJECT POINTER for Dog
    MOV X0, X20
    LDR X9, [X20, #0] ; Load final vtable ptr
    BL _DEBUG_PRINT_FINAL_OBJECT
    MOV X26, X20
    ADRP X9, L_str4_plus_8@PAGE
    ADD X9, X9, L_str4_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str5_plus_8@PAGE
    ADD X9, X9, L_str5_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    MOV X0, X27
    LDR X9, [X0, #0] ; Load vtable pointer for method call
    LDR X10, [X9, #16] ; Load method address for Speak
    BLR X10
    ADRP X9, L_str6_plus_8@PAGE
    ADD X9, X9, L_str6_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    MOV X0, X26
    LDR X9, [X0, #0] ; Load vtable pointer for method call
    LDR X10, [X9, #16] ; Load method address for Speak
    BLR X10
    ADRP X9, L_str7_plus_8@PAGE
    ADD X9, X9, L_str7_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    MOV X25, X27
    MOV X24, X26
    ADRP X9, L_str8_plus_8@PAGE
    ADD X9, X9, L_str8_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str9_plus_8@PAGE
    ADD X9, X9, L_str9_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    MOV X0, X25
    LDR X9, [X0, #0] ; Load vtable pointer for method call
    LDR X10, [X9, #16] ; Load method address for Speak
    BLR X10
    ADRP X9, L_str10_plus_8@PAGE
    ADD X9, X9, L_str10_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    MOV X0, X24
    LDR X9, [X0, #0] ; Load vtable pointer for method call
    LDR X10, [X9, #16] ; Load method address for Speak
    BLR X10
    ADRP X9, L_str11_plus_8@PAGE
    ADD X9, X9, L_str11_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    MOV X25, X26
    MOV X24, X27
    ADRP X9, L_str12_plus_8@PAGE
    ADD X9, X9, L_str12_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str13_plus_8@PAGE
    ADD X9, X9, L_str13_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    MOV X0, X25
    LDR X9, [X0, #0] ; Load vtable pointer for method call
    LDR X10, [X9, #16] ; Load method address for Speak
    BLR X10
    ADRP X9, L_str14_plus_8@PAGE
    ADD X9, X9, L_str14_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    MOV X0, X24
    LDR X9, [X0, #0] ; Load vtable pointer for method call
    LDR X10, [X9, #16] ; Load method address for Speak
    BLR X10
    ADRP X9, L_str15_plus_8@PAGE
    ADD X9, X9, L_str15_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    B L_START_ResultisCleanup_1
L_START_Exit_2:
    B L_4
L_START_ResultisCleanup_1:
    MOVZ X9, #0
    MOV X0, X9
    B L_4
    B L_START_Exit_2
L_4:
    LDP x19, x20, [x29, #48]
    LDP x24, x25, [x29, #64]
    LDP x26, x27, [x29, #80]
    LDR X28, [X29, #96] ; Restored Reg: X28 @ FP+96
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [SP, #0]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L___veneer_:
    movz x16, #13520
    movk x16, #1252, lsl #16
    movk x16, #1, lsl #32
    movk x16, #0, lsl #48
    blr x16

.section __DATA,__const
.p2align 3
L_str0:
    .quad 0xe
    ; (upper half)
.p2align 2
L_str0_plus_8:
    .long 0x41
    .long 0x6e
    .long 0x69
    .long 0x6d
    .long 0x61
    .long 0x6c
    .long 0x20
    .long 0x73
    .long 0x70
    .long 0x65
    .long 0x61
    .long 0x6b
    .long 0x73
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str1:
    .quad 0xb
    ; (upper half)
.p2align 2
L_str1_plus_8:
    .long 0x44
    .long 0x6f
    .long 0x67
    .long 0x20
    .long 0x73
    .long 0x70
    .long 0x65
    .long 0x61
    .long 0x6b
    .long 0x73
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str2:
    .quad 0x22
    ; (upper half)
.p2align 2
L_str2_plus_8:
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x20
    .long 0x56
    .long 0x69
    .long 0x72
    .long 0x74
    .long 0x75
    .long 0x61
    .long 0x6c
    .long 0x20
    .long 0x4d
    .long 0x65
    .long 0x74
    .long 0x68
    .long 0x6f
    .long 0x64
    .long 0x20
    .long 0x44
    .long 0x65
    .long 0x62
    .long 0x75
    .long 0x67
    .long 0x20
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x20
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str3:
    .quad 0x17
    ; (upper half)
.p2align 2
L_str3_plus_8:
    .long 0x53
    .long 0x74
    .long 0x65
    .long 0x70
    .long 0x20
    .long 0x31
    .long 0x3a
    .long 0x20
    .long 0x43
    .long 0x72
    .long 0x65
    .long 0x61
    .long 0x74
    .long 0x65
    .long 0x20
    .long 0x6f
    .long 0x62
    .long 0x6a
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x73
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str4:
    .quad 0x1c
    ; (upper half)
.p2align 2
L_str4_plus_8:
    .long 0x53
    .long 0x74
    .long 0x65
    .long 0x70
    .long 0x20
    .long 0x32
    .long 0x3a
    .long 0x20
    .long 0x44
    .long 0x69
    .long 0x72
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x20
    .long 0x6d
    .long 0x65
    .long 0x74
    .long 0x68
    .long 0x6f
    .long 0x64
    .long 0x20
    .long 0x63
    .long 0x61
    .long 0x6c
    .long 0x6c
    .long 0x73
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str5:
    .quad 0x12
    ; (upper half)
.p2align 2
L_str5_plus_8:
    .long 0x61
    .long 0x6e
    .long 0x69
    .long 0x6d
    .long 0x61
    .long 0x6c
    .long 0x2e
    .long 0x53
    .long 0x70
    .long 0x65
    .long 0x61
    .long 0x6b
    .long 0x28
    .long 0x29
    .long 0x20
    .long 0x2d
    .long 0x3e
    .long 0x20
    .long 0x0
    .long 0x0
.p2align 3
L_str6:
    .quad 0xf
    ; (upper half)
.p2align 2
L_str6_plus_8:
    .long 0x64
    .long 0x6f
    .long 0x67
    .long 0x2e
    .long 0x53
    .long 0x70
    .long 0x65
    .long 0x61
    .long 0x6b
    .long 0x28
    .long 0x29
    .long 0x20
    .long 0x2d
    .long 0x3e
    .long 0x20
    .long 0x0
    .long 0x0
.p2align 3
L_str7:
    .quad 0x1c
    ; (upper half)
.p2align 2
L_str7_plus_8:
    .long 0x53
    .long 0x74
    .long 0x65
    .long 0x70
    .long 0x20
    .long 0x33
    .long 0x3a
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
    .long 0x61
    .long 0x73
    .long 0x73
    .long 0x69
    .long 0x67
    .long 0x6e
    .long 0x6d
    .long 0x65
    .long 0x6e
    .long 0x74
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str8:
    .quad 0x29
    ; (upper half)
.p2align 2
L_str8_plus_8:
    .long 0x53
    .long 0x74
    .long 0x65
    .long 0x70
    .long 0x20
    .long 0x34
    .long 0x3a
    .long 0x20
    .long 0x43
    .long 0x61
    .long 0x6c
    .long 0x6c
    .long 0x73
    .long 0x20
    .long 0x74
    .long 0x68
    .long 0x72
    .long 0x6f
    .long 0x75
    .long 0x67
    .long 0x68
    .long 0x20
    .long 0x61
    .long 0x73
    .long 0x73
    .long 0x69
    .long 0x67
    .long 0x6e
    .long 0x65
    .long 0x64
    .long 0x20
    .long 0x76
    .long 0x61
    .long 0x72
    .long 0x69
    .long 0x61
    .long 0x62
    .long 0x6c
    .long 0x65
    .long 0x73
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str9:
    .quad 0x1e
    ; (upper half)
.p2align 2
L_str9_plus_8:
    .long 0x76
    .long 0x61
    .long 0x72
    .long 0x31
    .long 0x20
    .long 0x28
    .long 0x66
    .long 0x72
    .long 0x6f
    .long 0x6d
    .long 0x20
    .long 0x61
    .long 0x6e
    .long 0x69
    .long 0x6d
    .long 0x61
    .long 0x6c
    .long 0x29
    .long 0x2e
    .long 0x53
    .long 0x70
    .long 0x65
    .long 0x61
    .long 0x6b
    .long 0x28
    .long 0x29
    .long 0x20
    .long 0x2d
    .long 0x3e
    .long 0x20
    .long 0x0
    .long 0x0
.p2align 3
L_str10:
    .quad 0x1b
    ; (upper half)
.p2align 2
L_str10_plus_8:
    .long 0x76
    .long 0x61
    .long 0x72
    .long 0x32
    .long 0x20
    .long 0x28
    .long 0x66
    .long 0x72
    .long 0x6f
    .long 0x6d
    .long 0x20
    .long 0x64
    .long 0x6f
    .long 0x67
    .long 0x29
    .long 0x2e
    .long 0x53
    .long 0x70
    .long 0x65
    .long 0x61
    .long 0x6b
    .long 0x28
    .long 0x29
    .long 0x20
    .long 0x2d
    .long 0x3e
    .long 0x20
    .long 0x0
    .long 0x0
.p2align 3
L_str11:
    .quad 0x19
    ; (upper half)
.p2align 2
L_str11_plus_8:
    .long 0x53
    .long 0x74
    .long 0x65
    .long 0x70
    .long 0x20
    .long 0x35
    .long 0x3a
    .long 0x20
    .long 0x43
    .long 0x72
    .long 0x6f
    .long 0x73
    .long 0x73
    .long 0x20
    .long 0x61
    .long 0x73
    .long 0x73
    .long 0x69
    .long 0x67
    .long 0x6e
    .long 0x6d
    .long 0x65
    .long 0x6e
    .long 0x74
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str12:
    .quad 0x21
    ; (upper half)
.p2align 2
L_str12_plus_8:
    .long 0x53
    .long 0x74
    .long 0x65
    .long 0x70
    .long 0x20
    .long 0x36
    .long 0x3a
    .long 0x20
    .long 0x43
    .long 0x61
    .long 0x6c
    .long 0x6c
    .long 0x73
    .long 0x20
    .long 0x61
    .long 0x66
    .long 0x74
    .long 0x65
    .long 0x72
    .long 0x20
    .long 0x72
    .long 0x65
    .long 0x61
    .long 0x73
    .long 0x73
    .long 0x69
    .long 0x67
    .long 0x6e
    .long 0x6d
    .long 0x65
    .long 0x6e
    .long 0x74
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str13:
    .quad 0x1a
    ; (upper half)
.p2align 2
L_str13_plus_8:
    .long 0x76
    .long 0x61
    .long 0x72
    .long 0x31
    .long 0x20
    .long 0x28
    .long 0x6e
    .long 0x6f
    .long 0x77
    .long 0x20
    .long 0x64
    .long 0x6f
    .long 0x67
    .long 0x29
    .long 0x2e
    .long 0x53
    .long 0x70
    .long 0x65
    .long 0x61
    .long 0x6b
    .long 0x28
    .long 0x29
    .long 0x20
    .long 0x2d
    .long 0x3e
    .long 0x20
    .long 0x0
    .long 0x0
.p2align 3
L_str14:
    .quad 0x1d
    ; (upper half)
.p2align 2
L_str14_plus_8:
    .long 0x76
    .long 0x61
    .long 0x72
    .long 0x32
    .long 0x20
    .long 0x28
    .long 0x6e
    .long 0x6f
    .long 0x77
    .long 0x20
    .long 0x61
    .long 0x6e
    .long 0x69
    .long 0x6d
    .long 0x61
    .long 0x6c
    .long 0x29
    .long 0x2e
    .long 0x53
    .long 0x70
    .long 0x65
    .long 0x61
    .long 0x6b
    .long 0x28
    .long 0x29
    .long 0x20
    .long 0x2d
    .long 0x3e
    .long 0x20
    .long 0x0
    .long 0x0
.p2align 3
L_str15:
    .quad 0x16
    ; (upper half)
.p2align 2
L_str15_plus_8:
    .long 0x3d
    .long 0x3d
    .long 0x3d
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
L_Dog_vtable:
    .quad L_Animal_CREATE
    .quad L_Animal_RELEASE
    .quad L_Dog_Speak
L_Animal_vtable:
    .quad L_Animal_CREATE
    .quad L_Animal_RELEASE
    .quad L_Animal_Speak

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
.p2align 2
L__data_segment_base:
