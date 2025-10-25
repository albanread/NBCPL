CLASS Animal $(
    VIRTUAL ROUTINE Speak() BE WRITEF("Generic animal sound*N")
    ROUTINE Move() BE WRITEF("Animal moves*N")
$)

CLASS Dog EXTENDS Animal $(
    ROUTINE Speak() BE WRITEF("Woof!*N")
$)

CLASS Cat EXTENDS Animal $(
    ROUTINE Speak() BE WRITEF("Meow!*N")
$)

LET START() = VALOF {
    WRITEF("Testing Virtual Method Dispatch*N")
    WRITEF("==============================*N")

    LET dog = NEW Dog()
    LET cat = NEW Cat()

    WRITEF("Testing virtual method calls:*N")
    dog.Speak()    // Should print "Woof!"
    cat.Speak()    // Should print "Meow!"

    WRITEF("Testing non-virtual method calls:*N")
    dog.Move()     // Should print "Animal moves"
    cat.Move()     // Should print "Animal moves"

    WRITEF("Testing polymorphic behavior:*N")
    LET animal1 = dog
    LET animal2 = cat

    WRITES(TYPE(animal1))
    WRITES(" : ")
    WRITES(TYPE(animal2))

    // Both should call the correct virtual method
    animal1.Speak()  // Should print "Woof!"
    animal2.Speak()  // Should print "Meow!"

    WRITEF("All tests completed successfully!*N")
    RESULTIS 0
}
