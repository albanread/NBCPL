	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 16, 0	sdk_version 26, 0
	.globl	_extract_and_convert            ; -- Begin function extract_and_convert
	.p2align	2
_extract_and_convert:                   ; @extract_and_convert
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	str	x0, [sp, #24]
	str	w1, [sp, #20]
	ldr	w8, [sp, #20]
	cbnz	w8, LBB0_2
	b	LBB0_1
LBB0_1:
	ldr	w8, [sp, #24]
                                        ; kill: def $x8 killed $w8
                                        ; kill: def $w8 killed $w8 killed $x8
	str	w8, [sp, #16]
	b	LBB0_3
LBB0_2:
	ldr	x8, [sp, #24]
	lsr	x8, x8, #32
                                        ; kill: def $w8 killed $w8 killed $x8
	str	w8, [sp, #16]
	b	LBB0_3
LBB0_3:
	ldr	s0, [sp, #16]
	str	s0, [sp, #12]
	ldr	s0, [sp, #12]
	fcvt	d0, s0
	add	sp, sp, #32
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	_main                           ; -- Begin function main
	.p2align	2
_main:                                  ; @main
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #128
	stp	x29, x30, [sp, #112]            ; 16-byte Folded Spill
	add	x29, sp, #112
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	mov	w8, #0                          ; =0x0
	str	w8, [sp, #36]                   ; 4-byte Folded Spill
	stur	wzr, [x29, #-4]
	adrp	x0, l_.str@PAGE
	add	x0, x0, l_.str@PAGEOFF
	bl	_printf
	mov	w8, #1065353216                 ; =0x3f800000
	stur	w8, [x29, #-8]
	mov	w8, #1073741824                 ; =0x40000000
	stur	w8, [x29, #-12]
	ldur	w8, [x29, #-12]
	mov	x9, x8
	ldur	w8, [x29, #-8]
                                        ; kill: def $x8 killed $w8
	orr	x8, x8, x9, lsl #32
	stur	x8, [x29, #-24]
	ldur	x8, [x29, #-24]
	mov	x9, sp
	str	x8, [x9]
	adrp	x0, l_.str.1@PAGE
	add	x0, x0, l_.str.1@PAGEOFF
	bl	_printf
	ldr	w1, [sp, #36]                   ; 4-byte Folded Reload
	ldur	x0, [x29, #-24]
	bl	_extract_and_convert
	stur	d0, [x29, #-32]
	ldur	x0, [x29, #-24]
	mov	w1, #1                          ; =0x1
	str	w1, [sp, #12]                   ; 4-byte Folded Spill
	bl	_extract_and_convert
	stur	d0, [x29, #-40]
	ldur	d0, [x29, #-32]
	mov	x8, sp
	str	d0, [x8]
	adrp	x0, l_.str.2@PAGE
	add	x0, x0, l_.str.2@PAGEOFF
	str	x0, [sp, #16]                   ; 8-byte Folded Spill
	bl	_printf
	ldur	d0, [x29, #-40]
	mov	x8, sp
	str	d0, [x8]
	adrp	x0, l_.str.3@PAGE
	add	x0, x0, l_.str.3@PAGEOFF
	str	x0, [sp, #24]                   ; 8-byte Folded Spill
	bl	_printf
	mov	w8, #4048                       ; =0xfd0
	movk	w8, #16457, lsl #16
	stur	w8, [x29, #-44]
	mov	w8, #63572                      ; =0xf854
	movk	w8, #16429, lsl #16
	stur	w8, [x29, #-48]
	ldur	w8, [x29, #-48]
	mov	x9, x8
	ldur	w8, [x29, #-44]
                                        ; kill: def $x8 killed $w8
	orr	x8, x8, x9, lsl #32
	str	x8, [sp, #56]
	ldr	x8, [sp, #56]
	mov	x9, sp
	str	x8, [x9]
	adrp	x0, l_.str.4@PAGE
	add	x0, x0, l_.str.4@PAGEOFF
	bl	_printf
	ldr	w1, [sp, #36]                   ; 4-byte Folded Reload
	ldr	x0, [sp, #56]
	bl	_extract_and_convert
	ldr	w1, [sp, #12]                   ; 4-byte Folded Reload
	str	d0, [sp, #48]
	ldr	x0, [sp, #56]
	bl	_extract_and_convert
	ldr	x0, [sp, #16]                   ; 8-byte Folded Reload
	str	d0, [sp, #40]
	ldr	d0, [sp, #48]
	mov	x8, sp
	str	d0, [x8]
	bl	_printf
	ldr	x0, [sp, #24]                   ; 8-byte Folded Reload
	ldr	d0, [sp, #40]
	mov	x8, sp
	str	d0, [x8]
	bl	_printf
	ldr	w0, [sp, #36]                   ; 4-byte Folded Reload
	ldp	x29, x30, [sp, #112]            ; 16-byte Folded Reload
	add	sp, sp, #128
	ret
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__cstring,cstring_literals
l_.str:                                 ; @.str
	.asciz	"Testing FCVT S->D encoding with clang\n"

l_.str.1:                               ; @.str.1
	.asciz	"FPAIR bits: 0x%016llX\n"

l_.str.2:                               ; @.str.2
	.asciz	"Lane 0: %f\n"

l_.str.3:                               ; @.str.3
	.asciz	"Lane 1: %f\n"

l_.str.4:                               ; @.str.4
	.asciz	"\nFPAIR2 bits: 0x%016llX\n"

.subsections_via_symbols
