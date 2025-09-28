NewBCPL/CallFrameManager.md
```

```
# CallFrameManager Class Documentation

## Overview

The `CallFrameManager` class is responsible for managing the stack frame layout and generating prologue and epilogue code for functions. It ensures proper alignment, handles local variable declarations, and manages callee-saved registers.

---

## Key Features

- **Stack Frame Layout**: Calculates offsets for local variables and saved registers.
- **Prologue Generation**: Allocates stack space, saves registers, and sets up the frame pointer.
- **Epilogue Generation**: Restores registers, deallocates stack space, and prepares the return value.
- **Debugging**: Provides detailed debug output for stack frame layout and operations.

---

## Methods

### `CallFrameManager(RegisterManager& reg_manager, bool debug)`
Constructor that initializes the `CallFrameManager` with a reference to the `RegisterManager` and a debug flag.

### `void add_local(const std::string& variable_name, int size_in_bytes)`
Adds a local variable to the stack frame. The size must be a multiple of 8 bytes.

### `int get_offset(const std::string& variable_name) const`
Returns the stack offset for a given local variable. Throws an error if the prologue has not been generated.

### `std::vector<Instruction> generate_prologue()`
Generates the prologue code for the function, including saving registers and setting up the frame pointer.

### `std::vector<Instruction> generate_epilogue()`
Generates the epilogue code for the function, including restoring registers and deallocating stack space.

### `std::string display_frame_layout() const`
Returns a string representation of the stack frame layout, including offsets for variables and saved registers.

---

## Debugging

The `CallFrameManager` provides detailed debug output when the debug flag is enabled. Debug messages are logged to `std::cerr` and include information about local variable declarations, stack frame size, and register operations.

---

## Example Usage

```cpp
RegisterManager reg_manager;
CallFrameManager frame_manager(reg_manager, true);

// Add local variables
frame_manager.add_local("var1", 8);
frame_manager.add_local("var2", 16);

// Generate prologue and epilogue
std::vector<Instruction> prologue = frame_manager.generate_prologue();
std::vector<Instruction> epilogue = frame_manager.generate_epilogue();

// Display frame layout
std::cout << frame_manager.display_frame_layout() << std::endl;
```

---

## Notes

- Local variable sizes must be a multiple of 8 bytes to ensure proper alignment.
- The prologue must be generated before accessing variable offsets or generating the epilogue.
- The class assumes a 16-byte alignment for the stack frame.

---

## Dependencies

- `RegisterManager`: Manages register allocation and usage.
- `Encoder`: Generates machine instructions for stack operations.

---

## Conclusion

The `CallFrameManager` class is a critical component for managing function stack frames in the `NewBCPL` project. Its modular design and debugging capabilities make it a robust solution for stack management.