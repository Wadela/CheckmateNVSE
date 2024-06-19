#include "MainHeader.h"
#include "InitHooks.h"

#include <iostream>
#include <sstream>
#include <iomanip>

UInt32 SkillCheckVar = 0;
char newSkillString[516];
char customOffset[8] = "%s[%s] ";
char customOffset1[7] = "%s%s";
char MWString[14] = "Melee Weapons";
char EWString[15] = "Energy Weapons";

namespace DiaCondExtended
{

    //Takes the Actor Value Code stored in SkillCheckVar and converts it to a string
    const char* GetSkillCheckString()
    {
        const char* name = nullptr;
        if (SkillCheckVar <= eActorVal_FalloutMax)
        {
            //I'm so, so sorry if you speak another language, I'm lazy AF
            //If either MeleeWeapons or EnergyWeapons, changes the string to have a space
            if (SkillCheckVar == 0x26)
            {
                name = MWString;
            }
            else if (SkillCheckVar == 0x22)
            {
                name = EWString;
            }

            else
            {
                name = GetActorValueString(SkillCheckVar);
            }
        }
        else
        {
            name = "unknown";
        }
        return name;

    }

    //Grabs the AVcode from the ECX register at this address and stores it in SkillCheckVar
    _declspec(naked) void __fastcall GetAvCodeHook()
    {
        static const UInt32 returnAddr = 0x763C40;
        _asm
        {
            mov [ebp - 0x23C], ecx //Rewriting Overwritten Address
            mov SkillCheckVar, ecx //ECX = AvCode --> SkillCheckVar
            jmp returnAddr
        }
    }

    //Wipes the string buffer before the loop starts, so that the 'AV/Perk' category in the GECK is irrelevant 
    _declspec(naked) void __fastcall WipeBuffer()
    {
        static const UInt32 FormatString = 0x406D00;
        static const UInt32 returnAddr = 0x763BD9;
        _asm
        {
            mov [ebp - 0x230], eax  //Rewriting Overwritten Address
            lea edi, [ebp - 0x208]  
            mov ecx, 0x200
            xor eax, eax
            rep stosb               //Cleared StringBuffer
            jmp returnAddr

        }
    }

    //Modifies the construction of the failure string.
    _declspec(naked) void __fastcall StringModFail()
    {
        static const UInt32 FormatString = 0x406D00;
        static const UInt32 offsetAddr = 0x01072850;
        static const UInt32 returnAddr = 0x763CB7;
        _asm
        {
            call GetSkillCheckString
            push eax                            //eax = AvCode
            push offsetAddr                     //"%s %d/%d"
            push 0x200
            lea eax, [ebp + newSkillString]
            push eax                          
            call FormatString                   //FormatString(newSkillString, 512, "%s %d/%d", AvCode, ClampedActorValueI, ComparisonValue)
            add esp, 0x18
            lea eax, [ebp + newSkillString]     //Concatenated String 
            push eax
            lea edx, [ebp - 0x208]              //StringBuffer
            push edx
            push offset customOffset            //"%s[%s] "
            push 0x200
            lea eax, [ebp - 0x208]
            push eax
            call FormatString                   //FormatString(StringBuffer, 512, "%s[%s] ", StringBuffer, newSkillString)
            add esp, 0x14
            jmp returnAddr
        }
    }

    //Modifies the construction of the success string.
    _declspec(naked) void __fastcall StringModPass()
    {
        static const UInt32 FormatString = 0x406D00;
        static const UInt32 offsetAddr = 0x0104700C;
        static const UInt32 returnAddr = 0x763CE4;
        _asm
        {
            call GetSkillCheckString
            push eax                            //eax = AvCode
            push offsetAddr                     //"%s %d"
            push 0x200
            lea eax, [ebp + newSkillString]
            push eax
            call FormatString                   //FormatString(newSkillString, 512, "%s %d", AvCode, ComparisonValue)
            add esp, 0x14                      
            lea eax, [ebp + newSkillString]     //Concatenated String
            push eax
            lea edx, [ebp - 0x208]              //StringBuffer
            push edx
            push offset customOffset            //"%s[%s] "
            push 0x200
            lea eax, [ebp - 0x208]
            push eax
            call FormatString                   //FormatString(StringBuffer, 512, "%s[%s] ", StringBuffer, newSkillString)
            jmp returnAddr
        }
    }

    //Removes the appended ']' at the end of the skill tag
    _declspec(naked) void __fastcall EndModString()
    {
        static const UInt32 returnAddr = 0x763D1E;
        _asm
        {
            push offset customOffset1
            jmp returnAddr
        }
    }

    //Function to initialize hooks
    void InitHooks()
    {
        UInt32 avHookAddr = 0x763C3A;       //This is an Address where the ECX Register = AvCode
        UInt32 loopStartAddr = 0x763BD3;    //This Address is the start of the For Loop 
        UInt32 failStrAddr = 0x763C97;      //This Address is for the String Buffer right before the string is formatted (Failure Condition) 
        UInt32 passStrAddr = 0x763CC7;      //This Address is for the String Buffer right before the string is formatted (Success Condition) 
        UInt32 endFormatAddr = 0x763D19;    //THis Address pushes the string definition "%s]  %s" as an offset to close the Skill Tag up; ']'

        ULONG_PTR breakAddr1 = 0x763CE7;  //breakAddr1 and 2 are responsible for the break at the end of the for loop 
        ULONG_PTR breakAddr2 = 0x763CE8;

        ULONG_PTR loopStartAddrEnd = 0x763BD8; //Final byte of loopStartAddr
        ULONG_PTR failStrAddrEnd = 0x763C9C;   //Final byte of failStrAddr
        ULONG_PTR passStrAddrEnd = 0x763CCC;   //Final byte of passStrAddr

        //Extend Dialogue Conditions
        WriteRelJump(avHookAddr, UInt32(GetAvCodeHook)); 
        WriteRelJump(loopStartAddr, UInt32(WipeBuffer)); 
        WriteRelJump(failStrAddr, UInt32(StringModFail));
        WriteRelJump(passStrAddr, UInt32(StringModPass)); 
        WriteRelJump(endFormatAddr, UInt32(EndModString));

        //NOP excess byte from original address (Original 6 --> New 5 bytes)
        PatchMemoryNop(loopStartAddrEnd, 1); 
        PatchMemoryNop(failStrAddrEnd, 1);  
        PatchMemoryNop(passStrAddrEnd, 1); 

        //Remove loop break
        PatchMemoryNop(breakAddr1, 1);  
        PatchMemoryNop(breakAddr2, 1);  
    }
}
