# NewBCPL Objects

## A Pragmatic Approach to Structure

When designing NewBCPL, we considered adding traditional data structures like `structs` or `records`. However, in keeping with the spirit of BCPL, we wanted a single, powerful mechanism that could group both data and the procedures that operate on that data. The result is our simple, class-based object system.

It is important to state from the outset that this is a **minimal object implementation**. Our goal is not to replicate the complex hierarchies and feature sets of languages like C++ or Java. Instead, we provide a straightforward tool to help you better **structure your code and data**. Think of NewBCPL classes as enhanced records that bundle variables with the functions that manage them, promoting cleaner, more modular program design.

---

## Defining a `CLASS`

A new type of object is defined using the `CLASS` keyword. A class can optionally inherit from one parent class using `EXTENDS`. The body of the class, containing its members, is enclosed in `$( ... $)`.

Members can be **variables** (using `LET`, `FLET`, or `DECL`) or **methods** (`FUNCTION`s and `ROUTINE`s). Inside a method, the special keyword `SELF` is used to refer to the current object instance.

You can control member visibility with the `PUBLIC:`, `PRIVATE:`, and `PROTECTED:` keywords. By default, members are public.

```bcl
// A simple class to represent a point in 2D space
CLASS Point $(
  PUBLIC:
    LET x = 0
    LET y = 0

    ROUTINE move(dx, dy) BE $(
      SELF.x := SELF.x + dx
      SELF.y := SELF.y + dy
    $)
$)
```

### Creating and Using Objects
Objects are created dynamically on the heap using the NEW keyword. This expression allocates memory for the object, sets up its internal structure, and returns a pointer to it.

When an object is created with NEW, a special routine called CREATE is automatically called. This acts as the class constructor, and it's where you should place initialization logic.

Members are accessed using the familiar dot (.) operator.

```BCPL
LET my_point = NEW Point

// Access member variables
my_point.x := 10
my_point.y := 20

// Call a method
my_point.move(5, -3)
```

### Inheritance and Special Methods
NewBCPL supports single inheritance, allowing you to create more specialized classes. Within a child class, you can call a method from its parent using the SUPER keyword.

Code snippet

CLASS ColorPoint EXTENDS Point $(
  LET color = 0

  ROUTINE set(new_x, new_y, new_color) BE $(
    // Call the parent's 'move' method to set x and y
    SUPER.move(new_x, new_y)
    SELF.color := new_color
  $)
$)

Every class has two special methods that are central to an object's lifecycle:

CREATE: The constructor. It is automatically called by NEW. You can define your own CREATE routine to handle initialization and accept arguments (e.g., LET p = NEW Point(10, 20)).

RELEASE: The destructor. Its job is to free any resources the object may have acquired. With our Scope-Aware Memory Management (SAMM) system, RELEASE is called automatically when an object goes out of scope, but can be called manually if you have explicitly retained an object's memory.
