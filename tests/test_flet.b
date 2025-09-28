// Test file for FLET register allocation
// This tests whether FLET initialization properly uses allocated registers

LET START() BE
{
    FLET x = 3.14
    FLET y = 2.71
    FLET z = 1.41

    // Do some operations with these variables to ensure they stay live
    // and hopefully get allocated to registers (especially D8-D15)
    LET total = 0.0

    // Repeat operations to increase register pressure
    FOR i = 1 TO 5 DO
    {
        // Mix of operations to encourage register allocation
        total := total + x + y + z
        x := x + 0.1
        y := y + 0.2
        z := z + 0.3
        WRITEF("Values: %f %f %f, Total: %f*n", x, y, z, total)
    }

    WRITEF("Final values: x=%f, y=%f, z=%f, total=%f*n", x, y, z, total)
}
