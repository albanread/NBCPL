CLASS Animal $(
    VIRTUAL ROUTINE Speak() BE WRITEF("Generic animal sound*N")
    VIRTUAL ROUTINE Move() BE WRITEF("Animal moves*N")
    ROUTINE GetSpecies() BE WRITEF("Unknown species*N")
$)

CLASS Dog EXTENDS Animal $(
    ROUTINE Speak() BE WRITEF("Woof! Woof!*N")
    ROUTINE Move() BE WRITEF("Dog runs*N")
    ROUTINE GetSpecies() BE WRITEF("Canine*N")
$)

CLASS Cat EXTENDS Animal $(
    ROUTINE Speak() BE WRITEF("Meow! Meow!*N")
    ROUTINE Move() BE WRITEF("Cat prowls*N")
    ROUTINE GetSpecies() BE WRITEF("Feline*N")
$)

CLASS Bird EXTENDS Animal $(
    ROUTINE Speak() BE WRITEF("Tweet! Tweet!*N")
    ROUTINE Move() BE WRITEF("Bird flies*N")
    ROUTINE GetSpecies() BE WRITEF("Avian*N")
$)

LET START() = VALOF {
    WRITEF("=== Virtual Method Simple Stress Test ===*N*N")

    // Test 1: Create multiple objects
    WRITEF("Test 1: Creating objects*N")
    LET dog = NEW Dog()
    LET cat = NEW Cat()
    LET bird = NEW Bird()
    LET generic_animal = NEW Animal()

    WRITEF("Direct calls:*N")
    dog.Speak()
    cat.Speak()
    bird.Speak()
    generic_animal.Speak()

    WRITEF("*N")

    // Test 2: Polymorphic variables
    WRITEF("Test 2: Polymorphic variables*N")
    LET animal1 = dog
    LET animal2 = cat
    LET animal3 = bird

    WRITEF("Polymorphic calls:*N")
    animal1.Speak()    // Should call Dog::Speak
    animal2.Speak()    // Should call Cat::Speak
    animal3.Speak()    // Should call Bird::Speak

    WRITEF("*N")

    // Test 3: Variable reassignment
    WRITEF("Test 3: Variable reassignment*N")
    LET shape_shifter = dog
    WRITEF("Starting as dog: ")
    shape_shifter.Speak()

    shape_shifter := cat
    WRITEF("Changed to cat: ")
    shape_shifter.Speak()

    shape_shifter := bird
    WRITEF("Changed to bird: ")
    shape_shifter.Speak()

    shape_shifter := generic_animal
    WRITEF("Changed to generic animal: ")
    shape_shifter.Speak()

    WRITEF("*N")

    // Test 4: Chain assignments
    WRITEF("Test 4: Chain assignments*N")
    LET var1 = dog
    LET var2 = var1
    LET var3 = var2
    LET var4 = var3

    WRITEF("Original dog: ")
    dog.Speak()
    WRITEF("var1 (from dog): ")
    var1.Speak()
    WRITEF("var2 (from var1): ")
    var2.Speak()
    WRITEF("var3 (from var2): ")
    var3.Speak()
    WRITEF("var4 (from var3): ")
    var4.Speak()

    WRITEF("*N")

    // Test 5: Multiple virtual methods
    WRITEF("Test 5: Multiple virtual methods per class*N")
    LET animal_ptr = dog
    WRITEF("Dog as Animal:*N")
    animal_ptr.Speak()    // Virtual
    animal_ptr.Move()     // Virtual
    animal_ptr.GetSpecies()  // Non-virtual (should call Animal version)

    animal_ptr := cat
    WRITEF("Cat as Animal:*N")
    animal_ptr.Speak()    // Virtual
    animal_ptr.Move()     // Virtual
    animal_ptr.GetSpecies()  // Non-virtual (should call Animal version)

    WRITEF("*N")

    // Test 6: Rapid switching
    WRITEF("Test 6: Rapid switching*N")
    LET switcher = dog
    FOR i = 1 TO 10 DO $(
        IF i REM 3 = 0 THEN switcher := dog
        IF i REM 3 = 1 THEN switcher := cat
        IF i REM 3 = 2 THEN switcher := bird

        WRITEF("Switch *D: ", i)
        switcher.Speak()
    $)

    WRITEF("*N")

    // Test 7: Array of polymorphic objects
    WRITEF("Test 7: Array of polymorphic objects*N")
    LET animals = GETVEC(10)

    animals!0 := dog
    animals!1 := cat
    animals!2 := bird
    animals!3 := dog
    animals!4 := cat

    FOR i = 0 TO 4 DO $(
        WRITEF("Animal *D: ", i)
        animals!i.Speak()
    $)

    WRITEF("*NTesting movement:*N")
    FOR i = 0 TO 4 DO $(
        WRITEF("Animal *D: ", i)
        animals!i.Move()
    $)

    FREEVEC(animals)

    WRITEF("*N=== All stress tests completed successfully! ===*N")
    RESULTIS 0
}
