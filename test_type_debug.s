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
    STP X29, X30, [SP, #-160]!
    MOV X29, SP
    STP x19, x20, [x29, #80]
    STP x21, x22, [x29, #96]
    STP x23, x24, [x29, #112]
    STP x25, x26, [x29, #128]
    STP x27, x28, [x29, #144]
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
    ADRP X9, L_pair0@PAGE
    ADD X9, X9, L_pair0@PAGEOFF
    LDR X9, [X9, #0]
    MOV X26, X9
    ADRP X9, L_str2_plus_8@PAGE
    ADD X9, X9, L_str2_plus_8@PAGEOFF
    MOV X0, X9
    MOV X1, X26
    BL _WRITEF1
    ADRP X9, L_str3_plus_8@PAGE
    ADD X9, X9, L_str3_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    SUB SP, SP, #16
    ADRP X9, L_str4_plus_8@PAGE
    ADD X9, X9, L_str4_plus_8@PAGEOFF
    STR X9, [SP, #0]
    LDR X0, [SP, #0]
    ADD SP, SP, #16
    BL _WRITES
    ADRP X9, L_str5_plus_8@PAGE
    ADD X9, X9, L_str5_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str6_plus_8@PAGE
    ADD X9, X9, L_str6_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_pair1@PAGE
    ADD X9, X9, L_pair1@PAGEOFF
    LDR X9, [X9, #0]
    MOV X25, X9
    ADRP X9, L_str7_plus_8@PAGE
    ADD X9, X9, L_str7_plus_8@PAGEOFF
    MOV X0, X9
    MOV X1, X25
    BL _WRITEF1
    ADRP X9, L_str8_plus_8@PAGE
    ADD X9, X9, L_str8_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    SUB SP, SP, #16
    ADRP X9, L_str4_plus_8@PAGE
    ADD X9, X9, L_str4_plus_8@PAGEOFF
    STR X9, [SP, #0]
    LDR X0, [SP, #0]
    ADD SP, SP, #16
    BL _WRITES
    ADRP X9, L_str5_plus_8@PAGE
    ADD X9, X9, L_str5_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str9_plus_8@PAGE
    ADD X9, X9, L_str9_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    fmov D0, X26
    fmov D1, X25
    mul v0.2s, v0.2s, v1.2s    ; new dedicated 2s encoder
    fmov X27, D0
    MOV X23, X27
    ADRP X9, L_str10_plus_8@PAGE
    ADD X9, X9, L_str10_plus_8@PAGEOFF
    MOV X0, X9
    MOV X1, X23
    BL _WRITEF1
    ADRP X9, L_str11_plus_8@PAGE
    ADD X9, X9, L_str11_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    SUB SP, SP, #16
    ADRP X9, L_str4_plus_8@PAGE
    ADD X9, X9, L_str4_plus_8@PAGEOFF
    STR X9, [SP, #0]
    LDR X0, [SP, #0]
    ADD SP, SP, #16
    BL _WRITES
    ADRP X9, L_str5_plus_8@PAGE
    ADD X9, X9, L_str5_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str12_plus_8@PAGE
    ADD X9, X9, L_str12_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_pair0@PAGE
    ADD X9, X9, L_pair0@PAGEOFF
    LDR X9, [X9, #0]
    MOV X21, X9
    ADRP X9, L_str13_plus_8@PAGE
    ADD X9, X9, L_str13_plus_8@PAGEOFF
    MOV X0, X9
    MOV X1, X21
    BL _WRITEF1
    ADRP X9, L_str14_plus_8@PAGE
    ADD X9, X9, L_str14_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    SUB SP, SP, #16
    ADRP X9, L_str4_plus_8@PAGE
    ADD X9, X9, L_str4_plus_8@PAGEOFF
    STR X9, [SP, #0]
    LDR X0, [SP, #0]
    ADD SP, SP, #16
    BL _WRITES
    ADRP X9, L_str5_plus_8@PAGE
    ADD X9, X9, L_str5_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str15_plus_8@PAGE
    ADD X9, X9, L_str15_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_pair0@PAGE
    ADD X9, X9, L_pair0@PAGEOFF
    LDR X9, [X9, #0]
    ADRP X10, L_pair1@PAGE
    ADD X10, X10, L_pair1@PAGEOFF
    LDR X10, [X10, #0]
    MUL X9, X9, X10
    MOV X24, X9
    ADRP X9, L_str16_plus_8@PAGE
    ADD X9, X9, L_str16_plus_8@PAGEOFF
    MOV X0, X9
    MOV X1, X24
    BL _WRITEF1
    ADRP X9, L_str17_plus_8@PAGE
    ADD X9, X9, L_str17_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    SUB SP, SP, #16
    ADRP X9, L_str4_plus_8@PAGE
    ADD X9, X9, L_str4_plus_8@PAGEOFF
    STR X9, [SP, #0]
    LDR X0, [SP, #0]
    ADD SP, SP, #16
    BL _WRITES
    ADRP X9, L_str5_plus_8@PAGE
    ADD X9, X9, L_str5_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str18_plus_8@PAGE
    ADD X9, X9, L_str18_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str19_plus_8@PAGE
    ADD X9, X9, L_str19_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_pair0@PAGE
    ADD X9, X9, L_pair0@PAGEOFF
    LDR X9, [X9, #0]
    MOV X27, X9
    ADRP X9, L_str20_plus_8@PAGE
    ADD X9, X9, L_str20_plus_8@PAGEOFF
    MOV X0, X9
    MOV X1, X27
    BL _WRITEF1
    ADRP X9, L_str21_plus_8@PAGE
    ADD X9, X9, L_str21_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    SUB SP, SP, #16
    ADRP X9, L_str4_plus_8@PAGE
    ADD X9, X9, L_str4_plus_8@PAGEOFF
    STR X9, [SP, #0]
    LDR X0, [SP, #0]
    ADD SP, SP, #16
    BL _WRITES
    ADRP X9, L_str5_plus_8@PAGE
    ADD X9, X9, L_str5_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str22_plus_8@PAGE
    ADD X9, X9, L_str22_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_pair1@PAGE
    ADD X9, X9, L_pair1@PAGEOFF
    LDR X9, [X9, #0]
    MOV X22, X9
    ADRP X9, L_str20_plus_8@PAGE
    ADD X9, X9, L_str20_plus_8@PAGEOFF
    MOV X0, X9
    MOV X1, X22
    BL _WRITEF1
    ADRP X9, L_str21_plus_8@PAGE
    ADD X9, X9, L_str21_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    SUB SP, SP, #16
    ADRP X9, L_str4_plus_8@PAGE
    ADD X9, X9, L_str4_plus_8@PAGEOFF
    STR X9, [SP, #0]
    LDR X0, [SP, #0]
    ADD SP, SP, #16
    BL _WRITES
    ADRP X9, L_str5_plus_8@PAGE
    ADD X9, X9, L_str5_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str23_plus_8@PAGE
    ADD X9, X9, L_str23_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    fmov D0, X27
    fmov D1, X22
    mul v0.2s, v0.2s, v1.2s    ; new dedicated 2s encoder
    fmov X21, D0
    MOV X20, X21
    ADRP X9, L_str20_plus_8@PAGE
    ADD X9, X9, L_str20_plus_8@PAGEOFF
    MOV X0, X9
    MOV X1, X20
    BL _WRITEF1
    ADRP X9, L_str21_plus_8@PAGE
    ADD X9, X9, L_str21_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    SUB SP, SP, #16
    ADRP X9, L_str4_plus_8@PAGE
    ADD X9, X9, L_str4_plus_8@PAGEOFF
    STR X9, [SP, #0]
    LDR X0, [SP, #0]
    ADD SP, SP, #16
    BL _WRITES
    ADRP X9, L_str5_plus_8@PAGE
    ADD X9, X9, L_str5_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str24_plus_8@PAGE
    ADD X9, X9, L_str24_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str25_plus_8@PAGE
    ADD X9, X9, L_str25_plus_8@PAGEOFF
    MOV X0, X9
    MOV X1, X23
    BL _WRITEF1
    SUB SP, SP, #16
    ADRP X9, L_str4_plus_8@PAGE
    ADD X9, X9, L_str4_plus_8@PAGEOFF
    STR X9, [SP, #0]
    LDR X0, [SP, #0]
    ADD SP, SP, #16
    BL _WRITES
    ADRP X9, L_str26_plus_8@PAGE
    ADD X9, X9, L_str26_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str27_plus_8@PAGE
    ADD X9, X9, L_str27_plus_8@PAGEOFF
    MOV X0, X9
    MOV X1, X24
    BL _WRITEF1
    SUB SP, SP, #16
    ADRP X9, L_str4_plus_8@PAGE
    ADD X9, X9, L_str4_plus_8@PAGEOFF
    STR X9, [SP, #0]
    LDR X0, [SP, #0]
    ADD SP, SP, #16
    BL _WRITES
    ADRP X9, L_str26_plus_8@PAGE
    ADD X9, X9, L_str26_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str28_plus_8@PAGE
    ADD X9, X9, L_str28_plus_8@PAGEOFF
    MOV X0, X9
    MOV X1, X20
    BL _WRITEF1
    SUB SP, SP, #16
    ADRP X9, L_str4_plus_8@PAGE
    ADD X9, X9, L_str4_plus_8@PAGEOFF
    STR X9, [SP, #0]
    LDR X0, [SP, #0]
    ADD SP, SP, #16
    BL _WRITES
    ADRP X9, L_str26_plus_8@PAGE
    ADD X9, X9, L_str26_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str29_plus_8@PAGE
    ADD X9, X9, L_str29_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    B L_START_Exit_1
L_START_Exit_1:
    B L_0
L_0:
    LDP x19, x20, [x29, #80]
    LDP x21, x22, [x29, #96]
    LDP x23, x24, [x29, #112]
    LDP x25, x26, [x29, #128]
    LDP x27, x28, [x29, #144]
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [SP, #0]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET

.section __DATA,__const
.p2align 3
L_str0:
    .quad 0x26
    ; (upper half)
.p2align 2
L_str0_plus_8:
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0x20
    .long 0x44
    .long 0x65
    .long 0x74
    .long 0x61
    .long 0x69
    .long 0x6c
    .long 0x65
    .long 0x64
    .long 0x20
    .long 0x54
    .long 0x79
    .long 0x70
    .long 0x65
    .long 0x20
    .long 0x49
    .long 0x6e
    .long 0x66
    .long 0x65
    .long 0x72
    .long 0x65
    .long 0x6e
    .long 0x63
    .long 0x65
    .long 0x20
    .long 0x44
    .long 0x65
    .long 0x62
    .long 0x75
    .long 0x67
    .long 0x20
    .long 0x3d
    .long 0x3d
    .long 0x3d
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str1:
    .quad 0x19
    ; (upper half)
.p2align 2
L_str1_plus_8:
    .long 0x31
    .long 0x2e
    .long 0x20
    .long 0x53
    .long 0x69
    .long 0x6d
    .long 0x70
    .long 0x6c
    .long 0x65
    .long 0x20
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x20
    .long 0x76
    .long 0x61
    .long 0x72
    .long 0x69
    .long 0x61
    .long 0x62
    .long 0x6c
    .long 0x65
    .long 0x3a
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str2:
    .quad 0xb
    ; (upper half)
.p2align 2
L_str2_plus_8:
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x70
    .long 0x31
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x50
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str3:
    .quad 0xe
    ; (upper half)
.p2align 2
L_str3_plus_8:
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x54
    .long 0x59
    .long 0x50
    .long 0x45
    .long 0x28
    .long 0x70
    .long 0x31
    .long 0x29
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x0
    .long 0x0
.p2align 3
L_str4:
    .quad 0x4
    ; (upper half)
.p2align 2
L_str4_plus_8:
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x0
    .long 0x0
.p2align 3
L_str5:
    .quad 0x1
    ; (upper half)
.p2align 2
L_str5_plus_8:
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str6:
    .quad 0x1a
    ; (upper half)
.p2align 2
L_str6_plus_8:
    .long 0x32
    .long 0x2e
    .long 0x20
    .long 0x41
    .long 0x6e
    .long 0x6f
    .long 0x74
    .long 0x68
    .long 0x65
    .long 0x72
    .long 0x20
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x20
    .long 0x76
    .long 0x61
    .long 0x72
    .long 0x69
    .long 0x61
    .long 0x62
    .long 0x6c
    .long 0x65
    .long 0x3a
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str7:
    .quad 0xb
    ; (upper half)
.p2align 2
L_str7_plus_8:
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x70
    .long 0x32
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x50
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str8:
    .quad 0xe
    ; (upper half)
.p2align 2
L_str8_plus_8:
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x54
    .long 0x59
    .long 0x50
    .long 0x45
    .long 0x28
    .long 0x70
    .long 0x32
    .long 0x29
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x0
    .long 0x0
.p2align 3
L_str9:
    .quad 0x21
    ; (upper half)
.p2align 2
L_str9_plus_8:
    .long 0x33
    .long 0x2e
    .long 0x20
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
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
    .long 0x3a
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str10:
    .quad 0x10
    ; (upper half)
.p2align 2
L_str10_plus_8:
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x70
    .long 0x31
    .long 0x20
    .long 0x2a
    .long 0x20
    .long 0x70
    .long 0x32
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x50
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str11:
    .quad 0x13
    ; (upper half)
.p2align 2
L_str11_plus_8:
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x54
    .long 0x59
    .long 0x50
    .long 0x45
    .long 0x28
    .long 0x70
    .long 0x31
    .long 0x20
    .long 0x2a
    .long 0x20
    .long 0x70
    .long 0x32
    .long 0x29
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x0
    .long 0x0
.p2align 3
L_str12:
    .quad 0x1f
    ; (upper half)
.p2align 2
L_str12_plus_8:
    .long 0x34
    .long 0x2e
    .long 0x20
    .long 0x53
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x6c
    .long 0x65
    .long 0x20
    .long 0x64
    .long 0x69
    .long 0x72
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x20
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x20
    .long 0x6c
    .long 0x69
    .long 0x74
    .long 0x65
    .long 0x72
    .long 0x61
    .long 0x6c
    .long 0x3a
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str13:
    .quad 0x12
    ; (upper half)
.p2align 2
L_str13_plus_8:
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x28
    .long 0x36
    .long 0x2c
    .long 0x38
    .long 0x29
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x50
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str14:
    .quad 0x15
    ; (upper half)
.p2align 2
L_str14_plus_8:
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x54
    .long 0x59
    .long 0x50
    .long 0x45
    .long 0x28
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x28
    .long 0x36
    .long 0x2c
    .long 0x38
    .long 0x29
    .long 0x29
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x0
    .long 0x0
.p2align 3
L_str15:
    .quad 0x2a
    ; (upper half)
.p2align 2
L_str15_plus_8:
    .long 0x35
    .long 0x2e
    .long 0x20
    .long 0x49
    .long 0x6d
    .long 0x6d
    .long 0x65
    .long 0x64
    .long 0x69
    .long 0x61
    .long 0x74
    .long 0x65
    .long 0x20
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x20
    .long 0x6c
    .long 0x69
    .long 0x74
    .long 0x65
    .long 0x72
    .long 0x61
    .long 0x6c
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
    .long 0x3a
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str16:
    .quad 0x1e
    ; (upper half)
.p2align 2
L_str16_plus_8:
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x28
    .long 0x36
    .long 0x2c
    .long 0x38
    .long 0x29
    .long 0x20
    .long 0x2a
    .long 0x20
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x28
    .long 0x33
    .long 0x2c
    .long 0x32
    .long 0x29
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x50
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str17:
    .quad 0x21
    ; (upper half)
.p2align 2
L_str17_plus_8:
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x54
    .long 0x59
    .long 0x50
    .long 0x45
    .long 0x28
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x28
    .long 0x36
    .long 0x2c
    .long 0x38
    .long 0x29
    .long 0x20
    .long 0x2a
    .long 0x20
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x28
    .long 0x33
    .long 0x2c
    .long 0x32
    .long 0x29
    .long 0x29
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x0
    .long 0x0
.p2align 3
L_str18:
    .quad 0x1b
    ; (upper half)
.p2align 2
L_str18_plus_8:
    .long 0x36
    .long 0x2e
    .long 0x20
    .long 0x53
    .long 0x74
    .long 0x65
    .long 0x70
    .long 0x20
    .long 0x62
    .long 0x79
    .long 0x20
    .long 0x73
    .long 0x74
    .long 0x65
    .long 0x70
    .long 0x20
    .long 0x62
    .long 0x72
    .long 0x65
    .long 0x61
    .long 0x6b
    .long 0x64
    .long 0x6f
    .long 0x77
    .long 0x6e
    .long 0x3a
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str19:
    .quad 0x1b
    ; (upper half)
.p2align 2
L_str19_plus_8:
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x4c
    .long 0x65
    .long 0x66
    .long 0x74
    .long 0x20
    .long 0x6f
    .long 0x70
    .long 0x65
    .long 0x72
    .long 0x61
    .long 0x6e
    .long 0x64
    .long 0x20
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x28
    .long 0x36
    .long 0x2c
    .long 0x38
    .long 0x29
    .long 0x3a
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str20:
    .quad 0xf
    ; (upper half)
.p2align 2
L_str20_plus_8:
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x56
    .long 0x61
    .long 0x6c
    .long 0x75
    .long 0x65
    .long 0x3a
    .long 0x20
    .long 0x25
    .long 0x50
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str21:
    .quad 0xb
    ; (upper half)
.p2align 2
L_str21_plus_8:
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x54
    .long 0x79
    .long 0x70
    .long 0x65
    .long 0x3a
    .long 0x20
    .long 0x0
    .long 0x0
.p2align 3
L_str22:
    .quad 0x1c
    ; (upper half)
.p2align 2
L_str22_plus_8:
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x52
    .long 0x69
    .long 0x67
    .long 0x68
    .long 0x74
    .long 0x20
    .long 0x6f
    .long 0x70
    .long 0x65
    .long 0x72
    .long 0x61
    .long 0x6e
    .long 0x64
    .long 0x20
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0x28
    .long 0x33
    .long 0x2c
    .long 0x32
    .long 0x29
    .long 0x3a
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str23:
    .quad 0x1a
    ; (upper half)
.p2align 2
L_str23_plus_8:
    .long 0x20
    .long 0x20
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
    .long 0x20
    .long 0x72
    .long 0x65
    .long 0x73
    .long 0x75
    .long 0x6c
    .long 0x74
    .long 0x3a
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str24:
    .quad 0x18
    ; (upper half)
.p2align 2
L_str24_plus_8:
    .long 0xa
    .long 0x37
    .long 0x2e
    .long 0x20
    .long 0x53
    .long 0x75
    .long 0x6d
    .long 0x6d
    .long 0x61
    .long 0x72
    .long 0x79
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
    .long 0x73
    .long 0x3a
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str25:
    .quad 0x1c
    ; (upper half)
.p2align 2
L_str25_plus_8:
    .long 0x20
    .long 0x20
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
    .long 0x6d
    .long 0x75
    .long 0x6c
    .long 0x74
    .long 0x3a
    .long 0x20
    .long 0x25
    .long 0x50
    .long 0x20
    .long 0x28
    .long 0x74
    .long 0x79
    .long 0x70
    .long 0x65
    .long 0x3a
    .long 0x20
    .long 0x0
    .long 0x0
.p2align 3
L_str26:
    .quad 0x2
    ; (upper half)
.p2align 2
L_str26_plus_8:
    .long 0x29
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_str27:
    .quad 0x1c
    ; (upper half)
.p2align 2
L_str27_plus_8:
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x4c
    .long 0x69
    .long 0x74
    .long 0x65
    .long 0x72
    .long 0x61
    .long 0x6c
    .long 0x20
    .long 0x6d
    .long 0x75
    .long 0x6c
    .long 0x74
    .long 0x3a
    .long 0x20
    .long 0x20
    .long 0x25
    .long 0x50
    .long 0x20
    .long 0x28
    .long 0x74
    .long 0x79
    .long 0x70
    .long 0x65
    .long 0x3a
    .long 0x20
    .long 0x0
    .long 0x0
.p2align 3
L_str28:
    .quad 0x1c
    ; (upper half)
.p2align 2
L_str28_plus_8:
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x53
    .long 0x74
    .long 0x65
    .long 0x70
    .long 0x20
    .long 0x6d
    .long 0x75
    .long 0x6c
    .long 0x74
    .long 0x3a
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x25
    .long 0x50
    .long 0x20
    .long 0x28
    .long 0x74
    .long 0x79
    .long 0x70
    .long 0x65
    .long 0x3a
    .long 0x20
    .long 0x0
    .long 0x0
.p2align 3
L_str29:
    .quad 0x39
    ; (upper half)
.p2align 2
L_str29_plus_8:
    .long 0xa
    .long 0x45
    .long 0x78
    .long 0x70
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x65
    .long 0x64
    .long 0x20
    .long 0x62
    .long 0x65
    .long 0x68
    .long 0x61
    .long 0x76
    .long 0x69
    .long 0x6f
    .long 0x72
    .long 0x3a
    .long 0x20
    .long 0x41
    .long 0x6c
    .long 0x6c
    .long 0x20
    .long 0x73
    .long 0x68
    .long 0x6f
    .long 0x75
    .long 0x6c
    .long 0x64
    .long 0x20
    .long 0x62
    .long 0x65
    .long 0x20
    .long 0x28
    .long 0x31
    .long 0x38
    .long 0x2c
    .long 0x31
    .long 0x36
    .long 0x29
    .long 0x20
    .long 0x77
    .long 0x69
    .long 0x74
    .long 0x68
    .long 0x20
    .long 0x74
    .long 0x79
    .long 0x70
    .long 0x65
    .long 0x20
    .long 0x50
    .long 0x41
    .long 0x49
    .long 0x52
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 3
L_pair0:
    .quad 0x800000006
    ; (upper half)
L_pair1:
    .quad 0x200000003
    ; (upper half)

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
    .long 0x0
    .long 0x0
    .long 0x0
.p2align 2
L__data_segment_base:
