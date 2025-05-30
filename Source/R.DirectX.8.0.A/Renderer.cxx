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

#include "Graphics.Basic.hxx"
#include "Images.hxx"
#include "Mathematics.Basic.hxx"
#include "Renderer.hxx"
#include "RendererValues.hxx"

#include <math.h>
#include <stdio.h>

#define MAX_MESSAGE_BUFFER_LENGTH 512

#define MAX_SETTINGS_BUFFER_LENGTH 80

using namespace Images;
using namespace Mathematics;
using namespace Renderer;
using namespace RendererModuleValues;

namespace RendererModule
{
    RendererModuleState State;

    u32 DAT_6001dfcc; // TODO
    u32 DAT_6001dfd0; // TODO

    // 0x60003bf0
    void ReleaseRendererModule(void)
    {
        RestoreGameWindow();
    }

    // 0x60007660
    s32 AcquireSettingsValue(const s32 value, const char* section, const char* name)
    {
        char buffer[MAX_SETTINGS_BUFFER_LENGTH];

        sprintf(buffer, "%s_%s", section, name);

        const char* tv = getenv(buffer);

        if (tv == NULL)
        {
            sprintf(buffer, "THRASH_%s", name);

            const char* ttv = getenv(buffer);

            return ttv == NULL ? value : atoi(ttv);
        }

        return atoi(tv);
    }

    // 0x60008650
    // 0x60008690
    // NOTE: Originally there are 3 different methods for error, warning, and info (which is never being called).
    void Message(const u32 severity, const char* format, ...)
    {
        char buffer[MAX_MESSAGE_BUFFER_LENGTH];

        va_list args;
        va_start(args, format);
        vsnprintf_s(buffer, MAX_MESSAGE_BUFFER_LENGTH, format, args);
        va_end(args);

        if (severity == RENDERER_MODULE_MESSAGE_SEVERITY_ERROR)
        {
            if (State.Lambdas.Log != NULL) { State.Lambdas.Log(severity, buffer); }
            else { MessageBoxA(NULL, buffer, "Abort Message", MB_SETFOREGROUND | MB_SYSTEMMODAL | MB_ICONERROR | MB_OKCANCEL); }
        }
    }

    // 0x600076d0
    void AcquireRendererModuleDescriptor(RendererModuleDescriptor* desc, const char* section)
    {
        desc->Signature = AcquireSettingsValue(desc->Signature, section, "signature");
        desc->Version = AcquireSettingsValue(desc->Version, section, "version");

        {
            const u32 caps = desc->Caps;

            const u32 lw = AcquireSettingsValue((caps & RENDERER_MODULE_CAPS_LINE_WIDTH) >> 0, section, "linewidth");
            const u32 ts = AcquireSettingsValue((caps & RENDERER_MODULE_CAPS_TEXTURE_SQUARE) >> 1, section, "texturesquare");
            const u32 twp2 = AcquireSettingsValue((caps & RENDERER_MODULE_CAPS_TEXTURE_WIDTH_POW2) >> 2, section, "texturewidthpowerof2");
            const u32 thp2 = AcquireSettingsValue((caps & RENDERER_MODULE_CAPS_TEXTURE_HEIGHT_POW2) >> 3, section, "textureheightpowerof2");
            const u32 soft = AcquireSettingsValue((caps & RENDERER_MODULE_CAPS_SOFTWARE) >> 4, section, "software");
            const u32 win = AcquireSettingsValue((caps & RENDERER_MODULE_CAPS_WINDOWED) >> 5, section, "windowed");
            const u32 gc = AcquireSettingsValue((caps & RENDERER_MODULE_CAPS_GLOBAL_CUT) >> 6, section, "globalclut");
            const u32 tn2p = AcquireSettingsValue((caps & RENDERER_MODULE_CAPS_TRILINEAR_PASS) >> 7, section, "trilinear2pass");

            desc->Caps = ((lw & 1) << 0) | ((ts & 1) << 1) | ((twp2 & 1) << 2) | ((thp2 & 1) << 3)
                | ((soft & 1) << 4) | ((win & 1) << 5) | ((gc & 1) << 6) | ((tn2p & 1) << 7);
        }

        desc->MinimumTextureWidth = AcquireSettingsValue(desc->MinimumTextureWidth, section, "texturewidthmin");
        desc->MaximumTextureWidth = AcquireSettingsValue(desc->MaximumTextureWidth, section, "texturewidthmax");
        desc->MultipleTextureWidth = AcquireSettingsValue(desc->MultipleTextureWidth, section, "texturewidthmultiple");
        desc->MinimumTextureHeight = AcquireSettingsValue(desc->MinimumTextureHeight, section, "textureheightmin");
        desc->MaximumTextureHeight = AcquireSettingsValue(desc->MaximumTextureHeight, section, "textureheightmax");
        desc->MultipleTextureHeight = AcquireSettingsValue(desc->MultipleTextureHeight, section, "textureheightmultiple");
        desc->ClipAlign = AcquireSettingsValue(desc->ClipAlign, section, "clipalign");
        desc->MaximumSimultaneousTextures = AcquireSettingsValue(desc->MaximumSimultaneousTextures, section, "numstages");
        desc->SubType = AcquireSettingsValue(desc->SubType, section, "subtype");
        desc->MemorySize = AcquireSettingsValue(desc->MemorySize, section, "textureramsize");
        desc->MemoryType = AcquireSettingsValue(desc->MemoryType, section, "textureramtype");
        desc->DXV = AcquireSettingsValue(desc->DXV, section, "dxversion");
    }

    // 0x60003c00
    u32 AcquireRendererDeviceCount(void)
    {
        State.Devices.Count = State.DX.Instance->GetAdapterCount();

        if (State.Devices.Count == 0) { return State.Devices.Count; }

        const static D3DFORMAT formats[MAX_RENDERER_DEVICE_FORMAT_COUNT] = { D3DFMT_R5G6B5, D3DFMT_X1R5G5B5 };

        u32 actual = 0;

        for (u32 x = 0; x < State.Devices.Count; x++)
        {
            BOOL found = TRUE;

            for (u32 xx = 0; xx < MAX_RENDERER_DEVICE_FORMAT_COUNT; xx++)
            {
                if (State.DX.Instance->CheckDeviceType(x, D3DDEVTYPE_HAL, formats[xx], formats[xx], FALSE) == D3D_OK) { found = TRUE; break; }
            }

            if (found)
            {
                D3DADAPTER_IDENTIFIER8 identifier;
                ZeroMemory(&identifier, sizeof(D3DADAPTER_IDENTIFIER8));

                State.DX.Instance->GetAdapterIdentifier(x, D3DENUM_NO_WHQL_LEVEL, &identifier);

                strncpy_s(State.Devices.Enumeration.Names[actual], identifier.Description, MAX_RENDERER_MODULE_DEVICE_LONG_NAME_LENGTH);

                actual = actual + 1;
            }
        }

        State.Devices.Count = actual;

        return State.Devices.Count;
    }

    // 0x60003ed0
    void AcquireRendererDeviceFormats(void)
    {
        ModuleDescriptor.Capabilities.Capabilities = ModuleDescriptorDeviceCapabilities;
        ZeroMemory(ModuleDescriptor.Capabilities.Capabilities, MAX_DEVICE_CAPABILITIES_COUNT * sizeof(RendererModuleDescriptorDeviceCapabilities));

        BOOL found = FALSE;
        u32 actual = MIN_ACTUAL_DEVICE_CAPABILITIES_INDEX;
        u32 indx = RENDERER_RESOLUTION_MODE_NONE;

        const u32 count = State.DX.Instance->GetAdapterModeCount(State.Device.Index);

        for (u32 x = 0; x < count; x++)
        {
            D3DDISPLAYMODE mode;
            State.DX.Instance->EnumAdapterModes(State.Device.Index, x, &mode);

            const u32 format = AcquireRendererDeviceFormat(mode.Format);
            const u32 bits = AcquireRendererDeviceFormatSize(mode.Format);

            if (format == RENDERER_PIXEL_FORMAT_NONE || bits == 0 || mode.Width < GRAPHICS_RESOLUTION_512) { continue; }

            if (mode.Width == GRAPHICS_RESOLUTION_640)
            {
                if (mode.Width == GRAPHICS_RESOLUTION_640)
                {
                    if (mode.Height == GRAPHICS_RESOLUTION_480)
                    {
                        if (format == RENDERER_PIXEL_FORMAT_R5G6B5)
                        {
                            indx = RENDERER_RESOLUTION_MODE_640_480_16; found = TRUE;
                        }
                        else if (format == RENDERER_PIXEL_FORMAT_A8R8G8B8)
                        {
                            indx = RENDERER_RESOLUTION_MODE_640_480_32; found = TRUE;
                        }
                    }
                }
                else if (mode.Width == GRAPHICS_RESOLUTION_800)
                {
                    if (mode.Height == GRAPHICS_RESOLUTION_600 || format == RENDERER_PIXEL_FORMAT_R5G6B5)
                    {
                        indx = RENDERER_RESOLUTION_MODE_800_600_16; found = TRUE;
                    }
                }
                else if (mode.Width == GRAPHICS_RESOLUTION_1024)
                {
                    if (mode.Height == GRAPHICS_RESOLUTION_768 || format == RENDERER_PIXEL_FORMAT_R5G6B5)
                    {
                        indx = RENDERER_RESOLUTION_MODE_1024_768_16; found = TRUE;
                    }
                }
                else if (mode.Width == GRAPHICS_RESOLUTION_1280)
                {
                    if (mode.Height == GRAPHICS_RESOLUTION_1024 || format == RENDERER_PIXEL_FORMAT_R5G6B5)
                    {
                        indx = RENDERER_RESOLUTION_MODE_1280_1024_16; found = TRUE;
                    }
                }
                else if (mode.Width == GRAPHICS_RESOLUTION_1600)
                {
                    if (mode.Height == GRAPHICS_RESOLUTION_1200 || format == RENDERER_PIXEL_FORMAT_R5G6B5)
                    {
                        indx = RENDERER_RESOLUTION_MODE_1600_1200_16; found = TRUE;
                    }
                }

                if (found)
                {
                    ModuleDescriptor.Capabilities.Capabilities[indx].Width = mode.Width;
                    ModuleDescriptor.Capabilities.Capabilities[indx].Height = mode.Height;
                    ModuleDescriptor.Capabilities.Capabilities[indx].Bits = bits;
                    ModuleDescriptor.Capabilities.Capabilities[indx].Format = format;
                    ModuleDescriptor.Capabilities.Capabilities[indx].IsActive = TRUE;
                    ModuleDescriptor.Capabilities.Capabilities[indx].Unk03 = 3;
                    ModuleDescriptor.Capabilities.Capabilities[indx].Unk04 = 2;
                }
                else if (actual < MAX_DEVICE_CAPABILITIES_COUNT)
                {
                    ModuleDescriptor.Capabilities.Capabilities[actual].Width = mode.Width;
                    ModuleDescriptor.Capabilities.Capabilities[actual].Height = mode.Height;
                    ModuleDescriptor.Capabilities.Capabilities[actual].Bits = bits;
                    ModuleDescriptor.Capabilities.Capabilities[actual].Format = format;
                    ModuleDescriptor.Capabilities.Capabilities[actual].IsActive = TRUE;
                    ModuleDescriptor.Capabilities.Capabilities[actual].Unk03 = 3;
                    ModuleDescriptor.Capabilities.Capabilities[actual].Unk04 = 2;

                    actual = actual + 1;
                }
            }
        }

        ModuleDescriptor.Capabilities.Count = actual;
    }

    // 0x600040e0
    u32 AcquireRendererDeviceFormat(const D3DFORMAT format)
    {
        switch (format)
        {
        case D3DFMT_R8G8B8: { return RENDERER_PIXEL_FORMAT_R8G8B8; }
        case D3DFMT_A8R8G8B8:
        case D3DFMT_X8R8G8B8: { return RENDERER_PIXEL_FORMAT_A8R8G8B8; }
        case D3DFMT_R5G6B5: { return RENDERER_PIXEL_FORMAT_R5G6B5; }
        case D3DFMT_X1R5G5B5:
        case D3DFMT_A1R5G5B5: { return RENDERER_PIXEL_FORMAT_R5G5B5; }
        case D3DFMT_A4R4G4B4:
        case D3DFMT_X4R4G4B4: { return RENDERER_PIXEL_FORMAT_R4G4B4; }
        }

        return RENDERER_PIXEL_FORMAT_NONE;
    }

    // 0x60003e60
    u32 AcquireRendererDeviceFormatSize(const D3DFORMAT format)
    {
        switch (format)
        {
        case D3DFMT_R8G8B8: { return GRAPHICS_BITS_PER_PIXEL_24; }
        case D3DFMT_A8R8G8B8:
        case D3DFMT_X8R8G8B8: { return GRAPHICS_BITS_PER_PIXEL_32; }
        case D3DFMT_R5G6B5:
        case D3DFMT_X1R5G5B5:
        case D3DFMT_A1R5G5B5:
        case D3DFMT_A4R4G4B4:
        case D3DFMT_A8R3G3B2:
        case D3DFMT_X4R4G4B4: { return GRAPHICS_BITS_PER_PIXEL_16; }
        case D3DFMT_R3G3B2:
        case D3DFMT_A8: { return GRAPHICS_BITS_PER_PIXEL_8; }
        }

        return 0;
    }

    // 0x600030d0
    void InitializeTextureStateStates(void)
    {
        ZeroMemory(State.Textures.StageStates, MAX_TEXTURE_STATE_STATE_COUNT * sizeof(TextureStageState));
    }

    // 0x60003500
    u32 STDCALLAPI InitializeRendererDeviceExecute(const void*, const HWND hwnd, const u32 msg, const u32 wp, const u32 lp, HRESULT* result)
    {
        u32 bits = 0;
        u32 height = 0;
        u32 width = 0;

        const RendererModuleDescriptorDeviceCapabilities* caps = &ModuleDescriptor.Capabilities.Capabilities[wp];

        if (caps->IsActive)
        {
            width = caps->Width;
            height = caps->Height;
            bits = caps->Bits;
        }
        else
        {
            width = GRAPHICS_RESOLUTION_640;
            height = GRAPHICS_RESOLUTION_480;
            bits = GRAPHICS_BITS_PER_PIXEL_16;
        }

        D3DFORMAT format = D3DFMT_UNKNOWN;
        HWND window = NULL;
        BOOL windowed = FALSE;

        if (!State.Settings.IsWindowMode) { window = State.Window.HWND; }
        else
        {
            windowed = AcquireRendererDeviceDepthWindowFormat(&width, &height, &bits, &format);

            if (windowed) { window = hwnd; }
        }

        if (!windowed)
        {
            if (!AcquireRendererDeviceDepthFormat(&bits, &format))
            {
                if (result != NULL)
                {
                    SetEvent(State.Mutexes.Device);

                    *result = RENDERER_MODULE_FAILURE;
                }

                return RENDERER_MODULE_FAILURE;
            }
        }

        ZeroMemory(&State.Device.Presentation, sizeof(D3DPRESENT_PARAMETERS));

        State.Device.Presentation.EnableAutoDepthStencil = (State.DX.Surfaces.Bits != 0);
        State.Device.Presentation.hDeviceWindow = window;
        State.Device.Presentation.SwapEffect = D3DSWAPEFFECT_FLIP;
        State.Device.Presentation.MultiSampleType = D3DMULTISAMPLE_NONE;
        State.Device.Presentation.AutoDepthStencilFormat = format;
        State.Device.Presentation.FullScreen_RefreshRateInHz = 0;
        State.Device.Presentation.FullScreen_PresentationInterval = 0;
        State.Device.Presentation.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
        State.Device.Presentation.BackBufferWidth = width;
        State.Device.Presentation.BackBufferHeight = height;

        if (bits == GRAPHICS_BITS_PER_PIXEL_16) { State.Device.Presentation.BackBufferFormat = D3DFMT_R5G6B5; }
        else if (bits == GRAPHICS_BITS_PER_PIXEL_32) { State.Device.Presentation.BackBufferFormat = D3DFMT_X8R8G8B8; }

        State.Device.Presentation.BackBufferCount = lp - 1;
        State.Device.Presentation.Windowed = windowed;

        if (!IsRendererInit)
        {
            if (State.Scene.IsActive)
            {
                ToggleGameWindow();
                SyncGameWindow(RENDERER_MODULE_SYNC_NORMAL);
            }

            ResetTextures();

            ReleaseRendererWindows();

            ReleaseRendererObjects();

            AttemptRenderPackets();
        }

        HRESULT code = D3D_OK;

        while (TRUE)
        {
            code = D3D_OK;

            State.Device.Presentation.BackBufferWidth = width;
            State.Device.Presentation.BackBufferHeight = height;

            if (!IsRendererInit)
            {
                code = State.DX.Device->Reset(&State.Device.Presentation);
            }
            else
            {
                code = State.DX.Instance->CreateDevice(State.Device.Index, D3DDEVTYPE_HAL, window,
                    D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &State.Device.Presentation, &State.DX.Device);
            }

            if (code == D3D_OK || code == D3DERR_OUTOFVIDEOMEMORY) { break; }

            switch (width)
            {
            case GRAPHICS_RESOLUTION_1600: { width = GRAPHICS_RESOLUTION_1280; height = GRAPHICS_RESOLUTION_1024; break; }
            case GRAPHICS_RESOLUTION_1280: { width = GRAPHICS_RESOLUTION_1024; height = GRAPHICS_RESOLUTION_768; break; }
            case GRAPHICS_RESOLUTION_1024: { width = GRAPHICS_RESOLUTION_800; height = GRAPHICS_RESOLUTION_600; break; }
            case GRAPHICS_RESOLUTION_800: { width = GRAPHICS_RESOLUTION_640; height = GRAPHICS_RESOLUTION_480; break; }
            default: { break; }
            }
        }

        u32 exit = RENDERER_MODULE_FAILURE;

        if (code == D3D_OK)
        {
            for (u32 x = 0; x < MAX_DEVICE_CAPABILITIES_COUNT; x++)
            {
                if (ModuleDescriptor.Capabilities.Capabilities[x].Width != 0)
                {
                    ModuleDescriptor.Capabilities.Capabilities[x].Unk03 = State.Device.Presentation.BackBufferCount + 1;
                    ModuleDescriptor.Capabilities.Capabilities[x].Unk04 = State.Device.Presentation.BackBufferCount + 1;

                    if (State.Device.Presentation.EnableAutoDepthStencil)
                    {
                        ModuleDescriptor.Capabilities.Capabilities[x].Unk03 = ModuleDescriptor.Capabilities.Capabilities[x].Unk03 + 1;
                    }
                }
            }

            {
                D3DVIEWPORT8 vp;

                vp.X = 0;
                vp.Y = 0;
                vp.Width = width;
                vp.Height = height;
                vp.MinZ = 0.0f;
                vp.MaxZ = 1.0f;

                State.DX.Device->SetViewport(&vp);
            }

            BeginRendererScene();

            State.DX.Device->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &State.DX.Surfaces.Surfaces[2]); // TODO

            State.DX.Surfaces.Surfaces[2]->Release(); // TODO

            State.DX.Surfaces.Surfaces[3] = NULL; // TODO

            State.Device.Capabilities.IsDepthAvailable = FALSE;

            if (State.Device.Presentation.EnableAutoDepthStencil)
            {
                State.Device.Capabilities.IsDepthAvailable = TRUE;

                State.DX.Device->GetDepthStencilSurface(&State.DX.Surfaces.Surfaces[3]); // TODO
                State.DX.Surfaces.Surfaces[3]->Release(); // TODO
            }

            State.DX.Surfaces.Width = width;
            State.DX.Surfaces.Height = height;

            if (IsRendererInit)
            {
                AcquireRendererDeviceCapabilities();
                AcquireRendererTextureFormats();

                State.DX.Device->GetGammaRamp(&State.DX.State.Gamma);

                InitializeViewPort();

                if ((State.Device.Presentation.EnableAutoDepthStencil != 0) && (State.DX.Surfaces.Bits != 0))
                {
                    State.Device.Capabilities.IsDepthAvailable = TRUE;

                    if (format == D3DFMT_D15S1 || format == D3DFMT_D24S8 || format == D3DFMT_D24X4S4)
                    {
                        SelectState(RENDERER_MODULE_STATE_SELECT_STENCIL_FUNCTION, (void*)RENDERER_MODULE_STENCIL_FUNCTION_ALWAYS);
                        SelectState(RENDERER_MODULE_STATE_SELECT_STENCIL_FAIL_STATE, (void*)RENDERER_MODULE_STENCIL_FAIL_ZERO);
                        SelectState(RENDERER_MODULE_STATE_SELECT_STENCIL_DEPTH_FAIL_STATE, (void*)RENDERER_MODULE_STENCIL_DEPTH_FAIL_ZERO);
                        SelectState(RENDERER_MODULE_STATE_SELECT_STENCIL_PASS_STATE, (void*)RENDERER_MODULE_STENCIL_PASS_ZERO);
                        SelectState(RENDERER_MODULE_STATE_SELECT_STENCIL_STATE, (void*)RENDERER_MODULE_STENCIL_INACTIVE);
                    }
                }

                IsRendererInit = FALSE;
            }

            AcquireRendererDeviceMemorySize();

            if (State.Data.Vertexes.Buffer == NULL) { InitializeVertexBuffer(); }

            exit = RENDERER_MODULE_SUCCESS;
        }

        if (result != NULL)
        {
            SetEvent(State.Mutexes.Device);

            *result = exit;
        }

        return exit;
    }

    // 0x60003a50
    BOOL AcquireRendererDeviceDepthFormat(u32* bits, D3DFORMAT* result)
    {
        const D3DFORMAT x16[MAX_RENDERER_DEVICE_FORMAT_COUNT] = { D3DFMT_R5G6B5, D3DFMT_R5G6B5 };
        const D3DFORMAT x32[MAX_RENDERER_DEVICE_FORMAT_COUNT] = { D3DFMT_X8R8G8B8, D3DFMT_X8R8G8B8 };

        const D3DFORMAT* formats = (*bits == GRAPHICS_BITS_PER_PIXEL_32) ? x32 : x16;

        for (u32 x = 0; x < MAX_RENDERER_DEVICE_FORMAT_COUNT; x++)
        {
            if (State.DX.Instance->CheckDeviceFormat(State.Device.Index, D3DDEVTYPE_HAL, formats[x],
                D3DUSAGE_RENDERTARGET, D3DRTYPE_SURFACE, formats[x]) == D3D_OK)
            {
                if (State.DX.Instance->CheckDeviceType(State.Device.Index, D3DDEVTYPE_HAL, formats[x], formats[x], FALSE) == D3D_OK)
                {
                    if (AcquireRendererDeviceDepthFormat(State.Device.Index, formats[x], formats[x], result))
                    {
                        if (formats[x] == D3DFMT_R5G6B5) { *bits = GRAPHICS_BITS_PER_PIXEL_16; }
                        else if (formats[x] == D3DFMT_X8R8G8B8) { *bits = GRAPHICS_BITS_PER_PIXEL_32; }

                        return TRUE;
                    }
                }
            }
        }

        return FALSE;
    }

    // 0x60003cf0
    BOOL AcquireRendererDeviceDepthFormat(const u32 device, const D3DFORMAT adapter, const D3DFORMAT target, D3DFORMAT* result)
    {
        const D3DFORMAT x16[MAX_RENDERER_DEVICE_DEPTH_FORMAT_COUNT] = { D3DFMT_D16, D3DFMT_D32, D3DFMT_D24S8, D3DFMT_D24X8, D3DFMT_D24X4S4 };
        const D3DFORMAT x32[MAX_RENDERER_DEVICE_DEPTH_FORMAT_COUNT] = { D3DFMT_D32, D3DFMT_D24S8, D3DFMT_D24X8, D3DFMT_D24X4S4, D3DFMT_D16 };

        const D3DFORMAT* formats = NULL;

        switch (State.DX.Surfaces.Bits)
        {
        case GRAPHICS_BITS_PER_PIXEL_16: { formats = x16; break; }
        case GRAPHICS_BITS_PER_PIXEL_32: { formats = x32; break; }
        default: { return TRUE; }
        }

        for (u32 x = 0; x < MAX_RENDERER_DEVICE_DEPTH_FORMAT_COUNT; x++)
        {
            if (State.DX.Instance->CheckDeviceFormat(device, D3DDEVTYPE_HAL, adapter, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, formats[x]) == D3D_OK)
            {
                if (State.DX.Instance->CheckDepthStencilMatch(device, D3DDEVTYPE_HAL, adapter, target, formats[x]) == D3D_OK)
                {
                    *result = formats[x];

                    switch (formats[x])
                    {
                    case D3DFMT_D32:
                    case D3DFMT_D24S8:
                    case D3DFMT_D24X8:
                    case D3DFMT_D24X4S4: { State.DX.Surfaces.Bits = GRAPHICS_BITS_PER_PIXEL_32; break; }
                    case D3DFMT_D15S1: { State.DX.Surfaces.Bits = GRAPHICS_BITS_PER_PIXEL_16; State.Device.Capabilities.IsStencilBufferAvailable = TRUE; break; }
                    case D3DFMT_D16: { State.DX.Surfaces.Bits = GRAPHICS_BITS_PER_PIXEL_16; break; }
                    }

                    return TRUE;
                }
            }
        }

        *result = D3DFMT_UNKNOWN;

        return FALSE;
    }

    // 0x60003920
    BOOL AcquireRendererDeviceDepthWindowFormat(u32* width, u32* height, u32* bits, D3DFORMAT* format)
    {
        u32 awidth = *width;
        u32 aheight = *height;

        HDC hdc = GetDC(NULL);
        const u32 w = GetDeviceCaps(hdc, HORZRES);
        const u32 h = GetDeviceCaps(hdc, VERTRES);
        const u32 b = GetDeviceCaps(hdc, BITSPIXEL);

        if (b != GRAPHICS_BITS_PER_PIXEL_16 && b != GRAPHICS_BITS_PER_PIXEL_32) { return FALSE; }

        if (w < GRAPHICS_RESOLUTION_640) { return FALSE; }

        if (w < awidth) { awidth = w; }
        if (h < aheight) { aheight = h; }

        D3DFORMAT adapter = D3DFMT_UNKNOWN;
        D3DFORMAT target = D3DFMT_UNKNOWN;

        if (b == GRAPHICS_BITS_PER_PIXEL_16) { adapter = D3DFMT_R5G6B5; target = D3DFMT_R5G6B5; }
        else if (b == GRAPHICS_BITS_PER_PIXEL_32) { adapter = D3DFMT_X8R8G8B8; target = D3DFMT_X8R8G8B8; }

        if (State.DX.Instance->CheckDeviceFormat(State.Device.Index, D3DDEVTYPE_HAL,
            adapter, D3DUSAGE_RENDERTARGET, D3DRTYPE_SURFACE, target) == D3D_OK)
        {
            if (State.DX.Instance->CheckDeviceType(State.Device.Index, D3DDEVTYPE_HAL, adapter, target, TRUE) != D3D_OK) { return FALSE; }
        }

        if (!AcquireRendererDeviceDepthFormat(State.Device.Index, adapter, target, format)) { return FALSE; }

        *width = awidth;
        *height = aheight;
        *bits = b;

        return TRUE;
    }

    // 0x60001870
    void ReleaseRendererWindows(void)
    {
        for (u32 x = MIN_WINDOW_INDEX; x < State.Window.Count + MIN_WINDOW_INDEX; x++)
        {
            DestroyGameWindow(x);
        }

        State.Window.Count = 0;
    }

    // 0x60003b80
    void ReleaseRendererObjects(void)
    {
        if (State.DX.Surfaces.Surfaces[3] != NULL) // TODO
        {
            State.DX.Surfaces.Surfaces[3]->Release(); // TODO
            State.DX.Surfaces.Surfaces[3] = NULL; // TODO
        }

        if (State.DX.Surfaces.Surfaces[2] != NULL) // TODO
        {
            State.DX.Surfaces.Surfaces[2]->Release(); // TODO
            State.DX.Surfaces.Surfaces[2] = NULL; // TODO
        }

        for (s32 x = (MAX_ACTIVE_SURFACE_COUNT - 1); x >= 0; x--)
        {
            if (State.DX.Surfaces.Surfaces[x] != NULL)
            {
                State.DX.Surfaces.Surfaces[x]->Release();
                State.DX.Surfaces.Surfaces[x] = NULL;
            }
        }

        if (State.Data.Vertexes.Buffer != NULL)
        {
            while (State.Data.Vertexes.Buffer->Release() != D3D_OK) {}

            State.Data.Vertexes.Buffer = NULL;
        }
    }

    // 0x600021c0
    BOOL AttemptRenderPackets(void)
    {
        if (!State.Scene.IsActive) { return FALSE; }

        if (State.DX.Device != NULL)
        {
            RenderPackets();

            State.Scene.IsActive = State.DX.Device->EndScene() != D3D_OK;
        }

        return State.Scene.IsActive;
    }

    // 0x60001e90
    void RenderPackets(void)
    {
        BeginRendererScene();

        State.DX.Device->SetVertexShader(RendererCurrentShader);
        State.DX.Device->SetStreamSource(0, State.Data.Vertexes.Buffer, RendererVertexSize);

        for (u32 x = 0; x < State.Data.Packets.Count; x++)
        {
            const RendererPacket* packet = &State.Data.Packets.Packets[x + 1];

            State.DX.Device->DrawPrimitive(packet->Type, State.Data.Vertexes.StartIndex, packet->Count);

            State.Data.Vertexes.StartIndex = State.Data.Vertexes.StartIndex + packet->Size;
        }

        State.Data.Packets.Count = 0;
    }

    // 0x60002170
    BOOL BeginRendererScene(void)
    {
        if (State.Scene.IsActive) { return TRUE; }

        if (State.DX.Device != NULL)
        {
            if (State.Lock.IsActive) { UnlockGameWindow(NULL); }

            State.Scene.IsActive = State.DX.Device->BeginScene() == D3D_OK;
        }

        return State.Scene.IsActive;
    }

    // 0x60007900
    BOOL AcquireRendererDeviceCapabilities(void)
    {
        D3DCAPS8 caps;
        ZeroMemory(&caps, sizeof(D3DCAPS8));

        ZeroMemory(&State.Device.Capabilities, sizeof(RendererModuleDeviceCapabilities8));

        if (State.DX.Device->GetDeviceCaps(&caps) != D3D_OK) { return FALSE; }

        if (caps.DevCaps & D3DDEVCAPS_HWRASTERIZATION) { State.Device.Capabilities.IsAccelerated = TRUE; }

        State.Data.MaxPrimitiveCount = caps.MaxPrimitiveCount;

        MaxRendererSimultaneousTextures = MIN_SIMULTANEOUS_TEXTURE_COUNT;

        if ((caps.DevCaps & D3DDEVCAPS_SEPARATETEXTUREMEMORIES) && MIN_SIMULTANEOUS_TEXTURE_COUNT < caps.MaxSimultaneousTextures)
        {
            MaxRendererSimultaneousTextures = caps.MaxSimultaneousTextures;
        }

        State.Device.Capabilities.MaximumSimultaneousTextures = caps.MaxSimultaneousTextures;
        State.Device.Capabilities.MaxTextureRepeat = (f32)caps.MaxTextureRepeat;
        State.Device.Capabilities.MinTextureWidth = 1;
        State.Device.Capabilities.MinTextureHeight = 1;
        State.Device.Capabilities.MaxTextureWidth = caps.MaxTextureWidth;
        State.Device.Capabilities.MaxTextureHeight = caps.MaxTextureHeight;

        if (caps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY) { State.Device.Capabilities.IsSquareOnlyTextures = TRUE; }

        State.Device.Capabilities.MultipleTextureWidth = 1;
        State.Device.Capabilities.MultipleTextureHeight = 1;

        if (caps.TextureCaps & D3DPTEXTURECAPS_POW2)
        {
            State.Device.Capabilities.IsPowerOfTwoTexturesWidth = TRUE;
            State.Device.Capabilities.IsPowerOfTwoTexturesHeight = TRUE;
        }

        if (caps.TextureAddressCaps & D3DPTADDRESSCAPS_INDEPENDENTUV) { State.Device.Capabilities.IsTextureIndependentUVs = TRUE; }

        State.Device.Capabilities.IsGreenAllowSixBits = TRUE;

        if (caps.RasterCaps & D3DPRASTERCAPS_ANTIALIASEDGES) { State.Device.Capabilities.IsAntiAliasEdges = TRUE; }

        if (caps.RasterCaps & D3DPRASTERCAPS_ANISOTROPY)
        {
            State.Device.Capabilities.IsAnisotropyAvailable = TRUE;
            State.Device.Capabilities.MaxAnisotropy = caps.MaxAnisotropy;
        }

        if (caps.RasterCaps & D3DPRASTERCAPS_MIPMAPLODBIAS) { State.Device.Capabilities.IsMipMapBiasAvailable = TRUE; }

        if (caps.RasterCaps & D3DPRASTERCAPS_WBUFFER) { State.Device.Capabilities.IsWBufferAvailable = TRUE; }

        if (caps.RasterCaps & D3DPRASTERCAPS_WFOG) { State.Device.Capabilities.IsWFogAvailable = TRUE; }
        if (caps.RasterCaps & D3DPRASTERCAPS_DITHER) { State.Device.Capabilities.IsDitherAvailable = TRUE; }

        State.Device.Capabilities.IsPerspectiveTextures = (caps.TextureCaps & D3DPTEXTURECAPS_PERSPECTIVE) != 0;
        State.Device.Capabilities.IsAlphaTextures = (caps.TextureCaps & D3DPTEXTURECAPS_ALPHA) != 0;
        State.Device.Capabilities.IsAlphaBlending = (caps.ShadeCaps & D3DPSHADECAPS_ALPHAGOURAUDBLEND) != 0;
        State.Device.Capabilities.IsSpecularBlending = (caps.ShadeCaps & D3DPSHADECAPS_SPECULARGOURAUDRGB) != 0;

        if (State.Device.Capabilities.IsSpecularBlending) { State.Device.Capabilities.IsSpecularGouraudBlending = 1; }

        State.Device.Capabilities.IsModulateBlending = TRUE;

        if ((caps.SrcBlendCaps & D3DPBLENDCAPS_SRCALPHA) == 0)
        {
            State.Device.Capabilities.IsSourceAlphaBlending = TRUE;

            if ((caps.DestBlendCaps & D3DPBLENDCAPS_ONE) == 0) { State.Device.Capabilities.IsSourceAlphaBlending = FALSE; }
        }

        State.Device.Capabilities.IsColorBlending = (caps.ShadeCaps & D3DPSHADECAPS_COLORGOURAUDRGB) != 0;

        if (!State.Device.Capabilities.IsColorBlending) { State.Device.Capabilities.IsSourceAlphaBlending = FALSE; }

        if (isnan(caps.GuardBandLeft) == (caps.GuardBandLeft == 0.0f))
        {
            State.Device.Capabilities.GuardBandLeft = caps.GuardBandLeft;
            State.Device.Capabilities.GuardBandRight = caps.GuardBandRight;
            State.Device.Capabilities.GuardBandTop = caps.GuardBandTop;
            State.Device.Capabilities.GuardBandBottom = caps.GuardBandBottom;
        }

        State.Device.Capabilities.Unk29 = 0;
        State.Device.Capabilities.Unk32 = 0;
        State.Device.Capabilities.IsAlphaProperBlending = State.Device.Capabilities.IsAlphaBlending;

        State.Device.SubType = 0; // TODO

        if (caps.Caps2 & D3DCAPS2_CANRENDERWINDOWED) { State.Device.Capabilities.IsWindowModeAvailable = TRUE; }
        if (caps.Caps2 & D3DCAPS2_FULLSCREENGAMMA) { State.Device.Capabilities.IsGammaAvailable = TRUE; }

        if (caps.TextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR) { State.Device.Capabilities.IsInterpolationAvailable = TRUE; }

        if (caps.MaxSimultaneousTextures != 0)
        {
            const BOOL op1 = (caps.TextureOpCaps & (D3DTEXOPCAPS_MODULATE2X | D3DTEXOPCAPS_SELECTARG2 | D3DTEXOPCAPS_SELECTARG1)) != 0;
            const BOOL op2 = (caps.TextureOpCaps & (D3DTEXOPCAPS_MODULATE2X | D3DTEXOPCAPS_SELECTARG2 | D3DTEXOPCAPS_SELECTARG1 | D3DTEXOPCAPS_DISABLE)) != 0;
            const BOOL op3 = (caps.TextureOpCaps & (D3DTEXOPCAPS_MODULATE2X | D3DTEXOPCAPS_MODULATE)) != 0;

            for (u32 x = 0; x < State.Device.Capabilities.MaximumSimultaneousTextures; x++)
            {
                State.Textures.Stages[x].Unk11 = op1; // TODO
                State.Textures.Stages[x].Unk12 = op2; // TODO
                State.Textures.Stages[x].Unk10 = op3; // TODO
            }
        }

        return TRUE;
    }

    // 0x60004730
    void AcquireRendererTextureFormats(void)
    {
        State.Textures.Formats[0] = D3DFMT_UNKNOWN;
        State.Textures.Formats[1] = D3DFMT_UNKNOWN;

        State.Textures.Formats[2] = State.DX.Instance->CheckDeviceFormat(0, D3DDEVTYPE_HAL, D3DFMT_R5G6B5, D3DUSAGE_NONE, D3DRTYPE_TEXTURE, D3DFMT_P8) == D3D_OK ? D3DFMT_P8 : D3DFMT_UNKNOWN;
        State.Textures.Formats[3] = State.DX.Instance->CheckDeviceFormat(0, D3DDEVTYPE_HAL, D3DFMT_R5G6B5, D3DUSAGE_NONE, D3DRTYPE_TEXTURE, D3DFMT_A1R5G5B5) == D3D_OK ? D3DFMT_A1R5G5B5 : D3DFMT_UNKNOWN;
        State.Textures.Formats[4] = State.DX.Instance->CheckDeviceFormat(0, D3DDEVTYPE_HAL, D3DFMT_R5G6B5, D3DUSAGE_NONE, D3DRTYPE_TEXTURE, D3DFMT_R5G6B5) == D3D_OK ? D3DFMT_R5G6B5 : D3DFMT_UNKNOWN;
        State.Textures.Formats[5] = State.DX.Instance->CheckDeviceFormat(0, D3DDEVTYPE_HAL, D3DFMT_R5G6B5, D3DUSAGE_NONE, D3DRTYPE_TEXTURE, D3DFMT_R8G8B8) == D3D_OK ? D3DFMT_R8G8B8 : D3DFMT_UNKNOWN;
        State.Textures.Formats[6] = State.DX.Instance->CheckDeviceFormat(0, D3DDEVTYPE_HAL, D3DFMT_R5G6B5, D3DUSAGE_NONE, D3DRTYPE_TEXTURE, D3DFMT_A8R8G8B8) == D3D_OK ? D3DFMT_A8R8G8B8 : D3DFMT_UNKNOWN;
        State.Textures.Formats[7] = State.DX.Instance->CheckDeviceFormat(0, D3DDEVTYPE_HAL, D3DFMT_R5G6B5, D3DUSAGE_NONE, D3DRTYPE_TEXTURE, D3DFMT_A4R4G4B4) == D3D_OK ? D3DFMT_A4R4G4B4 : D3DFMT_UNKNOWN;

        State.Textures.Formats[12] = State.DX.Instance->CheckDeviceFormat(0, D3DDEVTYPE_HAL, D3DFMT_R5G6B5, D3DUSAGE_NONE, D3DRTYPE_TEXTURE, D3DFMT_DXT1) == D3D_OK ? D3DFMT_DXT1 : D3DFMT_UNKNOWN;
        State.Textures.Formats[13] = State.DX.Instance->CheckDeviceFormat(0, D3DDEVTYPE_HAL, D3DFMT_R5G6B5, D3DUSAGE_NONE, D3DRTYPE_TEXTURE, D3DFMT_DXT3) == D3D_OK ? D3DFMT_DXT3 : D3DFMT_UNKNOWN;

        State.Textures.Formats[16] = State.DX.Instance->CheckDeviceFormat(0, D3DDEVTYPE_HAL, D3DFMT_R5G6B5, D3DUSAGE_NONE, D3DRTYPE_TEXTURE, D3DFMT_V8U8) == D3D_OK ? D3DFMT_V8U8 : D3DFMT_UNKNOWN;
        State.Textures.Formats[17] = State.DX.Instance->CheckDeviceFormat(0, D3DDEVTYPE_HAL, D3DFMT_R5G6B5, D3DUSAGE_NONE, D3DRTYPE_TEXTURE, D3DFMT_X8L8V8U8) == D3D_OK ? D3DFMT_X8L8V8U8 : D3DFMT_UNKNOWN;
        State.Textures.Formats[18] = State.DX.Instance->CheckDeviceFormat(0, D3DDEVTYPE_HAL, D3DFMT_R5G6B5, D3DUSAGE_NONE, D3DRTYPE_TEXTURE, D3DFMT_L6V5U5) == D3D_OK ? D3DFMT_L6V5U5 : D3DFMT_UNKNOWN;
        State.Textures.Formats[19] = D3DFMT_UNKNOWN;
        State.Textures.Formats[20] = D3DFMT_UNKNOWN;
        State.Textures.Formats[21] = D3DFMT_UNKNOWN;
        State.Textures.Formats[22] = D3DFMT_UNKNOWN;

        State.Textures.Formats[27] = D3DFMT_UNKNOWN;
        State.Textures.Formats[28] = D3DFMT_UNKNOWN;
        State.Textures.Formats[29] = D3DFMT_UNKNOWN;
        State.Textures.Formats[30] = D3DFMT_UNKNOWN;
        State.Textures.Formats[31] = D3DFMT_UNKNOWN;
        State.Textures.Formats[32] = D3DFMT_UNKNOWN;
        State.Textures.Formats[33] = D3DFMT_UNKNOWN;

        RendererTextureFormatStates[0] = 0;
        RendererTextureFormatStates[1] = State.Textures.Formats[1] != D3DFMT_UNKNOWN;
        RendererTextureFormatStates[2] = State.Textures.Formats[2] != D3DFMT_UNKNOWN;
        RendererTextureFormatStates[3] = State.Textures.Formats[3] != D3DFMT_UNKNOWN;
        RendererTextureFormatStates[4] = State.Textures.Formats[4] != D3DFMT_UNKNOWN;
        RendererTextureFormatStates[5] = State.Textures.Formats[5] != D3DFMT_UNKNOWN;
        RendererTextureFormatStates[6] = State.Textures.Formats[6] != D3DFMT_UNKNOWN;
        RendererTextureFormatStates[7] = State.Textures.Formats[7] != D3DFMT_UNKNOWN;

        RendererTextureFormatStates[12] = State.Textures.Formats[12] != D3DFMT_UNKNOWN;
        RendererTextureFormatStates[13] = State.Textures.Formats[13] != D3DFMT_UNKNOWN;

        RendererTextureFormatStates[16] = State.Textures.Formats[16] != D3DFMT_UNKNOWN;
        RendererTextureFormatStates[17] = State.Textures.Formats[17] != D3DFMT_UNKNOWN;
        RendererTextureFormatStates[18] = State.Textures.Formats[18] != D3DFMT_UNKNOWN;
        RendererTextureFormatStates[19] = State.Textures.Formats[19] != D3DFMT_UNKNOWN;
        RendererTextureFormatStates[20] = State.Textures.Formats[20] != D3DFMT_UNKNOWN;
        RendererTextureFormatStates[21] = State.Textures.Formats[21] != D3DFMT_UNKNOWN;
        RendererTextureFormatStates[22] = State.Textures.Formats[22] != D3DFMT_UNKNOWN;
        RendererTextureFormatStates[27] = 0;
        RendererTextureFormatStates[28] = 0;
        RendererTextureFormatStates[29] = 0;
        RendererTextureFormatStates[30] = 0;
        RendererTextureFormatStates[31] = 0;
        RendererTextureFormatStates[32] = 0;
        RendererTextureFormatStates[33] = 0;
    }

    // 0x60001570
    void InitializeViewPort(void)
    {
        State.ViewPort.X0 = 0;
        State.ViewPort.Y0 = 0;
        State.ViewPort.X1 = 0;
        State.ViewPort.Y1 = 0;
    }

    // 0x60003b40
    void AcquireRendererDeviceMemorySize(void)
    {
        ModuleDescriptor.MemorySize = State.DX.Device->GetAvailableTextureMem();

        ModuleDescriptor.MemoryType = RENDERER_MODULE_TEXTURE_LOCATION_SYSTEM_MEMORY;

        if (MIN_SIMULTANEOUS_TEXTURE_COUNT < MaxRendererSimultaneousTextures)
        {
            ModuleDescriptor.MemorySize = ModuleDescriptor.MemorySize / MaxRendererSimultaneousTextures;
        }
    }

    // 0x60001d40
    void InitializeVertexBuffer(void)
    {
        if (State.DX.Device->CreateVertexBuffer(MAX_VERTEX_BUFFER_SIZE, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
            D3DFVF_NONE, D3DPOOL_DEFAULT, &State.Data.Vertexes.Buffer) == D3D_OK)
        {
            ZeroMemory(&State.Data.Packets.Packets[1], MAX_RENDER_PACKET_COUNT * sizeof(RendererPacket));

            State.Data.Packets.Count = 0;

            State.Data.Vertexes.Count = 0;
            State.Data.Vertexes.StartIndex = 0;
        }
    }

    // 0x60003430
    u32 STDCALLAPI InitializeRendererDeviceSurfacesExecute(const void*, const HWND hwnd, const u32 msg, const u32 wp, const u32 lp, HRESULT* result)
    {
        if (State.Lock.IsActive) { UnlockGameWindow(NULL); }

        AttemptRenderPackets();

        ReleaseRendererWindows();

        ResetTextures();

        if (State.DX.Device != NULL)
        {
            ReleaseRendererObjects();

            while (State.DX.Device->Release() != D3D_OK) {}

            State.DX.Device = NULL;
        }

        if (State.DX.Instance != NULL)
        {
            while (State.DX.Instance->Release() != D3D_OK) {}

            State.DX.Instance = NULL;
        }

        State.Scene.IsActive = FALSE;

        State.DX.Surfaces.Surfaces[2] = NULL; // TODO
        State.DX.Surfaces.Surfaces[3] = NULL; // TODO

        State.Device.Index = DEFAULT_DEVICE_INDEX;

        State.Lambdas.Log = NULL;

        IsRendererInit = FALSE;

        State.Window.HWND = NULL;

        if (result != NULL)
        {
            SetEvent(State.Mutexes.Surface);

            *result = RENDERER_MODULE_SUCCESS;
        }

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60008290
    void InitializeRendererModuleState(const u32 mode, const u32 pending, const u32 depth, const char* section)
    {
        SelectState(RENDERER_MODULE_STATE_SELECT_HINT_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_HINT_INACTIVE, section, "HINT"));
        SelectState(RENDERER_MODULE_STATE_SELECT_TEXTURE, NULL);
        SelectState(RENDERER_MODULE_STATE_SELECT_CULL_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_CULL_COUNTER_CLOCK_WISE, section, "CULL"));
        SelectState(RENDERER_MODULE_STATE_SELECT_TEXTURE_FILTER_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_TEXTURE_FILTER_LENEAR, section, "FILTER"));
        SelectState(RENDERER_MODULE_STATE_SELECT_SHADE_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_SHADE_GOURAUD, section, "SHADE"));
        SelectState(RENDERER_MODULE_STATE_SELECT_ALPHA_BLEND_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_ALPHA_BLEND_ACTIVE, section, "TRANSPARENCY"));
        SelectState(RENDERER_MODULE_STATE_SELECT_ALPHA_TEST_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_ALPHA_TEST_16, section, "ALPHATEST"));
        SelectState(RENDERER_MODULE_STATE_SELCT_ALPHA_FUNCTION,
            (void*)AcquireSettingsValue(RENDERER_MODULE_ALPHA_FUNCTION_GREATER, section, "ALPHACMP"));
        SelectState(RENDERER_MODULE_STATE_SELECT_TEXTURE_MIP_FILTER_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_TEXTURE_MIP_FILTER_POINT, section, "MIPMAP"));
        SelectState(RENDERER_MODULE_STATE_SELECT_MATERIAL,
            (void*)AcquireSettingsValue(0x00000000, section, "BACKGROUNDCOLOUR"));
        SelectState(RENDERER_MODULE_STATE_SELECT_CHROMATIC_COLOR,
            (void*)AcquireSettingsValue(0x00000000, section, "CHROMACOLOUR"));
        SelectState(RENDERER_MODULE_STATE_SELECT_DITHER_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_DITHER_INACTIVE, section, "DITHER")); // ORIGINAL: ACTIVE
        SelectState(RENDERER_MODULE_STATE_SELECT_FOG_ALPHAS, NULL);
        SelectState(RENDERER_MODULE_STATE_SELECT_FOG_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_FOG_INACTIVE, section, "FOGMODE"));

        {
            const f32 value = 0.0f;
            SelectState(RENDERER_MODULE_STATE_SELECT_FOG_DENSITY,
                (void*)AcquireSettingsValue((s32)(*(s32*)&value), section, "FOGDENSITY"));
        }

        {
            const f32 value = 0.0f;
            SelectState(RENDERER_MODULE_STATE_SELECT_FOG_START,
                (void*)AcquireSettingsValue((s32)(*(s32*)&value), section, "STATE_FOGZNEAR"));
        }

        {
            const f32 value = 1.0f;
            SelectState(RENDERER_MODULE_STATE_SELECT_FOG_END,
                (void*)AcquireSettingsValue((s32)(*(s32*)&value), section, "FOGZFAR"));
        }

        SelectState(RENDERER_MODULE_STATE_SELECT_FOG_COLOR,
            (void*)AcquireSettingsValue(GRAPCHICS_COLOR_WHITE, section, "FOGCOLOUR"));

        SelectState(RENDERER_MODULE_STATE_INDEX_SIZE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_INDEX_SIZE_4, section, "INDEXSIZE"));

        SelectState(RENDERER_MODULE_STATE_SELECT_BLEND_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_ALPHA_BLEND_SOURCE_INVERSE_SOURCE, section, "ALPHA"));
        SelectState(RENDERER_MODULE_STATE_SELECT_TEXTURE_ADDRESS_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_TEXTURE_ADDRESS_CLAMP, section, "TEXTURECLAMP"));

        {
            const f32 value = 1.0f;
            SelectState(RENDERER_MODULE_STATE_SELECT_GAMMA_CONTROL_STATE, (void*)(u32)(*(u32*)&value));
        }

        {
            const LONG value = 0;

            // NOTE: Change of type from f32 to a signed integer.

            SelectState(RENDERER_MODULE_STATE_SELECT_DEPTH_BIAS_STATE,
                (void*)AcquireSettingsValue(value, section, "DEPTHBIAS"));
        }

        {
            const f32 value = 0.0f;
            SelectState(RENDERER_MODULE_STATE_SELECT_MIP_MAP_LOD_BIAS_STATE, (void*)(u32)(*(u32*)&value));
        }

        SelectState(RENDERER_MODULE_STATE_SELECT_VERTEX_TYPE, (void*)RENDERER_MODULE_VERTEX_TYPE_RTLVX);
        SelectState(RENDERER_MODULE_STATE_SELECT_TEXTURE_STAGE_BLEND_STATE, (void*)RENDERER_MODULE_TEXTURE_STAGE_BLEND_NORMAL);
        SelectState(MAKETEXTURESTAGEMASK(RENDERER_TEXTURE_STAGE_1) | RENDERER_MODULE_STATE_SELECT_TEXTURE_STAGE_BLEND_STATE, (void*)RENDERER_MODULE_TEXTURE_STAGE_BLEND_DISABLE);
        SelectState(RENDERER_MODULE_STATE_SELECT_BACK_BUFFER_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_BACK_BUFFER_ACTIVE, section, "BACKBUFFERTYPE"));
        SelectState(RENDERER_MODULE_STATE_SELECT_FLAT_FANS_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_STATE_FLAT_FANS_ACTIVE, section, "FLATFANS"));
        SelectState(RENDERER_MODULE_STATE_SELECT_LINE_WIDTH, (void*)AcquireSettingsValue(1, section, "LINEWIDTH"));
        SelectState(RENDERER_MODULE_STATE_SELECT_STENCIL_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_STENCIL_INACTIVE, section, "STENCILBUFFER"));
        SelectState(RENDERER_MODULE_STATE_SELECT_DISPLAY_STATE, (void*)AcquireSettingsValue(mode, section, "DISPLAYMODE"));
        SelectState(RENDERER_MODULE_STATE_SELECT_LINE_DOUBLE_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_LINE_DOUBLE_INACTIVE, section, "LINEDOUBLE"));
        SelectState(RENDERER_MODULE_STATE_SELECT_GAME_WINDOW_INDEX, (void*)0); // TODO

        {
            const f32 value = 1.0f;
            SelectState(RENDERER_MODULE_STATE_SELECT_CLEAR_DEPTH_STATE, (void*)(u32)(*(u32*)&value));
        }

        SelectState(RENDERER_MODULE_STATE_SELECT_TOGGLE_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_TOGGLE_VERTICAL_SYNC, section, "FLIPRATE"));
        SelectState(RENDERER_MODULE_STATE_SELECT_SHAMELESS_PLUG_STATE,
            (void*)AcquireSettingsValue(0, section, "SHAMELESSPLUG")); // TODO

        {
            const u32 value = AcquireSettingsValue(depth < 0, section, "DEPTHBUFFER");
            const u32 result = SelectState(RENDERER_MODULE_STATE_SELECT_DEPTH_STATE, (void*)value);

            if (result == RENDERER_MODULE_FAILURE && value == RENDERER_MODULE_DEPTH_ACTIVE)
            {
                SelectState(RENDERER_MODULE_STATE_SELECT_DEPTH_STATE, (void*)RENDERER_MODULE_DEPTH_ACTIVE_W);
            }
        }

        SelectState(RENDERER_MODULE_STATE_SELCT_DEPTH_FUNCTION,
            (void*)AcquireSettingsValue(RENDERER_MODULE_DEPTH_FUNCTION_LESS_EQUAL, section, "DEPTHCMP"));
        SelectState(RENDERER_MODULE_STATE_SELECT_LOG_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_LOG_ACTIVE, section, "LOG"));
    }

    // 0x60007fe0
    u32 SelectBasicRendererState(const u32 state, void* value)
    {
        switch (state)
        {
        case RENDERER_MODULE_STATE_SELECT_CULL_STATE:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_CULL_NONE:
            {
                State.Settings.Cull = RENDERER_CULL_MODE_NONE;

                SelectRendererStateValue(state, (void*)RENDERER_MODULE_CULL_NONE);

                return RENDERER_MODULE_SUCCESS;
            }
            case RENDERER_MODULE_CULL_COUNTER_CLOCK_WISE:
            {
                State.Settings.Cull = RENDERER_CULL_MODE_COUNTER_CLOCK_WISE;

                SelectRendererStateValue(state, (void*)RENDERER_MODULE_CULL_COUNTER_CLOCK_WISE);

                return RENDERER_MODULE_FAILURE;
            }
            case RENDERER_MODULE_CULL_CLOCK_WISE:
            {
                State.Settings.Cull = RENDERER_CULL_MODE_CLOCK_WISE;

                SelectRendererStateValue(state, (void*)RENDERER_MODULE_CULL_CLOCK_WISE);

                return RENDERER_MODULE_FAILURE;
            }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            break;
        }
        case RENDERER_MODULE_STATE_SELECT_WINDOW_MODE_STATE:
        case RENDERER_MODULE_STATE_SELECT_EXECUTE_LAMBDA:
        case RENDERER_MODULE_STATE_SELECT_LOCK_WINDOW_LAMBDA:
        case RENDERER_MODULE_STATE_SELECT_LINE_VERTEX_SIZE:
        case RENDERER_MODULE_STATE_SELECT_DISPLAY_STATE:
        case RENDERER_MODULE_STATE_SELECT_GAME_WINDOW_INDEX:
        case RENDERER_MODULE_STATE_SELECT_SHAMELESS_PLUG_STATE:
        case RENDERER_MODULE_STATE_SELECT_LOG_STATE: { break; }
        case RENDERER_MODULE_STATE_SELECT_LAMBDAS:
        {
            const RendererModuleLambdaContainer* lambdas = (RendererModuleLambdaContainer*)value;

            State.Lambdas.Log = lambdas == NULL ? NULL : lambdas->Log;

            break;
        }
        case RENDERER_MODULE_STATE_SELECT_WINDOW: { State.Window.Parent.HWND = (HWND)value; break; }
        case RENDERER_MODULE_STATE_SELECT_LOG_LAMBDA: { State.Lambdas.Log = (RENDERERMODULELOGLAMBDA)value; break; }
        case RENDERER_MODULE_STATE_SELECT_VERSION: { RendererVersion = (u32)value; break; }
        case RENDERER_MODULE_STATE_SELECT_MEMORY_ALLOCATE_LAMBDA: { State.Lambdas.AllocateMemory = (RENDERERMODULEALLOCATEMEMORYLAMBDA)value; break; }
        case RENDERER_MODULE_STATE_SELECT_MEMORY_RELEASE_LAMBDA: { State.Lambdas.ReleaseMemory = (RENDERERMODULERELEASEMEMORYLAMBDA)value; break; }
        case RENDERER_MODULE_STATE_SELECT_SELECT_STATE_LAMBDA: { State.Lambdas.SelectState = (RENDERERMODULESELECTSTATELAMBDA)value; break; }
        case RENDERER_MODULE_STATE_55: { DAT_6001dfcc = (u32)value; break; }
        case RENDERER_MODULE_STATE_62: { DAT_6001dfd0 = (u32)value; break; }
        case RENDERER_MODULE_STATE_INDEX_SIZE:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_INDEX_SIZE_1: { RendererIndexSize = RENDERER_MODULE_INDEX_SIZE_1; break; }
            case RENDERER_MODULE_INDEX_SIZE_2: { RendererIndexSize = RENDERER_MODULE_INDEX_SIZE_2; break; }
            case RENDERER_MODULE_INDEX_SIZE_4: { RendererIndexSize = RENDERER_MODULE_INDEX_SIZE_4; break; }
            case RENDERER_MODULE_INDEX_SIZE_8: { RendererIndexSize = RENDERER_MODULE_INDEX_SIZE_8; break; }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            break;
        }
        default: { return RENDERER_MODULE_FAILURE; }
        }

        SelectRendererStateValue(state, value);

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60003020
    void SelectRendererStateValue(const u32 state, void* value)
    {
        const u32 actual = state & RENDERER_MODULE_SELECT_STATE_MASK;
        const u32 stage = MAKETEXTURESTAGEVALUE(state);

        const s32 indx = AcquireTextureStateStageIndex(actual);

        if (indx >= 0) { State.Textures.StageStates[indx].Values[stage] = (s32)value; }

        if (State.Lambdas.SelectState != NULL) { State.Lambdas.SelectState(actual, value); }
    }

    // 0x60002fd0
    s32 AcquireTextureStateStageIndex(const u32 state)
    {
        s32 sum = 0;
        u32 indx = 0;

        while (state < RendererModuleValues::MinMax[indx].Min || RendererModuleValues::MinMax[indx].Max < state)
        {
            sum = sum + (RendererModuleValues::MinMax[indx].Max - RendererModuleValues::MinMax[indx].Min);

            indx = indx + 1;

            if (5 < indx) { return -1; } // TODO
        }

        return (sum - RendererModuleValues::MinMax[indx].Min) + state;
    }

    // 0x60004140
    void InitializeRendererState(void)
    {
        SelectRendererTransforms(1.0, 65535.0);

        State.DX.Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        State.DX.Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        State.DX.Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
        State.DX.Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
        State.DX.Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
        State.DX.Device->SetRenderState(D3DRS_ALPHAREF, 0);

        if (!State.Device.Capabilities.IsDepthAvailable || State.DX.Surfaces.Bits == 0)
        {
            State.DX.Device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
            State.DX.Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
            State.DX.Device->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
        }
        else
        {
            State.DX.Device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
            State.DX.Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
            State.DX.Device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
        }

        State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
        State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);

        if (!State.Device.Capabilities.IsModulateBlending)
        {
            State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLOROP, D3DTOP_MODULATE);
            State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
            State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
            State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
            State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        }
        else
        {
            State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLOROP, D3DTOP_MODULATE);
            State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
            State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
            State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
            State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
            State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
        }

        State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
        State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

        State.DX.Device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
        State.DX.Device->SetRenderState(D3DRS_CLIPPING, 0);
        State.DX.Device->SetRenderState(D3DRS_LIGHTING, 0);

        {
            const char* value = getenv(RENDERER_MODULE_WIRE_FRAME_DX8_ENVIRONMENT_PROPERTY_NAME);

            State.DX.Device->SetRenderState(D3DRS_FILLMODE,
                (value == NULL || atoi(value) == 0) ? D3DFILL_SOLID : D3DFILL_WIREFRAME);
        }

        State.DX.Device->SetRenderState(D3DRS_DITHERENABLE, FALSE); // ORIGINAL: TRUE
        State.DX.Device->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
        State.DX.Device->SetRenderState(D3DRS_FOGENABLE, FALSE);
        State.DX.Device->SetRenderState(D3DRS_FOGCOLOR, 0x00ffffff);
        State.DX.Device->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
        State.DX.Device->SetRenderState(D3DRS_FOGVERTEXMODE, 0);

        {
            const f32 value = 0.0f;
            State.DX.Device->SetRenderState(D3DRS_FOGSTART, *(DWORD*)(&value));
        }

        {
            const f32 value = 1.0f;
            State.DX.Device->SetRenderState(D3DRS_FOGEND, *(DWORD*)(&value));
        }

        State.DX.Device->SetRenderState(D3DRS_TEXTUREFACTOR, GRAPCHICS_COLOR_WHITE);

        SelectState(RENDERER_MODULE_STATE_SELECT_TEXTURE_FILTER_STATE, (void*)RENDERER_MODULE_TEXTURE_FILTER_LENEAR);
        SelectState(RENDERER_MODULE_STATE_SELECT_TEXTURE_MIP_FILTER_STATE, (void*)RENDERER_MODULE_TEXTURE_MIP_FILTER_POINT);
    }

    // 0x600044c0
    u32 SelectRendererTransforms(const f32 zNear, const f32 zFar)
    {
        if ((zFar <= zNear) != (zFar == zNear)) { return TRUE; }

        D3DMATRIX world;

        world._11 = 1.0f;
        world._12 = 0.0f;
        world._13 = 0.0f;
        world._14 = 0.0f;

        world._21 = 0.0f;
        world._22 = 1.0f;
        world._23 = 0.0f;
        world._24 = 0.0f;

        world._31 = 0.0f;
        world._32 = 0.0f;
        world._33 = 1.0f;
        world._34 = 0.0f;

        world._41 = 0.0f;
        world._42 = 0.0f;
        world._43 = 0.0f;
        world._44 = 1.0f;

        D3DMATRIX view;

        view._11 = 1.0f;
        view._12 = 0.0f;
        view._13 = 0.0f;
        view._14 = 0.0f;

        view._21 = 0.0f;
        view._22 = 1.0f;
        view._23 = 0.0f;
        view._24 = 0.0f;

        view._31 = 0.0f;
        view._32 = 0.0f;
        view._33 = 1.0f;
        view._34 = 0.0f;

        view._41 = 0.0f;
        view._42 = 0.0f;
        view._43 = 0.0f;
        view._44 = 1.0f;

        D3DMATRIX projection;

        projection._11 = 1.0f;
        projection._12 = 0.0f;
        projection._13 = 0.0f;
        projection._14 = 0.0f;

        projection._21 = 0.0f;
        projection._22 = 1.0f;
        projection._23 = 0.0f;
        projection._24 = 0.0f;

        projection._31 = 0.0f;
        projection._32 = 0.0f;
        projection._33 = 1.0f;
        projection._34 = 0.0f;

        projection._41 = 0.0f;
        projection._42 = 0.0f;
        projection._43 = 0.0f;
        projection._44 = 1.0f;

        HRESULT result = State.DX.Device->SetTransform(D3DTS_WORLD, &world);

        if (result == D3D_OK)
        {
            result = State.DX.Device->SetTransform(D3DTS_VIEW, &view);

            if (result == D3D_OK)
            {
                projection._44 = zNear;
                projection._34 = 1.0f;
                projection._33 = zNear / (zFar - zNear) + 1.0f;
                projection._43 = 0.0f;

                result = State.DX.Device->SetTransform(D3DTS_PROJECTION, &projection);
            }
        }

        return result;
    }

    // 0x60001d80
    BOOL AreRenderPacketsComplete(const D3DPRIMITIVETYPE type, const u32 count)
    {
        u32 indx = 0;
        BOOL list = FALSE;
        BOOL result = FALSE;

        if (State.Data.Packets.Count != 0)
        {
            if (State.Data.Packets.Packets[State.Data.Packets.Count].Type == type)
            {
                switch (type)
                {
                case D3DPT_POINTLIST:
                case D3DPT_LINELIST:
                case D3DPT_TRIANGLELIST: { list = TRUE; break; }
                case D3DPT_LINESTRIP:
                case D3DPT_TRIANGLESTRIP:
                case D3DPT_TRIANGLEFAN:
                {
                    indx = State.Data.Packets.Count;

                    if ((MAX_RENDER_PACKET_COUNT - 1) < State.Data.Packets.Count) { result = TRUE; }

                    break;
                }
                }
            }
            else
            {
                indx = State.Data.Packets.Count;

                if ((MAX_RENDER_PACKET_COUNT - 1) < State.Data.Packets.Count) { result = TRUE; }
            }
        }

        if (MAX_VERTEX_COUNT < State.Data.Vertexes.Count + count) { result = TRUE; }

        if (MAX_VERTEX_COUNT < State.Data.Vertexes.StartIndex + count) { return TRUE; }

        if (!result)
        {
            u32 max = 0;

            if (list) { max = State.Data.Packets.Packets[indx + 1].Count; }

            switch (type)
            {
            case D3DPT_LINELIST: { max = max + count / 2; break; }
            case D3DPT_LINESTRIP: { max = (max - 1) + count; break; }
            case D3DPT_TRIANGLELIST: { max = max + count / 3; break; }
            case D3DPT_POINTLIST: { max = max + count; break; }
            case D3DPT_TRIANGLESTRIP:
            case D3DPT_TRIANGLEFAN: { max = (max - 2) + count; break; }
            }

            if (State.Data.MaxPrimitiveCount < max) { return TRUE; }
        }

        return result;
    }

    // 0x60001f40
    void RenderAllPackets(void)
    {
        RenderPackets();

        State.Data.Packets.Count = 0;
        State.Data.Vertexes.StartIndex = 0;
        State.Data.Vertexes.Count = 0;
    }

    // 0x60001f60
    void UpdateVertexValues(RTLVX2* vertex)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60001b10
    // a.k.a. showbackbuffer
    u32 ToggleRenderer(void)
    {
        if (State.DX.Device->TestCooperativeLevel() != D3D_OK)
        {
            if (IsRendererToggleLambdaActive)
            {
                if (State.Lambdas.RendererActiveLambda != NULL) { State.Lambdas.RendererActiveLambda(FALSE); }

                IsRendererToggleLambdaActive = FALSE;
            }

            while (TRUE)
            {
                if (State.DX.Device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
                {
                    ReleaseRendererObjects();

                    if (State.DX.Device->Reset(&State.Device.Presentation) == D3D_OK)
                    {
                        if (State.Lambdas.RendererActiveLambda != NULL) { State.Lambdas.RendererActiveLambda(TRUE); }

                        IsRendererToggleLambdaActive = TRUE;

                        return RENDERER_MODULE_SUCCESS;
                    }
                }

                if (State.Lambdas.RendererInactiveLambda != NULL) { State.Lambdas.RendererInactiveLambda(State.Lambdas.RendererInactiveLambdaValue); }

                Sleep(10);
            }
        }

        State.DX.Device->Present(NULL, NULL, State.Window.HWND, NULL);

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60001ca0
    u32 ClearRendererViewPort(const u32 x0, const u32 y0, const u32 x1, const u32 y1, const BOOL window)
    {
        D3DRECT rect;

        DWORD options = (window == FALSE); // D3DCLEAR_TARGET

        if (State.Device.Capabilities.IsDepthAvailable && State.DX.Surfaces.Bits != 0) { options = options | D3DCLEAR_ZBUFFER; }

        if (State.Device.Capabilities.IsStencilBufferAvailable) { options = options | D3DCLEAR_STENCIL; }

        if (x1 == 0)
        {
            rect.x1 = 0;
            rect.y1 = 0;
            rect.x2 = State.DX.Surfaces.Width;
            rect.y2 = State.DX.Surfaces.Height;
        }
        else
        {
            rect.x1 = x0;
            rect.x2 = x1;
            rect.y1 = y0;
            rect.y2 = y1;
        }

        return State.DX.Device->Clear(1, &rect, options, RendererClearColor, RendererClearDepth, 0) == D3D_OK;
    }

    // 0x600070d0
    RendererTexture* AllocateRendererDepthTexture(const u32 width, const u32 height, const u32 format, const u32 options, const u32 state, const BOOL destination)
    {
        if (State.Textures.Illegal) { return NULL; }

        if (!IsRendererTextureDepthFormatAllowed(format)) { return NULL; }

        RendererTexture* tex = AllocateRendererTexture();

        tex->Width = width;
        tex->Height = height;
        tex->PixelFormat = MAKEPIXELFORMAT(format);
        tex->PaletteMode = RENDERER_MODULE_PALETTE_NONE;

        tex->MipMapCount = MAKETEXTUREMIPMAPVALUE(state) != 0 ? (MAKETEXTUREMIPMAPVALUE(state) + 1) : 0;
        tex->Stage = MAKETEXTURESTAGEVALUE(state);
        
        tex->Texture = NULL;

        tex->PixelSize = PixelFormatSizes[format];
        tex->TextureFormat = AcquireRendererTextureDepthFormatIndex(format);
        tex->MemoryType = RENDERER_MODULE_TEXTURE_LOCATION_NON_LOCAL_VIDEO_MEMORY;

        tex->Is16Bit = FALSE;
        tex->Palette = INVALID_TEXTURE_PALETTE_VALUE;
        tex->Colors = 0;

        if (State.DX.Device->CreateRenderTarget(width, height, (D3DFORMAT)MAKEPIXELFORMAT(format), D3DMULTISAMPLE_NONE, TRUE, &tex->Surface) != D3D_OK)
        {
            ReleaseTexture(tex);

            return NULL;
        }

        tex->Previous = State.Textures.Current;
        State.Textures.Current = tex;

        return tex;
    }

    // 0x60007240
    BOOL IsRendererTextureDepthFormatAllowed(const u32 format)
    {
        return (RENDERER_PIXEL_FORMAT_D16 <= format && format <= RENDERER_PIXEL_FORMAT_D24X4S4);
    }

    // 0x600070c0
    RendererTexture* AllocateRendererTexture(void)
    {
        return (RendererTexture*)AllocateRendererTexture(sizeof(RendererTexture));
    }

    // 0x60007f90
    RendererTexture* AllocateRendererTexture(const u32 size)
    {
        if (State.Lambdas.AllocateMemory != NULL) { return (RendererTexture*)State.Lambdas.AllocateMemory(size); }

        return (RendererTexture*)malloc(size);
    }

    // 0x60007260
    D3DFORMAT AcquireRendererTextureDepthFormatIndex(const u32 format)
    {
        switch (format)
        {
        case RENDERER_PIXEL_FORMAT_D16: { return D3DFMT_D16; }
        case RENDERER_PIXEL_FORMAT_D24S8: { return D3DFMT_D24S8; }
        case RENDERER_PIXEL_FORMAT_D16L: { return D3DFMT_D16_LOCKABLE; }
        case RENDERER_PIXEL_FORMAT_D32: { return D3DFMT_D32; }
        case RENDERER_PIXEL_FORMAT_D15S1: { return D3DFMT_D15S1; }
        case RENDERER_PIXEL_FORMAT_D24X8: { return D3DFMT_D24X8; }
        case RENDERER_PIXEL_FORMAT_D24X4S4: { return D3DFMT_D24X4S4; }
        }

        return D3DFMT_UNKNOWN;
    }

    // 0x600018b0
    void SelectObjectReferenceCount(IUnknown* object, const u32 count)
    {
        if (object == NULL) { return; }

        u32 actual = object->AddRef() - 1;

        object->Release();

        if (actual != count)
        {
            while (actual < count) { actual = object->AddRef(); }
            while (count < actual) { actual = object->Release(); }
        }
    }

    // 0x60008730
    void InitializeRenderState55(void)
    {
        SelectBasicRendererState(RENDERER_MODULE_STATE_55, (void*)(DAT_6001dfcc + 1));
        SelectState(RENDERER_MODULE_STATE_SELECT_TEXTURE, NULL);
    }

    // 0x60007fb0
    u32 DisposeRendererTexture(RendererTexture* tex)
    {
        if (State.Lambdas.ReleaseMemory != NULL) { return State.Lambdas.ReleaseMemory(tex); }

        free(tex);

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60007620
    HRESULT InitializeRendererTexture(RendererTexture* tex)
    {
        const HRESULT result = State.DX.Device->CreateTexture(tex->Width, tex->Height,
            tex->MipMapCount + 1, D3DUSAGE_NONE, tex->TextureFormat, D3DPOOL_MANAGED, &tex->Texture);

        if (result == D3D_OK) { tex->MemoryType = RENDERER_MODULE_TEXTURE_LOCATION_NON_LOCAL_VIDEO_MEMORY; }

        return result;
    }

    // 0x60008910
    u32 AcquireTexturePalette()
    {
        if (IsTexturePaletteInactive)
        {
            InitializeTexturePalette();

            IsTexturePaletteInactive = FALSE;
        }

        if (TexturePaletteValues[0] == 0) { return U32_MAX; }

        u32 i = 0;
        u32 bits = 1;
        u32 value = 0;
        u32 indx = 1;
        u32 iteration = 9;

        while (TRUE)
        {
            i = (value - 1) + indx;

            for (u32 x = value; x < indx; x = x + 1)
            {
                if (TexturePaletteValues[i] != 0)
                {
                    TexturePaletteValues[i] = TexturePaletteValues[i] - 1;

                    value = x * 2;

                    break;
                }

                i = i + 1;
            }

            indx = indx << 1;
            iteration = iteration - 1;

            if (iteration == 0)
            {
                if (TexturePaletteIndexes[value] == 0) { value = value + 1; }

                indx = 0;

                do
                {
                    if ((bits & TexturePaletteIndexes[value]) != 0)
                    {
                        TexturePaletteIndexes[value] = ~bits & TexturePaletteIndexes[value];

                        return value * 0x20 + indx;
                    }

                    bits = bits << 1;
                    indx = indx + 1;
                } while (indx < 0x20);

                return value * 0x20 + indx;
            }
        }
    }

    // 0x60008870
    void InitializeTexturePalette(void)
    {
        u32 bits = 1;
        u32 value = 0x4000; // TODO

        u32* values = TexturePaletteValues;

        for (u32 x = 9; x != 0; x--) // TODO
        {
            if (bits != 0)
            {
                for (u32 xx = bits; xx != 0; xx--)
                {
                    values[xx] = value;
                }

                values = (u32*)((addr)values + (addr)bits);
            }

            value = value >> 1;
            bits = bits << 1;
        }

        for (u32 x = 0; x < MAX_TEXTURE_PALETTE_INDEX_COUNT; x++)
        {
            TexturePaletteIndexes[x] = U32_MAX;
        }
    }

    // 0x600088b0
    void ReleaseTexturePalette(const s32 palette)
    {
        u32 bits = 1;
        u32 value = 0x4000; // TODO

        for (u32 x = 9; x != 0; x--) // TODO
        {
            value = value >> 1;
            const u32 indx = (palette / value - 1) + bits;
            bits = bits << 1;

            TexturePaletteValues[indx] = TexturePaletteValues[indx] + 1;
        }

        TexturePaletteIndexes[palette / value] = TexturePaletteIndexes[palette / value] | 1 << ((byte)palette & 0x1f);
    }

    // 0x60001ae0
    BOOL SelectRendererState(const D3DRENDERSTATETYPE type, const DWORD value)
    {
        BeginRendererScene();

        RenderPackets();

        return State.DX.Device->SetRenderState(type, value) == D3D_OK;
    }

    // 0x60001be0
    void SelectRendererMaterial(const u32 color)
    {
        D3DMATERIAL8 material;
        ZeroMemory(&material, sizeof(D3DMATERIAL8));

        RendererClearColor = color;

        const f32 red = RGBA_GETRED(color) / 255.0f;
        const f32 green = RGBA_GETGREEN(color) / 255.0f;
        const f32 blue = RGBA_GETBLUE(color) / 255.0f;

        material.Diffuse.r = red;
        material.Diffuse.g = green;
        material.Diffuse.b = blue;

        material.Ambient.r = red;
        material.Ambient.g = green;
        material.Ambient.b = blue;

        State.DX.Device->SetMaterial(&material);
    }

    // 0x60001ab0
    BOOL SelectRendererTextureStage(const u32 stage, const D3DTEXTURESTAGESTATETYPE type, const DWORD value)
    {
        BeginRendererScene();

        RenderPackets();

        return State.DX.Device->SetTextureStageState(stage, type, value) == D3D_OK;
    }

    // 0x60006cb0
    void SelectRendererFogAlphas(const u8* input, u8* output)
    {
        if (input == NULL || output == NULL) { return; }

        for (u32 x = 0; x < MAX_INPUT_FOG_ALPHA_COUNT; x++)
        {
            u8 current = input[x];
            u8 next = input[Clamp<u32>(x + 1, 0, MAX_INPUT_FOG_ALPHA_COUNT - 1)];

            output[x * 4 + 0] = current;
            output[x * 4 + 1] = (u8)((s32)current + (s32)(0.33f * (next - current)));
            output[x * 4 + 2] = (u8)((s32)current + (s32)(0.67f * (next - current)));
            output[x * 4 + 3] = next;
        }
    }

    // 0x60002da0
    BOOL RestoreRendererSurfaces(void)
    {
        BOOL result = FALSE;

        for (u32 x = 0; x < 4; x++)
        {
            const HRESULT res = State.DX.Device->TestCooperativeLevel();

            if (res == D3DERR_DEVICENOTRESET || res == D3D_OK) { break; }
            else { Sleep(100); }
        }

        return result;
    }

    // 0x600072c0
    BOOL UpdateRendererTexture(RendererTexture* tex, const u32* pixels, const u32* palette)
    {
        if (tex == NULL) { return FALSE; }
        if (pixels == NULL && palette == NULL) { return FALSE; }

        if (palette != NULL) { if (!UpdateRendererTexturePalette(tex, palette)) { return FALSE; } }

        if (pixels == NULL) { return TRUE; }

        u32* px = (u32*)pixels;

        if (tex->PixelFormat == RENDERER_PIXEL_FORMAT_DXT1 || tex->PixelFormat == RENDERER_PIXEL_FORMAT_DXT3)
        {
            const u8 v0 = (*(u8*)((addr)px + (addr)0));
            const u8 v1 = (*(u8*)((addr)px + (addr)1));

            if ((v0 == 24 && v1 == 251) || (v0 == 26 && v1 == 251)) // TODO
            {
                px = (u32*)((addr)px + (addr)(2 * sizeof(u32)));
            }
        }

        for (u32 x = 0; x < tex->MipMapCount; x++)
        {
            IDirect3DSurface8* surface = NULL;
            tex->Texture->GetSurfaceLevel(x, &surface);

            D3DSURFACE_DESC desc;
            ZeroMemory(&desc, sizeof(D3DSURFACE_DESC));

            surface->GetDesc(&desc);

            D3DLOCKED_RECT lock;
            ZeroMemory(&lock, sizeof(D3DLOCKED_RECT));

            surface->LockRect(&lock, NULL, D3DLOCK_NOSYSLOCK | D3DLOCK_READONLY);
            surface->UnlockRect();

            D3DRECT rect;

            rect.x1 = 0;
            rect.x2 = desc.Width;

            rect.y1 = 0;
            rect.y2 = desc.Height;

            if (UpdateRendererTextureLevel(surface, NULL, &rect, px, tex->TextureFormat, lock.Pitch,
                NULL, &rect, IMAGE_CONTAINER_OPTIONS_COLOR, GRAPCHICS_COLOR_BLACK) != D3D_OK)
            {
                surface->Release(); return FALSE;
            }

            surface->Release();

            px = (u32*)((addr)px + (addr)(lock.Pitch * desc.Height));
        }

        return TRUE;
    }

    // 0x600087a0
    BOOL UpdateRendererTexturePalette(RendererTexture* tex, const u32* palette)
    {
        if (tex == NULL || palette == NULL) { return FALSE; }
        if (RendererTextureFormatStates[2] != 1) { return FALSE; } // TODO
        if (tex->PaletteMode != RENDERER_MODULE_PALETTE_ACQUIRE) { return FALSE; }

        if (tex->Palette != INVALID_TEXTURE_PALETTE_VALUE)
        {
            PALETTEENTRY entries[MAX_TEXTURE_PALETTE_COLOR_COUNT];

            for (u32 x = 0; x < MAX_TEXTURE_PALETTE_COLOR_COUNT; x++)
            {
                entries[x].peRed = RGBA_GETRED(palette[x]);
                entries[x].peGreen = RGBA_GETGREEN(palette[x]);
                entries[x].peBlue = RGBA_GETBLUE(palette[x]);
                entries[x].peFlags = RGBA_GETALPHA(palette[x]);
            }

            if (State.DX.Device->SetPaletteEntries(tex->Palette, entries) != D3D_OK) { return FALSE; }

            return State.DX.Device->SetCurrentTexturePalette(tex->Palette) == D3D_OK;
        }

        return FALSE;
    }

    // 0x60007450
    BOOL UpdateRendererTextureRectangle(RendererTexture* tex, const u32* pixels, const u32* palette, const s32 x, const s32 y, const s32 width, const s32 height, const s32 size, const s32 level)
    {
        if (tex == NULL) { return FALSE; }
        if (pixels == NULL && palette == NULL) { return FALSE; }

        if (-1 < x && -1 < y && 0 < width && 0 < height && 0 < size && -1 < level)
        {
            if (palette == NULL || UpdateRendererTexturePalette(tex, palette))
            {
                if (pixels == NULL) { return TRUE; }

                IDirect3DSurface8* surface = NULL;
                tex->Texture->GetSurfaceLevel(level, &surface);

                D3DSURFACE_DESC desc;
                ZeroMemory(&desc, sizeof(D3DSURFACE_DESC));

                surface->GetDesc(&desc);

                D3DRECT dst;

                dst.x1 = x;
                dst.x2 = x + width;
                dst.y1 = y;
                dst.y2 = height + y;

                D3DRECT src;

                src.x1 = 0;
                src.x2 = width;
                src.y1 = 0;
                src.y2 = height;

                if (dst.x2 <= desc.Width && dst.y2 <= desc.Height)
                {
                    u32* px = (u32*)pixels;

                    if (tex->PixelFormat == RENDERER_PIXEL_FORMAT_DXT1 || tex->PixelFormat == RENDERER_PIXEL_FORMAT_DXT3)
                    {
                        const u8 v0 = (*(u8*)((addr)px + (addr)0));
                        const u8 v1 = (*(u8*)((addr)px + (addr)1));

                        if ((v0 == 24 && v1 == 251) || (v0 == 26 && v1 == 251)) // TODO
                        {
                            px = (u32*)((addr)px + (addr)(2 * sizeof(u32)));
                        }
                    }

                    if (UpdateRendererTextureLevel(surface,
                        NULL, &dst, px, tex->TextureFormat, size,
                        NULL, &src, IMAGE_CONTAINER_OPTIONS_COLOR, GRAPCHICS_COLOR_BLACK) == D3D_OK)
                    {
                        surface->Release();

                        return TRUE;
                    }

                    surface->Release();
                }
            }
        }

        return FALSE;
    }

    // 0x60008ef6
    ImageFormatDescriptor* AcquireImageFormatDescriptor(const D3DFORMAT format)
    {
        for (u32 x = 0; x < MAX_IMAGE_FORMAT_DESCRIPTOR_COUNT; x++)
        {
            if (ImageFormatDescriptors[x].Format == format) { return &ImageFormatDescriptors[x]; }
        }

        return NULL;
    }

    // 0x60008f1a
    HRESULT UpdateRendererTextureLevel(IDirect3DSurface8* surface,
        const u8* dpal, const D3DRECT* drect,
        const u32* pixels, const D3DFORMAT format, const u32 pitch,
        const u8* spal, const D3DRECT* srect, const u32 options, const u32 color)
    {
        ImageContainer img;

        InitializeImageContainer(&img);

        if (surface == NULL || pixels == NULL || srect == NULL)
        {
            ReleaseImageContainer(&img);

            return D3DERR_INVALIDCALL;
        }

        D3DSURFACE_DESC desc;
        ZeroMemory(&desc, sizeof(D3DSURFACE_DESC));

        surface->GetDesc(&desc);

        RECT rect;

        if (drect == NULL)
        {
            rect.left = 0;
            rect.top = 0;
            rect.right = desc.Width;
            rect.bottom = desc.Height;
        }
        else
        {
            rect.left = drect->x1;
            rect.top = drect->y1;
            rect.right = drect->x2;
            rect.bottom = drect->y2;

            if (rect.left < 0 || desc.Width < rect.right || rect.right < rect.left || rect.top < 0 || desc.Height < rect.bottom || rect.bottom < rect.top)
            {
                ReleaseImageContainer(&img);

                return D3DERR_INVALIDCALL;
            }
        }

        D3DLOCKED_RECT lock;
        ZeroMemory(&lock, sizeof(D3DLOCKED_RECT));

        if (desc.Format == D3DFMT_DXT1 || desc.Format == D3DFMT_DXT2 || desc.Format == D3DFMT_YUY2
            || desc.Format == D3DFMT_DXT3 || desc.Format == D3DFMT_DXT4 || desc.Format == D3DFMT_DXT5 || desc.Format == D3DFMT_UYVY)
        {
            const HRESULT result = surface->LockRect(&lock, NULL, D3DLOCK_NONE);

            if (result != D3D_OK)
            {
                ReleaseImageContainer(&img);

                return result;
            }
        }
        else
        {
            const HRESULT result = surface->LockRect(&lock, &rect, D3DLOCK_NONE);

            if (result != D3D_OK)
            {
                ReleaseImageContainer(&img);

                return result;
            }

            rect.left = 0;
            rect.top = 0;
            rect.right = rect.right - rect.left;
            rect.bottom = rect.bottom - rect.top;
        }

        ImageContainerArgs dsti;
        ZeroMemory(&dsti, sizeof(ImageContainerArgs));

        dsti.Format = desc.Format;
        dsti.Pixels = lock.pBits;
        dsti.Stride = lock.Pitch;

        dsti.Width = desc.Width;
        dsti.Height = desc.Height;

        dsti.Unk08 = 0;
        dsti.Unk09 = 1;

        dsti.Dimensions.Left = rect.left;
        dsti.Dimensions.Top = rect.top;
        dsti.Dimensions.Right = rect.right;
        dsti.Dimensions.Bottom = rect.bottom;
        dsti.Dimensions.Min = 0;
        dsti.Dimensions.Max = 1;

        dsti.Color = GRAPCHICS_COLOR_BLACK;
        dsti.Palette = dpal;

        ImageContainerArgs srci;
        ZeroMemory(&srci, sizeof(ImageContainerArgs));

        srci.Format = format;
        srci.Pixels = (void*)pixels;
        srci.Stride = pitch;

        srci.Dimensions.Left = srect->x1;
        srci.Dimensions.Top = srect->y1;
        srci.Dimensions.Right = srect->x2;
        srci.Dimensions.Bottom = srect->y2;
        srci.Dimensions.Min = 0;
        srci.Dimensions.Max = 1;

        srci.Color = color;
        srci.Palette = spal;

        const HRESULT result = InitializeImageContainer(&img, &dsti, &srci, options == IMAGE_CONTAINER_OPTIONS_INVALID
            ? (AcquireImageFormatDescriptor(format)->Format == IMAGE_FORMAT_DESCRIPTOR_TYPE_UV
                ? (IMAGE_CONTAINER_OPTIONS_GRADIENT | IMAGE_CONTAINER_OPTIONS_BUMP_MAP) : (IMAGE_CONTAINER_OPTIONS_GRADIENT | IMAGE_CONTAINER_OPTIONS_UNKNOWN_4)) : options);

        surface->UnlockRect();

        ReleaseImageContainer(&img);

        return result;
    }
}