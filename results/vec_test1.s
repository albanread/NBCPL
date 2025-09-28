.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _WRITEN
.globl _BCPL_ALLOC_CHARS
.globl _RDCH
.globl _MALLOC
.globl _BCPL_ALLOC_WORDS
.globl _WRITEC
.globl _WRITES
.globl _FREEVEC
.p2align 2
_start:
    B L_START
L_START:
    STP X29, X30, [SP, #-144]!
    MOV X29, SP
    MOVZ X9, #30600
    MOVK X9, #21862, LSL #16
    MOVK X9, #13124, LSL #32
    MOVK X9, #4386, LSL #48 ; Load UPPER_CANARY_VALUE
    STR X9, [X29, #16] ; Store Upper Stack Canary
    MOVZ X9, #17
    MOVK X9, #61183, LSL #16
    MOVK X9, #52445, LSL #32
    MOVK X9, #43707, LSL #48 ; Load LOWER_CANARY_VALUE
    STR X9, [X29, #24] ; Store Lower Stack Canary
    STR X19, [X29, #120] ; [JIT_STORE] ; Saved Reg: X19 @ FP+120
    STR X20, [X29, #128] ; [JIT_STORE] ; Saved Reg: X20 @ FP+128
    MOVZ X10, #10
    MOV X0, X10
    ADRP X1, L_str0@PAGE
    ADD X1, X1, L_str0@PAGEOFF
    ADRP X2, L_str1@PAGE
    ADD X2, X2, L_str1@PAGEOFF
    BL _BCPL_ALLOC_WORDS
    STR X0, [X29, #112] ; V
    MOVZ X10, #10
    MOV X0, X10
    BL _BCPL_ALLOC_CHARS
    STR X0, [X29, #96]
    ADRP X10, L_str2@PAGE
    ADD X10, X10, L_str2@PAGEOFF
    STR X10, [X29, #104] ; S2
    ADRP X10, L_str3@PAGE
    ADD X10, X10, L_str3@PAGEOFF
    MOV X0, X10
    BL _WRITES
    LDR X10, [X28]
    LDR X11, [X28, #8]
    MUL X10, X10, X11
    MOV X0, X10
    BL _WRITEN
    ADRP X10, L_str4@PAGE
    ADD X10, X10, L_str4@PAGEOFF
    MOV X0, X10
    BL _WRITES
    MOVZ X10, #7
    STR X10, [X28] ; G
    ADRP X10, L_str3@PAGE
    ADD X10, X10, L_str3@PAGEOFF
    MOV X0, X10
    BL _WRITES
    LDR X10, [X28]
    LDR X11, [X28, #8]
    MUL X10, X10, X11
    MOV X0, X10
    BL _WRITEN
    ADRP X10, L_str4@PAGE
    ADD X10, X10, L_str4@PAGEOFF
    MOV X0, X10
    BL _WRITES
    ADRP X10, L_str5@PAGE
    ADD X10, X10, L_str5@PAGEOFF
    MOV X0, X10
    BL _WRITES
    LDR X21, [X29, #104]
    MOV X0, X21
    BL _WRITES
    LDR X22, [X29, #112]
    MOVZ X10, #0
    MOV X11, X22
    CMP X11, X10
    CSET X11, EQ
    CMP X11, XZR
    B.EQ L_0
    ADRP X10, L_str6@PAGE
    ADD X10, X10, L_str6@PAGEOFF
    MOV X0, X10
    BL _WRITES
    MOVZ X10, #0
    MOV X0, X10
    MOVZ X10, #1
    MOVK X10, #512, LSL #16
    MOVK X10, #0, LSL #32
    MOVK X10, #0, LSL #48
    MOV X16, X10
    SVC #128
L_0:
    MOVZ X10, #0
    STR X10, [X29, #40] ; loop_counter
    MOVZ X10, #1
    STR X10, [X29, #72] ; loop_step
    MOVZ X10, #10
    MOVZ X11, #1
    SUB X10, X10, X11
    STR X10, [X29, #48]
    B L_2
L_1:
    LDR X23, [X29, #40]
    MOVZ X10, #5
    MOV X11, X23
    MUL X11, X11, X10
    LDR X22, [X29, #112]
    LDR X23, [X29, #40]
    LSL X23, X23, #3
    ADD X10, X22, X23
    STR X11, [X10]
    LDR X10, [X29, #40]
    LDR X11, [X29, #72]
    ADD X10, X10, X11
    STR X10, [X29, #40]
L_2:
    LDR X10, [X29, #40]
    LDR X11, [X29, #48]
    CMP X10, X11
    B.LE L_1
    ADRP X10, L_str7@PAGE
    ADD X10, X10, L_str7@PAGEOFF
    MOV X0, X10
    BL _WRITES
    LDR X24, [X29, #32]
    MOV X0, X24
    BL _WRITEN
    ADRP X10, L_str4@PAGE
    ADD X10, X10, L_str4@PAGEOFF
    MOV X0, X10
    BL _WRITES
    MOVZ X10, #0
    STR X10, [X29, #40] ; loop_counter
    MOVZ X10, #1
    STR X10, [X29, #80] ; loop_step
    MOVZ X10, #10
    MOVZ X11, #1
    SUB X10, X10, X11
    STR X10, [X29, #56]
    B L_4
L_3:
    ADRP X10, L_str7@PAGE
    ADD X10, X10, L_str7@PAGEOFF
    MOV X0, X10
    BL _WRITES
    LDR X23, [X29, #40]
    MOV X0, X23
    BL _WRITEN
    ADRP X10, L_str4@PAGE
    ADD X10, X10, L_str4@PAGEOFF
    MOV X0, X10
    BL _WRITES
    LDR X22, [X29, #112]
    LDR X23, [X29, #40]
    LSL X23, X23, #3
    ADD X11, X22, X23
    LDR X10, [X11]
    MOV X0, X10
    BL _WRITEN
    LDR X10, [X29, #40]
    LDR X11, [X29, #80]
    ADD X10, X10, X11
    STR X10, [X29, #40]
L_4:
    LDR X10, [X29, #40]
    LDR X11, [X29, #56]
    CMP X10, X11
    B.LE L_3
    MOVZ X10, #0
    STR X10, [X29, #40] ; loop_counter
    MOVZ X10, #1
    STR X10, [X29, #88] ; loop_step
    MOVZ X10, #10
    MOVZ X11, #1
    SUB X10, X10, X11
    STR X10, [X29, #64]
    B L_6
L_5:
    LDR X23, [X29, #40]
    MOVZ X10, #120
    MOV X11, X23
    MUL X11, X11, X10
    MOVZ X10, #4
    SDIV X11, X11, X10
    LDR X23, [X29, #40]
    MOVZ X10, #3
    MOV X11, X23
    MUL X11, X11, X10
    ADD X11, X11, X11
    MOVZ X10, #7
    SUB X11, X11, X10
    LDR X22, [X29, #112]
    LDR X23, [X29, #40]
    LSL X23, X23, #3
    ADD X10, X22, X23
    STR X11, [X10]
    ADRP X10, L_str7@PAGE
    ADD X10, X10, L_str7@PAGEOFF
    MOV X0, X10
    BL _WRITES
    LDR X23, [X29, #40]
    MOV X0, X23
    BL _WRITEN
    ADRP X10, L_str4@PAGE
    ADD X10, X10, L_str4@PAGEOFF
    MOV X0, X10
    BL _WRITES
    LDR X22, [X29, #112]
    LDR X23, [X29, #40]
    LSL X23, X23, #3
    ADD X11, X22, X23
    LDR X10, [X11]
    MOV X0, X10
    BL _WRITEN
    LDR X10, [X29, #40]
    LDR X11, [X29, #88]
    ADD X10, X10, X11
    STR X10, [X29, #40]
L_6:
    LDR X10, [X29, #40]
    LDR X11, [X29, #64]
    CMP X10, X11
    B.LE L_5
    ADRP X10, L_str8@PAGE
    ADD X10, X10, L_str8@PAGEOFF
    MOV X0, X10
    BL _WRITES
    LDR X22, [X29, #112]
    MOV X0, X22
    BL _FREEVEC
    LDR X19, [X29, #120] ; [JIT_RESTORE] ; Restored Reg: X19 @ FP+120
    LDR X20, [X29, #128] ; [JIT_RESTORE] ; Restored Reg: X20 @ FP+128
    LDR X10, [X29, #16] ; Load Upper Stack Canary for check
    MOVZ X11, #30600
    MOVK X11, #21862, LSL #16
    MOVK X11, #13124, LSL #32
    MOVK X11, #4386, LSL #48 ; Load Expected UPPER_CANARY_VALUE
    CMP X10, X11 ; Compare Upper Canary
    B.NE L_START_stackprot_upper ; Branch if Upper Canary Corrupted
    LDR X10, [X29, #24] ; Load Lower Stack Canary for check
    MOVZ X11, #17
    MOVK X11, #61183, LSL #16
    MOVK X11, #52445, LSL #32
    MOVK X11, #43707, LSL #48 ; Load Expected LOWER_CANARY_VALUE
    CMP X10, X11 ; Compare Lower Canary
    B.NE L_START_stackprot_lower ; Branch if Lower Canary Corrupted
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #144 ; Deallocate space for saved FP/LR
    RET
L_START_stackprot_upper:
    BRK #0 ; Stack Corruption Detected for Upper Canary!
L_START_stackprot_lower:
    BRK #0 ; Stack Corruption Detected for Lower Canary!

.section __TEXT,__const
.p2align 3
L_str0:
    
    .long 0x5
    .long 0x53
    .long 0x54
    .long 0x41
    .long 0x52
    .long 0x54
    .long 0x0
    .long 0x0
L_str1:
    
    .long 0x0
    .long 0x0
    .long 0x0
L_str2:
    
    .long 0x7
    .long 0x48
    .long 0x65
    .long 0x6c
    .long 0x6c
    .long 0x6f
    .long 0x21
    .long 0xa
    .long 0x0
    .long 0x0
L_str3:
    
    .long 0x4
    .long 0x47
    .long 0x2a
    .long 0x48
    .long 0x3d
    .long 0x0
    .long 0x0
L_str4:
    
    .long 0x1
    .long 0xa
    .long 0x0
    .long 0x0
L_str5:
    
    .long 0x5
    .long 0x74
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0xa
    .long 0x0
    .long 0x0
L_str6:
    
    .long 0x5
    .long 0x46
    .long 0x41
    .long 0x49
    .long 0x4c
    .long 0xa
    .long 0x0
    .long 0x0
L_str7:
    
    .long 0x2
    .long 0x49
    .long 0x3d
    .long 0x0
    .long 0x0
L_str8:
    
    .long 0x4
    .long 0x45
    .long 0x4e
    .long 0x44
    .long 0xa
    .long 0x0
    .long 0x0
    .long 0x0, 0x0, 0x0, 0x0

.section __DATA,__data
.p2align 3
L__data_segment_base:
    
G:
    .quad 0x100
H:
    .quad 0x2
