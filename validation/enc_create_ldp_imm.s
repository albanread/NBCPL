.text
.global _start
_start:
    LDP x0, x1, [x2, #0]
    nop

test_label:
    nop

test_function:
    nop
    ret
