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

#include "Image.hxx"
#include "Images.hxx"

#include <Graphics.Basic.hxx>
#include <Images.hxx>

#define IMAGE_RELEASE                   0
#define IMAGE_READ                      1
#define IMAGE_WRITE                     2

#define VIRTUAL_METHOD(SELF, INDEX) (*((VOID***)SELF))[INDEX]

#define ALLOCATE(T) (T*)calloc(sizeof(T), 1)

typedef Images::AbstractImage*(__cdecl* INITIALIZEABSTRACTIMAGEACTION)(Images::ImageContainerArgs*);

typedef Images::ImageBitMap* (__thiscall* IMAGEBITMAPCONSTRUCTORACTION)(Images::ImageBitMap*, Images::ImageContainerArgs* args, const u32 bits, const u32 options);

typedef VOID(__thiscall* IMAGEBITMAPRELEASEACTION)(Images::ImageBitMap*, const u8 mode);
typedef VOID(__thiscall* IMAGEBITMAPREADACTION)(Images::ImageBitMap*, const u32 line, const u32 level, Images::ImageColor* pixels);
typedef VOID(__thiscall* IMAGEBITMAPWRITEACTION)(Images::ImageBitMap*, const u32 line, const u32 level, Images::ImageColor* pixels);

BOOL CompareImageBitMap(const Images::ImageBitMap* a, const Images::ImageBitMap* b);
BOOL CompareImageBitMap(HMODULE module, Images::ImageContainerArgs* a, Images::ImageContainerArgs* b, Images::ImageColor* colors);
BOOL ExecuteImageBitMap(HMODULE module, const D3DFORMAT format);
BOOL ExecuteImageBitMap(HMODULE module, const D3DFORMAT format, const u32 width, const u32 height, const BOOL gradient, const u32 color, Images::ImageColor* colors, const u8* palette, void* pixels);