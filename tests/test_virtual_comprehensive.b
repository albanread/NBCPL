CLASS Shape $(
    VIRTUAL ROUTINE GetArea() BE WRITEF("Shape area: 0*N")
    VIRTUAL ROUTINE GetPerimeter() BE WRITEF("Shape perimeter: 0*N")
    ROUTINE GetType() BE WRITEF("I am a Shape*N")
$)

CLASS Rectangle EXTENDS Shape $(
    LET width, height

    ROUTINE CREATE(w, h) BE $(
        width := w
        height := h
        WRITEF("Rectangle created: %d x %d*N", w, h)
    $)

    ROUTINE GetArea() BE WRITEF("Rectangle area: %d*N", width * height)
    ROUTINE GetPerimeter() BE WRITEF("Rectangle perimeter: %d*N", 2 * (width + height))
$)

CLASS Circle EXTENDS Shape $(
    LET radius

    ROUTINE CREATE(r) BE $(
        radius := r
        WRITEF("Circle created with radius: %d*N", r)
    $)

    ROUTINE GetArea() BE WRITEF("Circle area: %d*N", 3 * radius * radius)
    ROUTINE GetPerimeter() BE WRITEF("Circle perimeter: %d*N", 6 * radius)
$)

CLASS Square EXTENDS Rectangle $(
    ROUTINE CREATE(side) BE $(
        width := side
        height := side
        WRITEF("Square created with side: %d*N", side)
    $)

    ROUTINE GetArea() BE WRITEF("Square area: %d*N", width * width)
$)

LET START() BE VALOF $(
    WRITEF("=== Comprehensive Virtual Method Test ===*N")
    WRITEF("*N")

    // Test 1: Basic object creation and virtual calls
    WRITEF("Test 1: Basic Virtual Method Calls*N")
    WRITEF("-----------------------------------*N")

    LET rect = NEW Rectangle()
    rect.CREATE(10, 5)
    rect.GetArea()
    rect.GetPerimeter()
    WRITEF("*N")

    LET circle = NEW Circle()
    circle.CREATE(7)
    circle.GetArea()
    circle.GetPerimeter()
    WRITEF("*N")

    LET square = NEW Square()
    square.CREATE(8)
    square.GetArea()
    square.GetPerimeter()
    WRITEF("*N")

    // Test 2: Polymorphic variable assignment
    WRITEF("Test 2: Polymorphic Behavior*N")
    WRITEF("-----------------------------*N")

    LET shape1 = rect
    LET shape2 = circle
    LET shape3 = square

    WRITEF("shape1 (assigned from rect):*N")
    shape1.GetArea()

    WRITEF("shape2 (assigned from circle):*N")
    shape2.GetArea()

    WRITEF("shape3 (assigned from square):*N")
    shape3.GetArea()
    WRITEF("*N")

    // Test 3: Non-virtual method calls
    WRITEF("Test 3: Non-Virtual Method Calls*N")
    WRITEF("---------------------------------*N")

    shape1.GetType()
    shape2.GetType()
    shape3.GetType()
    WRITEF("*N")

    // Test 4: Multiple levels of inheritance
    WRITEF("Test 4: Multiple Inheritance Levels*N")
    WRITEF("-----------------------------------*N")

    LET base_shape = square
    WRITEF("Square as base Shape:*N")
    base_shape.GetArea()      // Should call Square::GetArea (virtual)
    base_shape.GetType()      // Should call Shape::GetType (non-virtual)
    WRITEF("*N")

    // Test 5: Virtual method calls on different object types
    WRITEF("Test 5: Mixed Object Type Calls*N")
    WRITEF("-------------------------------*N")

    rect.GetArea()
    circle.GetArea()
    square.GetArea()
    WRITEF("*N")

    WRITEF("=== All Virtual Method Tests Completed Successfully! ===*N")
    RESULTIS 0
$)
