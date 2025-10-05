	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 16, 0	sdk_version 26, 0
	.globl	_make_fpair                     ; -- Begin function make_fpair
	.p2align	2
_make_fpair:                            ; @make_fpair
	.cfi_startproc
; %bb.0:
	fmov	w8, s0
	fmov	w9, s1
	orr	x0, x8, x9, lsl #32
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	_fpair_lane0                    ; -- Begin function fpair_lane0
	.p2align	2
_fpair_lane0:                           ; @fpair_lane0
	.cfi_startproc
; %bb.0:
	fmov	s0, w0
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	_fpair_lane1                    ; -- Begin function fpair_lane1
	.p2align	2
_fpair_lane1:                           ; @fpair_lane1
	.cfi_startproc
; %bb.0:
	lsr	x8, x0, #32
	fmov	s0, w8
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	_main                           ; -- Begin function main
	.p2align	2
_main:                                  ; @main
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #64
	stp	x22, x21, [sp, #16]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #32]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #48]             ; 16-byte Folded Spill
	add	x29, sp, #48
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
Lloh0:
	adrp	x0, l_str@PAGE
Lloh1:
	add	x0, x0, l_str@PAGEOFF
	bl	_puts
Lloh2:
	adrp	x0, l_str.6@PAGE
Lloh3:
	add	x0, x0, l_str.6@PAGEOFF
	bl	_puts
	mov	x8, #1065353216                 ; =0x3f800000
	movk	x8, #16384, lsl #48
	str	x8, [sp]
Lloh4:
	adrp	x19, l_.str.2@PAGE
Lloh5:
	add	x19, x19, l_.str.2@PAGEOFF
	mov	x0, x19
	bl	_printf
	mov	x8, #4607182418800017408        ; =0x3ff0000000000000
	str	x8, [sp]
Lloh6:
	adrp	x20, l_.str.3@PAGE
Lloh7:
	add	x20, x20, l_.str.3@PAGEOFF
	mov	x0, x20
	bl	_printf
	mov	x8, #4611686018427387904        ; =0x4000000000000000
	str	x8, [sp]
Lloh8:
	adrp	x21, l_.str.4@PAGE
Lloh9:
	add	x21, x21, l_.str.4@PAGEOFF
	mov	x0, x21
	bl	_printf
Lloh10:
	adrp	x0, l_str.7@PAGE
Lloh11:
	add	x0, x0, l_str.7@PAGEOFF
	bl	_puts
	mov	x8, #1109917696                 ; =0x42280000
	movk	x8, #17094, lsl #48
	str	x8, [sp]
	mov	x0, x19
	bl	_printf
	mov	x8, #4631107791820423168        ; =0x4045000000000000
	str	x8, [sp]
	mov	x0, x20
	bl	_printf
	mov	x8, #211106232532992            ; =0xc00000000000
	movk	x8, #16472, lsl #48
	str	x8, [sp]
	mov	x0, x21
	bl	_printf
	mov	w0, #0                          ; =0x0
	ldp	x29, x30, [sp, #48]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #32]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]             ; 16-byte Folded Reload
	add	sp, sp, #64
	ret
	.loh AdrpAdd	Lloh10, Lloh11
	.loh AdrpAdd	Lloh8, Lloh9
	.loh AdrpAdd	Lloh6, Lloh7
	.loh AdrpAdd	Lloh4, Lloh5
	.loh AdrpAdd	Lloh2, Lloh3
	.loh AdrpAdd	Lloh0, Lloh1
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__cstring,cstring_literals
l_.str.2:                               ; @.str.2
	.asciz	"Bit pattern: 0x%016lX\n"

l_.str.3:                               ; @.str.3
	.asciz	"Lane 0: %f\n"

l_.str.4:                               ; @.str.4
	.asciz	"Lane 1: %f\n"

l_str:                                  ; @str
	.asciz	"Testing FPAIR-like operations with clang"

l_str.6:                                ; @str.6
	.asciz	"Created fpair(1.0, 2.0)"

l_str.7:                                ; @str.7
	.asciz	"\nCreated fpair(42.0, 99.0)"

.subsections_via_symbols
