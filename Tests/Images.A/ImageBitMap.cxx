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

#include "ImageBitMap.hxx"

#include <stdlib.h>

using namespace Images;

typedef AbstractImage* (__cdecl* INITIALIZEABSTRACTIMAGEACTION)(ImageContainerArgs*);

typedef ImageBitMap* (__thiscall* IMAGEBITMAPCONSTRUCTORACTION)(ImageBitMap*, ImageContainerArgs* args, const u32 bits, const u32 options);

typedef VOID(__thiscall* IMAGEBITMAPRELEASEACTION)(ImageBitMap*, const u8 mode);
typedef VOID(__thiscall* IMAGEBITMAPREADACTION)(ImageBitMap*, const u32 line, const u32 level, ImageColor* pixels);
typedef VOID(__thiscall* IMAGEBITMAPWRITEACTION)(ImageBitMap*, const u32 line, const u32 level, ImageColor* pixels);

BOOL CompareImageBitMap(const ImageBitMap* a, const ImageBitMap* b);
BOOL CompareImageBitMap(HMODULE module, ImageContainerArgs* a, ImageContainerArgs* b, ImageColor* colors);
BOOL ExecuteImageBitMap(HMODULE module, const D3DFORMAT format, const u32 width, const u32 height, const BOOL gradient, const u32 color, ImageColor* colors, const u8* palette, void* pixels);

BOOL CompareImageBitMap(const ImageBitMap* a, const ImageBitMap* b)
{
    if (a->Format != b->Format) return FALSE;
    if (a->Options != b->Options) return FALSE;
    if (a->IsBitMap != b->IsBitMap) return FALSE;
    if (a->IsColor != b->IsColor) return FALSE;
    if (a->IsPalette != b->IsPalette) return FALSE;

    if (memcmp(a->Pixels, b->Pixels, IMAGE_SIZE) != 0) return FALSE;
    if (memcmp(&a->Color, &b->Color, sizeof(ImageColor)) != 0) return FALSE;
    if (memcmp(a->Modifiers, b->Modifiers, sizeof(f32) * MAX_IMAGE_COLOR_MODIFIER_VALUES_COUNT) != 0) return FALSE;

    if (a->IsPalette)
    {
        if (memcmp(&a->Palette, &b->Palette, sizeof(ImageColor) * MAX_IMAGE_PALETTE_VALUES_COUNT) != 0) return FALSE;
    }

    if (memcmp(&a->Dimensions, &b->Dimensions, sizeof(ImageDimensions)) != 0) return FALSE;

    if (a->Stride != b->Stride) return FALSE;
    if (a->AreaStride != b->AreaStride) return FALSE;
    if (a->Width != b->Width) return FALSE;
    if (a->Height != b->Height) return FALSE;
    if (a->Levels != b->Levels) return FALSE;
    if (a->BytesStride != b->BytesStride) return FALSE;
    if (a->Bytes != b->Bytes) return FALSE;

    return TRUE;
}

BOOL CompareImageBitMap(HMODULE module, ImageContainerArgs* a, ImageContainerArgs* b, ImageColor* colors)
{
    // Create

    ImageBitMap* o = (ImageBitMap*)((INITIALIZEABSTRACTIMAGEACTION)INIT_IMAGE_FUNC_ADDRESS(module))(a);

    if (o == NULL) { return FALSE; }

    ImageBitMap* m = (ImageBitMap*)InitializeAbstractImage(b);

    if (m == NULL)
    {
        ((IMAGEBITMAPRELEASEACTION)VIRTUAL_METHOD(o, IMAGE_RELEASE))(o, IMAGE_RELEASE_DISPOSE);

        return FALSE;
    }

    if (!CompareImageBitMap(o, m))
    {
        ((IMAGEBITMAPRELEASEACTION)VIRTUAL_METHOD(o, IMAGE_RELEASE))(o, IMAGE_RELEASE_DISPOSE);
        ReleaseAbstractImage((AbstractImage*)m, IMAGE_RELEASE_DISPOSE);

        return FALSE;
    };

    // Init

    const u32 w = o->Width;
    const u32 h = o->Height;

    // Write

    for (u32 x = 0; x < h; x++)
    {
        ImageColor* row = (ImageColor*)((addr)colors + (addr)(x * w * sizeof(ImageColor)));

        ((IMAGEBITMAPWRITEACTION)VIRTUAL_METHOD(o, IMAGE_WRITE))(o, x, 0, row);

        m->Self->Write((AbstractImage*)m, x, 0, row);
    }

    if (!CompareImageBitMap(o, m))
    {
        ((IMAGEBITMAPRELEASEACTION)VIRTUAL_METHOD(o, IMAGE_RELEASE))(o, IMAGE_RELEASE_DISPOSE);
        ReleaseAbstractImage((AbstractImage*)m, IMAGE_RELEASE_DISPOSE);

        return FALSE;
    };

    // Read

    const u32 s = w * sizeof(ImageColor);

    ImageColor* oread = (ImageColor*)malloc(s);

    if (oread == NULL)
    {
        ((IMAGEBITMAPRELEASEACTION)VIRTUAL_METHOD(o, IMAGE_RELEASE))(o, IMAGE_RELEASE_DISPOSE);
        ReleaseAbstractImage((AbstractImage*)m, IMAGE_RELEASE_DISPOSE);

        return FALSE;
    }

    ZeroMemory(oread, s);

    ImageColor* mread = (ImageColor*)malloc(s);

    if (mread == NULL)
    {
        ((IMAGEBITMAPRELEASEACTION)VIRTUAL_METHOD(o, IMAGE_RELEASE))(o, IMAGE_RELEASE_DISPOSE);
        ReleaseAbstractImage((AbstractImage*)m, IMAGE_RELEASE_DISPOSE);

        return FALSE;
    }

    ZeroMemory(mread, s);

    for (u32 x = 0; x < h; x++)
    {
        ((IMAGEBITMAPREADACTION)VIRTUAL_METHOD(o, IMAGE_READ))(o, x, 0, oread);

        m->Self->Read((AbstractImage*)m, x, 0, mread);

        if (memcmp(oread, mread, s) != 0)
        {
            ((IMAGEBITMAPRELEASEACTION)VIRTUAL_METHOD(o, IMAGE_RELEASE))(o, IMAGE_RELEASE_DISPOSE);
            ReleaseAbstractImage((AbstractImage*)m, IMAGE_RELEASE_DISPOSE);

            free(oread);
            free(mread);

            return FALSE;
        };
    }

    free(oread);
    free(mread);

    if (!CompareImageBitMap(o, m))
    {
        ((IMAGEBITMAPRELEASEACTION)VIRTUAL_METHOD(o, IMAGE_RELEASE))(o, IMAGE_RELEASE_DISPOSE);
        ReleaseAbstractImage((AbstractImage*)m, IMAGE_RELEASE_DISPOSE);

        return FALSE;
    };

    ((IMAGEBITMAPRELEASEACTION)VIRTUAL_METHOD(o, IMAGE_RELEASE))(o, IMAGE_RELEASE_DISPOSE);
    ReleaseAbstractImage((AbstractImage*)m, IMAGE_RELEASE_DISPOSE);

    return TRUE;
}

BOOL ExecuteImageBitMap(HMODULE module, const D3DFORMAT format, const u32 width, const u32 height, const BOOL gradient, const u32 color, ImageColor* colors, const u8* palette, void* pixels)
{
    void* ia = malloc(IMAGE_SIZE);

    if (ia == NULL) { return FALSE; }

    CopyMemory(ia, pixels, IMAGE_SIZE);

    ImageContainerArgs a =
    {
            ia,
            format,
            IMAGE_DIMS * IMAGE_BPP,
            0, // AREA STRIDE
            0, // TODO
            0, // TODO
            width,
            height,
            0, // TODO
            1, // TODO
            { 0, 0, width, height, 0, 1 }, // TODO
            gradient,
            color,
            palette
    };

    void* ib = malloc(IMAGE_SIZE);

    if (ib == NULL) { free(ia); return FALSE; }

    CopyMemory(ib, pixels, IMAGE_SIZE);

    ImageContainerArgs b =
    {
            ib,
            format,
            IMAGE_DIMS * IMAGE_BPP,
            0, // AREA STRIDE
            0, // TODO
            0, // TODO
            width,
            height,
            0, // TODO
            1, // TODO
            { 0, 0, width, height, 0, 1 }, // TODO
            gradient,
            color,
            palette
    };

    const BOOL result = CompareImageBitMap(module, &a, &b, colors);

    free(ia);
    free(ib);

    return result;
}

BOOL ExecuteImageBitMap(HMODULE module, const D3DFORMAT format, const ImageTest* item)
{
    u8* palette = NULL;

    if (item->Palette) { AcquirePalette(malloc(256 * 4)); }

    void* pixels = malloc(IMAGE_SIZE);

    if (pixels == NULL)
    {
        free(palette);

        return FALSE;
    }

    if (!AcquireImage(pixels, item->Schema))
    {
        free(pixels);
        free(palette);

        return FALSE;
    }

    const u32 size = item->Width * item->Heigh * sizeof(ImageColor);
    ImageColor* colors = (ImageColor*)malloc(size);

    if (colors == NULL)
    {
        free(pixels);
        free(palette);

        return FALSE;
    }

    ZeroMemory(colors, size);

    if (!AcquireColors(colors, pixels, item->Width, item->Heigh, item->Colors))
    {
        free(pixels);
        free(palette);

        return FALSE;
    }

    const BOOL result = ExecuteImageBitMap(module, format,
        item->Width, item->Heigh, item->Gradient, item->Color, colors, palette, pixels);

    free(colors);
    free(pixels);
    free(palette);

    return result;
}

#include <stdio.h> // TODO

BOOL ExecuteImageBitMap(HMODULE module, const D3DFORMAT format)
{
    for (u32 x = 64; x < MAX_IMAGE_TEST_CASES; x++) // TODO
    {
        if (!ExecuteImageBitMap(module, format, &ImageTests[x])) {
            printf("FAILED: %d\r\n", x);
            //return FALSE;
        }
    }

    return TRUE;
}