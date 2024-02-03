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

namespace RendererModuleValues
{
    extern BOOL IsRendererInit; // 0x6001c080
    extern u32 RendererVertexSize; // 0x6001c084
    extern u32 RendererCurrentShader; // 0x6001c088

    extern u32 MaxRendererSimultaneousTextures; // 0x6001c0ac
    extern u32 RendererVersion; // 0x6001c0b0

    extern u32 RendererIndexSize; // 0x6001c0b8

    extern u8 RendererFogAlphas[MAX_OUTPUT_FOG_ALPHA_COUNT]; // 0x6001f9a0

    extern RendererModule::RendererModuleDescriptor ModuleDescriptor; // 0x600404c0
    extern RendererModule::RendererModuleDescriptorDeviceCapabilities ModuleDescriptorDeviceCapabilities[MAX_DEVICE_CAPABILITIES_COUNT]; // 0x600405a0

    extern s32 RendererTextureFormatStates[MAX_ACTIVE_USABLE_TEXTURE_FORMAT_COUNT]; // 0x6001d9e0

    extern RendererModule::MinMax MinMax[6]; // 0x6001c050

    extern s32 UnknownArray06[MAX_ACTIVE_UNKNOWN_COUNT]; // 0x6001c010 // TODO
}