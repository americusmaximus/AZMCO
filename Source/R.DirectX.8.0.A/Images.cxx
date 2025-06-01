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

#include "FPU.hxx"
#include "Graphics.Basic.hxx"
#include "Images.hxx"
#include "Mathematics.Basic.hxx"
#include "Renderer.hxx"
#include "RendererValues.hxx"

#include <math.h>

#define ALIGNMENTVALUE  4
#define ALIGNMENTMASK   0xFFFFFFFC
#define ALIGN(x)        (((x) + 3) & ALIGNMENTMASK)

using namespace Mathematics;
using namespace RendererModuleValues;

namespace Images
{
    // 0x60018540
    ImageCalls AbstractImageUYVYSelf = { (RELEASEIMAGECALL)ReleaseAbstractImageUYVY, (READIMAGECALL)ReadImageUYVY, (WRITEDIMAGECALL)WriteImageUYVY };

    // 0x60018580
    ImageCalls AbstractImageDXTSelf = { (RELEASEIMAGECALL)ReleaseAbstractImageDXT, (READIMAGECALL)ReadImageDXT , (WRITEDIMAGECALL)WriteImageDXT };

    // 0x600184d0
    ImageCalls ImageBitMapSelf = { (RELEASEIMAGECALL)ReleaseImageBitMap, (READIMAGECALL)ReadImageBitMap, (WRITEDIMAGECALL)WriteImageBitMap };

    // 0x6001858c
    ImageCalls ImageR8G8B8Self = { (RELEASEIMAGECALL)ReleaseAbstractImage, (READIMAGECALL)ReadImageR8G8B8, (WRITEDIMAGECALL)WriteImageR8G8B8 };

    // 0x60018598
    ImageCalls ImageA8R8G8B8Self = { (RELEASEIMAGECALL)ReleaseAbstractImage, (READIMAGECALL)ReadImageA8R8G8B8, (WRITEDIMAGECALL)WriteImageA8R8G8B8 };

    // 0x600185a4
    ImageCalls ImageX8R8G8B8Self = { (RELEASEIMAGECALL)ReleaseAbstractImage, (READIMAGECALL)ReadImageX8R8G8B8, (WRITEDIMAGECALL)WriteImageX8R8G8B8 };
    
    // 0x600185b0
    ImageCalls ImageR5G6B5Self = { (RELEASEIMAGECALL)ReleaseAbstractImage, (READIMAGECALL)ReadImageR5G6B5, (WRITEDIMAGECALL)WriteImageR5G6B5 };

    // 0x600185bc
    ImageCalls ImageX1R5G5B5Self = { (RELEASEIMAGECALL)ReleaseAbstractImage, (READIMAGECALL)ReadImageX1R5G5B5, (WRITEDIMAGECALL)WriteImageX1R5G5B5 };

    // 0x600185c8
    ImageCalls ImageA1R5G5B5Self = { (RELEASEIMAGECALL)ReleaseAbstractImage, (READIMAGECALL)ReadImageA1R5G5B5, (WRITEDIMAGECALL)WriteImageA1R5G5B5 };

    // 0x600185d4
    ImageCalls ImageA4R4G4B4Self = { (RELEASEIMAGECALL)ReleaseAbstractImage, (READIMAGECALL)ReadImageA4R4G4B4, (WRITEDIMAGECALL)WriteImageA4R4G4B4 };

    // 0x600185e0
    ImageCalls ImageR3G3B2Self = { (RELEASEIMAGECALL)ReleaseAbstractImage, (READIMAGECALL)ReadImageR3G3B2, (WRITEDIMAGECALL)WriteImageR3G3B2 };

    // 0x600185ec
    ImageCalls ImageA8Self = { (RELEASEIMAGECALL)ReleaseAbstractImage, (READIMAGECALL)ReadImageA8, (WRITEDIMAGECALL)WriteImageA8 };
    
    // 0x600185f8
    ImageCalls ImageA8R3G3B2Self = { (RELEASEIMAGECALL)ReleaseAbstractImage, (READIMAGECALL)ReadImageA8R3G3B2, (WRITEDIMAGECALL)WriteImageA8R3G3B2 };

    // 0x60018604
    ImageCalls ImageX4R4G4B4Self = { (RELEASEIMAGECALL)ReleaseAbstractImage, (READIMAGECALL)ReadImageX4R4G4B4, (WRITEDIMAGECALL)WriteImageX4R4G4B4 };

    // 0x60018610
    ImageCalls ImageA8P8Self = { (RELEASEIMAGECALL)ReleaseAbstractImage, (READIMAGECALL)ReadImageA8P8, (WRITEDIMAGECALL)WriteImageA8P8 };

    // 0x6001861c
    ImageCalls ImageP8Self = { (RELEASEIMAGECALL)ReleaseAbstractImage, (READIMAGECALL)ReadImageP8, (WRITEDIMAGECALL)WriteImageP8 };

    // 0x60018628
    ImageCalls ImageL8Self = { (RELEASEIMAGECALL)ReleaseAbstractImage, (READIMAGECALL)ReadImageL8, (WRITEDIMAGECALL)WriteImageL8 };

    // 0x60018634
    ImageCalls ImageA8L8Self = { (RELEASEIMAGECALL)ReleaseAbstractImage, (READIMAGECALL)ReadImageA8L8, (WRITEDIMAGECALL)WriteImageA8L8 };

    // 0x60018640
    ImageCalls ImageA4L4Self = { (RELEASEIMAGECALL)ReleaseAbstractImage, (READIMAGECALL)ReadImageA4L4, (WRITEDIMAGECALL)WriteImageA4L4 };

    // 0x6001864c
    ImageCalls ImageV8U8Self = { (RELEASEIMAGECALL)ReleaseAbstractImage, (READIMAGECALL)ReadImageV8U8, (WRITEDIMAGECALL)WriteImageV8U8 };

    // 0x60018658
    ImageCalls ImageL6V5U5Self = { (RELEASEIMAGECALL)ReleaseAbstractImage, (READIMAGECALL)ReadImageL6V5U5, (WRITEDIMAGECALL)WriteImageL6V5U5 };

    // 0x60018664
    ImageCalls ImageX8L8V8U8Self = { (RELEASEIMAGECALL)ReleaseAbstractImage, (READIMAGECALL)ReadImageX8L8V8U8, (WRITEDIMAGECALL)WriteImageX8L8V8U8 };

    // 0x60018670
    ImageCalls ImageQ8W8V8U8Self = { (RELEASEIMAGECALL)ReleaseAbstractImage, (READIMAGECALL)ReadImageQ8W8V8U8, (WRITEDIMAGECALL)WriteImageQ8W8V8U8 };

    // 0x6001867c
    ImageCalls ImageV16U16Self = { (RELEASEIMAGECALL)ReleaseAbstractImage, (READIMAGECALL)ReadImageV16U16, (WRITEDIMAGECALL)WriteImageV16U16 };

    // 0x60018688
    ImageCalls ImageW11V11U10Self = { (RELEASEIMAGECALL)ReleaseAbstractImage, (READIMAGECALL)ReadImageW11V11U10, (WRITEDIMAGECALL)WriteImageW11V11U10 };

    // 0x60018694
    ImageCalls ImageUYVYSelf = { (RELEASEIMAGECALL)ReleaseImageUYVY, (READIMAGECALL)ReadImageUYVY, (WRITEDIMAGECALL)WriteImageUYVY };

    // 0x600186a0
    ImageCalls ImageYUY2Self = { (RELEASEIMAGECALL)ReleaseImageUYVY, (READIMAGECALL)ReadImageUYVY, (WRITEDIMAGECALL)WriteImageUYVY };

    // 0x600186ac
    ImageCalls ImageDXT1Self = { (RELEASEIMAGECALL)ReleaseImageDXT, (READIMAGECALL)ReadImageDXT, (WRITEDIMAGECALL)WriteImageDXT };

    // 0x600186b8
    ImageCalls ImageDXT2Self = { (RELEASEIMAGECALL)ReleaseImageDXT, (READIMAGECALL)ReadImageDXT, (WRITEDIMAGECALL)WriteImageDXT };

    // 0x600186c4
    ImageCalls ImageDXT3Self = { (RELEASEIMAGECALL)ReleaseImageDXT, (READIMAGECALL)ReadImageDXT, (WRITEDIMAGECALL)WriteImageDXT };
    
    // 0x600186d0
    ImageCalls ImageDXT4Self = { (RELEASEIMAGECALL)ReleaseImageDXT, (READIMAGECALL)ReadImageDXT, (WRITEDIMAGECALL)WriteImageDXT };

    // 0x600186dc
    ImageCalls ImageDXT5Self = { (RELEASEIMAGECALL)ReleaseImageDXT, (READIMAGECALL)ReadImageDXT, (WRITEDIMAGECALL)WriteImageDXT };

    // 0x60009150
    void InitializeImageContainer(ImageContainer* self)
    {
        self->Destination = NULL;
        self->Source = NULL;
    }

    // 0x6000915a
    void ReleaseImageContainer(ImageContainer* self)
    {
        if (self->Destination != NULL) { self->Destination->Self->Release(self->Destination, IMAGE_RELEASE_DISPOSE); }
        if (self->Source != NULL) { self->Source->Self->Release(self->Source, IMAGE_RELEASE_DISPOSE); }
    }

    // 0x6000c360
    HRESULT InitializeImageContainer(ImageContainer* self, ImageContainerArgs* dst, ImageContainerArgs* src, const u32 options)
    {
        self->Destination = NULL;
        self->Source = NULL;
        self->Options = options;

        if ((options & IMAGE_CONTAINER_OPTIONS_LOW_VALUE_MASK) == IMAGE_CONTAINER_OPTIONS_NONE
            || (IMAGE_CONTAINER_OPTIONS_UNKNOWN_4 | IMAGE_CONTAINER_OPTIONS_COLOR) < (options & IMAGE_CONTAINER_OPTIONS_LOW_VALUE_MASK)
            || (options & IMAGE_CONTAINER_OPTIONS_HIGH_VALUE_MASK)) { return D3DERR_INVALIDCALL; }

        dst->IsGradient = options & IMAGE_CONTAINER_OPTIONS_GRADIENT;

        self->Destination = InitializeAbstractImage(dst);

        if (self->Destination != NULL)
        {
            self->Source = InitializeAbstractImage(src);

            if (self->Source != NULL
                && self->Destination->Options == self->Source->Options
                &&
                (TRUE /* && TODO NOT IMPLEMENTED */))
            {
                if (self->Destination != NULL) { self->Destination->Self->Release(self->Destination, IMAGE_RELEASE_DISPOSE); self->Destination = NULL; }
                if (self->Source != NULL) { self->Source->Self->Release(self->Source, IMAGE_RELEASE_DISPOSE); self->Source = NULL; }

                return NO_ERROR;
            }
        }

        if (self->Destination != NULL) { self->Destination->Self->Release(self->Destination, IMAGE_RELEASE_DISPOSE); self->Destination = NULL; }
        if (self->Source != NULL) { self->Source->Self->Release(self->Source, IMAGE_RELEASE_DISPOSE); self->Source = NULL; }

        return E_FAIL;
    }

    // 0x6000f55a
    AbstractImage* InitializeAbstractImage(ImageContainerArgs* args)
    {
        AbstractImage* result = NULL;

        switch (args->Format)
        {
        case D3DFMT_R8G8B8:
        {
            result = (AbstractImage*)malloc(sizeof(ImageBitMap));

            if (result != NULL)
            {
                ZeroMemory(result, sizeof(ImageBitMap));

                InitializeImageBitMap((ImageBitMap*)result, args, GRAPHICS_BITS_PER_PIXEL_24, IMAGE_OPTIONS_UNKNOWN_1);

                result->Self = &ImageR8G8B8Self;

                return result;
            }

            break;
        }
        case D3DFMT_A8R8G8B8:
        {
            result = (AbstractImage*)malloc(sizeof(ImageBitMap));

            if (result != NULL)
            {
                ZeroMemory(result, sizeof(ImageBitMap));

                InitializeImageBitMap((ImageBitMap*)result, args, GRAPHICS_BITS_PER_PIXEL_32, IMAGE_OPTIONS_UNKNOWN_1);

                result->Self = &ImageA8R8G8B8Self;

                return result;
            }

            break;
        }
        case D3DFMT_X8R8G8B8:
        {
            result = (AbstractImage*)malloc(sizeof(ImageBitMap));

            if (result != NULL)
            {
                ZeroMemory(result, sizeof(ImageBitMap));

                InitializeImageBitMap((ImageBitMap*)result, args, GRAPHICS_BITS_PER_PIXEL_32, IMAGE_OPTIONS_UNKNOWN_1);

                result->Self = &ImageX8R8G8B8Self;

                return result;
            }

            break;
        }
        case D3DFMT_R5G6B5:
        {
            result = (AbstractImage*)malloc(sizeof(ImageBitMap));

            if (result != NULL)
            {
                ZeroMemory(result, sizeof(ImageBitMap));

                InitializeImageBitMap((ImageBitMap*)result, args, GRAPHICS_BITS_PER_PIXEL_16, IMAGE_OPTIONS_UNKNOWN_1);

                result->Self = &ImageR5G6B5Self;

                return result;
            }

            break;
        }
        case D3DFMT_X1R5G5B5:
        {
            result = (AbstractImage*)malloc(sizeof(ImageBitMap));

            if (result != NULL)
            {
                ZeroMemory(result, sizeof(ImageBitMap));

                InitializeImageBitMap((ImageBitMap*)result, args, GRAPHICS_BITS_PER_PIXEL_16, IMAGE_OPTIONS_UNKNOWN_1);

                result->Self = &ImageX1R5G5B5Self;

                return result;
            }

            break;
        }
        case D3DFMT_A1R5G5B5:
        {
            result = (AbstractImage*)malloc(sizeof(ImageBitMap));

            if (result != NULL)
            {
                ZeroMemory(result, sizeof(ImageBitMap));

                InitializeImageBitMap((ImageBitMap*)result, args, GRAPHICS_BITS_PER_PIXEL_16, IMAGE_OPTIONS_UNKNOWN_1);

                result->Self = &ImageA1R5G5B5Self;

                return result;
            }

            break;
        }
        case D3DFMT_A4R4G4B4:
        {
            result = (AbstractImage*)malloc(sizeof(ImageBitMap));

            if (result != NULL)
            {
                ZeroMemory(result, sizeof(ImageBitMap));

                InitializeImageBitMap((ImageBitMap*)result, args, GRAPHICS_BITS_PER_PIXEL_16, IMAGE_OPTIONS_UNKNOWN_1);

                result->Self = &ImageA4R4G4B4Self;

                return result;
            }

            break;
        }
        case D3DFMT_R3G3B2:
        {
            result = (AbstractImage*)malloc(sizeof(ImageBitMap));

            if (result != NULL)
            {
                ZeroMemory(result, sizeof(ImageBitMap));

                InitializeImageBitMap((ImageBitMap*)result, args, GRAPHICS_BITS_PER_PIXEL_8, IMAGE_OPTIONS_UNKNOWN_1);

                result->Self = &ImageR3G3B2Self;

                return result;
            }

            break;
        }
        case D3DFMT_A8:
        {
            result = (AbstractImage*)malloc(sizeof(ImageBitMap));

            if (result != NULL)
            {
                ZeroMemory(result, sizeof(ImageBitMap));

                InitializeImageBitMap((ImageBitMap*)result, args, GRAPHICS_BITS_PER_PIXEL_8, IMAGE_OPTIONS_UNKNOWN_1);

                result->Self = &ImageA8Self;

                return result;
            }

            break;
        }
        case D3DFMT_A8R3G3B2:
        {
            result = (AbstractImage*)malloc(sizeof(ImageBitMap));

            if (result != NULL)
            {
                ZeroMemory(result, sizeof(ImageBitMap));

                InitializeImageBitMap((ImageBitMap*)result, args, GRAPHICS_BITS_PER_PIXEL_16, IMAGE_OPTIONS_UNKNOWN_1);

                result->Self = &ImageA8R3G3B2Self;

                return result;
            }

            break;
        }
        case D3DFMT_X4R4G4B4:
        {
            result = (AbstractImage*)malloc(sizeof(ImageBitMap));

            if (result != NULL)
            {
                ZeroMemory(result, sizeof(ImageBitMap));

                InitializeImageBitMap((ImageBitMap*)result, args, GRAPHICS_BITS_PER_PIXEL_16, IMAGE_OPTIONS_UNKNOWN_1);

                result->Self = &ImageX4R4G4B4Self;

                return result;
            }

            break;
        }
        case D3DFMT_A8P8:
        {
            result = (AbstractImage*)malloc(sizeof(ImageBitMap));

            if (result != NULL)
            {
                ZeroMemory(result, sizeof(ImageBitMap));

                InitializeImageBitMap((ImageBitMap*)result, args, GRAPHICS_BITS_PER_PIXEL_16, IMAGE_OPTIONS_UNKNOWN_2);

                result->Self = &ImageA8P8Self;

                return result;
            }

            break;
        }
        case D3DFMT_P8:
        {
            result = (AbstractImage*)malloc(sizeof(ImageBitMap));

            if (result != NULL)
            {
                ZeroMemory(result, sizeof(ImageBitMap));

                InitializeImageBitMap((ImageBitMap*)result, args, GRAPHICS_BITS_PER_PIXEL_8, IMAGE_OPTIONS_UNKNOWN_2);

                result->Self = &ImageP8Self;

                return result;
            }

            break;
        }
        case D3DFMT_L8:
        {
            result = (AbstractImage*)malloc(sizeof(ImageBitMap));

            if (result != NULL)
            {
                ZeroMemory(result, sizeof(ImageBitMap));

                InitializeImageBitMap((ImageBitMap*)result, args, GRAPHICS_BITS_PER_PIXEL_8, IMAGE_OPTIONS_UNKNOWN_1);

                result->Self = &ImageL8Self;

                return result;
            }

            break;
        }
        case D3DFMT_A8L8:
        {
            result = (AbstractImage*)malloc(sizeof(ImageBitMap));

            if (result != NULL)
            {
                ZeroMemory(result, sizeof(ImageBitMap));

                InitializeImageBitMap((ImageBitMap*)result, args, GRAPHICS_BITS_PER_PIXEL_16, IMAGE_OPTIONS_UNKNOWN_1);

                result->Self = &ImageA8L8Self;

                return result;
            }

            break;
        }
        case D3DFMT_A4L4:
        {
            result = (AbstractImage*)malloc(sizeof(ImageBitMap));

            if (result != NULL)
            {
                ZeroMemory(result, sizeof(ImageBitMap));

                InitializeImageBitMap((ImageBitMap*)result, args, GRAPHICS_BITS_PER_PIXEL_8, IMAGE_OPTIONS_UNKNOWN_1);

                result->Self = &ImageA4L4Self;

                return result;
            }

            break;
        }
        case D3DFMT_V8U8:
        {
            result = (AbstractImage*)malloc(sizeof(ImageBitMap));

            if (result != NULL)
            {
                ZeroMemory(result, sizeof(ImageBitMap));

                InitializeImageBitMap((ImageBitMap*)result, args, GRAPHICS_BITS_PER_PIXEL_16, IMAGE_OPTIONS_UNKNOWN_3);

                result->Self = &ImageV8U8Self;

                return result;
            }

            break;
        }
        case D3DFMT_L6V5U5:
        {
            result = (AbstractImage*)malloc(sizeof(ImageBitMap));

            if (result != NULL)
            {
                ZeroMemory(result, sizeof(ImageBitMap));

                InitializeImageBitMap((ImageBitMap*)result, args, GRAPHICS_BITS_PER_PIXEL_16, IMAGE_OPTIONS_UNKNOWN_3);

                result->Self = &ImageL6V5U5Self;

                return result;
            }

            break;
        }
        case D3DFMT_X8L8V8U8:
        {
            result = (AbstractImage*)malloc(sizeof(ImageBitMap));

            if (result != NULL)
            {
                ZeroMemory(result, sizeof(ImageBitMap));

                InitializeImageBitMap((ImageBitMap*)result, args, GRAPHICS_BITS_PER_PIXEL_32, IMAGE_OPTIONS_UNKNOWN_3);

                result->Self = &ImageX8L8V8U8Self;

                return result;
            }

            break;
        }
        case D3DFMT_Q8W8V8U8:
        {
            result = (AbstractImage*)malloc(sizeof(ImageBitMap));

            if (result != NULL)
            {
                ZeroMemory(result, sizeof(ImageBitMap));

                InitializeImageBitMap((ImageBitMap*)result, args, GRAPHICS_BITS_PER_PIXEL_32, IMAGE_OPTIONS_UNKNOWN_3);

                result->Self = &ImageQ8W8V8U8Self;

                return result;
            }

            break;
        }
        case D3DFMT_V16U16:
        {
            result = (AbstractImage*)malloc(sizeof(ImageBitMap));

            if (result != NULL)
            {
                ZeroMemory(result, sizeof(ImageBitMap));

                InitializeImageBitMap((ImageBitMap*)result, args, GRAPHICS_BITS_PER_PIXEL_32, IMAGE_OPTIONS_UNKNOWN_3);

                result->Self = &ImageV16U16Self;

                return result;
            }

            break;
        }
        case D3DFMT_W11V11U10:
        {
            result = (AbstractImage*)malloc(sizeof(ImageBitMap));

            if (result != NULL)
            {
                ZeroMemory(result, sizeof(ImageBitMap));

                InitializeImageBitMap((ImageBitMap*)result, args, GRAPHICS_BITS_PER_PIXEL_32, IMAGE_OPTIONS_UNKNOWN_3);

                result->Self = &ImageW11V11U10Self;

                return result;
            }

            break;
        }
        case D3DFMT_DXT1:
        {
            result = (AbstractImage*)malloc(sizeof(ImageDXT));

            if (result != NULL)
            {
                ZeroMemory(result, sizeof(ImageDXT));

                InitializeImageDXT((ImageDXT*)result, args);

                result->Self = &ImageDXT1Self;

                return result;
            }

            break;
        }
        case D3DFMT_DXT2:
        {
            result = (AbstractImage*)malloc(sizeof(ImageDXT));

            if (result != NULL)
            {
                ZeroMemory(result, sizeof(ImageDXT));

                InitializeImageDXT((ImageDXT*)result, args);

                result->Self = &ImageDXT2Self;

                return result;
            }

            break;
        }
        case D3DFMT_DXT3:
        {
            result = (AbstractImage*)malloc(sizeof(ImageDXT));

            if (result != NULL)
            {
                ZeroMemory(result, sizeof(ImageDXT));

                InitializeImageDXT((ImageDXT*)result, args);

                result->Self = &ImageDXT3Self;

                return result;
            }

            break;
        }
        case D3DFMT_DXT4:
        {
            result = (AbstractImage*)malloc(sizeof(ImageDXT));

            if (result != NULL)
            {
                ZeroMemory(result, sizeof(ImageDXT));

                InitializeImageDXT((ImageDXT*)result, args);

                result->Self = &ImageDXT4Self;

                return result;
            }

            break;
        }
        case D3DFMT_DXT5:
        {
            result = (AbstractImage*)malloc(sizeof(ImageDXT));

            if (result != NULL)
            {
                ZeroMemory(result, sizeof(ImageDXT));

                InitializeImageDXT((ImageDXT*)result, args);

                result->Self = &ImageDXT5Self;

                return result;
            }

            break;
        }
        case D3DFMT_UYVY:
        {
            result = (AbstractImage*)malloc(sizeof(ImageUYVY));

            if (result != NULL)
            {
                ZeroMemory(result, sizeof(ImageUYVY));

                InitializeImageUYVY((ImageUYVY*)result, args);

                result->Self = &ImageUYVYSelf;

                return result;
            }

            break;
        }
        case D3DFMT_YUY2:
        {
            result = (AbstractImage*)malloc(sizeof(ImageUYVY));

            if (result != NULL)
            {
                ZeroMemory(result, sizeof(ImageUYVY));

                InitializeImageUYVY((ImageUYVY*)result, args);

                result->Self = &ImageYUY2Self;

                return result;
            }

            break;
        }
        }

        return result;
    }

    // 0x6000c47c
    void AcquireImageColor(ImageColor* self, const u32 color)
    {
        self->R = RGBA_GETRED(color) * (1.0f / 255.0f);
        self->G = RGBA_GETGREEN(color) * (1.0f / 255.0f);
        self->B = RGBA_GETBLUE(color) * (1.0f / 255.0f);
        self->A = RGBA_GETALPHA(color) * (1.0f / 255.0f);
    }

    // 0x6000f4a0
    AbstractImage* ReleaseAbstractImage(AbstractImage* self, const u32 mode)
    {
        AssignImageBitMapSelf((ImageBitMap*)self);

        if (mode & IMAGE_RELEASE_DISPOSE) { free(self); }

        return self;
    }

    // 0x6000c82f
    ImageBitMap* InitializeImageBitMap(ImageBitMap* self, ImageContainerArgs* args, const u32 bits, const ImageOptions options)
    {
        self->Self = &ImageBitMapSelf;

        self->Pixels = args->Pixels;
        self->Format = args->Format;
        self->Stride = args->Stride;

        self->AreaStride = args->AreaStride;

        CopyMemory(&self->Dimensions, &args->Dimensions, sizeof(ImageDimensions));

        ImageColor color;
        AcquireImageColor(&color, args->Color);

        CopyMemory(&self->Color, &color, sizeof(ImageColor));
        
        self->IsColor = args->Color != GRAPCHICS_COLOR_BLACK;

        self->Modifiers = args->IsGradient ? ImageGradientColorModifiers : ImageLinearColorModifiers;

        self->Bytes = bits >> 3;
        self->IsBitMap = (bits != GRAPHICS_BITS_PER_PIXEL_0);
        self->Options = options;

        if (self->Options == IMAGE_OPTIONS_UNKNOWN_2)
        {
            self->Options = IMAGE_OPTIONS_UNKNOWN_1;
            self->IsPalette = TRUE;

            if (args->Palette == NULL)
            {
                for (u32 x = 0; x < MAX_IMAGE_PALETTE_VALUES_COUNT; x++)
                {
                    self->Palette[x].R = 1.0f;
                    self->Palette[x].G = 1.0f;
                    self->Palette[x].B = 1.0f;
                    self->Palette[x].A = 1.0f;
                }
            }
            else
            {
                for (u32 x = 0; x < MAX_IMAGE_PALETTE_VALUES_COUNT; x++)
                {
                    self->Palette[x].R = args->Palette[x * 4 + 0] * (1.0f / 255.0f);
                    self->Palette[x].G = args->Palette[x * 4 + 1] * (1.0f / 255.0f);
                    self->Palette[x].B = args->Palette[x * 4 + 2] * (1.0f / 255.0f);
                    self->Palette[x].A = args->Palette[x * 4 + 3] * (1.0f / 255.0f);
                }
            }
        }
        else { self->IsPalette = FALSE; }
        
        self->Height = self->Dimensions.Bottom - self->Dimensions.Top;
        self->Levels = self->Dimensions.Max - self->Dimensions.Min;
        self->Width = self->Dimensions.Right - self->Dimensions.Left;
        self->BytesStride = self->Bytes * self->Width;

        if (self->IsBitMap)
        {
            self->Pixels = (void*)((addr)self->Pixels
                + (addr)(self->AreaStride * self->Dimensions.Min)
                + (addr)(self->Stride * self->Dimensions.Top)
                + (addr)(self->Bytes * self->Dimensions.Left));

            self->Dimensions.Left = 0;
            self->Dimensions.Top = 0;
            self->Dimensions.Right = self->Width;
            self->Dimensions.Bottom = self->Height;

            self->Dimensions.Min = 0;
            self->Dimensions.Max = self->Levels;
        }

        return self;
    }

    // 0x6000c472
    // 0x6000f4bc
    void AssignImageBitMapSelf(ImageBitMap* self)
    {
        self->Self = &ImageBitMapSelf;
    }

    // 0x6000ca18
    ImageBitMap* ReleaseImageBitMap(ImageBitMap* self, const u32 mode)
    {
        AssignImageBitMapSelf(self);

        if (mode & IMAGE_RELEASE_DISPOSE) { free(self); }

        return self;
    }

    // 0x6000c479
    void ReadImageBitMap(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels) { }

    // 0x6000c479
    void WriteImageBitMap(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels) { }

    // 0x6000ca34
    void ImproveImageColors(AbstractImage* self, ImageColor* pixels)
    {
        for (u32 x = 0; x < self->Width; x++)
        {
            if (pixels[x].R == self->Color.R && pixels[x].G == self->Color.G
                && pixels[x].B == self->Color.B && pixels[x].A == self->Color.A)
            {
                pixels[x].R = 0.0f;
                pixels[x].G = 0.0f;
                pixels[x].B = 0.0f;
                pixels[x].A = 0.0f;
            }
        }
    }

    // 0x6000dd03
    void ReadImageR8G8B8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        u8* values = (u8*)((addr)self->Pixels + (addr)(line * self->Stride) + (addr)(level * self->AreaStride));

        for (u32 x = 0; x < self->Width; x++)
        {
            pixels[x].R = values[2] * (1.0f / 255.0f);
            pixels[x].G = values[1] * (1.0f / 255.0f);
            pixels[x].B = values[0] * (1.0f / 255.0f);
            pixels[x].A = 1.0f;

            values = (u8*)((addr)values + 3 * sizeof(u8));
        }

        if (self->IsColor) { ImproveImageColors((AbstractImage*)self, pixels); }
    }

    // 0x6000caa4
    void WriteImageR8G8B8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        FPUSTART();

        u8* values = (u8*)((addr)self->Pixels + (addr)(self->Stride * line) + (addr)(self->AreaStride * level));

        for (u32 x = 0; x < self->Width; x++)
        {
            const f32 value = self->Modifiers[(level & 3) + (line & 3) * 8 + (x & 3)];

            values[0] = (u8)(pixels[x].B * 255.0f + value);
            values[1] = (u8)(pixels[x].G * 255.0f + value);
            values[2] = (u8)(pixels[x].R * 255.0f + value);

            values = (u8*)((addr)values + 3 * sizeof(u8));
        }

        FPUEND();
    }

    // 0x6000dd8f
    void ReadImageA8R8G8B8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        u8* values = (u8*)((addr)self->Pixels + (addr)(line * self->Stride) + (addr)(level * self->AreaStride));

        for (u32 x = 0; x < self->Width; x++)
        {
            pixels[x].R = values[2] * (1.0f / 255.0f);
            pixels[x].G = values[1] * (1.0f / 255.0f);
            pixels[x].B = values[0] * (1.0f / 255.0f);
            pixels[x].A = values[3] * (1.0f / 255.0f);

            values = (u8*)((addr)values + 4 * sizeof(u8));
        }

        if (self->IsColor) { ImproveImageColors((AbstractImage*)self, pixels); }
    }

    // 0x6000cb65
    void WriteImageA8R8G8B8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        FPUSTART();

        u8* values = (u8*)((addr)self->Pixels + (addr)(self->Stride * line) + (addr)(self->AreaStride * level));

        for (u32 x = 0; x < self->Width; x++)
        {
            const f32 value = self->Modifiers[(level & 3) + (line & 3) * 8 + (x & 3)];

            values[0] = (u8)(pixels[x].B * 255.0f + value);
            values[1] = (u8)(pixels[x].G * 255.0f + value);
            values[2] = (u8)(pixels[x].R * 255.0f + value);
            values[3] = (u8)(pixels[x].A * 255.0f + value);

            values = (u8*)((addr)values + 4 * sizeof(u8));
        }

        FPUEND();
    }

    // 0x6000de31
    void ReadImageX8R8G8B8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        u8* values = (u8*)((addr)self->Pixels + (addr)(line * self->Stride) + (addr)(level * self->AreaStride));

        for (u32 x = 0; x < self->Width; x++)
        {
            pixels[x].R = values[2] * (1.0f / 255.0f);
            pixels[x].G = values[1] * (1.0f / 255.0f);
            pixels[x].B = values[0] * (1.0f / 255.0f);
            pixels[x].A = 1.0f;

            values = (u8*)((addr)values + 4 * sizeof(u8));
        }

        if (self->IsColor) { ImproveImageColors((AbstractImage*)self, pixels); }
    }

    // 0x6000cc5e
    void WriteImageX8R8G8B8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        FPUSTART();

        u8* values = (u8*)((addr)self->Pixels + (addr)(self->Stride * line) + (addr)(self->AreaStride * level));

        for (u32 x = 0; x < self->Width; x++)
        {
            const f32 value = self->Modifiers[(level & 3) + (line & 3) * 8 + (x & 3)];

            values[3] = 0;
            values[2] = (u8)(pixels[x].R * 255.0f + value);
            values[1] = (u8)(pixels[x].G * 255.0f + value);
            values[0] = (u8)(pixels[x].B * 255.0f + value);

            values = (u8*)((addr)values + 4 * sizeof(u8));
        }

        FPUEND();
    }

    // 0x6000dec9
    void ReadImageR5G6B5(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        u16* values = (u16*)((addr)self->Pixels + (addr)(line * self->Stride) + (addr)(level * self->AreaStride));

        for (u32 x = 0; x < self->Width; x++)
        {
            pixels[x].R = (values[x] >> 11) * (1.0f / 31.0f);
            pixels[x].G = ((values[x] & 0x7e0) >> 5) * (1.0f / 63.0f);
            pixels[x].B = (values[x] & 0x1f) * (1.0f / 31.0f);
            pixels[x].A = 1.0f;
        }

        if (self->IsColor) { ImproveImageColors((AbstractImage*)self, pixels); }
    }

    // 0x6000cd35
    void WriteImageR5G6B5(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        FPUSTART();

        u16* values = (u16*)((addr)self->Pixels + (addr)(self->Stride * line) + (addr)(self->AreaStride * level));

        for (u32 x = 0; x < self->Width; x++)
        {
            const f32 value = self->Modifiers[(level & 3) + (line & 3) * 8 + (x & 3)];

            values[x] = (u16)((((u32)(pixels[x].R * 31.0f + value)) << 11)
                            | (((u32)(pixels[x].G * 63.0f + value)) << 5)
                            | (((u32)(pixels[x].B * 31.0f + value)) << 0));
        }

        FPUEND();
    }

    // 0x6000df6e
    void ReadImageX1R5G5B5(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        u16* values = (u16*)((addr)self->Pixels + (addr)(line * self->Stride) + (addr)(level * self->AreaStride));

        for (u32 x = 0; x < self->Width; x++)
        {
            pixels[x].R = ((values[x] & 0x7c00) >> 10) * (1.0f / 31.0f);
            pixels[x].G = ((values[x] & 0x3e0) >> 5) * (1.0f / 31.0f);
            pixels[x].B = (values[x] & 0x1f) * (1.0f / 31.0f);
            pixels[x].A = 1.0f;
        }

        if (self->IsColor) { ImproveImageColors((AbstractImage*)self, pixels); }
    }

    // 0x6000ce0c
    void WriteImageX1R5G5B5(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        FPUSTART();

        u16* values = (u16*)((addr)self->Pixels + (addr)(self->Stride * line) + (addr)(self->AreaStride * level));

        for (u32 x = 0; x < self->Width; x++)
        {
            const f32 value = self->Modifiers[(level & 3) + (line & 3) * 8 + (x & 3)];

            values[x] = (u16)((((u32)(pixels[x].R * 31.0f + value)) << 10)
                            | (((u32)(pixels[x].G * 31.0f + value)) << 5)
                            | (((u32)(pixels[x].B * 31.0f + value)) << 0));
        }

        FPUEND();
    }

    // 0x6000e010
    void ReadImageA1R5G5B5(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        u16* values = (u16*)((addr)self->Pixels + (addr)(line * self->Stride) + (addr)(level * self->AreaStride));

        for (u32 x = 0; x < self->Width; x++)
        {
            pixels[x].R = ((values[x] & 0x7c00) >> 10) * (1.0f / 31.0f);
            pixels[x].G = ((values[x] & 0x3e0) >> 5) * (1.0f / 31.0f);
            pixels[x].B = (values[x] & 0x1f) * (1.0f / 31.0f);
            pixels[x].A = (f32)(values[x] >> 0xf);
        }

        if (self->IsColor) { ImproveImageColors((AbstractImage*)self, pixels); }
    }

    // 0x6000cee3
    void WriteImageA1R5G5B5(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        FPUSTART();

        u16* values = (u16*)((addr)self->Pixels + (addr)(self->Stride * line) + (addr)(self->AreaStride * level));

        for (u32 x = 0; x < self->Width; x++)
        {
            const f32 value = self->Modifiers[(level & 3) + (line & 3) * 8 + (x & 3)];

            values[x] = (u16)((((u32)(pixels[x].A + value)) << 15)
                            | (((u32)(pixels[x].R * 31.0f + value)) << 10)
                            | (((u32)(pixels[x].G * 31.0f + value)) << 5)
                            | (((u32)(pixels[x].B * 31.0f + value)) << 0));
        }

        FPUEND();
    }

    // 0x6000e0c0
    void ReadImageA4R4G4B4(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        u8* values = (u8*)((addr)self->Pixels + (addr)(line * self->Stride) + (addr)(level * self->AreaStride));

        for (u32 x = 0; x < self->Width; x++)
        {
            pixels[x].R = (values[x * 2 + 1] & 0xf) * (1.0f / 15.0f);
            pixels[x].G = (values[x * 2 + 0] >> 4) * (1.0f / 15.0f);
            pixels[x].B = (values[x * 2 + 0] & 0xf) * (1.0f / 15.0f);
            pixels[x].A = (values[x * 2 + 1] >> 4) * (1.0f / 15.0f);
        }

        if (self->IsColor) { ImproveImageColors((AbstractImage*)self, pixels); }
    }
    
    // 0x6000cfd6
    void WriteImageA4R4G4B4(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        FPUSTART();

        u16* values = (u16*)((addr)self->Pixels + (addr)(self->Stride * line) + (addr)(self->AreaStride * level));

        for (u32 x = 0; x < self->Width; x++)
        {
            const f32 value = self->Modifiers[(level & 3) + (line & 3) * 8 + (x & 3)];

            values[x] = (u16)((((u32)(pixels[x].A * 15.0f + value)) << 12)
                            | (((u32)(pixels[x].R * 15.0f + value)) << 8)
                            | (((u32)(pixels[x].G * 15.0f + value)) << 4)
                            | (((u32)(pixels[x].B * 15.0f + value)) << 0));
        }

        FPUEND();
    }

    // 0x6000e16f
    void ReadImageR3G3B2(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        u8* values = (u8*)((addr)self->Pixels + (addr)(line * self->Stride) + (addr)(level * self->AreaStride));

        for (u32 x = 0; x < self->Width; x++)
        {
            pixels[x].R = (values[x] >> 5) * (1.0f / 7.0f);
            pixels[x].G = ((values[x] >> 2) & 7) * (1.0f / 7.0f);
            pixels[x].B = (values[x] & 3) * (1.0f / 3.0f);
            pixels[x].A = 1.0f;
        }

        if (self->IsColor) { ImproveImageColors((AbstractImage*)self, pixels); }
    }

    // 0x6000d0cf
    void WriteImageR3G3B2(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        FPUSTART();

        u8* values = (u8*)((addr)self->Pixels + (addr)(self->Stride * line) + (addr)(self->AreaStride * level));

        for (u32 x = 0; x < self->Width; x++)
        {
            const f32 value = self->Modifiers[(level & 3) + (line & 3) * 8 + (x & 3)];

            values[x] = (u8)((((u32)(pixels[x].R * 7.0f + value)) << 5)
                            | (((u32)(pixels[x].G * 7.0f + value)) << 2)
                            | (((u32)(pixels[x].B * 3.0f + value)) << 0));
        }

        FPUEND();
    }

    // 0x6000e211
    void ReadImageA8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        u8* values = (u8*)((addr)self->Pixels + (addr)(line * self->Stride) + (addr)(level * self->AreaStride));

        for (u32 x = 0; x < self->Width; x++)
        {
            pixels[x].R = 1.0f;
            pixels[x].G = 1.0f;
            pixels[x].B = 1.0f;
            pixels[x].A = values[x] * (1.0f / 255.0f);
        }

        if (self->IsColor) { ImproveImageColors((AbstractImage*)self, pixels); }
    }

    // 0x6000d1a0
    void WriteImageA8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        FPUSTART();

        u8* values = (u8*)((addr)self->Pixels + (addr)(self->Stride * line) + (addr)(self->AreaStride * level));

        for (u32 x = 0; x < self->Width; x++)
        {
            const f32 value = self->Modifiers[(level & 3) + (line & 3) * 8 + (x & 3)];

            values[x] = (u8)(pixels[x].A * 255.0f + value);
        }

        FPUEND();
    }

    // 0x6000e283
    void ReadImageA8R3G3B2(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        u8* values = (u8*)((addr)self->Pixels + (addr)(line * self->Stride) + (addr)(level * self->AreaStride));

        for (u32 x = 0; x < self->Width; x++)
        {
            pixels[x].R = (values[0] >> 5) * (1.0f / 7.0f);
            pixels[x].G = ((values[0] >> 2) & 7) * (1.0f / 7.0f);
            pixels[x].B = (values[0] & 3) * (1.0f / 3.0f);
            pixels[x].A = values[1] * (1.0f / 255.0f);

            values = (u8*)((addr)values + 2 * sizeof(u8));
        }

        if (self->IsColor) { ImproveImageColors((AbstractImage*)self, pixels); }
    }

    // 0x6000d222
    void WriteImageA8R3G3B2(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        FPUSTART();

        u16* values = (u16*)((addr)self->Pixels + (addr)(self->Stride * line) + (addr)(self->AreaStride * level));

        for (u32 x = 0; x < self->Width; x++)
        {
            const f32 value = self->Modifiers[(level & 3) + (line & 3) * 8 + (x & 3)];

            values[x] = (u16)((((u32)(pixels[x].A * 255.0f + value)) << 8)
                            | (((u32)(pixels[x].R * 7.0f + value)) << 5)
                            | (((u32)(pixels[x].G * 7.0f + value)) << 2)
                            | (((u32)(pixels[x].B * 3.0f + value)) << 0));
        }

        FPUEND();
    }

    // 0x6000e335
    void ReadImageX4R4G4B4(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        u8* values = (u8*)((addr)self->Pixels + (addr)(line * self->Stride) + (addr)(level * self->AreaStride));

        for (u32 x = 0; x < self->Width; x++)
        {
            pixels[x].R = (values[1] & 0xf) * (1.0f / 15.0f);
            pixels[x].G = (values[0] >> 4) * (1.0f / 15.0f);
            pixels[x].B = (values[0] & 0xf) * (1.0f / 15.0f);
            pixels[x].A = 1.0f;

            values = (u8*)((addr)values + 2 * sizeof(u8));
        }

        if (self->IsColor) { ImproveImageColors((AbstractImage*)self, pixels); }
    }

    // 0x6000d31b
    void WriteImageX4R4G4B4(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        FPUSTART();

        u16* values = (u16*)((addr)self->Pixels + (addr)(self->Stride * line) + (addr)(self->AreaStride * level));

        for (u32 x = 0; x < self->Width; x++)
        {
            const f32 value = self->Modifiers[(level & 3) + (line & 3) * 8 + (x & 3)];

            values[x] = (u16)((((u32)(pixels[x].R * 15.0f + value)) << 8)
                            | (((u32)(pixels[x].G * 15.0f + value)) << 4)
                            | (((u32)(pixels[x].B * 15.0f + value)) << 0));
        }

        FPUEND();
    }

    // 0x6000e3d4
    void ReadImageA8P8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        u8* values = (u8*)((addr)self->Pixels + (addr)(line * self->Stride) + (addr)(level * self->AreaStride));

        for (u32 x = 0; x < self->Width; x++)
        {
            ImageColor color = self->Palette[values[0]];

            pixels[x].R = color.R;
            pixels[x].G = color.G;
            pixels[x].B = color.B;
            pixels[x].A = values[1] * (1.0f / 255.0f);

            values = (u8*)((addr)values + 2 * sizeof(u8));
        }

        if (self->IsColor) { ImproveImageColors((AbstractImage*)self, pixels); }
    }

    // 0x6000c7d0
    void AcquirePaletteColor(ImageColor* self, ImageColor* color, ImageColor* palette)
    {
        color->R = self->R - palette->R;
        color->G = self->G - palette->G;
        color->B = self->B - palette->B;
        color->A = self->A - palette->A;
    }

    // 0x6000d3f2
    void WriteImageA8P8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        FPUSTART();

        u16* values = (u16*)((addr)self->Pixels + (addr)(self->Stride * line) + (addr)(self->AreaStride * level));

        for (u32 x = 0; x < self->Width; x++)
        {
            const f32 value = self->Modifiers[(level & 3) + (line & 3) * 8 + (x & 3)];

            u32 indx = 0;

            for (u32 xx = 0; xx < MAX_IMAGE_PALETTE_VALUES_COUNT; xx++)
            {
                ImageColor pixel;
                AcquirePaletteColor(pixels, &pixel, &self->Palette[xx]);

                const f32 color =
                    pixel.R * pixel.R + pixel.G * pixel.G + pixel.B * pixel.B;

                if (color < F32_MAX) { indx = xx; }
            }

            values[x] = (u16)((((u32)(pixels[x].A * 255.0f + value)) << 8) | indx);
        }

        FPUEND();
    }

    // 0x6000e45b
    void ReadImageP8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        u8* values = (u8*)((addr)self->Pixels + (addr)(line * self->Stride) + (addr)(level * self->AreaStride));

        for (u32 x = 0; x < self->Width; x++)
        {
            ImageColor color = self->Palette[values[x]];

            pixels[x].R = color.R;
            pixels[x].G = color.G;
            pixels[x].B = color.B;
            pixels[x].A = color.A;
        }

        if (self->IsColor) { ImproveImageColors((AbstractImage*)self, pixels); }
    }

    // 0x6000d4ef
    void WriteImageP8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        u8* values = (u8*)((addr)self->Pixels + (addr)(self->Stride * line) + (addr)(self->AreaStride * level));

        for (u32 x = 0; x < self->Width; x++)
        {
            u32 indx = 0;
            f32 value = F32_MAX;

            for (u32 xx = 0; xx < MAX_IMAGE_PALETTE_VALUES_COUNT; xx++)
            {
                ImageColor pixel;
                AcquirePaletteColor(&pixels[x], &pixel, &self->Palette[xx]);

                const f32 color =
                    pixel.R * pixel.R + pixel.G * pixel.G + pixel.B * pixel.B + pixel.A * pixel.A;

                if (color < value)
                {
                    indx = xx;
                    value = color;
                }
            }

            values[x] = (u8)indx;
        }
    }

    // 0x6000e4bf
    void ReadImageL8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        u8* values = (u8*)((addr)self->Pixels + (addr)(line * self->Stride) + (addr)(level * self->AreaStride));

        for (u32 x = 0; x < self->Width; x++)
        {
            const f32 color = values[x] * (1.0f * (1.0f / 255.0f));

            pixels[x].R = color;
            pixels[x].G = color;
            pixels[x].B = color;
            pixels[x].A = 1.0f;
        }

        if (self->IsColor) { ImproveImageColors((AbstractImage*)self, pixels); }
    }

    // 0x6000d5a9
    void WriteImageL8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        FPUSTART();

        u8* values = (u8*)((addr)self->Pixels + (addr)(self->Stride * line) + (addr)(self->AreaStride * level));

        for (u32 x = 0; x < self->Width; x++)
        {
            const f32 value = self->Modifiers[(level & 3) + (line & 3) * 8 + (x & 3)];

            values[x] = (u8)(value + 255.0f * (pixels[x].R * 0.2125f + pixels[x].G * 0.7154f + pixels[x].B * 0.0721f));
        }

        FPUEND();
    }

    // 0x6000e52d
    void ReadImageA8L8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        u8* values = (u8*)((addr)self->Pixels + (addr)(line * self->Stride) + (addr)(level * self->AreaStride));

        for (u32 x = 0; x < self->Width; x++)
        {
            const f32 color = values[0] * (1.0f / 255.0f);

            pixels[x].R = color;
            pixels[x].G = color;
            pixels[x].B = color;
            pixels[x].A = values[1] * (1.0f / 255.0f);

            values = (u8*)((addr)values + 2 * sizeof(u8));
        }

        if (self->IsColor) { ImproveImageColors((AbstractImage*)self, pixels); }
    }

    // 0x6000d647
    void WriteImageA8L8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        FPUSTART();

        u16* values = (u16*)((addr)self->Pixels + (addr)(self->Stride * line) + (addr)(self->AreaStride * level));

        for (u32 x = 0; x < self->Width; x++)
        {
            const f32 value = self->Modifiers[(level & 3) + (line & 3) * 8 + (x & 3)];

            values[x] = (u16)(((u32)(pixels[x].A * 255.0f + value) << 8)
                            | ((u32)((pixels[x].R * 0.2125f + pixels[x].G * 0.7154f + pixels[x].B * 0.0721f) * 255.0f + value)));
        }

        FPUEND();
    }

    // 0x6000e5b6
    void ReadImageA4L4(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        u8* values = (u8*)((addr)self->Pixels + (addr)(line * self->Stride) + (addr)(level * self->AreaStride));

        for (u32 x = 0; x < self->Width; x++)
        {
            const f32 color = (values[x] & 0xf) * (1.0f / 15.0f);

            pixels[x].R = color;
            pixels[x].G = color;
            pixels[x].B = color;
            pixels[x].A = (values[x] >> 4) * (1.0f / 15.0f);
        }

        if (self->IsColor) { ImproveImageColors((AbstractImage*)self, pixels); }
    }

    // 0x6000d717
    void WriteImageA4L4(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        FPUSTART();

        u8* values = (u8*)((addr)self->Pixels + (addr)(self->Stride * line) + (addr)(self->AreaStride * level));

        for (u32 x = 0; x < self->Width; x++)
        {
            const f32 value = self->Modifiers[(level & 3) + (line & 3) * 8 + (x & 3)];

            values[x] = (u8)(((u32)(pixels[x].A * 15.0f + value) << 4)
                | ((u32)((pixels[x].R * 0.2125f + pixels[x].G * 0.7154f + pixels[x].B * 0.0721f) * 15.0f + value)));
        }

        FPUEND();
    }

    // 0x6000c4d2
    void ReadImageV8U8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        s8* values = (s8*)((addr)self->Pixels + (addr)(line * self->Stride) + (addr)(level * self->AreaStride));

        for (u32 x = 0; x < self->Width; x++)
        {
            pixels[x].R = values[0] * (1.0f / 128.0f);
            pixels[x].G = values[1] * (1.0f / 128.0f);
            pixels[x].B = 0.0f;
            pixels[x].A = 1.0f;

            values = (s8*)((addr)values + 2 * sizeof(s8));
        }
    }

    // 0x6000d7df
    void WriteImageV8U8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        FPUSTART();

        u16* values = (u16*)((addr)self->Pixels + (addr)(self->Stride * line) + (addr)(self->AreaStride * level));

        for (u32 x = 0; x < self->Width; x++)
        {
            const f32 value = self->Modifiers[(level & 3) + (line & 3) * 8 + (x & 3)];

            values[x] = (u16)(((u32)(pixels[x].G * 128.0f + value) << 8) | ((u32)(pixels[x].R * 128.0f + value)));
        }

        FPUEND();
    }

    // 0x6000c53b
    void ReadImageL6V5U5(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        u16* values = (u16*)((addr)self->Pixels + (addr)(line * self->Stride) + (addr)(level * self->AreaStride));

        for (u32 x = 0; x < self->Width; x++)
        {
            const u16 v = values[x];

            pixels[x].R = (f32)((s32)((s8)((u8)v << 3)) >> 3) * (1.0f / 16.0f);
            pixels[x].G = (f32)((s32)((s8)((u8)(v >> 5) << 3)) >> 3) * (1.0f / 16.0f);
            pixels[x].B = 0.0f;
            pixels[x].A = (f32)(v >> 10) * (1.0f / 63.0f);
        }
    }

    // 0x6000d88f
    void WriteImageL6V5U5(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        FPUSTART();

        u16* values = (u16*)((addr)self->Pixels + (addr)(self->Stride * line) + (addr)(self->AreaStride * level));

        for (u32 x = 0; x < self->Width; x++)
        {
            const f32 value = self->Modifiers[(level & 3) + (line & 3) * 8 + (x & 3)];

            values[x] = (u16)(((u32)(pixels[x].A * 63.0f + value) << 10)
                            | (((u32)(pixels[x].G * 16.0f + value) & 0x1f) << 5)
                            | ((u32)(pixels[x].R * 16.0f + value) & 0x1f));
        }

        FPUEND();
    }

    // 0x6000c5d1
    void ReadImageX8L8V8U8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        s8* values = (s8*)((addr)self->Pixels + (addr)(line * self->Stride) + (addr)(level * self->AreaStride));

        for (u32 x = 0; x < self->Width; x++)
        {
            pixels[x].R = values[0] * (1.0f / 128.0f);
            pixels[x].G = values[1] * (1.0f / 128.0f);
            pixels[x].B = 0.0f;
            pixels[x].A = (u8)values[2] * (1.0f / 255.0f);

            values = (s8*)((addr)values + 4 * sizeof(s8));
        }
    }

    // 0x6000d96e
    void WriteImageX8L8V8U8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        FPUSTART();

        u32* values = (u32*)((addr)self->Pixels + (addr)(self->Stride * line) + (addr)(self->AreaStride * level));

        for (u32 x = 0; x < self->Width; x++)
        {
            const f32 value = self->Modifiers[(level & 3) + (line & 3) * 8 + (x & 3)];

            values[x] = (u32)(((u32)(pixels[x].A * 255.0f + value) << 16)
                            | (((u32)(pixels[x].G * 128.0f + value) & 0xff) << 8)
                            | ((u32)(pixels[x].R * 128.0f + value) & 0xff));
        }

        FPUEND();
    }

    // 0x6000c649
    void ReadImageQ8W8V8U8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        s8* values = (s8*)((addr)self->Pixels + (addr)(line * self->Stride) + (addr)(level * self->AreaStride));

        for (u32 x = 0; x < self->Width; x++)
        {
            pixels[x].R = values[0] * (1.0f / 128.0f);
            pixels[x].G = values[1] * (1.0f / 128.0f);
            pixels[x].B = values[2] * (1.0f / 128.0f);
            pixels[x].A = values[3] * (1.0f / 128.0f);

            values = (s8*)((addr)values + 4 * sizeof(s8));
        }
    }

    // 0x6000da55
    void WriteImageQ8W8V8U8(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        FPUSTART();

        u32* values = (u32*)((addr)self->Pixels + (addr)(self->Stride * line) + (addr)(self->AreaStride * level));

        for (u32 x = 0; x < self->Width; x++)
        {
            const f32 value = self->Modifiers[(level & 3) + (line & 3) * 8 + (x & 3)];

            values[x] = (u32)(((u32)(pixels[x].A * 128.0f + value) << 24)
                            | (((u32)(pixels[x].B * 128.0f + value) & 0xff) << 16)
                            | (((u32)(pixels[x].G * 128.0f + value) & 0xff) << 8)
                            | ((u32)(pixels[x].R * 128.0f + value) & 0xff));
        }

        FPUEND();
    }

    // 0x6000c6c7
    void ReadImageV16U16(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        s16* values = (s16*)((addr)self->Pixels + (addr)(line * self->Stride) + (addr)(level * self->AreaStride));

        for (u32 x = 0; x < self->Width; x++)
        {
            pixels[x].R = values[x * 2 + 0] * (1.0f / 32768.0f);
            pixels[x].G = values[x * 2 + 1] * (1.0f / 32768.0f);
            pixels[x].B = 0.0f;
            pixels[x].A = 1.0f;
        }
    }

    // 0x6000db66
    void WriteImageV16U16(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        FPUSTART();

        u32* values = (u32*)((addr)self->Pixels + (addr)(self->Stride * line) + (addr)(self->AreaStride * level));

        for (u32 x = 0; x < self->Width; x++)
        {
            const f32 value = self->Modifiers[(level & 3) + (line & 3) * 8 + (x & 3)];

            values[x] = (u32)(((u32)(pixels[x].G * 32768.0f + value) << 16)
                            | ((u32)(pixels[x].R * 32768.0f + value) & 0xffff));
        }

        FPUEND();
    }

    // 0x6000c731
    void ReadImageW11V11U10(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        s32* values = (s32*)((addr)self->Pixels + (addr)(line * self->Stride) + (addr)(level * self->AreaStride));

        for (u32 x = 0; x < self->Width; x++)
        {
            pixels[x].R = ((s16)((u16)values[x] << 6) >> 6) * (1.0f / 512.0f);
            pixels[x].G = ((s16)((u16)(values[x] >> 10) << 5) >> 5) * (1.0f / 1024.0f);
            pixels[x].B = (values[x] >> 21) * (1.0f / 1024.0f);
            pixels[x].A = 1.0f;
        }
    }

    // 0x6000dc17
    void WriteImageW11V11U10(ImageBitMap* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        FPUSTART();

        u32* values = (u32*)((addr)self->Pixels + (addr)(self->Stride * line) + (addr)(self->AreaStride * level));

        for (u32 x = 0; x < self->Width; x++)
        {
            const f32 value = self->Modifiers[(level & 3) + (line & 3) * 8 + (x & 3)];

            values[x] = (u32)((((u32)(pixels[x].B * 1024.0f + value) & 0xfffffffe) << 21)
                            | (((u32)(pixels[x].G * 1024.0f + value) & 0x7ff) << 10)
                            | ((u32)(pixels[x].R * 512.0f + value) & 0x3ff));
        }

        FPUEND();
    }

    // 0x6000ecbb
    ImageDXT* InitializeImageDXT(ImageDXT* self, ImageContainerArgs* args)
    {
        InitializeImageBitMap((ImageBitMap*)self, args, GRAPHICS_BITS_PER_PIXEL_0, IMAGE_OPTIONS_UNKNOWN_1);

        self->Self = &AbstractImageDXTSelf;

        self->Unk0x1098 = 0;

        switch (args->Width)
        {
        case 1: // TODO
        {
            self->Unk0x109c = 0; // TODO
            self->Unk0x1098 = 1; // TODO

            break;
        }
        case 2: // TODO
        {
            self->Unk0x109c = 1; // TODO
            self->Unk0x1098 = 1; // TODO

            break;
        }
        default:
        {
            self->Unk0x109c = 3; // TODO
            self->Unk0x1098 = 1; // TODO

            break;
        }
        }

        switch (args->Height)
        {
        case 1: // TODO
        {
            self->Unk0x10a0 = 0; // TODO
            self->Unk0x1098 = 1; // TODO

            break;
        }
        case 2: // TODO
        {
            self->Unk0x10a0 = 1; // TODO
            self->Unk0x1098 = 1; // TODO

            break;
        }
        default:
        {
            self->Unk0x10a0 = 3; // TODO
            self->Unk0x1098 = 0; // TODO
        }
        }

        const u32 rigth = ALIGN(self->Dimensions.Right);
        const u32 left = self->Dimensions.Left & ALIGNMENTMASK;

        self->ActualRight = rigth;
        self->ActualLeft = left;
        self->ActualWidth = rigth - left; 

        self->MinLine = 0;
        self->MinLevel = 0;
        self->MaxLine = 0;
        self->MaxLevel = 0;
        
        self->Unk0x10a4 = 0;
        
        self->IsAlloc = TRUE;
        self->IsInit = FALSE;

        for (u32 x = 0; x < IMAGE_QUAD_COLOR_COUNT; x++)
        {
            const u32 size = self->ActualWidth * sizeof(ImageColor);

            self->Colors[x] = (ImageColor*)malloc(size);

            if (self->Colors[x] == NULL) { self->IsAlloc = FALSE; }
            else { ZeroMemory(self->Colors[x], size); }
        }

        return self;
    }

    // 0x6000edd2
    HRESULT InitializeImageDXT(ImageDXT* self)
    {
        if (!self->IsInit || !self->IsAlloc) { return NO_ERROR; }

        ImageQuad quad;

        const BOOL alpha = self->Format == D3DFMT_DXT2 || self->Format == D3DFMT_DXT4;

        for (u32 x = self->ActualLeft; x < self->ActualRight; x = x + IMAGE_QUAD_COLOR_COUNT)
        {
            FPUSTART();

            if (!alpha)
            {
                for (u32 xx = 0; xx < IMAGE_QUAD_COLOR_COUNT; xx++)
                {
                    ImageColor* colors = (ImageColor*)((addr)self->Colors[xx] + (addr)(x - self->ActualLeft));

                    for (u32 xxx = 0; xxx < IMAGE_QUAD_COLOR_COUNT; xxx++)
                    {
                        const u32 r = (u32)(colors[xx].R * 255.0f + 0.5f);
                        const u32 g = (u32)(colors[xx].G * 255.0f + 0.5f);
                        const u32 b = (u32)(colors[xx].B * 255.0f + 0.5f);
                        const u32 a = (u32)(colors[xx].A * 255.0f + 0.5f);

                        quad.ARGB[xxx] = RGBA_MAKE(r, g, b, a);
                    }
                }
            }
            else
            {
                for (u32 xx = 0; xx < IMAGE_QUAD_COLOR_COUNT; xx++)
                {
                    ImageColor* colors = (ImageColor*)((addr)self->Colors[xx] + (addr)(x - self->ActualLeft));

                    for (u32 xxx = 0; xxx < IMAGE_QUAD_COLOR_COUNT; xxx++)
                    {
                        const u32 r = (u32)(colors[xx].R * colors[xx].A * 255.0f + 0.5f);
                        const u32 g = (u32)(colors[xx].G * colors[xx].A * 255.0f + 0.5f);
                        const u32 b = (u32)(colors[xx].B * colors[xx].A * 255.0f + 0.5f);
                        const u32 a = (u32)(colors[xx].A * 255.0f + 0.5f);

                        quad.ARGB[xxx] = RGBA_MAKE(r, g, b, a);
                    }
                }
            }

            FPUEND();

            if (self->Unk0x1098 != 0) // TODO
            {
                for (u32 xx = 0; xx < IMAGE_QUAD_COLOR_COUNT; xx++)
                {
                    for (u32 xxx = 0; xxx < IMAGE_QUAD_COLOR_COUNT; xxx++)
                    {
                        quad.ARGB[xxx] = quad.ARGB[(self->Unk0x109c & xxx) + (self->Unk0x10a0 & xx)];
                    }
                }
            }

            if (self->Format == D3DFMT_DXT1)
            {
                UnpackImageDXT1(x >> 4, &quad,
                    (u16*)((addr)self->Pixels + (addr)((self->MinLine >> 2) * self->Stride)
                        + (addr)(self->MinLevel * self->AreaStride) + (addr)((x >> 4) * 8)));
            }
            // TODO
            //else if (self->Format == D3DFMT_DXT2 || self->Format == D3DFMT_DXT3)
            //{
            //    UnpackImageDXT2(x / IMAGE_DXT_DIMENSION * 16, values,
            //        (u16*)((addr)self->Pixels + (addr)(self->MinLine / IMAGE_DXT_DIMENSION * self->Stride)
            //            + (addr)(self->MinLevel * self->AreaStride) + (addr)(x / IMAGE_DXT_DIMENSION * 16))); // TODO
            //}
            //else if (self->Format == D3DFMT_DXT4 || self->Format == D3DFMT_DXT5)
            //{
            //    UnpackImageDXT4(x / IMAGE_DXT_DIMENSION * 16, values,
            //        (u16*)((addr)self->Pixels + (addr)(self->MinLine / IMAGE_DXT_DIMENSION * self->Stride)
            //            + (addr)(self->MinLevel * self->AreaStride) + (addr)(x / IMAGE_DXT_DIMENSION * 16))); // TODO
            //}
        }

        self->IsInit = FALSE;

        return NO_ERROR;
    }

    // 0x6000f506
    // 0x6000fbdf
    void AssignImageDXTSelf(ImageDXT* self)
    {
        self->Self = &AbstractImageDXTSelf;

        InitializeImageDXT(self);

        for (u32 x = 0; x < IMAGE_QUAD_COLOR_COUNT; x++)
        {
            if (self->Colors[x] != NULL) { free(self->Colors[x]); }
        }

        AssignImageBitMapSelf((ImageBitMap*)self);
    }

    // 0x6000fb86
    ImageDXT* ReleaseAbstractImageDXT(ImageDXT* self, const u32 mode)
    {
        AssignImageDXTSelf(self);

        if (mode & IMAGE_RELEASE_DISPOSE) { free(self); }

        return self;
    }

    // 0x6000fbc3
    ImageDXT* ReleaseImageDXT(ImageDXT* self, const u32 mode)
    {
        AssignImageDXTSelf(self);

        if (mode & IMAGE_RELEASE_DISPOSE) { free(self); }

        return self;
    }

    // 0x6000f0ff
    HRESULT AcquireImageDXT(ImageDXT* self, const u32 line, const u32 level, const BOOL fill)
    {
        if (!self->IsAlloc) { return ERROR_OUTOFMEMORY; }

        if (line < self->MinLine || self->MaxLine <= line
            || level < self->MinLevel || self->MaxLevel <= level)
        {
            {
                const HRESULT result = InitializeImageDXT(self);

                if (result != NO_ERROR) { return result; }
            }

            self->MinLine = line & ALIGNMENTMASK;
            self->MaxLine = (line & ALIGNMENTMASK) + 4;
            self->MinLevel = level;
            self->MaxLevel = level + 1;

            if (self->MinLine < self->Unk0x10a4) { self->Unk0x10a4 = self->MinLine; }

            if (fill)
            {
                const BOOL alpha = self->Format == D3DFMT_DXT2 || self->Format == D3DFMT_DXT4;

                for (u32 x = self->ActualLeft; x < self->ActualRight; x = x + IMAGE_QUAD_COLOR_COUNT)
                {
                    ImageQuad quad;
                    ZeroMemory(&quad, sizeof(ImageQuad));

                    if (self->Format == D3DFMT_DXT1)
                    {
                        u16* pixels = (u16*)((addr)self->Pixels
                            + (addr)(self->AreaStride * level) + (addr)(self->Stride * (line >> 2))
                            + (addr)((self->ActualLeft >> 2) * 8));

                        AcquireImageQuadDXT1(pixels, &quad);
                    }
                    //else if (self->Format == D3DFMT_DXT2 || self->Format == D3DFMT_DXT4)
                    //{
                    //    TODO
                    //}
                    //else if (self->Format == D3DFMT_DXT3 || self->Format == D3DFMT_DXT5)
                    //{
                    //    TODO
                    //}

                    for (u32 xx = 0; xx < IMAGE_QUAD_COLOR_COUNT; xx++)
                    {
                        ImageColor* colors = self->Colors[x - self->ActualLeft + xx];

                        for (u32 xxx = 0; xxx < IMAGE_QUAD_COLOR_COUNT; xxx++)
                        {
                            const u32 pixel = quad.ARGB[xxx];

                            colors[xxx].A = RGBA_GETALPHA(pixel) / 255.0f;
                            colors[xxx].R = RGBA_GETRED(pixel) / 255.0f;
                            colors[xxx].G = RGBA_GETGREEN(pixel) / 255.0f;
                            colors[xxx].B = RGBA_GETBLUE(pixel) / 255.0f;

                            if (alpha)
                            {
                                // TODO
                            }
                        }
                    }
                }
            }
        }

        return NO_ERROR;
    }

    // 0x6000f42c
    void ReadImageDXT(ImageDXT* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        if (AcquireImageDXT(self, line + self->Dimensions.Top, level + self->Dimensions.Min, TRUE) == NO_ERROR)
        {
            ImageColor* colors = (ImageColor*)((addr)self->Colors[0]
                + (addr)(IMAGE_QUAD_COLOR_COUNT * (line + self->Dimensions.Top - self->MinLine))
                + (addr)(IMAGE_QUAD_PIXEL_COUNT * (self->Dimensions.Left - self->ActualLeft)));

            CopyMemory(pixels, colors, self->Width * sizeof(ImageColor));

            if (self->IsColor) { ImproveImageColors((AbstractImage*)self, pixels); }
        }
    }

    // 0x6000f3a6
    void WriteImageDXT(ImageDXT* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        if (AcquireImageDXT(self, line + self->Dimensions.Top, level + self->Dimensions.Min, self->ActualWidth != self->Width) == NO_ERROR)
        {
            ImageColor* colors = (ImageColor*)((addr)self->Colors[0]
                + (addr)(IMAGE_QUAD_COLOR_COUNT * (line + self->Dimensions.Top - self->MinLine))
                + (addr)(IMAGE_QUAD_PIXEL_COUNT * (self->Dimensions.Left - self->ActualLeft)));

            CopyMemory(colors, pixels, self->Width * sizeof(ImageColor));

            self->IsInit = TRUE;
        }
    }

    // 0x6000fbe4
    void UnpackImageDXT1(const u32 indx, ImageQuad* quad, u16* pixels)
    {
        // Simple
        {
            u32 x = 0;

            for (; x < IMAGE_QUAD_PIXEL_COUNT; x++)
            {
                if (quad->ARGB[x] & 0x80000000) { break; } // Sign bit is set, it is a negative value.
            }

            if (x == IMAGE_QUAD_PIXEL_COUNT)
            {
                // TODO FUN_600106e2(indx, quad, pixels);

                return;
            }
        }

        // Complex
        {
            // TODO
            //for (u32 x = 0; x < IMAGE_QUAD_PIXEL_COUNT; x++)
            //{
            //    u32 xx = 0;

            //    for (; xx < IMAGE_QUAD_PIXEL_COUNT; xx++)
            //    {
            //        if ((s32)colors[xx] < 0 && (colors[xx] & 0xffffff) == x) { break; }
            //    }

            //    if (xx == IMAGE_DXT_DIMENSION_SEGMENT)
            //    {
            //        xx = 0;

            //        for (; xx < IMAGE_DXT_DIMENSION_SEGMENT; xx++)
            //        {
            //            if (-1 < (s32)colors[xx]) { colors[x] = (u32)x; }
            //        }

            //        FUN_60010014(colors, pixels, x, x);

            //        return;
            //    }
            //}
        }
    }

    // 0x6000fc4d
    void AcquireImageQuadDXT1(const u16* pixels, ImageQuad* quad)
    {
        AcquireImageQuad(pixels, quad);

        for (u32 x = 0; x < IMAGE_QUAD_PIXEL_COUNT; x++)
        {
            if (quad->Pixels[x].A == 0x00)
            {
                quad->Pixels[x].R = quad->Pixels[x].G = quad->Pixels[x].B = quad->Pixels[x].A = 0x00;
            }
        }
    }

    // 0x6000e63f
    ImageUYVY* InitializeImageUYVY(ImageUYVY* self, ImageContainerArgs* args)
    {
        InitializeImageBitMap((ImageBitMap*)self, args, GRAPHICS_BITS_PER_PIXEL_0, IMAGE_OPTIONS_UNKNOWN_1);

        self->Self = &AbstractImageUYVYSelf;

        // Align to two
        const s32 left = self->Dimensions.Left & 0xFFFFFFFE;
        const s32 right = (self->Dimensions.Right + 1) & 0xFFFFFFFE;

        self->ActualRight = right;
        self->ActualLeft = left;
        self->ActualWidth = right - left;

        self->MinLine = 0;
        self->MinLevel = 0;
        self->MaxLine = 0;
        self->MaxLevel = 0;

        self->IsAlloc = TRUE;
        self->IsInit = FALSE;

        {
            const u32 size = (self->ActualWidth - self->ActualLeft) * sizeof(ImageColor);

            self->Colors = (ImageColor*)malloc(size);

            if (self->Colors == NULL) { self->IsAlloc = FALSE; }
            else { ZeroMemory(self->Colors, size); }
        }

        if (args->Format == D3DFMT_UYVY)
        {
            self->LumaShift = 8;
            self->ValueShift = 0;
        }
        else
        {
            self->LumaShift = 0;
            self->ValueShift = 8;
        }

        return self;
    }

    // 0x6000e710
    // The UYVY color format is basically a 16 bit color format.
    // In contrast to the RGB formats, it contains no values for red, green or blue.
    // It uses values for luminance and chrominance instead.
    // Y stands for the luminance. U (or Cb) and V (or Cr) describe the chrominance.
    // 
    // Each 4 bytes represent the color for 2 neighboring pixels: [ U0 | Y0 | V0 | Y1 ]
    // 
    // Y0 is the brightness of pixel 0, Y1 the brightness of pixel 1. U0 and V0 is the color of both pixels.
    HRESULT InitializeImageUYVY(ImageUYVY* self)
    {
        if (!self->IsInit || !self->IsAlloc) { return NO_ERROR; }

        u16* values = (u16*)((addr)self->Pixels
                + (addr)(self->MinLevel * self->AreaStride + self->MinLine * self->Stride + 2 * self->ActualLeft));

        for (u32 x = self->ActualLeft; x < self->ActualRight; x = x + 2)
        {
            const ImageColor* colors = &self->Colors[x];

            const u32 yb = Clamp<u32>(16 + (u32)( colors[0].R * 65.481f + colors[0].G * 128.553f + colors[0].B * 24.966f + 0.5f), 0, 255);
            const u32 yr = Clamp<u32>(16 + (u32)( colors[1].R * 65.481f + colors[1].G * 128.553f + colors[1].B * 24.966f + 0.5f), 0, 255);
            const u32 cb = Clamp<u32>(128 + (u32)(-colors[0].R * 37.797f - colors[0].G * 74.203f  + colors[0].B * 112.0f  + 0.5f), 0, 255);
            const u32 cr = Clamp<u32>(128 + (u32)( colors[0].R * 112.0f  - colors[0].G * 93.786f  - colors[0].B * 18.214f + 0.5f), 0, 255);

            values[0] = (u16)(yb << (self->LumaShift & 0x1f)) | (cb << (self->ValueShift & 0x1f));
            values[1] = (u16)(yr << (self->LumaShift & 0x1f)) | (cr << (self->ValueShift & 0x1f));

            values = (u16*)((addr)values + 2 * sizeof(u16));
        }

        self->IsInit = FALSE;

        return NO_ERROR;
    }

    // 0x6000f4c1
    // 0x6000fbbe
    void AssignImageUYVYSelf(ImageUYVY* self)
    {
        self->Self = &AbstractImageUYVYSelf;

        InitializeImageUYVY(self);

        if (self->Colors != NULL) { free(self->Colors); }

        AssignImageBitMapSelf((ImageBitMap*)self);
    }

    // 0x6000fb6a
    ImageUYVY* ReleaseAbstractImageUYVY(ImageUYVY* self, const u32 mode)
    {
        AssignImageUYVYSelf(self);

        if (mode & IMAGE_RELEASE_DISPOSE) { free(self); }

        return self;
    }

    // 0x6000fba2
    ImageUYVY* ReleaseImageUYVY(ImageUYVY* self, const u32 mode)
    {
        AssignImageUYVYSelf(self);

        if (mode & IMAGE_RELEASE_DISPOSE) { free(self); }

        return self;
    }

    // 0x6000ec4c
    void ReadImageUYVY(ImageUYVY* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        if (AcquireImageUYVY(self, line + self->Dimensions.Top, level + self->Dimensions.Min, TRUE) == NO_ERROR)
        {
            ImageColor* colors = (ImageColor*)((addr)self->Colors + (addr)(self->Dimensions.Left - self->ActualLeft));

            CopyMemory(pixels, colors, (self->Width & 0xFFFFFFFC) * sizeof(ImageColor));

            if (self->IsColor) { ImproveImageColors((AbstractImage*)self, pixels); }
        }
    }

    // 0x6000ebd4
    void WriteImageUYVY(ImageUYVY* self, const u32 line, const u32 level, ImageColor* pixels)
    {
        if (AcquireImageUYVY(self, line + self->Dimensions.Top, level + self->Dimensions.Min, self->ActualWidth != self->Width) == NO_ERROR)
        {
            ImageColor* colors = (ImageColor*)((addr)self->Colors + (addr)(self->Dimensions.Left - self->ActualLeft));

            CopyMemory(colors, pixels, (self->Width & 0xFFFFFFFC) * sizeof(ImageColor));

            self->IsInit = TRUE;
        }
    }

    // 0x6000e903
    HRESULT AcquireImageUYVY(ImageUYVY* self, const u32 line, const u32 level, const BOOL fill)
    {
        if (!self->IsAlloc) { return ERROR_OUTOFMEMORY; }

        if (line < self->MinLine || self->MaxLine <= line
            || level < self->MinLevel || self->MaxLevel <= level)
        {
            {
                const HRESULT result = InitializeImageUYVY(self);

                if (result != NO_ERROR) { return result; }
            }

            self->MinLine = line;
            self->MaxLine = line + 1;

            self->MinLevel = level;
            self->MaxLevel = level + 1;

            if (fill)
            {
                ImageColor* colors = self->Colors;
                u16* values = (u16*)((addr)self->Pixels
                    + (addr)(self->AreaStride * level + self->Stride * line + 2 * self->ActualLeft));

                for (u32 x = self->ActualLeft; x < self->ActualRight; x = x + 2)
                {
                    const f32 cb = -128.0f + (values[0] >> (self->ValueShift & 0x1f) & 0xff);
                    const f32 cr = -128.0f + (values[1] >> (self->ValueShift & 0x1f) & 0xff);

                    //

                    const f32 yb = 0.00456621f * (-16.0f + (f32)(values[0] >> (self->LumaShift & 0x1f) & 0xff));

                    f32 r = yb + cr * 0.00625893f;
                    f32 g = yb - cb * 0.00153632f - cr * 0.00318811f;
                    f32 b = yb + cb * 0.00791071f;

                    //

                    colors[x + 0].A = 1.0f;

                    r = max(0.0f, min(r, 1.0f));
                    g = max(0.0f, min(g, 1.0f));
                    b = max(0.0f, min(b, 1.0f));

                    colors[x + 0].R = r;
                    colors[x + 0].G = g;
                    colors[x + 0].B = b;

                    //

                    const f32 yc = 0.00456621f * (-16.0f + (f32)(values[1] >> (self->LumaShift & 0x1f) & 0xff));

                    r = yc + cr * 0.00625893f;
                    g = yc - cb * 0.00153632f - cr * 0.00318811f;
                    b = yc + cb * 0.00791071f;

                    //
                    colors[x + 1].A = 1.0f;

                    r = max(0.0f, min(r, 1.0f));
                    g = max(0.0f, min(g, 1.0f));
                    b = max(0.0f, min(b, 1.0f));

                    colors[x + 1].R = r;
                    colors[x + 1].G = g;
                    colors[x + 1].B = b;

                    //

                    values = (u16*)((addr)values + 2 * sizeof(u16));
                }
            }
        }

        return NO_ERROR;
    }

    // 0x6000fd0a
    // A1R5G5B5 -> A8R8G8B8 ???
    void AcquireImageColor(const u16 in, ImagePixel* out)
    {
        ImagePixel result;

        result.A = 0x00;

        result.R = ((in >> 8) & 0xF8) | ((in >> 8) >> 5);
        result.G = ((in >> 5) << 2) | ((u8)((in >> 5) << 2) >> 6);
        result.B = (in << 3) | ((u8)(in << 3) >> 5);

        *out = result;
    }

    // 0x600104ab
    // https://en.wikipedia.org/wiki/S3_Texture_Compression
    void AcquireImageQuad(const u16* pixels, ImageQuad* quad)
    {
        if (pixels == NULL) { ZeroMemory(quad, sizeof(ImageQuad)); return; }

        ImagePixel colors[IMAGE_QUAD_COLOR_COUNT];

        AcquireImageColor(pixels[0], &colors[0]);
        AcquireImageColor(pixels[1], &colors[1]);

        colors[0].A = 0xFF;
        colors[1].A = 0xFF;
        colors[2].A = 0xFF;

        if (pixels[0] >= pixels[1])
        {
            // c2 = 2/3 * c0 + 1/3 * c1

            colors[2].R = (2 * (u32)colors[0].R  + 1 + (u32)colors[1].R) / 3;
            colors[2].G = (2 * (u32)colors[0].G  + 1 + (u32)colors[1].G) / 3;
            colors[2].B = (2 * (u32)colors[0].B  + 1 + (u32)colors[1].B) / 3;
            
            // c3 = 1/3 * c0 + 2/3 * c1

            colors[3].R = ((u32)colors[0].R + 1 + 2 * (u32)colors[1].R) / 3;
            colors[3].G = ((u32)colors[0].G + 1 + 2 * (u32)colors[1].G) / 3;
            colors[3].B = ((u32)colors[0].B + 1 + 2 * (u32)colors[1].B) / 3;

            colors[3].A = 0xFF;
        }
        else
        {
            // c2 = 1/2 * c0 + 1/2 * c1

            colors[2].R = (colors[0].R + colors[1].R) / 2;
            colors[2].G = (colors[0].G + colors[1].G) / 2;
            colors[2].B = (colors[0].B + colors[1].B) / 2;

            // c3 is transparent black
            colors[3].R = colors[3].G = colors[3].B = colors[3].A = 0x00;
        }

        u32 pix = pixels[2];

        for (u32 x = 0; x < IMAGE_QUAD_PIXEL_COUNT; x++)
        {
            const u32 indx = pix & 3; // % 4

            quad->Pixels[x].R = colors[indx].R;
            quad->Pixels[x].G = colors[indx].G;
            quad->Pixels[x].B = colors[indx].B;
            quad->Pixels[x].A = colors[indx].A;

            pix = pix >> 2;
        }
    }
}