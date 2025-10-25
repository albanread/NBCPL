CLASS Animal $(
    VIRTUAL ROUTINE Speak() BE WRITEF("Animal speaks*N")
$)

CLASS Dog EXTENDS Animal $(
    ROUTINE Speak() BE WRITEF("Dog speaks*N")
$)

CLASS Cat EXTENDS Animal $(
    ROUTINE Speak() BE WRITEF("Cat speaks*N")
$)

LET START() = VALOF {
    WRITEF("=== Register Debug Test ===*N")

    // Create objects
    LET dog = NEW Dog()
    LET cat = NEW Cat()

    WRITEF("Initial direct calls:*N")
    dog.Speak()
    cat.Speak()

    WRITEF("*NFirst assignment:*N")
    LET var1 = dog
    var1.Speak()

    WRITEF("*NSecond assignment:*N")
    LET var2 = cat
    var2.Speak()

    WRITEF("*NReassignment test:*N")
    var1 := cat
    WRITEF("var1 after reassigning to cat: ")
    var1.Speak()

    WRITEF("*NOriginal objects after reassignment:*N")
    WRITEF("dog: ")
    dog.Speak()
    WRITEF("cat: ")
    cat.Speak()

    RESULTIS 0
}
