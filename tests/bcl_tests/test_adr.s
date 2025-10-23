.globl _start

.section __TEXT,__text
_start:
    // Test basic ADR to code label
    ADR X1, code_label

    // Test ADR with arithmetic to code label
    ADR X2, code_label+8

    // Test ADR with parentheses to code label
    ADR X3, (code_label + 8)

    // Jump over the data
    B end_test

code_label:
    .quad 0x1234567890ABCDEF

end_test:
    // Exit
    MOV X0, #0
    MOV X16, #1
    SVC #0x80
