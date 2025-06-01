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

#include "BitMap.hxx"
#include "ImageBitMap.hxx"

#include <FPU.cxx>
#include <RendererValues.cxx>
#include <Images.cxx>

BOOL AcquireImage(void* pixels, const ColorSchema schema)
{
    switch (schema)
    {
    case COLORSCHEMA_BLACK: { ZeroMemory(pixels, IMAGE_SIZE); break; }
    case COLORSCHEMA_WHITE: { FillMemory(pixels, 0xFF, IMAGE_SIZE); break; }
    case COLORSCHEMA_REPEAT:
    {
        u8* colors = (u8*)pixels;

        for (u32 x = 0; x < IMAGE_DIMS; x++)
        {
            for (u32 xx = 0; xx < IMAGE_DIMS * IMAGE_BPP; xx = xx + IMAGE_BPP)
            {
                u8* pix = (u8*)((addr)colors + (addr)xx);

                pix[0] = x % 256;
                pix[1] = (xx + 0) % 256;
                pix[2] = (xx + 1) % 256;
                pix[3] = (xx + 2) % 256;
            }

            colors = (u8*)((addr)colors + (addr)(IMAGE_DIMS * IMAGE_BPP));
        }

        break;
    }
    case COLORSCHEMA_IMAGE:
    {
        ZeroMemory(pixels, IMAGE_SIZE);

        return LoadPixels(IMAGE_NAME, (u32*)pixels, NULL, NULL);
    }
    default: { return FALSE; }
    }

    return TRUE;
}

BOOL AcquirePalette(void* palette)
{
    if (palette == NULL) { return FALSE; }

    for (u32 x = 0; x < 256; x++)
    {
        u8* color = (u8*)((addr)palette + x * 4);

        color[0] = color[1] = color[2] = color[3] = x;
    }

    return TRUE;
}

BOOL AcquireColors(ImageColor* colors, void* pixels, const u32 width, const u32 height, const BOOL generate)
{
    if (colors == NULL) { return FALSE; }

    if (generate)
    {
        for (u32 x = 0; x < height; x++)
        {
            ImageColor* row = (ImageColor*)((addr)colors + (addr)(x * width * sizeof(ImageColor)));

            for (u32 xx = 0; xx < width; xx++)
            {
                const f32 c = (f32)xx / (f32)width;

                row[xx].A = row[xx].R = row[xx].G = row[xx].B = c;
            }
        }
    }
    else
    {
        // Copy pixels as A8R8G8B8

        for (u32 x = 0; x < height; x++)
        {
            u32* pix = (u32*)((addr)pixels + (addr)(x * IMAGE_DIMS * IMAGE_BPP));
            ImageColor* row = (ImageColor*)((addr)colors + (addr)(x * width * sizeof(ImageColor)));

            for (u32 xx = 0; xx < width; xx++)
            {
                const u32 c = pix[xx];

                row[xx].A = ((c >> 24) & 0xFF) / 255.0f;
                row[xx].R = ((c >> 16) & 0xFF) / 255.0f;
                row[xx].G = ((c >> 8) & 0xFF) / 255.0f;
                row[xx].B = ((c >> 0) & 0xFF) / 255.0f;
            }
        }
    }

    return TRUE;
}
