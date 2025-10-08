.text
.global _start
_start:
    MOVK x0, #4660, LSL #16
    nop

test_label:
    nop

test_function:
    nop
    ret
