CLASS Animal $(
    VIRTUAL ROUTINE Speak() BE WRITEF("Animal speaks*N")
$)

CLASS Dog EXTENDS Animal $(
    ROUTINE Speak() BE WRITEF("Dog speaks*N")
$)

LET START() = VALOF {
    WRITEF("=== Virtual Method Address Test ===*N")

    WRITEF("Step 1: Create Animal*N")
    LET animal = NEW Animal()
    WRITEF("Animal object created*N")
    animal.Speak()

    WRITEF("Step 2: Create Dog*N")
    LET dog = NEW Dog()
    WRITEF("Dog object created*N")
    dog.Speak()

    WRITEF("Step 3: Assign variables*N")
    LET var1 = animal
    LET var2 = dog
    WRITEF("Variables assigned*N")

    WRITEF("Step 4: Call methods through variables*N")
    WRITEF("var1.Speak() should call Animal::Speak: ")
    var1.Speak()
    WRITEF("var2.Speak() should call Dog::Speak: ")
    var2.Speak()

    WRITEF("Step 5: Call animal.Speak() again*N")
    WRITEF("animal.Speak() should still call Animal::Speak: ")
    animal.Speak()

    WRITEF("Step 6: Call dog.Speak() again*N")
    WRITEF("dog.Speak() should still call Dog::Speak: ")
    dog.Speak()

    WRITEF("=== Address Test Complete ===*N")
    RESULTIS 0
}
