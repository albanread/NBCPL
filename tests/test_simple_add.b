// Simple test with just float addition to ensure basic operations work

LET START() BE
{
    // Initial value
    FLET f = 1.0

    // Print the initial value
    WRITEF("Initial value: %f*n", f)

    // Simple addition - no need to store result in the same variable
    FLET g = f +# 2.0

    // Print the result
    WRITEF("After adding 2.0: %f*n", g)
}
