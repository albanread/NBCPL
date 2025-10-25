CLASS Base $(
    VIRTUAL ROUTINE Speak() BE WRITEF("Base speaks*N")
    ROUTINE Move() BE WRITEF("Base moves*N")
$)

CLASS Derived EXTENDS Base $(
    ROUTINE Speak() BE WRITEF("Derived speaks*N")
$)

LET START() = VALOF {
    WRITEF("Testing Simple Virtual Inheritance*N")

    LET base_obj = NEW Base()
    LET derived_obj = NEW Derived()

    WRITEF("Direct calls:*N")
    base_obj.Speak()
    derived_obj.Speak()

    WRITEF("Polymorphic calls:*N")
    LET poly1 = base_obj
    LET poly2 = derived_obj

    poly1.Speak()
    poly2.Speak()

    WRITEF("Non-virtual calls:*N")
    poly1.Move()
    poly2.Move()

    WRITEF("Test completed successfully*N")
    RESULTIS 0
}
