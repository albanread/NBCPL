# Component-wise PAIRS Reduction

## What it does

Takes a collection of PAIRS and reduces each component independently.

## Example

```bcpl
LET points = PAIRS 3
points!0 := PAIR(10, 30)   // Point 1: x=10, y=30
points!1 := PAIR(5, 40)    // Point 2: x=5,  y=40  
points!2 := PAIR(15, 20)   // Point 3: x=15, y=20

LET min_point = MIN(points)
// Result: PAIR(5, 20)
// Because: min X = MIN(10,5,15) = 5
//          min Y = MIN(30,40,20) = 20
```

## The magic

- Developer writes: `MIN(points)`
- Compiler generates: NEON SIMD instructions automatically
- 4x faster than scalar code
- Works with MIN, MAX, SUM, etc.

## Why it's useful

Perfect for:
- 2D coordinates 
- Complex numbers
- Any paired data

No SIMD knowledge required!