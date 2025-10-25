CLASS BaseShape $(
    VIRTUAL ROUTINE GetArea() = VALOF RESULTIS 0
    VIRTUAL ROUTINE GetPerimeter() = VALOF RESULTIS 0
    VIRTUAL ROUTINE Draw() BE WRITEF("Drawing base shape*N")
    ROUTINE GetType() BE WRITEF("Base Shape*N")
$)

CLASS Rectangle EXTENDS BaseShape $(
    LET width = 0
    LET height = 0

    ROUTINE SetDimensions(w, h) BE $(
        width := w
        height := h
    $)

    ROUTINE GetArea() = VALOF RESULTIS width * height
    ROUTINE GetPerimeter() = VALOF RESULTIS 2 * (width + height)
    ROUTINE Draw() BE WRITEF("Drawing rectangle %Dx%D*N", width, height)
$)

CLASS Circle EXTENDS BaseShape $(
    LET radius = 0

    ROUTINE SetRadius(r) BE radius := r
    ROUTINE GetArea() = VALOF RESULTIS 3 * radius * radius  // Simplified π ≈ 3
    ROUTINE GetPerimeter() = VALOF RESULTIS 6 * radius      // Simplified 2π ≈ 6
    ROUTINE Draw() BE WRITEF("Drawing circle with radius %D*N", radius)
$)

CLASS Square EXTENDS Rectangle $(
    ROUTINE SetSide(s) BE SetDimensions(s, s)
    ROUTINE Draw() BE WRITEF("Drawing square %Dx%D*N", width, width)
$)

LET START() = VALOF {
    WRITEF("=== Virtual Method Stress Test ===*N*N")

    // Test 1: Basic polymorphism with arrays
    WRITEF("Test 1: Polymorphic array operations*N")
    LET shapes = GETVEC(10)

    shapes!0 := NEW Rectangle()
    shapes!1 := NEW Circle()
    shapes!2 := NEW Square()
    shapes!3 := NEW Rectangle()
    shapes!4 := NEW Circle()

    // Initialize shapes with different values
    LET rect1 = shapes!0
    rect1.SetDimensions(5, 3)

    LET circle1 = shapes!1
    circle1.SetRadius(4)

    LET square1 = shapes!2
    square1.SetSide(6)

    LET rect2 = shapes!3
    rect2.SetDimensions(2, 8)

    LET circle2 = shapes!4
    circle2.SetRadius(3)

    // Test virtual method calls through array elements
    FOR i = 0 TO 4 DO $(
        LET shape = shapes!i
        WRITEF("Shape %D: ", i)
        shape.Draw()
        WRITEF("  Area: %D, Perimeter: %D*N", shape.GetArea(), shape.GetPerimeter())
    $)

    WRITEF("*N")

    // Test 2: Deep variable assignment chains
    WRITEF("Test 2: Deep assignment chains*N")
    LET var1 = shapes!0  // Rectangle
    LET var2 = var1      // Should still be Rectangle
    LET var3 = var2      // Should still be Rectangle
    LET var4 = var3      // Should still be Rectangle

    WRITEF("Original rectangle: ")
    shapes!0.Draw()
    WRITEF("var1 (from shapes!0): ")
    var1.Draw()
    WRITEF("var2 (from var1): ")
    var2.Draw()
    WRITEF("var3 (from var2): ")
    var3.Draw()
    WRITEF("var4 (from var3): ")
    var4.Draw()

    WRITEF("*N")

    // Test 3: Reassignment stress test
    WRITEF("Test 3: Variable reassignment stress*N")
    LET morphing_shape = shapes!0  // Start as Rectangle
    WRITEF("Starting as: ")
    morphing_shape.Draw()

    morphing_shape := shapes!1     // Change to Circle
    WRITEF("Changed to: ")
    morphing_shape.Draw()

    morphing_shape := shapes!2     // Change to Square
    WRITEF("Changed to: ")
    morphing_shape.Draw()

    morphing_shape := shapes!3     // Change to Rectangle again
    WRITEF("Changed to: ")
    morphing_shape.Draw()

    WRITEF("*N")

    // Test 4: Multiple variables pointing to same object
    WRITEF("Test 4: Multiple variables, same object*N")
    LET alias1 = shapes!1  // Circle
    LET alias2 = shapes!1  // Same Circle
    LET alias3 = alias1    // Also same Circle

    WRITEF("Original circle: ")
    shapes!1.Draw()
    WRITEF("alias1: ")
    alias1.Draw()
    WRITEF("alias2: ")
    alias2.Draw()
    WRITEF("alias3: ")
    alias3.Draw()

    WRITEF("*N")

    // Test 5: Mixed virtual and non-virtual calls
    WRITEF("Test 5: Mixed virtual/non-virtual calls*N")
    FOR i = 0 TO 2 DO $(
        LET shape = shapes!i
        WRITEF("Shape %D:*N", i)
        shape.GetType()     // Non-virtual (should always call BaseShape)
        shape.Draw()        // Virtual (should call correct derived class)
        WRITEF("*N")
    $)

    // Test 6: Inheritance chain virtual calls
    WRITEF("Test 6: Inheritance chain (Square -> Rectangle -> BaseShape)*N")
    LET square_as_base = shapes!2      // Square as BaseShape
    LET square_as_rect = shapes!2      // Square as Rectangle

    WRITEF("Square called as BaseShape: ")
    square_as_base.Draw()              // Should call Square::Draw
    WRITEF("Square called as Rectangle: ")
    square_as_rect.Draw()              // Should also call Square::Draw

    WRITEF("*N")

    // Test 7: Function parameter polymorphism
    WRITEF("Test 7: Function parameter polymorphism*N")

    // Test each shape type
    TestShapePolymorphism(shapes!0)    // Rectangle
    TestShapePolymorphism(shapes!1)    // Circle
    TestShapePolymorphism(shapes!2)    // Square

    WRITEF("*N")

    // Test 8: Return value polymorphism
    WRITEF("Test 8: Return value polymorphism*N")
    LET returned_shape = GetLargestShape(shapes, 5)
    WRITEF("Largest shape: ")
    returned_shape.Draw()
    WRITEF("Area: %D*N", returned_shape.GetArea())

    WRITEF("*N=== All virtual method stress tests completed ===*N")

    FREEVEC(shapes, 10)
    RESULTIS 0
$)

LET TestShapePolymorphism(shape) BE $(
    WRITEF("  Polymorphic test: ")
    shape.Draw()
    WRITEF("  Area: %D*N", shape.GetArea())
$)

LET GetLargestShape(shape_array, count) = VALOF $(
    LET largest = shape_array!0
    LET max_area = largest.GetArea()

    FOR i = 1 TO count-1 DO $(
        LET current = shape_array!i
        LET current_area = current.GetArea()
        IF current_area > max_area THEN $(
            largest := current
            max_area := current_area
        $)
    $)

    RESULTIS largest
$)
