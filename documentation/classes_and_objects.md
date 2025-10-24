# Classes and Objects in NewBCPL

## Overview

NewBCPL extends classic BCPL with a modern, pragmatic object system that maintains the language's simplicity while providing powerful object-oriented programming capabilities. The system uses vtable-based method dispatch, single inheritance, and integrates seamlessly with SAMM (Scope Aware Memory Management) for automatic memory cleanup.

## Table of Contents

- [Class Declaration](#class-declaration)
- [Object Creation and Memory Management](#object-creation-and-memory-management)
- [Member Access and Method Calls](#member-access-and-method-calls)
- [Inheritance](#inheritance)
- [Special Methods (CREATE/RELEASE)](#special-methods-createrelease)
- [Visibility Control](#visibility-control)
- [Virtual Methods and Polymorphism](#virtual-methods-and-polymorphism)
- [Memory Layout](#memory-layout)
- [Integration with SAMM](#integration-with-samm)
- [Examples](#examples)
- [Implementation Details](#implementation-details)

## Class Declaration

Classes are declared using the `CLASS` keyword, with the class body enclosed in `$( ... $)`:

```bcl
CLASS Point $(
    DECL x, y  // Member variables
    
    ROUTINE CREATE(initialX, initialY) BE $(
        x := initialX
        y := initialY
    $)
    
    FUNCTION getX() = x
    FUNCTION getY() = y
    
    ROUTINE move(dx, dy) BE $(
        x := x + dx
        y := y + dy
    $)
$)
```

### Member Types

- **Variables**: Declared with `DECL`, `LET`, or `FLET`
- **Methods**: Declared with `FUNCTION` (returns a value) or `ROUTINE` (void)
- **Initialization**: Use `LET variable = value` for initialized members

## Object Creation and Memory Management

Objects are created on the heap using the `NEW` keyword:

```bcl
LET point = NEW Point(10, 20)  // Creates object and calls CREATE
LET shape = NEW Shape          // Creates object with default CREATE
```

### Memory Allocation

- Objects are allocated on the heap using `OBJECT_HEAP_ALLOC`
- Each object starts with an 8-byte vtable pointer at offset 0
- Member variables are allocated at 8-byte aligned offsets starting from offset 8
- Memory is automatically tracked by the heap manager

## Member Access and Method Calls

Use the dot operator (`.`) to access members and call methods:

```bcl
// Member variable access
point.x := 50
LET currentX = point.x

// Method calls
point.move(5, -3)
LET x_value = point.getX()
```

### Method Dispatch

- Methods are called through vtable lookup for virtual dispatch
- The object pointer (`this`) is automatically passed as the first argument
- Method addresses are resolved at runtime through the vtable

## Inheritance

NewBCPL supports single inheritance using the `EXTENDS` keyword:

```bcl
CLASS ColorPoint EXTENDS Point $(
    DECL color
    
    ROUTINE CREATE(x, y, c) BE $(
        // Initialize parent members
        SELF.x := x
        SELF.y := y
        color := c
    $)
    
    FUNCTION getColor() = color
$)
```

### Calling Parent Methods

Use the `SUPER` keyword to call parent class methods:

```bcl
CLASS Shape3D EXTENDS Shape $(
    DECL z
    
    ROUTINE move(dx, dy, dz) BE $(
        SUPER.move(dx, dy)  // Call parent's move method
        z := z + dz
    $)
$)
```

## Special Methods (CREATE/RELEASE)

Every class has two special lifecycle methods:

### CREATE (Constructor)

- Automatically called when `NEW` is used
- Can accept parameters: `NEW Point(10, 20)`
- Used for object initialization
- Always placed at vtable slot 0

```bcl
ROUTINE CREATE(initial_value) BE $(
    count := initial_value
    WRITES("Object created with value: ")
    WRITEN(count)
    WRITEC('*N')
$)
```

### RELEASE (Destructor)

- Called automatically when object goes out of scope (with SAMM)
- Can be called manually for explicit cleanup
- Always placed at vtable slot 1
- Should clean up resources acquired by the object

```bcl
ROUTINE RELEASE() BE $(
    WRITES("Cleaning up object*N")
    IF buffer ~= 0 THEN $(
        FREEVEC(buffer)
        buffer := 0
    $)
    // Object memory freed automatically
$)
```

## Visibility Control

Control member access with visibility modifiers:

```bcl
CLASS BankAccount $(
    PUBLIC:
        FUNCTION getBalance() = balance
        ROUTINE deposit(amount) BE $(
            balance := balance + amount
        $)
    
    PRIVATE:
        DECL balance  // Only accessible within class
        
    PROTECTED:
        ROUTINE updateBalance(amount) BE $(
            balance := amount
        $)
$)
```

- **PUBLIC**: Accessible from anywhere (default)
- **PRIVATE**: Only accessible within the same class
- **PROTECTED**: Accessible within class and subclasses

## Virtual Methods and Polymorphism

Methods can be declared virtual for runtime polymorphism:

```bcl
CLASS Animal $(
    VIRTUAL ROUTINE makeSound() BE $(
        WRITES("Generic animal sound*N")
    $)
$)

CLASS Dog EXTENDS Animal $(
    ROUTINE makeSound() BE $(  // Overrides parent method
        WRITES("Woof!*N")
    $)
$)
```

### Final Methods

Prevent method overriding with `FINAL`:

```bcl
FINAL ROUTINE criticalMethod() BE $(
    // This method cannot be overridden
$)
```

## Memory Layout

### Object Structure

```
Offset 0:  vtable pointer (8 bytes)
Offset 8:  first member variable (8 bytes)
Offset 16: second member variable (8 bytes)
...
```

### VTable Structure

```
Slot 0: CREATE method
Slot 1: RELEASE method  
Slot 2: first user method
Slot 3: second user method
...
```

## Integration with SAMM

NewBCPL objects integrate seamlessly with SAMM for automatic memory management:

### Automatic Cleanup

```bcl
LET processData() BE $(
    LET processor = NEW DataProcessor(1024)  // Allocated in current scope
    processor.process()
    // processor automatically cleaned up when scope exits
$)
```

### Manual Lifetime Control

```bcl
LET createPersistentObject() = VALOF $(
    LET obj = NEW MyClass()
    RETAIN obj  // Prevent automatic cleanup
    RESULTIS obj  // Caller now owns the object
$)
```

### Scope-Based Management

- Objects are automatically tracked per lexical scope
- RELEASE called in reverse order of creation when scope exits
- Can be disabled per-object with RETAIN
- Compatible with manual memory management

## Examples

### Basic Class Usage

```bcl
CLASS Counter $(
    LET count = 0
    
    ROUTINE CREATE(initial) BE $(
        count := initial
    $)
    
    ROUTINE increment() BE $(
        count := count + 1
    $)
    
    FUNCTION getValue() = count
$)

LET START() BE $(
    LET c = NEW Counter(10)
    c.increment()
    WRITEN(c.getValue())  // Prints 11
$)
```

### Inheritance Example

```bcl
CLASS Vehicle $(
    DECL speed, fuel
    
    ROUTINE CREATE(initialSpeed) BE $(
        speed := initialSpeed
        fuel := 100
    $)
    
    VIRTUAL ROUTINE accelerate() BE $(
        speed := speed + 10
        fuel := fuel - 5
    $)
$)

CLASS Car EXTENDS Vehicle $(
    DECL doors
    
    ROUTINE CREATE(speed, numDoors) BE $(
        SUPER.CREATE(speed)
        doors := numDoors
    $)
    
    ROUTINE accelerate() BE $(  // Override
        speed := speed + 15  // Cars accelerate faster
        fuel := fuel - 3     // But use less fuel
    $)
$)
```

### Resource Management Example

```bcl
CLASS FileHandler $(
    DECL file_handle, filename
    
    ROUTINE CREATE(name) BE $(
        filename := name
        file_handle := openFile(filename)
        IF file_handle = 0 THEN $(
            WRITES("Failed to open file: ")
            WRITES(filename)
            WRITEC('*N')
        $)
    $)
    
    ROUTINE RELEASE() BE $(
        IF file_handle ~= 0 THEN $(
            closeFile(file_handle)
            WRITES("Closed file: ")
            WRITES(filename)
            WRITEC('*N')
        $)
    $)
    
    FUNCTION isOpen() = file_handle ~= 0
$)
```

## Implementation Details

### VTable Generation

- Each class gets a vtable with method pointers
- CREATE always at slot 0, RELEASE at slot 1
- Inherited methods keep parent's slot numbers
- New methods get assigned sequential slots

### Method Name Mangling

- Methods are mangled as `ClassName::methodName`
- Enables proper linking and inheritance resolution
- Handles method overriding correctly

### Type Inference

- Member types are inferred from usage and initialization
- Float members detected from FLET declarations
- Object types tracked through class table

### Memory Safety

- Double-free protection through bloom filters
- Heap block tracking for leak detection
- Automatic cleanup integration with SAMM
- Signal-safe heap dumping for debugging

### Performance Considerations

- Single vtable indirection for method calls
- 8-byte aligned member access
- Efficient register usage for member access
- Optimized ARM64 instruction generation

## Best Practices

1. **Initialize members in CREATE**: Always initialize member variables in the constructor
2. **Clean up in RELEASE**: Free any allocated resources in the destructor
3. **Use RETAIN judiciously**: Only use RETAIN when object lifetime extends beyond natural scope
4. **Prefer composition**: Use inheritance for "is-a" relationships, composition for "has-a"
5. **Make methods virtual when needed**: Only use virtual for methods that need polymorphic behavior
6. **Handle SAMM integration**: Be aware of automatic cleanup when designing object lifecycles

## Debugging and Diagnostics

The class system includes extensive debugging support:

- Class layout validation
- VTable structure logging
- Memory leak detection
- Heap usage tracking
- Method resolution tracing

Enable debugging with compiler flags or environment variables to get detailed information about class processing and object lifecycle management.