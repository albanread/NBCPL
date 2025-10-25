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
    WRITEF("=== VTable Bug Reproduction Test ===*N")

    // Create multiple objects to increase chances of register/temp reuse
    LET animal = NEW Animal()
    LET dog = NEW Dog()
    LET cat = NEW Cat()

    WRITEF("Direct calls (should work correctly):*N")
    animal.Speak()  // Should print "Animal speaks"
    dog.Speak()     // Should print "Dog speaks"
    cat.Speak()     // Should print "Cat speaks"

    WRITEF("*NVariable-to-variable assignments (potential bug area):*N")

    // This is the pattern that triggers the bug according to the context:
    // Variables assigned from other variables sometimes call wrong virtual method
    LET var1 = animal    // var1 should have Animal's vtable
    LET var2 = dog       // var2 should have Dog's vtable
    LET var3 = cat       // var3 should have Cat's vtable

    WRITEF("var1 (from animal): ")
    var1.Speak()    // Should print "Animal speaks" but may print wrong method

    WRITEF("var2 (from dog): ")
    var2.Speak()    // Should print "Dog speaks" but may print wrong method

    WRITEF("var3 (from cat): ")
    var3.Speak()    // Should print "Cat speaks" but may print wrong method

    // Try another level of variable assignment
    WRITEF("*NSecond-level assignments:*N")
    LET var4 = var1     // var4 = var1 = animal
    LET var5 = var2     // var5 = var2 = dog

    WRITEF("var4 (from var1/animal): ")
    var4.Speak()    // Should print "Animal speaks"

    WRITEF("var5 (from var2/dog): ")
    var5.Speak()    // Should print "Dog speaks"

    WRITEF("*NMixed assignments (stress test):*N")
    LET temp = animal
    temp := dog      // Reassign temp to dog
    WRITEF("temp (reassigned to dog): ")
    temp.Speak()    // Should print "Dog speaks"

    temp := cat      // Reassign temp to cat
    WRITEF("temp (reassigned to cat): ")
    temp.Speak()    // Should print "Cat speaks"

    WRITEF("*N=== End Test ===*N")
    RESULTIS 0
}
