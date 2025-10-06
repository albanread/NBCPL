.text
.global _start
_start:
    STP x0, x1, [x2, #16]
    nop

test_label:
    nop

test_function:
    nop
    ret
