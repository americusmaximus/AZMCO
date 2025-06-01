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

#define IMAGE_BPP                       4
#define IMAGE_DIMS                      2048
#define IMAGE_SIZE                      (IMAGE_DIMS * IMAGE_DIMS * IMAGE_BPP)

#define IMAGE_NAME                      "..\\Content\\Image.bmp"

#define IMAGE_RELEASE                   0
#define IMAGE_READ                      1
#define IMAGE_WRITE                     2

#define VIRTUAL_METHOD(SELF, INDEX) (*((VOID***)SELF))[INDEX]

#define ALLOCATE(T) (T*)calloc(sizeof(T), 1)

#define INIT_IMAGE_BASE_ADDRESS         (0x6000F55A - 0x60000000)
#define INIT_IMAGE_FUNC_ADDRESS(M)      ((addr)M + (addr)INIT_IMAGE_BASE_ADDRESS)