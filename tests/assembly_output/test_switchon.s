.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _START
.globl _WRITES
.globl _WRITEN
.globl _BCPL_ALLOC_CHARS
.globl _PACKSTRING
.globl _STRCMP
.globl _RDCH
.globl _STRCOPY
.globl _WRITEF
.globl _SPIT
.globl _MALLOC
.globl _BCPL_ALLOC_WORDS
.globl _FINISH
.globl _WRITEC
.globl _SLURP
.globl _STRLEN
.globl _UNPACKSTRING
.globl _FREEVEC
.p2align 2
_start:
_START:
    B L_START
L_START:
    STP X29, X30, [SP, #-48]!
    MOV X29, SP
    STR X20, [X29, #32] ; [JIT_STORE] ; Saved Reg: X20 @ FP+32
    STR X21, [X29, #40] ; Saved Reg: X21 @ FP+40
L_START_Entry_0:
    B L_START_BB_1
L_START_BB_1:
    MOVZ X10, #3
    STR X10, [X29, #24]
    B L_START_BB_2
L_START_BB_2:
    B L_START_BB_3
L_START_BB_3:
    LDR X20, [X29, #24]
    MOVZ X10, #1
    MOVK X10, #0, LSL #16
    MOVK X10, #0, LSL #32
    MOVK X10, #0, LSL #48
    CMP X20, X10
    B.EQ L_START_Case_5
    MOVZ X10, #2
    MOVK X10, #0, LSL #16
    MOVK X10, #0, LSL #32
    MOVK X10, #0, LSL #48
    CMP X20, X10
    B.EQ L_START_Case_6
    B L_START_Default_7
L_START_BB_9:
    B L_START_Exit_8
L_START_Case_5:
    ADRP X10, L_str0@PAGE
    ADD X10, X10, L_str0@PAGEOFF
    MOV X0, X10
    BL _WRITES
    B L_START_SwitchEnd_4
L_START_Case_6:
    ADRP X10, L_str1@PAGE
    ADD X10, X10, L_str1@PAGEOFF
    MOV X0, X10
    BL _WRITES
    B L_START_SwitchEnd_4
L_START_Default_7:
    ADRP X10, L_str2@PAGE
    ADD X10, X10, L_str2@PAGEOFF
    MOV X0, X10
    BL _WRITES
    B L_START_SwitchEnd_4
L_START_Exit_8:
    B L_0
L_START_SwitchEnd_4:
    MOVZ X10, #0
    MOV X0, X10
    B L_0
    B L_START_Exit_8
L_0:
    LDR X20, [X29, #32] ; [JIT_RESTORE] ; Restored Reg: X20 @ FP+32
    LDR X21, [X29, #40] ; Restored Reg: X21 @ FP+40
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #48 ; Deallocate space for saved FP/LR
    RET

.section __TEXT,__const
.p2align 3
L_str0:
    
    .long 0x4
    .long 0x4f
    .long 0x6e
    .long 0x65
    .long 0xa
    .long 0x0
    .long 0x0
L_str1:
    
    .long 0x4
    .long 0x54
    .long 0x77
    .long 0x6f
    .long 0xa
    .long 0x0
    .long 0x0
L_str2:
    
    .long 0x6
    .long 0x4f
    .long 0x74
    .long 0x68
    .long 0x65
    .long 0x72
    .long 0xa
    .long 0x0
    .long 0x0
    .long 0x0, 0x0, 0x0, 0x0

.section __DATA,__data
.p2align 3
    .long 0x0
    .long 0x0
