# Type Inference in NewBCPL

## The BCPL Philosophy: The Machine Word is King

The original **BCPL**, in a brilliant homage to the hardware it ran on, was fundamentally an **untyped language**. It had only one data type: the machine word. Whether a word represented an integer, a character, or a memory address was a matter of interpretation left to the programmer and the context of its use. This design choice gave programmers immense power and flexibility, allowing them to write highly efficient, low-level code without the constraints of a rigid type system. It was simplicity in its purest form.

---

## NewBCPL's Modern Approach: Implicit Typing Through Inference

NewBCPL honors this philosophy by doing everything possible to avoid burdening the user with explicit type declarations. Our goal is for the programmer to write code that feels natural and direct, while the compiler handles the details of type management. We achieve this through **extensive type inference** across the entire compilation process.

The NewBCPL compiler is designed to be smart. It analyzes how you initialize and use your variables to automatically determine their types. For example:
* Initializing a variable with a floating-point literal (`LET x = 3.14`) tells the compiler that `x` is a float.
* When a function parameter is used in floating-point arithmetic, the compiler infers that the parameter itself must be a float.
* The type of a collection, like a list or vector, is inferred from the types of the elements it contains at creation time.

This powerful inference engine allows you to focus on your program's logic, not on satisfying a type checker.

---

## The Types You See

On the surface, the NewBCPL type system is intentionally minimal. The types you will most commonly interact with are:

* **Integers:** The default type for most variables, declared with `LET`.
* **Floats:** Declared with `FLET` or inferred from floating-point literals.
* **Strings, Lists, and Vectors:** These structured types are understood by the compiler, and their specific element types are inferred automatically when you create them.
* **Objects:** When you instantiate a class with `NEW`, the compiler knows the variable holds a pointer to an object of that specific class type.



---

## What Lurks Beneath: The Compiler's Type System 🕵️

Behind this simple facade lies a more sophisticated and detailed type system that the compiler uses internally. This system, represented by an enumeration called `VarType`, allows the compiler to manage modern data structures and perform powerful optimizations using static analysis.

Instead of just "integer" or "pointer," the compiler works with highly specific types like:
* `POINTER_TO_INT_VEC`
* `POINTER_TO_FLOAT_LIST`
* `POINTER_TO_OBJECT`
* `CONST_POINTER_TO_ANY_LIST`

This internal system is designed with BCPL's efficiency in mind. Many complex types are represented as **bitwise flags** on a single machine word. For example, the compiler knows a variable is a list by checking if both the `POINTER_TO` and `LIST` flags are set in its `VarType` value. This allows for extremely fast type checks during compilation.

For the advanced programmer who needs to interact with this underlying system, NewBCPL provides the `SETTYPE` intrinsic. This allows you to directly manipulate a variable's internal `VarType`, offering a powerful (but sharp!) tool for low-level programming tasks. This blend of high-level inference and optional low-level control is central to the NewBCPL philosophy.



## A Controversial Concession: Explicit Type Hints

More controversially, we also support an optional **type hint system**. While our primary goal is a language where types are almost never written, we concede that our type inferencer is not yet perfect. For those rare cases where the compiler cannot deduce the correct type, you can provide an explicit hint using the `AS` keyword.

For situations requiring more explicit control, the `AS` keyword allows you to provide a precise type annotation, such as `LET my_list AS LIST OF STRING`.

The syntax is designed to be readable and clear, specifying containers and the types they hold. The `^` character is a shorthand for `POINTER OF`.

Let us be clear on our philosophy: we do not believe this feature should be necessary. Every time you feel compelled to write AS, we feel you have found a bug. It is a bug in our type inferencer, which has failed in its primary duty to understand your code's intent. We view every use of AS as a bug report in disguise.
