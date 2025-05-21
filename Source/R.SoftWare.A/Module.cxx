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
#include "Mathematics.Basic.hxx"
#include "Module.hxx"
#include "RendererValues.hxx"

#include <math.h>
#include <stdlib.h>

using namespace Mathematics;
using namespace Renderer;
using namespace RendererModuleValues;

namespace RendererModule
{
    // 0x60001000
    // a.k.a. THRASH_about
    DLLAPI RendererModuleDescriptor* STDCALLAPI AcquireDescriptor(void)
    {
        ModuleDescriptor.Version = RendererVersion;

        ModuleDescriptor.MaximumTextureWidth = 256;
        ModuleDescriptor.MaximumTextureHeight = 256;

        ModuleDescriptor.MinimumTextureWidth = 16;
        ModuleDescriptor.MultipleTextureWidth = 1;

        ModuleDescriptor.MinimumTextureHeight = 16;
        ModuleDescriptor.MultipleTextureHeight = 1;

        ModuleDescriptor.Caps = ModuleDescriptor.Caps
            | RENDERER_MODULE_CAPS_SOFTWARE | RENDERER_MODULE_CAPS_TEXTURE_HEIGHT_POW2
            | RENDERER_MODULE_CAPS_TEXTURE_WIDTH_POW2 | RENDERER_MODULE_CAPS_TEXTURE_SQUARE | RENDERER_MODULE_CAPS_LINE_WIDTH;

        ModuleDescriptor.Signature = RENDERER_MODULE_SIGNATURE_STRA;
        ModuleDescriptor.Size = sizeof(RendererModuleDescriptor);

        ModuleDescriptor.MemorySize = 0;
        ModuleDescriptor.MemoryType = MIN_DEVICE_AVAIABLE_VIDEO_MEMORY;

        ModuleDescriptor.ClipAlign = 0;

        ModuleDescriptor.ActiveTextureFormatStatesCount = MAX_ACTIVE_USABLE_TEXTURE_FORMAT_COUNT;
        ModuleDescriptor.TextureFormatStates = RendererTextureFormatStates;

        ModuleDescriptor.ActiveUnknownValuesCount = MAX_ACTIVE_UNKNOWN_COUNT;
        ModuleDescriptor.UnknownValues = UnknownArray06;

        strcpy(ModuleDescriptor.Name, RENDERER_MODULE_NAME);

        ModuleDescriptor.SubType = 0;

        return &ModuleDescriptor;
    }

    // 0x60001160
    // a.k.a. THRASH_clearwindow
    DLLAPI u32 STDCALLAPI ClearGameWindow(void)
    {
        return RendererClearGameWindow();
    }

    // 0x600011c0
    // a.k.a. THRASH_clip
    DLLAPI u32 STDCALLAPI ClipGameWindow(const u32 x0, const u32 y0, const u32 x1, const u32 y1)
    {
        const u32 x = (s32)x0 < 0 ? 0 : x0;
        const u32 y = (s32)y0 < 0 ? 0 : y0;

        const u32 width = State.Renderer.Settings.Width < x1 ? State.Renderer.Settings.Width : x1;
        const u32 height = State.Renderer.Settings.Height < y1 ? State.Renderer.Settings.Height : y1;

        State.ViewPort.X = x;
        State.ViewPort.Y = y;

        State.ViewPort.Left = width - x;
        State.ViewPort.Right = width - 1;
        State.ViewPort.Top = height - y;
        State.ViewPort.Bottom = height - 1;

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x600020b0
    // a.k.a. THRASH_drawline
    DLLAPI void STDCALLAPI DrawLine(RVX* a, RVX* b)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60002270
    // a.k.a. THRASH_drawlinemesh
    DLLAPI void STDCALLAPI DrawLineMesh(const u32 count, RVX* vertexes, const u32* indexes)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60002440
    // a.k.a. THRASH_drawlinestrip
    DLLAPI void STDCALLAPI DrawLineStrip(const u32 count, RVX* vertexes)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x600022b0
    // a.k.a. THRASH_drawlinestrip
    // NOTE: Never being called by the application.
    DLLAPI void STDCALLAPI DrawLineStrips(const u32 count, RVX* vertexes, const u32* indexes)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60002310
    // a.k.a. THRASH_drawpoint
    DLLAPI void STDCALLAPI DrawPoint(RVX* vertex)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60002370
    // a.k.a. THRASH_drawpointmesh
    DLLAPI void STDCALLAPI DrawPointMesh(const u32 count, RVX* vertexes, const u32* indexes)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60002470
    // a.k.a. THRASH_drawpointstrip
    DLLAPI void STDCALLAPI DrawPointStrip(const u32 count, RVX* vertexes)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60001e80
    // a.k.a. THRASH_drawquad
    DLLAPI void STDCALLAPI DrawQuad(RVX* a, RVX* b, RVX* c, RVX* d)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60001f80
    // a.k.a. THRASH_drawquadmesh
    DLLAPI void STDCALLAPI DrawQuadMesh(const u32 count, RVX* vertexes, const u32* indexes)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x600022f0
    // a.k.a. THRASH_drawsprite
    // NOTE: Never being called by the application.
    DLLAPI void STDCALLAPI DrawSprite(RVX*, RVX*) { return; }

    // 0x60002300
    // a.k.a. THRASH_drawspritemesh
    // NOTE: Never being called by the application.
    DLLAPI void STDCALLAPI DrawSpriteMesh(const u32, RVX*, const u32*) { return; }

    // 0x60001dd0
    // a.k.a. THRASH_drawtri
    DLLAPI void STDCALLAPI DrawTriangle(RVX* a, RVX* b, RVX* c)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60002400
    // a.k.a. THRASH_drawtrifan
    DLLAPI void STDCALLAPI DrawTriangleFan(const u32 count, RVX* vertexes)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60002060
    // a.k.a. THRASH_drawtrifan
    DLLAPI void STDCALLAPI DrawTriangleFans(const u32 count, RVX* vertexes, const u32* indexes)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60001e40
    // a.k.a. THRASH_drawtrimesh
    DLLAPI void STDCALLAPI DrawTriangleMesh(const u32 count, RVX* vertexes, const u32* indexes)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x600023a0
    // a.k.a. THRASH_drawtristrip
    // NOTE: Triangle strip vertex order: 0 1 2, 1 3 2, 2 3 4, 3 5 4, 4 5 6, ...
    DLLAPI void STDCALLAPI DrawTriangleStrip(const u32 count, RVX* vertexes)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60001fd0
    // a.k.a. THRASH_drawtristrip
    DLLAPI void STDCALLAPI DrawTriangleStrips(const u32 count, RVX* vertexes, const u32* indexes)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60001170
    // a.k.a. THRASH_flushwindow
    DLLAPI u32 STDCALLAPI FlushGameWindow(void)
    {
        if (State.Renderer.Surface.Mode != RENDERER_MODULE_STATE_RENDERER_MODE_0)
        {
            FUN_60004510(State.Renderer.Surface.Mode);
        
            return RENDERER_MODULE_SUCCESS;
        }

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60002490
    // a.k.a. THRASH_getstate
    DLLAPI u32 STDCALLAPI AcquireState(const u32 state)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x600011a0
    // a.k.a. THRASH_idle
    DLLAPI void STDCALLAPI Idle(void) { }

    // 0x60002c30
    // a.k.a. THRASH_init
    DLLAPI u32 STDCALLAPI Init(void)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60002fb0
    // a.k.a. THRASH_is
    DLLAPI u32 STDCALLAPI Is(void) { return RENDERER_MODULE_SW_ACCELERATION_AVAILABLE; }

    // 0x60001230
    // a.k.a. THRASH_lockwindow
    DLLAPI RendererModuleWindowLock* STDCALLAPI LockGameWindow(void)
    {
        if (State.DX.Surfaces.Window == State.DX.Surfaces.Active[2])
        {
            if (State.Renderer.Surface.Mode != RENDERER_MODULE_STATE_RENDERER_MODE_0)
            {
                FUN_60004510(RENDERER_MODULE_STATE_RENDERER_MODE_0);
            }

            State.Lock.State.Data = AcquireRendererSurface();

            State.Lock.State.Stride = RendererSurfaceStride;

            State.Lock.State.Width = State.Window.Width;
            State.Lock.State.Height = State.Window.Height;
            State.Lock.State.Format = State.DX.Surfaces.Bits == GRAPHICS_BITS_PER_PIXEL_15
                ? RENDERER_PIXEL_FORMAT_R5G5B5 : RENDERER_PIXEL_FORMAT_R5G6B5;

            return &State.Lock.State;
        }

        if (State.DX.Surfaces.Window == State.DX.Surfaces.Active[1])
        {
            RECT rect;
            GetClientRect(State.Window.HWND, &rect);

            POINT point;
            ZeroMemory(&point, sizeof(POINT));
            ClientToScreen(State.Window.HWND, &point);
            OffsetRect(&rect, point.x, point.y);

            State.Lambdas.Lambdas.LockWindow(TRUE);

            DDSURFACEDESC2 desc;
            ZeroMemory(&desc, sizeof(DDSURFACEDESC2));

            desc.dwSize = sizeof(DDSURFACEDESC2);

            State.Lock.Surface = State.DX.Surfaces.Window;

            State.DX.Code = State.DX.Surfaces.Window->Lock(NULL, &desc, DDLOCK_NOSYSLOCK | DDLOCK_WAIT, NULL);

            if (State.DX.Code != DD_OK) { return NULL; }

            State.Lock.State.Data = (void*)((addr)desc.lpSurface + (addr)(rect.left * sizeof(u16) + rect.top * desc.lPitch));

            State.Lock.State.Width = State.Window.Width;
            State.Lock.State.Height = State.Window.Height;

            State.Lock.State.Stride = desc.lPitch;

            if (desc.ddpfPixelFormat.dwRGBBitCount == GRAPHICS_BITS_PER_PIXEL_16)
            {
                State.Lock.State.Format = desc.ddpfPixelFormat.dwGBitMask == 0x7e0
                    ? RENDERER_PIXEL_FORMAT_R5G6B5 : RENDERER_PIXEL_FORMAT_R5G5B5;
            }
            else if (desc.ddpfPixelFormat.dwRGBBitCount == GRAPHICS_BITS_PER_PIXEL_32) { State.Lock.State.Format = RENDERER_PIXEL_FORMAT_R8G8B8; }

            State.Lock.IsActive = TRUE;

            return &State.Lock.State;
        }

        return NULL;
    }

    // 0x60001190
    // a.k.a. THRASH_pageflip
    DLLAPI void STDCALLAPI ToggleGameWindow(void)
    {
        Idle();

        ToggleRenderer();
    }

    // 0x60002fc0
    // a.k.a. THRASH_readrect
    DLLAPI u32 STDCALLAPI ReadRectangle(const u32 x, const u32 y, const u32 width, const u32 height, u32* pixels)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60002c60
    // a.k.a. THRASH_restore
    DLLAPI u32 STDCALLAPI RestoreGameWindow(void)
    {
        if (State.Lock.IsActive) { UnlockGameWindow(NULL); }

        RendererVideoMode = DEFAULT_RENDERER_MODE;

        return ReleaseRendererWindow() == DD_OK;
    }

    // 0x60002c90
    // a.k.a. THRASH_selectdisplay
    DLLAPI u32 STDCALLAPI SelectDevice(const s32 indx)
    {
        RendererDeviceIndex = indx;

        s32 index = DEFAULT_DEVICE_INDEX;

        DirectDrawEnumerateA(EnumerateRendererDevices, &index);

        return index == INVALID_DEVICE_INDEX;
    }

    // 0x600030e0
    // a.k.a. THRASH_setstate
    DLLAPI u32 STDCALLAPI SelectState(const u32 state, void* value)
    {
        switch (state)
        {
        case RENDERER_MODULE_STATE_NONE: { SelectRendererStateValue(state, value); return RENDERER_MODULE_SUCCESS; }
        case RENDERER_MODULE_STATE_SELECT_TEXTURE:
        {
            SelectTexture((RendererTexture*)value);

            SelectRendererStateValue(state, value);

            return RENDERER_MODULE_SUCCESS;
        }
        switch ((u32)value)
        {
        case RENDERER_MODULE_CULL_NONE:
        {
            RendererCullMode = RENDERER_CULL_MODE_NONE;

            SelectRendererStateValue(state, (void*)RENDERER_MODULE_CULL_NONE);

            return RENDERER_MODULE_SUCCESS;
        }
        case RENDERER_MODULE_CULL_COUNTER_CLOCK_WISE:
        {
            RendererCullMode = RENDERER_CULL_MODE_COUNTER_CLOCK_WISE;

            SelectRendererStateValue(state, (void*)RENDERER_MODULE_CULL_COUNTER_CLOCK_WISE);

            return RENDERER_MODULE_SUCCESS;
        }
        case RENDERER_MODULE_CULL_CLOCK_WISE:
        {
            RendererCullMode = RENDERER_CULL_MODE_CLOCK_WISE;

            SelectRendererStateValue(state, (void*)RENDERER_MODULE_CULL_CLOCK_WISE);

            return RENDERER_MODULE_SUCCESS;
        }
        default: { return RENDERER_MODULE_FAILURE; }
        }
        case RENDERER_MODULE_STATE_SELECT_MATERIAL:
        {
            SelectRendererMaterial((u32)value);

            SelectRendererStateValue(state, value);

            return RENDERER_MODULE_SUCCESS;
        }
        case RENDERER_MODULE_STATE_SELECT_DEPTH_STATE:
        case RENDERER_MODULE_STATE_SELECT_MIP_MAP_LOD_BIAS_STATE:
        case RENDERER_MODULE_STATE_SELECT_ANTI_ALIAS_STATE:
        case RENDERER_MODULE_STATE_SELECT_TEXTURE_ADDRESS_STATE:
        case RENDERER_MODULE_STATE_SELECT_FOG_ALPHAS: { break; }
        case RENDERER_MODULE_STATE_SELECT_DITHER_STATE:
        case RENDERER_MODULE_STATE_SELECT_TEXTURE_FILTER_STATE:
        case RENDERER_MODULE_STATE_SELECT_ALPHA_BLEND_STATE:
        case RENDERER_MODULE_STATE_SELECT_TEXTURE_MIP_FILTER_STATE:
        case RENDERER_MODULE_STATE_SELECT_CHROMATIC_COLOR:
        case RENDERER_MODULE_STATE_SELECT_FOG_DENSITY:
        case RENDERER_MODULE_STATE_SELECT_FOG_COLOR:
        case RENDERER_MODULE_STATE_SELECT_LINE_WIDTH:
        case RENDERER_MODULE_STATE_SELECT_FLAT_FANS_STATE:
        case RENDERER_MODULE_STATE_SELECT_FOG_STATE:
        case RENDERER_MODULE_STATE_SELECT_FOG_START:
        case RENDERER_MODULE_STATE_SELECT_FOG_END:
        case RENDERER_MODULE_STATE_SELECT_DEPTH_BIAS_STATE:
        case RENDERER_MODULE_STATE_SELECT_WINDOW:
        case RENDERER_MODULE_STATE_SELECT_LOG_LAMBDA:
        case RENDERER_MODULE_STATE_SELECT_EXECUTE_LAMBDA:
        case RENDERER_MODULE_STATE_SELECT_LOCK_WINDOW_LAMBDA: { return RENDERER_MODULE_FAILURE; }
        case RENDERER_MODULE_STATE_SELECT_SHADE_STATE:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_SHADE_FLAT:
            {
                SelectRendererShadeMode(RENDERER_MODULE_SHADE_FLAT);

                SelectRendererStateValue(state, (void*)RENDERER_MODULE_SHADE_FLAT);

                break;
            }
            case RENDERER_MODULE_SHADE_GOURAUD:
            {
                SelectRendererShadeMode(RENDERER_MODULE_SHADE_GOURAUD);

                SelectRendererStateValue(state, (void*)RENDERER_MODULE_SHADE_GOURAUD);

                break;
            }
            default: { SelectRendererStateValue(state, value); break; }
            }

            return RENDERER_MODULE_SUCCESS;
        }
        case RENDERER_MODULE_STATE_SELECT_WINDOW_MODE_STATE:
        {
            State.Settings.IsWindowMode = (BOOL)value;

            SelectRendererStateValue(state, value);

            return RENDERER_MODULE_SUCCESS;
        }
        case RENDERER_MODULE_STATE_SELECT_LAMBDAS:
        {
            CopyMemory(&State.Lambdas.Lambdas, value, sizeof(RendererModuleLambdaContainer));

            SelectRendererStateValue(state, value);

            return RENDERER_MODULE_SUCCESS;
        }
        case RENDERER_MODULE_STATE_SELECT_WINDOW_LOCK_STATE:
        {
            const RendererModuleWindowLock* lock = (RendererModuleWindowLock*)value;

            if (lock == NULL) { return RENDERER_MODULE_FAILURE; }

            State.Renderer.Surface.Surface = lock->Data;
            RendererSurfaceStride = lock->Stride;
            State.Renderer.Settings.Width = lock->Width;
            State.Renderer.Settings.Height = lock->Height;

            switch (lock->Format)
            {
            case RENDERER_PIXEL_FORMAT_R5G6B5: { State.DX.Surfaces.Bits = GRAPHICS_BITS_PER_PIXEL_16; break; }
            case RENDERER_PIXEL_FORMAT_R5G5B5:
            case RENDERER_PIXEL_FORMAT_A1R5G5B5: { State.DX.Surfaces.Bits = GRAPHICS_BITS_PER_PIXEL_15; break; }
            default: { Message("STATE_RENDERINFO -- texture type not supported.\n"); }
            }

            SelectRendererColorMasks(State.DX.Surfaces.Bits);

            if (State.Renderer.Surface.Mode != RENDERER_MODULE_STATE_RENDERER_MODE_0) { ModifyRendererSurface(); }

            SelectRendererStateValue(state, value);

            return RENDERER_MODULE_SUCCESS;
        }
        case RENDERER_MODULE_STATE_SELECT_BLEND_STATE:
        {
            FUN_600016e0(value);
            FUN_60001730(value);
            FUN_600017b0(value);
            
            Idle();

            SelectRendererStateValue(RENDERER_MODULE_STATE_SELECT_BLEND_STATE, value);

            return RENDERER_MODULE_SUCCESS;
        }
        case RENDERER_MODULE_STATE_SELECT_RENDERER_LAMBDAS:
        {
            if ((u32)value != RENDERER_MODULE_STATE_RENDERER_LAMBDAS_DEFAULT)
            {
                FUN_60015b10(value);
                SelectMainRendererLambdas();
            }
            else { SelectIndexedRendererLambdas(); }

            SelectRendererStateValue(RENDERER_MODULE_STATE_SELECT_RENDERER_LAMBDAS, value);

            return RENDERER_MODULE_SUCCESS;
        }
        case RENDERER_MODULE_STATE_702:
        {
            switch ((u32)value)
            {
            case 0:
            case 3:
            case 4:
            case 7: { Idle(); break; }
            default: { SelectRendererLambdas(); break; }
            }

            SelectRendererStateValue(RENDERER_MODULE_STATE_702, value);

            return RENDERER_MODULE_SUCCESS;
        }
        case RENDERER_MODULE_STATE_703:
        {
            DAT_6002a58c = value;

            SelectRendererStateValue(RENDERER_MODULE_STATE_703, value);

            return RENDERER_MODULE_SUCCESS;
        }
        case RENDERER_MODULE_STATE_704:
        {
            DAT_600212ec = value;

            SelectRendererStateValue(RENDERER_MODULE_STATE_704, value);

            return RENDERER_MODULE_SUCCESS;
        }
        case RENDERER_MODULE_STATE_705:
        {
            DAT_600212f0 = value;

            SelectRendererStateValue(RENDERER_MODULE_STATE_705, value);

            return RENDERER_MODULE_SUCCESS;
        }
        case RENDERER_MODULE_STATE_SELECT_RENDERER_STATE:
        {
            State.Renderer.IsActive = (u32)value == RENDERER_MODULE_RENDERER_STATE_ACTIVE;

            SelectIndexedRendererLambdas();

            SelectRendererStateValue(RENDERER_MODULE_STATE_SELECT_RENDERER_STATE, value);

            return RENDERER_MODULE_SUCCESS;
        }
        case RENDERER_MODULE_STATE_707:
        {
            FUN_60001830(value);

            SelectRendererStateValue(RENDERER_MODULE_STATE_707, value);

            return RENDERER_MODULE_SUCCESS;
        }
        case RENDERER_MODULE_STATE_SELECT_RENDERER_MODE:
        {
            State.Renderer.Surface.Mode = (u32)value;

            SelectRendererStateValue(RENDERER_MODULE_STATE_SELECT_RENDERER_MODE, value);

            return RENDERER_MODULE_SUCCESS;
        }
        case RENDERER_MODULE_STATE_SELECT_AUTO_UNLOCK_MODE:
        {
            State.Settings.IsAutoUnlock = (u32)value == RENDERER_MODULE_STATE_AUTO_UNLOCK_MODE_UNLOCK;

            SelectRendererStateValue(RENDERER_MODULE_STATE_SELECT_AUTO_UNLOCK_MODE, value);

            return RENDERER_MODULE_SUCCESS;
        }
        }

        const u32 result = SelectBasicRendererState(state, value);

        if (result != RENDERER_MODULE_FAILURE) { SelectRendererStateValue(state, value); }
        
        return result;
    }

    // 0x60003c10
    // a.k.a. THRASH_settexture
    DLLAPI u32 STDCALLAPI SelectTexture(RendererTexture* tex)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60002cc0
    // a.k.a. THRASH_setvideomode
    DLLAPI u32 STDCALLAPI SelectVideoMode(const u32 mode, const u32 pending, const u32 depth)
    {
        SelectBasicRendererState(RENDERER_MODULE_STATE_62, (void*)(DAT_60a2a5d4 + 1)); // TODO

        if (mode == RendererVideoMode) { return RENDERER_MODULE_SUCCESS; }

        if (State.DX.Instance == NULL) { InitializeRendererDeviceLambdas(); }
        else if (State.Lock.IsActive) { UnlockGameWindow(NULL); }

        if (State.DX.Instance == NULL) { return RENDERER_MODULE_FAILURE; }

        SetForegroundWindow(State.Window.HWND);
        PostMessageA(State.Window.HWND, RENDERER_MODULE_WINDOW_MESSAGE_INITIALIZE_SURFACES, mode, pending);
        WaitForSingleObject(State.Mutex, INFINITE);

        if (State.DX.Code != DD_OK) { Message("SOFTTRI_setdisplaymode - ERROR CODE (softtristatus) %8x\n", State.DX.Code); }

        SelectRendererSettings(State.Window.Width, State.Window.Height, State.Window.Bits);

        SelectGameWindow(1); // TODO

        RendererModuleWindowLock* lock = LockGameWindow();

        if (lock != NULL)
        {
            switch (lock->Format)
            {
            case RENDERER_PIXEL_FORMAT_P8: { State.DX.Surfaces.Bits = GRAPHICS_BITS_PER_PIXEL_8; break; }
            case RENDERER_PIXEL_FORMAT_R5G5B5:
            case RENDERER_PIXEL_FORMAT_A1R5G5B5: { State.DX.Surfaces.Bits = GRAPHICS_BITS_PER_PIXEL_15; break; }
            case RENDERER_PIXEL_FORMAT_R8G8B8: { State.DX.Surfaces.Bits = GRAPHICS_BITS_PER_PIXEL_24; break; }
            case RENDERER_PIXEL_FORMAT_A8R8G8B8: { State.DX.Surfaces.Bits = GRAPHICS_BITS_PER_PIXEL_32; break; }
            default: { State.DX.Surfaces.Bits = GRAPHICS_BITS_PER_PIXEL_16; break; }
            }

            UnlockGameWindow(NULL);

            State.DX.Bits = State.DX.Surfaces.Bits;

            SelectRendererColorMasks(State.DX.Surfaces.Bits);
        }

        InitializeRendererModuleState(mode, pending, depth, ENVIRONMENT_SECTION_NAME);

        return State.DX.Code == DD_OK ? RENDERER_MODULE_SUCCESS : RENDERER_MODULE_FAILURE;
    }

    // 0x600011b0
    // a.k.a. THRASH_sync
    DLLAPI u32 STDCALLAPI SyncGameWindow(const u32) { return RENDERER_MODULE_FAILURE; }

    // 0x600036c0
    // a.k.a. THRASH_talloc
    DLLAPI RendererTexture* STDCALLAPI AllocateTexture(const u32 width, const u32 height, const u32 format, const u32 options, const u32 state)
    {
        // TODO NOT IMPLEMENTED

        return NULL;
    }


    // 0x600011b0
    // a.k.a. THRASH_tfree
    DLLAPI u32 STDCALLAPI ReleaseTexture(RendererTexture* tex) { return RENDERER_MODULE_FAILURE; }

    // 0x60003f10
    // a.k.a. THRASH_treset
    DLLAPI u32 STDCALLAPI ResetTextures(void)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x600037f0
    // a.k.a. THRASH_tupdate
    DLLAPI RendererTexture* STDCALLAPI UpdateTexture(RendererTexture* tex, const u32* pixels, const u32* palette)
    {
        // TODO NOT IMPLEMENTED

        return NULL;
    }

    // 0x600013c0
    // a.k.a. THRASH_unlockwindow
    DLLAPI u32 STDCALLAPI UnlockGameWindow(const RendererModuleWindowLock* state)
    {
        if (State.DX.Surfaces.Window == State.DX.Surfaces.Active[2]) { return RENDERER_MODULE_SUCCESS; }

        if (State.DX.Surfaces.Window != State.DX.Surfaces.Active[1]) { return RENDERER_MODULE_FAILURE; }

        if (!State.Lock.IsActive || State.Lock.Surface == NULL)
        {
            Message("SOFTTRI_unlockwindow - ATTEMPTING TO UNLOCK WINDOW THAT ISN`T LOCKED.\n");

            return RENDERER_MODULE_SUCCESS;
        }

        State.Lock.Surface->Unlock(state == NULL ? NULL : (LPRECT)state->Data); // TODO ???

        State.Lock.Surface = NULL;
        State.Lock.IsActive = FALSE;

        State.Lambdas.Lambdas.LockWindow(FALSE);

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x600010d0
    // a.k.a. THRASH_window
    DLLAPI u32 STDCALLAPI SelectGameWindow(const u32 indx)
    {
        State.DX.Surfaces.Window = State.DX.Surfaces.Active[indx];

        if (State.DX.Surfaces.Window == State.DX.Surfaces.Active[2])
        {
            State.Renderer.Surface.Surface = State.Renderer.Active.Surface;

            RendererSurfaceStride = State.Renderer.Active.Stride;

            State.Renderer.Settings.Height = State.Renderer.Active.Height;
            State.Renderer.Settings.Width = State.Renderer.Active.Width;

            State.DX.Surfaces.Bits = State.DX.Bits;

            SelectRendererColorMasks(State.DX.Bits);

            if (State.Renderer.Surface.Mode != RENDERER_MODULE_STATE_RENDERER_MODE_0) { ModifyRendererSurface(); }
        }

        SelectRendererStateValue(RENDERER_MODULE_STATE_SELECT_GAME_WINDOW_INDEX, (void*)indx);

        return State.DX.Surfaces.Window != NULL;
    }

    // 0x60003050
    // a.k.a. THRASH_writerect
    DLLAPI u32 STDCALLAPI WriteRectangle(const u32 x, const u32 y, const u32 width, const u32 height, const u32* pixels)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }
}