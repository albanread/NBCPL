CLASS Animal $(
    VIRTUAL ROUTINE Speak() BE WRITEF("Animal speaks*N")
$)

CLASS Dog EXTENDS Animal $(
    ROUTINE Speak() BE WRITEF("Woof!*N")
$)

LET START() = VALOF {
    WRITEF("Testing Virtual Methods*N")

    LET dog = NEW Dog()
    dog.Speak()

    WRITEF("Test complete*N")
    RESULTIS 0
}
