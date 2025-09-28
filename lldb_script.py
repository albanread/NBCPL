import lldb
import os

def run_bcpl_test(debugger, command, result, internal_dict):
    """
    LLDB command to run a BCPL program. If the program crashes or hits a breakpoint, 
    it prints registers and disassembles 100 instructions around the program counter.
    """

    target = debugger.GetSelectedTarget()
    if not target or not target.IsValid() or not target.GetExecutable().GetFilename():
        res = lldb.SBCommandReturnObject()
        debugger.GetCommandInterpreter().HandleCommand("file ./NewBCPL", res)
        if not res.Succeeded():
            result.AppendMessage("Error: Could not set target to ./NewBCPL")
            return
        target = debugger.GetSelectedTarget()

    # Set synchronous mode to get process updates
    debugger.SetAsync(False)

    # Run the program
    debugger.HandleCommand("run")

    process = target.GetProcess()
    if process and process.IsValid() and process.GetState() == lldb.eStateStopped:
        thread = process.GetSelectedThread()
        if thread and thread.IsValid():
            stop_reason = thread.GetStopReason()
            
            is_breakpoint = stop_reason == lldb.eStopReasonBreakpoint
            if not is_breakpoint and stop_reason == lldb.eStopReasonException:
                stop_description = thread.GetStopDescription(256)
                if "EXC_BREAKPOINT" in stop_description or "breakpoint" in stop_description.lower():
                    is_breakpoint = True

            if is_breakpoint:
                result.AppendMessage("Program hit a breakpoint. Dumping state:")
            elif stop_reason == lldb.eStopReasonException:
                result.AppendMessage("Program crashed. Dumping state:")
            else:
                result.AppendMessage(f"Program stopped for reason: {lldb.SBDebugger.StopReasonAsString(stop_reason)}")
                return # No dump for other reasons

            # Print registers
            result.AppendMessage("\n[Registers]\n")
            frame = thread.GetFrameAtIndex(0)
            if frame.IsValid():
                for regset in frame.GetRegisters():
                    result.AppendMessage(f"{regset.GetName()}:")
                    for reg in regset:
                        result.AppendMessage(f"  {reg.GetName()}: {reg.GetValue()}")
            else:
                result.AppendMessage("Error: Could not get frame to read registers.")

            # Disassemble around PC
            result.AppendMessage("\n[Disassembly around PC]\n")
            pc_address = frame.GetPC()
            if pc_address != lldb.LLDB_INVALID_ADDRESS:
                # Disassemble 100 instructions around PC
                # This is a simplified approach; a more robust solution would involve
                # calculating start/end addresses based on instruction size and PC.
                # For now, we'll just disassemble from PC.
                instructions = target.ReadInstructions(lldb.SBAddress(pc_address, target), 100)
                for instr in instructions:
                    result.AppendMessage(str(instr))
            else:
                result.AppendMessage("Error: Could not get PC address for disassembly.")

        else:
            result.AppendMessage("Error: No valid thread when stopped.")
    elif process and process.GetState() == lldb.eStateExited:
        result.AppendMessage("Program exited normally.")
    else:
        result.AppendMessage("Error: Program did not run or stop as expected.")


def __lldb_init_module(debugger, internal_dict):
    """
    LLDB initialization function to register the custom command.
    """
    debugger.HandleCommand(
        'command script add -f lldb_script.run_bcpl_test run_bcpl_test'
    )
    print("The 'run_bcpl_test' command has been installed.")