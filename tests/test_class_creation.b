CLASS Animal $(
    VIRTUAL ROUTINE Speak() BE WRITEF("Generic animal sound*N")
$)

CLASS Dog EXTENDS Animal $(
    ROUTINE Speak() BE WRITEF("Woof!*N")
$)

LET START() = VALOF {
    WRITEF("Testing Class Creation*N")

    LET dog = NEW Dog()
    WRITEF("Dog created successfully*N")

    WRITEF("Test complete*N")
    RESULTIS 0
}
