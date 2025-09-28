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
    STP X29, X30, [SP, #-80]!
    MOV X29, SP
    STR X19, [X29, #40] ; Saved Reg: X19 @ FP+40
    STR X20, [X29, #48] ; Saved Reg: X20 @ FP+48
    STR X21, [X29, #56] ; Saved Reg: X21 @ FP+56
    STR X22, [X29, #64] ; Saved Reg: X22 @ FP+64
    STR X28, [X29, #72] ; Saved Reg: X28 @ FP+72
    MOVZ X10, #8, LSL #16
    ADD X19, X28, X10
L_START_Entry_0:
    MOVZ X10, #1
    MOV X0, X10
    ADRP X1, L_str0@PAGE
    ADD X1, X1, L_str0@PAGEOFF
    ADRP X2, L_str1@PAGE
    ADD X2, X2, L_str1@PAGEOFF
    BL _BCPL_ALLOC_WORDS
    STR X0, [X29, #16] ; MEM
    MOV x20, x0
    STR X20, [X29, #24] ; P
    MOVZ X10, #11
    LDR X21, [X29, #24] ; P
    STR X10, [X21]
    MOVZ X11, #10
    MOV X0, X11
    ADRP X1, L_str0@PAGE
    ADD X1, X1, L_str0@PAGEOFF
    ADRP X2, L_str1@PAGE
    ADD X2, X2, L_str1@PAGEOFF
    BL _BCPL_ALLOC_WORDS
    STR X0, [X29, #32] ; V
    MOVZ X11, #10
    LDR X22, [X29, #32] ; V
    MOVZ X12, #1
    LSL X12, X12, #3
    ADD X13, X22, X12
    STR X11, [X13]
    ADRP X10, L_str2@PAGE
    ADD X10, X10, L_str2@PAGEOFF
    ADD X10, X10, #8
    MOV X0, X10
    BL _WRITES
    MOVZ X10, #10
    MOV X0, X10
    BL _WRITEC
    LDR X23, [X29, #24] ; P
    LDR X10, [X23]
    MOV X0, X10
    BL _WRITEN
    B L_START_Exit_1
L_START_Exit_1:
    B L_0
L_0:
    LDR X19, [X29, #40] ; Restored Reg: X19 @ FP+40
    LDR X20, [X29, #48] ; Restored Reg: X20 @ FP+48
    LDR X21, [X29, #56] ; Restored Reg: X21 @ FP+56
    LDR X22, [X29, #64] ; Restored Reg: X22 @ FP+64
    LDR X28, [X29, #72] ; Restored Reg: X28 @ FP+72
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET

.section __TEXT,__const
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
    
    .quad 0xc
    ; (upper half)
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x20
    .long 0x72
    .long 0x65
    .long 0x73
    .long 0x75
    .long 0x6c
    .long 0x74
    .long 0x73
    .long 0x0
    .long 0x0

.section __DATA,__data
.p2align 3
    .long 0x0
    .long 0x0
    .long 0x0
