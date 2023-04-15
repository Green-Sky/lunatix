// This file is part of the Luau programming language and is licensed under MIT License; see LICENSE.txt for details
#include "Luau/UnwindBuilderWin.h"

#include <string.h>

// Information about the Windows x64 unwinding data setup can be found at:
// https://docs.microsoft.com/en-us/cpp/build/exception-handling-x64 [x64 exception handling]

#define UWOP_PUSH_NONVOL 0
#define UWOP_ALLOC_LARGE 1
#define UWOP_ALLOC_SMALL 2
#define UWOP_SET_FPREG 3
#define UWOP_SAVE_NONVOL 4
#define UWOP_SAVE_NONVOL_FAR 5
#define UWOP_SAVE_XMM128 8
#define UWOP_SAVE_XMM128_FAR 9
#define UWOP_PUSH_MACHFRAME 10

namespace Luau
{
namespace CodeGen
{

void UnwindBuilderWin::setBeginOffset(size_t beginOffset)
{
    this->beginOffset = beginOffset;
}

size_t UnwindBuilderWin::getBeginOffset() const
{
    return beginOffset;
}

void UnwindBuilderWin::startInfo() {}

void UnwindBuilderWin::startFunction()
{
    // End offset is filled in later and everything gets adjusted at the end
    UnwindFunctionWin func;
    func.beginOffset = 0;
    func.endOffset = 0;
    func.unwindInfoOffset = uint32_t(rawDataPos - rawData);
    unwindFunctions.push_back(func);

    unwindCodes.clear();
    unwindCodes.reserve(16);

    prologSize = 0;

    // rax has register index 0, which in Windows unwind info means that frame register is not used
    frameReg = X64::rax;
    frameRegOffset = 0;

    // Return address was pushed by calling the function
    stackOffset = 8;
}

void UnwindBuilderWin::spill(int espOffset, X64::RegisterX64 reg)
{
    prologSize += 5; // REX.W mov [rsp + imm8], reg
}

void UnwindBuilderWin::save(X64::RegisterX64 reg)
{
    prologSize += 2; // REX.W push reg
    stackOffset += 8;
    unwindCodes.push_back({prologSize, UWOP_PUSH_NONVOL, reg.index});
}

void UnwindBuilderWin::allocStack(int size)
{
    LUAU_ASSERT(size >= 8 && size <= 128 && size % 8 == 0);

    prologSize += 4; // REX.W sub rsp, imm8
    stackOffset += size;
    unwindCodes.push_back({prologSize, UWOP_ALLOC_SMALL, uint8_t((size - 8) / 8)});
}

void UnwindBuilderWin::setupFrameReg(X64::RegisterX64 reg, int espOffset)
{
    LUAU_ASSERT(espOffset < 256 && espOffset % 16 == 0);

    frameReg = reg;
    frameRegOffset = uint8_t(espOffset / 16);

    if (espOffset != 0)
        prologSize += 5; // REX.W lea rbp, [rsp + imm8]
    else
        prologSize += 3; // REX.W mov rbp, rsp

    unwindCodes.push_back({prologSize, UWOP_SET_FPREG, frameRegOffset});
}

void UnwindBuilderWin::finishFunction(uint32_t beginOffset, uint32_t endOffset)
{
    unwindFunctions.back().beginOffset = beginOffset;
    unwindFunctions.back().endOffset = endOffset;

    // Windows unwind code count is stored in uint8_t, so we can't have more
    LUAU_ASSERT(unwindCodes.size() < 256);

    LUAU_ASSERT(stackOffset % 16 == 0 && "stack has to be aligned to 16 bytes after prologue");

    UnwindInfoWin info;
    info.version = 1;
    info.flags = 0; // No EH
    info.prologsize = prologSize;
    info.unwindcodecount = uint8_t(unwindCodes.size());

    LUAU_ASSERT(frameReg.index < 16);
    info.framereg = frameReg.index;

    LUAU_ASSERT(frameRegOffset < 16);
    info.frameregoff = frameRegOffset;

    LUAU_ASSERT(rawDataPos + sizeof(info) <= rawData + kRawDataLimit);
    memcpy(rawDataPos, &info, sizeof(info));
    rawDataPos += sizeof(info);

    if (!unwindCodes.empty())
    {
        // Copy unwind codes in reverse order
        // Some unwind codes take up two array slots, but we don't use those atm
        uint8_t* unwindCodePos = rawDataPos + sizeof(UnwindCodeWin) * (unwindCodes.size() - 1);
        LUAU_ASSERT(unwindCodePos <= rawData + kRawDataLimit);

        for (size_t i = 0; i < unwindCodes.size(); i++)
        {
            memcpy(unwindCodePos, &unwindCodes[i], sizeof(UnwindCodeWin));
            unwindCodePos -= sizeof(UnwindCodeWin);
        }
    }

    rawDataPos += sizeof(UnwindCodeWin) * unwindCodes.size();

    // Size has to be even, but unwind code count doesn't have to
    if (unwindCodes.size() % 2 != 0)
        rawDataPos += sizeof(UnwindCodeWin);

    LUAU_ASSERT(rawDataPos <= rawData + kRawDataLimit);
}

void UnwindBuilderWin::finishInfo() {}

size_t UnwindBuilderWin::getSize() const
{
    return sizeof(UnwindFunctionWin) * unwindFunctions.size() + size_t(rawDataPos - rawData);
}

size_t UnwindBuilderWin::getFunctionCount() const
{
    return unwindFunctions.size();
}

void UnwindBuilderWin::finalize(char* target, size_t offset, void* funcAddress, size_t funcSize) const
{
    // Copy adjusted function information
    for (UnwindFunctionWin func : unwindFunctions)
    {
        // Code will start after the unwind info
        func.beginOffset += uint32_t(offset);

        // Whole block is a part of a 'single function'
        if (func.endOffset == kFullBlockFuncton)
            func.endOffset = uint32_t(funcSize);
        else
            func.endOffset += uint32_t(offset);

        // Unwind data is placed right after the RUNTIME_FUNCTION data
        func.unwindInfoOffset += uint32_t(sizeof(UnwindFunctionWin) * unwindFunctions.size());
        memcpy(target, &func, sizeof(func));
        target += sizeof(func);
    }

    // Copy unwind codes
    memcpy(target, rawData, size_t(rawDataPos - rawData));
}

} // namespace CodeGen
} // namespace Luau
