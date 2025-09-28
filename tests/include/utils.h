// BCPL Utility Functions Header
// Common utility functions and routines for BCPL programs

// --- String functions ---

// Returns the length of a BCPL string
LET LENGTH(S) = VALOF
$(
    LET I = 0
    IF S = 0 THEN RESULTIS 0
    WHILE S!I /= 0 DO I := I + 1
    RESULTIS I
$)

// Append string S2 to S1, modifying S1 in place
// Returns S1
LET APPEND(S1, S2) = VALOF
$(
    LET N1 = LENGTH(S1)
    LET N2 = LENGTH(S2)
    LET I = 0
    
    IF S1 = 0 RETURN 0
    IF S2 = 0 RETURN S1
    
    WHILE I < N2 DO
    $(
        S1!(N1 + I) := S2!I
        I := I + 1
    $)
    
    S1!(N1 + N2) := 0  // Null terminator
    RESULTIS S1
$)

// --- Math functions ---

// Returns the maximum of two numbers
LET MAX(A, B) = A > B -> A, B

// Returns the minimum of two numbers
LET MIN(A, B) = A < B -> A, B

// Returns the greatest common divisor of A and B
LET GCD(A, B) = VALOF
$(
    LET X = ABS(A)
    LET Y = ABS(B)
    LET R = 0
    
    IF X = 0 RESULTIS Y
    IF Y = 0 RESULTIS X
    
    UNTIL Y = 0 DO
    $(
        R := X REM Y
        X := Y
        Y := R
    $)
    
    RESULTIS X
$)

// --- Output formatting ---

// Writes a newline character
LET NEWLINE() BE WRITES("*N")

// Writes N space characters
LET SPACES(N) BE 
$(
    FOR I = 1 TO N DO WRITES(" ")
$)

// Writes a character N times
LET WRITEREP(CH, N) BE
$(
    FOR I = 1 TO N DO WRITES(CH)
$)

// Conditional writing - only writes if condition is true
LET WRITESIF(COND, S) BE IF COND THEN WRITES(S)