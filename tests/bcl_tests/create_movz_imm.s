.text
.global _start
_start:
    MOVZ x0, #4660, LSL #16
    nop

test_label:
    nop

test_function:
    nop
    ret
