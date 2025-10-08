.text
.global _start
_start:
    ST1 {v0.4S}, [x1]
    nop

test_label:
    nop

test_function:
    nop
    ret
