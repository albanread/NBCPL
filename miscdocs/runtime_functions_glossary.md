# NewBCPL Runtime Functions Glossary

This document provides a comprehensive reference for all runtime functions available in NewBCPL, organized by functional category.

## Table of Contents

- [Basic I/O Functions](#basic-io-functions)
- [String Operations](#string-operations)
- [Memory Management](#memory-management)
- [Mathematical Functions](#mathematical-functions)
- [Random Number Generation](#random-number-generation)
- [List Operations](#list-operations)
- [File I/O](#file-io)
- [System Functions](#system-functions)
- [Object-Oriented Support](#object-oriented-support)
- [Type Conversion and Checking](#type-conversion-and-checking)
- [Error Handling](#error-handling)

---

## Basic I/O Functions

### Output Functions

**WRITES(string_ptr)**
- **Purpose**: Write a string to standard output
- **Parameters**: `string_ptr` (INTEGER) - Pointer to BCPL string
- **Returns**: Nothing (routine)
- **Example**: `WRITES("Hello, World!*N")`

**WRITEN(value)**
- **Purpose**: Write an integer value to standard output
- **Parameters**: `value` (INTEGER) - Integer to write
- **Returns**: Nothing (routine)
- **Example**: `WRITEN(42)`

**FWRITE(value)**
- **Purpose**: Write a floating-point value to standard output
- **Parameters**: `value` (FLOAT) - Float to write
- **Returns**: Nothing (routine)
- **Example**: `FWRITE(3.14159)`

**WRITEC(char)**
- **Purpose**: Write a single character to standard output
- **Parameters**: `char` (INTEGER) - Character code to write
- **Returns**: Nothing (routine)
- **Example**: `WRITEC(65)` // Writes 'A'

**NEWLINE()**
- **Purpose**: Write a newline character to standard output
- **Parameters**: None
- **Returns**: Nothing (routine)
- **Example**: `NEWLINE()`

### Formatted Output

**WRITEF(format_string, ...)**
- **Purpose**: Write formatted output (variadic function)
- **Parameters**: `format_string` (STRING) - Format string with placeholders
- **Returns**: Nothing (routine)
- **Format Codes**:
  - `%n` - Insert integer argument
  - `%s` - Insert string argument
  - `%c` - Insert character argument
  - `%f` - Insert float argument
- **Example**: `WRITEF("Value: %n, Name: %s*N", 42, "test")`

**WRITEF1(format, arg1)** through **WRITEF7(format, arg1, ..., arg7)**
- **Purpose**: Specific arity versions of WRITEF for 1-7 arguments
- **Parameters**: Format string plus 1-7 arguments
- **Returns**: Nothing (routine)

### Input Functions

**RDCH()**
- **Purpose**: Read a single character from standard input
- **Parameters**: None
- **Returns**: INTEGER - Character code read, or -1 for EOF
- **Example**: `LET ch = RDCH()`

---

## String Operations

**STRLEN(string_ptr)**
- **Purpose**: Get the length of a BCPL string
- **Parameters**: `string_ptr` (INTEGER) - Pointer to BCPL string
- **Returns**: INTEGER - Length of string in characters
- **Example**: `LET len = STRLEN("Hello")`

**STRCMP(string1, string2)**
- **Purpose**: Compare two BCPL strings
- **Parameters**: 
  - `string1` (INTEGER) - Pointer to first string
  - `string2` (INTEGER) - Pointer to second string
- **Returns**: INTEGER - 0 if equal, <0 if string1 < string2, >0 if string1 > string2
- **Example**: `IF STRCMP(name, "admin") = 0 THEN ...`

**STRCOPY(dest, src)**
- **Purpose**: Copy one BCPL string to another
- **Parameters**:
  - `dest` (INTEGER) - Pointer to destination string buffer
  - `src` (INTEGER) - Pointer to source string
- **Returns**: INTEGER - Pointer to destination string
- **Example**: `STRCOPY(buffer, source_string)`

**PACKSTRING(bcpl_string)**
- **Purpose**: Convert BCPL string to byte vector
- **Parameters**: `bcpl_string` (INTEGER) - Pointer to BCPL string
- **Returns**: INTEGER - Pointer to byte vector
- **Example**: `LET bytes = PACKSTRING(my_string)`

**UNPACKSTRING(byte_vector)**
- **Purpose**: Convert byte vector to BCPL string
- **Parameters**: `byte_vector` (INTEGER) - Pointer to byte vector
- **Returns**: INTEGER - Pointer to BCPL string
- **Example**: `LET str = UNPACKSTRING(byte_data)`

**SPLIT(source_string, delimiter)**
- **Purpose**: Split a string into a list using delimiter
- **Parameters**:
  - `source_string` (INTEGER) - String to split
  - `delimiter` (INTEGER) - Delimiter string
- **Returns**: INTEGER - Pointer to list of string parts
- **Example**: `LET parts = SPLIT("a,b,c", ",")`

**JOIN(string_list, delimiter)**
- **Purpose**: Join a list of strings with delimiter
- **Parameters**:
  - `string_list` (INTEGER) - List of strings
  - `delimiter` (INTEGER) - Delimiter string
- **Returns**: INTEGER - Pointer to joined string
- **Example**: `LET result = JOIN(word_list, " ")`

---

## Memory Management

**BCPL_ALLOC_WORDS(size, type, init_value)**
- **Purpose**: Allocate memory for words (8-byte units)
- **Parameters**:
  - `size` (INTEGER) - Number of words to allocate
  - `type` (INTEGER) - Type tag for allocation
  - `init_value` (INTEGER) - Initial value for words
- **Returns**: INTEGER - Pointer to allocated memory
- **Example**: `LET buffer = BCPL_ALLOC_WORDS(100, 0, 0)`

**BCPL_ALLOC_CHARS(size)**
- **Purpose**: Allocate memory for characters (byte units)
- **Parameters**: `size` (INTEGER) - Number of bytes to allocate
- **Returns**: INTEGER - Pointer to allocated memory
- **Example**: `LET char_buffer = BCPL_ALLOC_CHARS(256)`

**MALLOC(size)**
- **Purpose**: Alias for BCPL_ALLOC_WORDS (compatibility)
- **Parameters**: `size` (INTEGER) - Number of words to allocate
- **Returns**: INTEGER - Pointer to allocated memory
- **Example**: `LET ptr = MALLOC(50)`

**FREEVEC(ptr)**
- **Purpose**: Free previously allocated memory
- **Parameters**: `ptr` (INTEGER) - Pointer to memory to free
- **Returns**: Nothing (routine)
- **Example**: `FREEVEC(buffer)`

---

## Mathematical Functions

### Trigonometric Functions

**FSIN(x)**
- **Purpose**: Compute sine of x (x in radians)
- **Parameters**: `x` (FLOAT) - Angle in radians
- **Returns**: FLOAT - Sine of x
- **Example**: `FLET result = FSIN(1.5708)` // sin(π/2) ≈ 1.0

**FCOS(x)**
- **Purpose**: Compute cosine of x (x in radians)
- **Parameters**: `x` (FLOAT) - Angle in radians
- **Returns**: FLOAT - Cosine of x
- **Example**: `FLET result = FCOS(0.0)` // cos(0) = 1.0

**FTAN(x)**
- **Purpose**: Compute tangent of x (x in radians)
- **Parameters**: `x` (FLOAT) - Angle in radians
- **Returns**: FLOAT - Tangent of x
- **Example**: `FLET result = FTAN(0.7854)` // tan(π/4) ≈ 1.0

### Exponential and Logarithmic Functions

**FEXP(x)**
- **Purpose**: Compute e raised to the power of x
- **Parameters**: `x` (FLOAT) - Exponent
- **Returns**: FLOAT - e^x
- **Example**: `FLET result = FEXP(1.0)` // e^1 ≈ 2.718

**FLOG(x)**
- **Purpose**: Compute natural logarithm of x
- **Parameters**: `x` (FLOAT) - Input value (must be positive)
- **Returns**: FLOAT - ln(x)
- **Example**: `FLET result = FLOG(2.718)` // ln(e) ≈ 1.0

### Other Mathematical Functions

**FABS(x)**
- **Purpose**: Compute absolute value of x
- **Parameters**: `x` (FLOAT) - Input value
- **Returns**: FLOAT - |x|
- **Example**: `FLET result = FABS(-3.14)` // Result: 3.14

---

## Random Number Generation

**RAND(max_val)**
- **Purpose**: Generate random integer between 0 and max_val (inclusive)
- **Parameters**: `max_val` (INTEGER) - Maximum value for random number
- **Returns**: INTEGER - Random integer in range [0, max_val]
- **Example**: `LET dice = RAND(5) + 1` // Dice roll 1-6

**FRND()**
- **Purpose**: Generate random floating-point number between 0.0 and 1.0
- **Parameters**: None
- **Returns**: FLOAT - Random float in range [0.0, 1.0)
- **Example**: `FLET probability = FRND()`

---

## List Operations

### List Creation and Basic Operations

**BCPL_LIST_CREATE_EMPTY()**
- **Purpose**: Create an empty list
- **Parameters**: None
- **Returns**: INTEGER - Pointer to empty list
- **Example**: `LET my_list = BCPL_LIST_CREATE_EMPTY()`

**BCPL_LIST_APPEND_INT(list, value)**
- **Purpose**: Append an integer to a list
- **Parameters**:
  - `list` (INTEGER) - Pointer to list
  - `value` (INTEGER) - Integer value to append
- **Returns**: Nothing (routine)
- **Example**: `BCPL_LIST_APPEND_INT(my_list, 42)`

**BCPL_LIST_APPEND_FLOAT(list, value)**
- **Purpose**: Append a float to a list
- **Parameters**:
  - `list` (INTEGER) - Pointer to list
  - `value` (FLOAT) - Float value to append
- **Returns**: Nothing (routine)
- **Example**: `BCPL_LIST_APPEND_FLOAT(my_list, 3.14)`

**BCPL_LIST_APPEND_STRING(list, string)**
- **Purpose**: Append a string to a list
- **Parameters**:
  - `list` (INTEGER) - Pointer to list
  - `string` (INTEGER) - Pointer to string to append
- **Returns**: Nothing (routine)
- **Example**: `BCPL_LIST_APPEND_STRING(my_list, "hello")`

### List Access Functions

**BCPL_LIST_GET_HEAD_AS_INT(list)**
- **Purpose**: Get the first element of a list as an integer
- **Parameters**: `list` (INTEGER) - Pointer to list
- **Returns**: INTEGER - First element as integer
- **Example**: `LET first = BCPL_LIST_GET_HEAD_AS_INT(my_list)`

**BCPL_LIST_GET_HEAD_AS_FLOAT(list)**
- **Purpose**: Get the first element of a list as a float
- **Parameters**: `list` (INTEGER) - Pointer to list
- **Returns**: FLOAT - First element as float
- **Example**: `FLET first = BCPL_LIST_GET_HEAD_AS_FLOAT(my_list)`

**BCPL_LIST_GET_TAIL(list)**
- **Purpose**: Get the rest of the list (all elements except first)
- **Parameters**: `list` (INTEGER) - Pointer to list
- **Returns**: INTEGER - Pointer to tail of list
- **Example**: `LET rest = BCPL_LIST_GET_TAIL(my_list)`

**BCPL_LIST_GET_NTH(list, index)**
- **Purpose**: Get the nth element of a list
- **Parameters**:
  - `list` (INTEGER) - Pointer to list
  - `index` (INTEGER) - Index of element (0-based)
- **Returns**: INTEGER - Pointer to nth element
- **Example**: `LET third = BCPL_LIST_GET_NTH(my_list, 2)`

### List Manipulation Functions

**COPYLIST(list)**
- **Purpose**: Create a shallow copy of a list
- **Parameters**: `list` (INTEGER) - Pointer to original list
- **Returns**: INTEGER - Pointer to copied list
- **Example**: `LET copy = COPYLIST(original)`

**DEEPCOPYLIST(list)**
- **Purpose**: Create a deep copy of a list (recursively copies all elements)
- **Parameters**: `list` (INTEGER) - Pointer to original list
- **Returns**: INTEGER - Pointer to deep copied list
- **Example**: `LET deep_copy = DEEPCOPYLIST(nested_list)`

**REVERSE(list)**
- **Purpose**: Reverse the order of elements in a list
- **Parameters**: `list` (INTEGER) - Pointer to list
- **Returns**: INTEGER - Pointer to reversed list
- **Example**: `LET reversed = REVERSE(my_list)`

**CONCAT(list1, list2)**
- **Purpose**: Concatenate two lists
- **Parameters**:
  - `list1` (INTEGER) - Pointer to first list
  - `list2` (INTEGER) - Pointer to second list
- **Returns**: INTEGER - Pointer to concatenated list
- **Example**: `LET combined = CONCAT(list_a, list_b)`

**FIND(list, value, type)**
- **Purpose**: Find an element in a list
- **Parameters**:
  - `list` (INTEGER) - Pointer to list to search
  - `value` (INTEGER) - Value to find
  - `type` (INTEGER) - Type tag of value
- **Returns**: INTEGER - Pointer to found element, or 0 if not found
- **Example**: `LET found = FIND(my_list, 42, ATOM_INT)`

### Convenient Aliases

**APND(list, value)**
- **Purpose**: Alias for BCPL_LIST_APPEND_INT
- **Parameters**: Same as BCPL_LIST_APPEND_INT
- **Example**: `APND(my_list, 123)`

**FPND(list, value)**
- **Purpose**: Alias for BCPL_LIST_APPEND_FLOAT
- **Parameters**: Same as BCPL_LIST_APPEND_FLOAT
- **Example**: `FPND(my_list, 2.5)`

**SPND(list, string)**
- **Purpose**: Alias for BCPL_LIST_APPEND_STRING
- **Parameters**: Same as BCPL_LIST_APPEND_STRING
- **Example**: `SPND(my_list, "text")`

**LPND(list1, list2)**
- **Purpose**: Append one list to another
- **Parameters**:
  - `list1` (INTEGER) - Target list
  - `list2` (INTEGER) - List to append
- **Returns**: Nothing (routine)
- **Example**: `LPND(main_list, sub_list)`

### List Memory Management

**BCPL_FREE_LIST(list)**
- **Purpose**: Free memory used by a list
- **Parameters**: `list` (INTEGER) - Pointer to list to free
- **Returns**: Nothing (routine)
- **Example**: `BCPL_FREE_LIST(old_list)`

**BCPL_FREE_LIST_SAFE(list)**
- **Purpose**: Safely free a list with error checking
- **Parameters**: `list` (INTEGER) - Pointer to list to free
- **Returns**: Nothing (routine)
- **Example**: `BCPL_FREE_LIST_SAFE(maybe_null_list)`

---

## File I/O

**SLURP(filename)**
- **Purpose**: Read entire file contents into a string
- **Parameters**: `filename` (INTEGER) - Pointer to filename string
- **Returns**: INTEGER - Pointer to string containing file contents
- **Example**: `LET content = SLURP("data.txt")`

**SPIT(content, filename)**
- **Purpose**: Write string contents to a file
- **Parameters**:
  - `content` (INTEGER) - Pointer to string to write
  - `filename` (INTEGER) - Pointer to filename string
- **Returns**: Nothing (routine)
- **Example**: `SPIT("Hello, file!", "output.txt")`

---

## System Functions

**FINISH()**
- **Purpose**: Terminate the program normally
- **Parameters**: None
- **Returns**: Does not return (terminates program)
- **Example**: `FINISH()`

**BCPL_FREE_CELLS()**
- **Purpose**: Trigger garbage collection of free list cells
- **Parameters**: None
- **Returns**: Nothing (routine)
- **Example**: `BCPL_FREE_CELLS()`

**GET_FREE_LIST_HEAD_ADDR()**
- **Purpose**: Get address of the free list head (internal use)
- **Parameters**: None
- **Returns**: INTEGER - Address of free list head
- **Example**: `LET addr = GET_FREE_LIST_HEAD_ADDR()`

---

## Object-Oriented Support

**OBJECT_HEAP_ALLOC(class_ptr)**
- **Purpose**: Allocate memory for an object instance
- **Parameters**: `class_ptr` (INTEGER) - Pointer to class definition
- **Returns**: INTEGER - Pointer to allocated object
- **Example**: `LET obj = OBJECT_HEAP_ALLOC(MyClass)`

**OBJECT_HEAP_FREE(object_ptr)**
- **Purpose**: Free memory used by an object
- **Parameters**: `object_ptr` (INTEGER) - Pointer to object to free
- **Returns**: Nothing (routine)
- **Example**: `OBJECT_HEAP_FREE(old_obj)`

**RUNTIME_METHOD_LOOKUP(class_ptr, method_hash)**
- **Purpose**: Look up a method in a class's virtual table
- **Parameters**:
  - `class_ptr` (INTEGER) - Pointer to class
  - `method_hash` (INTEGER) - Hash of method name
- **Returns**: INTEGER - Pointer to method implementation
- **Example**: Internal use by method dispatch

---

## Type Conversion and Checking

**SETTYPE(value, type)**
- **Purpose**: Set the runtime type of a value (pseudo-function)
- **Parameters**:
  - `value` (INTEGER) - Value to type
  - `type` (INTEGER) - Type tag to assign
- **Returns**: INTEGER - Typed value
- **Note**: Handled specially by the compiler
- **Example**: `LET typed_val = SETTYPE(42, ATOM_INT)`

**BCPL_GET_ATOM_TYPE(atom)**
- **Purpose**: Get the runtime type of an atom
- **Parameters**: `atom` (INTEGER) - Pointer to atom
- **Returns**: INTEGER - Type tag of atom
- **Example**: `LET type = BCPL_GET_ATOM_TYPE(some_value)`

### Type Assertion Functions

**AS_INT(value)**
- **Purpose**: Assert that value is an integer and extract it
- **Parameters**: `value` (INTEGER) - Typed value to extract
- **Returns**: INTEGER - Extracted integer value
- **Example**: `LET num = AS_INT(typed_value)`

**AS_FLOAT(value)**
- **Purpose**: Assert that value is a float and extract it
- **Parameters**: `value` (INTEGER) - Typed value to extract
- **Returns**: FLOAT - Extracted float value
- **Example**: `FLET num = AS_FLOAT(typed_value)`

**AS_STRING(value)**
- **Purpose**: Assert that value is a string and extract it
- **Parameters**: `value` (INTEGER) - Typed value to extract
- **Returns**: INTEGER - Pointer to extracted string
- **Example**: `LET str = AS_STRING(typed_value)`

**AS_LIST(value)**
- **Purpose**: Assert that value is a list and extract it
- **Parameters**: `value` (INTEGER) - Typed value to extract
- **Returns**: INTEGER - Pointer to extracted list
- **Example**: `LET list = AS_LIST(typed_value)`

---

## Error Handling

**BCPL_GET_LAST_ERROR(buffer)**
- **Purpose**: Get the last error message
- **Parameters**: `buffer` (INTEGER) - Buffer to store error message
- **Returns**: Nothing (routine)
- **Example**: `BCPL_GET_LAST_ERROR(error_buffer)`

**BCPL_CLEAR_ERRORS()**
- **Purpose**: Clear all pending errors
- **Parameters**: None
- **Returns**: Nothing (routine)
- **Example**: `BCPL_CLEAR_ERRORS()`

**BCPL_CHECK_AND_DISPLAY_ERRORS()**
- **Purpose**: Check for errors and display them if any exist
- **Parameters**: None
- **Returns**: Nothing (routine)
- **Example**: `BCPL_CHECK_AND_DISPLAY_ERRORS()`

---

## Function Type Reference

### Return Types
- **INTEGER**: Functions that return integer values
- **FLOAT**: Functions that return floating-point values
- **Routine**: Functions that don't return values (procedures)

### Parameter Types
- **INTEGER**: Integer parameters (includes pointers, which are addresses)
- **FLOAT**: Floating-point parameters
- **STRING**: String parameters (actually INTEGER pointers to strings)

### Calling Conventions
- Integer functions store result in `X0` register
- Float functions store result in `D0` register
- Arguments are passed in `X0`, `X1`, `X2`, ... for integers and `D0`, `D1`, `D2`, ... for floats
- Mixed argument types are handled according to each parameter's declared type

---

## Notes

1. **Memory Management**: Always pair allocation functions with their corresponding free functions to avoid memory leaks.

2. **String Handling**: BCPL strings are length-prefixed and stored as word arrays. The runtime handles conversion between different string representations.

3. **Lists**: NewBCPL lists are dynamically typed and can contain mixed types of elements (integers, floats, strings, other lists).

4. **Error Handling**: Many functions set internal error states that can be checked with the error handling functions.

5. **Type Safety**: The `AS_*` functions perform runtime type checking and will terminate the program if the assertion fails.

6. **Float Precision**: All floating-point operations use double precision (64-bit).

7. **Performance**: List operations and string operations may allocate memory. Consider using the bulk operations when working with large datasets.

---

*This glossary covers NewBCPL Runtime v1.0.0. For the most up-to-date information, consult the source code in the `runtime/` directory.*