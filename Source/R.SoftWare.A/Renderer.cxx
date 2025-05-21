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
#include "Renderer.hxx"
#include "RendererValues.hxx"

#include <malloc.h>
#include <math.h>
#include <stdio.h>

#define MAX_SETTINGS_BUFFER_LENGTH 80

using namespace Renderer;
using namespace RendererModuleValues;

namespace RendererModule
{
    RendererModuleState State;

    u32 DAT_60a2a5d0;
    u32 DAT_60a2a5d4;

    // 0x60016df0
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

    // 0x600015b0
    void SelectRendererColorMasks(const u32 bits)
    {
        if (bits == GRAPHICS_BITS_PER_PIXEL_16)
        {
            RedRendererColorMask = 0xf800;
            GreenRendererColorMask = 0x7e0;
            BlueRendererColorMask = 0x1f;
            NonGreenRendererColorMask = 0xf81f;

            State.Renderer.Colors.Unknown1 = Unknown16BitColors1;
            State.Renderer.Colors.Unknown2 = Unknown16BitColors2;
            State.Renderer.Colors.Unknown3 = Unknown16BitColors3;
            State.Renderer.Colors.Unknown4 = Unknown16BitColors4;

            GreenRendererColorShift = 11;
            GreenRendererColorLength = 6;
        }
        else
        {
            RedRendererColorMask = 0x7c00;
            GreenRendererColorMask = 0x3e0;
            BlueRendererColorMask = 0x1f;
            NonGreenRendererColorMask = 0x7c1f;

            State.Renderer.Colors.Unknown1 = Unknown32BitColors1;
            State.Renderer.Colors.Unknown2 = Unknown32BitColors2;
            State.Renderer.Colors.Unknown3 = Unknown32BitColors3;
            State.Renderer.Colors.Unknown4 = Unknown32BitColors4;

            GreenRendererColorShift = 10;
            GreenRendererColorLength = 5;
        }
    }
    // 0x60001440
    void ToggleRenderer(void)
    {
        State.Lambdas.Lambdas.LockWindow(TRUE);

        DDSURFACEDESC2 desc;
        ZeroMemory(&desc, sizeof(DDSURFACEDESC2));

        desc.dwSize = sizeof(DDSURFACEDESC2);

        State.DX.Code = State.DX.Surfaces.Active[1]->Lock(NULL, &desc, DDLOCK_NOSYSLOCK | DDLOCK_WAIT, NULL);

        if (State.DX.Code == DD_OK)
        {
            State.DX.Surfaces.Stride = desc.lPitch;

            if (!State.Settings.IsWindowMode) { AcquireRendererSurface(desc.lpSurface); }
            else
            {
                RECT rect;
                GetClientRect(State.Window.HWND, &rect);

                POINT point;
                ZeroMemory(&point, sizeof(POINT));

                ClientToScreen(State.Window.HWND, &point);
                OffsetRect(&rect, point.x, point.y);

                void* src = AcquireRendererSurface();
                void* dst = (void*)((addr)desc.lpSurface + (addr)(rect.left * sizeof(u16) + rect.top * desc.lPitch));

                for (u32 xx = 0; xx < State.Renderer.Settings.Height; xx++)
                {
                    CopyMemory(dst, src, RendererSurfaceStride);

                    dst = (void*)((addr)dst + (addr)desc.lPitch);
                    src = (void*)((addr)src + (addr)RendererSurfaceStride);
                }
            }

            State.DX.Surfaces.Active[1]->Unlock((LPRECT)desc.lpSurface);

            State.Lock.Surface = NULL;
            State.Lock.IsActive = FALSE;

            State.Lambdas.Lambdas.LockWindow(FALSE);
        }
        else
        {
            OutputDebugStringA("lock failed on writebuffer\n");

            if (State.Settings.IsAutoUnlock)
            {
                State.DX.Surfaces.Active[1]->Unlock((LPRECT)desc.lpSurface);

                State.Lock.Surface = NULL;
                State.Lock.IsActive = FALSE;

                State.Lambdas.Lambdas.LockWindow(FALSE);
            }
        }
    }

    // 0x60001aa0
    void SelectIndexedRendererLambdas(void)
    {
        State.Renderer.Lambdas.Active = &State.Renderer.Lambdas.Lambdas[State.Renderer.Lambdas.Index + 1];
    }

    // 0x60001ac0
    void SelectMainRendererLambdas(void)
    {
        State.Renderer.Lambdas.Active = &State.Renderer.Lambdas.Lambdas[0];
    }

    // 0x60001ad0
    u32 AcquireColorValue(const u32 color)
    {
        if (State.DX.Surfaces.Bits == GRAPHICS_BITS_PER_PIXEL_16)
        {
            return ((color >> 3 & 0x1f0000 | color & 0xfc00) >> 2 | color & 0xf8) >> 3;
        }

        if (State.DX.Surfaces.Bits == GRAPHICS_BITS_PER_PIXEL_15)
        {
            return ((color >> 3 & 0x1f0000 | color & 0xf800) >> 3 | color & 0xf8) >> 3;
        }

        return color;
    }
    
    // 0x60001bc0
    void SelectRendererMaterial(const u32 color)
    {
        u32 value = AcquireColorValue(color);

        value = value | (value << GRAPHICS_BITS_PER_PIXEL_16);

        State.Renderer.Material = value;

        State.Renderer.Materials[0] = value;
        State.Renderer.Materials[1] = value;
    }

    // 0x60001bf0
    void SelectRendererSettings(const u32 width, const u32 height, const u32 bits)
    {
        if (State.Renderer.Surface.Allocated != NULL) { free(State.Renderer.Surface.Allocated); }

        RendererSurfaceStride = width * (bits >> 3);

        State.Renderer.Settings.Length = RendererSurfaceStride * height;

        State.Renderer.Settings.Width = width;
        State.Renderer.Settings.Height = height;

        ClipGameWindow(0, 0, width, height);

        State.Renderer.Surface.Allocated = malloc(State.Renderer.Settings.Length + RENDERER_SURFACE_SIZE_MOFIFIER);

        {
            void* ptr = (void*)(((addr)State.Renderer.Surface.Allocated
                + (addr)RENDERER_SURFACE_POINTER_ALIGNEMENT_OFFSET) & RENDERER_SURFACE_ALIGNMENT_MASK);

            State.Renderer.Surface.Surface = (void*)((addr)ptr + (addr)RENDERER_SURFACE_SIZE_OFFSET);

            InitializeRendererModuleSurfaceDescriptor((RendererModuleSurfaceDescriptor*)((void*)((addr)ptr + (addr)RENDERER_SURFACE_SIZE_DESCRIPTOR_OFFSET)),
                State.Renderer.Settings.Width, State.Renderer.Settings.Height, State.DX.Surfaces.Bits);
        }

        State.Renderer.Active.Stride = RendererSurfaceStride;

        State.Renderer.Active.Length = State.Renderer.Settings.Length;

        State.Renderer.Active.Width = State.Renderer.Settings.Width;
        State.Renderer.Active.Height = State.Renderer.Settings.Height;

        State.Renderer.Active.Surface = State.Renderer.Surface.Surface;
    }

    // 0x60001cc0
    void InitializeRendererModuleSurfaceDescriptor(RendererModuleSurfaceDescriptor* self, const u32 width, const u32 height, const u32 bits)
    {
        switch (bits)
        {
        case GRAPHICS_BITS_PER_PIXEL_8: { self->Unk00 = (self->Unk00 & 0xffffff7b | 0x7b) & 0xff; break; }
        case GRAPHICS_BITS_PER_PIXEL_9:
        case GRAPHICS_BITS_PER_PIXEL_10:
        case GRAPHICS_BITS_PER_PIXEL_11:
        case GRAPHICS_BITS_PER_PIXEL_12:
        case GRAPHICS_BITS_PER_PIXEL_13:
        case GRAPHICS_BITS_PER_PIXEL_14:
        case GRAPHICS_BITS_PER_PIXEL_17:
        case GRAPHICS_BITS_PER_PIXEL_18:
        case GRAPHICS_BITS_PER_PIXEL_19:
        case GRAPHICS_BITS_PER_PIXEL_20:
        case GRAPHICS_BITS_PER_PIXEL_21:
        case GRAPHICS_BITS_PER_PIXEL_22:
        case GRAPHICS_BITS_PER_PIXEL_23:
        case GRAPHICS_BITS_PER_PIXEL_25:
        case GRAPHICS_BITS_PER_PIXEL_26:
        case GRAPHICS_BITS_PER_PIXEL_27:
        case GRAPHICS_BITS_PER_PIXEL_28:
        case GRAPHICS_BITS_PER_PIXEL_29:
        case GRAPHICS_BITS_PER_PIXEL_30:
        case GRAPHICS_BITS_PER_PIXEL_31: { self->Unk00 = (self->Unk00 & 0xffffff00) & 0xff; break; }
        case GRAPHICS_BITS_PER_PIXEL_15: { self->Unk00 = (self->Unk00 & 0xffffff7e | 0x7e) & 0xff; break; }
        case GRAPHICS_BITS_PER_PIXEL_16: { self->Unk00 = (self->Unk00 & 0xffffff78 | 0x78) & 0xff; break; }
        case GRAPHICS_BITS_PER_PIXEL_24: { self->Unk00 = (self->Unk00 & 0xffffff7f | 0x7f) & 0xff; break; }
        case GRAPHICS_BITS_PER_PIXEL_32: { self->Unk00 = (self->Unk00 & 0xffffff7d | 0x7d) & 0xff; break; }
        default: { self->Unk00 = (self->Unk00 & 0xffffff2a | 0x2a) & 0xff; break; }
        }

        self->Unk03 = 0;
        self->Unk04 = 0;
        self->Width = width;
        self->Height = height;
        self->Unk05 = self->Unk05 & 0x2000; // TODO
    }

    // 0x60001db0
    void* AcquireRendererSurface(void)
    {
        return State.Renderer.Surface.Surface;
    }

    // 0x60001dc0
    // 0x60004769
    u32 RendererClearGameWindow(void)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x600024b0
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

    // 0x60002500
    void SelectRendererStateValue(const u32 state, void* value)
    {
        const s32 indx = AcquireTextureStateStageIndex(state);

        if (indx >= 0) { State.Textures.StageStates[indx] = (s32)value; }

        if (State.Lambdas.SelectState != NULL) { State.Lambdas.SelectState(state, value); }
    }

    // 0x60002530
    BOOL CALLBACK EnumerateRendererDevices(GUID* uid, LPSTR name, LPSTR description, LPVOID context)
    {
        const s32 indx = *(s32*)context;

        if (indx != RendererDeviceIndex)
        {
            *(s32*)context = indx + 1;

            return TRUE;
        }

        if (uid != NULL)
        {
            State.Device.ID = &State.Device.Value;

            State.Device.Value = *uid;

            *(s32*)context = INVALID_DEVICE_INDEX;

            return FALSE;
        }

        State.Device.ID = NULL;

        *(s32*)context = INVALID_DEVICE_INDEX;

        return TRUE;
    }

    // 0x600025a0
    u32 InitializeRendererDeviceLambdas(void)
    {
        if (State.Mutex == NULL) { State.Mutex = CreateEventA(NULL, FALSE, FALSE, NULL); }

        State.Window.HWND = State.Lambdas.Lambdas.AcquireWindow();

        if (State.Window.HWND != NULL)
        {
            State.Lambdas.Lambdas.Execute(RENDERER_MODULE_WINDOW_MESSAGE_INITIALIZE_DEVICE, (RENDERERMODULEEXECUTECALLBACK)InitializeRendererDeviceExecute);
            State.Lambdas.Lambdas.Execute(RENDERER_MODULE_WINDOW_MESSAGE_RELEASE_DEVICE, (RENDERERMODULEEXECUTECALLBACK)ReleaseRendererDeviceExecute);
            State.Lambdas.Lambdas.Execute(RENDERER_MODULE_WINDOW_MESSAGE_INITIALIZE_SURFACES, (RENDERERMODULEEXECUTECALLBACK)InitializeRendererDeviceSurfacesExecute);

            State.Settings.CooperativeLevel = State.Settings.CooperativeLevel & ~(DDSCL_EXCLUSIVE | DDSCL_NORMAL | DDSCL_FULLSCREEN);

            State.Settings.CooperativeLevel = State.Settings.IsWindowMode
                ? State.Settings.CooperativeLevel | DDSCL_NORMAL
                : State.Settings.CooperativeLevel | DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN;

            SetForegroundWindow(State.Window.HWND);
            PostMessageA(State.Window.HWND, RENDERER_MODULE_WINDOW_MESSAGE_INITIALIZE_DEVICE, 0, 0);
            WaitForSingleObject(State.Mutex, INFINITE);

            return State.DX.Code;
        }

        State.DX.Code = RENDERER_MODULE_INITIALIZE_DEVICE_SUCCESS;

        return RENDERER_MODULE_INITIALIZE_DEVICE_SUCCESS;
    }
    
    // 0x60002670
    u32 STDCALLAPI InitializeRendererDeviceExecute(const void*, const HWND hwnd, const u32 msg, const u32 wp, const u32 lp, HRESULT* result)
    {
        {
            const char* value = getenv(RENDERER_MODULE_DISPLAY_ENVIRONMENT_PROPERTY_NAME);

            if (value == NULL)
            {
                if (State.Device.ID == NULL) { SelectDevice(DEFAULT_DEVICE_INDEX); }
            }
            else
            {
                const s32 index = atoi(value);

                if (DEFAULT_DEVICE_INDEX < index)
                {
                    if (SelectDevice(index) == RENDERER_MODULE_FAILURE) { SelectDevice(DEFAULT_DEVICE_INDEX); }
                }
                else { SelectDevice(DEFAULT_DEVICE_INDEX); }
            }
        }

        IDirectDraw* instance = NULL;

        State.DX.Code = DirectDrawCreate(NULL, &instance, NULL);

        if (State.DX.Code == DD_OK)
        {
            State.DX.Code = instance->QueryInterface(IID_IDirectDraw4, (LPVOID*)&State.DX.Instance);

            State.Lambdas.Lambdas.SelectInstance(instance);

            instance->Release();

            if (State.DX.Code == DD_OK)
            {
                State.DX.Code = State.DX.Instance->SetCooperativeLevel(State.Window.HWND, State.Settings.CooperativeLevel);

                if (State.DX.Code == DD_OK)
                {
                    DWORD free = 0;
                    DWORD total = 0;

                    DDSCAPS2 caps;
                    ZeroMemory(&caps, sizeof(DDSCAPS2));

                    caps.dwCaps = DDSCAPS_VIDEOMEMORY;

                    State.Settings.MaxAvailableMemory =
                        State.DX.Instance->GetAvailableVidMem(&caps, &total, &free) == DD_OK
                        ? free : DEFAULT_DEVICE_AVAIABLE_VIDEO_MEMORY;

                    DDCAPS hal;
                    ZeroMemory(&hal, sizeof(DDCAPS));

                    hal.dwSize = sizeof(DDCAPS);

                    DDCAPS hel;
                    ZeroMemory(&hel, sizeof(DDCAPS));

                    hel.dwSize = sizeof(DDCAPS);

                    State.DX.Instance->GetCaps(&hal, &hel);

                    SYSTEM_INFO info;
                    GetSystemInfo(&info);

                    ModuleDescriptor.Capabilities.Capabilities =
                        (RendererModuleDescriptorDeviceCapabilities*)VirtualAlloc(NULL, info.dwPageSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

                    ZeroMemory(ModuleDescriptor.Capabilities.Capabilities, info.dwPageSize);

                    ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Width = GRAPHICS_RESOLUTION_640;
                    ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Height = GRAPHICS_RESOLUTION_480;
                    ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Bits = GRAPHICS_BITS_PER_PIXEL_16;
                    ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Format = RENDERER_PIXEL_FORMAT_R5G6B5;
                    ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Unk03 = 2;
                    ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Unk04 = 0;
                    ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].IsActive = TRUE;

                    ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_800_600_16].Width = GRAPHICS_RESOLUTION_800;
                    ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_800_600_16].Height = GRAPHICS_RESOLUTION_600;
                    ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_800_600_16].Bits = GRAPHICS_BITS_PER_PIXEL_16;
                    ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_800_600_16].Format = RENDERER_PIXEL_FORMAT_R5G6B5;
                    ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_800_600_16].Unk03 = 2;
                    ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_800_600_16].Unk04 = 0;
                    ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_800_600_16].IsActive = TRUE;

                    ModuleDescriptor.Capabilities.Count = 3;

                    State.DX.Instance->EnumDisplayModes(DDEDM_NONE, NULL,
                        &ModuleDescriptor.Capabilities.Count, EnumerateRendererDeviceModes);

                    DWORD previous = 0;
                    VirtualProtect(ModuleDescriptor.Capabilities.Capabilities, info.dwPageSize, PAGE_READONLY, &previous);
                }
            }
        }

        SetEvent(State.Mutex);

        *result = State.DX.Code;

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60002890
    HRESULT CALLBACK EnumerateRendererDeviceModes(LPDDSURFACEDESC2 desc, LPVOID context)
    {
        u32 bits = desc->ddpfPixelFormat.dwRGBBitCount == GRAPHICS_BITS_PER_PIXEL_15
            ? GRAPHICS_BITS_PER_PIXEL_16 : desc->ddpfPixelFormat.dwRGBBitCount;

        const u32 format = AcquirePixelFormat(&desc->ddpfPixelFormat);

        if (format != RENDERER_PIXEL_FORMAT_NONE)
        {
            const u32 width = desc->dwWidth;
            const u32 height = desc->dwHeight;

            RendererModuleDescriptorDeviceCapabilities* caps = NULL;

            if (width == GRAPHICS_RESOLUTION_640 && height == GRAPHICS_RESOLUTION_480 && bits == GRAPHICS_BITS_PER_PIXEL_16)
            {
                caps = &ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16];
            }
            else if (width == GRAPHICS_RESOLUTION_800 && height == GRAPHICS_RESOLUTION_600 && bits == GRAPHICS_BITS_PER_PIXEL_16)
            {
                caps = &ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_800_600_16];
            }
            else
            {
                SYSTEM_INFO info;
                GetSystemInfo(&info);

                const u32 indx = *(u32*)context;

                if (info.dwPageSize / sizeof(RendererModuleDescriptorDeviceCapabilities) <= indx) { return DDENUMRET_CANCEL; }

                *(u32*)context = indx + 1;

                caps = &ModuleDescriptor.Capabilities.Capabilities[indx];
            }

            caps->Width = width;
            caps->Height = height;
            caps->Bits = format == RENDERER_PIXEL_FORMAT_R5G5B5 ? GRAPHICS_BITS_PER_PIXEL_15 : bits;
            caps->Format = format;
            caps->Unk03 = 2;
            caps->Unk04 = 0;
            caps->IsActive = TRUE;
        }

        return DDENUMRET_OK;
    }

    // 0x60002980
    u32 STDCALLAPI InitializeRendererDeviceSurfacesExecute(const void*, const HWND hwnd, const u32 msg, const u32 wp, const u32 lp, HRESULT* result)
    {
        ReleaseRendererDeviceSurfaces();

        State.Window.Width = ModuleDescriptor.Capabilities.Capabilities[wp].Width;
        State.Window.Height = ModuleDescriptor.Capabilities.Capabilities[wp].Height;

        State.DX.Bits = ModuleDescriptor.Capabilities.Capabilities[wp].Bits;
        State.DX.Surfaces.Bits = ModuleDescriptor.Capabilities.Capabilities[wp].Bits;
        State.Window.Bits = ModuleDescriptor.Capabilities.Capabilities[wp].Bits == GRAPHICS_BITS_PER_PIXEL_15
            ? GRAPHICS_BITS_PER_PIXEL_16 : ModuleDescriptor.Capabilities.Capabilities[wp].Bits;

        SelectRendererColorMasks(ModuleDescriptor.Capabilities.Capabilities[wp].Bits);

        State.DX.Code = DD_OK;

        if (!State.Settings.IsWindowMode)
        {
            State.DX.Code = State.DX.Instance->SetDisplayMode(ModuleDescriptor.Capabilities.Capabilities[wp].Width,
                ModuleDescriptor.Capabilities.Capabilities[wp].Height, State.Window.Bits, 0, 0);

            if (State.DX.Code == DD_OK)
            {
                DDSURFACEDESC2 desc;
                ZeroMemory(&desc, sizeof(DDSURFACEDESC2));

                desc.dwSize = sizeof(DDSURFACEDESC2);
                desc.dwFlags = DDSD_CAPS;
                desc.ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_PRIMARYSURFACE;

                State.DX.Code = State.DX.Instance->CreateSurface(&desc, &State.DX.Surfaces.Main, NULL);

                if (State.DX.Surfaces.Main != NULL)
                {
                    State.DX.Surfaces.Main->QueryInterface(IID_IDirectDrawSurface4, (LPVOID*)&State.DX.Surfaces.Active[1]);
                }
            }
        }
        else
        {
            DDSURFACEDESC2 desc;
            ZeroMemory(&desc, sizeof(DDSURFACEDESC2));

            desc.dwSize = sizeof(DDSURFACEDESC2);
            desc.dwFlags = DDSD_CAPS;
            desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

            State.DX.Code = State.DX.Instance->CreateSurface(&desc, &State.DX.Surfaces.Main, NULL);

            if (State.DX.Code != DD_OK)
            {
                if (State.DX.Code == DDERR_OUTOFMEMORY || State.DX.Code == DDERR_OUTOFVIDEOMEMORY)
                {
                    Message("There was not enough video memory to create the rendering surface.\nTo run this program in a window of this size, please adjust your display settings for a smaller desktop area or a lower palette size and restart the program.");
                }
                else { Message("CreateSurface for window front buffer failed %8x.\n", State.DX.Code); }

                SetEvent(State.Mutex);

                *result = State.DX.Code;

                return RENDERER_MODULE_SUCCESS;
            }

            if (State.DX.Surfaces.Main != NULL)
            {
                State.DX.Surfaces.Main->QueryInterface(IID_IDirectDrawSurface2, (LPVOID*)&State.DX.Surfaces.Active[1]);
            }
        }

        State.DX.Surfaces.Active[2] = (IDirectDrawSurface4*)0x1234; // TODO

        SetEvent(State.Mutex);

        *result = State.DX.Code;

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60002b20
    u32 STDCALLAPI ReleaseRendererDeviceExecute(const void*, const HWND hwnd, const u32 msg, const u32 wp, const u32 lp, HRESULT* result)
    {
        ReleaseRendererDeviceSurfaces();

        State.Lambdas.Lambdas.SelectInstance(NULL);

        State.DX.Instance->Release();
        State.DX.Instance = NULL;

        SetEvent(State.Mutex);

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60002b60
    u32 ReleaseRendererWindow(void)
    {
        if (State.DX.Instance == NULL) { return RENDERER_MODULE_FAILURE; }

        SetForegroundWindow(State.Window.HWND);
        PostMessageA(State.Window.HWND, RENDERER_MODULE_WINDOW_MESSAGE_RELEASE_DEVICE, 0, 0);
        WaitForSingleObject(State.Mutex, INFINITE);
        CloseHandle(State.Mutex);

        State.Mutex = NULL;
        State.Window.HWND = NULL;

        return State.DX.Code;
    }

    // 0x60002bd0
    void ReleaseRendererDeviceSurfaces(void)
    {
        for (s32 x = (MAX_ACTIVE_SURFACE_COUNT - 1); x >= 0; x--)
        {
            if (State.DX.Surfaces.Active[x] != NULL)
            {
                if (State.DX.Surfaces.Active != (void*)0x1234) // TODO
                {
                    State.DX.Surfaces.Active[x]->Release();
                    State.DX.Surfaces.Active[x] = NULL;
                }
            }
        }

        if (State.DX.Surfaces.Back != NULL)
        {
            State.DX.Surfaces.Back->Release();
            State.DX.Surfaces.Back = NULL;
        }

        if (State.DX.Surfaces.Main != NULL)
        {
            State.DX.Surfaces.Main->Release();
            State.DX.Surfaces.Main = NULL;
        }
    }

    // 0x60002ef0
    u32 AcquirePixelFormat(const DDPIXELFORMAT* format)
    {
        const u32 bits = format->dwRGBBitCount;

        const u32 red = format->dwRBitMask;
        const u32 green = format->dwGBitMask;
        const u32 blue = format->dwBBitMask;

        if (bits == GRAPHICS_BITS_PER_PIXEL_16)
        {
            if (red == 0x7c00 && green == 0x3e0 && blue == 0x1f) { return RENDERER_PIXEL_FORMAT_R5G5B5; }
            else if (red == 0xf800 && green == 0x7e0 && blue == 0x1f) { return RENDERER_PIXEL_FORMAT_R5G6B5; }
            else if (red == 0xf00 && green == 0xf0 && blue == 0xf && format->dwRGBAlphaBitMask == 0xf000) { return RENDERER_PIXEL_FORMAT_R4G4B4; }
        }
        else if (red == 0xff0000 && green == 0xff00 && blue == 0xff)
        {
            if (bits == GRAPHICS_BITS_PER_PIXEL_24) { return RENDERER_PIXEL_FORMAT_R8G8B8; }
            else if (bits == GRAPHICS_BITS_PER_PIXEL_32) { return RENDERER_PIXEL_FORMAT_A8R8G8B8; }
        }

        return RENDERER_PIXEL_FORMAT_NONE;
    }

    // 0x60003f60
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

                return RENDERER_MODULE_SUCCESS;
            }
            case RENDERER_MODULE_CULL_CLOCK_WISE:
            {
                State.Settings.Cull = RENDERER_CULL_MODE_CLOCK_WISE;

                SelectRendererStateValue(state, (void*)RENDERER_MODULE_CULL_CLOCK_WISE);

                return RENDERER_MODULE_SUCCESS;
            }
            default: { return RENDERER_MODULE_FAILURE; }
            }
        }
        case RENDERER_MODULE_STATE_SELECT_WINDOW_MODE_STATE:
        case RENDERER_MODULE_STATE_SELECT_LAMBDAS:
        case RENDERER_MODULE_STATE_SELECT_WINDOW:
        case RENDERER_MODULE_STATE_SELECT_LOG_LAMBDA:
        case RENDERER_MODULE_STATE_SELECT_EXECUTE_LAMBDA:
        case RENDERER_MODULE_STATE_SELECT_LOCK_WINDOW_LAMBDA:
        case RENDERER_MODULE_STATE_SELECT_LINE_VERTEX_SIZE:
        case RENDERER_MODULE_STATE_SELECT_MEMORY_ALLOCATE_LAMBDA:
        case RENDERER_MODULE_STATE_SELECT_MEMORY_RELEASE_LAMBDA: { break; }
        case RENDERER_MODULE_STATE_SELECT_VERSION:
        {
            const u32 version = (u32)value;

            RendererVersion = (version < RENDERER_MODULE_VERSION_104 && RENDERER_MODULE_VERSION_107 < version)
                ? version : RENDERER_MODULE_VERSION_107;

            break;
        }
        case RENDERER_MODULE_STATE_SELECT_SELECT_STATE_LAMBDA: { State.Lambdas.SelectState = (RENDERERMODULESELECTSTATELAMBDA)value; break; }
        case RENDERER_MODULE_STATE_55: { DAT_60a2a5d0 = (u32)value; break; }
        case RENDERER_MODULE_STATE_SELECT_DISPLAY_STATE:
        case RENDERER_MODULE_STATE_SELECT_GAME_WINDOW_INDEX:
        case RENDERER_MODULE_STATE_SELECT_SHAMELESS_PLUG_STATE: { break; }
        case RENDERER_MODULE_STATE_62: { DAT_60a2a5d4 = (u32)value; break; }
        default: { return RENDERER_MODULE_FAILURE; }
        }

        SelectRendererStateValue(state, value);

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60004160
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
        SelectState(RENDERER_MODULE_STATE_SELECT_TEXTURE_MIP_FILTER_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_TEXTURE_MIP_FILTER_POINT, section, "MIPMAP"));
        SelectState(RENDERER_MODULE_STATE_SELECT_MATERIAL,
            (void*)AcquireSettingsValue(0x00000000, section, "BACKGROUNDCOLOUR"));
        SelectState(RENDERER_MODULE_STATE_SELECT_CHROMATIC_COLOR,
            (void*)AcquireSettingsValue(0x00000000, section, "CHROMACOLOUR"));
        SelectState(RENDERER_MODULE_STATE_SELECT_DITHER_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_DITHER_ACTIVE, section, "DITHER"));
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
        SelectState(RENDERER_MODULE_STATE_SELECT_BLEND_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_ALPHA_BLEND_SOURCE_INVERSE_SOURCE, section, "ALPHA"));
        SelectState(RENDERER_MODULE_STATE_SELECT_TEXTURE_ADDRESS_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_TEXTURE_ADDRESS_CLAMP, section, "TEXTURECLAMP"));

        {
            const f32 value = 1.0f;
            SelectState(RENDERER_MODULE_STATE_SELECT_GAMMA_CONTROL_STATE, (void*)(u32)(*(u32*)&value));
        }

        {
            const f32 value = 0.0f;
            SelectState(RENDERER_MODULE_STATE_SELECT_DEPTH_BIAS_STATE,
                (void*)AcquireSettingsValue((u32)(*(u32*)&value), section, "DEPTHBIAS"));
        }

        {
            const f32 value = 0.0f;
            SelectState(RENDERER_MODULE_STATE_SELECT_MIP_MAP_LOD_BIAS_STATE, (void*)(u32)(*(u32*)&value));
        }

        SelectState(RENDERER_MODULE_STATE_MAX_PENDING_STATE,
            (void*)AcquireSettingsValue(pending - 2U & ((s32)(pending - 2U) < 0) - 1, section, "MAXPENDING")); // TODO
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
        SelectState(RENDERER_MODULE_STATE_SELECT_SHAMELESS_PLUG_STATE,
            (void*)AcquireSettingsValue(0, section, "SHAMELESSPLUG")); // TODO

        {
            const u32 value = AcquireSettingsValue(((depth < 1) - 1) & 2, section, "DEPTHBUFFER");
            const u32 result = SelectState(RENDERER_MODULE_STATE_SELECT_DEPTH_STATE, (void*)value);

            if (result == RENDERER_MODULE_FAILURE && value == RENDERER_MODULE_DEPTH_ACTIVE_W)
            {
                SelectState(RENDERER_MODULE_STATE_SELECT_DEPTH_STATE, (void*)RENDERER_MODULE_DEPTH_ACTIVE);
            }
        }

        SelectState(RENDERER_MODULE_STATE_SELCT_DEPTH_FUNCTION,
            (void*)AcquireSettingsValue(RENDERER_MODULE_DEPTH_FUNCTION_LESS_EQUAL, section, "DEPTHCMP"));
    }

    // 0x60004510
    void FUN_60004510(const u32 mode)
    {
        if (RendererSurfaceStride != State.Renderer.Active.Stride)
        {
            RendererSurfaceStride = State.Renderer.Active.Stride;

            State.Renderer.Settings.Width = State.Renderer.Active.Width;
            State.Renderer.Settings.Height = State.Renderer.Active.Height;

            if (mode == RENDERER_MODULE_STATE_RENDERER_MODE_1)
            {
                TODO
            }
            else if (mode == RENDERER_MODULE_STATE_RENDERER_MODE_2)
            {
                TODO
            }
            else if (mode == RENDERER_MODULE_STATE_RENDERER_MODE_3)
            {

                TODO
            }
        }
    }

    // 0x600044d0
    void ModifyRendererSurface(void)
    {
        if (RendererSurfaceStride == State.Renderer.Active.Stride)
        {
            RendererSurfaceStride = RendererSurfaceStride * 2;

            State.Renderer.Settings.Width = State.Renderer.Settings.Width * 2;

            State.Renderer.Settings.Height = State.Renderer.Settings.Height / 2;
        }
    }

    // 0x60004700
    void AcquireRendererSurface(void* surface)
    {
        void* src = State.Renderer.Surface.Surface;
        void* dst = surface;

        for (u32 xx = 0; xx < State.Renderer.Settings.Height; xx++)
        {
            CopyMemory(dst, src, State.Renderer.Settings.Width * sizeof(u16));

            src = (void*)((addr)src + (addr)State.DX.Surfaces.Stride);
            dst = (void*)((addr)dst + (addr)RendererSurfaceStride);
        }
    }
}