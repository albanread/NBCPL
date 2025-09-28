.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _START
.globl _WRITEN
.globl _SDL2_SET_DRAW_COLOR
.globl _GET_FREE_LIST_HEAD_ADDR
.globl _BCPL_LIST_APPEND_STRING
.globl _WRITEF
.globl _BCPL_LIST_GET_TAIL
.globl _CONCAT
.globl _REVERSE
.globl _WRITEF6
.globl _SDL2_DESTROY_RENDERER
.globl _BCPL_LIST_GET_NTH
.globl _FILE_OPEN_READ
.globl _BCPL_CHECK_AND_DISPLAY_ERRORS
.globl _DEEPCOPYLIST
.globl _BCPL_FREE_LIST_SAFE
.globl _SDL2_MIXER_ALLOCATE_CHANNELS
.globl _BCPL_CLEAR_ERRORS
.globl _FLOG
.globl _FILTER
.globl _RDCH
.globl _WRITEF2
.globl _SDL2_MIXER_PAUSE_MUSIC
.globl _BCPL_ALLOC_WORDS
.globl _FILE_READ
.globl _WRITEC
.globl _WRITEF3
.globl _SPLIT
.globl _BCPL_LIST_CREATE_EMPTY
.globl _SDL2_CREATE_RENDERER
.globl _UNPACKSTRING
.globl _SDL2_DELAY
.globl _FREEVEC
.globl _BCPL_CONCAT_LISTS
.globl _SDL2_GET_EVENT_KEY
.globl _BCPL_LIST_APPEND_FLOAT
.globl _FPND
.globl _FEXP
.globl _WRITEF4
.globl _NEWLINE
.globl _BCPL_LIST_GET_HEAD_AS_FLOAT
.globl _OBJECT_HEAP_FREE
.globl _BCPL_LIST_GET_HEAD_AS_INT
.globl _RUNTIME_METHOD_LOOKUP
.globl _FCOS
.globl _APND
.globl _SDL2_MIXER_PAUSE
.globl _PACKSTRING
.globl _LPND
.globl _BCPL_LIST_APPEND_INT
.globl _SDL2_MIXER_GET_ERROR
.globl _BCPL_FREE_LIST
.globl _FILE_CLOSE
.globl _FIND
.globl _WRITEF1
.globl _FILE_WRITE
.globl _SDL2_GET_EVENT_MOUSE
.globl _SDL2_CREATE_WINDOW
.globl _FILE_TELL
.globl _RAND
.globl _SDL2_FILL_RECT
.globl _FWRITE
.globl _BCPL_ALLOC_CHARS
.globl _WRITEF5
.globl _FABS
.globl _SDL2_GET_CURRENT_VIDEO_DRIVER
.globl _SDL2_INIT_SUBSYSTEMS
.globl _FTAN
.globl _SPIT
.globl _FRND
.globl _SDL2_MIXER_PLAYING_MUSIC
.globl _SDL2_MIXER_PLAY_MUSIC
.globl _FILE_READS
.globl _BCPL_GET_ATOM_TYPE
.globl _SDL2_MIXER_RESUME_MUSIC
.globl _FILE_EOF
.globl _STRCMP
.globl _SDL2_MIXER_LOAD_WAV
.globl _SDL2_GET_VIDEO_DRIVERS
.globl _FINISH
.globl _SDL2_MIXER_FREE_MUSIC
.globl _SDL2_MIXER_RESUME
.globl _WRITES
.globl _SDL2_MIXER_FADE_IN_MUSIC
.globl _JOIN
.globl _BCPL_FREE_CELLS
.globl _MALLOC
.globl _SDL2_MIXER_LOAD_MUS
.globl _SDL2_SET_WINDOW_TITLE
.globl _WRITEF7
.globl _SDL2_DESTROY_WINDOW
.globl _SDL2_MIXER_FADE_OUT_MUSIC
.globl _SDL2_TEST_BASIC
.globl _OBJECT_HEAP_ALLOC
.globl _SDL2_MIXER_PAUSED
.globl _SDL2_MIXER_CLOSE_AUDIO
.globl _SDL2_DRAW_LINE
.globl _SDL2_MIXER_VOLUME_MUSIC
.globl _FSIN
.globl _FILE_OPEN_APPEND
.globl _SDL2_CREATE_WINDOW_EX
.globl _STRCOPY
.globl _FILE_OPEN_WRITE
.globl _SDL2_GET_TICKS
.globl _RETURNNODETOFREELIST
.globl ___SDL2_STATIC_BUILD
.globl _SDL2_CREATE_RENDERER_EX
.globl _SPND
.globl _STRLEN
.globl _SDL2_DRAW_POINT
.globl _SDL2_POLL_EVENT
.globl _SDL2_MIXER_OPEN_AUDIO
.globl _SDL2_MIXER_QUIT
.globl _BCPL_GET_LAST_ERROR
.globl _SDL2_PRESENT
.globl _COPYLIST
.globl _SDL2_DRAW_RECT
.globl _SDL2_GET_DISPLAY_MODES
.globl _SDL2_GET_ERROR
.globl _SDL2_MIXER_PAUSED_MUSIC
.globl _SLURP
.globl _SDL2_MIXER_PLAY_CHANNEL
.globl _SDL2_CLEAR_ERROR
.globl _SDL2_SET_WINDOW_SIZE
.globl _SDL2_MIXER_PLAYING
.globl _SDL2_GET_EVENT_BUTTON
.globl _SETTYPE
.globl _FILE_SEEK
.globl _SDL2_GET_VERSION
.globl _SDL2_MIXER_PLAY_CHANNEL_TIMED
.globl _SDL2_MIXER_VOLUME
.globl _SDL2_INIT
.globl _FILE_WRITES
.globl _SDL2_QUIT
.globl _SDL2_MIXER_FREE_CHUNK
.globl _BCPL_LIST_GET_REST
.globl _SDL2_CLEAR
.globl _SDL2_MIXER_HALT_CHANNEL
.globl _PIC_RUNTIME_HELPER
.globl _SDL2_MIXER_HALT_MUSIC
.globl _DEEPCOPYLITERALLIST
.p2align 2
_start:
_START:
    B L_START
L_START:
    STP X29, X30, [SP, #-176]!
    MOV X29, SP
    STR D15, [X29, #80]
    STR X19, [X29, #88]
    STR X20, [X29, #96]
    STR X21, [X29, #104]
    STR X22, [X29, #112]
    STR X23, [X29, #120]
    STR X24, [X29, #128]
    STR X25, [X29, #136]
    STR X26, [X29, #144]
    STR X27, [X29, #152]
    STR X28, [X29, #160]
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
    MOVZ X9, #8, LSL #16
    ADD X19, X28, X9
L_START_Entry_0:
    MOVZ X9, #5
    MOV X0, X9
    ADRP X1, L_str0@PAGE
    ADD X1, X1, L_str0@PAGEOFF
    ADRP X2, L_str1@PAGE
    ADD X2, X2, L_str1@PAGEOFF
    MOV X27, X0
    MOVZ X9, #0
    CMP X27, X9
    CSET X10, EQ
    CMP X10, XZR
    B.EQ L_START_Join_2
    B L_START_Then_1
L_START_BlockCleanup_11:
    B L_START_Exit_12
L_START_Exit_12:
    B L_0
L_START_ForBody_4:
    SCVTF D0, X26
    ADRP X10, L_float0@PAGE
    ADD X10, X10, L_float0@PAGEOFF
    LDR D0, [X10, #0]
    FMUL D0, D0, D0
    LSL X26, X26, #3
    ADD X10, X27, X26
    STR D0, [X10, #0]
    B L_START_ForIncrement_5
L_START_ForEachBody_8:
    LSL X23, X23, #3
    ADD X10, X25, X23
    LDR D0, [X10, #0]
    FMOV D15, D0
    FMOV D15, D0
    ADRP X10, L_str2@PAGE
    ADD X10, X10, L_str2@PAGEOFF
    ADD X10, X10, #8
    MOV X0, X10
    FMOV X10, D15
    MOV X1, X10
    BL _WRITEF1
    B L_START_ForEachIncrement_9
L_START_ForEachExit_10:
    MOVZ W9, #10
    MOV X0, X9
    BL _WRITEC
    B L_START_BlockCleanup_11
L_START_ForEachHeader_7:
    CMP X23, X24
    CSET X9, GE
    CMP X9, XZR
    B.NE L_START_ForEachExit_10
    B L_START_ForEachBody_8
L_START_ForEachIncrement_9:
    MOVZ X9, #1
    ADD X23, X23, X9
    B L_START_ForEachHeader_7
L_START_ForExit_6:
    ADRP X9, L_str3@PAGE
    ADD X9, X9, L_str3@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    MOV X25, X27
    SUB X10, X25, #8
    LDR X9, [X10, #0] ; Load vector/table/string length
    MOV X24, X9
    MOVZ X9, #0
    MOV X23, X9
    B L_START_ForEachHeader_7
L_START_ForHeader_3:
    MOV X9, X26
    MOVZ X10, #5
    MOVZ X11, #1
    SUB X10, X10, X11
    CMP X9, X10
    B.GT L_START_ForExit_6
    B L_START_ForBody_4
L_START_ForIncrement_5:
    MOVZ X9, #1
    ADD X26, X26, X9
    B L_START_ForHeader_3
L_START_Join_2:
    MOVZ X9, #0
    MOV X26, X9
    B L_START_ForHeader_3
L_START_Then_1:
    ADRP X9, L_str4@PAGE
    ADD X9, X9, L_str4@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    MOVZ X9, #0
    MOV X0, X9
    MOVZ X9, #1
    MOVK X9, #512, LSL #16
    MOV X16, X9
    SVC #128
L_0:
    LDR D15, [X29, #80] ; Restored Reg: D15 @ FP+80
    LDR X19, [X29, #88] ; Restored Reg: X19 @ FP+88
    LDR X20, [X29, #96] ; Restored Reg: X20 @ FP+96
    LDR X21, [X29, #104] ; Restored Reg: X21 @ FP+104
    LDR X22, [X29, #112] ; Restored Reg: X22 @ FP+112
    LDR X23, [X29, #120] ; Restored Reg: X23 @ FP+120
    LDR X24, [X29, #128] ; Restored Reg: X24 @ FP+128
    LDR X25, [X29, #136] ; Restored Reg: X25 @ FP+136
    LDR X26, [X29, #144] ; Restored Reg: X26 @ FP+144
    LDR X27, [X29, #152] ; Restored Reg: X27 @ FP+152
    LDR X28, [X29, #160] ; Restored Reg: X28 @ FP+160
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET

.section __DATA,__const
.p2align 3
L_str0:
    .quad 0x5
    ; (upper half)
    .long 0x53
    .long 0x54
    .long 0x41
    .long 0x52
    .long 0x54
    .long 0x0
    .long 0x0
L_str1:
    .quad 0x0
    ; (upper half)
    .long 0x0
    .long 0x0
L_str2:
    .quad 0x3
    ; (upper half)
    .long 0x25
    .long 0x66
    .long 0x20
    .long 0x0
    .long 0x0
L_str3:
    .quad 0x14
    ; (upper half)
    .long 0x46
    .long 0x6f
    .long 0x72
    .long 0x65
    .long 0x61
    .long 0x63
    .long 0x68
    .long 0x20
    .long 0x66
    .long 0x6c
    .long 0x6f
    .long 0x61
    .long 0x74
    .long 0x20
    .long 0x74
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x3a
    .long 0xa
    .long 0x0
    .long 0x0
L_str4:
    .quad 0x1d
    ; (upper half)
    .long 0x46
    .long 0x41
    .long 0x49
    .long 0x4c
    .long 0x3a
    .long 0x20
    .long 0x46
    .long 0x56
    .long 0x45
    .long 0x43
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
    .long 0x61
    .long 0x69
    .long 0x6c
    .long 0x65
    .long 0x64
    .long 0xa
    .long 0x0
    .long 0x0
L_float0:
    .quad 0x4004000000000000
    ; (upper half)

.section __DATA,__data
.p2align 3
    .long 0x0
    .long 0x0
L__data_segment_base:
