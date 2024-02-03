/*
Copyright (c) 2023 - 2024 Americus Maximus

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

#include "DirectX.hxx"
#include "RendererModule.Export.hxx"

#define DEFAULT_DEVICE_INDEX 0
#define ENVIRONMENT_SECTION_NAME "DX8"
#define MAX_ACTIVE_SURFACE_COUNT 8
#define MAX_ACTIVE_UNKNOWN_COUNT 11
#define MAX_ACTIVE_USABLE_TEXTURE_FORMAT_COUNT 34
#define MAX_DEVICE_CAPABILITIES_COUNT 256
#define MAX_ENUMERATE_DEVICE_NAME_COUNT 60 /* ORIGINAL: 10 */
#define MAX_INPUT_FOG_ALPHA_COUNT 64
#define MAX_OUTPUT_FOG_ALPHA_COUNT 256
#define MAX_OUTPUT_FOG_ALPHA_VALUE 255
#define MAX_RENDER_PACKET_COUNT 10000
#define MAX_RENDERER_DEVICE_DEPTH_FORMAT_COUNT 5
#define MAX_RENDERER_DEVICE_FORMAT_COUNT 2
#define MAX_TEXTURE_FORMAT_COUNT 34
#define MAX_TEXTURE_STAGE_COUNT 8
#define MAX_TEXTURE_STATE_STATE_COUNT 120
#define MAX_VERTEX_BUFFER_SIZE 2621400
#define MIN_ACTUAL_DEVICE_CAPABILITIES_INDEX 8
#define MIN_SIMULTANEOUS_TEXTURE_COUNT 1
#define MIN_WINDOW_INDEX 8

namespace Renderer
{

}

namespace RendererModule
{
    extern u32 DAT_6001dfcc; // 0x6001dfcc
    extern u32 DAT_6001dfd0; // 0x6001dfd0

    struct MinMax
    {
        u32 Min;
        u32 Max;
    };

    struct TextureStageState
    {
        s32 Values[MAX_TEXTURE_STAGE_COUNT];
    };

    struct TextureStage
    {
        s32 Unk01; // TODO
        s32 Unk02; // TODO
        s32 Unk03; // TODO
        s32 Unk04; // TODO
        s32 Unk05; // TODO
        s32 Unk06; // TODO
        s32 Unk07; // TODO
        s32 Unk08; // TODO
        s32 Unk09; // TODO
        s32 Unk10; // TODO
        s32 Unk11; // TODO
        s32 Unk12; // TODO
    };

    struct RendererPacket
    {
        D3DPRIMITIVETYPE PrimitiveType;
        u32 PrimitiveCount;
        u32 StartIndex;
    };

    struct RendererModuleState
    {
        struct
        {
            BOOL IsInit; // 0x6001de5c

            IDirect3D8* Instance; // 0x6001de14
            IDirect3DDevice8* Device; // 0x6001de18

            struct
            {
                D3DGAMMARAMP Gamma; // 0x6001faa0
            } State;

            struct
            {
                u32 Width; // 0x6001dfb8
                u32 Height; // 0x6001dfbc

                u32 Bits; // 0x6001dfc0

                IDirect3DSurface8* Surfaces[MAX_ACTIVE_SURFACE_COUNT]; // 0x6001de20
            } Surfaces;
        } DX;

        struct
        {
            u32 MaxPrimitiveCount; // 0x6001daa4

            struct
            {
                u32 Count; // 0x6001daa0

                RendererPacket Packets[MAX_RENDER_PACKET_COUNT]; // 0x60021000
            } Packets;

            struct
            {
                u32 Count; // 0x6001dab0

                IDirect3DVertexBuffer8* Buffer; // 0x6001daa8

                u32 StartIndex; // 0x6001daac
            } Vertexes;
        } Data;

        struct
        {
            u32 Index; // 0x6001de58

            u32 SubType; // 0x6001dfb4

            D3DPRESENT_PARAMETERS Presentation; // 0x600200a0

            RendererModuleDeviceCapabilities8 Capabilities; // 0x6001f2c0
        } Device;

        struct
        {
            u32 Count; // 0x6001de54

            struct
            {
                char Names[MAX_ENUMERATE_DEVICE_NAME_COUNT][MAX_RENDERER_MODULE_DEVICE_LONG_NAME_LENGTH]; // 0x6001dad0
            } Enumeration;
        } Devices;

        struct
        {
            RENDERERMODULELOGLAMBDA Log; // 0x6001c108

            RENDERERMODULESELECTSTATELAMBDA SelectState; // 0x6001dfe8
            RENDERERMODULEALLOCATEMEMORYLAMBDA AllocateMemory; // 0x6001dfec
            RENDERERMODULERELEASEMEMORYLAMBDA ReleaseMemory; // 0x6001dff0

            RendererModuleLambdaContainer Lambdas; // 0x600200e0
        } Lambdas;

        struct
        {
            BOOL IsActive; // 0x6001da98
        } Lock;

        struct
        {
            HANDLE Device; // 0x6001de50

            HANDLE Surface; // 0x6001de60
        } Mutexes;

        struct
        {
            BOOL IsWindowMode; // 0x6001de44

            u32 Cull; // 0x6001dff4
        } Settings;

        struct
        {
            BOOL IsActive; // 0x6001da94
        } Scene;

        struct
        {
            D3DFORMAT Formats[MAX_TEXTURE_FORMAT_COUNT]; // 0x6001efa0

            TextureStage Stages[MAX_TEXTURE_STAGE_COUNT]; // 0x6001f140

            TextureStageState StageStates[MAX_TEXTURE_STATE_STATE_COUNT]; // 0x60020100
        } Textures;

        struct
        {
            u32 X0; // 0x6001da7c
            u32 Y0; // 0x6001da80
            u32 X1; // 0x6001da84
            u32 Y1; // 0x6001da88
        } ViewPort;

        struct
        {
            u32 Count; // 0x6001da90

            struct
            {
                HWND HWND; // 0x6001dfc8
            } Parent;

            HWND HWND; // 0x6001de40
        } Window;
    };

    extern RendererModuleState State;

    BOOL AcquireRendererDeviceCapabilities(void);
    BOOL AcquireRendererDeviceDepthFormat(const u32 device, const D3DFORMAT adapter, const D3DFORMAT target, D3DFORMAT* result);
    BOOL AcquireRendererDeviceDepthFormat(u32* bits, D3DFORMAT* result);
    BOOL AcquireRendererDeviceDepthWindowFormat(u32* width, u32* height, u32* bits, D3DFORMAT* format);
    BOOL BeginRendererScene(void);
    s32 AcquireSettingsValue(const s32 value, const char* section, const char* name);
    s32 AcquireTextureStateStageIndex(const u32 state);
    u32 AcquireRendererDeviceCount(void);
    u32 AcquireRendererDeviceFormat(const D3DFORMAT format);
    u32 AcquireRendererDeviceFormatSize(const D3DFORMAT format);
    u32 SelectBasicRendererState(const u32 state, void* value);
    u32 SelectRendererTransforms(const f32 zNear, const f32 zFar);
    u32 STDCALLAPI InitializeRendererDeviceExecute(const void*, const HWND hwnd, const u32 msg, const u32 wp, const u32 lp, HRESULT* result);
    u32 STDCALLAPI InitializeRendererDeviceSurfacesExecute(const void*, const HWND hwnd, const u32 msg, const u32 wp, const u32 lp, HRESULT* result);
    void AcquireRendererDeviceFormats(void);
    void AcquireRendererDeviceMemorySize(void);
    void AcquireRendererModuleDescriptor(RendererModuleDescriptor* desc, const char* section);
    void AcquireRendererTextureFormats(void);
    void AttemptRenderPackets(void);
    void InitializeRendererModuleState(const u32 mode, const u32 pending, const u32 depth, const char* section);
    void InitializeRendererState(void);
    void InitializeTextureStateStates(void);
    void InitializeVertexBuffer(void);
    void InitializeViewPort(void);
    void ReleaseRendererModule(void);
    void ReleaseRendererObjects(void);
    void ReleaseRendererWindows(void);
    void RenderPackets(void);
    void SelectRendererStateValue(const u32 state, void* value);
}