	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 16, 0	sdk_version 26, 0
	.globl	_extract_lane_0                 ; -- Begin function extract_lane_0
	.p2align	2
_extract_lane_0:                        ; @extract_lane_0
	.cfi_startproc
; %bb.0:
	fmov	s0, w0
	fcvt	d0, s0
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	_extract_lane_1                 ; -- Begin function extract_lane_1
	.p2align	2
_extract_lane_1:                        ; @extract_lane_1
	.cfi_startproc
; %bb.0:
	lsr	x8, x0, #32
	fmov	s0, w8
	fcvt	d0, s0
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	_extract_lane_0_manual          ; -- Begin function extract_lane_0_manual
	.p2align	2
_extract_lane_0_manual:                 ; @extract_lane_0_manual
	.cfi_startproc
; %bb.0:
	fmov	s0, w0
	fcvt	d0, s0
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	_extract_lane_1_manual          ; -- Begin function extract_lane_1_manual
	.p2align	2
_extract_lane_1_manual:                 ; @extract_lane_1_manual
	.cfi_startproc
; %bb.0:
	lsr	x8, x0, #32
	fmov	s0, w8
	fcvt	d0, s0
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	_main                           ; -- Begin function main
	.p2align	2
_main:                                  ; @main
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #48
	stp	x20, x19, [sp, #16]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #32]             ; 16-byte Folded Spill
	add	x29, sp, #32
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
Lloh0:
	adrp	x0, l_str@PAGE
Lloh1:
	add	x0, x0, l_str@PAGEOFF
	bl	_puts
	mov	x8, #1065353216                 ; =0x3f800000
	movk	x8, #16384, lsl #48
	str	x8, [sp]
Lloh2:
	adrp	x0, l_.str.1@PAGE
Lloh3:
	add	x0, x0, l_.str.1@PAGEOFF
	bl	_printf
	mov	x19, #4607182418800017408       ; =0x3ff0000000000000
	str	x19, [sp]
Lloh4:
	adrp	x0, l_.str.2@PAGE
Lloh5:
	add	x0, x0, l_.str.2@PAGEOFF
	bl	_printf
	mov	x20, #4611686018427387904       ; =0x4000000000000000
	str	x20, [sp]
Lloh6:
	adrp	x0, l_.str.3@PAGE
Lloh7:
	add	x0, x0, l_.str.3@PAGEOFF
	bl	_printf
	str	x19, [sp]
Lloh8:
	adrp	x0, l_.str.4@PAGE
Lloh9:
	add	x0, x0, l_.str.4@PAGEOFF
	bl	_printf
	str	x20, [sp]
Lloh10:
	adrp	x0, l_.str.5@PAGE
Lloh11:
	add	x0, x0, l_.str.5@PAGEOFF
	bl	_printf
	mov	w0, #0                          ; =0x0
	ldp	x29, x30, [sp, #32]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #16]             ; 16-byte Folded Reload
	add	sp, sp, #48
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
l_.str.1:                               ; @.str.1
	.asciz	"FPAIR bits: 0x%016llX\n"

l_.str.2:                               ; @.str.2
	.asciz	"NEON lane 0: %f\n"

l_.str.3:                               ; @.str.3
	.asciz	"NEON lane 1: %f\n"

l_.str.4:                               ; @.str.4
	.asciz	"Manual lane 0: %f\n"

l_.str.5:                               ; @.str.5
	.asciz	"Manual lane 1: %f\n"

l_str:                                  ; @str
	.asciz	"Testing NEON lane extraction vs manual extraction"

.subsections_via_symbols
