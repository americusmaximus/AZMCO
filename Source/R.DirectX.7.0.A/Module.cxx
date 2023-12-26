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

#include "Graphics.Basic.hxx"
#include "Module.hxx"
#include "RendererValues.hxx"
#include "Settings.hxx"

#include <stdlib.h>

using namespace Renderer;
using namespace RendererModuleValues;
using namespace Settings;

namespace RendererModule
{
    // 0x60001000
    // a.k.a. THRASH_about
    DLLAPI RendererModuleDescriptor* STDCALLAPI AcquireDescriptor(void)
    {
        ModuleDescriptor.Version = RendererVersion;
        ModuleDescriptor.Signature = RENDERER_MODULE_SIGNATURE_D3D7;
        ModuleDescriptor.Unk1 = 0xd0; // TODO

        if (State.Device.Capabilities.MinTextureWidth == 0)
        {
            ModuleDescriptor.MaximumTextureWidth = 256;
            ModuleDescriptor.MaximumTextureHeight = 256;
            ModuleDescriptor.MinimumTextureWidth = 8;
            ModuleDescriptor.MinimumTextureHeight = 8;
            ModuleDescriptor.MultipleTextureWidth = 1;
            ModuleDescriptor.MultipleTextureHeight = 1;

            ModuleDescriptor.Caps = RENDERER_MODULE_CAPS_WINDOWED | ((State.DX.Active.IsSoft & 1) << 4)
                | RENDERER_MODULE_CAPS_SOFTWARE | RENDERER_MODULE_CAPS_TEXTURE_HEIGHT_POW2
                | RENDERER_MODULE_CAPS_TEXTURE_WIDTH_POW2 | RENDERER_MODULE_CAPS_TEXTURE_SQUARE | RENDERER_MODULE_CAPS_LINE_WIDTH;

            ModuleDescriptor.MaximumSimultaneousTextures = 1;
        }
        else
        {
            ModuleDescriptor.MaximumTextureWidth = State.Device.Capabilities.MaxTextureWidth;
            ModuleDescriptor.MaximumTextureHeight = State.Device.Capabilities.MaxTextureHeight;
            ModuleDescriptor.MinimumTextureWidth = State.Device.Capabilities.MinTextureWidth;
            ModuleDescriptor.MinimumTextureHeight = State.Device.Capabilities.MinTextureHeight;
            ModuleDescriptor.MultipleTextureWidth = State.Device.Capabilities.MultipleTextureWidth;
            ModuleDescriptor.MultipleTextureHeight = State.Device.Capabilities.MultipleTextureHeight;

            ModuleDescriptor.Caps = RENDERER_MODULE_CAPS_WINDOWED | ((State.DX.Active.IsSoft & 1) << 4)
                | (State.Device.Capabilities.IsPowerOfTwoTexturesHeight & 1) << 3
                | (State.Device.Capabilities.IsPowerOfTwoTexturesWidth & 1) << 2
                | (State.Device.Capabilities.IsSquareOnlyTextures & 1) << 1 | RENDERER_MODULE_CAPS_LINE_WIDTH;

            ModuleDescriptor.MaximumSimultaneousTextures = State.Device.Capabilities.MaximumSimultaneousTextures;
        }

        ModuleDescriptor.ClipAlign = 0;
        ModuleDescriptor.DXV = RENDERER_MODULE_VERSION_DX7;
        ModuleDescriptor.Author = RENDERER_MODULE_AUTHOR;
        ModuleDescriptor.Unk4 = 0x14; // TODO
        ModuleDescriptor.TextureFormatStates = RendererTextureFormatStates;
        ModuleDescriptor.Unk5 = 4; // TODO

        ModuleDescriptor.Unk6 = UnknownArray06;

        ModuleDescriptor.Capabilities.Capabilities = ModuleDescriptorDeviceCapabilities;

        strcpy(ModuleDescriptor.Name, RENDERER_MODULE_NAME);

        SelectRendererDevice();

        AcquireRendererModuleDescriptor(&ModuleDescriptor, ENVIRONMENT_SECTION_NAME);

        return &ModuleDescriptor;
    }
    
    // 0x600012e0
    // a.k.a. THRASH_clearwindow
    DLLAPI u32 STDCALLAPI ClearGameWindow()
    {
        return ClearRendererViewPort(State.ViewPort.X0, State.ViewPort.Y0,
            State.ViewPort.X1 + State.ViewPort.X0, State.ViewPort.Y0 + State.ViewPort.Y1,
            State.DX.Surfaces.Window == State.DX.Surfaces.Depth);
    }

    // 0x600013b0
    // a.k.a. THRASH_clip
    DLLAPI u32 STDCALLAPI ClipGameWindow(const u32 x0, const u32 y0, const u32 x1, const u32 y1)
    {
        State.ViewPort.X0 = x0;
        State.ViewPort.Y0 = y0;
        State.ViewPort.X1 = x1 - x0;
        State.ViewPort.Y1 = y1 - y0;

        AttemptRenderScene();

        if (State.Scene.IsActive) { EndRendererScene(); }

        D3DVIEWPORT7 vp;
        ZeroMemory(&vp, sizeof(D3DVIEWPORT7));

        vp.dwX = x0;
        vp.dwY = y0;
        vp.dvMinZ = 0.0f;
        vp.dvMaxZ = 0.9999847f;
        vp.dwWidth = x1 - x0;
        vp.dwHeight = y1 - y0;

        State.DX.Device->SetViewport(&vp);

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60001870
    // a.k.a. THRASH_createwindow
    DLLAPI u32 STDCALLAPI CreateGameWindow(const u32 width, const u32 height, const u32 format, const u32 options)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60001900
    // a.k.a. THRASH_destroywindow
    DLLAPI u32 STDCALLAPI DestroyGameWindow(const u32 indx)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60001b00
    // // a.k.a. THRASH_drawline
    DLLAPI void STDCALLAPI DrawLine(RVX* a, RVX* b)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60001b20
    // a.k.a. THRASH_drawlinemesh
    DLLAPI void STDCALLAPI DrawLineMesh(const u32 count, RVX* vertexes, const u32* indexes)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60001d00
    // a.k.a. THRASH_drawlinestrip
    // NOTE: Never being called by the application.
    DLLAPI void STDCALLAPI DrawLineStrip(const u32 count, RVX* vertexes)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60001b40
    // a.k.a. THRASH_drawlinestrip
    DLLAPI void STDCALLAPI DrawLineStrips(const u32 count, RVX* vertexes, const u32* indexes)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60001b80
    // a.k.a. THRASH_drawpoint
    DLLAPI void STDCALLAPI DrawPoint(RVX* vertex)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60001ba0
    // a.k.a. THRASH_drawpointmesh
    DLLAPI void STDCALLAPI DrawPointMesh(const u32 count, RVX* vertexes, const u32* indexes)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60001d30
    // a.k.a. THRASH_drawpointstrip
    DLLAPI void STDCALLAPI DrawPointStrip(const u32 count, RVX* vertexes)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60001a80
    // a.k.a. THRASH_drawquad
    DLLAPI void STDCALLAPI DrawQuad(RVX* a, RVX* b, RVX* c, RVX* d)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60001ae0
    // a.k.a. THRASH_drawquadmesh
    DLLAPI void STDCALLAPI DrawQuadMesh(const u32 count, RVX* vertexes, const u32* indexes)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60001bd0
    // a.k.a. THRASH_drawsprite
    DLLAPI void STDCALLAPI DrawSprite(RVX* a, RVX* b)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60001c30
    // a.k.a. THRASH_drawspritemesh
    DLLAPI void STDCALLAPI DrawSpriteMesh(const u32 count, RVX* vertexes, const u32* indexes)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x600019c0
    // a.k.a. THRASH_drawtri
    DLLAPI void STDCALLAPI DrawTriangle(RVX* a, RVX* b, RVX* c)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60001cd0
    // a.k.a. THRASH_drawtrifan
    // NOTE: Never being called by the application.
    DLLAPI void STDCALLAPI DrawTriangleFan(const u32 count, RVX* vertexes)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60001a60
    // a.k.a. THRASH_drawtrifan
    DLLAPI void STDCALLAPI DrawTriangleFans(const u32 count, RVX* vertexes, const u32* indexes)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60001a20
    // a.k.a. THRASH_drawtrimesh
    DLLAPI void STDCALLAPI DrawTriangleMesh(const u32 count, RVX* vertexes, const u32* indexes)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60001c80
    // a.k.a. THRASH_drawtristrip
    // NOTE: Triangle strip vertex order: 0 1 2, 1 3 2, 2 3 4, 3 5 4, 4 5 6, ...
    // NOTE: Never being called by the application.
    DLLAPI void STDCALLAPI DrawTriangleStrip(const u32 count, RVX* vertexes)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60001a40
    // a.k.a. THRASH_drawtristrip
    DLLAPI void STDCALLAPI DrawTriangleStrips(const u32 count, RVX* vertexes, const u32* indexes)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60001320
    // a.k.a. THRASH_flushwindow
    DLLAPI u32 STDCALLAPI FlushGameWindow(void)
    {
        return EndRendererScene();
    }

    // 0x60001dc0
    // a.k.a. THRASH_getstate
    // NOTE: Never being called by the application.
    DLLAPI u32 STDCALLAPI AcquireState(const u32 stage)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60001950
    // a.k.a. THRASH_getwindowtexture
    // NOTE: Never being called by the application.
    DLLAPI RendererTexture* STDCALLAPI AcquireGameWindowTexture(const u32 indx)
    {
        // TODO NOT IMPLEMENTED

        return NULL;
    }

    // 0x6000bb00
    // a.k.a. THRASH_idle
    DLLAPI void STDCALLAPI Idle(void) { }

    // 0x600036b0
    // a.k.a. THRASH_init
    DLLAPI u32 STDCALLAPI Init(void)
    {
        RendererState = STATE_INACTIVE;

        InitializeSettings();

        AcquireRendererDeviceCount();

        InitializeTextureStateStates();

        atexit(ReleaseRendererModule);

        return State.Devices.Count;
    }

    // 0x60003a90
    // a.k.a. THRASH_is
    DLLAPI u32 STDCALLAPI Is(void)
    {
        HWND hwnd = GetDesktopWindow();
        HDC hdc = GetWindowDC(hwnd);

        if (GetDeviceCaps(hdc, BITSPIXEL) < GRAPHICS_BITS_PER_PIXEL_8)
        {
            ReleaseDC(hwnd, hdc);

            return RENDERER_MODULE_FAILURE;
        }

        IDirectDraw7* instance = NULL;
        HRESULT result = DirectDrawCreateEx(NULL, (void**)&instance, IID_IDirectDraw7, NULL);

        if (result == DD_OK)
        {
            DDCAPS caps;
            ZeroMemory(&caps, sizeof(DDCAPS));

            caps.dwSize = sizeof(DDCAPS);

            result = instance->GetCaps(&caps, NULL);

            if ((caps.dwCaps & DDCAPS_3D) && result == DD_OK)
            {
                IDirect3D7* dx = NULL;
                result = instance->QueryInterface(IID_IDirect3D7, (void**)&dx);

                instance->Release();

                if (result == DD_OK && dx != NULL)
                {
                    dx->Release();

                    return RENDERER_MODULE_DX7_ACCELERATION_AVAILABLE;
                }

                if (dx != NULL) { dx->Release(); }
            }
        }

        if (instance != NULL) { instance->Release(); }

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60001470
    // a.k.a. THRASH_lockwindow
    DLLAPI RendererModuleWindowLock* STDCALLAPI LockGameWindow(void)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60001330
    // a.k.a. THRASH_pageflip
    DLLAPI u32 STDCALLAPI ToggleGameWindow(void)
    {
        EndRendererScene();

        if (State.Lock.IsActive) { LOGERROR("D3D pageflip called in while locked\n"); }

        return ToggleRenderer();
    }

    // 0x60001d60
    // a.k.a. THRASH_readrect
    DLLAPI u32 STDCALLAPI ReadRectangle(const u32 x, const u32 y, const u32 width, const u32 height, u32* pixels)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60003ba0
    // a.k.a. THRASH_readrect
    DLLAPI u32 STDCALLAPI ReadRectangles(const u32 x, const u32 y, const u32 width, const u32 height, u32* pixels, void*)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60003700
    // a.k.a. THRASH_restore
    DLLAPI u32 STDCALLAPI RestoreGameWindow(void)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x600020f0
    // a.k.a. THRASH_selectdisplay
    DLLAPI u32 STDCALLAPI SelectDevice(const s32 indx)
    {
        State.Device.Identifier = NULL;

        if (State.DX.Instance != NULL) { RestoreGameWindow(); }

        const char* name = NULL;

        if (indx < DEFAULT_DEVICE_INDEX || State.Devices.Count <= indx)
        {
            RendererDeviceIndex = DEFAULT_DEVICE_INDEX;
            State.Device.Identifier = State.Devices.Indexes[DEFAULT_DEVICE_INDEX];
            name = State.Devices.Enumeration.Names[DEFAULT_DEVICE_INDEX];
        }
        else
        {
            RendererDeviceIndex = indx;
            State.Device.Identifier = State.Devices.Indexes[indx];
            name = State.Devices.Enumeration.Names[indx];
        }

        strncpy(State.Device.Name, name, MAX_ENUMERATE_DEVICE_NAME_LENGTH);

        if (State.Lambdas.Lambdas.Execute != NULL)
        {
            State.Lambdas.Lambdas.Execute(RENDERER_MODULE_WINDOW_MESSAGE_INITIALIZE_DEVICE, (RENDERERMODULEEXECUTECALLBACK)InitializeRendererDeviceExecute);
            State.Lambdas.Lambdas.Execute(RENDERER_MODULE_WINDOW_MESSAGE_RELEASE_DEVICE, (RENDERERMODULEEXECUTECALLBACK)ReleaseRendererDeviceExecute);
            State.Lambdas.Lambdas.Execute(RENDERER_MODULE_WINDOW_MESSAGE_INITIALIZE_SURFACES, (RENDERERMODULEEXECUTECALLBACK)InitializeRendererDeviceSurfacesExecute);

            if (State.Lambdas.Lambdas.Execute != NULL)
            {
                if (GetWindowThreadProcessId(State.Window.Parent.HWND, NULL) != GetCurrentThreadId())
                {
                    InitializeRendererDeviceLambdas();

                    return RENDERER_MODULE_SUCCESS;
                }
            }
        }

        InitializeRendererDevice();

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60003d10
    // a.k.a. THRASH_setstate
    DLLAPI u32 STDCALLAPI SelectState(const u32 state, void* value)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60008980
    // a.k.a. THRASH_settexture
    DLLAPI u32 STDCALLAPI SelectTexture(RendererTexture* tex)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60003790
    // a.k.a. THRASH_setvideomode
    DLLAPI u32 STDCALLAPI SelectVideoMode(const u32 mode, const u32 pending, const u32 depth)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60001370
    // a.k.a. THRASH_sync
    DLLAPI u32 STDCALLAPI SyncGameWindow(const u32)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60008e70
    // a.k.a. THRASH_talloc
    DLLAPI RendererTexture* STDCALLAPI AllocateTexture(const u32 width, const u32 height, const u32 format, const u32 options, const u32 state)
    {
        // TODO NOT IMPLEMENTED

        return NULL;
    }

    // 0x60008eb0
    // a.k.a. THRASH_tfree
    DLLAPI u32 STDCALLAPI ReleaseTexture(RendererTexture* tex)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60008fc0
    // a.k.a. THRASH_treset
    DLLAPI u32 STDCALLAPI ResetTextures(void)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60008f20
    // a.k.a. THRASH_tupdate
    DLLAPI RendererTexture* STDCALLAPI UpdateTexture(RendererTexture* tex, const u32* pixels, const u32* palette)
    {
        // TODO NOT IMPLEMENTED

        return NULL;
    }

    // 0x60008f60
    // a.k.a. THRASH_tupdaterect
    DLLAPI RendererTexture* STDCALLAPI UpdateTextureRectangle(RendererTexture* tex, const u32* pixels, const u32* palette, const s32 x, const s32 y, const s32 width, const s32 height, const s32 size, const s32 level)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60001820
    // a.k.a.  _THRASH_unlockwindow
    DLLAPI u32 STDCALLAPI UnlockGameWindow(const RendererModuleWindowLock* state)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x600011a0
    // a.k.a. THRASH_window
    DLLAPI u32 STDCALLAPI SelectGameWindow(const u32 indx)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60001d90
    // a.k.a. THRASH_writerect
    DLLAPI u32 STDCALLAPI WriteRectangle(const u32 x, const u32 y, const u32 width, const u32 height, const u32* pixels)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60003c50
    // a.k.a. THRASH_writerect
    DLLAPI u32 STDCALLAPI WriteRectangles(const u32 x, const u32 y, const u32 width, const u32 height, const u32* pixels, void*)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }
}