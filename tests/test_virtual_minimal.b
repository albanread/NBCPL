CLASS Animal $(
    VIRTUAL ROUTINE Speak() BE WRITEF("Animal speaks*N")
$)

CLASS Dog EXTENDS Animal $(
    ROUTINE Speak() BE WRITEF("Dog speaks*N")
$)

LET START() = VALOF {
    WRITEF("=== Minimal Virtual Test ===*N")

    LET animal = NEW Animal()
    LET dog = NEW Dog()

    WRITEF("Direct calls:*N")
    animal.Speak()
    dog.Speak()

    WRITEF("Variable assignment:*N")
    LET var1 = animal
    var1.Speak()

    WRITEF("Should print 'Animal speaks' but if bug exists will print 'Dog speaks'*N")

    RESULTIS 0
}
