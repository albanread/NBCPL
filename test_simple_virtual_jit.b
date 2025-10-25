CLASS Animal $(
    VIRTUAL ROUTINE Speak() BE WRITEF("Animal speaks*N")
$)

CLASS Dog EXTENDS Animal $(
    ROUTINE Speak() BE WRITEF("Dog speaks*N")
$)

LET START() = VALOF {
    WRITEF("Simple Virtual JIT Test*N")

    LET dog = NEW Dog()
    dog.Speak()

    WRITEF("Test complete*N")
    RESULTIS 0
}
