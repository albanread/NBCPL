.text
.global _start
_start:
    LDP x0, x1, [x2, #16]!
    nop

test_label:
    nop

test_function:
    nop
    ret
