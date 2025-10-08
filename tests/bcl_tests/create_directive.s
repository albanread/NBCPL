.text
.global _start
_start:
    .quad 0x123456789abcdef0 ; test_data
    nop

test_label:
    nop

test_function:
    nop
    ret
