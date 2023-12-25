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

#ifdef __WATCOMC__
#include <RendererModule.Export.hxx>
#else
#include "RendererModule.Export.hxx"
#endif

#include "DirectDraw.hxx"

#define DEPTH_BIT_MASK_32_BIT 0x100
#define DEPTH_BIT_MASK_24_BIT 0x200
#define DEPTH_BIT_MASK_16_BIT 0x400
#define DEPTH_BIT_MASK_8_BIT 0x800

#define ENVIRONMENT_SECTION_NAME "DX7"
#define MAX_DEVICE_CAPABILITIES_COUNT 128 /* ORIGINAL: 100 */
#define MAX_ENUMERATE_DEVICE_COUNT 60 /* ORIGINAL: 10 */
#define MAX_ENUMERATE_DEVICE_NAME_COUNT 60 /* ORIGINAL: 10 */
#define MAX_ENUMERATE_DEVICE_NAME_LENGTH 80
#define MAX_TEXTURE_STAGE_COUNT 8
#define MAX_TEXTURE_STATE_STATE_COUNT 120
#define MAX_USABLE_TEXTURE_FORMAT_COUNT 22
#define STATE_ACTIVE 1
#define STATE_INACTIVE 0
#define STATE_INITIAL (-1)

namespace Renderer
{

}

namespace RendererModule
{
    struct TextureStageState
    {
        s32 Values[MAX_TEXTURE_STAGE_COUNT];
    };

    struct RendererModuleState
    {
        struct
        {
            struct
            {
                BOOL IsSoft; // 0x60057dbc
            } Active;
        } DX;

        struct
        {
            GUID* Identifier; // 0x6001772c

            struct
            {
                u32 MinTextureWidth; // 0x600596c0
                u32 MaxTextureWidth; // 0x600596c4
                u32 MinTextureHeight; // 0x600596c8
                u32 MaxTextureHeight; // 0x600596cc
                u32 MultipleTextureWidth; // 0x600596d0
                BOOL IsPowerOfTwoTexturesWidth; // 0x600596d4
                u32 MultipleTextureHeight; // 0x600596d8
                BOOL IsPowerOfTwoTexturesHeight; // 0x600596dc
                u32 MaximumSimultaneousTextures; // 0x600596e0
                BOOL IsSquareOnlyTextures; // 0x600596e4
            } Capabilities;
        } Device;

        struct
        {
            u32 Count; // 0x60017734

            GUID* Indexes[MAX_ENUMERATE_DEVICE_COUNT]; // 0x6007a3e0

            struct
            {
                u32 Count; // 0x60057e48
                BOOL IsAvailable; // 0x60057e4c

                char Names[MAX_ENUMERATE_DEVICE_NAME_COUNT][MAX_ENUMERATE_DEVICE_NAME_LENGTH]; // 0x60017390

                struct
                {
                    GUID* Indexes[MAX_ENUMERATE_DEVICE_COUNT]; // 0x60057e58
                    GUID Identifiers[MAX_ENUMERATE_DEVICE_COUNT]; // 0x60057e98
                } Identifiers;
            } Enumeration;
        } Devices;

        struct
        {
            RendererModuleLambdaContainer Lambdas; // 0x6007a3c0
        } Lambdas;

        struct
        {
            TextureStageState StageStates[MAX_TEXTURE_STATE_STATE_COUNT]; // 0x6007a420
        } Textures;

        struct
        {
            HWND HWND; // 0x6007a408
        } Window;
    };

    extern RendererModuleState State;

    BOOL AcquireRendererDeviceAccelerationState(const u32 indx);
    BOOL CALLBACK EnumerateDirectDrawDevices(GUID* uid, LPSTR name, LPSTR description, LPVOID context);
    s32 AcquireSettingsValue(const s32 value, const char* section, const char* name);
    u32 AcquireDirectDrawDeviceCount(GUID** uids, HMONITOR** monitors, const char* section);
    u32 AcquireRendererDeviceCount(void);
    void AcquireRendererModuleDescriptor(RendererModuleDescriptor* desc, const char* section);
    void InitializeTextureStateStates(void);
    void SelectRendererDevice(void);
}