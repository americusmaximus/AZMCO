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

#define STATE_INITIAL (-1)
#define STATE_INACTIVE 0
#define STATE_ACTIVE 1

#define CLEAR_DEPTH_VALUE (1.0f)
#define DEFAULT_FOG_COLOR 0x00FF0000
#define DEFAULT_FOG_DINSITY (1.0f)
#define DEFAULT_FOG_END (1.0f)
#define DEFAULT_FOG_START (0.0f)
#define DXT_FORMAT_DXT1 1
#define DXT_FORMAT_DXT2 2
#define DXT_FORMAT_DXT3 3
#define DXT_FORMAT_NONE 0
#define ENVIRONMENT_SECTION_NAME "DX7"
#define INVALID_TEXTURE_FORMAT_COUNT (-1)
#define INVALID_TEXTURE_FORMAT_INDEX (-1)
#define LOCK_NONE 0
#define LOCK_READ 1
#define LOCK_WRITE 2
#define MAX_ACTIVE_SURFACE_COUNT 8
#define MAX_DEVICE_CAPABILITIES_COUNT 128 /* ORIGINAL: 100 */
#define MAX_ENUMERATE_DEVICE_COUNT 60 /* ORIGINAL: 10 */
#define MAX_ENUMERATE_DEVICE_NAME_COUNT 60 /* ORIGINAL: 10 */
#define MAX_LARGE_INDEX_COUNT 65536
#define MAX_OUTPUT_FOG_ALPHA_COUNT 256
#define MAX_OUTPUT_FOG_ALPHA_VALUE 255
#define MAX_TEXTURE_DEPTH_FORMAT_COUNT 16 /* ORIGINAL: 6 */
#define MAX_TEXTURE_FORMAT_COUNT 128 /* ORIGINAL: 32 */
#define MAX_TEXTURE_PALETTE_COLOR_COUNT 256
#define MAX_TEXTURE_STAGE_COUNT 8
#define MAX_TEXTURE_STATE_STATE_COUNT 120
#define MAX_USABLE_TEXTURE_FORMAT_COUNT 22
#define MAX_USABLE_TEXTURE_FORMAT_INDEX_COUNT 32
#define MAX_VERTEX_COUNT 32768
#define MAX_WINDOW_COUNT 65536
#define MIN_DEVICE_AVAIABLE_VIDEO_MEMORY (16 * 1024 * 1024) /* ORIGINAL: 0x200000 (2 MB) */
#define WINDOW_OFFSET 8

#if !defined(__WATCOMC__) && _MSC_VER <= 1200
inline void LOGERROR(...) { }
inline void LOGWARNING(...) { }
inline void LOGMESSAGE(...) { }
#else
#define LOGERROR(...) Message(RENDERER_MODULE_MESSAGE_SEVERITY_ERROR, __VA_ARGS__)
#define LOGWARNING(...) Message(RENDERER_MODULE_MESSAGE_SEVERITY_WARNING, __VA_ARGS__)
#define LOGMESSAGE(...) Message(RENDERER_MODULE_MESSAGE_SEVERITY_MESSAGE, __VA_ARGS__)
#endif

namespace Renderer
{
    struct RendererTexture
    {
        s32 Unk00; // TODO
        u32 Width;
        u32 Height;
        s32 FormatIndexValue; // TODO
        u32 Options;
        s32 MipMapCount;
        u32 Stage;
        RendererTexture* Previous;
        u32 UnknownFormatIndexValue; // TODO
        s32 FormatIndex; // TODO
        u32 MemoryType;
        BOOL Is16Bit;
        IDirectDrawSurface7* Surface;
        IDirectDrawSurface7* Texture;
        IDirectDrawPalette* Palette;
        DDSURFACEDESC2 Descriptor;
        u32 Colors;
    };
}

namespace RendererModule
{
    extern u32 DAT_600178a0; // 0x600178a0
    extern u32 DAT_60057e00; // 0x60057e00
    extern u32 DAT_60057e04; // 0x60057e04
    extern u32 DAT_600596b0; // 0x600596b0
    extern u32 DAT_600596bc; // 0x600596bc

    struct MinMax
    {
        u32 Min;
        u32 Max;
    };

    struct TextureStageState
    {
        s32 Values[MAX_TEXTURE_STAGE_COUNT];
    };

    struct TextureFormat
    {
        DDSURFACEDESC2 Descriptor;

        BOOL IsPalette;
        u32 RedBitCount;
        u32 BlueBitCount;
        u32 GreenBitCount;
        u32 PaletteColorBits;
        u32 AlphaBitCount;

        u32 DXTF; // Format
        u32 DXTT; // Type
        u32 DXTN; // Name
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

    struct TriangulationCapabilities
    {
        BOOL Unk01; // TODO
        BOOL Unk02; // TODO
        BOOL Unk03; // TODO
        BOOL Unk04; // TODO
        BOOL Unk05; // TODO
        BOOL Unk06; // TODO
        BOOL Unk07; // TODO
        BOOL Unk08; // TODO
        BOOL Unk09; // TODO
        BOOL Unk10; // TODO
        BOOL Unk11; // TODO
        BOOL Unk12; // TODO
        BOOL Unk13; // TODO
        BOOL Unk14; // TODO
        BOOL Unk15; // TODO
        BOOL Unk16; // TODO
    };

    struct RendererModuleWindow
    {
        Renderer::RendererTexture* Texture;
        IDirectDrawSurface7* Surface;
    };

    struct RendererModuleState
    {
        struct
        {
            HRESULT Code; // 0x600176d4

            IDirectDrawClipper* Clipper; // 0x600176ec
            IDirectDrawGammaControl* GammaControl; // 0x600176f0
            IDirectDraw7* Instance; // 0x600176f4

            IDirect3D7* DirectX; // 0x60057de8
            IDirect3DDevice7* Device; // 0x60057dec

            struct
            {
                BOOL IsInit; // 0x60057dc4
                BOOL IsActive; // 0x60059720
                BOOL IsSoft; // 0x60057dbc

                IDirectDraw7* Instance; // 0x60057dd0

                struct
                {
                    struct
                    {
                        IDirectDrawSurface7* Main; // 0x60057ddc
                        IDirectDrawSurface7* Back; // 0x60057de0
                    } Active;

                    IDirectDrawSurface7* Main; // 0x60057dd4
                    IDirectDrawSurface7* Back; // 0x60057dd8
                } Surfaces;
            } Active;

            struct
            {
                IDirectDrawSurface7* Main; // 0x600176f8
                IDirectDrawSurface7* Back; // 0x600176fc

                IDirectDrawSurface7* Active[MAX_ACTIVE_SURFACE_COUNT]; // 0x60017700

                IDirectDrawSurface7* Window; // 0x60017720

                IDirectDrawSurface7* Depth; // 0x60057de4
            } Surfaces;
        } DX;

        struct
        {
            struct
            {
                u32 Count; // 0x600578c8

                u16 Indexes[MAX_LARGE_INDEX_COUNT]; // 0x600378b8

                u16 Large[MAX_LARGE_INDEX_COUNT]; // 0x600597a0
            } Indexes;

            struct
            {
                u32 Count; // 0x600578c4

                u32 Vertexes[MAX_VERTEX_COUNT]; // 0x600178b8
            } Vertexes;
        } Data;

        struct
        {
            GUID* Identifier; // 0x6001772c

            struct
            {
                BOOL IsAccelerated; // 0x60059640
                u32 RendererDepthBits; // 0x60059644
                u32 RendererBits; // 0x60059648
                u32 RendererDeviceDepthBits; // 0x6005964c
                BOOL IsDepthVideoMemoryCapable; // 0x60059650
                BOOL IsDepthAvailable; // 0x60059654
                BOOL IsGreenAllowSixBits; // 0x60059658
                BOOL IsVideoMemoryCapable; // 0x6005965c
                BOOL IsDitherAvailable; // 0x60059660
                BOOL IsNoVerticalSyncAvailable; // 0x60059664
                BOOL IsWBufferAvailable; // 060059668
                BOOL IsWFogAvailable; // 0x6005966c
                BOOL IsWindowMode; // 0x60059670
                BOOL IsTrilinearInterpolationAvailable; // 0x60059674
                BOOL IsDepthBufferRemovalAvailable; // 0x60059678
                BOOL IsPerspectiveTextures; // 0x6005967c
                BOOL IsAlphaFlatBlending; // 0x60059680
                BOOL IsAlphaProperBlending; // 0x60059684
                BOOL IsAlphaTextures; // 0x60059688
                BOOL IsModulateBlending; // 0x6005968c
                BOOL IsSourceAlphaBlending; // 0x60059690
                u32 AntiAliasing; // 0x60059694
                BOOL IsColorBlending; // 0x60059698
                BOOL IsAnisotropyAvailable; // 0x6005969c
                BOOL IsGammaAvailable; // 0x600596a0
                BOOL IsSpecularGouraudBlending; // 0x600596a4
                BOOL IsStencilBufferAvailable; // 0x600596a8
                BOOL IsSpecularBlending; // 0x600596ac

                BOOL IsTextureIndependentUVs; // 0x600596b4
                BOOL IsMipMapBiasAvailable; // 0x600596b8

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
                BOOL IsAntiAliasEdges; // 0x600596e8
                f32 GuardBandLeft; // 0x600596ec
                f32 GuardBandRight; // 0x600596f0
                f32 GuardBandTop; // 0x600596f4
                f32 GuardBandBottom; // 0x600596f8
                f32 MaxTextureRepeat; // 0x600596fc
                BOOL IsAlphaComparisonAvailable; // 0x60059700
                u32 MaxAnisotropy; // 0x60059704

                u32 MaxActiveLights; // 0x60059724

                u32 MaxUserClipPlanes; // 0x6005972c
                u32 MaxVertexBlendMatrices; // 0x60059730
                BOOL IsTransformLightBufferSystemMemoryAvailable; // 0x60059734
                BOOL IsTransformLightBufferVideoMemoryAvailable; // 0x60059738

                TriangulationCapabilities Capabilities; // 0x60059740
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

                char Names[MAX_ENUMERATE_DEVICE_NAME_COUNT][MAX_RENDERER_MODULE_DEVICE_LONG_NAME_LENGTH]; // 0x60017390

                struct
                {
                    GUID* Indexes[MAX_ENUMERATE_DEVICE_COUNT]; // 0x60057e58
                    GUID Identifiers[MAX_ENUMERATE_DEVICE_COUNT]; // 0x60057e98
                } Identifiers;
            } Enumeration;
        } Devices;

        struct
        {
            RENDERERMODULELOGLAMBDA Log; // 0x60017180

            RENDERERMODULESELECTSTATELAMBDA SelectState; // 0x60057e1c
            RENDERERMODULEALLOCATEMEMORYLAMBDA AllocateMemory; // 0x60057e20
            RENDERERMODULERELEASEMEMORYLAMBDA ReleaseMemory; // 0x60057e24

            RendererModuleLambdaContainer Lambdas; // 0x6007a3c0
        } Lambdas;

        struct
        {
            BOOL IsActive; // 0x600172a8

            IDirectDrawSurface7* Surface; // 0x600172ac

            RendererModuleWindowLock State; // 0x60017290
        } Lock;

        HANDLE Mutex; // 0x60017728

        struct
        {
            BOOL IsActive; // 0x600172a4
        } Scene;

        struct
        {
            u32 Cull; // 0x60057e28

            BOOL IsWindowModeActive; // 0x600178b0
            BOOL IsToggleAllowed; // 0x600178b4

            u32 CooperativeLevel; // 0x600176d8
            BOOL IsWindowMode; // 0x600176dc

            u32 MaxAvailableMemory; // 0x600176e4

            u32 Acceleration; // 0x60017894
            u32 ClipPlaneState; // 0x60017898

            BOOL IsFogActive; // 0x600178a4

            DDGAMMARAMP GammaControl; // 0x60079da0

            RendererModuleWindowLock Lock; // 0x6007b320
        } Settings;

        struct
        {
            u32 Count; // 0x600578d4
            BOOL Illegal; // 0x600578d8

            Renderer::RendererTexture* Current; // 0x60057e38

            Renderer::RendererTexture* Recent; // 0x600178a8

            TextureStage Stages[MAX_TEXTURE_STAGE_COUNT]; // 0x600594a0

            TextureStageState StageStates[MAX_TEXTURE_STATE_STATE_COUNT]; // 0x6007a420

            struct
            {
                u32 Count; // 0x600593a0

                TextureFormat Formats[MAX_TEXTURE_FORMAT_COUNT]; // 0x60057fa0
                s32 Indexes[MAX_USABLE_TEXTURE_FORMAT_INDEX_COUNT]; // 0x600593c0

                struct
                {
                    u32 Count; // 0x6001773c

                    u32 Formats[MAX_TEXTURE_DEPTH_FORMAT_COUNT]; // 0x6007a3a0
                } Depth;
            } Formats;
        } Textures;

        struct
        {
            u32 X0; // 0x600172b0
            u32 Y0; // 0x600172b4
            u32 X1; // 0x600172b8
            u32 Y1; // 0x600172bc
        } ViewPort;

        struct
        {
            u32 Count; // 0x600172c4

            u32 Index; // 0x60017724

            struct
            {
                HWND HWND; // 0x60057dfc
            } Parent;

            HWND HWND; // 0x6007a408

            u32 Height; // 0x60059494

            u32 Width; // 0x60059620

            u32 Bits; // 0x60057dcc
        } Window;

        RendererModuleWindow Windows[MAX_WINDOW_COUNT]; // 0x6007b340
    };

    extern RendererModuleState State;

    void Message(const u32 severity, const char* format, ...);

    BOOL AcquireRendererDeviceAccelerationState(const u32 indx);
    BOOL AcquireRendererDeviceDepthBufferRemovalCapabilities(void);
    BOOL AcquireRendererDeviceState(void);
    BOOL AcquireRendererDeviceTrilinearInterpolationCapabilities(void);
    BOOL BeginRendererScene(void);
    BOOL CALLBACK EnumerateDirectDrawDevices(GUID* uid, LPSTR name, LPSTR description, LPVOID context);
    BOOL EndRendererScene(void);
    BOOL InitializeRendererDeviceDepthSurfaces(const u32 width, const u32 height, IDirectDrawSurface7* depth, IDirectDrawSurface7* surf);
    BOOL RenderPoints(Renderer::RVX* vertexes, const u32 count);
    BOOL RenderTriangleFans(Renderer::RVX* vertexes, const u32 vertexCount, const u32 indexCount, const u32* indexes);
    BOOL RenderTriangleStrips(Renderer::RVX* vertexes, const u32 vertexCount, const u32 indexCount, const u32* indexes);
    BOOL RestoreRendererSurfaces(void);
    BOOL SelectRendererState(const D3DRENDERSTATETYPE type, const DWORD value);
    BOOL SelectRendererTexture(Renderer::RendererTexture* tex);
    BOOL SelectRendererTextureStage(const u32 stage, const D3DTEXTURESTAGESTATETYPE type, const DWORD value);
    BOOL UpdateRendererTexture(Renderer::RendererTexture* tex, const u32* pixels);
    BOOL UpdateRendererTexture(Renderer::RendererTexture* tex, const u32* pixels, const u32* palette);
    BOOL UpdateRendererTexture(Renderer::RendererTexture* tex, const u32* pixels, const u32* palette, const u32 x, const u32 y, const u32 width, const u32 height, const u32 size);
    const char* AcquireRendererMessage(const HRESULT code);
    const char* AcquireRendererMessageDescription(const HRESULT code);
    HRESULT CALLBACK EnumerateDirectDrawAcceleratedDevices(LPSTR description, LPSTR name, LPD3DDEVICEDESC7 desc, LPVOID context);
    HRESULT CALLBACK EnumerateRendererDeviceDepthPixelFormats(LPDDPIXELFORMAT format, LPVOID ctx);
    HRESULT CALLBACK EnumerateRendererDeviceModes(LPDDSURFACEDESC2 desc, LPVOID context);
    HRESULT CALLBACK EnumerateRendererDevicePixelFormats(LPDDPIXELFORMAT format, LPVOID context);
    HRESULT CALLBACK EnumerateRendererDeviceTextureFormats(LPDDPIXELFORMAT format, LPVOID context);
    inline f32 AcquireNormal(const f32x3* a, const f32x3* b, const f32x3* c) { return (b->X - a->X) * (c->Y - a->Y) - (c->X - a->X) * (b->Y - a->Y); };
    Renderer::RendererTexture* AllocateRendererTexture(const u32 size);
    Renderer::RendererTexture* AllocateRendererTexture(const u32 width, const u32 height, const u32 format, const u32 options, const u32 state, const BOOL destination);
    Renderer::RendererTexture* InitializeRendererTexture(void);
    RendererModuleWindowLock* RendererLock(const u32 mode);
    s32 AcquireMinimumRendererDeviceResolutionModeIndex(const u32 width, const u32 height, const u32 bpp);
    s32 AcquireRendererDeviceResulutionModeScore(const RendererModuleDescriptorDeviceCapabilities* caps, const u32 width, const u32 height, const u32 bpp);
    s32 AcquireRendererDeviceTextureFormatIndex(const u32 palette, const u32 alpha, const u32 red, const u32 green, const u32 blue, const u32 dxtf, const u32 dxtt);
    s32 AcquireSettingsValue(const s32 value, const char* section, const char* name);
    s32 AcquireTextureStateStageIndex(const u32 state);
    s32 InitializeRendererTextureDetails(Renderer::RendererTexture* tex, const BOOL destination);
    u32 AcquireDirectDrawDeviceCount(GUID** uids, HMONITOR** monitors, const char* section);
    u32 AcquirePixelFormat(const DDPIXELFORMAT* format);
    u32 AcquireRendererDeviceCount(void);
    u32 ClearRendererViewPort(const u32 x0, const u32 y0, const u32 x1, const u32 y1, const BOOL window);
    u32 DisposeRendererTexture(Renderer::RendererTexture* tex);
    u32 InitializeRendererDevice(void);
    u32 InitializeRendererDeviceAcceleration(void);
    u32 InitializeRendererDeviceLambdas(void);
    u32 ReleaseRendererDeviceInstance(void);
    u32 ReleaseRendererWindow(void);
    u32 SelectBasicRendererState(const u32 state, void* value);
    u32 SelectRendererTransforms(const f32 zNear, const f32 zFar);
    u32 STDCALLAPI InitializeRendererDeviceExecute(const void*, const HWND hwnd, const u32 msg, const u32 wp, const u32 lp, HRESULT* result);
    u32 STDCALLAPI InitializeRendererDeviceSurfacesExecute(const void*, const HWND hwnd, const u32 msg, const u32 wp, const u32 lp, HRESULT* result);
    u32 STDCALLAPI ReleaseRendererDeviceExecute(const void*, const HWND hwnd, const u32 msg, const u32 wp, const u32 lp, HRESULT* result);
    u32 ToggleRenderer(void);
    void AcquireRendererDeviceTextureFormats(void);
    void AcquireRendererModuleDescriptor(RendererModuleDescriptor* desc, const char* section);
    void AcquireWindowModeCapabilities(void);
    void AssignRendererDeviceResolutionMode(const s32 src, const u32 dst);
    void AttemptRenderScene(void);
    void InitializeConcreteRendererDevice(void);
    void InitializeRendererModuleState(const u32 mode, const u32 pending, const u32 depth, const char* section);
    void InitializeRendererState(void);
    void InitializeRenderState55(void); // TODO
    void InitializeTextureStateStates(void);
    void InitializeViewPort(void);
    void ReleaseRendererDevice(void);
    void ReleaseRendererDeviceSurfaces(void);
    void ReleaseRendererTexture(Renderer::RendererTexture* tex);
    void ReleaseRendererWindows(void);
    void RendererRenderScene(void);
    void RenderLine(Renderer::RVX* a, Renderer::RVX* b);
    void RenderLineMesh(Renderer::RVX* vertexes, const u32* indexes, const u32 count);
    void RenderQuad(Renderer::RVX* a, Renderer::RVX* b, Renderer::RVX* c, Renderer::RVX* d);
    void RenderQuadMesh(Renderer::RVX* vertexes, const u32* indexes, const u32 count);
    void RenderTriangle(Renderer::RVX* a, Renderer::RVX* b, Renderer::RVX* c);
    void RenderTriangleMesh(Renderer::RVX* vertexes, const u32* indexes, const u32 count);
    void SelectRendererDevice(void);
    void SelectRendererDeviceType(const u32 type);
    void SelectRendererFogAlphas(const u8* input, u8* output);
    void SelectRendererMaterial(const u32 color);
    void SelectRendererStateValue(const u32 state, void* value);
    void SelectRendererVertexCount(void);
}