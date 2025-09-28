# BCPL Preprocessor

The preprocessor is a component of the BCPL compiler that handles file inclusion and other preprocessing directives before lexical analysis begins.

## Overview

The preprocessor takes a root source file as input and produces a single, flattened string of source code with all GET directives resolved and their corresponding file contents inserted. This ensures that the Lexer and Parser only ever see a single, complete stream of code, simplifying their design.

## Features

### GET Directive Processing

The preprocessor recursively processes `GET` directives, which allow source files to include other files. For example:

```bcpl
GET "libhdr.h"

LET START() = VALOF
$(
  // Code that uses functions defined in libhdr.h
$)
```

### Include Path Resolution

The preprocessor resolves file paths in the following order:
1. As specified directly (if an absolute path)
2. Relative to the including file's directory
3. In the include paths specified with the `-I` command line option
4. In the current working directory

### Error Handling

The preprocessor provides detailed error messages for:
- Files that cannot be found
- Circular dependencies (e.g., file A includes file B which includes file A)

### Source Mapping

The preprocessor inserts `//LINE` directives in the output source code to maintain accurate line number and file information for error reporting. This ensures that errors are reported with respect to the original source files, not the flattened output.

## Command Line Options

The preprocessor can be controlled via the following command line options:

- `--no-preprocessor`: Disables the preprocessor entirely. GET directives will be treated as syntax errors.
- `-I path`, `--include-path path`: Adds a directory to the include search path. Multiple paths can be specified.
- `--trace-preprocessor`: Enables detailed logging of preprocessor operations.

## Example

### Input Files

main.bcl:
```bcpl
GET "utils.h"

LET Main() BE
$(
  PrintGreeting("World")
$)
```

utils.h:
```bcpl
// Utility functions

LET PrintGreeting(name) BE
$(
  WRITES("Hello, ")
  WRITES(name)
  WRITES("!*n")
$)
```

### Preprocessor Output

```bcpl
//LINE 1 "/path/to/main.bcl"
//LINE 1 "/path/to/utils.h"
// Utility functions

LET PrintGreeting(name) BE
$(
  WRITES("Hello, ")
  WRITES(name)
  WRITES("!*n")
$)
//LINE 3 "/path/to/main.bcl"

LET Main() BE
$(
  PrintGreeting("World")
$)
```

This flattened output is what the lexer and parser will process.