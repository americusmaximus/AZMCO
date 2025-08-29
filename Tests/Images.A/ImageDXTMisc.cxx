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

#define TO_GRAY_SCALE_BASE_ADDRESS          (0x6000FC73 - 0x60000000)
#define TO_GRAY_SCALE_ADDRESS(M)            ((addr)M + (addr)TO_GRAY_SCALE_BASE_ADDRESS)

#define FROM_GRAY_SCALE_BASE_ADDRESS        (0x6000FCA5 - 0x60000000)
#define FROM_GRAY_SCALE_ADDRESS(M)          ((addr)M + (addr)FROM_GRAY_SCALE_BASE_ADDRESS)

#define MIX_GRAY_SCALE_COLORS_BASE_ADDRESS  (0x6000FE81 - 0x60000000)
#define MIX_GRAY_SCALE_COLORS_ADDRESS(M)    ((addr)M + (addr)MIX_GRAY_SCALE_COLORS_BASE_ADDRESS)

#define SHADE_COLORS_BASE_ADDRESS           (0x6000FDF6 - 0x60000000)
#define SHADE_COLORS_ADDRESS(M)             ((addr)M + (addr)SHADE_COLORS_BASE_ADDRESS)

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
    const u32 size = sizeof(f32) * RGB_COLOR_COUNT;

    f32 o[RGB_COLOR_COUNT];
    ZeroMemory(o, size);

    f32 m[RGB_COLOR_COUNT];
    ZeroMemory(m, size);

    ImageDXTColorsToGrayScaleWrapper(module, test->In, o);
    ImageDXTColorsToGrayScale(test->In, m);

    for (u32 x = 0; x < RGB_COLOR_COUNT; x++)
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
    const u32 size = sizeof(u8) * RGB_COLOR_COUNT;

    u8 o[RGB_COLOR_COUNT];
    ZeroMemory(o, size);

    u8 m[RGB_COLOR_COUNT];
    ZeroMemory(m, size);

    ImageDXTColorsFromGrayScaleWrapper(module, test->In, o);
    ImageDXTColorsFromGrayScale(test->In, m);

    return memcmp(o, m, size) == 0 && memcmp(o, test->Out, size) == 0;
}

void MixGrayScaleColorsWrapper(HMODULE m, f32* a, f32* b)
{
    __asm
    {
        mov edx, a;
        mov edi, b;

        mov ecx, m;
        add ecx, 0xFE81; // MIX_GRAY_SCALE_COLORS_ADDRESS

        call ecx;
    };

    return;
}

#define MAX_MIX_GRAY_SCALE_TEST_COUNT   4

struct MixGrayScaleColorsTest
{
    f32 InA[RGB_COLOR_COUNT];
    f32 InB[RGB_COLOR_COUNT];
    f32 OutA[RGB_COLOR_COUNT];
    f32 OutB[RGB_COLOR_COUNT];
};

static const MixGrayScaleColorsTest MixGrayScaleColorsTests[MAX_MIX_GRAY_SCALE_TEST_COUNT] =
{
    { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
    { { 17.0f, 0.19f, 0.55f }, { -145.0f, 0.77f, 0.87f }, { 0.082f, 0.25f, 0.583f }, { 0.0f, 0.25f, 0.583f } },
    { { -17.0f, -0.19f, 0.55f }, { 16.5f, 0.77f, 1.87f }, { 0.0f, 0.297f, 1.219f }, { 0.082f, 0.299f, 1.223f } }
};

BOOL ExecuteMixGrayScaleColors(HMODULE module, const MixGrayScaleColorsTest* test)
{
    const u32 size = sizeof(f32) * RGB_COLOR_COUNT;

    f32 ao[RGB_COLOR_COUNT];
    CopyMemory(ao, test->InA, size);

    f32 bo[RGB_COLOR_COUNT];
    CopyMemory(bo, test->InB, size);

    MixGrayScaleColorsWrapper(module, ao, bo);

    f32 am[RGB_COLOR_COUNT];
    CopyMemory(am, test->InA, size);

    f32 bm[RGB_COLOR_COUNT];
    CopyMemory(bm, test->InB, size);

    MixGrayScaleColors(am, bm);

    for (u32 x = 0; x < RGB_COLOR_COUNT; x++)
    {
        if (e < fabs(ao[x] - am[x])) { return FALSE; }
        if (e < fabs(bo[x] - bm[x])) { return FALSE; }
        if (e < fabs(am[x] - test->OutA[x])) { return FALSE; }
        if (e < fabs(bm[x] - test->OutB[x])) { return FALSE; }
    }

    return TRUE;
}

struct ShadeColorsTest
{
    u16 InPixels[2];
    f32 InA[RGB_COLOR_COUNT];
    f32 InB[RGB_COLOR_COUNT];
    u32 Count;

    u16 OutPixels[2];
    f32 OutA[RGB_COLOR_COUNT];
    f32 OutB[RGB_COLOR_COUNT];
};

#define MAX_SHADE_COLOR_TEST_COUNT 4

static const ShadeColorsTest ShadeColorsTests[MAX_SHADE_COLOR_TEST_COUNT] =
{
    {
        { 0x00FF, 0xABCD }, { 0.5f, 0.12f, 0.87f }, { 0.63f, 0.71f, 0.66f },
        0,
        { 8532, 51586 }, { 0.053f, 0.095f, 0.039f }, { 0.005f, 0.114f, 0.249f }
    },
    {
        { 0x00FF, 0xABCD }, { 0.5f, 0.12f, 0.87f }, { 0.63f, 0.71f, 0.66f },
        16,
        { 51586, 8532 }, { 0.005f, 0.114f, 0.249f }, { 0.053f, 0.095f, 0.039f }
    },
    {
        { 0xF66A, 0x1818 }, { 0.75f, 0.48f, 0.27f }, { 1.0f, 0.73f, 0.19f },
        4,
        { 39300, 56899 }, { 0.01f, 0.114f, 0.188f }, { 0.007f, 0.485f, 0.268f }
    },
    {
        { 0, 0 }, { 0.0f, 0.0f, 0.0f }, { 0.0228524953f, 0.609400034f, 0.152350008f },
        16,
        { 32744, 0 }, { 0.0212235320f, 0.609399974f, 0.148854136f }, { 0.0f, 0.0f, 0.0f }
    }
};

void ImageDXTShadeColorsWrapper(HMODULE m, u16* pixels, f32* a, f32* b, u32 count)
{
    __asm
    {
        mov esi, pixels;

        push count;
        push b;
        push a;

        mov ecx, m;
        add ecx, 0xFDF6; // SHADE_COLORS_ADDRESS

        call ecx;

        pop eax;
        pop eax;
        pop eax;
    };

    return;
}

BOOL ExecuteImageDXTShadeColors(HMODULE module, const ShadeColorsTest* test)
{
    u16 p1[2];
    CopyMemory(p1, test->InPixels, 2 * sizeof(u16));

    f32 a1[RGB_COLOR_COUNT];
    CopyMemory(a1, test->InA, RGB_COLOR_COUNT * sizeof(f32));

    f32 b1[RGB_COLOR_COUNT];
    CopyMemory(b1, test->InB, RGB_COLOR_COUNT * sizeof(f32));

    ImageDXTShadeColors(p1, a1, b1, test->Count);

    u16 p2[2];
    CopyMemory(p2, test->InPixels, 2 * sizeof(u16));

    f32 a2[RGB_COLOR_COUNT];
    CopyMemory(a2, test->InA, RGB_COLOR_COUNT * sizeof(f32));

    f32 b2[RGB_COLOR_COUNT];
    CopyMemory(b2, test->InB, RGB_COLOR_COUNT * sizeof(f32));

    ImageDXTShadeColorsWrapper(module, p2, a2, b2, test->Count);

    if (memcmp(p1, p2, 2 * sizeof(u16)) != 0 || memcmp(p2, test->OutPixels, 2 * sizeof(u16)) != 0) { return FALSE; }

    for (u32 x = 0; x < RGB_COLOR_COUNT; x++)
    {
        if (e < fabs(a1[x] - a2[x])) { return FALSE; }
        if (e < fabs(b1[x] - b2[x])) { return FALSE; }
        if (e < fabs(a2[x] - test->OutA[x])) { return FALSE; }
        if (e < fabs(b2[x] - test->OutB[x])) { return FALSE; }
    }

    return TRUE;
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

    // Mixing
    for (u32 x = 0; x < MAX_MIX_GRAY_SCALE_TEST_COUNT; x++)
    {
        if (!ExecuteMixGrayScaleColors(module, &MixGrayScaleColorsTests[x])) { return FALSE; }
    }

    // Shading
    for (u32 x = 0; x < MAX_SHADE_COLOR_TEST_COUNT; x++)
    {
        if(!ExecuteImageDXTShadeColors(module, &ShadeColorsTests[x])) { return FALSE; }
    }

    return TRUE;
}