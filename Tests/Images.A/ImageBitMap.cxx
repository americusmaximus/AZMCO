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

#define INIT_IMAGE_BASE_ADDRESS         (0x6000F55A - 0x60000000)
#define INIT_IMAGE_FUNC_ADDRESS(M)      ((addr)M + (addr)INIT_IMAGE_BASE_ADDRESS)

using namespace Images;

const static ImageTest Items[] =
{
    { 128, 128, FALSE, GRAPCHICS_COLOR_BLACK,   FALSE, FALSE, COLORSCHEMA_BLACK },
    { 128, 128, TRUE, GRAPCHICS_COLOR_BLACK,    FALSE, FALSE, COLORSCHEMA_BLACK },

    { 128, 128, FALSE, GRAPCHICS_COLOR_BLACK,   FALSE, TRUE, COLORSCHEMA_BLACK },
    { 128, 128, TRUE, GRAPCHICS_COLOR_BLACK,    FALSE, TRUE, COLORSCHEMA_BLACK },

    { 128, 128, FALSE, GRAPCHICS_COLOR_WHITE,   FALSE, FALSE, COLORSCHEMA_BLACK },
    { 128, 128, TRUE, GRAPCHICS_COLOR_WHITE,    FALSE, FALSE, COLORSCHEMA_BLACK },

    { 128, 128, FALSE, GRAPCHICS_COLOR_WHITE,   FALSE, TRUE, COLORSCHEMA_BLACK },
    { 128, 128, TRUE, GRAPCHICS_COLOR_WHITE,    FALSE, TRUE, COLORSCHEMA_BLACK },

    { 128, 128, FALSE, 0x00FABECD,              FALSE, FALSE, COLORSCHEMA_BLACK },
    { 128, 128, TRUE, 0x00FABECD,               FALSE, FALSE, COLORSCHEMA_BLACK },

    { 128, 128, FALSE, 0x00FABECD,              FALSE, TRUE, COLORSCHEMA_BLACK },
    { 128, 128, TRUE, 0x00FABECD,               FALSE, TRUE, COLORSCHEMA_BLACK },

    { 128, 128, FALSE, 0xFFFABECD,              FALSE, FALSE, COLORSCHEMA_BLACK },
    { 128, 128, TRUE, 0xFFFABECD,               FALSE, FALSE, COLORSCHEMA_BLACK },

    { 128, 128, FALSE, 0xFFFABECD,              FALSE, TRUE, COLORSCHEMA_BLACK },
    { 128, 128, TRUE, 0xFFFABECD,               FALSE, TRUE, COLORSCHEMA_BLACK },

    { 128, 128, FALSE, GRAPCHICS_COLOR_BLACK,   TRUE, FALSE, COLORSCHEMA_BLACK },
    { 128, 128, TRUE, GRAPCHICS_COLOR_BLACK,    TRUE, FALSE, COLORSCHEMA_BLACK },

    { 128, 128, FALSE, GRAPCHICS_COLOR_BLACK,   TRUE, TRUE, COLORSCHEMA_BLACK },
    { 128, 128, TRUE, GRAPCHICS_COLOR_BLACK,    TRUE, TRUE, COLORSCHEMA_BLACK },

    { 128, 128, FALSE, GRAPCHICS_COLOR_WHITE,   TRUE, FALSE, COLORSCHEMA_BLACK },
    { 128, 128, TRUE, GRAPCHICS_COLOR_WHITE,    TRUE, FALSE, COLORSCHEMA_BLACK },

    { 128, 128, FALSE, GRAPCHICS_COLOR_WHITE,   TRUE, TRUE, COLORSCHEMA_BLACK },
    { 128, 128, TRUE, GRAPCHICS_COLOR_WHITE,    TRUE, TRUE, COLORSCHEMA_BLACK },

    { 128, 128, FALSE, 0x00FABECD,              TRUE, FALSE, COLORSCHEMA_BLACK },
    { 128, 128, TRUE, 0x00FABECD,               TRUE, FALSE, COLORSCHEMA_BLACK },

    { 128, 128, FALSE, 0x00FABECD,              TRUE, TRUE, COLORSCHEMA_BLACK },
    { 128, 128, TRUE, 0x00FABECD,               TRUE, TRUE, COLORSCHEMA_BLACK },

    { 128, 128, FALSE, 0xFFFABECD,              TRUE, FALSE, COLORSCHEMA_BLACK },
    { 128, 128, TRUE, 0xFFFABECD,               TRUE, FALSE, COLORSCHEMA_BLACK },

    { 128, 128, FALSE, 0xFFFABECD,              TRUE, TRUE, COLORSCHEMA_BLACK },
    { 128, 128, TRUE, 0xFFFABECD,               TRUE, TRUE, COLORSCHEMA_BLACK },


    { 128, 128, FALSE, GRAPCHICS_COLOR_BLACK,   FALSE, FALSE, COLORSCHEMA_WHITE },
    { 128, 128, TRUE, GRAPCHICS_COLOR_BLACK,    FALSE, FALSE, COLORSCHEMA_WHITE },

    { 128, 128, FALSE, GRAPCHICS_COLOR_BLACK,   FALSE, TRUE, COLORSCHEMA_WHITE },
    { 128, 128, TRUE, GRAPCHICS_COLOR_BLACK,    FALSE, TRUE, COLORSCHEMA_WHITE },

    { 128, 128, FALSE, GRAPCHICS_COLOR_WHITE,   FALSE, FALSE, COLORSCHEMA_WHITE },
    { 128, 128, TRUE, GRAPCHICS_COLOR_WHITE,    FALSE, FALSE, COLORSCHEMA_WHITE },

    { 128, 128, FALSE, GRAPCHICS_COLOR_WHITE,   FALSE, TRUE, COLORSCHEMA_WHITE },
    { 128, 128, TRUE, GRAPCHICS_COLOR_WHITE,    FALSE, TRUE, COLORSCHEMA_WHITE },

    { 128, 128, FALSE, 0x00FABECD,              FALSE, FALSE, COLORSCHEMA_WHITE },
    { 128, 128, TRUE, 0x00FABECD,               FALSE, FALSE, COLORSCHEMA_WHITE },

    { 128, 128, FALSE, 0x00FABECD,              FALSE, TRUE, COLORSCHEMA_WHITE },
    { 128, 128, TRUE, 0x00FABECD,               FALSE, TRUE, COLORSCHEMA_WHITE },

    { 128, 128, FALSE, 0xFFFABECD,              FALSE, FALSE, COLORSCHEMA_WHITE },
    { 128, 128, TRUE, 0xFFFABECD,               FALSE, FALSE, COLORSCHEMA_WHITE },

    { 128, 128, FALSE, 0xFFFABECD,              FALSE, TRUE, COLORSCHEMA_WHITE },
    { 128, 128, TRUE, 0xFFFABECD,               FALSE, TRUE, COLORSCHEMA_WHITE },

    { 128, 128, FALSE, GRAPCHICS_COLOR_BLACK,   TRUE, FALSE, COLORSCHEMA_WHITE },
    { 128, 128, TRUE, GRAPCHICS_COLOR_BLACK,    TRUE, FALSE, COLORSCHEMA_WHITE },
    { 128, 128, FALSE, GRAPCHICS_COLOR_BLACK,   TRUE, TRUE, COLORSCHEMA_WHITE },
    { 128, 128, TRUE, GRAPCHICS_COLOR_BLACK,    TRUE, TRUE, COLORSCHEMA_WHITE },

    { 128, 128, FALSE, GRAPCHICS_COLOR_WHITE,   TRUE, FALSE, COLORSCHEMA_WHITE },
    { 128, 128, TRUE, GRAPCHICS_COLOR_WHITE,    TRUE, FALSE, COLORSCHEMA_WHITE },
    { 128, 128, FALSE, GRAPCHICS_COLOR_WHITE,   TRUE, TRUE, COLORSCHEMA_WHITE },
    { 128, 128, TRUE, GRAPCHICS_COLOR_WHITE,    TRUE, TRUE, COLORSCHEMA_WHITE },

    { 128, 128, FALSE, 0x00FABECD,              TRUE, FALSE, COLORSCHEMA_WHITE },
    { 128, 128, TRUE, 0x00FABECD,               TRUE, FALSE, COLORSCHEMA_WHITE },
    { 128, 128, FALSE, 0x00FABECD,              TRUE, TRUE, COLORSCHEMA_WHITE },
    { 128, 128, TRUE, 0x00FABECD,               TRUE, TRUE, COLORSCHEMA_WHITE },

    { 128, 128, FALSE, 0xFFFABECD,              TRUE, FALSE, COLORSCHEMA_WHITE },
    { 128, 128, TRUE, 0xFFFABECD,               TRUE, FALSE, COLORSCHEMA_WHITE },
    { 128, 128, FALSE, 0xFFFABECD,              TRUE, TRUE, COLORSCHEMA_WHITE },
    { 128, 128, TRUE, 0xFFFABECD,               TRUE, TRUE, COLORSCHEMA_WHITE },


    { 128, 128, FALSE, GRAPCHICS_COLOR_BLACK,   FALSE, FALSE, COLORSCHEMA_REPEAT },
    { 128, 128, TRUE, GRAPCHICS_COLOR_BLACK,    FALSE, FALSE, COLORSCHEMA_REPEAT },

    { 128, 128, FALSE, GRAPCHICS_COLOR_BLACK,   FALSE, TRUE, COLORSCHEMA_REPEAT },
    { 128, 128, TRUE, GRAPCHICS_COLOR_BLACK,    FALSE, TRUE, COLORSCHEMA_REPEAT },

    { 128, 128, FALSE, GRAPCHICS_COLOR_WHITE,   FALSE, FALSE, COLORSCHEMA_REPEAT },
    { 128, 128, TRUE, GRAPCHICS_COLOR_WHITE,    FALSE, FALSE, COLORSCHEMA_REPEAT },

    { 128, 128, FALSE, GRAPCHICS_COLOR_WHITE,   FALSE, TRUE, COLORSCHEMA_REPEAT },
    { 128, 128, TRUE, GRAPCHICS_COLOR_WHITE,    FALSE, TRUE, COLORSCHEMA_REPEAT },

    { 128, 128, FALSE, 0x00FABECD,              FALSE, FALSE, COLORSCHEMA_REPEAT },
    { 128, 128, TRUE, 0x00FABECD,               FALSE, FALSE, COLORSCHEMA_REPEAT },

    { 128, 128, FALSE, 0x00FABECD,              FALSE, TRUE, COLORSCHEMA_REPEAT },
    { 128, 128, TRUE, 0x00FABECD,               FALSE, TRUE, COLORSCHEMA_REPEAT },

    { 128, 128, FALSE, 0xFFFABECD,              FALSE, FALSE, COLORSCHEMA_REPEAT },
    { 128, 128, TRUE, 0xFFFABECD,               FALSE, FALSE, COLORSCHEMA_REPEAT },

    { 128, 128, FALSE, 0xFFFABECD,              FALSE, TRUE, COLORSCHEMA_REPEAT },
    { 128, 128, TRUE, 0xFFFABECD,               FALSE, TRUE, COLORSCHEMA_REPEAT },

    { 128, 128, FALSE, GRAPCHICS_COLOR_BLACK,   TRUE, FALSE, COLORSCHEMA_REPEAT },
    { 128, 128, TRUE, GRAPCHICS_COLOR_BLACK,    TRUE, FALSE, COLORSCHEMA_REPEAT },

    { 128, 128, FALSE, GRAPCHICS_COLOR_BLACK,   TRUE, TRUE, COLORSCHEMA_REPEAT },
    { 128, 128, TRUE, GRAPCHICS_COLOR_BLACK,    TRUE, TRUE, COLORSCHEMA_REPEAT },

    { 128, 128, FALSE, GRAPCHICS_COLOR_WHITE,   TRUE, FALSE, COLORSCHEMA_REPEAT },
    { 128, 128, TRUE, GRAPCHICS_COLOR_WHITE,    TRUE, FALSE, COLORSCHEMA_REPEAT },

    { 128, 128, FALSE, GRAPCHICS_COLOR_WHITE,   TRUE, TRUE, COLORSCHEMA_REPEAT },
    { 128, 128, TRUE, GRAPCHICS_COLOR_WHITE,    TRUE, TRUE, COLORSCHEMA_REPEAT },

    { 128, 128, FALSE, 0x00FABECD,              TRUE, FALSE, COLORSCHEMA_REPEAT },
    { 128, 128, TRUE, 0x00FABECD,               TRUE, FALSE, COLORSCHEMA_REPEAT },

    { 128, 128, FALSE, 0x00FABECD,              TRUE, TRUE, COLORSCHEMA_REPEAT },
    { 128, 128, TRUE, 0x00FABECD,               TRUE, TRUE, COLORSCHEMA_REPEAT },

    { 128, 128, FALSE, 0xFFFABECD,              TRUE, FALSE, COLORSCHEMA_REPEAT },
    { 128, 128, TRUE, 0xFFFABECD,               TRUE, FALSE, COLORSCHEMA_REPEAT },

    { 128, 128, FALSE, 0xFFFABECD,              TRUE, TRUE, COLORSCHEMA_REPEAT },
    { 128, 128, TRUE, 0xFFFABECD,               TRUE, TRUE, COLORSCHEMA_REPEAT },


    { 512, 512, FALSE, GRAPCHICS_COLOR_BLACK,   FALSE, FALSE, COLORSCHEMA_IMAGE },
    { 512, 512, TRUE, GRAPCHICS_COLOR_BLACK,    FALSE, FALSE, COLORSCHEMA_IMAGE },

    { 512, 512, FALSE, GRAPCHICS_COLOR_BLACK,   FALSE, TRUE, COLORSCHEMA_IMAGE },
    { 512, 512, TRUE, GRAPCHICS_COLOR_BLACK,    FALSE, TRUE, COLORSCHEMA_IMAGE },

    { 512, 512, FALSE, GRAPCHICS_COLOR_WHITE,   FALSE, FALSE, COLORSCHEMA_IMAGE },
    { 512, 512, TRUE, GRAPCHICS_COLOR_WHITE,    FALSE, FALSE, COLORSCHEMA_IMAGE },

    { 512, 512, FALSE, GRAPCHICS_COLOR_WHITE,   FALSE, TRUE, COLORSCHEMA_IMAGE },
    { 512, 512, TRUE, GRAPCHICS_COLOR_WHITE,    FALSE, TRUE, COLORSCHEMA_IMAGE },

    { 512, 512, FALSE, 0x00FABECD,              FALSE, FALSE, COLORSCHEMA_IMAGE },
    { 512, 512, TRUE, 0x00FABECD,               FALSE, FALSE, COLORSCHEMA_IMAGE },

    { 512, 512, FALSE, 0x00FABECD,              FALSE, TRUE, COLORSCHEMA_IMAGE },
    { 512, 512, TRUE, 0x00FABECD,               FALSE, TRUE, COLORSCHEMA_IMAGE },

    { 512, 512, FALSE, 0xFFFABECD,              FALSE, FALSE, COLORSCHEMA_IMAGE },
    { 512, 512, TRUE, 0xFFFABECD,               FALSE, FALSE, COLORSCHEMA_IMAGE },

    { 512, 512, FALSE, 0xFFFABECD,              FALSE, TRUE, COLORSCHEMA_IMAGE },
    { 512, 512, TRUE, 0xFFFABECD,               FALSE, TRUE, COLORSCHEMA_IMAGE },

    { 512, 512, FALSE, GRAPCHICS_COLOR_BLACK,   TRUE, FALSE, COLORSCHEMA_IMAGE },
    { 512, 512, TRUE, GRAPCHICS_COLOR_BLACK,    TRUE, FALSE, COLORSCHEMA_IMAGE },

    { 512, 512, FALSE, GRAPCHICS_COLOR_BLACK,   TRUE, TRUE, COLORSCHEMA_IMAGE },
    { 512, 512, TRUE, GRAPCHICS_COLOR_BLACK,    TRUE, TRUE, COLORSCHEMA_IMAGE },

    { 512, 512, FALSE, GRAPCHICS_COLOR_WHITE,   TRUE, FALSE, COLORSCHEMA_IMAGE },
    { 512, 512, TRUE, GRAPCHICS_COLOR_WHITE,    TRUE, FALSE, COLORSCHEMA_IMAGE },

    { 512, 512, FALSE, GRAPCHICS_COLOR_WHITE,   TRUE, TRUE, COLORSCHEMA_IMAGE },
    { 512, 512, TRUE, GRAPCHICS_COLOR_WHITE,    TRUE, TRUE, COLORSCHEMA_IMAGE },

    { 512, 512, FALSE, 0x00FABECD,              TRUE, FALSE, COLORSCHEMA_IMAGE },
    { 512, 512, TRUE, 0x00FABECD,               TRUE, FALSE, COLORSCHEMA_IMAGE },

    { 512, 512, FALSE, 0x00FABECD,              TRUE, TRUE, COLORSCHEMA_IMAGE },
    { 512, 512, TRUE, 0x00FABECD,               TRUE, TRUE, COLORSCHEMA_IMAGE },

    { 512, 512, FALSE, 0xFFFABECD,              TRUE, FALSE, COLORSCHEMA_IMAGE },
    { 512, 512, TRUE, 0xFFFABECD,               TRUE, FALSE, COLORSCHEMA_IMAGE },

    { 512, 512, FALSE, 0xFFFABECD,              TRUE, TRUE, COLORSCHEMA_IMAGE },
    { 512, 512, TRUE, 0xFFFABECD,               TRUE, TRUE, COLORSCHEMA_IMAGE },
};

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
    if (memcmp(&a->Palette, &b->Palette, sizeof(ImageColor) * MAX_IMAGE_PALETTE_VALUES_COUNT) != 0) return FALSE;
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
        ((IMAGEBITMAPWRITEACTION)VIRTUAL_METHOD(o, IMAGE_WRITE))(o, x, 0, colors);

        m->Self->Write((AbstractImage*)m, x, 0, colors);
    }

    if (!CompareImageBitMap(o, m))
    {
        ((IMAGEBITMAPRELEASEACTION)VIRTUAL_METHOD(o, IMAGE_RELEASE))(o, IMAGE_RELEASE_DISPOSE);
        ReleaseAbstractImage((AbstractImage*)m, IMAGE_RELEASE_DISPOSE);

        return FALSE;
    };

    // Read

    for (u32 x = 0; x < h; x++)
    {
        ImageColor* row = (ImageColor*)((addr)colors + (addr)(x * w * sizeof(ImageColor)));

        ((IMAGEBITMAPREADACTION)VIRTUAL_METHOD(o, IMAGE_READ))(o, x, 0, row);

        m->Self->Read((AbstractImage*)m, x, 0, row);
    }

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

BOOL ExecuteImageBitMap(HMODULE module, const D3DFORMAT format)
{
    for (u32 x = 0; x < _countof(Items); x++)
    {
        if (!ExecuteImageBitMap(module, format, &Items[x])) { return FALSE; }
    }

    return TRUE;
}