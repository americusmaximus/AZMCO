/*
Copyright (c) 2024 - 2025 Americus Maximus

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

#pragma once

#include "Basic.hxx"
#include "DirectX.hxx"

#define IMAGE_RELEASE_NONE                          0
#define IMAGE_RELEASE_DISPOSE                       1

#define IMAGE_CONTAINER_OPTIONS_NONE                0x00000000

#define IMAGE_CONTAINER_OPTIONS_COLOR               0x00000001
#define IMAGE_CONTAINER_OPTIONS_BUMP_MAP            0x00000002
#define IMAGE_CONTAINER_OPTIONS_UNKNOWN_4           0x00000004 /* TODO */

#define IMAGE_CONTAINER_OPTIONS_LOW_VALUE_MASK      0x0000FFFF
#define IMAGE_CONTAINER_OPTIONS_HIGH_VALUE_MASK     0xFFF00000

#define IMAGE_CONTAINER_OPTIONS_GRADIENT            0x00080000

#define IMAGE_CONTAINER_OPTIONS_INVALID             0xFFFFFFFF

#define MAX_IMAGE_PALETTE_VALUES_COUNT              256

#define MAX_IMAGE_COLOR_MODIFIER_VALUES_COUNT       32

#define IMAGE_QUAD_COLOR_COUNT                      4
#define IMAGE_QUAD_PIXEL_COUNT                      (IMAGE_QUAD_COLOR_COUNT * IMAGE_QUAD_COLOR_COUNT)

#define RGB_COLOR_COUNT                             3

namespace Images
{
    enum ImageFormatDescriptorType
    {
        IMAGE_FORMAT_DESCRIPTOR_TYPE_RGB            = 0,
        IMAGE_FORMAT_DESCRIPTOR_TYPE_PALETTE        = 1,
        IMAGE_FORMAT_DESCRIPTOR_TYPE_LUMINANCE      = 2,
        IMAGE_FORMAT_DESCRIPTOR_TYPE_UV             = 3,
        IMAGE_FORMAT_DESCRIPTOR_TYPE_FORCE_DWORD    = 0x7FFFFFFF
    };

    struct ImageFormatDescriptor
    {
        D3DFORMAT Format;

        ImageFormatDescriptorType Type;

        u32 Bits;

        u32 Luminance;
        union
        {
            u32 Alpha;
            u32 Q;
        };
        union
        {
            u32 Red;
            u32 W;
        };
        union
        {
            u32 Green;
            u32 V;
        };
        union
        {
            u32 Blue;
            u32 U;
        };

        u32 Unk08; // TODO
        u32 Unk09; // TODO
    };

    struct ImageColor
    {
        f32 R;
        f32 G;
        f32 B;
        f32 A;
    };

    struct ImageDimensions
    {
        u32 Left;
        u32 Top;
        u32 Right;
        u32 Bottom;
        u32 Min;
        u32 Max;
    };

    struct ImageContainerArgs
    {
        void*           Pixels;
        D3DFORMAT       Format;
        u32             Stride;
        u32             AreaStride;
        ImageDimensions Source;
        ImageDimensions Target;
        BOOL            IsGradient;
        u32             Color;
        const u8*       Palette;
    };

    struct ImagePixel
    {
        u8 B;
        u8 G;
        u8 R;
        u8 A;
    };

    struct ImageQuad
    {
        union
        {
            u32         ARGB[IMAGE_QUAD_PIXEL_COUNT];
            ImagePixel  Pixels[IMAGE_QUAD_PIXEL_COUNT];
        };
    };

    struct AbstractImage;

    typedef AbstractImage* (*RELEASEIMAGECALL)(AbstractImage* self, const u32 mode);
    typedef void(*READIMAGECALL)(AbstractImage* self, const u32 line, const u32 level, ImageColor* pixels);
    typedef void(*WRITEDIMAGECALL)(AbstractImage* self, const u32 line, const u32 level, ImageColor* pixels);

    struct ImageCalls
    {
        RELEASEIMAGECALL Release;
        READIMAGECALL Read;
        WRITEDIMAGECALL Write;
    };

    enum ImageOptions
    {
        IMAGE_OPTIONS_NONE          = 0,
        IMAGE_OPTIONS_UNKNOWN_1     = 1,
        IMAGE_OPTIONS_UNKNOWN_2     = 2,
        IMAGE_OPTIONS_UNKNOWN_3     = 3,
        IMAGE_OPTIONS_FORCE_DWORD   = 0x7FFFFFFF
    };

    struct AbstractImage
    {
        ImageCalls* Self;
        D3DFORMAT Format;
        ImageOptions Options;
        BOOL IsBitMap;
        BOOL IsColor;
        BOOL IsPalette;
        void* Pixels;
        ImageColor Color; // Color Key
        f32* Modifiers;
        ImageColor Palette[MAX_IMAGE_PALETTE_VALUES_COUNT];
        ImageDimensions Dimensions;
        u32 Stride;         // Stride in bits.
        u32 AreaStride;     // Stride for the level.
        s32 Width;
        s32 Height;
        s32 Levels;
    };

    struct ImageContainer
    {
        AbstractImage* Source;
        AbstractImage* Destination;
        u32 Options;
    };

    // INHERITANCE: AbstractImage
    struct ImageBitMap
    {
        ImageCalls* Self;
        D3DFORMAT Format;
        ImageOptions Options;
        BOOL IsBitMap;
        BOOL IsColor;
        BOOL IsPalette;
        void* Pixels;
        ImageColor Color; // Color Key
        f32* Modifiers;
        ImageColor Palette[MAX_IMAGE_PALETTE_VALUES_COUNT];
        ImageDimensions Dimensions;
        u32 Stride;         // Stride in bits.
        u32 AreaStride;     // Stride for the level.
        s32 Width;
        s32 Height;
        s32 Levels;
        u32 BytesStride;    // Stride in bytes.
        u32 Bytes;          // Size of the pixel in bytes.
    };

    // INHERITANCE: ImageBitMap
    struct ImageDXT
    {
        ImageCalls* Self;
        D3DFORMAT Format;
        ImageOptions Options;
        BOOL IsBitMap;
        BOOL IsColor;
        BOOL IsPalette;
        void* Pixels;
        ImageColor Color; // Color Key
        f32* Modifiers;
        ImageColor Palette[MAX_IMAGE_PALETTE_VALUES_COUNT];
        ImageDimensions Dimensions;
        u32 Stride;         // Stride in bits.
        u32 AreaStride;     // Stride for the level.
        s32 Width;
        s32 Height;
        s32 Levels;
        u32 BytesStride;    // Stride in bytes.
        u32 Bytes;          // Size of the pixel in bytes.

        // DXT

        ImageColor* Colors[IMAGE_QUAD_COLOR_COUNT];
        u32 ActualLeft;
        u32 MinLine;
        u32 ActualRight;
        u32 MaxLine;
        u32 MinLevel;
        u32 MaxLevel;
        u32 ActualWidth;
        BOOL IsInit;
        BOOL IsAlloc;
        s32 Unk0x1098; // TODO
        s32 Unk0x109c; // TODO
        s32 Unk0x10a0; // TODO
        s32 Unk0x10a4; // TODO
    };

    // YCbCr
    // INHERITANCE: ImageBitMap
    struct ImageUYVY
    {
        ImageCalls* Self;
        D3DFORMAT Format;
        ImageOptions Options;
        BOOL IsBitMap;
        BOOL IsColor;
        BOOL IsPalette;
        void* Pixels;
        ImageColor Color; // Color Key
        f32* Modifiers;
        ImageColor Palette[MAX_IMAGE_PALETTE_VALUES_COUNT];
        ImageDimensions Dimensions;
        u32 Stride;         // Stride in bits.
        u32 AreaStride;     // Stride for the level.
        s32 Width;
        s32 Height;
        s32 Levels;
        u32 BytesStride;    // Stride in bytes.
        u32 Bytes;          // Size of the pixel in bytes.

        // UYVY

        ImageColor* Colors;
        u32 ActualLeft;
        u32 MinLine;
        u32 ActualRight;
        u32 MaxLine;
        u32 MinLevel;
        u32 MaxLevel;
        u32 ActualWidth;
        BOOL IsInit;
        BOOL IsAlloc;
        u32 LumaShift;
        u32 ValueShift;
    };

    void AcquireImageColor(ImageColor* self, const u32 color);

    void InitializeImageContainer(ImageContainer* self);
    void ReleaseImageContainer(ImageContainer* self);

    AbstractImage* InitializeAbstractImage(ImageContainerArgs* args);
    AbstractImage* ReleaseAbstractImage(AbstractImage* self, const u32 mode);

    void ImproveImageColors(AbstractImage* self, ImageColor* pixels);

    HRESULT InitializeImageContainer(ImageContainer* self, ImageContainerArgs* dst, ImageContainerArgs* src, const u32 options);

    ImageBitMap* InitializeImageBitMap(ImageBitMap* self, ImageContainerArgs* args, const u32 bits, const ImageOptions type);
    ImageBitMap* ReleaseImageBitMap(ImageBitMap* self, const u32 mode);
    void AssignImageBitMapSelf(ImageBitMap* self);

    void ReadImageBitMap(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);
    void WriteImageBitMap(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);

    void ReadImageR8G8B8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);
    void WriteImageR8G8B8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);

    void ReadImageA8R8G8B8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);
    void WriteImageA8R8G8B8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);

    void ReadImageX8R8G8B8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);
    void WriteImageX8R8G8B8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);

    void ReadImageR5G6B5(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);
    void WriteImageR5G6B5(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);

    void ReadImageX1R5G5B5(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);
    void WriteImageX1R5G5B5(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);

    void ReadImageA1R5G5B5(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);
    void WriteImageA1R5G5B5(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);

    void ReadImageA4R4G4B4(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);
    void WriteImageA4R4G4B4(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);

    void ReadImageR3G3B2(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);
    void WriteImageR3G3B2(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);

    void ReadImageA8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);
    void WriteImageA8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);

    void ReadImageA8R3G3B2(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);
    void WriteImageA8R3G3B2(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);

    void ReadImageX4R4G4B4(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);
    void WriteImageX4R4G4B4(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);

    void AcquirePaletteColor(ImageColor* self, ImageColor* color, ImageColor* palette);

    void ReadImageA8P8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);
    void WriteImageA8P8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);

    void ReadImageP8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);
    void WriteImageP8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);

    void ReadImageL8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);
    void WriteImageL8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);

    void ReadImageA8L8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);
    void WriteImageA8L8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);

    void ReadImageA4L4(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);
    void WriteImageA4L4(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);

    void ReadImageV8U8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);
    void WriteImageV8U8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);

    void ReadImageL6V5U5(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);
    void WriteImageL6V5U5(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);

    void ReadImageX8L8V8U8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);
    void WriteImageX8L8V8U8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);

    void ReadImageQ8W8V8U8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);
    void WriteImageQ8W8V8U8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);

    void ReadImageV16U16(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);
    void WriteImageV16U16(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);

    void ReadImageW11V11U10(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);
    void WriteImageW11V11U10(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels);

    HRESULT AcquireImageDXT(ImageDXT* self, const u32 line, const u32 level, const BOOL fill);
    HRESULT InitializeImageDXT(ImageDXT* self);
    ImageDXT* InitializeImageDXT(ImageDXT* self, ImageContainerArgs* args);
    ImageDXT* ReleaseImageDXT(ImageDXT* self, const u32 mode);
    ImageDXT* ReleaseAbstractImageDXT(ImageDXT* self, const u32 mode);
    void AssignImageDXTSelf(ImageDXT* self);

    void UnpackImageDXT1(const u32 indx, ImageQuad* quad, u16* pixels);
    
    void FUN_600106e2(const u32 indx, ImageQuad* quad, u16* pixels); // TODO
    void FUN_60010014(ImageQuad* quad, u16* pixels, u32 color, u32 alpha); // TODO

    void ReadImageDXT(ImageDXT* self, const u32 line, const u32 level, ImageColor* pixels);
    void WriteImageDXT(ImageDXT* self, const u32 line, const u32 level, ImageColor* pixels);

    HRESULT AcquireImageUYVY(ImageUYVY* self, const u32 line, const u32 level, const BOOL fill);
    HRESULT InitializeImageUYVY(ImageUYVY* self);
    ImageUYVY* InitializeImageUYVY(ImageUYVY* self, ImageContainerArgs* args);
    ImageUYVY* ReleaseAbstractImageUYVY(ImageUYVY* self, const u32 mode);
    ImageUYVY* ReleaseImageUYVY(ImageUYVY* self, const u32 mode);
    void AssignImageUYVYSelf(ImageUYVY* self);

    void ReadImageUYVY(ImageUYVY* self, const u32 line, const u32 level, ImageColor* pixels);
    void WriteImageUYVY(ImageUYVY* self, const u32 line, const u32 level, ImageColor* pixels);

    void AcquireImageColorQuad(const u16* pixels, ImageQuad* quad);
    void AcquireImagePixelQuad(const ImageQuad* quad, u16* pixels, u16 color);

    void AcquireImageColor(const u16 in, ImagePixel* out);
    void AcquireImageQuadDXT1(const u16* pixels, ImageQuad* quad);
    void AcquireImagePixel(ImagePixel in, u16* out);

    void ImageDXTColorMultiply(const f32* in, f32* out);
    void ImageDXTColorsFromGrayScale(const f32* in, u8* out);
    void ImageDXTColorsToGrayScale(const u8* in, f32* out);

    void FUN_6000fdf6(u16* pixels, f32* a, f32* b, u32 count); // TODO
    void MixGrayScaleColors(f32* a, f32* b);
}