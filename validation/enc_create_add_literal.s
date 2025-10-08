.text
.global _start
_start:
    ADD x0, x0, #:lo12:test_label
    nop

test_label:
    nop

test_function:
    nop
    ret
