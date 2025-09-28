// Test with explicit execution order
// This ensures the operations happen in the correct sequence

LET START() BE
{
    // Initial value
    FLET f = 1.0

    // First operation: add 2.0 to f
    f := f +# 2.0

    // Explicitly print the intermediate value to verify it's 3.0
    WRITEF("After addition: %f*n", f)

    // Second operation: multiply f by 10.0 and store in result
    FLET result = f *# 10.0

    // Print the final result to verify it's 30.0
    WRITEF("Final result: %f*n", result)
}
