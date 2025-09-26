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

#include "ImageDXT1.hxx"

#include <stdlib.h>

using namespace Images;

#define IMAGE_UNPACK_DXT_BASE_ADDRESS       (0x60010714 - 0x60000000)
#define IMAGE_UNPACK_DXT_FUNC_ADDRESS(M)    ((addr)M + (addr)IMAGE_UNPACK_DXT_BASE_ADDRESS)

#define IMAGE_INITIALIZE_BASE_ADDRESS       (0x6000EDD2 - 0x60000000)
#define IMAGE_INITIALIZE_FUNC_ADDRESS(M)    ((addr)M + (addr)IMAGE_INITIALIZE_BASE_ADDRESS)

#define ACQUIRE_IMAGE_QUAD_BASE_ADDRESS       (0x60010594 - 0x60000000)
#define ACQUIRE_IMAGE_QUAD_FUNC_ADDRESS(M)    ((addr)M + (addr)ACQUIRE_IMAGE_QUAD_BASE_ADDRESS)

typedef AbstractImage* (__cdecl* INITIALIZEABSTRACTIMAGEACTION)(ImageContainerArgs*);

typedef ImageDXT* (__thiscall* IMAGEDXTCONSTRUCTORACTION)(ImageDXT*, ImageContainerArgs* args, const u32 bits, const u32 options);

typedef VOID(__thiscall* IMAGEDXTRELEASEACTION)(ImageDXT*, const u8 mode);
typedef VOID(__thiscall* IMAGEDXTREADACTION)(ImageDXT*, const u32 line, const u32 level, ImageColor* pixels);
typedef VOID(__thiscall* IMAGEDXTWRITEACTION)(ImageDXT*, const u32 line, const u32 level, ImageColor* pixels);

typedef HRESULT(__thiscall* IMAGEINITIALIZEDXTACTION)(ImageDXT*);
typedef VOID(__cdecl* ACQUIREIMAGEDXTACTION)(const u16* pixels, ImageQuad* quad);

struct ImageUnpackTest
{
    u16         Color;
    ImageQuad   Quad;
    u16         Pixels[IMAGE_QUAD_PIXEL_COUNT];
};

#define IMAGE_UNPACK_TEST_COUNT 7

static const ImageUnpackTest ImageUnpackTests[IMAGE_UNPACK_TEST_COUNT] =
{
    { 0x00FF, {}, {} },
    {
        0xFFFF,
        { 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
        // 0xFFFF 0xFFFF 0xFFFF 0xFFFF ... zeroes ... 
    },
    {
        0xFFFF,
        { 0xFFAD5552, 0xFF7E503C, 0xFFAD5552, 0xFF7E503C, 0xFFAD5552, 0xFF7E503C, 0xFFAD5552, 0xFF7E503C, 0xFFAD5552, 0xFFAD5552, 0xFFAD5552, 0xFFAD5552, 0xFFAD5552, 0xFFAD5552, 0xFFAD5552, 0xFFAD5552 },
        { 0xAAAA, 0xAAAA, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
        // 0xFFFF 0xFFFF 0xFFFF 0xFFFF 0xAAAA 0x7A87 0x4444 0x0000 ... zeroes ...
    },
    {
        0xABCD,
        { 0xFFAD5552, 0xFF7E503C, 0xFFAD5552, 0xFF7E503C, 0xFFAD5552, 0xFF7E503C, 0xFFAD5552, 0xFF7E503C, 0xFFAD5552, 0xFFAD5552, 0xFFAD5552, 0xFFAD5552, 0xFFAD5552, 0xFFAD5552, 0xFFAD5552, 0xFFAD5552 },
        { 0xAAAA, 0xAAAA, 0x0F0C, 0x3330, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
        // 0xFFFF 0xFFFF 0xFFFF 0xFFFF 0xAAAA 0x7A87 0x4444 0x0000 ... zeroes ...
    },
    {
        0x77AA,
        { 0xFF555555, 0xFF555555, 0xFF555555, 0xFF555555, 0xFF555555, 0xFF555555, 0xFF555555, 0xFF555555, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF },
        { 0xFFFF, 0xFFFF, 0x3333, 0xC0C0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
        // 0xFFFF 0xFFFF 0xFFFF 0xFFFF 0xFFFF 0x52AA 0x5555 0x0000 ... zeroes ...
    },
    {
        0xFFFF,
        { 0xFF555555, 0xFF555555, 0xFF555555, 0xFF555555, 0xFF555555, 0xFF555555, 0xFF555555, 0xFF555555, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF },
        { 0x52AA, 0x52AA, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
        // 0xFFFF 0xFFFF 0xFFFF 0xFFFF 0xFFFF 0x52AA 0x5555 0x0000 ... zeroes ...
    },
    {
        0xABCD,
        { 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000 },
        { 0x0000, 0x0000, 0x0F0C, 0x3330, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
        // 0xFFFF 0xFFFF 0xFFFF 0xFFFF ... zeroes ... 
    }
};

void UnpackImageDXT23Wrapper(HMODULE m, u16 color, const ImageQuad* quad, u16* pixels)
{
    __asm
    {
        push ecx;

        push pixels;
        push quad;

        movzx ecx, color;

        mov eax, m;
        add eax, 0x10714; // IMAGE_UNPACK_DXT_BASE_ADDRESS

        call eax;

        pop eax;
        pop eax;

        pop ecx;
    }

    return;
}

BOOL ExecuteUnpackImageDXT23(HMODULE module, const ImageUnpackTest* test)
{
    ImageQuad q1;
    CopyMemory(&q1, &test->Quad, sizeof(ImageQuad));

    u16 p1[IMAGE_QUAD_PIXEL_COUNT];
    CopyMemory(p1, &test->Pixels, sizeof(u16) * IMAGE_QUAD_PIXEL_COUNT);

    UnpackImageDXT23Wrapper(module, test->Color, &q1, p1);

    ImageQuad q2;
    CopyMemory(&q2, &test->Quad, sizeof(ImageQuad));

    u16 p2[IMAGE_QUAD_PIXEL_COUNT];
    CopyMemory(p2, &test->Pixels, sizeof(u16) * IMAGE_QUAD_PIXEL_COUNT);

    UnpackImageDXT23(test->Color, &q2, p2);

    return memcmp(&q1, &q2, sizeof(ImageQuad)) == 0
        && memcmp(p1, p2, sizeof(u16) * IMAGE_QUAD_PIXEL_COUNT) == 0;
}

#define ACQUIRE_IMAGE_QUAD_TEST_COUNT    5

struct AcquireImageQuadTest
{
    u16 InPixels[IMAGE_QUAD_PIXEL_COUNT];
};

static const AcquireImageQuadTest AcquireImageQuadTests[ACQUIRE_IMAGE_QUAD_TEST_COUNT] =
{
    {
        { 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
    },
    {
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
    },
    {
        { 0x0011, 0x0555, 0x0AAA, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
    },
    {
        { 0xF7FD, 0x52AA, 0x5542, 0x5555, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
    },
    {
       { 0xF7BE, 0xEF7D, 0xADAD, 0xADAD, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
    }
};

BOOL ExecuteAcquireImageQuadDXT23(HMODULE module, const AcquireImageQuadTest* test)
{
    ACQUIREIMAGEDXTACTION action =
        (ACQUIREIMAGEDXTACTION)ACQUIRE_IMAGE_QUAD_FUNC_ADDRESS(module);

    ImageQuad q1;
    ZeroMemory(&q1,sizeof(ImageQuad));

    u16 p1[IMAGE_QUAD_PIXEL_COUNT];
    CopyMemory(p1, &test->InPixels, sizeof(u16) * IMAGE_QUAD_PIXEL_COUNT);

    action(p1, &q1);

    ImageQuad q2;
    ZeroMemory(&q2, sizeof(ImageQuad));

    u16 p2[IMAGE_QUAD_PIXEL_COUNT];
    CopyMemory(p2, &test->InPixels, sizeof(u16) * IMAGE_QUAD_PIXEL_COUNT);

    AcquireImageQuadDXT23(p2, &q2);

    return memcmp(&q1, &q2, sizeof(ImageQuad)) == 0
        && memcmp(p1, p2, sizeof(u16) * IMAGE_QUAD_PIXEL_COUNT) == 0;
}

BOOL ExecuteImageDXT23(HMODULE module, const D3DFORMAT format, const ImageTest* item);
BOOL ExecuteImageDXT23(HMODULE module, const D3DFORMAT format, const u32 width, const u32 height, const BOOL gradient, const u32 color, ImageColor* colors, const u8* palette, void* pixels);
BOOL CompareImageDXT23(HMODULE module, ImageContainerArgs* a, ImageContainerArgs* b, ImageColor* colors);

BOOL CompareImageDXT23(const ImageDXT* a, const ImageDXT* b, const BOOL colors)
{
    if (a->Format != b->Format) return FALSE;
    if (a->Options != b->Options) return FALSE;
    if (a->IsBitMap != b->IsBitMap) return FALSE;
    if (a->IsColor != b->IsColor) return FALSE;
    if (a->IsPalette != b->IsPalette) return FALSE;

    if (memcmp(a->Pixels, b->Pixels, IMAGE_SIZE) != 0) { return FALSE; }

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

    // DXT

    if (colors)
    {
        for (u32 x = 0; x < IMAGE_QUAD_COLOR_COUNT; x++)
        {
            if (memcmp(a->Colors[x], b->Colors[x], a->ActualWidth * sizeof(ImageColor)) != 0) return FALSE;
        }
    }

    if (a->ActualLeft != b->ActualLeft) return FALSE;
    if (a->MinLine != b->MinLine) return FALSE;
    if (a->ActualRight != b->ActualRight) return FALSE;
    if (a->MaxLine != b->MaxLine) return FALSE;
    if (a->MinLevel != b->MinLevel) return FALSE;
    if (a->MaxLevel != b->MaxLevel) return FALSE;
    if (a->ActualWidth != b->ActualWidth) return FALSE;
    if (a->IsAlloc != b->IsAlloc) return FALSE;
    if (a->Unk0x1098 != b->Unk0x1098) return FALSE;
    if (a->Unk0x109c != b->Unk0x109c) return FALSE;
    if (a->Unk0x10a0 != b->Unk0x10a0) return FALSE;
    if (a->Unk0x10a4 != b->Unk0x10a4) return FALSE;

    return TRUE;
}

BOOL CompareImageInitializeDXT23(HMODULE module, ImageContainerArgs* a, ImageContainerArgs* b, ImageColor* colors)
{
    // Create

    ImageDXT* o = (ImageDXT*)((INITIALIZEABSTRACTIMAGEACTION)INIT_IMAGE_FUNC_ADDRESS(module))(a);

    if (o == NULL) { return FALSE; }

    // Original code's new operator leaves first 8 bytes with garbage values.
    for (u32 x = 0; x < IMAGE_QUAD_COLOR_COUNT; x++)
    {
        ZeroMemory(o->Colors[x], o->ActualWidth * sizeof(ImageColor));
    }

    ImageDXT* m = (ImageDXT*)InitializeAbstractImage(b);

    if (m == NULL)
    {
        ((IMAGEDXTRELEASEACTION)VIRTUAL_METHOD(o, IMAGE_RELEASE))(o, IMAGE_RELEASE_DISPOSE);

        return FALSE;
    }

    if (!CompareImageDXT23(o, m, TRUE))
    {
        ((IMAGEDXTRELEASEACTION)VIRTUAL_METHOD(o, IMAGE_RELEASE))(o, IMAGE_RELEASE_DISPOSE);
        ReleaseAbstractImage((AbstractImage*)m, IMAGE_RELEASE_DISPOSE);

        return FALSE;
    }

    // Initialize
    ((IMAGEINITIALIZEDXTACTION)IMAGE_INITIALIZE_FUNC_ADDRESS(module))(o);

    InitializeImageDXT(m);

    if (!CompareImageDXT23(o, m, TRUE))
    {
        ((IMAGEDXTRELEASEACTION)VIRTUAL_METHOD(o, IMAGE_RELEASE))(o, IMAGE_RELEASE_DISPOSE);
        ReleaseAbstractImage((AbstractImage*)m, IMAGE_RELEASE_DISPOSE);

        return FALSE;
    }

    ((IMAGEDXTRELEASEACTION)VIRTUAL_METHOD(o, IMAGE_RELEASE))(o, IMAGE_RELEASE_DISPOSE);
    ReleaseAbstractImage((AbstractImage*)m, IMAGE_RELEASE_DISPOSE);

    return TRUE;
}

BOOL ExecuteImageInitializeDXT23(HMODULE module, const D3DFORMAT format, const u32 width, const u32 height, const BOOL gradient, const u32 color, ImageColor* colors, const u8* palette, void* pixels)
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
        { 0, 0, width, height, 0, 1 },
        { 0, 0, width, height, 0, 1 },
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
        { 0, 0, width, height, 0, 1 },
        { 0, 0, width, height, 0, 1 },
        gradient,
        color,
        palette
    };

    const BOOL result = CompareImageInitializeDXT23(module, &a, &b, colors);

    free(ia);
    free(ib);

    return result;
}

BOOL ExecuteImageInitializeDXT23(HMODULE module, const D3DFORMAT format, const ImageTest* item)
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

    const BOOL result = ExecuteImageInitializeDXT23(module, format,
        item->Width, item->Heigh, item->Gradient, item->Color, colors, palette, pixels);

    free(colors);
    free(pixels);
    free(palette);

    return result;
}

BOOL CompareImageDXT23(HMODULE module, ImageContainerArgs* a, ImageContainerArgs* b, ImageColor* colors)
{
    // Create

    ImageDXT* o = (ImageDXT*)((INITIALIZEABSTRACTIMAGEACTION)INIT_IMAGE_FUNC_ADDRESS(module))(a);

    if (o == NULL) { return FALSE; }

    // Original code's new operator leaves first 8 bytes with garbage values.
    for (u32 x = 0; x < IMAGE_QUAD_COLOR_COUNT; x++)
    {
        ZeroMemory(o->Colors[x], o->ActualWidth * sizeof(ImageColor));
    }

    ImageDXT* m = (ImageDXT*)InitializeAbstractImage(b);

    if (m == NULL)
    {
        ((IMAGEDXTRELEASEACTION)VIRTUAL_METHOD(o, IMAGE_RELEASE))(o, IMAGE_RELEASE_DISPOSE);

        return FALSE;
    }

    if (!CompareImageDXT23(o, m, TRUE))
    {
        ((IMAGEDXTRELEASEACTION)VIRTUAL_METHOD(o, IMAGE_RELEASE))(o, IMAGE_RELEASE_DISPOSE);
        ReleaseAbstractImage((AbstractImage*)m, IMAGE_RELEASE_DISPOSE);

        return FALSE;
    }

    // Init

    const u32 w = o->Width;
    const u32 h = o->Height;

    // Write

    for (u32 x = 0; x < h; x++)
    {
        ImageColor* row = (ImageColor*)((addr)colors + (addr)(x * w * sizeof(ImageColor)));

        ((IMAGEDXTWRITEACTION)VIRTUAL_METHOD(o, IMAGE_WRITE))(o, x, 0, row);

        m->Self->Write((AbstractImage*)m, x, 0, row);

        if (!CompareImageDXT23(o, m, TRUE))
        {
            ((IMAGEDXTRELEASEACTION)VIRTUAL_METHOD(o, IMAGE_RELEASE))(o, IMAGE_RELEASE_DISPOSE);
            ReleaseAbstractImage((AbstractImage*)m, IMAGE_RELEASE_DISPOSE);

            return FALSE;
        }
    }

    // Read

    const u32 s = w * sizeof(ImageColor);

    ImageColor* oread = (ImageColor*)malloc(s);

    if (oread == NULL)
    {
        ((IMAGEDXTRELEASEACTION)VIRTUAL_METHOD(o, IMAGE_RELEASE))(o, IMAGE_RELEASE_DISPOSE);
        ReleaseAbstractImage((AbstractImage*)m, IMAGE_RELEASE_DISPOSE);

        return FALSE;
    }

    ZeroMemory(oread, s);

    ImageColor* mread = (ImageColor*)malloc(s);

    if (mread == NULL)
    {
        ((IMAGEDXTRELEASEACTION)VIRTUAL_METHOD(o, IMAGE_RELEASE))(o, IMAGE_RELEASE_DISPOSE);
        ReleaseAbstractImage((AbstractImage*)m, IMAGE_RELEASE_DISPOSE);

        return FALSE;
    }

    ZeroMemory(mread, s);

    for (u32 x = 0; x < h; x++)
    {
        ((IMAGEDXTREADACTION)VIRTUAL_METHOD(o, IMAGE_READ))(o, x, 0, oread);

        m->Self->Read((AbstractImage*)m, x, 0, mread);

        if (!CompareImageDXT23(o, m, TRUE))
        {
            ((IMAGEDXTRELEASEACTION)VIRTUAL_METHOD(o, IMAGE_RELEASE))(o, IMAGE_RELEASE_DISPOSE);
            ReleaseAbstractImage((AbstractImage*)m, IMAGE_RELEASE_DISPOSE);

            return FALSE;
        }
    }

    free(oread);
    free(mread);

    if (!CompareImageDXT23(o, m, TRUE))
    {
        ((IMAGEDXTRELEASEACTION)VIRTUAL_METHOD(o, IMAGE_RELEASE))(o, IMAGE_RELEASE_DISPOSE);
        ReleaseAbstractImage((AbstractImage*)m, IMAGE_RELEASE_DISPOSE);

        return FALSE;
    }

    ((IMAGEDXTRELEASEACTION)VIRTUAL_METHOD(o, IMAGE_RELEASE))(o, IMAGE_RELEASE_DISPOSE);
    ReleaseAbstractImage((AbstractImage*)m, IMAGE_RELEASE_DISPOSE);

    return TRUE;
}

BOOL ExecuteImageDXT23(HMODULE module, const D3DFORMAT format, const u32 width, const u32 height, const BOOL gradient, const u32 color, ImageColor* colors, const u8* palette, void* pixels)
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
        { 0, 0, width, height, 0, 1 },
        { 0, 0, width, height, 0, 1 },
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
        { 0, 0, width, height, 0, 1 },
        { 0, 0, width, height, 0, 1 },
        gradient,
        color,
        palette
    };

    const BOOL result = CompareImageDXT23(module, &a, &b, colors);

    free(ia);
    free(ib);

    return result;
}

BOOL ExecuteImageDXT23(HMODULE module, const D3DFORMAT format, const ImageTest* item)
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

    const BOOL result = ExecuteImageDXT23(module, format,
        item->Width, item->Heigh, item->Gradient, item->Color, colors, palette, pixels);

    free(colors);
    free(pixels);
    free(palette);

    return result;
}

BOOL ExecuteImageDXT23(HMODULE module, const D3DFORMAT format)
{
    for (u32 x = 0; x < IMAGE_UNPACK_TEST_COUNT; x++)
    {
        if (!ExecuteUnpackImageDXT23(module, &ImageUnpackTests[x])) { return FALSE; }
    }

    for (u32 x = 0; x < ACQUIRE_IMAGE_QUAD_TEST_COUNT; x++)
    {
        if (!ExecuteAcquireImageQuadDXT23(module, &AcquireImageQuadTests[x])) { return FALSE; }
    }

    for (u32 x = 0; x < MAX_IMAGE_TEST_CASES; x++)
    {
        if (!ExecuteImageInitializeDXT23(module, format, &ImageTests[x])) { return FALSE; }
    }

    for (u32 x = 0; x < 16 /* MAX_IMAGE_TEST_CASES */; x++)
    {
        if (!ExecuteImageDXT23(module, format, &ImageTests[x])) { return FALSE; }
    }

    return TRUE;
}