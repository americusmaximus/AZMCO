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

#ifdef __WATCOMC__
#include <RendererModule.Export.hxx>
#else
#include "RendererModule.Export.hxx"
#endif

#include "DirectDraw.hxx"

#define DEFAULT_DEVICE_AVAIABLE_VIDEO_MEMORY (16 * 1024 * 1024) /* ORIGINAL: 0x200000 (2 MB) */
#define DEFAULT_RENDERER_MODE (-1)
#define DEFAULT_RENDERER_SURFACE_STRIDE (GRAPHICS_RESOLUTION_640 * sizeof(u16))
#define ENVIRONMENT_SECTION_NAME "SOFTTRI"
#define MAX_ACTIVE_SURFACE_COUNT 8
#define MAX_ACTIVE_UNKNOWN_COUNT 4
#define MAX_ACTIVE_USABLE_TEXTURE_FORMAT_COUNT 9
#define MAX_RENDERER_LAMBDAS_COUNT 13
#define MAX_RENDERER_MATERIAL_COLOR_COUNT 8
#define MAX_TEXTURE_STATE_STATE_COUNT 120
#define MAX_UNKNOWN_COLOR_ARAY_COUNT 16
#define MAX_UNKNOWN_COUNT (MAX_ACTIVE_UNKNOWN_COUNT + 2)
#define MAX_USABLE_TEXTURE_FORMAT_COUNT (MAX_ACTIVE_USABLE_TEXTURE_FORMAT_COUNT + 2)
#define MIN_DEVICE_AVAIABLE_VIDEO_MEMORY (16 * 1024 * 1024) /* ORIGINAL: 0x8000 (32 KB) */
#define RENDERER_SURFACE_ALIGNMENT_MASK 0xffffff00
#define RENDERER_SURFACE_POINTER_ALIGNEMENT_OFFSET 0x10
#define RENDERER_SURFACE_SIZE_DESCRIPTOR_OFFSET (256 - 16) /* RENDERER_SURFACE_SIZE_OFFSET - sizeof(RendererModuleSurfaceDescriptor) */
#define RENDERER_SURFACE_SIZE_MOFIFIER (256 + 16) /* RENDERER_SURFACE_SIZE_OFFSET + sizeof(RendererModuleSurfaceDescriptor) */
#define RENDERER_SURFACE_SIZE_OFFSET 256

#define RENDERER_CULL_MODE_CLOCK_WISE           0x00000000
#define RENDERER_CULL_MODE_NONE                 0x00000001
#define RENDERER_CULL_MODE_COUNTER_CLOCK_WISE   0x80000000

namespace Renderer
{

}

namespace RendererModule
{
    extern u32 DAT_60a2a5d0; // 0x60a2a5d0
    extern u32 DAT_60a2a5d4; // 0x60a2a5d4

    struct MinMax
    {
        u32 Min;
        u32 Max;
    };

    struct RendererModuleSurfaceDescriptor
    {
        u32 Unk00; // TODO
        u16 Width;
        u16 Height;
        u16 Unk03; // TODO
        u16 Unk04; // TODO
        u32 Unk05; // TODO
    };

    struct RendererModuleRendererLambdaContainer
    {
        void* Unk00;
        void* Unk01;
        void* Unk02;
        void* Unk03;
        void* Unk04;
        void* Unk05;
        void* Unk06;
        void* Unk07;
    };

    struct RendererModuleState
    {
        struct
        {
            GUID* ID; // 0x6002a588
            GUID Value; // 0x6002a528
        } Device;

        struct
        {
            u32 Bits; // 0x60a2c2e8

            HRESULT Code; // 0x6002a53c

            IDirectDraw4* Instance; // 0x6002a550

            struct
            {
                u32 Bits; // 0x60a2c2ec

                IDirectDrawSurface4* Main; // 0x6002a554
                IDirectDrawSurface4* Back; // 0x6002a558

                u32 Stride; // 0x60a2d420

                IDirectDrawSurface4* Active[MAX_ACTIVE_SURFACE_COUNT]; // 0x6002a55c

                IDirectDrawSurface4* Window; // 0x6002a57c
            } Surfaces;
        } DX;

        struct
        {
            RENDERERMODULESELECTSTATELAMBDA SelectState; // 0x60a2a5ec

            RendererModuleLambdaContainer Lambdas; // 0x60a2c2c0
        } Lambdas;

        struct
        {
            BOOL IsActive; // 0x6002a32c
            IDirectDrawSurface4* Surface; // 0x6002a330

            RendererModuleWindowLock State; // 0x6002a318
        } Lock;

        HANDLE Mutex; // 0x6002a580

        struct
        {
            struct
            {
                u32 Stride; // 0x6002a500
                u32 Length; // 0x6002a504
                u32 Width; // 0x6002a508
                u32 Height; // 0x6002a50c
                void* Surface; // 0x6002a510
            } Active;

            struct
            {
                u32* Unknown1; // 0x60a2d410
                u32* Unknown2; // 0x60a2d414
                u32* Unknown3; // 0x60a2d418
                u32* Unknown4; // 0x60a2d41c
            } Colors;

            struct
            {
                void* Allocated; // 0x6002a4fc
                void* Surface; // 0x6002a4f8

                u32 Mode; // 0x6002a520
            } Surface;

            struct
            {
                u32 Length; // 0x6002a4ec
                u32 Width; // 0x6002a4f0
                u32 Height; // 0x6002a4f4
            } Settings;

            // Contains doubled material color, example: (color << 16 | color)
            u32 Material; // 0x6002a514

            // Contains doubled material colors, example: (color << 16 | color)
            u32 Materials[MAX_RENDERER_MATERIAL_COLOR_COUNT]; // 0x60a2c4e0

            struct
            {
                u32 Index; // 0x60a2c298

                RendererModuleRendererLambdaContainer* Active; // 0x6002a518
                RendererModuleRendererLambdaContainer Lambdas[MAX_RENDERER_LAMBDAS_COUNT]; // 0x6002a34c
            } Lambdas;

            BOOL IsActive; // 0x6002a51c
        } Renderer;

        struct
        {
            BOOL IsAutoUnlock; // 0x6002a524

            u32 CooperativeLevel; // 0x6002a540
            BOOL IsWindowMode; // 0x6002a544

            u32 MaxAvailableMemory; // 0x6002a54c

            u32 Cull; // 0x60a2a5f8
        } Settings;

        struct
        {
            s32 StageStates[MAX_TEXTURE_STATE_STATE_COUNT]; // 0x60a2c300
        } Textures;

        struct
        {
            u32 X; // 0x6002a334
            u32 Y; // 0x6002a338

            u32 Left; // 0x6002a33c
            u32 Top; // 0x6002a340
            u32 Right; // 0x6002a344
            u32 Bottom; // 0x6002a348
        } ViewPort;

        struct
        {
            u32 Bits; // 0x60a2c2f0

            u32 Width; // 0x60a2c2e0
            u32 Height; // 0x60a2c2e4

            HWND HWND; // 0x6002a538
        } Window;
    };

    extern RendererModuleState State;

    void Message(const char* format, ...);

    BOOL CALLBACK EnumerateRendererDevices(GUID* uid, LPSTR name, LPSTR description, LPVOID context);
    HRESULT CALLBACK EnumerateRendererDeviceModes(LPDDSURFACEDESC2 desc, LPVOID context);
    s32 AcquireSettingsValue(const s32 value, const char* section, const char* name);
    s32 AcquireTextureStateStageIndex(const u32 state);
    u32 AcquireColorValue(const u32 color);
    u32 AcquirePixelFormat(const DDPIXELFORMAT* format);
    u32 InitializeRendererDeviceLambdas(void);
    u32 ReleaseRendererWindow(void);
    u32 RendererClearGameWindow(void);
    u32 SelectBasicRendererState(const u32 state, void* value);
    u32 STDCALLAPI InitializeRendererDeviceExecute(const void*, const HWND hwnd, const u32 msg, const u32 wp, const u32 lp, HRESULT* result);
    u32 STDCALLAPI InitializeRendererDeviceSurfacesExecute(const void*, const HWND hwnd, const u32 msg, const u32 wp, const u32 lp, HRESULT* result);
    u32 STDCALLAPI ReleaseRendererDeviceExecute(const void*, const HWND hwnd, const u32 msg, const u32 wp, const u32 lp, HRESULT* result);
    void AcquireRendererSurface(void* surface);
    void InitializeRendererModuleState(const u32 mode, const u32 pending, const u32 depth, const char* section);
    void InitializeRendererModuleSurfaceDescriptor(RendererModuleSurfaceDescriptor* self, const u32 width, const u32 height, const u32 bits);
    void ModifyRendererSurface(void);
    void ReleaseRendererDeviceSurfaces(void);
    void SelectIndexedRendererLambdas(void);
    void SelectMainRendererLambdas(void);
    void SelectRendererColorMasks(const u32 bits);
    void SelectRendererMaterial(const u32 color);
    void SelectRendererSettings(const u32 width, const u32 height, const u32 bits);
    void SelectRendererStateValue(const u32 state, void* value);
    void ToggleRenderer(void);
    void* AcquireRendererSurface(void);
}