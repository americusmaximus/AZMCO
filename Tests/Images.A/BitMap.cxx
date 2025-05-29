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

#include <stdio.h>
#include <stdlib.h>

typedef struct Color
{
    BYTE B;
    BYTE G;
    BYTE R;
} COLOR, * LPCOLOR;

using namespace Images;

BOOL SavePixels(LPCSTR name, const ImageColor* pixels, const u32 width, const u32 height)
{
    return SavePixels(name, pixels, width, height, width);
}

BOOL SavePixels(LPCSTR name, const ImageColor* pixels, const u32 width, const u32 height, const u32 stride)
{
    BITMAPFILEHEADER header;
    BITMAPINFOHEADER info;

    const u32 bits = sizeof(COLOR) << 3;
    const u32 bistride = ((((width * bits) + 31) & ~31) >> 3);

    const u32 size = bistride * height;

    header.bfType = 0x4D42; // 'BM'
    header.bfSize = size + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    header.bfReserved1 = header.bfReserved2 = 0;
    header.bfOffBits = header.bfSize - size;

    info.biSize = sizeof(BITMAPINFOHEADER);
    info.biWidth = width;
    info.biHeight = height;
    info.biPlanes = 1;
    info.biBitCount = bits;
    info.biCompression = BI_RGB;
    info.biSizeImage = size;
    info.biXPelsPerMeter = 0;
    info.biYPelsPerMeter = 0;
    info.biClrUsed = 0;
    info.biClrImportant = 0;

    LPCOLOR colors = (LPCOLOR)malloc(size);

    if (colors == NULL) { return FALSE; }

    ZeroMemory(colors, size);

    for (u32 y = 0; y < height; y++)
    {
        LPCOLOR line = (LPCOLOR)((addr)colors + (addr)((height - y - 1) * bistride));

        for (u32 x = 0; x < width; x++)
        {
            const ImageColor pixel = pixels[y * stride + x];

            line[x].R = (BYTE)(pixel.R * 255.0f);
            line[x].G = (BYTE)(pixel.G * 255.0f);
            line[x].B = (BYTE)(pixel.B * 255.0f);
        }
    }

    FILE* f = NULL;

    if (fopen_s(&f, name, "wb") != 0) { return FALSE; }

    BOOL result = TRUE;

    result = fwrite(&header, 1, sizeof(BITMAPFILEHEADER), f) == sizeof(BITMAPFILEHEADER);
    result = fwrite(&info, 1, sizeof(BITMAPINFOHEADER), f) == sizeof(BITMAPINFOHEADER);
    result = fwrite(colors, 1, size, f) == size;
    result = fclose(f);

    free(colors);

    return result;
}

BOOL LoadPixels(LPCSTR name, u32* pixels, u32* width, u32* height)
{
    BITMAPFILEHEADER header;
    BITMAPINFOHEADER info;

    FILE* f = NULL;

    if (fopen_s(&f, name, "rb") != 0) { return FALSE; }

    BOOL result = TRUE;

    result = fread(&header, 1, sizeof(BITMAPFILEHEADER), f) == sizeof(BITMAPFILEHEADER);
    result = fread(&info, 1, sizeof(BITMAPINFOHEADER), f) == sizeof(BITMAPINFOHEADER);

    LPCOLOR colors = (LPCOLOR)malloc(info.biSizeImage);

    if (colors == NULL)
    {
        fclose(f);

        return FALSE;
    }

    result = fread(colors, 1, info.biSizeImage, f) == info.biSizeImage;
    
    fclose(f);

    const u32 stride = info.biSizeImage / info.biHeight;

    for (u32 y = 0; y < info.biHeight; y++)
    {
        LPCOLOR line = (LPCOLOR)((addr)colors + (addr)((info.biHeight - y - 1) * stride));

        for (u32 x = 0; x < info.biWidth; x++)
        {
            u32* pixel = (u32*)(((addr)pixels + (addr)(y * stride + x) * 4));

            *pixel = (0xFF << 24) | (line[x].R << 16) | (line[x].G << 8) | line[x].B;
        }
    }

    free(colors);

    if (width != NULL) { *width = info.biWidth; }
    if (height != NULL) { *height = info.biHeight; }

    return result;
}