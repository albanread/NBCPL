CLASS Animal $(
    VIRTUAL ROUTINE Speak() BE WRITEF("Animal speaks*N")
$)

CLASS Dog EXTENDS Animal $(
    ROUTINE Speak() BE WRITEF("Dog speaks*N")
$)

LET START() = VALOF {
    WRITEF("=== Virtual Method Sequence Test ===*N")

    WRITEF("Step 1: Create Animal*N")
    LET animal = NEW Animal()
    animal.Speak()

    WRITEF("Step 2: Assign Animal to variable*N")
    LET var1 = animal
    WRITEF("var1 should call Animal::Speak: ")
    var1.Speak()

    WRITEF("Step 3: Create Dog*N")
    LET dog = NEW Dog()
    dog.Speak()

    WRITEF("Step 4: Test var1 again (should still be Animal)*N")
    WRITEF("var1 should still call Animal::Speak: ")
    var1.Speak()

    WRITEF("Step 5: Assign Dog to new variable*N")
    LET var2 = dog
    WRITEF("var2 should call Dog::Speak: ")
    var2.Speak()

    WRITEF("Step 6: Test var1 once more*N")
    WRITEF("var1 should STILL call Animal::Speak: ")
    var1.Speak()

    RESULTIS 0
}
