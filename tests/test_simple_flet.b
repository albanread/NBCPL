// Simple test for FLET register allocation

LET START() BE
{
    // Simple FLET declaration
    FLET f = 1.0

    // Use the variable to prevent it from being optimized away
    WRITEF("f = %f*n", f)

    // Modify the variable using :=
    f := f + 2.0
    WRITEF("f = %f*n", f)
}
