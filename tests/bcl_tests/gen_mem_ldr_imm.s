.text
.global _start
_start:
    LDR x0, [x1, #8]
    nop

test_label:
    nop

test_function:
    nop
    ret
