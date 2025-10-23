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

L_START:
    STP X29, X30, [SP, #-96]!
    MOV X29, SP
    STP x19, x20, [x29, #40]
    STP x25, x26, [x29, #56]
    STP x27, x28, [x29, #72]
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
L_START_Entry_0:
    BL _HeapManager_enter_scope
    ADRP X0, L_list0_header@PAGE
    ADD X0, X0, L_list0_header@PAGEOFF
    BL _DEEPCOPYLITERALLIST
    MOV X25, X0
    MOV X9, X25
    ADD X9, X9, #16
    LDR X10, [X9, #0]
    MOV X26, X10
    B L_START_ForEachHeader_1
L_START_Exit_5:
    B L_0
L_START_ForEachAdvance_4:
    CMP X26, #0
    B.EQ L_2
    LDR X9, [X26, #16]
    B L_1
L_2:
    MOV X9, XZR
L_1:
    MOV X26, X9
    B L_START_ForEachHeader_1
L_START_ForEachBody_2:
    CMP X26, #0
    B.EQ L_4
    LDR X9, [X26, #8]
    ADD X9, X9, #8
    B L_3
L_4:
    MOV X9, XZR
L_3:
    MOV X27, X9
    BL _HeapManager_enter_scope
    ADRP X9, L_str3_plus_8@PAGE
    ADD X9, X9, L_str3_plus_8@PAGEOFF
    MOV X0, X9
    MOV X1, X27
    BL _WRITEF1
    BL _HeapManager_exit_scope
    B L_START_ForEachAdvance_4
L_START_ForEachExit_3:
    BL _HeapManager_exit_scope
    B L_START_Exit_5
L_START_ForEachHeader_1:
    CMP X26, XZR
    B.EQ L_START_ForEachExit_3
    B L_START_ForEachBody_2
L_0:
    LDP x19, x20, [x29, #40]
    LDP x25, x26, [x29, #56]
    LDP x27, x28, [x29, #72]
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [SP, #0]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L___veneer_:
    movz x16, #28392
    movk x16, #1324, lsl #16
    movk x16, #1, lsl #32
    movk x16, #0, lsl #48
    blr x16

.section __DATA,__const
.p2align 3
L_str0:
    .quad 0x1
    ; (upper half)
.p2align 2
L_str0_plus_8:
    .long 0x61
    .long 0x0
    .long 0x0
.p2align 3
L_str1:
    .quad 0x6
    ; (upper half)
.p2align 2
L_str1_plus_8:
    .long 0x73
    .long 0x74
    .long 0x72
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x0
    .long 0x0
.p2align 3
L_str2:
    .quad 0x4
    ; (upper half)
.p2align 2
L_str2_plus_8:
    .long 0x6c
    .long 0x69
    .long 0x73
    .long 0x74
    .long 0x0
    .long 0x0
.p2align 3
L_str3:
    .quad 0xc
    ; (upper half)
.p2align 2
L_str3_plus_8:
    .long 0x53
    .long 0x74
    .long 0x72
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x3a
    .long 0x20
    .long 0x25
    .long 0x73
    .long 0x20
    .long 0xa
    .long 0x0
    .long 0x0
.p2align 2
L_list0_header:
    .long 0x0
    .long 0x0
    .quad L_list0_node_2
    .quad L_list0_node_0
    .quad 0x3
    ; (upper half)
.p2align 2
L_list0_node_0:
    .long 0x3
    .long 0x0
    .quad L_str0
    .quad L_list0_node_1
.p2align 2
L_list0_node_1:
    .long 0x3
    .long 0x0
    .quad L_str1
    .quad L_list0_node_2
.p2align 2
L_list0_node_2:
    .long 0x3
    .long 0x0
    .quad L_str2
    .quad 0x0
    ; (upper half)

.section __DATA,__data
.p2align 3
.p2align 2
L__data_segment_base:
