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

#include "ImageDXTColors.hxx"

using namespace Images;

#define ACQUIRE_IMAGE_PIXEL_BASE_ADDRESS        (0x6000FCDA - 0x60000000)
#define ACQUIRE_IMAGE_PIXEL_ADDRESS(M)          ((addr)M + (addr)ACQUIRE_IMAGE_PIXEL_BASE_ADDRESS)

#define ACQUIREIMAGECOLOR_BASE_ADDRESS          (0x6000FD0A - 0x60000000)
#define ACQUIREIMAGECOLOR_FUNC_ADDRESS(M)       ((addr)M + (addr)ACQUIREIMAGECOLOR_BASE_ADDRESS)

struct ImagePixelTest
{
    ImagePixel  Input;
    u16         Output;
};

#define MAX_IMAGE_PIXEL_TEST_COUNT  10

static const ImagePixelTest ImagePixelTests[MAX_IMAGE_PIXEL_TEST_COUNT] =
{
    { { 0x00, 0x00, 0x00, 0x00 }, 0x0000 },
    { { 0xFF, 0xFF, 0xFF, 0x00 }, 0xFFFF },
    { { 0x52, 0x55, 0xAD, 0x00 }, 0xAAAA },
    { { 0xD6, 0x5D, 0xFF, 0x00 }, 0xFAFA },
    { { 0x29, 0x51, 0x10, 0x00 }, 0x1285 },
    { { 0x00, 0x41, 0xFF, 0x00 }, 0xFA00 },
    { { 0x00, 0x96, 0x08, 0x00 }, 0x0CA0 },
    { { 0x8C, 0xFF, 0x18, 0x00 }, 0x1FF1 },
    { { 0x00, 0x04, 0x21, 0x00 }, 0x2020 },
    { { 0x63, 0xBA, 0xDE, 0x00 }, 0xDDCC }
};

struct ImageColorTest
{
    u16         Input;
    ImagePixel  Output;
};

#define MAX_IMAGE_COLOR_TEST_COUNT  10

static const ImageColorTest ImageColorTests[MAX_IMAGE_COLOR_TEST_COUNT] =
{
    { 0x0000, { 0x00, 0x00, 0x00, 0x00 } },
    { 0xFFFF, { 0xFF, 0xFF, 0xFF, 0x00 } },
    { 0xAAAA, { 0x52, 0x55, 0xAD, 0x00 } },
    { 0xFAFA, { 0xD6, 0x5D, 0xFF, 0x00 } },
    { 0x1285, { 0x29, 0x51, 0x10, 0x00 } },
    { 0xFA00, { 0x00, 0x41, 0xFF, 0x00 } },
    { 0x0CA0, { 0x00, 0x96, 0x08, 0x00 } },
    { 0x1FF1, { 0x8C, 0xFF, 0x18, 0x00 } },
    { 0x2020, { 0x00, 0x04, 0x21, 0x00 } },
    { 0xDDCC, { 0x63, 0xBA, 0xDE, 0x00 } }
};

void AcquireImagePixelWrapper(HMODULE m, const ImagePixel* input, u16* output)
{
    __asm
    {
        mov eax, input;
        mov ebx, output;
        mov ecx, m;

        push ebx;

        add ecx, 0xFCDA; // ACQUIRE_IMAGE_PIXEL_ADDRESS
        call ecx;

        pop ecx;
    }

    return;
}

void AcquireImageColorWrapper(HMODULE m, const u16* input, ImagePixel* output)
{
    __asm
    {
        mov ebx, output;
        mov eax, input;
        mov ecx, m;

        push ecx;
        push ebx;

        add ecx, 0xFD0A; // ACQUIREIMAGECOLOR_FUNC_ADDRESS
        call ecx;

        pop ecx;
        pop ecx;
    }

    return;
}

BOOL ExecuteImageDXTColors(HMODULE module)
{
    for (u32 x = 0; x < MAX_IMAGE_PIXEL_TEST_COUNT; x++)
    {
        u16 o, m;

        AcquireImagePixelWrapper(module, &ImagePixelTests[x].Input, &o);
        AcquireImagePixel(ImagePixelTests[x].Input, &m);

        if (memcmp(&o, &m, sizeof(u16)) != 0
            || memcmp(&o, &ImagePixelTests[x].Output, sizeof(u16) != 0)) {
            return FALSE;
        }
    }

    for (u32 x = 0; x < MAX_IMAGE_COLOR_TEST_COUNT; x++)
    {
        ImagePixel o, m;

        AcquireImageColorWrapper(module, &ImageColorTests[x].Input, &o);
        AcquireImageColor(ImageColorTests[x].Input, &m);

        if (memcmp(&o, &m, sizeof(ImagePixel)) != 0
            || memcmp(&o, &ImageColorTests[x].Output, sizeof(ImagePixel) != 0)) {
            return FALSE;
        }
    }

    return TRUE;
}