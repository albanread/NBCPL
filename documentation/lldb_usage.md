lldb_usage.py

This new tool automates debugging your JIT-compiled code by scripting LLDB to run a test, stop at a key point, and dump the state of the machine.

Purpose
The lldb_script.py file creates a new, custom LLDB command called run_bcpl_test. The purpose of this command is to:
Start your NewBCPL compiler under the debugger.
Automatically set a breakpoint on the C++ runtime function WRITES.
Run the specified BCPL test file.
When the program calls WRITES and hits the breakpoint, it will automatically dump the CPU registers and the top 64 bytes of the stack for inspection.
Finally, it attempts to disassemble a hardcoded region of memory where the JIT code is expected to be.
Usage
You can run the debugging script in two ways: automatically using a source file, or manually by typing the commands into LLDB.

Automatic Execution (Recommended)
This method uses the provided lldb_commands.txt file to run the entire session with a single command.
Ensure the NewBCPL executable, your test file (e.g., test_strings.bcl), and lldb_script.py are all in the same directory.
Create a command file named
lldb_commands.txt with the following content:
command script import lldb_script.py
file ./NewBCPL
settings set -- target.run-args "test_strings.bcl" "--run"
run_bcpl_test

Launch LLDB from your terminal, telling it to execute the commands from the file:
Bash
lldb --source lldb_commands.txt
LLDB will start, load the script, set up the target and arguments, and run the test, printing the register and stack dump when the

WRITES function is hit.

Manual Execution
If you prefer to run the commands step-by-step.
Start LLDB and load the NewBCPL executable:
Bash
lldb ./NewBCPL
Inside LLDB, import the custom Python script:
Code snippet
(lldb) command script import lldb_script.py
Set the command-line arguments for your BCPL test file:
Code snippet
(lldb) settings set -- target.run-args "test_strings.bcl" "--run"
Execute the custom command to run the test:
Code snippet
(lldb) run_bcpl_test
Important Notes
Breakpoint Target: The script always breaks on entry to the WRITES function, not on the BRK instruction in your BCPL code.
Hardcoded Disassembly: The script currently uses a hardcoded address range to disassemble the JIT code. You will need to manually update this in lldb_script.py if the JIT buffer address changes.
