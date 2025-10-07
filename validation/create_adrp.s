.text
.global _start
_start:
    ADRP x0, test_label
    nop

test_label:
    nop

test_function:
    nop
    ret
