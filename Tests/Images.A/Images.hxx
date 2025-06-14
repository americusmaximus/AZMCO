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

#pragma once

#include "Base.hxx"

#include <Images.hxx>

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

enum ColorSchema
{
    COLORSCHEMA_BLACK = 0,
    COLORSCHEMA_WHITE = 1,
    COLORSCHEMA_REPEAT = 2,
    COLORSCHEMA_IMAGE = 3
};

struct ImageContainer
{
    Images::ImageContainerArgs  Args;
    void* Pixels;
    Images::ImageBitMap* Image;
};

struct ImageTest
{
    u32         Width;
    u32         Heigh;
    BOOL        Gradient;
    u32         Color;
    BOOL        Colors;
    BOOL        Palette;
    ColorSchema Schema;
};

#define MAX_IMAGE_TEST_CASES        128

extern const ImageTest ImageTests[MAX_IMAGE_TEST_CASES];

BOOL AcquireImage(void* pixels, const ColorSchema schema);
BOOL AcquirePalette(void* palette);
BOOL AcquireColors(Images::ImageColor* colors, void* pixels, const u32 width, const u32 height, const BOOL generate);