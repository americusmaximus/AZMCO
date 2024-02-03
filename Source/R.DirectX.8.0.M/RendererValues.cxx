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

#include "Graphics.Basic.hxx"
#include "RendererValues.hxx"

using namespace Renderer;
using namespace RendererModule;

namespace RendererModuleValues
{
    u32 MaxRendererSimultaneousTextures = MIN_SIMULTANEOUS_TEXTURE_COUNT;
    u32 RendererVersion = RENDERER_MODULE_VERSION_115;

    u32 RendererIndexSize = RENDERER_MODULE_INDEX_SIZE_4;

    BOOL IsRendererInit = TRUE;
    u32 RendererVertexSize = sizeof(RTLVX);
    u32 RendererCurrentShader = D3DFVF_TEX1 | D3DFVF_SPECULAR | D3DFVF_DIFFUSE | D3DFVF_XYZRHW;

    u8 RendererFogAlphas[MAX_OUTPUT_FOG_ALPHA_COUNT];

    RendererModuleDescriptor ModuleDescriptor;
    RendererModuleDescriptorDeviceCapabilities ModuleDescriptorDeviceCapabilities[MAX_DEVICE_CAPABILITIES_COUNT];

    s32 RendererTextureFormatStates[MAX_USABLE_TEXTURE_FORMAT_COUNT] =
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    }; // TODO

    RendererModule::MinMax MinMax[6] =
    {
        { 0x0,   0x4a },
        { 0x64,  0x6e },
        { 0xc8,  0xd7 },
        { 0x12c, 0x13b },
        { 0x190, 0x19f },
        { 0x2bc, 0x2cb }
    }; // TODO

    s32 UnknownArray06[MAX_ACTIVE_UNKNOWN_COUNT] =
    {
        0, 0, 0, 0, 4, 0, 0, 0, 0, 0
    }; // TODO
}