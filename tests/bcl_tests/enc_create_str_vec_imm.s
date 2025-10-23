.text
.global _start
_start:
    STR q0, [x1, #0]
    nop

test_label:
    nop

test_function:
    nop
    ret
