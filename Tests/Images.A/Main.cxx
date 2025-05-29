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

#include "FPU.hxx"

#include "ImageBitMap.hxx"

#include <stdio.h>

s32 main(s32 argc, char** argv)
{
    if (argc != 2)
    {
        printf("Usage: Images.exe <path to reference module>\r\n\tExample: Images.exe dx8z.dll\r\n");

        return EXIT_FAILURE;
    }

    HMODULE module = LoadLibraryA(argv[1]);

    if (module == NULL)
    {
        printf("Unable to load %s\r\n", argv[1]);

        return EXIT_FAILURE;
    }

    printf("FPU: %s\r\n", FPU(module) ? "OK" : "ERROR");

    //printf("A1R5G5B5: %s\r\n", ExecuteImageBitMap(module, D3DFMT_A1R5G5B5) ? "OK" : "ERROR"); // OK
    //printf("A4L4: %s\r\n", ExecuteImageBitMap(module, D3DFMT_A4L4) ? "OK" : "ERROR"); // OK
    //printf("A4R4G4B4: %s\r\n", ExecuteImageBitMap(module, D3DFMT_A4R4G4B4) ? "OK" : "ERROR"); // OK
    //printf("A8: %s\r\n", ExecuteImageBitMap(module, D3DFMT_A8) ? "OK" : "ERROR"); // OK
    //printf("A8L8: %s\r\n", ExecuteImageBitMap(module, D3DFMT_A8L8) ? "OK" : "ERROR"); // OK
    //printf("A8P8: %s\r\n", ExecuteImageBitMap(module, D3DFMT_A8P8) ? "OK" : "ERROR"); // OK
    //printf("A8R3G3B2: %s\r\n", ExecuteImageBitMap(module, D3DFMT_A8R3G3B2) ? "OK" : "ERROR"); // OK
    //printf("A8R8G8B8: %s\r\n", ExecuteImageBitMap(module, D3DFMT_A8R8G8B8) ? "OK" : "ERROR"); // OK

    // TODO
    //printf("DXT1: %s\r\n", ExecuteImageBitMap(module, D3DFMT_DXT1) ? "OK" : "ERROR");
    //printf("DXT2: %s\r\n", ExecuteImageBitMap(module, D3DFMT_DXT2) ? "OK" : "ERROR");
    //printf("DXT3: %s\r\n", ExecuteImageBitMap(module, D3DFMT_DXT3) ? "OK" : "ERROR");
    //printf("DXT4: %s\r\n", ExecuteImageBitMap(module, D3DFMT_DXT4) ? "OK" : "ERROR");
    //printf("DXT5: %s\r\n", ExecuteImageBitMap(module, D3DFMT_DXT5) ? "OK" : "ERROR");

    printf("L6V5U5: %s\r\n", ExecuteImageBitMap(module, D3DFMT_L6V5U5) ? "OK" : "ERROR");
    printf("L8: %s\r\n", ExecuteImageBitMap(module, D3DFMT_L8) ? "OK" : "ERROR");
    printf("P8: %s\r\n", ExecuteImageBitMap(module, D3DFMT_P8) ? "OK" : "ERROR");
    printf("Q8W8V8U8: %s\r\n", ExecuteImageBitMap(module, D3DFMT_Q8W8V8U8) ? "OK" : "ERROR");
    printf("R3G3B2: %s\r\n", ExecuteImageBitMap(module, D3DFMT_R3G3B2) ? "OK" : "ERROR");
    printf("R5G6B5: %s\r\n", ExecuteImageBitMap(module, D3DFMT_R5G6B5) ? "OK" : "ERROR");
    printf("R8G8B8: %s\r\n", ExecuteImageBitMap(module, D3DFMT_R8G8B8) ? "OK" : "ERROR");
    printf("UYVY: %s\r\n", ExecuteImageBitMap(module, D3DFMT_UYVY) ? "OK" : "ERROR");
    printf("V16U16: %s\r\n", ExecuteImageBitMap(module, D3DFMT_V16U16) ? "OK" : "ERROR");
    printf("V8U8: %s\r\n", ExecuteImageBitMap(module, D3DFMT_V8U8) ? "OK" : "ERROR");
    printf("W11V11U10: %s\r\n", ExecuteImageBitMap(module, D3DFMT_W11V11U10) ? "OK" : "ERROR");
    printf("X1R5G5B5: %s\r\n", ExecuteImageBitMap(module, D3DFMT_X1R5G5B5) ? "OK" : "ERROR");
    printf("X4R4G4B4: %s\r\n", ExecuteImageBitMap(module, D3DFMT_X4R4G4B4) ? "OK" : "ERROR");
    printf("X8L8V8U8: %s\r\n", ExecuteImageBitMap(module, D3DFMT_X8L8V8U8) ? "OK" : "ERROR");
    printf("X8R8G8B8: %s\r\n", ExecuteImageBitMap(module, D3DFMT_X8R8G8B8) ? "OK" : "ERROR");
    printf("YUY2: %s\r\n", ExecuteImageBitMap(module, D3DFMT_YUY2) ? "OK" : "ERROR");

    FreeLibrary(module);

    return EXIT_SUCCESS;
}