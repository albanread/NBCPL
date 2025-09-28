.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _START
.globl _HeapManager_exit_scope
.globl _WRITEN
.globl _WRITES
.globl _BCPL_LIST_APPEND_STRING
.globl _WRITEF
.globl _BCPL_LIST_GET_NTH
.globl _FILE_OPEN_READ
.globl _BCPL_CHECK_AND_DISPLAY_ERRORS
.globl _RDCH
.globl _BCPL_ALLOC_WORDS
.globl _FILE_READ
.globl _WRITEC
.globl _HEAPMANAGER_ISSAMMENABLED
.globl _SPLIT
.globl _BCPL_LIST_CREATE_EMPTY
.globl _GET_FREE_LIST_HEAD_ADDR
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
.globl _FPND
.globl _FEXP
.globl _WRITEF4
.globl _HEAPMANAGER_ENTER_SCOPE
.globl _FCOS
.globl _APND
.globl _WRITEF2
.globl _BCPL_CLEAR_ERRORS
.globl _FILTER
.globl _FLOG
.globl _GETVEC
.globl _LPND
.globl _BCPL_LIST_APPEND_INT
.globl _RETURNNODETOFREELIST
.globl _MALLOC
.globl _JOIN
.globl _BCPL_FREE_CELLS
.globl _STRCOPY
.globl _PACKSTRING
.globl _BCPL_FREE_LIST_SAFE
.globl _DEEPCOPYLIST
.globl _BCPL_FREE_LIST
.globl _FILE_CLOSE
.globl _FIND
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
.globl _SDL2_GET_CURRENT_VIDEO_DRIVER
.globl _SDL2_INIT_SUBSYSTEMS
.globl _SDL2_GET_VIDEO_DRIVERS
.globl _FINISH
.globl _SDL2_GET_EVENT_MOUSE
.globl _SDL2_SET_WINDOW_TITLE
.globl _WRITEF7
.globl _SDL2_DESTROY_WINDOW
.globl _HeapManager_enter_scope
.globl _FABS
.globl _DEEPCOPYLITERALLIST
.globl _FIX
.globl _WRITEF1
.globl _FILE_WRITE
.globl _COPYLIST
.globl _SDL2_DRAW_RECT
.globl _SDL2_GET_DISPLAY_MODES
.globl _BCPL_GET_LAST_ERROR
.globl _SDL2_PRESENT
.globl _SPND
.globl _SDL2_CREATE_RENDERER_EX
.globl _BCPL_LIST_GET_REST
.globl _SDL2_CLEAR
.globl _SDL2_DESTROY_RENDERER
.globl _SDL2_SET_DRAW_COLOR
.globl _SDL2_INIT
.globl _SDL2_FILL_RECT
.globl _FWRITE
.globl _CONCAT
.globl _SDL2_DRAW_LINE
.globl _SDL2_CREATE_RENDERER
.globl _SDL2_CLEAR_ERROR
.globl _SDL2_SET_WINDOW_SIZE
.globl _SETTYPE
.globl _FILE_SEEK
.globl _SDL2_GET_VERSION
.globl _SDL2_DRAW_POINT
.globl _HEAPMANAGER_EXIT_SCOPE
.globl _SDL2_GET_EVENT_BUTTON
.globl _WRITEF6
.globl _HEAPMANAGER_SETSAMMENABLED
.globl _FILE_OPEN_WRITE
.globl _SDL2_GET_TICKS
.globl ___SDL2_STATIC_BUILD
.globl _HEAPMANAGER_WAITFORSAMM
.globl _RND
.globl _STRLEN
.globl _SDL2_POLL_EVENT
.globl _SDL2_GET_ERROR
.p2align 2
_start:
_START:
    B L_START
    
; --- Veneer Section ---
    ; --- End Veneer Section ---

L_Point_CREATE:
    STP X29, X30, [SP, #-96]!
    MOV X29, SP
    STR X19, [X29, #40] ; Saved Reg: X19 @ FP+40
    STR X20, [X29, #48] ; Saved Reg: X20 @ FP+48
    STR X25, [X29, #56] ; Saved Reg: X25 @ FP+56
    STR X26, [X29, #64] ; Saved Reg: X26 @ FP+64
    STR X27, [X29, #72] ; Saved Reg: X27 @ FP+72
    STR X28, [X29, #80] ; Saved Reg: X28 @ FP+80
    MOV X27, X0
    MOV X26, X1
    MOV X25, X2
    MOV X27, X0    // Move parameter '_this' from X0 to X27
    MOV X26, X1    // Move parameter 'initialX' from X1 to X26
    MOV X25, X2    // Move parameter 'initialY' from X2 to X25
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
L_Point_CREATE_Entry_0:
    STR X26, [X27, #8] ; Store to member x
    STR X25, [X27, #16] ; Store to member y
    ADRP X9, L_str0@PAGE
    ADD X9, X9, L_str0@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    LDR X9, [X27, #8] ; x
    MOV X0, X9
    BL _WRITEN
    ADRP X9, L_str1@PAGE
    ADD X9, X9, L_str1@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    LDR X9, [X27, #16] ; y
    MOV X0, X9
    BL _WRITEN
    ADRP X9, L_str2@PAGE
    ADD X9, X9, L_str2@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    B L_Point_CREATE_Exit_1
L_Point_CREATE_Exit_1:
    B L_0
L_0:
    LDR X19, [X29, #40] ; Restored Reg: X19 @ FP+40
    LDR X20, [X29, #48] ; Restored Reg: X20 @ FP+48
    LDR X25, [X29, #56] ; Restored Reg: X25 @ FP+56
    LDR X26, [X29, #64] ; Restored Reg: X26 @ FP+64
    LDR X27, [X29, #72] ; Restored Reg: X27 @ FP+72
    LDR X28, [X29, #80] ; Restored Reg: X28 @ FP+80
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_Point_getX:
    LDR X0, [X0, #8] ; Optimized accessor load (int/ptr)
    RET
L_Point_getY:
    LDR X0, [X0, #16] ; Optimized accessor load (int/ptr)
    RET
L_Point_set:
    STP X29, X30, [SP, #-64]!
    MOV X29, SP
    STR X19, [X29, #40] ; Saved Reg: X19 @ FP+40
    STR X20, [X29, #48] ; Saved Reg: X20 @ FP+48
    STR X28, [X29, #56] ; Saved Reg: X28 @ FP+56
    MOV X15, X0
    MOV X14, X1
    MOV X13, X2
    MOV X15, X0    // Move parameter '_this' from X0 to X15
    MOV X14, X1    // Move parameter 'newX' from X1 to X14
    MOV X13, X2    // Move parameter 'newY' from X2 to X13
L_Point_set_Entry_0:
    STR X14, [X15, #8] ; Store to member x
    STR X13, [X15, #16] ; Store to member y
    B L_Point_set_Exit_1
L_Point_set_Exit_1:
    B L_3
L_3:
    LDR X19, [X29, #40] ; Restored Reg: X19 @ FP+40
    LDR X20, [X29, #48] ; Restored Reg: X20 @ FP+48
    LDR X28, [X29, #56] ; Restored Reg: X28 @ FP+56
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_Point_RELEASE:
    STP X29, X30, [SP, #-48]!
    MOV X29, SP
    STR X19, [X29, #24] ; Saved Reg: X19 @ FP+24
    STR X27, [X29, #32] ; Saved Reg: X27 @ FP+32
    STR X28, [X29, #40] ; Saved Reg: X28 @ FP+40
    MOV X27, X0
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
L_Point_RELEASE_Entry_0:
    ADRP X9, L_str3@PAGE
    ADD X9, X9, L_str3@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    B L_Point_RELEASE_Exit_1
L_Point_RELEASE_Exit_1:
    B L_4
L_4:
    LDR X19, [X29, #24] ; Restored Reg: X19 @ FP+24
    LDR X27, [X29, #32] ; Restored Reg: X27 @ FP+32
    LDR X28, [X29, #40] ; Restored Reg: X28 @ FP+40
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_ColorPoint_CREATE:
    STP X29, X30, [SP, #-96]!
    MOV X29, SP
    STR X19, [X29, #48] ; Saved Reg: X19 @ FP+48
    STR X20, [X29, #56] ; Saved Reg: X20 @ FP+56
    STR X21, [X29, #64] ; Saved Reg: X21 @ FP+64
    STR X27, [X29, #72] ; Saved Reg: X27 @ FP+72
    STR X28, [X29, #80] ; Saved Reg: X28 @ FP+80
    MOV X27, X0
    MOV X14, X1
    MOV X13, X2
    MOV X15, X3
    MOV X27, X0    // Move parameter '_this' from X0 to X27
    MOV X14, X1    // Move parameter 'initialX' from X1 to X14
    MOV X13, X2    // Move parameter 'initialY' from X2 to X13
    MOV X15, X3    // Move parameter 'initialColor' from X3 to X15
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
L_ColorPoint_CREATE_Entry_0:
    STR X15, [X27, #24] ; Store to member color
    MOV X20, X14
    MOV X21, X13
    MOV X0, X27
    MOV X1, X20
    MOV X2, X21
    BL L_Point_CREATE
    ADRP X9, L_str4@PAGE
    ADD X9, X9, L_str4@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    LDR X9, [X27, #8] ; x
    MOV X0, X9
    BL _WRITEN
    ADRP X9, L_str1@PAGE
    ADD X9, X9, L_str1@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    LDR X9, [X27, #16] ; y
    MOV X0, X9
    BL _WRITEN
    ADRP X9, L_str5@PAGE
    ADD X9, X9, L_str5@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    LDR X9, [X27, #24] ; color
    MOV X0, X9
    BL _WRITEN
    ADRP X9, L_str6@PAGE
    ADD X9, X9, L_str6@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    B L_ColorPoint_CREATE_Exit_1
L_ColorPoint_CREATE_Exit_1:
    B L_5
L_5:
    LDR X19, [X29, #48] ; Restored Reg: X19 @ FP+48
    LDR X20, [X29, #56] ; Restored Reg: X20 @ FP+56
    LDR X21, [X29, #64] ; Restored Reg: X21 @ FP+64
    LDR X27, [X29, #72] ; Restored Reg: X27 @ FP+72
    LDR X28, [X29, #80] ; Restored Reg: X28 @ FP+80
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_ColorPoint_setColor:
    STR X1, [X0, #24] ; Optimized setter store
    RET
L_ColorPoint_getColor:
    LDR X0, [X0, #24] ; Optimized accessor load (int/ptr)
    RET
L_makePoint:
    STP X29, X30, [SP, #-48]!
    MOV X29, SP
    STR X19, [X29, #24] ; Saved Reg: X19 @ FP+24
    STR X27, [X29, #32] ; Saved Reg: X27 @ FP+32
    STR X28, [X29, #40] ; Saved Reg: X28 @ FP+40
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
L_makePoint_Entry_0:
    BL _HeapManager_enter_scope
    MOVZ X0, #24
    BL _OBJECT_HEAP_ALLOC
    MOV X20, X0
    ADRP X9, L_Point_vtable@PAGE
    ADD X9, X9, L_Point_vtable@PAGEOFF
    STR X9, [X20, #0] ; store vtable ptr
    MOV X27, X20
    B L_makePoint_ResultisCleanup_1
L_makePoint_Exit_2:
    B L_8
L_makePoint_ResultisCleanup_1:
    MOV X0, X27
    B L_8
    B L_makePoint_Exit_2
L_8:
    LDR X19, [X29, #24] ; Restored Reg: X19 @ FP+24
    LDR X27, [X29, #32] ; Restored Reg: X27 @ FP+32
    LDR X28, [X29, #40] ; Restored Reg: X28 @ FP+40
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_START:
    STP X29, X30, [SP, #-64]!
    MOV X29, SP
    STR X19, [X29, #32] ; Saved Reg: X19 @ FP+32
    STR X27, [X29, #40] ; Saved Reg: X27 @ FP+40
    STR X28, [X29, #48] ; Saved Reg: X28 @ FP+48
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
L_START_Entry_0:
    BL _HeapManager_enter_scope
    ADRP X9, L_str7@PAGE
    ADD X9, X9, L_str7@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    ADRP X9, L_str8@PAGE
    ADD X9, X9, L_str8@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    ADRP X9, L_str9@PAGE
    ADD X9, X9, L_str9@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    BL L_makePoint
    MOV X27, X0
    ADRP X9, L_str10@PAGE
    ADD X9, X9, L_str10@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    LDR X9, [X27, #0] ; Load vtable pointer
    LDR X10, [X9, #16] ; Load method address
    MOV X0, X27
    BLR X10
    BL _WRITEN
    ADRP X9, L_str11@PAGE
    ADD X9, X9, L_str11@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    LDR X9, [X27, #0] ; Load vtable pointer
    LDR X10, [X9, #24] ; Load method address
    MOV X0, X27
    BLR X10
    BL _WRITEN
    ADRP X9, L_str6@PAGE
    ADD X9, X9, L_str6@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    ADRP X9, L_str12@PAGE
    ADD X9, X9, L_str12@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    MOVZ X9, #100
    MOV X20, X9
    MOVZ X9, #200
    MOV X21, X9
    MOV X0, X27
    MOV X1, X20
    MOV X2, X21
    BL L_Point_set
    ADRP X9, L_str13@PAGE
    ADD X9, X9, L_str13@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    LDR X9, [X27, #0] ; Load vtable pointer
    LDR X10, [X9, #16] ; Load method address
    MOV X0, X27
    BLR X10
    BL _WRITEN
    ADRP X9, L_str11@PAGE
    ADD X9, X9, L_str11@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    LDR X9, [X27, #0] ; Load vtable pointer
    LDR X10, [X9, #24] ; Load method address
    MOV X0, X27
    BLR X10
    BL _WRITEN
    ADRP X9, L_str14@PAGE
    ADD X9, X9, L_str14@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    ADRP X9, L_str15@PAGE
    ADD X9, X9, L_str15@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    MOVZ X0, #32
    BL _OBJECT_HEAP_ALLOC
    MOV X20, X0
    ADRP X9, L_ColorPoint_vtable@PAGE
    ADD X9, X9, L_ColorPoint_vtable@PAGEOFF
    STR X9, [X20, #0] ; store vtable ptr
    MOV X27, X20
    MOVZ X9, #130
    MOV X20, X9
    MOVZ X9, #140
    MOV X21, X9
    MOVZ X9, #255
    MOV X22, X9
    MOV X0, X27
    MOV X1, X20
    MOV X2, X21
    MOV X3, X22
    BL L_ColorPoint_CREATE
    ADRP X9, L_str10@PAGE
    ADD X9, X9, L_str10@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    LDR X9, [X27, #0] ; Load vtable pointer
    LDR X10, [X9, #16] ; Load method address
    MOV X0, X27
    BLR X10
    BL _WRITEN
    ADRP X9, L_str11@PAGE
    ADD X9, X9, L_str11@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    LDR X9, [X27, #0] ; Load vtable pointer
    LDR X10, [X9, #24] ; Load method address
    MOV X0, X27
    BLR X10
    BL _WRITEN
    ADRP X9, L_str16@PAGE
    ADD X9, X9, L_str16@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    LDR X9, [X27, #0] ; Load vtable pointer
    LDR X10, [X9, #48] ; Load method address
    MOV X0, X27
    BLR X10
    BL _WRITEN
    ADRP X9, L_str6@PAGE
    ADD X9, X9, L_str6@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    ADRP X9, L_str12@PAGE
    ADD X9, X9, L_str12@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    MOVZ X9, #60
    MOV X20, X9
    MOVZ X9, #80
    MOV X21, X9
    MOV X0, X27
    MOV X1, X20
    MOV X2, X21
    BL L_Point_set
    MOVZ X9, #123
    MOV X20, X9
    MOV X0, X27
    MOV X1, X20
    BL L_ColorPoint_setColor
    ADRP X9, L_str13@PAGE
    ADD X9, X9, L_str13@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    LDR X9, [X27, #0] ; Load vtable pointer
    LDR X10, [X9, #16] ; Load method address
    MOV X0, X27
    BLR X10
    BL _WRITEN
    ADRP X9, L_str11@PAGE
    ADD X9, X9, L_str11@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    LDR X9, [X27, #0] ; Load vtable pointer
    LDR X10, [X9, #24] ; Load method address
    MOV X0, X27
    BLR X10
    BL _WRITEN
    ADRP X9, L_str16@PAGE
    ADD X9, X9, L_str16@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    LDR X9, [X27, #0] ; Load vtable pointer
    LDR X10, [X9, #48] ; Load method address
    MOV X0, X27
    BLR X10
    BL _WRITEN
    ADRP X9, L_str6@PAGE
    ADD X9, X9, L_str6@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    ADRP X9, L_str17@PAGE
    ADD X9, X9, L_str17@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    BL _HeapManager_exit_scope
    B L_START_Exit_1
L_START_Exit_1:
    B L_9
L_9:
    LDR X19, [X29, #32] ; Restored Reg: X19 @ FP+32
    LDR X27, [X29, #40] ; Restored Reg: X27 @ FP+40
    LDR X28, [X29, #48] ; Restored Reg: X28 @ FP+48
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L___veneer_:
    movz x16, #40496
    movk x16, #85, lsl #16
    movk x16, #1, lsl #32
    movk x16, #0, lsl #48
    blr x16

.section __DATA,__const
.p2align 3
L_str0:
    .quad 0x12
    ; (upper half)
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
L_str1:
    .quad 0x2
    ; (upper half)
    .long 0x2c
    .long 0x20
    .long 0x0
    .long 0x0
L_str2:
    .quad 0x2
    ; (upper half)
    .long 0x29
    .long 0xa
    .long 0x0
    .long 0x0
L_str3:
    .quad 0x10
    ; (upper half)
    .long 0x50
    .long 0x6f
    .long 0x69
    .long 0x6e
    .long 0x74
    .long 0x20
    .long 0x64
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x72
    .long 0x6f
    .long 0x79
    .long 0x65
    .long 0x64
    .long 0xa
    .long 0x0
    .long 0x0
L_str4:
    .quad 0x17
    ; (upper half)
    .long 0x43
    .long 0x6f
    .long 0x6c
    .long 0x6f
    .long 0x72
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
L_str5:
    .quad 0xd
    ; (upper half)
    .long 0x29
    .long 0x20
    .long 0x77
    .long 0x69
    .long 0x74
    .long 0x68
    .long 0x20
    .long 0x63
    .long 0x6f
    .long 0x6c
    .long 0x6f
    .long 0x72
    .long 0x20
    .long 0x0
    .long 0x0
L_str6:
    .quad 0x1
    ; (upper half)
    .long 0xa
    .long 0x0
    .long 0x0
L_str7:
    .quad 0x3a
    ; (upper half)
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x63
    .long 0x6c
    .long 0x61
    .long 0x73
    .long 0x73
    .long 0x20
    .long 0x6d
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
    .long 0x77
    .long 0x69
    .long 0x74
    .long 0x68
    .long 0x20
    .long 0x63
    .long 0x6f
    .long 0x72
    .long 0x72
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x20
    .long 0x6f
    .long 0x66
    .long 0x66
    .long 0x73
    .long 0x65
    .long 0x74
    .long 0x73
    .long 0xa
    .long 0x0
    .long 0x0
L_str8:
    .quad 0x33
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
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0xa
    .long 0xa
    .long 0x0
    .long 0x0
L_str9:
    .quad 0x1b
    ; (upper half)
    .long 0x31
    .long 0x2e
    .long 0x20
    .long 0x42
    .long 0x61
    .long 0x73
    .long 0x69
    .long 0x63
    .long 0x20
    .long 0x50
    .long 0x6f
    .long 0x69
    .long 0x6e
    .long 0x74
    .long 0x20
    .long 0x63
    .long 0x6c
    .long 0x61
    .long 0x73
    .long 0x73
    .long 0x20
    .long 0x74
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x3a
    .long 0xa
    .long 0x0
    .long 0x0
L_str10:
    .quad 0x12
    ; (upper half)
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
L_str11:
    .quad 0x4
    ; (upper half)
    .long 0x2c
    .long 0x20
    .long 0x79
    .long 0x3d
    .long 0x0
    .long 0x0
L_str12:
    .quad 0x16
    ; (upper half)
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
L_str13:
    .quad 0x16
    ; (upper half)
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
L_str14:
    .quad 0x2
    ; (upper half)
    .long 0xa
    .long 0xa
    .long 0x0
    .long 0x0
L_str15:
    .quad 0x24
    ; (upper half)
    .long 0x32
    .long 0x2e
    .long 0x20
    .long 0x49
    .long 0x6e
    .long 0x68
    .long 0x65
    .long 0x72
    .long 0x69
    .long 0x74
    .long 0x65
    .long 0x64
    .long 0x20
    .long 0x43
    .long 0x6f
    .long 0x6c
    .long 0x6f
    .long 0x72
    .long 0x50
    .long 0x6f
    .long 0x69
    .long 0x6e
    .long 0x74
    .long 0x20
    .long 0x63
    .long 0x6c
    .long 0x61
    .long 0x73
    .long 0x73
    .long 0x20
    .long 0x74
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x3a
    .long 0xa
    .long 0x0
    .long 0x0
L_str16:
    .quad 0x8
    ; (upper half)
    .long 0x2c
    .long 0x20
    .long 0x63
    .long 0x6f
    .long 0x6c
    .long 0x6f
    .long 0x72
    .long 0x3d
    .long 0x0
    .long 0x0
L_str17:
    .quad 0x1d
    ; (upper half)
    .long 0xa
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
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
    .long 0xa
    .long 0x0
    .long 0x0
L_ColorPoint_vtable:
    .quad L_ColorPoint_CREATE
    .quad L_Point_RELEASE
    .quad L_Point_getX
    .quad L_Point_getY
    .quad L_Point_set
    .quad L_ColorPoint_setColor
    .quad L_ColorPoint_getColor
L_Point_vtable:
    .quad L_Point_CREATE
    .quad L_Point_RELEASE
    .quad L_Point_getX
    .quad L_Point_getY
    .quad L_Point_set

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
    .long 0x0
    .long 0x0
L__data_segment_base:
