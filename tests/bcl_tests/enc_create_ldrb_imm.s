.text
.global _start
_start:
    LDRB w0, [x1]
    nop

test_label:
    nop

test_function:
    nop
    ret
