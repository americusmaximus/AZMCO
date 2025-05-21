/*
Copyright (c) 2024 Americus Maximus

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

#include "Renderer.hxx"

#define DEFAULT_DEVICE_INDEX 0
#define INVALID_DEVICE_INDEX (-1)

namespace RendererModuleValues
{
    extern s32 RendererTextureFormatStates[MAX_USABLE_TEXTURE_FORMAT_COUNT]; // 0x60021030

    extern s32 UnknownArray06[MAX_UNKNOWN_COUNT]; // 0x6002105c // TODO

    extern u32 RendererSurfaceStride; // 0x60021074

    extern u32 Unknown32BitColors1[MAX_UNKNOWN_COLOR_ARAY_COUNT]; // 0x60021098
    extern u32 Unknown32BitColors2[MAX_UNKNOWN_COLOR_ARAY_COUNT]; // 0x60021118
    extern u32 Unknown32BitColors3[MAX_UNKNOWN_COLOR_ARAY_COUNT]; // 0x600210d8
    extern u32 Unknown32BitColors4[MAX_UNKNOWN_COLOR_ARAY_COUNT]; // 0x60021158

    extern u32 Unknown16BitColors1[MAX_UNKNOWN_COLOR_ARAY_COUNT]; // 0x60021198
    extern u32 Unknown16BitColors2[MAX_UNKNOWN_COLOR_ARAY_COUNT]; // 0x60021218
    extern u32 Unknown16BitColors3[MAX_UNKNOWN_COLOR_ARAY_COUNT]; // 0x600211d8
    extern u32 Unknown16BitColors4[MAX_UNKNOWN_COLOR_ARAY_COUNT]; // 0x60021258

    extern u32 GreenRendererColorMask; // 0x60021298
    extern u32 RedRendererColorMask; // 0x6002129c
    extern u32 BlueRendererColorMask; // 0x600212a0
    extern u32 NonGreenRendererColorMask; // 0x600212a4
    extern u32 GreenRendererColorShift; // 0x600212a8
    extern u32 GreenRendererColorLength; // 0x600212ac

    extern RendererModule::MinMax MinMax[6]; // 0x600212b0 // TODO

    extern s32 RendererVideoMode; // 0x600212e0

    extern u32 RendererCullMode; // 0x600212e8

    extern u32 RendererVersion; // 0x600212f4

    extern s32 RendererDeviceIndex; // 0x6002a584

    extern RendererModule::RendererModuleDescriptor ModuleDescriptor; // 0x60a2d440
}