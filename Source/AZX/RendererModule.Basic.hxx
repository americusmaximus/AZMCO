/*
Copyright (c) 2023 Americus Maximus

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

#include "Basic.hxx"
#include "Renderer.Basic.hxx"
#include "Native.Basic.hxx"

#define RENDERER_MODULE_FAILURE FALSE
#define RENDERER_MODULE_SUCCESS TRUE

#define RENDERER_MODULE_DX7_ACCELERATION_AVAILABLE 85

#define RENDERER_MODULE_VERSION_113 113

#define RENDERER_MODULE_SIGNATURE_D3D7 0x44334437 /* D3D7 */
#define RENDERER_MODULE_SIGNATURE_D3D8 0x44334438 /* D3D8 */

#define RENDERER_MODULE_VERSION_DX7 7
#define RENDERER_MODULE_VERSION_DX8 8

#define RENDERER_MODULE_MESSAGE_SEVERITY_ERROR 0
#define RENDERER_MODULE_MESSAGE_SEVERITY_WARNING 1
#define RENDERER_MODULE_MESSAGE_SEVERITY_MESSAGE 2

#define MAX_RENDERER_MODULE_DEVICE_NAME_LENGTH 32

#define RENDERER_MODULE_STATE_SELECT_WINDOW_MODE_STATE 18

#define RENDERER_MODULE_CAPS_NONE 0
#define RENDERER_MODULE_CAPS_LINE_WIDTH 0x1
#define RENDERER_MODULE_CAPS_TEXTURE_SQUARE 0x2
#define RENDERER_MODULE_CAPS_TEXTURE_WIDTH_POW2 0x4
#define RENDERER_MODULE_CAPS_TEXTURE_HEIGHT_POW2 0x8
#define RENDERER_MODULE_CAPS_SOFTWARE 0x10
#define RENDERER_MODULE_CAPS_WINDOWED 0x20
#define RENDERER_MODULE_CAPS_GLOBAL_CUT 0x40
#define RENDERER_MODULE_CAPS_TRILINEAR_PASS 0x80
#define RENDERER_MODULE_CAPS_UNKNOWN 0x100

#define RENDERER_MODULE_DISPLAY_ENVIRONMENT_PROPERTY_NAME "THRASH_DISPLAY"

#define RENDERER_MODULE_WIRE_FRAME_DX7_ENVIRONMENT_PROPERTY_NAME "DX7_WIREFRAME"
#define RENDERER_MODULE_WIRE_FRAME_DX8_ENVIRONMENT_PROPERTY_NAME "DX8_WIREFRAME"

#define RENDERER_MODULE_WINDOW_MESSAGE_INITIALIZE_DEVICE 0x464
#define RENDERER_MODULE_WINDOW_MESSAGE_INITIALIZE_SURFACES 0x465
#define RENDERER_MODULE_WINDOW_MESSAGE_RELEASE_DEVICE 0x468

typedef const HWND(STDCALLAPI* RENDERERMODULEACQUIREWINDOWLAMBDA)(void);
typedef const u32(STDCALLAPI* RENDERERMODULEEXECUTECALLBACK)(const void*, const HWND hwnd, const u32 msg, const u32 wp, const u32 lp, HRESULT* result); // TODO
typedef const u32(STDCALLAPI* RENDERERMODULERELEASEMEMORYLAMBDA)(const void*);
typedef const void(STDCALLAPI* RENDERERMODULEEXECUTELAMBDA)(const u32 message, const RENDERERMODULEEXECUTECALLBACK lambda);
typedef const void(STDCALLAPI* RENDERERMODULELOCKWINDOWLAMBDA)(const BOOL state);
typedef const void(STDCALLAPI* RENDERERMODULELOGLAMBDA)(const u32 severity, const char* message);
typedef const void(STDCALLAPI* RENDERERMODULESELECTINSTANCELAMBDA)(const void*);
typedef const void(STDCALLAPI* RENDERERMODULESELECTSTATELAMBDA)(const u32, void*);
typedef void* (STDCALLAPI* RENDERERMODULEALLOCATEMEMORYLAMBDA)(const u32 size);

namespace RendererModule
{
    struct RendererModuleWindowLock;

    struct RendererModuleLambdaContainer
    {
        void* L1; // TODO
        RENDERERMODULEACQUIREWINDOWLAMBDA AcquireWindow;
        RENDERERMODULEEXECUTELAMBDA Execute;
        void* L4; // TODO
        RENDERERMODULESELECTINSTANCELAMBDA SelectInstance;
        RENDERERMODULELOCKWINDOWLAMBDA LockWindow;
        RENDERERMODULELOGLAMBDA Log;
        void* L8; // TODO
    };

    struct RendererModuleDescriptorDeviceCapabilities
    {
        u32 Width;
        u32 Height;
        u32 Bits;
        u32 Format;

        BOOL IsActive;

        u32 Unk03; // TODO
        u32 Unk04; // TODO
        u32 Unk05; // TODO
        u32 Unk06; // TODO
        u32 Unk07; // TODO
    };

    struct RendererModuleDescriptor
    {
        u32 Signature;
        s32 Unk1; // TODO
        u32 Version;
        u32 Caps;
        u32 MinimumTextureWidth;
        u32 MaximumTextureWidth;
        u32 MultipleTextureWidth;
        u32 MinimumTextureHeight;
        u32 MaximumTextureHeight;
        u32 MultipleTextureHeight;
        u32 ClipAlign;

        s32 Unk4; // TODO

        s32* TextureFormatStates; // TODO

        s32 Unk5; // TODO
        s32* Unk6; // TODO

        struct
        {
            u32 Count;
            RendererModuleDescriptorDeviceCapabilities* Capabilities;
        } Capabilities;

        u32 MaximumSimultaneousTextures;
        s32 Unk7; // TODO
        char Name[MAX_RENDERER_MODULE_DEVICE_NAME_LENGTH];
        u32 SubType;

        u32 MemorySize;
        u32 MemoryType;

        // DirectX 6.0

        const char* Author;

        u32 DXV;
    };
}