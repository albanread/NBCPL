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
    STP X29, X30, [SP, #-128]!
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
    STR X19, [X29, #112] ; [JIT_STORE] ; Saved Reg: X19 @ FP+112
    STR X20, [X29, #120] ; [JIT_STORE] ; Saved Reg: X20 @ FP+120
L_START_Entry_0:
    ADRP X10, L_str0@PAGE
    ADD X10, X10, L_str0@PAGEOFF
    STR X10, [X29, #96]
    MOVZ W10, #33
    STR X10, [X29, #88]
    MOVZ X10, #123
    STR X10, [X29, #80]
    MOVZ X10, #456
    SUB X10, XZR, X10
    STR X10, [X29, #72]
    MOVZ X10, #0
    STR X10, [X29, #104]
    ADRP X10, L_str1@PAGE
    ADD X10, X10, L_str1@PAGEOFF
    MOV X21, X10
    SCVTF D0, X21
    BL _WRITEF
    ADRP X10, L_str2@PAGE
    ADD X10, X10, L_str2@PAGEOFF
    MOV X21, X10
    SCVTF D0, X21
    BL _WRITEF
    ADRP X10, L_str3@PAGE
    ADD X10, X10, L_str3@PAGEOFF
    MOV X21, X10
    LDR X22, [X29, #80]
    MOV X23, X22
    SCVTF D0, X21
    SCVTF D1, X23
    BL _WRITEF
    ADRP X10, L_str4@PAGE
    ADD X10, X10, L_str4@PAGEOFF
    MOV X21, X10
    LDR X22, [X29, #72]
    MOV X23, X22
    SCVTF D0, X21
    SCVTF D1, X23
    BL _WRITEF
    ADRP X10, L_str5@PAGE
    ADD X10, X10, L_str5@PAGEOFF
    MOV X21, X10
    LDR X22, [X29, #104]
    MOV X23, X22
    SCVTF D0, X21
    SCVTF D1, X23
    BL _WRITEF
    ADRP X10, L_str6@PAGE
    ADD X10, X10, L_str6@PAGEOFF
    MOV X21, X10
    SCVTF D0, X21
    BL _WRITEF
    ADRP X10, L_str7@PAGE
    ADD X10, X10, L_str7@PAGEOFF
    MOV X21, X10
    ADRP X10, L_str8@PAGE
    ADD X10, X10, L_str8@PAGEOFF
    MOV X22, X10
    SCVTF D0, X21
    SCVTF D1, X22
    BL _WRITEF
    ADRP X10, L_str9@PAGE
    ADD X10, X10, L_str9@PAGEOFF
    MOV X21, X10
    LDR X22, [X29, #96]
    MOV X23, X22
    SCVTF D0, X21
    SCVTF D1, X23
    BL _WRITEF
    ADRP X10, L_str10@PAGE
    ADD X10, X10, L_str10@PAGEOFF
    MOV X21, X10
    SCVTF D0, X21
    BL _WRITEF
    ADRP X10, L_str11@PAGE
    ADD X10, X10, L_str11@PAGEOFF
    MOV X21, X10
    MOVZ W10, #65
    MOV X22, X10
    SCVTF D0, X21
    SCVTF D1, X22
    BL _WRITEF
    ADRP X10, L_str12@PAGE
    ADD X10, X10, L_str12@PAGEOFF
    MOV X21, X10
    LDR X22, [X29, #88]
    MOV X23, X22
    SCVTF D0, X21
    SCVTF D1, X23
    BL _WRITEF
    ADRP X10, L_str13@PAGE
    ADD X10, X10, L_str13@PAGEOFF
    MOV X21, X10
    SCVTF D0, X21
    BL _WRITEF
    ADRP X10, L_str14@PAGE
    ADD X10, X10, L_str14@PAGEOFF
    MOV X21, X10
    MOVZ X10, #100
    MOV X22, X10
    SCVTF D0, X21
    SCVTF D1, X22
    BL _WRITEF
    ADRP X10, L_str15@PAGE
    ADD X10, X10, L_str15@PAGEOFF
    MOV X21, X10
    MOVZ X10, #100
    MOV X22, X10
    SCVTF D0, X21
    SCVTF D1, X22
    BL _WRITEF
    ADRP X10, L_str16@PAGE
    ADD X10, X10, L_str16@PAGEOFF
    MOV X21, X10
    SCVTF D0, X21
    BL _WRITEF
    ADRP X10, L_str17@PAGE
    ADD X10, X10, L_str17@PAGEOFF
    MOV X21, X10
    SCVTF D0, X21
    BL _WRITEF
    ADRP X10, L_str18@PAGE
    ADD X10, X10, L_str18@PAGEOFF
    MOV X21, X10
    SCVTF D0, X21
    BL _WRITEF
    ADRP X10, L_str19@PAGE
    ADD X10, X10, L_str19@PAGEOFF
    MOV X21, X10
    SCVTF D0, X21
    BL _WRITEF
    ADRP X10, L_str20@PAGE
    ADD X10, X10, L_str20@PAGEOFF
    MOV X21, X10
    SCVTF D0, X21
    BL _WRITEF
    ADRP X10, L_str21@PAGE
    ADD X10, X10, L_str21@PAGEOFF
    MOV X21, X10
    SCVTF D0, X21
    BL _WRITEF
    ADRP X10, L_str22@PAGE
    ADD X10, X10, L_str22@PAGEOFF
    MOV X21, X10
    LDR X22, [X29, #96]
    MOV X23, X22
    MOVZ X10, #1
    MOV X24, X10
    SCVTF D0, X21
    SCVTF D1, X23
    SCVTF D2, X24
    BL _WRITEF
    ADRP X10, L_str23@PAGE
    ADD X10, X10, L_str23@PAGEOFF
    MOV X21, X10
    LDR X22, [X29, #80]
    MOV X23, X22
    LDR X24, [X29, #72]
    MOV X25, X24
    ADRP X10, L_str24@PAGE
    ADD X10, X10, L_str24@PAGEOFF
    MOV X26, X10
    SCVTF D0, X21
    SCVTF D1, X23
    SCVTF D2, X25
    SCVTF D3, X26
    BL _WRITEF
    ADRP X10, L_str25@PAGE
    ADD X10, X10, L_str25@PAGEOFF
    MOV X21, X10
    SCVTF D0, X21
    BL _WRITEF
    ADRP X10, L_str26@PAGE
    ADD X10, X10, L_str26@PAGEOFF
    MOV X21, X10
    SCVTF D0, X21
    BL _WRITEF
    ADRP X10, L_str27@PAGE
    ADD X10, X10, L_str27@PAGEOFF
    MOV X21, X10
    SCVTF D0, X21
    BL _WRITEF
    B L_START_Exit_1
L_START_Exit_1:
    B L_0
L_0:
    LDR X19, [X29, #112] ; [JIT_RESTORE] ; Restored Reg: X19 @ FP+112
    LDR X20, [X29, #120] ; [JIT_RESTORE] ; Restored Reg: X20 @ FP+120
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
    ADD SP, SP, #128 ; Deallocate space for saved FP/LR
    RET
L_START_stackprot_upper:
    BRK #0 ; Stack Corruption Detected for Upper Canary!
L_START_stackprot_lower:
    BRK #0 ; Stack Corruption Detected for Lower Canary!

.section __TEXT,__const
.p2align 3
L_str0:
    
    .long 0x3
    .long 0x42
    .long 0x43
    .long 0x50
    .long 0x4c
    .long 0x0
L_str1:
    
    .long 0x25
    .long 0xa
    .long 0xa
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x20
    .long 0x53
    .long 0x74
    .long 0x61
    .long 0x72
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x57
    .long 0x52
    .long 0x49
    .long 0x54
    .long 0x45
    .long 0x46
    .long 0x20
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x20
    .long 0x53
    .long 0x75
    .long 0x69
    .long 0x74
    .long 0x65
    .long 0x20
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0xa
    .long 0xa
    .long 0x0
L_str2:
    
    .long 0x1f
    .long 0x31
    .long 0x2e
    .long 0x20
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x25
    .long 0x6e
    .long 0x20
    .long 0x28
    .long 0x44
    .long 0x65
    .long 0x63
    .long 0x69
    .long 0x6d
    .long 0x61
    .long 0x6c
    .long 0x20
    .long 0x4e
    .long 0x75
    .long 0x6d
    .long 0x62
    .long 0x65
    .long 0x72
    .long 0x29
    .long 0x3a
    .long 0xa
    .long 0x0
L_str3:
    
    .long 0xf
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x50
    .long 0x6f
    .long 0x73
    .long 0x69
    .long 0x74
    .long 0x69
    .long 0x76
    .long 0x65
    .long 0x3a
    .long 0x20
    .long 0x25
    .long 0x6e
    .long 0xa
    .long 0x0
L_str4:
    
    .long 0xf
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x4e
    .long 0x65
    .long 0x67
    .long 0x61
    .long 0x74
    .long 0x69
    .long 0x76
    .long 0x65
    .long 0x3a
    .long 0x20
    .long 0x25
    .long 0x6e
    .long 0xa
    .long 0x0
L_str5:
    
    .long 0x10
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x5a
    .long 0x65
    .long 0x72
    .long 0x6f
    .long 0x3a
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x25
    .long 0x6e
    .long 0xa
    .long 0xa
    .long 0x0
L_str6:
    
    .long 0x17
    .long 0x32
    .long 0x2e
    .long 0x20
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x25
    .long 0x73
    .long 0x20
    .long 0x28
    .long 0x53
    .long 0x74
    .long 0x72
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x29
    .long 0x3a
    .long 0xa
    .long 0x0
L_str7:
    
    .long 0x15
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x53
    .long 0x74
    .long 0x72
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x6c
    .long 0x69
    .long 0x74
    .long 0x65
    .long 0x72
    .long 0x61
    .long 0x6c
    .long 0x3a
    .long 0x20
    .long 0x25
    .long 0x73
    .long 0xa
    .long 0x0
L_str8:
    
    .long 0xb
    .long 0x48
    .long 0x65
    .long 0x6c
    .long 0x6c
    .long 0x6f
    .long 0x2c
    .long 0x20
    .long 0x57
    .long 0x6f
    .long 0x72
    .long 0x6c
    .long 0x64
    .long 0x0
L_str9:
    
    .long 0x17
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x53
    .long 0x74
    .long 0x72
    .long 0x69
    .long 0x6e
    .long 0x67
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
    .long 0x20
    .long 0x25
    .long 0x73
    .long 0xa
    .long 0xa
    .long 0x0
L_str10:
    
    .long 0x1a
    .long 0x33
    .long 0x2e
    .long 0x20
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x25
    .long 0x63
    .long 0x20
    .long 0x28
    .long 0x43
    .long 0x68
    .long 0x61
    .long 0x72
    .long 0x61
    .long 0x63
    .long 0x74
    .long 0x65
    .long 0x72
    .long 0x29
    .long 0x3a
    .long 0xa
    .long 0x0
L_str11:
    
    .long 0x18
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x43
    .long 0x68
    .long 0x61
    .long 0x72
    .long 0x61
    .long 0x63
    .long 0x74
    .long 0x65
    .long 0x72
    .long 0x20
    .long 0x6c
    .long 0x69
    .long 0x74
    .long 0x65
    .long 0x72
    .long 0x61
    .long 0x6c
    .long 0x3a
    .long 0x20
    .long 0x25
    .long 0x63
    .long 0xa
    .long 0x0
L_str12:
    
    .long 0x1a
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x43
    .long 0x68
    .long 0x61
    .long 0x72
    .long 0x61
    .long 0x63
    .long 0x74
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
    .long 0x3a
    .long 0x20
    .long 0x25
    .long 0x63
    .long 0xa
    .long 0xa
    .long 0x0
L_str13:
    
    .long 0x32
    .long 0x34
    .long 0x2e
    .long 0x20
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x25
    .long 0x68
    .long 0x20
    .long 0x28
    .long 0x48
    .long 0x65
    .long 0x78
    .long 0x29
    .long 0x20
    .long 0x61
    .long 0x6e
    .long 0x64
    .long 0x20
    .long 0x25
    .long 0x6f
    .long 0x20
    .long 0x28
    .long 0x4f
    .long 0x63
    .long 0x74
    .long 0x61
    .long 0x6c
    .long 0x29
    .long 0x20
    .long 0x66
    .long 0x6f
    .long 0x72
    .long 0x20
    .long 0x6e
    .long 0x75
    .long 0x6d
    .long 0x62
    .long 0x65
    .long 0x72
    .long 0x20
    .long 0x31
    .long 0x30
    .long 0x30
    .long 0x3a
    .long 0xa
    .long 0x0
L_str14:
    
    .long 0x12
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x48
    .long 0x65
    .long 0x78
    .long 0x61
    .long 0x64
    .long 0x65
    .long 0x63
    .long 0x69
    .long 0x6d
    .long 0x61
    .long 0x6c
    .long 0x3a
    .long 0x20
    .long 0x25
    .long 0x68
    .long 0xa
    .long 0x0
L_str15:
    
    .long 0x13
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x4f
    .long 0x63
    .long 0x74
    .long 0x61
    .long 0x6c
    .long 0x3a
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x25
    .long 0x6f
    .long 0xa
    .long 0xa
    .long 0x0
L_str16:
    
    .long 0x21
    .long 0x35
    .long 0x2e
    .long 0x20
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x46
    .long 0x6f
    .long 0x72
    .long 0x6d
    .long 0x61
    .long 0x74
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x43
    .long 0x68
    .long 0x61
    .long 0x72
    .long 0x61
    .long 0x63
    .long 0x74
    .long 0x65
    .long 0x72
    .long 0x73
    .long 0x3a
    .long 0xa
    .long 0x0
L_str17:
    
    .long 0x21
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x54
    .long 0x68
    .long 0x69
    .long 0x73
    .long 0x20
    .long 0x6c
    .long 0x69
    .long 0x6e
    .long 0x65
    .long 0x20
    .long 0x65
    .long 0x6e
    .long 0x64
    .long 0x73
    .long 0x20
    .long 0x77
    .long 0x69
    .long 0x74
    .long 0x68
    .long 0x20
    .long 0x61
    .long 0x20
    .long 0x6e
    .long 0x65
    .long 0x77
    .long 0x6c
    .long 0x69
    .long 0x6e
    .long 0x65
    .long 0x2e
    .long 0xa
    .long 0x0
L_str18:
    
    .long 0x18
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x54
    .long 0x68
    .long 0x69
    .long 0x73
    .long 0x9
    .long 0x6c
    .long 0x69
    .long 0x6e
    .long 0x65
    .long 0x9
    .long 0x75
    .long 0x73
    .long 0x65
    .long 0x73
    .long 0x9
    .long 0x74
    .long 0x61
    .long 0x62
    .long 0x73
    .long 0x2e
    .long 0xa
    .long 0xa
    .long 0x0
L_str19:
    
    .long 0x25
    .long 0x36
    .long 0x2e
    .long 0x20
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x25
    .long 0x25
    .long 0x20
    .long 0x28
    .long 0x4c
    .long 0x69
    .long 0x74
    .long 0x65
    .long 0x72
    .long 0x61
    .long 0x6c
    .long 0x20
    .long 0x50
    .long 0x65
    .long 0x72
    .long 0x63
    .long 0x65
    .long 0x6e
    .long 0x74
    .long 0x20
    .long 0x53
    .long 0x69
    .long 0x67
    .long 0x6e
    .long 0x29
    .long 0x3a
    .long 0xa
    .long 0x0
L_str20:
    
    .long 0x21
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x54
    .long 0x68
    .long 0x69
    .long 0x73
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
    .long 0x31
    .long 0x30
    .long 0x30
    .long 0x25
    .long 0x25
    .long 0x20
    .long 0x63
    .long 0x6f
    .long 0x72
    .long 0x72
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x2e
    .long 0xa
    .long 0xa
    .long 0x0
L_str21:
    
    .long 0x27
    .long 0x37
    .long 0x2e
    .long 0x20
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x61
    .long 0x20
    .long 0x63
    .long 0x6f
    .long 0x6d
    .long 0x62
    .long 0x69
    .long 0x6e
    .long 0x61
    .long 0x74
    .long 0x69
    .long 0x6f
    .long 0x6e
    .long 0x20
    .long 0x6f
    .long 0x66
    .long 0x20
    .long 0x73
    .long 0x70
    .long 0x65
    .long 0x63
    .long 0x69
    .long 0x66
    .long 0x69
    .long 0x65
    .long 0x72
    .long 0x73
    .long 0x3a
    .long 0xa
    .long 0x0
L_str22:
    
    .long 0x32
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x54
    .long 0x68
    .long 0x65
    .long 0x20
    .long 0x6c
    .long 0x61
    .long 0x6e
    .long 0x67
    .long 0x75
    .long 0x61
    .long 0x67
    .long 0x65
    .long 0x20
    .long 0x69
    .long 0x73
    .long 0x20
    .long 0x25
    .long 0x73
    .long 0x2e
    .long 0x20
    .long 0x56
    .long 0x65
    .long 0x72
    .long 0x73
    .long 0x69
    .long 0x6f
    .long 0x6e
    .long 0x3a
    .long 0x20
    .long 0x25
    .long 0x6e
    .long 0x2e
    .long 0x20
    .long 0x52
    .long 0x61
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x3a
    .long 0x20
    .long 0x31
    .long 0x30
    .long 0x30
    .long 0x25
    .long 0x25
    .long 0x21
    .long 0xa
    .long 0x0
L_str23:
    
    .long 0x14
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x44
    .long 0x61
    .long 0x74
    .long 0x61
    .long 0x3a
    .long 0x20
    .long 0x25
    .long 0x6e
    .long 0x2c
    .long 0x20
    .long 0x25
    .long 0x6e
    .long 0x2c
    .long 0x20
    .long 0x25
    .long 0x73
    .long 0xa
    .long 0xa
    .long 0x0
L_str24:
    
    .long 0x2
    .long 0x65
    .long 0x6e
    .long 0x64
    .long 0x0
L_str25:
    
    .long 0x36
    .long 0x38
    .long 0x2e
    .long 0x20
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x61
    .long 0x20
    .long 0x6c
    .long 0x69
    .long 0x74
    .long 0x65
    .long 0x72
    .long 0x61
    .long 0x6c
    .long 0x20
    .long 0x73
    .long 0x74
    .long 0x72
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x77
    .long 0x69
    .long 0x74
    .long 0x68
    .long 0x20
    .long 0x6e
    .long 0x6f
    .long 0x20
    .long 0x66
    .long 0x6f
    .long 0x72
    .long 0x6d
    .long 0x61
    .long 0x74
    .long 0x20
    .long 0x73
    .long 0x70
    .long 0x65
    .long 0x63
    .long 0x69
    .long 0x66
    .long 0x69
    .long 0x65
    .long 0x72
    .long 0x73
    .long 0x3a
    .long 0xa
    .long 0x0
L_str26:
    
    .long 0x21
    .long 0x20
    .long 0x20
    .long 0x20
    .long 0x54
    .long 0x68
    .long 0x69
    .long 0x73
    .long 0x20
    .long 0x69
    .long 0x73
    .long 0x20
    .long 0x6a
    .long 0x75
    .long 0x73
    .long 0x74
    .long 0x20
    .long 0x61
    .long 0x20
    .long 0x73
    .long 0x69
    .long 0x6d
    .long 0x70
    .long 0x6c
    .long 0x65
    .long 0x20
    .long 0x73
    .long 0x74
    .long 0x72
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x2e
    .long 0xa
    .long 0xa
    .long 0x0
L_str27:
    
    .long 0x23
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x20
    .long 0x57
    .long 0x52
    .long 0x49
    .long 0x54
    .long 0x45
    .long 0x46
    .long 0x20
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x20
    .long 0x53
    .long 0x75
    .long 0x69
    .long 0x74
    .long 0x65
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
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0xa
    .long 0xa
    .long 0x0
    .long 0x0, 0x0, 0x0, 0x0

.section __DATA,__data
.p2align 3
