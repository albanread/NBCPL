
# Proposal: Enhanced List Syntax for NewBCPL

## Overview

This proposal describes an enhancement to the NewBCPL language and compiler to allow users to create list literals using parentheses, e.g.:

```bcpl
LET list = (1, 2, "fish")
```

Currently, list literals are only supported via the keywords `LIST` or `MANIFESTLIST`. This change aims to make list creation more natural and concise for users, and also allows direct assignment of literal lists using the `:=` operator.

---

## Motivation

- **User-Friendly Syntax:** Parentheses are familiar and concise for denoting collections.
- **Consistency:** Allows lists to be created directly in variable initializers and other contexts.
- **Readability:** Improves code clarity, especially for simple data structures.

---

## Proposed Changes

### 1. Parser Update

- **Contextual Parsing:**
  The parser will treat parenthesized, comma-separated sequences as list literals only in specific contexts (e.g., after `LET =`).
- **ListExpression Node:**
  When parsing an initializer after `LET =`, if the next token is `(`, the parser will parse the contents as a list and produce a `ListExpression` AST node.
- **No Global Change:**
  Parentheses will continue to denote grouped expressions and function calls elsewhere. Only LET initializers, assignment statements using `:=`, and other explicit contexts will use the new list literal behavior.

### 2. Syntax Examples

```bcpl
LET a = (1, 2, 3)
LET b = ("foo", "bar", 42)
LET c = (1, "hello", (2, 3))
manifest colors = ("red", "green", "blue")

a := (1, 2, 3)
b := ("foo", "bar", 42)
c := (1, "hello", (2, 3))
```

### 3. Restrictions

- **Only constant literals** (numbers, strings, lists) are allowed as list elements.
- **No expressions** (e.g., `x+1`) inside list literals.
- **No ambiguity:**
  Grouped expressions like `(1 + 2)` and function calls like `foo(1, 2)` remain unchanged.
- **Assignment support:**
  Literal lists can be assigned directly using `:=` as well as LET initializers.

---

## Impact Analysis

### Expression Evaluation

- **Ambiguity Avoidance:**
  By restricting list-literal parsing to LET initializers, there is no negative impact on grouped expressions or function calls.
- **Parser Logic:**
  The parser must distinguish between:
    - Grouped expressions: `(1 + 2)`
    - List literals: `(1, 2, "fish")`
    - Function calls: `foo(1, 2)`

### Risks

- **If implemented globally:**
  Could introduce ambiguity and parsing errors.
- **If implemented contextually:**
  No negative impact; existing code and semantics remain unchanged.

---

## Implementation Steps

1. **Update `parse_expression()` and LET initializer logic:**
   - When encountering `(` after `=`, parse as a list literal if the contents are comma-separated.
2. **Create `ListExpression` AST node:**
   - Store the parsed elements as initializers.
3. **Emit lists as before:**
   - Code generation and runtime representation remain unchanged.

---

## Summary Table

| Syntax           | Old Meaning         | New Meaning (after change) | Risk?         |
|------------------|--------------------|----------------------------|---------------|
| `(1 + 2)`        | Grouped expression | Grouped expression         | No            |
| `(1, 2, 3)`      | Error or grouping  | List literal (in LET or :=)| No (contextual)|
| `foo(1, 2)`      | Function call      | Function call              | No            |
| `LET x = (1,2)`  | Error or grouping  | List literal               | No (contextual)|
| `x := (1,2)`     | Error or grouping  | List literal               | No (contextual)|

---

## Recommendation

- **Adopt contextual parsing for list literals using parentheses in LET initializers.**
- **Do not globally treat parentheses as lists.**
- **Maintain existing behavior for grouped expressions and function calls.**

---

## Example Usage

```bcpl
LET nums = (10, 20, 30)
LET values = (1, "foo", (2, 3))
manifest palette = ("red", "green", "blue")

nums := (10, 20, 30)
values := (1, "foo", (2, 3))
palette := ("red", "green", "blue")
```

---

## Author

*This proposal was prepared by an expert engineer based on a review of the NewBCPL compiler source code and language design.*

---
