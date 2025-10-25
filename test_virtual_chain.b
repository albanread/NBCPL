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
    WRITEF("=== Virtual Method Chain Assignment Test ===*N*N")

    // Create original objects
    LET dog = NEW Dog()
    LET cat = NEW Cat()
    LET bird = NEW Bird()

    WRITEF("Test 1: Direct object calls*N")
    WRITEF("Dog: ")
    dog.Speak()
    WRITEF("Cat: ")
    cat.Speak()
    WRITEF("Bird: ")
    bird.Speak()
    WRITEF("*N")

    // Test single-level assignment
    WRITEF("Test 2: Single-level assignment*N")
    LET animal1 = dog
    LET animal2 = cat
    LET animal3 = bird

    WRITEF("animal1 (from dog): ")
    animal1.Speak()
    WRITEF("animal2 (from cat): ")
    animal2.Speak()
    WRITEF("animal3 (from bird): ")
    animal3.Speak()
    WRITEF("*N")

    // Test chain assignments
    WRITEF("Test 3: Chain assignments (dog -> var1 -> var2 -> var3)*N")
    LET var1 = dog
    LET var2 = var1
    LET var3 = var2
    LET var4 = var3

    WRITEF("Original dog: ")
    dog.Speak()
    WRITEF("var1: ")
    var1.Speak()
    WRITEF("var2: ")
    var2.Speak()
    WRITEF("var3: ")
    var3.Speak()
    WRITEF("var4: ")
    var4.Speak()
    WRITEF("*N")

    // Test variable reassignment
    WRITEF("Test 4: Variable reassignment*N")
    LET morphing = dog
    WRITEF("Starting as dog: ")
    morphing.Speak()

    morphing := cat
    WRITEF("Changed to cat: ")
    morphing.Speak()

    morphing := bird
    WRITEF("Changed to bird: ")
    morphing.Speak()

    morphing := dog
    WRITEF("Back to dog: ")
    morphing.Speak()
    WRITEF("*N")

    // Test multiple virtual methods
    WRITEF("Test 5: Multiple virtual methods*N")
    LET tester = dog
    WRITEF("Dog methods: ")
    tester.Speak()
    WRITEF("Dog moves: ")
    tester.Move()

    tester := cat
    WRITEF("Cat methods: ")
    tester.Speak()
    WRITEF("Cat moves: ")
    tester.Move()

    tester := bird
    WRITEF("Bird methods: ")
    tester.Speak()
    WRITEF("Bird moves: ")
    tester.Move()
    WRITEF("*N")

    // Test rapid switching
    WRITEF("Test 6: Rapid switching*N")
    LET switcher = dog

    switcher := dog
    WRITEF("Switch 1: ")
    switcher.Speak()

    switcher := cat
    WRITEF("Switch 2: ")
    switcher.Speak()

    switcher := bird
    WRITEF("Switch 3: ")
    switcher.Speak()

    switcher := dog
    WRITEF("Switch 4: ")
    switcher.Speak()

    switcher := cat
    WRITEF("Switch 5: ")
    switcher.Speak()
    WRITEF("*N")

    // Test cross-assignments
    WRITEF("Test 7: Cross-assignments*N")
    LET cross1 = dog
    LET cross2 = cat
    LET cross3 = bird

    WRITEF("Before swap:*N")
    WRITEF("cross1: ")
    cross1.Speak()
    WRITEF("cross2: ")
    cross2.Speak()
    WRITEF("cross3: ")
    cross3.Speak()

    // Swap them around
    LET temp = cross1
    cross1 := cross2
    cross2 := cross3
    cross3 := temp

    WRITEF("After swap:*N")
    WRITEF("cross1: ")
    cross1.Speak()
    WRITEF("cross2: ")
    cross2.Speak()
    WRITEF("cross3: ")
    cross3.Speak()
    WRITEF("*N")

    WRITEF("=== All chain assignment tests completed successfully! ===*N")
    RESULTIS 0
}
