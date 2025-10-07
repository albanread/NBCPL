.text
.global _start
_start:
    STR x0, [x1, #8]
    nop

test_label:
    nop

test_function:
    nop
    ret
