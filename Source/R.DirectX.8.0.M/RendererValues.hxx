/*
Copyright (c) 2023 - 2025 Americus Maximus

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

// #include "Images.hxx" // TODO
#include "Renderer.hxx"

namespace RendererModuleValues
{
    extern s32 UnknownArray06[MAX_ACTIVE_UNKNOWN_COUNT]; // 0x6001c010 // TODO
    extern u32 RendererShadeMode; // 0x6001c038
    extern u32 RendererClearColor; // 0x6001c03c
    extern BOOL IsRendererToggleLambdaActive; // 0x6001c040

    extern RendererModule::MinMax MinMax[6]; // 0x6001c04c
    extern BOOL IsRendererInit; // 0x6001c07c
    extern u32 RendererVertexSize; // 0x6001c080
    extern u32 RendererCurrentShader; // 0x6001c084
    extern f32 RendererClearDepth; // 0x6001c088
    extern u32 RendererFogState; // 0x6001c08c
    extern u32 RendererFogColor; // 0x6001c090
    extern f32 RendererFogEnd; // 0x6001c094
    extern f32 RendererMinVertexDepth; // 0x6001c098
    extern f32 RendererMaxVertexDepth; // 0x6001c09c
    extern D3DCMPFUNC RendererDepthFunction; // 0x6001c0a0
    extern D3DCMPFUNC RendererAlphaFunction; // 0x6001c0a4
    extern u32 RendererVertexStride; // 0x6001c0a8
    extern u32 MaxRendererSimultaneousTextures; // 0x6001c0ac
    extern u32 RendererVersion; // 0x6001c0b0

    extern u32 RendererIndexSize; // 0x6001c0b8

    extern BOOL RendererToggleState; // 0x6001c040

    extern u8 RendererFogAlphas[MAX_OUTPUT_FOG_ALPHA_COUNT]; // 0x600208a0

    extern RendererModule::RendererModuleDescriptor ModuleDescriptor; // 0x600428c0
    extern RendererModule::RendererModuleDescriptorDeviceCapabilities ModuleDescriptorDeviceCapabilities[MAX_DEVICE_CAPABILITIES_COUNT]; // 0x600429a0

    extern s32 RendererTextureFormatStates[MAX_USABLE_TEXTURE_FORMAT_COUNT]; // 0x6001d9e0

    extern u32 PixelFormatSizes[MAX_USABLE_TEXTURE_FORMAT_INDEX_COUNT]; // 0x6001c198

    extern BOOL IsTexturePaletteInactive; // 0x6001c220

    extern u32 TexturePaletteIndexes[MAX_TEXTURE_PALETTE_INDEX_COUNT]; // 0x6001f040
    extern u32 TexturePaletteValues[MAX_TEXTURE_PALETTE_INDEX_COUNT]; // 0x6001f840
}