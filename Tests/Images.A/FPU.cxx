/*
Copyright (c) 2025 Americus Maximus

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "FPU.hxx"

#include <float.h>

#include <FPU.hxx>

#define FPU_START_ADDRESS   0x6000C7FC
#define FPU_END_ADDRESS     0x6000C81F

#define BASE_ADDRESS        0x60000000

typedef VOID(*FPUSTARTACTION)();
typedef VOID(*FPUENDACTION)();

#define MODE_COUNT          6

BOOL FPU(HMODULE module)
{
    u32 words[MODE_COUNT];
    ZeroMemory(words, MODE_COUNT * sizeof(u32));

    // #

    // Get original x87 control word.
    u32 result = __control87_2(0, 0, &words[0], 0);

    ((FPUSTARTACTION)((addr)module + (addr)(FPU_START_ADDRESS - BASE_ADDRESS)))();

    // Get the x87 control word after modification.
    result = __control87_2(0, 0, &words[1], 0);

    ((FPUENDACTION)((addr)module + (addr)(FPU_END_ADDRESS - BASE_ADDRESS)))();

    // Get the x87 control word after modification.
    result = __control87_2(0, 0, &words[2], 0);

    // #

    result = __control87_2(0, 0, &words[3], 0);

    FPUSTART();

    result = __control87_2(0, 0, &words[4], 0);

    FPUEND();

    result = __control87_2(0, 0, &words[5], 0);

    return words[1] == words[4];
}