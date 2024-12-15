#include "MainHeader.h"
#include "InitHooks.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstring>  // For std::memset

namespace DiaCondExtended
{
    // Constants for the various actor value strings
    const char MWString[] = "Melee Weapons";
    const char EWString[] = "Energy Weapons";
    const char unknown[] = "unknown";

    UInt32 SkillCheckVar = 0; // Actor Value Code (AV Code)
    char newSkillString[516];  // Buffer for formatted skill check string
    char customOffset[8] = "%s[%s] ";
    char customOffset1[7] = "%s%s"; // Format string for success case

    // Returns a string representation of the Actor Value (AV) code
    const char* GetSkillCheckString()
    {
        if (SkillCheckVar <= eActorVal_FalloutMax)
        {
            if (SkillCheckVar == 0x26) { return MWString; }
            if (SkillCheckVar == 0x22) { return EWString; }

            return GetActorValueString(SkillCheckVar);  // Fetch the Actor Value string
        }
        return unknown;
    }

    // Hook to grab the AV code from the ECX register and store it in SkillCheckVar
    _declspec(naked) void __fastcall GetAvCodeHook()
    {
        static const UInt32 returnAddr = 0x763C40;

        _asm
        {
            mov[ebp - 0x23C], ecx      // Save ECX value
            mov SkillCheckVar, ecx     // Store ECX value as the AV code
            jmp returnAddr            // Return to original function
        }
    }

    // Clears the string buffer at the start of processing
    _declspec(naked) void __fastcall WipeBuffer()
    {
        static const UInt32 returnAddr = 0x763BD9;

        _asm
        {
            lea edi, [ebp - 0x208]   // Load address of string buffer
            mov ecx, 0x200           // Set buffer size
            xor eax, eax             // Clear eax register (used by rep stosb)
            rep stosb                // Fill string buffer with zeros
            jmp returnAddr           // Return to original function
        }
    }

    // Modifies the failure string construction
    _declspec(naked) void __stdcall StringModFail()
    {
        static const UInt32 returnAddr = 0x763CB7;
        static const UInt32 offsetAddr = 0x01072850;

        _asm
        {
            call GetSkillCheckString  // Get AV code string
            push eax                  // Push AV code string to the stack
            push offsetAddr           // Push format string for failure
            push 0x200                // Buffer size for formatted string
            lea eax, newSkillString   // Load address of newSkillString
            push eax                  // Push buffer for formatted string
            call FormatString         // Format the failure string

            add esp, 0x18             // Clean up the stack (4 arguments)
            lea eax, newSkillString   // Load the concatenated failure string
            push eax                  // Push formatted string
            lea edx, [ebp - 0x208]    // Load address of string buffer
            push edx                  // Push string buffer to the stack
            push offset customOffset  // Push format string for success message
            push 0x200                // Buffer size for formatted string
            lea eax, [ebp - 0x208]    // Load address of string buffer
            push eax                  // Push string buffer
            call FormatString         // Format the success string

            add esp, 0x14             // Clean up the stack
            jmp returnAddr           // Return to the original function
        }
    }

    // Modifies the success string construction
    _declspec(naked) void __stdcall StringModPass()
    {
        static const UInt32 returnAddr = 0x763CE4;
        static const UInt32 offsetAddr = 0x0104700C;

        _asm
        {
            call GetSkillCheckString  // Get AV code string
            push eax                  // Push AV code string to the stack
            push offsetAddr           // Push format string for success
            push 0x200                // Buffer size for formatted string
            lea eax, newSkillString   // Load address of newSkillString
            push eax                  // Push buffer for formatted string
            call FormatString         // Format the success string

            add esp, 0x14             // Clean up the stack
            lea eax, newSkillString   // Load the concatenated success string
            push eax                  // Push formatted string
            lea edx, [ebp - 0x208]    // Load address of string buffer
            push edx                  // Push string buffer to the stack
            push offset customOffset  // Push format string for success message
            push 0x200                // Buffer size for formatted string
            lea eax, [ebp - 0x208]    // Load address of string buffer
            push eax                  // Push string buffer
            call FormatString         // Format the string buffer

            jmp returnAddr           // Return to the original function
        }
    }

    // Removes the appended ']' at the end of the skill tag
    _declspec(naked) void __fastcall EndModString()
    {
        static const UInt32 returnAddr = 0x763D1E;
        _asm
        {
            push offset customOffset1  // Push the modified format string
            jmp returnAddr            // Return to the original function
        }
    }

    // Initializes all hooks for modifying dialogue conditions
    void InitHooks()
    {
        // Memory addresses for hook locations
        const UInt32 avHookAddr = 0x763C3A;
        const UInt32 loopStartAddr = 0x763BD3;
        const UInt32 failStrAddr = 0x763C97;
        const UInt32 passStrAddr = 0x763CC7;
        const UInt32 endFormatAddr = 0x763D19;

        // Break addresses for loop control
        const ULONG_PTR breakAddr1 = 0x763CE7;
        const ULONG_PTR breakAddr2 = 0x763CE8;

        // Write the hooks into the game code
        WriteRelJump(avHookAddr, UInt32(GetAvCodeHook));
        WriteRelJump(loopStartAddr, UInt32(WipeBuffer));
        WriteRelJump(failStrAddr, UInt32(StringModFail));
        WriteRelJump(passStrAddr, UInt32(StringModPass));
        WriteRelJump(endFormatAddr, UInt32(EndModString));

        // Remove the loop break instructions to modify loop behavior
        PatchMemoryNop(breakAddr1, 1);
        PatchMemoryNop(breakAddr2, 1);
    }
}
