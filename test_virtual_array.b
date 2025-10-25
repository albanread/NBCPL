CLASS Animal $(
    VIRTUAL ROUTINE Speak() BE WRITEF("Generic animal sound*N")
    VIRTUAL ROUTINE Move() BE WRITEF("Animal moves*N")
$)

CLASS Dog EXTENDS Animal $(
    ROUTINE Speak() BE WRITEF("Woof!*N")
    ROUTINE Move() BE WRITEF("Dog runs*N")
$)

CLASS Cat EXTENDS Animal $(
    ROUTINE Speak() BE WRITEF("Meow!*N")
    ROUTINE Move() BE WRITEF("Cat prowls*N")
$)

CLASS Bird EXTENDS Animal $(
    ROUTINE Speak() BE WRITEF("Tweet!*N")
    ROUTINE Move() BE WRITEF("Bird flies*N")
$)

LET START() = VALOF {
    WRITEF("=== Virtual Method Array Test ===*N*N")

    // Create objects
    LET dog = NEW Dog()
    LET cat = NEW Cat()
    LET bird = NEW Bird()

    WRITEF("Test 1: Direct calls*N")
    dog.Speak()
    cat.Speak()
    bird.Speak()
    WRITEF("*N")

    // Test polymorphic variables
    WRITEF("Test 2: Polymorphic variables*N")
    LET animal1 = dog
    LET animal2 = cat
    LET animal3 = bird

    animal1.Speak()
    animal2.Speak()
    animal3.Speak()
    WRITEF("*N")

    // Test variable reassignment
    WRITEF("Test 3: Variable reassignment*N")
    LET shape_shifter = dog
    WRITEF("As dog: ")
    shape_shifter.Speak()

    shape_shifter := cat
    WRITEF("As cat: ")
    shape_shifter.Speak()

    shape_shifter := bird
    WRITEF("As bird: ")
    shape_shifter.Speak()
    WRITEF("*N")

    // Test array of polymorphic objects
    WRITEF("Test 4: Array polymorphism*N")
    LET animals = GETVEC(5)

    animals!0 := dog
    animals!1 := cat
    animals!2 := bird
    animals!3 := dog
    animals!4 := cat

    WRITEF("Speaking:*N")
    animals!0.Speak()
    animals!1.Speak()
    animals!2.Speak()
    animals!3.Speak()
    animals!4.Speak()

    WRITEF("Moving:*N")
    animals!0.Move()
    animals!1.Move()
    animals!2.Move()
    animals!3.Move()
    animals!4.Move()

    FREEVEC(animals)

    // Test chain assignments
    WRITEF("*NTest 5: Chain assignments*N")
    LET var1 = dog
    LET var2 = var1
    LET var3 = var2
    LET var4 = var3

    WRITEF("Chain calls (all should be dog):*N")
    var1.Speak()
    var2.Speak()
    var3.Speak()
    var4.Speak()

    WRITEF("*N=== All tests completed ===*N")
    RESULTIS 0
}
