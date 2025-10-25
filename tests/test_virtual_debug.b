CLASS Animal $(
    VIRTUAL ROUTINE Speak() BE WRITEF("Animal speaks*N")
$)

CLASS Dog EXTENDS Animal $(
    ROUTINE Speak() BE WRITEF("Dog speaks*N")
$)

LET START() = VALOF {
    WRITEF("=== Virtual Method Debug Test ===*N")

    WRITEF("Step 1: Create objects*N")
    LET animal = NEW Animal()
    LET dog = NEW Dog()

    WRITEF("Step 2: Direct method calls*N")
    WRITEF("animal.Speak() -> ")
    animal.Speak()
    WRITEF("dog.Speak() -> ")
    dog.Speak()

    WRITEF("Step 3: Variable assignment*N")
    LET var1 = animal
    LET var2 = dog

    WRITEF("Step 4: Calls through assigned variables*N")
    WRITEF("var1 (from animal).Speak() -> ")
    var1.Speak()
    WRITEF("var2 (from dog).Speak() -> ")
    var2.Speak()

    WRITEF("Step 5: Cross assignment*N")
    var1 := dog
    var2 := animal

    WRITEF("Step 6: Calls after reassignment*N")
    WRITEF("var1 (now dog).Speak() -> ")
    var1.Speak()
    WRITEF("var2 (now animal).Speak() -> ")
    var2.Speak()

    WRITEF("=== Test Complete ===*N")
    RESULTIS 0
}
