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

#include "ImageDXTMisc.hxx"

#include <math.h>

#define ACQUIRE_IMAGE_COLOR_MULTIPLY_BASE_ADDRESS   (0x6000FD57 - 0x60000000)
#define ACQUIRE_IMAGE_COLOR_MULTIPLY_ADDRESS(M)     ((addr)M + (addr)ACQUIRE_IMAGE_COLOR_MULTIPLY_BASE_ADDRESS)

#define TO_GRAY_SCALE_BASE_ADDRESS      (0x6000FC73 - 0x60000000)
#define TO_GRAY_SCALE_ADDRESS(M)        ((addr)M + (addr)TO_GRAY_SCALE_BASE_ADDRESS)

#define FROM_GRAY_SCALE_BASE_ADDRESS    (0x6000FCA5 - 0x60000000)
#define FROM_GRAY_SCALE_ADDRESS(M)      ((addr)M + (addr)TO_GRAY_SCALE_BASE_ADDRESS)

#define MAX_MULTIPLY_VALUE_COUNT    9

using namespace Images;

static const f32 e = 0.001f;

struct ImageColorsMultiplyTest
{
    f32 In[MAX_MULTIPLY_VALUE_COUNT];
    f32 Out[MAX_MULTIPLY_VALUE_COUNT];
};

#define MAX_IMAGE_COLOR_MULTIPLY_TEST_COUNT 4

static const ImageColorsMultiplyTest ImageColorsMultiplyTests[MAX_IMAGE_COLOR_MULTIPLY_TEST_COUNT] =
{
    {
        { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f }
    },
    {
        { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f },
        { 3.0f, 3.0f, 3.0f, 0.0f, 3.0f, 3.0f, 0.0f, 0.0f, 3.0f }
    },
    {
        { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f },
        { 14.0f, 30.0f, 42.0f, 0.0f, 65.0f, 90.0f, 0.0f, 0.0f, 126.0f }
    },
    {
        { 1.0f, 0.0f, -1.0f, 2.0f, 3.0f, -2.0f, 4.0f, 5.0f, -4.0f },
        { 2.0f, 2.0f, 3.0f, 0.0f, 13.0f, 2.0f, 0.0f, 0.0f, 21.0f }
    }
};

void ImageDXTColorMultiplyWrapper(HMODULE m, const f32* input, f32* output)
{
    __asm
    {
        mov eax, input;
        mov ecx, output;

        mov ebx, m;
        add ebx, 0xFD57; // ACQUIRE_IMAGE_COLOR_MULTIPLY_ADDRESS

        call ebx;
    }

    return;
}

BOOL ExecuteImageDXTColorMultiply(HMODULE module, const ImageColorsMultiplyTest* test)
{
    const u32 size = sizeof(f32) * MAX_MULTIPLY_VALUE_COUNT;

    f32 o[MAX_MULTIPLY_VALUE_COUNT];
    ZeroMemory(o, size);

    f32 m[MAX_MULTIPLY_VALUE_COUNT];
    ZeroMemory(m, size);

    ImageDXTColorMultiplyWrapper(module, test->In, o);
    ImageDXTColorMultiply(test->In, m);

    return memcmp(o, m, size) == 0 && memcmp(o, test->Out, size) == 0;
}

struct ToGrayScaleTest
{
    u8  In[3];
    f32 Out[3];
};

#define MAX_TO_GRAY_SCALSE_TEST_COUNT   4

static const ToGrayScaleTest ToGrayScaleTests[MAX_TO_GRAY_SCALSE_TEST_COUNT] =
{
    { { 0x00, 0x00, 0x00 }, { 0.0f, 0.0f, 0.0f } },
    { { 0xFF, 0xFF, 0xFF }, { 0.082f, 0.60939997f, 0.3086f } },
    { { 0xAA, 0x99, 0x18 }, { 0.0546f, 0.3654f, 0.0290f } },
    { { 0x55, 0x77, 0x69 }, { 0.0273f, 0.2843f, 0.1270f } }
};

void ImageDXTColorsToGrayScaleWrapper(HMODULE m, const u8* input, f32* output)
{
    __asm
    {
        mov eax, input;
        push eax;

        mov eax, output;

        mov ecx, m;
        add ecx, 0x0FC73; // TO_GRAY_SCALE_ADDRESS

        call ecx;

        pop eax;
    };

    return;
}

BOOL ExecuteImageDXTColorsToGrayScale(HMODULE module, const ToGrayScaleTest* test)
{
    const u32 size = sizeof(f32) * 3;

    f32 o[3];
    ZeroMemory(o, size);

    f32 m[3];
    ZeroMemory(m, size);

    ImageDXTColorsToGrayScaleWrapper(module, test->In, o);
    ImageDXTColorsToGrayScale(test->In, m);

    for (u32 x = 0; x < 3; x++)
    {
        if (e < fabs(o[x] - m[x])) { return FALSE; }
        if (e < fabs(o[x] - test->Out[x])) { return FALSE; }
    }

    return TRUE;
}

struct FromGrayScaleTest
{
    f32 In[3];
    u8  Out[3];
};

#define MAX_FROM_GRAY_SCALSE_TEST_COUNT 4

static const FromGrayScaleTest FromGrayScaleTests[MAX_FROM_GRAY_SCALSE_TEST_COUNT] =
{
    { { 0.0f, 0.0f, 0.0f }, { 0, 0, 0} },
    { { 1.0f, 1.0f, 1.0f }, { 37, 162, 58 } },
    { { 0.55f, 0.86f, 0.27f }, { 174, 103, 223 } },
    { { 0.12f, 0.69f, 0.37f }, { 117, 32, 49 } }
};

void ImageDXTColorsFromGrayScaleWrapper(HMODULE m, const f32* input, u8* output)
{
    __asm
    {
        mov eax, output;
        push eax;

        mov eax, input;

        mov ecx, m;
        add ecx, 0xFCA5; // FROM_GRAY_SCALE_ADDRESS

        call ecx;

        pop eax;
    };

    return;
}

BOOL ExecuteImageDXTColorsFromGrayScale(HMODULE module, const FromGrayScaleTest* test)
{
    const u32 size = sizeof(u8) * 3;

    u8 o[3];
    ZeroMemory(o, size);

    u8 m[3];
    ZeroMemory(m, size);

    ImageDXTColorsFromGrayScaleWrapper(module, test->In, o);
    ImageDXTColorsFromGrayScale(test->In, m);

    return memcmp(o, m, size) == 0 && memcmp(o, test->Out, size) == 0;
}

BOOL ExecuteImageDXTMisc(HMODULE module)
{
    for (u32 x = 0; x < MAX_IMAGE_COLOR_MULTIPLY_TEST_COUNT; x++)
    {
        if (!ExecuteImageDXTColorMultiply(module, &ImageColorsMultiplyTests[x])) { return FALSE; }
    }

    // To Gray Scale
    for (u32 x = 0; x < MAX_TO_GRAY_SCALSE_TEST_COUNT; x++)
    {
        if (!ExecuteImageDXTColorsToGrayScale(module, &ToGrayScaleTests[x])) { return FALSE; }
    }

    // From Gray Scale
    for (u32 x = 0; x < MAX_FROM_GRAY_SCALSE_TEST_COUNT; x++)
    {
        if (!ExecuteImageDXTColorsFromGrayScale(module, &FromGrayScaleTests[x])) { return FALSE; }
    }

    return TRUE;
}