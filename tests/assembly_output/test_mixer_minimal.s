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
    STP X29, X30, [SP, #-80]!
    MOV X29, SP
    STR X19, [X29, #48] ; Saved Reg: X19 @ FP+48
    STR X27, [X29, #56] ; Saved Reg: X27 @ FP+56
    STR X28, [X29, #64] ; Saved Reg: X28 @ FP+64
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
    MOVZ X9, #8, LSL #16
    ADD X19, X28, X9
L_START_Entry_0:
    ADRP X9, L_str0@PAGE
    ADD X9, X9, L_str0@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    MOVZ X9, #16
    MOV X20, X9
    MOV X0, X20
    LDR X10, [X19, #664]
    BLR X10
    MOV X27, X0
    ADRP X10, L_str1@PAGE
    ADD X10, X10, L_str1@PAGEOFF
    ADD X10, X10, #8
    MOV X0, X10
    MOV X1, X27
    BL _WRITEF1
    MOVZ X9, #44100
    MOV X21, X9
    MOVZ X10, #32784
    SUB X11, XZR, X10
    MOV X22, X11
    MOVZ X10, #2
    MOV X23, X10
    MOVZ X12, #2048
    MOV X24, X12
    MOV X0, X21
    MOV X1, X22
    MOV X2, X23
    MOV X3, X24
    LDR X13, [X19, #912]
    BLR X13
    MOV X27, X0
    ADRP X13, L_str2@PAGE
    ADD X13, X13, L_str2@PAGEOFF
    ADD X13, X13, #8
    MOV X0, X13
    MOV X1, X27
    BL _WRITEF1
    MOVZ X9, #1
    SUB X10, XZR, X9
    MOV X25, X10
    MOVZ X9, #1
    SUB X11, XZR, X9
    MOV X26, X11
    MOV X0, X25
    MOV X1, X26
    LDR X9, [X19, #936]
    BLR X9
    MOV X27, X0
    ADRP X9, L_str3@PAGE
    ADD X9, X9, L_str3@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    MOV X1, X27
    BL _WRITEF1
    MOVZ X9, #1
    SUB X10, XZR, X9
    MOV X27, X10
    MOV X0, X27
    LDR X9, [X19, #1104]
    BLR X9
    MOV X27, X0
    ADRP X9, L_str4@PAGE
    ADD X9, X9, L_str4@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    MOV X1, X27
    BL _WRITEF1
    BL _SDL2_MIXER_CLOSE_AUDIO
    BL _SDL2_QUIT
    ADRP X9, L_str5@PAGE
    ADD X9, X9, L_str5@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    B L_START_BlockCleanup_1
L_START_BlockCleanup_1:
    B L_START_Exit_2
L_START_Exit_2:
    B L_0
L_0:
    LDR X19, [X29, #48] ; Restored Reg: X19 @ FP+48
    LDR X27, [X29, #56] ; Restored Reg: X27 @ FP+56
    LDR X28, [X29, #64] ; Restored Reg: X28 @ FP+64
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET

.section __DATA,__const
.p2align 3
L_str0:
    .quad 0x2c
    ; (upper half)
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x53
    .long 0x44
    .long 0x4c
    .long 0x5f
    .long 0x6d
    .long 0x69
    .long 0x78
    .long 0x65
    .long 0x72
    .long 0x20
    .long 0x66
    .long 0x75
    .long 0x6e
    .long 0x63
    .long 0x74
    .long 0x69
    .long 0x6f
    .long 0x6e
    .long 0x20
    .long 0x72
    .long 0x65
    .long 0x67
    .long 0x69
    .long 0x73
    .long 0x74
    .long 0x72
    .long 0x61
    .long 0x74
    .long 0x69
    .long 0x6f
    .long 0x6e
    .long 0x2e
    .long 0x2e
    .long 0x2e
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0
L_str1:
    .quad 0x21
    ; (upper half)
    .long 0x53
    .long 0x44
    .long 0x4c
    .long 0x32
    .long 0x5f
    .long 0x49
    .long 0x4e
    .long 0x49
    .long 0x54
    .long 0x5f
    .long 0x53
    .long 0x55
    .long 0x42
    .long 0x53
    .long 0x59
    .long 0x53
    .long 0x54
    .long 0x45
    .long 0x4d
    .long 0x53
    .long 0x20
    .long 0x72
    .long 0x65
    .long 0x73
    .long 0x75
    .long 0x6c
    .long 0x74
    .long 0x3a
    .long 0x20
    .long 0x25
    .long 0x64
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0
L_str2:
    .quad 0x22
    ; (upper half)
    .long 0x53
    .long 0x44
    .long 0x4c
    .long 0x32
    .long 0x5f
    .long 0x4d
    .long 0x49
    .long 0x58
    .long 0x45
    .long 0x52
    .long 0x5f
    .long 0x4f
    .long 0x50
    .long 0x45
    .long 0x4e
    .long 0x5f
    .long 0x41
    .long 0x55
    .long 0x44
    .long 0x49
    .long 0x4f
    .long 0x20
    .long 0x72
    .long 0x65
    .long 0x73
    .long 0x75
    .long 0x6c
    .long 0x74
    .long 0x3a
    .long 0x20
    .long 0x25
    .long 0x64
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0
L_str3:
    .quad 0x14
    ; (upper half)
    .long 0x43
    .long 0x75
    .long 0x72
    .long 0x72
    .long 0x65
    .long 0x6e
    .long 0x74
    .long 0x20
    .long 0x76
    .long 0x6f
    .long 0x6c
    .long 0x75
    .long 0x6d
    .long 0x65
    .long 0x3a
    .long 0x20
    .long 0x25
    .long 0x64
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0
L_str4:
    .quad 0x18
    ; (upper half)
    .long 0x41
    .long 0x76
    .long 0x61
    .long 0x69
    .long 0x6c
    .long 0x61
    .long 0x62
    .long 0x6c
    .long 0x65
    .long 0x20
    .long 0x63
    .long 0x68
    .long 0x61
    .long 0x6e
    .long 0x6e
    .long 0x65
    .long 0x6c
    .long 0x73
    .long 0x3a
    .long 0x20
    .long 0x25
    .long 0x64
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0
L_str5:
    .quad 0x28
    ; (upper half)
    .long 0x53
    .long 0x44
    .long 0x4c
    .long 0x5f
    .long 0x6d
    .long 0x69
    .long 0x78
    .long 0x65
    .long 0x72
    .long 0x20
    .long 0x74
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
    .long 0x21
    .long 0x25
    .long 0x4e
    .long 0x0
    .long 0x0

.section __DATA,__data
.p2align 3
    .long 0x0
    .long 0x0
    .long 0x0
    .long 0x0
L__data_segment_base:
