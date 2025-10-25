CLASS Animal $(
    VIRTUAL ROUTINE Speak() BE WRITEF("Animal speaks*N")
$)

CLASS Dog EXTENDS Animal $(
    ROUTINE Speak() BE WRITEF("Dog speaks*N")
$)

LET START() = VALOF {
    WRITEF("=== Simple Virtual Fix Test ===*N")

    LET dog = NEW Dog()

    WRITEF("Direct call: ")
    dog.Speak()

    LET var1 = dog
    WRITEF("Through variable: ")
    var1.Speak()

    RESULTIS 0
}
