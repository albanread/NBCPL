CLASS Animal $(
    VIRTUAL ROUTINE Speak() BE WRITEF("Animal speaks*N")
$)

CLASS Dog EXTENDS Animal $(
    ROUTINE Speak() BE WRITEF("Dog speaks*N")
$)

LET START() = VALOF {
    WRITEF("=== Virtual Method Order Test ===*N")

    WRITEF("Test 1: Create Animal first, then Dog*N")
    LET animal1 = NEW Animal()
    LET dog1 = NEW Dog()

    LET var_a1 = animal1
    LET var_d1 = dog1

    WRITEF("var_a1 (from animal1): ")
    var_a1.Speak()
    WRITEF("var_d1 (from dog1): ")
    var_d1.Speak()
    WRITEF("*N")

    WRITEF("Test 2: Create Dog first, then Animal*N")
    LET dog2 = NEW Dog()
    LET animal2 = NEW Animal()

    LET var_d2 = dog2
    LET var_a2 = animal2

    WRITEF("var_d2 (from dog2): ")
    var_d2.Speak()
    WRITEF("var_a2 (from animal2): ")
    var_a2.Speak()
    WRITEF("*N")

    WRITEF("Test 3: Interleaved assignments*N")
    LET animal3 = NEW Animal()
    LET var_a3 = animal3
    LET dog3 = NEW Dog()
    LET var_d3 = dog3

    WRITEF("var_a3 (from animal3): ")
    var_a3.Speak()
    WRITEF("var_d3 (from dog3): ")
    var_d3.Speak()
    WRITEF("*N")

    WRITEF("=== Test Complete ===*N")
    RESULTIS 0
}
