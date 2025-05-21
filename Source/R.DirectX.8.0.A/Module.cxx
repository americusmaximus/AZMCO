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

#include "Graphics.Basic.hxx"
#include "Mathematics.Basic.hxx"
#include "Module.hxx"
#include "RendererValues.hxx"
#include "Settings.hxx"

using namespace Mathematics;
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

        ModuleDescriptor.Caps = RENDERER_MODULE_CAPS_UNKNOWN | RENDERER_MODULE_CAPS_WINDOWED | RENDERER_MODULE_CAPS_LINE_WIDTH
            | (((State.Device.Capabilities.IsPowerOfTwoTexturesHeight & 1) << 3) | ((State.Device.Capabilities.IsPowerOfTwoTexturesWidth & 1) << 2) | ((State.Device.Capabilities.IsSquareOnlyTextures & 1) << 1));

        ModuleDescriptor.MinimumTextureWidth = State.Device.Capabilities.MinTextureWidth;
        ModuleDescriptor.MaximumTextureWidth = State.Device.Capabilities.MaxTextureWidth;
        ModuleDescriptor.MultipleTextureWidth = State.Device.Capabilities.MultipleTextureWidth;
        ModuleDescriptor.MinimumTextureHeight = State.Device.Capabilities.MinTextureHeight;
        ModuleDescriptor.MaximumTextureHeight = State.Device.Capabilities.MaxTextureHeight;
        ModuleDescriptor.MultipleTextureHeight = State.Device.Capabilities.MultipleTextureHeight;
        ModuleDescriptor.MaximumSimultaneousTextures = State.Device.Capabilities.MaximumSimultaneousTextures;

        ModuleDescriptor.SubType = State.Device.SubType;

        ModuleDescriptor.Signature = RENDERER_MODULE_SIGNATURE_D3D8;

        ModuleDescriptor.Size = sizeof(RendererModuleDescriptor);
        ModuleDescriptor.ClipAlign = 0;
        ModuleDescriptor.DXV = RENDERER_MODULE_VERSION_DX8;
        ModuleDescriptor.Author = RENDERER_MODULE_AUTHOR;

        ModuleDescriptor.ActiveTextureFormatStatesCount = MAX_ACTIVE_USABLE_TEXTURE_FORMAT_COUNT;
        ModuleDescriptor.TextureFormatStates = RendererTextureFormatStates;

        ModuleDescriptor.ActiveUnknownValuesCount = MAX_ACTIVE_UNKNOWN_COUNT;
        ModuleDescriptor.UnknownValues = UnknownArray06;

        ModuleDescriptor.Capabilities.Capabilities = ModuleDescriptorDeviceCapabilities;

        strcpy(ModuleDescriptor.Name, RENDERER_MODULE_NAME);

        AcquireRendererModuleDescriptor(&ModuleDescriptor, ENVIRONMENT_SECTION_NAME);

        return &ModuleDescriptor;
    }
    
    // 0x60001390
    // a.k.a. THRASH_clearwindow
    DLLAPI u32 STDCALLAPI ClearGameWindow(void)
    {
        return ClearRendererViewPort(State.ViewPort.X0, State.ViewPort.Y0,
            State.ViewPort.X1 + State.ViewPort.X0, State.ViewPort.Y0 + State.ViewPort.Y1,
            State.Window.Surface == State.DX.Surfaces.Surfaces[3]); // TODO
    }

    // 0x600014c0
    // a.k.a. THRASH_clip
    DLLAPI u32 STDCALLAPI ClipGameWindow(const u32 x0, const u32 y0, const u32 x1, const u32 y1)
    {
        State.ViewPort.X0 = x0;
        State.ViewPort.Y0 = y0;
        State.ViewPort.X1 = x1 - x0;
        State.ViewPort.Y1 = y1 - y0;

        RenderPackets();
        AttemptRenderPackets();

        D3DVIEWPORT8 vp;
        ZeroMemory(&vp, sizeof(D3DVIEWPORT8));

        vp.X = x0;
        vp.Y = y0;
        vp.MinZ = 0.0f;
        vp.MaxZ = 0.9999847f;
        vp.Width = x1 - x0;
        vp.Height = y1 - y0;

        const HRESULT result = State.DX.Device->SetViewport(&vp);

        BeginRendererScene();

        return result == D3D_OK ? RENDERER_MODULE_SUCCESS : RENDERER_MODULE_FAILURE;
    }

    // 0x60001760
    // a.k.a. THRASH_createwindow
    // NOTE: Never being called by the application.
    DLLAPI u32 STDCALLAPI CreateGameWindow(const u32 width, const u32 height, const u32 format, const u32 options)
    {
        const u32 indx = State.Window.Count + MIN_WINDOW_INDEX;

        if (State.Device.Capabilities.Unk32 == 0) { return RENDERER_MODULE_FAILURE; }

        RendererTexture* surface = AllocateTexture(width, height, format, 0, 0);

        if (surface == NULL) { return RENDERER_MODULE_FAILURE; }

        State.Windows[indx].Surface = surface;

        RendererTexture* stencil = AllocateRendererDepthTexture(width, height, RENDERER_PIXEL_FORMAT_D16L, 0, 0, TRUE);

        if (stencil == NULL)
        {
            DestroyGameWindow(indx);

            return RENDERER_MODULE_FAILURE;
        }

        State.Windows[indx].Stencil = stencil;

        State.Window.Count = State.Window.Count + 1;

        return indx;
    }

    // 0x60001810
    // a.k.a. THRASH_destroywindow
    // NOTE: Never being called by the application.
    DLLAPI u32 STDCALLAPI DestroyGameWindow(const u32 indx)
    {
        if ((MIN_WINDOW_INDEX - 1) < indx && indx < (State.Window.Count + MIN_WINDOW_INDEX))
        {
            if (State.Windows[indx].Stencil != NULL) { ReleaseTexture(State.Windows[indx].Stencil); }
            if (State.Windows[indx].Surface != NULL) { ReleaseTexture(State.Windows[indx].Surface); }

            State.Windows[indx].Stencil = NULL;
            State.Windows[indx].Surface = NULL;
        }

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60002940
    // a.k.a. THRASH_drawline
    DLLAPI void STDCALLAPI DrawLine(RVX* a, RVX* b)
    {
        State.Data.Counters.Lines = State.Data.Counters.Lines + 1;

        if (IsNotEnoughRenderPackets(D3DPT_LINELIST, 2)) { RenderAllPackets(); }

        if (State.Data.Packets.Count == 0)
        {
            State.Data.Packets.Packets[1].PrimitiveType = D3DPT_LINELIST;
            State.Data.Packets.Packets[1].PrimitiveCount = 1;
            State.Data.Packets.Packets[1].StartIndex = 2;

            State.Data.Packets.Count = 1;
        }
        else if (State.Data.Packets.Packets[State.Data.Packets.Count].PrimitiveType == D3DPT_LINELIST)
        {
            State.Data.Packets.Packets[State.Data.Packets.Count].PrimitiveCount =
                State.Data.Packets.Packets[State.Data.Packets.Count].PrimitiveCount + 1;

            State.Data.Packets.Packets[State.Data.Packets.Count].StartIndex =
                State.Data.Packets.Packets[State.Data.Packets.Count].StartIndex + 2;
        }
        else
        {
            State.Data.Packets.Packets[State.Data.Packets.Count + 1].PrimitiveType = D3DPT_LINELIST;
            State.Data.Packets.Packets[State.Data.Packets.Count + 1].PrimitiveCount = 1;
            State.Data.Packets.Packets[State.Data.Packets.Count + 1].StartIndex = 2;

            State.Data.Packets.Count = State.Data.Packets.Count + 1;
        }

        RVX* buffer = NULL;
        State.Data.Vertexes.Buffer->Lock(State.Data.Vertexes.Count * RendererVertexSize,
            RendererVertexSize * 2, (BYTE**)&buffer, D3DLOCK_NOOVERWRITE);

        memcpy(buffer, a, RendererVertexSize);
        memcpy((void*)((addr)buffer + (addr)RendererVertexSize), b, RendererVertexSize);

        UpdateVertexValues((RTLVX2*)buffer);
        UpdateVertexValues((RTLVX2*)((addr)buffer + (addr)RendererVertexSize));

        State.Data.Vertexes.Buffer->Unlock();

        State.Data.Vertexes.Count = State.Data.Vertexes.Count + 2;
    }

    // 0x60002a70
    // a.k.a. THRASH_drawlinemesh
    DLLAPI void STDCALLAPI DrawLineMesh(const u32 count, RVX* vertexes, const u32* indexes)
    {
        if ((s32)count <= 0) { return; }

        State.Data.Counters.Lines = State.Data.Counters.Lines + count;

        u32 a = 0;
        u32 b = 0;

        for (u32 x = 0; x < count; x++)
        {
            void* indxs = (void*)((addr)indexes + (addr)(x * RendererIndexSize * 2));

            if (RendererIndexSize == RENDERER_MODULE_INDEX_SIZE_1)
            {
                a = ((u8*)indxs)[0];
                b = ((u8*)indxs)[1];
            }
            else if (RendererIndexSize == RENDERER_MODULE_INDEX_SIZE_2
                || RendererIndexSize != RENDERER_MODULE_INDEX_SIZE_4)
            {
                a = ((u16*)indxs)[0];
                b = ((u16*)indxs)[1];
            }
            else
            {
                a = ((u32*)indxs)[0];
                b = ((u32*)indxs)[1];
            }

            DrawLine((RVX*)((addr)vertexes + (addr)(a * RendererVertexSize)),
                (RVX*)((addr)vertexes + (addr)(b * RendererVertexSize)));
        }
    }

    // 0x60002e70
    // a.k.a. THRASH_drawlinestrip
    DLLAPI void STDCALLAPI DrawLineStrip(const u32 count, RVX* vertexes)
    {
        if ((s32)count <= 0) { return; }

        const RTLVX* vs = (RTLVX*)vertexes;

        for (u32 x = 0; x < count; x++) { DrawLine((RVX*)&vs[x + 0], (RVX*)&vs[x + 1]); }
    }

    // 0x60002af0
    // a.k.a. THRASH_drawlinestrip
    // NOTE: Never being called by the application.
    DLLAPI void STDCALLAPI DrawLineStrips(const u32 count, RVX* vertexes, const u32* indexes)
    {
        if ((s32)count <= 0) { return; }

        State.Data.Counters.Lines = State.Data.Counters.Lines + 1;

        u32 value = count & 0x80000001;
        BOOL allowed = value == 0;

        if ((s32)value < 0) { allowed = (value - 1 | 0xfffffffe) == 0xffffffff; }

        if (!allowed) { return; }

        if (IsNotEnoughRenderPackets(D3DPT_LINESTRIP, count + 1)) { RenderAllPackets(); }

        State.Data.Packets.Packets[State.Data.Packets.Count + 1].PrimitiveType = D3DPT_LINESTRIP;
        State.Data.Packets.Packets[State.Data.Packets.Count + 1].PrimitiveCount = count;
        State.Data.Packets.Packets[State.Data.Packets.Count + 1].StartIndex = count + 1;

        State.Data.Packets.Count = State.Data.Packets.Count + 1;

        RVX* buffer = NULL;
        State.Data.Vertexes.Buffer->Lock(State.Data.Vertexes.Count * RendererVertexSize,
            RendererVertexSize * (count + 1), (BYTE**)&buffer, D3DLOCK_NOOVERWRITE);

        for (u32 x = 0; x < (count + 1); x++)
        {
            u32 indx = 0;

            {
                void* indxs = (void*)((addr)indexes + (addr)(x * RendererIndexSize));

                if (RendererIndexSize == RENDERER_MODULE_INDEX_SIZE_1) { indx = ((u8*)indxs)[0]; }
                else if (RendererIndexSize == RENDERER_MODULE_INDEX_SIZE_2 || RendererIndexSize != RENDERER_MODULE_INDEX_SIZE_4) { indx = ((u16*)indxs)[0]; }
                else { indx = ((u32*)indxs)[0]; }
            }

            RVX* vertex = (RVX*)((addr)buffer + (addr)(indx * RendererVertexSize));

            memcpy(vertex, (RVX*)((addr)vertexes + (addr)(indx * RendererVertexSize)), RendererVertexSize);

            UpdateVertexValues((RTLVX2*)vertex);
        }

        State.Data.Vertexes.Buffer->Unlock();

        State.Data.Vertexes.Count = State.Data.Vertexes.Count + count + 1;
    }

    // 0x60002c40
    // a.k.a. THRASH_drawpoint
    DLLAPI void STDCALLAPI DrawPoint(RVX* vertex)
    {
        State.Data.Counters.Vertexes = State.Data.Counters.Vertexes + 1;

        if (IsNotEnoughRenderPackets(D3DPT_POINTLIST, 1)) { RenderAllPackets(); }

        if (State.Data.Packets.Count == 0)
        {
            State.Data.Packets.Packets[1].PrimitiveType = D3DPT_POINTLIST;
            State.Data.Packets.Packets[1].PrimitiveCount = 1;
            State.Data.Packets.Packets[1].StartIndex = 1;

            State.Data.Packets.Count = 1;
        }
        else if (State.Data.Packets.Packets[State.Data.Packets.Count].PrimitiveType == D3DPT_POINTLIST)
        {
            State.Data.Packets.Packets[State.Data.Packets.Count].PrimitiveCount =
                State.Data.Packets.Packets[State.Data.Packets.Count].PrimitiveCount + 1;

            State.Data.Packets.Packets[State.Data.Packets.Count].StartIndex =
                State.Data.Packets.Packets[State.Data.Packets.Count].StartIndex + 1;
        }
        else
        {
            State.Data.Packets.Packets[State.Data.Packets.Count + 1].PrimitiveType = D3DPT_POINTLIST;
            State.Data.Packets.Packets[State.Data.Packets.Count + 1].PrimitiveCount = 1;
            State.Data.Packets.Packets[State.Data.Packets.Count + 1].StartIndex = 1;

            State.Data.Packets.Count = State.Data.Packets.Count + 1;
        }

        RVX* buffer = NULL;
        State.Data.Vertexes.Buffer->Lock(State.Data.Vertexes.Count * RendererVertexSize,
            RendererVertexSize, (BYTE**)&buffer, D3DLOCK_NOOVERWRITE);

        memcpy(buffer, vertex, RendererVertexSize);

        UpdateVertexValues((RTLVX2*)buffer);

        State.Data.Vertexes.Buffer->Unlock();

        State.Data.Vertexes.Count = State.Data.Vertexes.Count + 1;
    }

    // 0x60002d40
    // a.k.a. THRASH_drawpointmesh
    DLLAPI void STDCALLAPI DrawPointMesh(const u32 count, RVX* vertexes, const u32* indexes)
    {
        if ((s32)count <= 0) { return; }

        State.Data.Counters.Vertexes = State.Data.Counters.Vertexes + count;

        for (u32 x = 0; x < count; x++)
        {
            u32 indx = 0;

            {
                void* indxs = (void*)((addr)indexes + (addr)(x * RendererIndexSize));

                if (RendererIndexSize == RENDERER_MODULE_INDEX_SIZE_1) { indx = ((u8*)indxs)[0]; }
                else if (RendererIndexSize == RENDERER_MODULE_INDEX_SIZE_2 || RendererIndexSize != RENDERER_MODULE_INDEX_SIZE_4) { indx = ((u16*)indxs)[0]; }
                else { indx = ((u32*)indxs)[0]; }
            }

            DrawPoint((RVX*)((addr)vertexes + (addr)(indx * RendererVertexSize)));
        }
    }

    // 0x60002ea0
    // a.k.a. THRASH_drawpointstrip
    DLLAPI void STDCALLAPI DrawPointStrip(const u32 count, RVX* vertexes)
    {
        if ((s32)count <= 0) { return; }

        const RTLVX* vs = (RTLVX*)vertexes;

        for (u32 x = 0; x < count; x++) { DrawPoint((RVX*)&vs[x + 0]); }
    }

    // 0x600026b0
    // a.k.a. THRASH_drawquad
    DLLAPI void STDCALLAPI DrawQuad(RVX* a, RVX* b, RVX* c, RVX* d)
    {
        State.Data.Counters.Quads = State.Data.Counters.Quads + 1;

        if (IsNotEnoughRenderPackets(D3DPT_TRIANGLELIST, 6)) { RenderAllPackets(); }

        if (State.Data.Packets.Count == 0)
        {
            State.Data.Packets.Packets[1].PrimitiveType = D3DPT_TRIANGLELIST;
            State.Data.Packets.Packets[1].PrimitiveCount = 2;
            State.Data.Packets.Packets[1].StartIndex = 6;

            State.Data.Packets.Count = 1;
        }
        else if (State.Data.Packets.Packets[State.Data.Packets.Count].PrimitiveType == D3DPT_TRIANGLELIST)
        {
            State.Data.Packets.Packets[State.Data.Packets.Count].PrimitiveCount =
                State.Data.Packets.Packets[State.Data.Packets.Count].PrimitiveCount + 2;

            State.Data.Packets.Packets[State.Data.Packets.Count].StartIndex =
                State.Data.Packets.Packets[State.Data.Packets.Count].StartIndex + 6;
        }
        else
        {
            State.Data.Packets.Packets[State.Data.Packets.Count + 1].PrimitiveType = D3DPT_TRIANGLELIST;
            State.Data.Packets.Packets[State.Data.Packets.Count + 1].PrimitiveCount = 2;
            State.Data.Packets.Packets[State.Data.Packets.Count + 1].StartIndex = 6;

            State.Data.Packets.Count = State.Data.Packets.Count + 1;
        }

        RVX* buffer = NULL;
        State.Data.Vertexes.Buffer->Lock(State.Data.Vertexes.Count * RendererVertexSize,
            RendererVertexSize * 4, (BYTE**)&buffer, D3DLOCK_NOOVERWRITE);

        // A
        {
            memcpy((void*)((addr)buffer + (addr)(RendererVertexSize * 0)), a, RendererVertexSize);
            UpdateVertexValues((RTLVX2*)((addr)buffer + (addr)(RendererVertexSize * 0)));
        }

        // B
        {
            memcpy((void*)((addr)buffer + (addr)(RendererVertexSize * 1)), b, RendererVertexSize);
            UpdateVertexValues((RTLVX2*)((addr)buffer + (addr)(RendererVertexSize * 1)));
        }

        // C
        {
            memcpy((void*)((addr)buffer + (addr)(RendererVertexSize * 2)), c, RendererVertexSize);
            UpdateVertexValues((RTLVX2*)((addr)buffer + (addr)(RendererVertexSize * 2)));
        }

        // A
        {
            memcpy((void*)((addr)buffer + (addr)(RendererVertexSize * 3)), a, RendererVertexSize);
            UpdateVertexValues((RTLVX2*)((addr)buffer + (addr)(RendererVertexSize * 3)));
        }

        // C
        {
            memcpy((void*)((addr)buffer + (addr)(RendererVertexSize * 4)), c, RendererVertexSize);
            UpdateVertexValues((RTLVX2*)((addr)buffer + (addr)(RendererVertexSize * 4)));
        }

        // D
        {
            memcpy((void*)((addr)buffer + (addr)(RendererVertexSize * 5)), d, RendererVertexSize);
            UpdateVertexValues((RTLVX2*)((addr)buffer + (addr)(RendererVertexSize * 5)));
        }

        State.Data.Vertexes.Buffer->Unlock();

        State.Data.Vertexes.Count = State.Data.Vertexes.Count + 6;
    }

    // 0x60002890
    // a.k.a. THRASH_drawquadmesh
    DLLAPI void STDCALLAPI DrawQuadMesh(const u32 count, RVX* vertexes, const u32* indexes)
    {
        if ((s32)count <= 0) { return; }

        State.Data.Counters.Quads = State.Data.Counters.Quads + count;

        u32 a = 0;
        u32 b = 0;
        u32 c = 0;
        u32 d = 0;

        for (u32 x = 0; x < count; x++)
        {
            void* indxs = (void*)((addr)indexes + (addr)(x * RendererIndexSize * 4));

            if (RendererIndexSize == RENDERER_MODULE_INDEX_SIZE_1)
            {
                a = ((u8*)indxs)[0];
                b = ((u8*)indxs)[1];
                c = ((u8*)indxs)[2];
                d = ((u8*)indxs)[3];
            }
            else if (RendererIndexSize == RENDERER_MODULE_INDEX_SIZE_2
                || RendererIndexSize != RENDERER_MODULE_INDEX_SIZE_4)
            {
                a = ((u16*)indxs)[0];
                b = ((u16*)indxs)[1];
                c = ((u16*)indxs)[2];
                d = ((u16*)indxs)[3];
            }
            else
            {
                a = ((u32*)indxs)[0];
                b = ((u32*)indxs)[1];
                c = ((u32*)indxs)[2];
                d = ((u32*)indxs)[3];
            }

            DrawQuad((RVX*)((addr)vertexes + (addr)(a * RendererVertexSize)),
                (RVX*)((addr)vertexes + (addr)(b * RendererVertexSize)),
                (RVX*)((addr)vertexes + (addr)(c * RendererVertexSize)),
                (RVX*)((addr)vertexes + (addr)(d * RendererVertexSize)));
        }
    }

    // 0x60001900
    // a.k.a. THRASH_drawsprite
    // NOTE: Never being called by the application.
    DLLAPI void STDCALLAPI DrawSprite(RVX* a, RVX* b)
    {
        RTLVX c;
        memcpy(&c, b, RendererVertexSize);

        c.XYZ.Y = ((RTLVX*)a)->XYZ.Y;
        c.UV.Y = ((RTLVX*)a)->UV.Y;

        RTLVX d;
        memcpy(&d, b, RendererVertexSize);

        d.XYZ.X = ((RTLVX*)a)->XYZ.X;
        d.UV.X = ((RTLVX*)a)->XYZ.X;

        DrawQuad(a, (RVX*)&c, b, (RVX*)&d);
    }

    // 0x60001980
    // a.k.a. THRASH_drawspritemesh
    // NOTE: Never being called by the application.
    DLLAPI void STDCALLAPI DrawSpriteMesh(const u32 count, RVX* vertexes, const u32* indexes)
    {
        if ((s32)count <= 0) { return; }

        u32 a = 0;
        u32 b = 0;

        for (u32 x = 0; x < count; x++)
        {
            void* indxs = (void*)((addr)indexes + (addr)(x * RendererIndexSize * 2));

            if (RendererIndexSize == RENDERER_MODULE_INDEX_SIZE_1)
            {
                a = ((u8*)indxs)[0];
                b = ((u8*)indxs)[1];
            }
            else if (RendererIndexSize == RENDERER_MODULE_INDEX_SIZE_2
                || RendererIndexSize != RENDERER_MODULE_INDEX_SIZE_4)
            {
                a = ((u16*)indxs)[0];
                b = ((u16*)indxs)[1];
            }
            else
            {
                a = ((u32*)indxs)[0];
                b = ((u32*)indxs)[1];
            }

            DrawSprite((RVX*)((addr)vertexes + (addr)(a * RendererVertexSize)),
                (RVX*)((addr)vertexes + (addr)(b * RendererVertexSize)));
        }
    }

    // 0x60002200
    // a.k.a. THRASH_drawtri
    DLLAPI void STDCALLAPI DrawTriangle(RVX* a, RVX* b, RVX* c)
    {
        if (State.Settings.Cull == RENDERER_CULL_MODE_NONE
            || (AcquireNormal((f32x3*)a, (f32x3*)b, (f32x3*)c) & RENDERER_CULL_MODE_COUNTER_CLOCK_WISE) != State.Settings.Cull)
        {
            State.Data.Counters.Triangles = State.Data.Counters.Triangles + 1;

            if (IsNotEnoughRenderPackets(D3DPT_TRIANGLELIST, 3)) { RenderAllPackets(); }

            if (State.Data.Packets.Count == 0)
            {
                State.Data.Packets.Packets[1].PrimitiveType = D3DPT_TRIANGLELIST;
                State.Data.Packets.Packets[1].PrimitiveCount = 1;
                State.Data.Packets.Packets[1].StartIndex = 3;

                State.Data.Packets.Count = 1;
            }
            else if (State.Data.Packets.Packets[State.Data.Packets.Count].PrimitiveType == D3DPT_TRIANGLELIST)
            {
                State.Data.Packets.Packets[State.Data.Packets.Count].PrimitiveCount =
                    State.Data.Packets.Packets[State.Data.Packets.Count].PrimitiveCount + 1;

                State.Data.Packets.Packets[State.Data.Packets.Count].StartIndex =
                    State.Data.Packets.Packets[State.Data.Packets.Count].StartIndex + 3;
            }
            else
            {
                State.Data.Packets.Packets[State.Data.Packets.Count + 1].PrimitiveType = D3DPT_TRIANGLELIST;
                State.Data.Packets.Packets[State.Data.Packets.Count + 1].PrimitiveCount = 1;
                State.Data.Packets.Packets[State.Data.Packets.Count + 1].StartIndex = 3;

                State.Data.Packets.Count = State.Data.Packets.Count + 1;
            }

            RVX* buffer = NULL;
            State.Data.Vertexes.Buffer->Lock(State.Data.Vertexes.Count * RendererVertexSize,
                RendererVertexSize * 3, (BYTE**)&buffer, D3DLOCK_NOOVERWRITE);

            // A
            {
                memcpy((void*)((addr)buffer + (addr)(RendererVertexSize * 0)), a, RendererVertexSize);
                UpdateVertexValues((RTLVX2*)((addr)buffer + (addr)(RendererVertexSize * 0)));
            }

            // B
            {
                memcpy((void*)((addr)buffer + (addr)(RendererVertexSize * 1)), b, RendererVertexSize);
                UpdateVertexValues((RTLVX2*)((addr)buffer + (addr)(RendererVertexSize * 1)));
            }

            // C
            {
                memcpy((void*)((addr)buffer + (addr)(RendererVertexSize * 2)), c, RendererVertexSize);
                UpdateVertexValues((RTLVX2*)((addr)buffer + (addr)(RendererVertexSize * 2)));
            }

            State.Data.Vertexes.Buffer->Unlock();

            State.Data.Vertexes.Count = State.Data.Vertexes.Count + 3;
        }
    }

    // 0x60002e40
    // a.k.a. THRASH_drawtrifan
    DLLAPI void STDCALLAPI DrawTriangleFan(const u32 count, RVX* vertexes)
    {
        if ((s32)count <= 0) { return; }

        const RTLVX* vs = (RTLVX*)vertexes;

        for (u32 x = 0; x < count; x++) { DrawTriangle(vertexes, (RVX*)&vs[x + 1], (RVX*)&vs[x + 2]); }
    }

    // 0x60002580
    // a.k.a. THRASH_drawtrifan
    DLLAPI void STDCALLAPI DrawTriangleFans(const u32 count, RVX* vertexes, const u32* indexes)
    {
        State.Data.Counters.TriangleFans = State.Data.Counters.TriangleFans + 1;

        if (IsNotEnoughRenderPackets(D3DPT_TRIANGLEFAN, count + 2)) { RenderAllPackets(); }

        State.Data.Packets.Packets[State.Data.Packets.Count + 1].PrimitiveType = D3DPT_TRIANGLEFAN;
        State.Data.Packets.Packets[State.Data.Packets.Count + 1].PrimitiveCount = count;
        State.Data.Packets.Packets[State.Data.Packets.Count + 1].StartIndex = count + 2;

        State.Data.Packets.Count = State.Data.Packets.Count + 1;

        RVX* buffer = NULL;
        State.Data.Vertexes.Buffer->Lock(State.Data.Vertexes.Count * RendererVertexSize,
            RendererVertexSize * (count + 2), (BYTE**)&buffer, D3DLOCK_NOOVERWRITE);

        for (u32 x = 0; x < (count + 2); x++)
        {
            u32 indx = 0;

            {
                void* indxs = (void*)((addr)indexes + (addr)(x * RendererIndexSize));

                if (RendererIndexSize == RENDERER_MODULE_INDEX_SIZE_1) { indx = ((u8*)indxs)[0]; }
                else if (RendererIndexSize == RENDERER_MODULE_INDEX_SIZE_2 || RendererIndexSize != RENDERER_MODULE_INDEX_SIZE_4) { indx = ((u16*)indxs)[0]; }
                else { indx = ((u32*)indxs)[0]; }
            }

            memcpy((void*)((addr)buffer + (addr)(x * RendererVertexSize)), (void*)((addr)vertexes + (addr)(indx * RendererVertexSize)), RendererVertexSize);
            UpdateVertexValues((RTLVX2*)((addr)buffer + (addr)(x * RendererVertexSize)));
        }

        State.Data.Vertexes.Buffer->Unlock();

        State.Data.Vertexes.Count = State.Data.Vertexes.Count + (count + 2);
    }

    // 0x600023b0
    // a.k.a. THRASH_drawtrimesh
    DLLAPI void STDCALLAPI DrawTriangleMesh(const u32 count, RVX* vertexes, const u32* indexes)
    {
        if ((s32)count <= 0) { return; }

        u32 a = 0;
        u32 b = 0;
        u32 c = 0;

        State.Data.Counters.Triangles = State.Data.Counters.Triangles + count;

        for (u32 x = 0; x < count; x++)
        {
            void* indxs = (void*)((addr)indexes + (addr)(x * RendererIndexSize * 3));

            if (RendererIndexSize == RENDERER_MODULE_INDEX_SIZE_1)
            {
                a = ((u8*)indxs)[0];
                b = ((u8*)indxs)[1];
                c = ((u8*)indxs)[2];
            }
            else if (RendererIndexSize == RENDERER_MODULE_INDEX_SIZE_2
                || RendererIndexSize != RENDERER_MODULE_INDEX_SIZE_4)
            {
                a = ((u16*)indxs)[0];
                b = ((u16*)indxs)[1];
                c = ((u16*)indxs)[2];
            }
            else
            {
                a = ((u32*)indxs)[0];
                b = ((u32*)indxs)[1];
                c = ((u32*)indxs)[2];
            }

            DrawTriangle((RVX*)((addr)vertexes + (addr)(a * RendererVertexSize)),
                (RVX*)((addr)vertexes + (addr)(b * RendererVertexSize)),
                (RVX*)((addr)vertexes + (addr)(c * RendererVertexSize)));
        }
    }

    // 0x60002df0
    // a.k.a. THRASH_drawtristrip
    DLLAPI void STDCALLAPI DrawTriangleStrip(const u32 count, RVX* vertexes)
    {
        if ((s32)count <= 0) { return; }

        const RTLVX* vs = (RTLVX*)vertexes;

        DrawTriangle((RVX*)&vs[0], (RVX*)&vs[1], (RVX*)&vs[2]);

        for (u32 x = 1; x < count; x = x + 2)
        {
            DrawTriangle((RVX*)&vs[x + 0], (RVX*)&vs[x + 2], (RVX*)&vs[x + 1]);

            if ((x + 1) < count) { DrawTriangle((RVX*)&vs[x + 1], (RVX*)&vs[x + 2], (RVX*)&vs[x + 3]); }
        }
    }

    // 0x60002450
    // a.k.a. THRASH_drawtristrip
    DLLAPI void STDCALLAPI DrawTriangleStrips(const u32 count, RVX* vertexes, const u32* indexes)
    {
        State.Data.Counters.TriangleStrips = State.Data.Counters.TriangleStrips + 1;

        if (IsNotEnoughRenderPackets(D3DPT_TRIANGLESTRIP, count + 2)) { RenderAllPackets(); }

        State.Data.Packets.Packets[State.Data.Packets.Count + 1].PrimitiveType = D3DPT_TRIANGLESTRIP;
        State.Data.Packets.Packets[State.Data.Packets.Count + 1].PrimitiveCount = count;
        State.Data.Packets.Packets[State.Data.Packets.Count + 1].StartIndex = count + 2;

        State.Data.Packets.Count = State.Data.Packets.Count + 1;

        RVX* buffer = NULL;
        State.Data.Vertexes.Buffer->Lock(State.Data.Vertexes.Count * RendererVertexSize,
            RendererVertexSize * (count + 2), (BYTE**)&buffer, D3DLOCK_NOOVERWRITE);

        for (u32 x = 0; x < (count + 2); x++)
        {
            u32 indx = 0;

            {
                void* indxs = (void*)((addr)indexes + (addr)(x * RendererIndexSize));

                if (RendererIndexSize == RENDERER_MODULE_INDEX_SIZE_1) { indx = ((u8*)indxs)[0]; }
                else if (RendererIndexSize == RENDERER_MODULE_INDEX_SIZE_2 || RendererIndexSize != RENDERER_MODULE_INDEX_SIZE_4) { indx = ((u16*)indxs)[0]; }
                else { indx = ((u32*)indxs)[0]; }
            }

            memcpy((void*)((addr)buffer + (addr)(x * RendererVertexSize)), (void*)((addr)vertexes + (addr)(indx * RendererVertexSize)), RendererVertexSize);
            UpdateVertexValues((RTLVX2*)((addr)buffer + (addr)(x * RendererVertexSize)));
        }

        State.Data.Vertexes.Buffer->Unlock();

        State.Data.Vertexes.Count = State.Data.Vertexes.Count + (count + 2);
    }

    // 0x600013d0
    // a.k.a. THRASH_flushwindow
    DLLAPI u32 STDCALLAPI FlushGameWindow(void)
    {
        return AttemptRenderPackets();
    }
    
    // 0x60002f30
    // a.k.a. THRASH_getstate
    DLLAPI u32 STDCALLAPI AcquireState(const u32 state)
    {
        const s32 indx = AcquireTextureStateStageIndex(state & RENDERER_MODULE_SELECT_STATE_MASK);

        return  (-1 < indx) ? State.Textures.StageStates[indx].Values[MAKETEXTURESTAGEVALUE(state)] : indx;
    }

    // 0x600017f0
    // a.k.a. THRASH_getwindowtexture
    DLLAPI RendererTexture* STDCALLAPI AcquireGameWindowTexture(const u32 indx)
    {
        return ((MIN_WINDOW_INDEX - 1) < indx && indx < MAX_WINDOW_TEXTURE_COUNT) ? State.Windows[indx].Surface : NULL;
    }

    // 0x60008750
    // a.k.a. THRASH_idle
    DLLAPI void STDCALLAPI Idle(void) { return; }

    // 0x600030f0
    // a.k.a. THRASH_init
    DLLAPI u32 STDCALLAPI Init(void)
    {
        InitializeSettings();

        if (State.DX.Instance == NULL)
        {
            State.DX.Instance = Direct3DCreate8(D3D_SDK_VERSION);

            if (State.DX.Instance == NULL) { return RENDERER_MODULE_FAILURE; }
        }

        AcquireRendererDeviceCount();

        AcquireRendererDeviceFormats();

        InitializeTextureStateStates();

        if (State.Lambdas.Lambdas.Execute != NULL)
        {
            State.Lambdas.Lambdas.Execute(RENDERER_MODULE_WINDOW_MESSAGE_INITIALIZE_DEVICE, (RENDERERMODULEEXECUTECALLBACK)InitializeRendererDeviceExecute);
            State.Lambdas.Lambdas.Execute(RENDERER_MODULE_WINDOW_MESSAGE_INITIALIZE_SURFACES, (RENDERERMODULEEXECUTECALLBACK)InitializeRendererDeviceSurfacesExecute);
        }

        atexit(ReleaseRendererModule);

        for (u32 x = 0; x < MAX_OUTPUT_FOG_ALPHA_COUNT; x++)
        {
            RendererFogAlphas[x] = MAX_OUTPUT_FOG_ALPHA_VALUE - x;
        }

        State.DX.IsInit = TRUE;

        return State.Devices.Count;
    }

    // 0x60004a70
    // a.k.a. THRASH_is
    DLLAPI u32 STDCALLAPI Is(void)
    {
        return RENDERER_MODULE_DX8_ACCELERATION_AVAILABLE;
    }

    // 0x60001590
    // a.k.a. THRASH_lockwindow
    DLLAPI RendererModuleWindowLock* STDCALLAPI LockGameWindow(void)
    {
        if (State.Window.Surface == NULL || State.Window.Index == 1
            || State.Window.Index == 3 || MIN_WINDOW_INDEX <= State.Window.Index) // TODO
        {
            return NULL;
        }

        if (State.Lock.IsActive) { UnlockGameWindow(NULL); }

        if (State.Scene.IsActive) { AttemptRenderPackets(); }

        if (State.Lambdas.Lambdas.LockWindow != NULL) { State.Lambdas.Lambdas.LockWindow(TRUE); }

        IDirect3DSurface8* surface = (State.Window.Index < MIN_WINDOW_INDEX)
            ? State.DX.Surfaces.Surfaces[State.Window.Index]
            : State.Windows[State.Window.Index].Surface->Surface;

        if (surface == NULL) { return NULL; }

        D3DLOCKED_RECT rect;
        ZeroMemory(&rect, sizeof(D3DLOCKED_RECT));

        if (surface->LockRect(&rect, NULL, D3DLOCK_NOSYSLOCK) == D3D_OK)
        {
            D3DSURFACE_DESC desc;
            ZeroMemory(&desc, sizeof(D3DSURFACE_DESC));

            surface->GetDesc(&desc);

            const u32 bits = AcquireRendererDeviceFormatSize(desc.Format);

            switch (bits)
            {
            case GRAPHICS_BITS_PER_PIXEL_16:
            {
                State.Lock.Surface = surface;

                State.Lock.State.Data = rect.pBits;
                State.Lock.State.Stride = rect.Pitch;
                State.Lock.State.Format = RENDERER_PIXEL_FORMAT_R5G6B5;
                State.Lock.State.Width = desc.Width;
                State.Lock.State.Height = desc.Height;

                State.Lock.IsActive = TRUE;

                return &State.Lock.State;
            }
            case GRAPHICS_BITS_PER_PIXEL_24:
            {
                State.Lock.Surface = surface;

                State.Lock.State.Data = rect.pBits;
                State.Lock.State.Stride = rect.Pitch;
                State.Lock.State.Format = RENDERER_PIXEL_FORMAT_R8G8B8;
                State.Lock.State.Width = desc.Width;
                State.Lock.State.Height = desc.Height;

                State.Lock.IsActive = TRUE;

                return &State.Lock.State;
            }
            case GRAPHICS_BITS_PER_PIXEL_32:
            {
                State.Lock.Surface = surface;

                State.Lock.State.Data = rect.pBits;
                State.Lock.State.Stride = rect.Pitch;
                State.Lock.State.Format = RENDERER_PIXEL_FORMAT_A8R8G8B8;
                State.Lock.State.Width = desc.Width;
                State.Lock.State.Height = desc.Height;

                State.Lock.IsActive = TRUE;

                return &State.Lock.State;
            }
            }

            surface->UnlockRect();
        }

        if (State.Lambdas.Lambdas.LockWindow != NULL) { State.Lambdas.Lambdas.LockWindow(FALSE); }

        return NULL;
    }

    // 0x600013e0
    // a.k.a. THRASH_pageflip
    DLLAPI void STDCALLAPI ToggleGameWindow(void)
    {
        Idle();
        FlushGameWindow();

        if (State.Lock.IsActive) { LOGERROR("pageflip called in while locked\n"); }

        ToggleRenderer();

        State.Data.Counters.Triangles = 0;
        State.Data.Counters.Quads = 0;
        State.Data.Counters.Lines = 0;
        State.Data.Counters.TriangleStrips = 0;
        State.Data.Counters.TriangleFans = 0;
        State.Data.Counters.Vertexes = 0;

        BeginRendererScene();
    }

    // 0x60002ed0
    // a.k.a. THRASH_readrect
    DLLAPI u32 STDCALLAPI ReadRectangle(const u32 x, const u32 y, const u32 width, const u32 height, u32* pixels)
    {
        return ReadRectangles(x, y, width, height, pixels, 0);
    }

    // 0x60004a80
    // a.k.a. THRASH_readrect
    // NOTE: Never being called by the application.
    DLLAPI u32 STDCALLAPI ReadRectangles(const u32 x, const u32 y, const u32 width, const u32 height, u32* pixels, const u32 stride)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60003380
    // a.k.a. THRASH_restore
    DLLAPI u32 STDCALLAPI RestoreGameWindow(void)
    {
        if (!State.DX.IsInit) { return RENDERER_MODULE_FAILURE; }

        HWND window = NULL;

        if (State.Lambdas.Lambdas.Execute != NULL)
        {
            if (GetWindowThreadProcessId(State.Window.Parent.HWND, NULL) != GetCurrentThreadId())
            {
                window = State.Lambdas.Lambdas.AcquireWindow();

                State.Mutexes.Surface = CreateEventA(NULL, FALSE, FALSE, NULL);

                SetForegroundWindow(window);
                PostMessageA(window, RENDERER_MODULE_WINDOW_MESSAGE_INITIALIZE_SURFACES, 0, 0);

                if (WaitForSingleObject(State.Mutexes.Surface, 10000) == WAIT_OBJECT_0)
                {
                    State.DX.IsInit = FALSE;

                    return RENDERER_MODULE_SUCCESS;
                }
            }
        }

        InitializeRendererDeviceSurfacesExecute(0, window, RENDERER_MODULE_WINDOW_MESSAGE_INITIALIZE_DEVICE, 0, 0, NULL);

        State.DX.IsInit = FALSE;

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60003180
    // a.k.a. THRASH_selectdisplay
    DLLAPI u32 STDCALLAPI SelectDevice(const s32 indx)
    {
        if (!State.Device.IsInit)
        {
            s32 actual = indx;

            if (State.Lambdas.Lambdas.AcquireWindow != NULL || State.Window.HWND != NULL)
            {
                const char* value = getenv(RENDERER_MODULE_DISPLAY_ENVIRONMENT_PROPERTY_NAME);

                if (value != NULL) { actual = atoi(value); }
            }

            State.Device.Index = DEFAULT_DEVICE_INDEX;

            if ((DEFAULT_DEVICE_INDEX - 1) < indx && indx < State.Devices.Count) { State.Device.Index = indx; }

            strncpy(ModuleDescriptor.DeviceName, State.Devices.Enumeration.Names[indx], MAX_RENDERER_MODULE_DEVICE_LONG_NAME_LENGTH);

            State.Device.IsInit = TRUE;
        }

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60004bc0
    // a.k.a. THRASH_setstate
    DLLAPI u32 STDCALLAPI SelectState(const u32 state, void* value)
    {
        const u32 actual = state & RENDERER_MODULE_SELECT_STATE_MASK;
        const u32 stage = MAKETEXTURESTAGEVALUE(state);

        u32 result = RENDERER_MODULE_FAILURE;

        switch (actual)
        {
        case RENDERER_MODULE_STATE_NONE:
        case RENDERER_MODULE_STATE_49:
        case RENDERER_MODULE_STATE_50:
        case RENDERER_MODULE_STATE_51: { return RENDERER_MODULE_FAILURE; }
        case RENDERER_MODULE_STATE_SELECT_TEXTURE:
        {
            SelectTexture((RendererTexture*)value);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_CULL_STATE:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_CULL_NONE:
            {
                SelectRendererState(D3DRS_CULLMODE, D3DCULL_NONE);

                State.Settings.Cull = RENDERER_CULL_MODE_NONE;

                break;
            }
            case RENDERER_MODULE_CULL_COUNTER_CLOCK_WISE:
            {
                SelectRendererState(D3DRS_CULLMODE, D3DCULL_CCW);

                State.Settings.Cull = RENDERER_CULL_MODE_COUNTER_CLOCK_WISE;

                break;
            }
            case RENDERER_MODULE_CULL_CLOCK_WISE:
            {
                SelectRendererState(D3DRS_CULLMODE, D3DCULL_CW);

                State.Settings.Cull = RENDERER_CULL_MODE_CLOCK_WISE;

                break;
            }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_MATERIAL:
        {
            SelectRendererMaterial((u32)value);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_DEPTH_STATE:
        {
            if (!State.Device.Capabilities.IsDepthAvailable) { return RENDERER_MODULE_FAILURE; }
            if (State.DX.Surfaces.Bits == 0) { return RENDERER_MODULE_FAILURE; }

            switch ((u32)value)
            {
            case RENDERER_MODULE_DEPTH_INACTIVE:
            {
                SelectRendererState(D3DRS_ZWRITEENABLE, FALSE);
                SelectRendererState(D3DRS_ZENABLE, D3DZB_FALSE);
                SelectRendererState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

                break;
            }
            case RENDERER_MODULE_DEPTH_ACTIVE:
            {
                SelectRendererState(D3DRS_ZWRITEENABLE, TRUE);
                SelectRendererState(D3DRS_ZENABLE, D3DZB_TRUE);
                SelectRendererState(D3DRS_ZFUNC, RendererDepthFunction);

                break;
            }
            case RENDERER_MODULE_DEPTH_ACTIVE_W:
            {
                if (State.Device.Capabilities.IsWBufferAvailable)
                {
                    SelectRendererState(D3DRS_ZENABLE, D3DZB_TRUE);
                    SelectRendererState(D3DRS_ZWRITEENABLE, TRUE);
                    SelectRendererState(D3DRS_ZFUNC, RendererDepthFunction);
                    SelectRendererState(D3DRS_ZENABLE, D3DZB_USEW);
                }
                else
                {
                    SelectRendererState(D3DRS_ZWRITEENABLE, TRUE);
                    SelectRendererState(D3DRS_ZENABLE, D3DZB_TRUE);
                    SelectRendererState(D3DRS_ZFUNC, RendererDepthFunction);

                    return RENDERER_MODULE_FAILURE;
                }

                break;
            }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_DITHER_STATE:
        {
            SelectRendererState(D3DRS_DITHERENABLE, ((u32)value) != 0 ? TRUE : FALSE);

            if (!State.Device.Capabilities.IsDitherAvailable) { return RENDERER_MODULE_FAILURE; }

            result = State.Device.Capabilities.IsDitherAvailable; break;
        }
        case RENDERER_MODULE_STATE_SELECT_SHADE_STATE:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_SHADE_FLAT:
            case RENDERER_MODULE_SHADE_GOURAUD:
            {
                if ((u32)value == RENDERER_MODULE_SHADE_FLAT && SettingsState.FlatShading)
                {
                    SelectRendererState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
                    SelectRendererState(D3DRS_SPECULARENABLE, FALSE);

                    RendererShadeMode = RENDERER_MODULE_SHADE_FLAT;
                }
                else
                {
                    SelectRendererState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
                    SelectRendererState(D3DRS_SPECULARENABLE, FALSE);

                    RendererShadeMode = RENDERER_MODULE_SHADE_GOURAUD;
                }

                break;
            }
            case RENDERER_MODULE_SHADE_GOURAUD_SPECULAR:
            {
                if (!State.Device.Capabilities.IsSpecularGouraudBlending) { return RENDERER_MODULE_FAILURE; }

                SelectRendererState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
                SelectRendererState(D3DRS_SPECULARENABLE, TRUE);

                RendererShadeMode = RENDERER_MODULE_SHADE_GOURAUD;

                break;
            }
            case RENDERER_MODULE_SHADE_3: { break; }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_TEXTURE_FILTER_STATE:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_TEXTURE_FILTER_POINT:
            {
                SelectRendererTextureStage(stage, D3DTSS_MAGFILTER, D3DTEXF_POINT);
                SelectRendererTextureStage(stage, D3DTSS_MINFILTER, D3DTEXF_POINT);

                break;
            }
            case RENDERER_MODULE_TEXTURE_FILTER_LENEAR:
            {
                SelectRendererTextureStage(stage, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
                SelectRendererTextureStage(stage, D3DTSS_MINFILTER, D3DTEXF_LINEAR);

                break;
            }
            case RENDERER_MODULE_TEXTURE_FILTER_ANISOTROPY:
            {
                if (!State.Device.Capabilities.IsAnisotropyAvailable) { return RENDERER_MODULE_FAILURE; }

                SelectRendererTextureStage(stage, D3DTSS_MAGFILTER, D3DTEXF_ANISOTROPIC);
                SelectRendererTextureStage(stage, D3DTSS_MINFILTER, D3DTEXF_ANISOTROPIC);

                break;
            }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_MIP_MAP_LOD_BIAS_STATE:
        {
            if (!State.Device.Capabilities.IsMipMapBiasAvailable) { return RENDERER_MODULE_FAILURE; }

            SelectRendererTextureStage(stage, D3DTSS_MIPMAPLODBIAS, (DWORD)value);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_ALPHA_BLEND_STATE:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_ALPHA_BLEND_NONE: { SelectRendererState(D3DRS_ALPHABLENDENABLE, FALSE); break; }
            case RENDERER_MODULE_ALPHA_BLEND_UNKNOWN_1:
            case RENDERER_MODULE_ALPHA_BLEND_UNKNOWN_3: { break; }
            case RENDERER_MODULE_ALPHA_BLEND_ACTIVE: { SelectRendererState(D3DRS_ALPHABLENDENABLE, TRUE); break; }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_TEXTURE_MIP_FILTER_STATE:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_TEXTURE_MIP_FILTER_NONE: { SelectRendererTextureStage(stage, D3DTSS_MIPFILTER, D3DTEXF_NONE); break; }
            case RENDERER_MODULE_TEXTURE_MIP_FILTER_POINT: { SelectRendererTextureStage(stage, D3DTSS_MIPFILTER, D3DTEXF_POINT); break; }
            case RENDERER_MODULE_TEXTURE_MIP_FILTER_LINEAR:
            {
                if (!State.Device.Capabilities.IsInterpolationAvailable) { return RENDERER_MODULE_FAILURE; }

                SelectRendererTextureStage(stage, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

                break;
            }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_CHROMATIC_COLOR:
        case RENDERER_MODULE_STATE_SELECT_LINE_WIDTH:
        case RENDERER_MODULE_STATE_SELECT_FLAT_FANS_STATE:
        case RENDERER_MODULE_STATE_SELECT_TOGGLE_STATE: { result = RENDERER_MODULE_SUCCESS; break; }
        case RENDERER_MODULE_STATE_SELECT_TEXTURE_ADDRESS_STATE:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_TEXTURE_ADDRESS_CLAMP:
            {
                SelectRendererTextureStage(stage, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
                SelectRendererTextureStage(stage, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

                break;
            }
            case RENDERER_MODULE_TEXTURE_ADDRESS_WRAP:
            {
                SelectRendererTextureStage(stage, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
                SelectRendererTextureStage(stage, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);

                break;
            }
            case RENDERER_MODULE_TEXTURE_ADDRESS_MIRROR:
            {
                SelectRendererTextureStage(stage, D3DTSS_ADDRESSU, D3DTADDRESS_MIRROR);
                SelectRendererTextureStage(stage, D3DTSS_ADDRESSV, D3DTADDRESS_MIRROR);

                break;
            }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_FOG_DENSITY:
        {
            State.Settings.FogDensity = *(f32*)&value;

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_FOG_COLOR:
        {
            RendererFogColor = ((u32)value) & RENDERER_MODULE_FOG_COLOR_MASK;

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_WINDOW_MODE_STATE:
        {
            State.Settings.IsWindowMode = (BOOL)value;

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_LAMBDAS:
        {
            if (value == NULL)
            {
                ZeroMemory(&State.Lambdas.Lambdas, sizeof(RendererModuleLambdaContainer));
            }
            else
            {
                const RendererModuleLambdaContainer* lambdas = (RendererModuleLambdaContainer*)value;

                State.Lambdas.Log = lambdas->Log;

                CopyMemory(&State.Lambdas.Lambdas, lambdas, sizeof(RendererModuleLambdaContainer));
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_FOG_ALPHAS:
        case RENDERER_MODULE_STATE_SELECT_FOG_ALPHAS_ALTERNATIVE:
        {
            if (value != NULL) { SelectRendererFogAlphas((u8*)value, RendererFogAlphas); }

            State.Settings.IsFogActive = TRUE;

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_FOG_STATE:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_FOG_INACTIVE:
            {
                State.Settings.IsFogStateActive = FALSE;

                SelectRendererState(D3DRS_FOGENABLE, FALSE);
                SelectRendererState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);

                break;
            }
            case RENDERER_MODULE_FOG_ACTIVE:
            {
                State.Settings.IsFogStateActive = TRUE;

                SelectRendererState(D3DRS_FOGENABLE, FALSE);
                SelectRendererState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);

                break;
            }
            case RENDERER_MODULE_FOG_ACTIVE_LINEAR:
            {
                State.Settings.IsFogActive = FALSE;
                RendererFogState = RENDERER_MODULE_FOG_ACTIVE_LINEAR;

                break;
            }
            case RENDERER_MODULE_FOG_ACTIVE_EXP:
            {
                State.Settings.IsFogActive = FALSE;
                RendererFogState = RENDERER_MODULE_FOG_ACTIVE_EXP;

                break;
            }
            case RENDERER_MODULE_FOG_ACTIVE_EXP2:
            {
                State.Settings.IsFogActive = FALSE;
                RendererFogState = RENDERER_MODULE_FOG_ACTIVE_EXP2;

                break;
            }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_FOG_START:
        {
            State.Settings.FogStart = *(f32*)&value;

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_FOG_END:
        {
            RendererFogEnd = *(f32*)&value;

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_DEPTH_BIAS_STATE:
        case RENDERER_MODULE_STATE_SELECT_DEPTH_BIAS_STATE_ALTERNATIVE:
        {
            SelectRendererState(D3DRS_ZBIAS, ((long)value < 16 ? 16 : (long)value));

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_WINDOW:
        {
            State.Window.HWND = (HWND)value;

            SelectBasicRendererState(RENDERER_MODULE_STATE_SELECT_WINDOW, value);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_RESTORE_RENDERER_SURFACES:
        {
            if (!RestoreRendererSurfaces()) { return RENDERER_MODULE_FAILURE; }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_ALPHA_TEST_STATE:
        {
            if ((u32)value == RENDERER_MODULE_ALPHA_TEST_0)
            {
                SelectRendererState(D3DRS_ALPHATESTENABLE, FALSE);
                SelectRendererState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
            }
            else
            {
                SelectRendererState(D3DRS_ALPHATESTENABLE, TRUE);
                SelectRendererState(D3DRS_ALPHAFUNC, RendererAlphaFunction);
                SelectRendererState(D3DRS_ALPHAREF, (DWORD)value);
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELCT_DEPTH_FUNCTION:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_DEPTH_FUNCTION_NEVER:
            {
                RendererDepthFunction = D3DCMP_NEVER;

                SelectRendererState(D3DRS_ZFUNC, D3DCMP_NEVER);

                break;
            }
            case RENDERER_MODULE_DEPTH_FUNCTION_LESS:
            {
                RendererDepthFunction = D3DCMP_LESS;

                SelectRendererState(D3DRS_ZFUNC, D3DCMP_LESS);

                break;
            }
            case RENDERER_MODULE_DEPTH_FUNCTION_EQUAL:
            {
                RendererDepthFunction = D3DCMP_EQUAL;

                SelectRendererState(D3DRS_ZFUNC, D3DCMP_EQUAL);

                break;
            }
            case RENDERER_MODULE_DEPTH_FUNCTION_LESS_EQUAL:
            {
                RendererDepthFunction = D3DCMP_LESSEQUAL;

                SelectRendererState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

                break;
            }
            case RENDERER_MODULE_DEPTH_FUNCTION_GREATER:
            {
                RendererDepthFunction = D3DCMP_GREATER;

                SelectRendererState(D3DRS_ZFUNC, D3DCMP_GREATER);

                break;
            }
            case RENDERER_MODULE_DEPTH_FUNCTION_NOT_EQUAL:
            {
                RendererDepthFunction = D3DCMP_NOTEQUAL;

                SelectRendererState(D3DRS_ZFUNC, D3DCMP_NOTEQUAL);

                break;
            }
            case RENDERER_MODULE_DEPTH_FUNCTION_GREATER_EQUAL:
            {
                RendererDepthFunction = D3DCMP_GREATEREQUAL;

                SelectRendererState(D3DRS_ZFUNC, D3DCMP_GREATEREQUAL);

                break;
            }
            case RENDERER_MODULE_DEPTH_FUNCTION_ALWAYS:
            {
                RendererDepthFunction = D3DCMP_ALWAYS;

                SelectRendererState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

                break;
            }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_TEXTURE_STAGE_BLEND_STATE:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_TEXTURE_STAGE_BLEND_NORMAL:
            {
                if (stage == RENDERER_TEXTURE_STAGE_0)
                {
                    if (!State.Device.Capabilities.IsModulateBlending)
                    {
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLOROP, D3DTOP_MODULATE);
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                    }
                    else
                    {
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLOROP, D3DTOP_MODULATE);
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
                    }
                }
                else if (State.Device.Capabilities.MaximumSimultaneousTextures < 2)
                {
                    SelectRendererTextureStage(stage, D3DTSS_COLOROP, D3DTOP_DISABLE);
                    SelectRendererTextureStage(stage, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                    SelectRendererTextureStage(stage, D3DTSS_COLORARG2, D3DTA_CURRENT);
                    SelectRendererTextureStage(stage, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
                    SelectRendererTextureStage(stage, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                    SelectRendererTextureStage(stage, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
                }
                else
                {
                    SelectRendererTextureStage(stage, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
                    SelectRendererTextureStage(stage, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                    SelectRendererTextureStage(stage, D3DTSS_COLORARG2, D3DTA_CURRENT);
                    SelectRendererTextureStage(stage, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
                    SelectRendererTextureStage(stage, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                    SelectRendererTextureStage(stage, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
                }

                result = RENDERER_MODULE_SUCCESS; break;
            }
            case RENDERER_MODULE_TEXTURE_STAGE_BLEND_ADD:
            {
                SelectRendererTextureStage(stage, D3DTSS_COLOROP, D3DTOP_ADD);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG2, D3DTA_CURRENT);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAOP, D3DTOP_ADD);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAARG2, (stage != RENDERER_TEXTURE_STAGE_0) ? D3DTA_CURRENT : D3DTA_DIFFUSE);

                result = State.Textures.Stages[stage].Unk01; break;
            }
            case RENDERER_MODULE_TEXTURE_STAGE_BLEND_DISABLE:
            {
                SelectRendererTextureStage(stage, D3DTSS_COLOROP, D3DTOP_DISABLE);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG1, D3DTA_CURRENT);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG2, D3DTA_CURRENT);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

                result = RENDERER_MODULE_SUCCESS; break;
            }
            case RENDERER_MODULE_TEXTURE_STAGE_BLEND_MODULATE:
            {
                SelectRendererTextureStage(stage, D3DTSS_COLOROP, D3DTOP_MODULATE);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG2, D3DTA_CURRENT);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAARG2, (stage != RENDERER_TEXTURE_STAGE_0) ? D3DTA_CURRENT : D3DTA_DIFFUSE);

                result = State.Textures.Stages[stage].Unk03; break;
            }
            case RENDERER_MODULE_TEXTURE_STAGE_BLEND_SUBTRACT:
            {
                SelectRendererTextureStage(stage, D3DTSS_COLOROP, D3DTOP_SUBTRACT);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG2, D3DTA_CURRENT);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

                result = State.Textures.Stages[stage].Unk02; break;
            }
            case RENDERER_MODULE_TEXTURE_STAGE_BLEND_ARG2:
            {
                SelectRendererTextureStage(stage, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG2, D3DTA_CURRENT);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAOP, D3DTOP_ADDSIGNED);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAARG1, D3DTA_COMPLEMENT | D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

                result = State.Textures.Stages[stage].Unk07; break;
            }
            case RENDERER_MODULE_TEXTURE_STAGE_BLEND_MODULATE_2X:
            {
                SelectRendererTextureStage(stage, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG2, D3DTA_CURRENT);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAOP, D3DTOP_MODULATE2X);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAARG2, (stage != RENDERER_TEXTURE_STAGE_0) ? D3DTA_CURRENT : D3DTA_DIFFUSE);

                result = State.Textures.Stages[stage].Unk04; break;
            }
            case RENDERER_MODULE_TEXTURE_STAGE_BLEND_MODULATE_4X:
            {
                SelectRendererTextureStage(stage, D3DTSS_COLOROP, D3DTOP_MODULATE4X);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG2, D3DTA_CURRENT);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAOP, D3DTOP_MODULATE4X);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAARG2, (stage != RENDERER_TEXTURE_STAGE_0) ? D3DTA_CURRENT : D3DTA_DIFFUSE);

                result = State.Textures.Stages[stage].Unk05; break;
            }
            case RENDERER_MODULE_TEXTURE_STAGE_BLEND_TEXTURE_ALPHA:
            {
                if (stage == RENDERER_TEXTURE_STAGE_0)
                {
                    if (!State.Device.Capabilities.IsModulateBlending)
                    {
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLOROP, D3DTOP_MODULATE);
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                    }
                    else
                    {
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLOROP, D3DTOP_MODULATE);
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
                    }

                    result = State.Textures.Stages[RENDERER_TEXTURE_STAGE_0].Unk09;
                }
                else
                {
                    SelectRendererTextureStage(stage, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA);
                    SelectRendererTextureStage(stage, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                    SelectRendererTextureStage(stage, D3DTSS_COLORARG2, D3DTA_CURRENT);
                    SelectRendererTextureStage(stage, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
                    SelectRendererTextureStage(stage, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                    SelectRendererTextureStage(stage, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

                    result = State.Textures.Stages[stage].Unk09;
                }

                break;
            }
            case RENDERER_MODULE_TEXTURE_STAGE_BLEND_ADD_SIGNED:
            {
                SelectRendererTextureStage(stage, D3DTSS_COLOROP, D3DTOP_ADDSIGNED);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG2, D3DTA_CURRENT);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAOP, D3DTOP_ADDSIGNED);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAARG2, (stage != RENDERER_TEXTURE_STAGE_0) ? D3DTA_CURRENT : D3DTA_DIFFUSE);

                result = State.Textures.Stages[stage].Unk08; break;
            }
            case RENDERER_MODULE_TEXTURE_STAGE_BLEND_DOT_PRODUCT_3:
            {
                if (stage == RENDERER_TEXTURE_STAGE_0)
                {
                    SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLOROP, D3DTOP_DOTPRODUCT3);
                    SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                    SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
                    SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
                    SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                }
                else
                {
                    SelectRendererTextureStage(stage, D3DTSS_COLOROP, D3DTOP_MODULATE);
                    SelectRendererTextureStage(stage, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
                    SelectRendererTextureStage(stage, D3DTSS_COLORARG2, D3DTA_CURRENT);
                    SelectRendererTextureStage(stage, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
                    SelectRendererTextureStage(stage, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
                }

                result = State.Textures.Stages[stage].Unk10; break;
            }
            case RENDERER_MODULE_TEXTURE_STAGE_BLEND_BUMP_ENV_MAP:
            {
                SelectRendererTextureStage(stage, D3DTSS_COLOROP, D3DTOP_BUMPENVMAP);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG2, (stage != RENDERER_TEXTURE_STAGE_0) ? D3DTA_CURRENT : D3DTA_DIFFUSE);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

                result = State.Textures.Stages[stage].Unk11; break;
            }
            case RENDERER_MODULE_TEXTURE_STAGE_BLEND_BUMP_ENV_MAP_LUMINANCE:
            {
                SelectRendererTextureStage(stage, D3DTSS_COLOROP, D3DTOP_BUMPENVMAPLUMINANCE);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG2, (stage != RENDERER_TEXTURE_STAGE_0) ? D3DTA_CURRENT : D3DTA_DIFFUSE);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

                result = State.Textures.Stages[stage].Unk12; break;
            }
            case RENDERER_MODULE_TEXTURE_STAGE_BLEND_ADD_BLEND_FACTOR_ALPHA:
            {
                SelectRendererTextureStage(stage, D3DTSS_COLOROP, D3DTOP_ADD);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG2, D3DTA_CURRENT);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAOP, D3DTOP_BLENDFACTORALPHA);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

                result = RENDERER_MODULE_SUCCESS; break;
            }
            case RENDERER_MODULE_TEXTURE_STAGE_BLEND_BLEND_FACTOR_ALPHA_ARG1:
            {
                SelectRendererTextureStage(stage, D3DTSS_COLOROP, D3DTOP_BLENDFACTORALPHA);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG2, D3DTA_CURRENT);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

                result = RENDERER_MODULE_SUCCESS; break;
            }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            if (result == RENDERER_MODULE_FAILURE) { return RENDERER_MODULE_FAILURE; }

            break;
        }
        case RENDERER_MODULE_STATE_SELECT_VERTEX_TYPE:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_VERTEX_TYPE_RTLVX:
            {
                RendererVertexSize = sizeof(RTLVX);
                RendererCurrentShader = D3DFVF_TEX1 | D3DFVF_SPECULAR | D3DFVF_DIFFUSE | D3DFVF_XYZRHW;

                State.DX.Device->SetVertexShader(D3DFVF_TEX1 | D3DFVF_SPECULAR | D3DFVF_DIFFUSE | D3DFVF_XYZRHW);

                SelectState(RENDERER_MODULE_STATE_SELECT_TEXTURE_COORDINATE_INDEX_STATE, (void*)0);
                SelectState(MAKETEXTURESTAGEMASK(RENDERER_TEXTURE_STAGE_1) | RENDERER_MODULE_STATE_SELECT_TEXTURE_COORDINATE_INDEX_STATE, (void*)0);

                break;
            }
            case RENDERER_MODULE_VERTEX_TYPE_RTLVX2:
            {
                RendererVertexSize = sizeof(RTLVX2);
                RendererCurrentShader = D3DFVF_TEX2 | D3DFVF_SPECULAR | D3DFVF_DIFFUSE | D3DFVF_XYZRHW;

                State.DX.Device->SetVertexShader(D3DFVF_TEX2 | D3DFVF_SPECULAR | D3DFVF_DIFFUSE | D3DFVF_XYZRHW);

                SelectState(RENDERER_MODULE_STATE_SELECT_TEXTURE_COORDINATE_INDEX_STATE, (void*)0);
                SelectState(MAKETEXTURESTAGEMASK(RENDERER_TEXTURE_STAGE_1) | RENDERER_MODULE_STATE_SELECT_TEXTURE_COORDINATE_INDEX_STATE, (void*)1);

                break;
            }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_GAMMA_CONTROL_STATE:
        case RENDERER_MODULE_STATE_SELECT_GAMMA_CONTROL_STATE_ALTERNATIVE:
        {
            if (!State.Device.Capabilities.IsGammaAvailable) { return RENDERER_MODULE_FAILURE; }
            if (State.DX.Device == NULL) { return RENDERER_MODULE_FAILURE; }
            if (State.Settings.IsWindowMode) { return RENDERER_MODULE_FAILURE; }

            const f32 actual = *(f32*)&value;

            f32 modifier = 0.0f;
            
            if ((actual < 0.0f) == (actual == 0.0f))
            {
                modifier = (actual < 4.0f) ? (actual * 257.0f) : 1028.0f;
            }

            D3DGAMMARAMP gamma;

            for (u32 x = 0; x < 256; x++)
            {
                gamma.red[x] = (u16)Clamp<u32>((u32)(State.DX.State.Gamma.red[x] * modifier + 0.5f), U16_MIN, U16_MAX);
                gamma.green[x] = (u16)Clamp<u32>((u32)(State.DX.State.Gamma.green[x] * modifier + 0.5f), U16_MIN, U16_MAX);
                gamma.blue[x] = (u16)Clamp<u32>((u32)(State.DX.State.Gamma.blue[x] * modifier + 0.5f), U16_MIN, U16_MAX);
            }

            State.DX.Device->SetGammaRamp(0, &gamma);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_STENCIL_STATE:
        {
            if (!State.Device.Capabilities.IsStencilBufferAvailable) { return RENDERER_MODULE_FAILURE; }

            switch ((u32)value)
            {
            case RENDERER_MODULE_STENCIL_INACTIVE: { SelectRendererState(D3DRS_STENCILENABLE, FALSE); break; }
            case RENDERER_MODULE_STENCIL_ACTIVE: { SelectRendererState(D3DRS_STENCILENABLE, TRUE); break; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_STENCIL_FUNCTION:
        {
            if (!State.Device.Capabilities.IsStencilBufferAvailable) { return RENDERER_MODULE_FAILURE; }

            switch ((u32)value)
            {
            case RENDERER_MODULE_STENCIL_FUNCTION_NEVER: { SelectRendererState(D3DRS_STENCILFUNC, D3DCMP_NEVER); break; }
            case RENDERER_MODULE_STENCIL_FUNCTION_LESS: { SelectRendererState(D3DRS_STENCILFUNC, D3DCMP_LESS); break; }
            case RENDERER_MODULE_STENCIL_FUNCTION_EQUAL: { SelectRendererState(D3DRS_STENCILFUNC, D3DCMP_EQUAL); break; }
            case RENDERER_MODULE_STENCIL_FUNCTION_LESS_EQUAL: { SelectRendererState(D3DRS_STENCILFUNC, D3DCMP_LESSEQUAL); break; }
            case RENDERER_MODULE_STENCIL_FUNCTION_GREATER: { SelectRendererState(D3DRS_STENCILFUNC, D3DCMP_GREATER); break; }
            case RENDERER_MODULE_STENCIL_FUNCTION_NOT_EQUAL: { SelectRendererState(D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL); break; }
            case RENDERER_MODULE_STENCIL_FUNCTION_GREATER_EQUAL: { SelectRendererState(D3DRS_STENCILFUNC, D3DCMP_GREATEREQUAL); break; }
            case RENDERER_MODULE_STENCIL_FUNCTION_ALWAYS: { SelectRendererState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS); break; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_STENCIL_FAIL_STATE:
        {
            if (!State.Device.Capabilities.IsStencilBufferAvailable) { return RENDERER_MODULE_FAILURE; }

            switch ((u32)value)
            {
            case RENDERER_MODULE_STENCIL_FAIL_KEEP: { SelectRendererState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP); break; }
            case RENDERER_MODULE_STENCIL_FAIL_ZERO: { SelectRendererState(D3DRS_STENCILFAIL, D3DSTENCILOP_ZERO); break; }
            case RENDERER_MODULE_STENCIL_FAIL_REPLACE: { SelectRendererState(D3DRS_STENCILFAIL, D3DSTENCILOP_REPLACE); break; }
            case RENDERER_MODULE_STENCIL_FAIL_INCREMENT_CLAMP: { SelectRendererState(D3DRS_STENCILFAIL, D3DSTENCILOP_INCRSAT); break; }
            case RENDERER_MODULE_STENCIL_FAIL_DECREMENT_CLAMP: { SelectRendererState(D3DRS_STENCILFAIL, D3DSTENCILOP_DECRSAT); break; }
            case RENDERER_MODULE_STENCIL_FAIL_INVERT: { SelectRendererState(D3DRS_STENCILFAIL, D3DSTENCILOP_INVERT); break; }
            case RENDERER_MODULE_STENCIL_FAIL_INCREMENT: { SelectRendererState(D3DRS_STENCILFAIL, D3DSTENCILOP_INCR); break; }
            case RENDERER_MODULE_STENCIL_FAIL_DECREMENT: { SelectRendererState(D3DRS_STENCILFAIL, D3DSTENCILOP_DECR); break; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_STENCIL_DEPTH_FAIL_STATE:
        {
            if (!State.Device.Capabilities.IsStencilBufferAvailable) { return RENDERER_MODULE_FAILURE; }

            switch ((u32)value)
            {
            case RENDERER_MODULE_STENCIL_DEPTH_FAIL_KEEP: { SelectRendererState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP); break; }
            case RENDERER_MODULE_STENCIL_DEPTH_FAIL_ZERO: { SelectRendererState(D3DRS_STENCILZFAIL, D3DSTENCILOP_ZERO); break; }
            case RENDERER_MODULE_STENCIL_DEPTH_FAIL_REPLACE: { SelectRendererState(D3DRS_STENCILZFAIL, D3DSTENCILOP_REPLACE); break; }
            case RENDERER_MODULE_STENCIL_DEPTH_FAIL_INCREMENT_CLAMP: { SelectRendererState(D3DRS_STENCILZFAIL, D3DSTENCILOP_INCRSAT); break; }
            case RENDERER_MODULE_STENCIL_DEPTH_FAIL_DECREMENT_CLAMP: { SelectRendererState(D3DRS_STENCILZFAIL, D3DSTENCILOP_DECRSAT); break; }
            case RENDERER_MODULE_STENCIL_DEPTH_FAIL_INVERT: { SelectRendererState(D3DRS_STENCILZFAIL, D3DSTENCILOP_INVERT); break; }
            case RENDERER_MODULE_STENCIL_DEPTH_FAIL_INCREMENT: { SelectRendererState(D3DRS_STENCILZFAIL, D3DSTENCILOP_INCR); break; }
            case RENDERER_MODULE_STENCIL_DEPTH_FAIL_DECREMENT: { SelectRendererState(D3DRS_STENCILZFAIL, D3DSTENCILOP_DECR); break; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_STENCIL_PASS_STATE:
        {
            if (!State.Device.Capabilities.IsStencilBufferAvailable) { return RENDERER_MODULE_FAILURE; }

            switch ((u32)value)
            {
            case RENDERER_MODULE_STENCIL_PASS_KEEP: { SelectRendererState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP); break; }
            case RENDERER_MODULE_STENCIL_PASS_ZERO: { SelectRendererState(D3DRS_STENCILPASS, D3DSTENCILOP_ZERO); break; }
            case RENDERER_MODULE_STENCIL_PASS_REPLACE: { SelectRendererState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE); break; }
            case RENDERER_MODULE_STENCIL_PASS_INCREMENT_CLAMP: { SelectRendererState(D3DRS_STENCILPASS, D3DSTENCILOP_INCRSAT); break; }
            case RENDERER_MODULE_STENCIL_PASS_DECREMENT_CLAMP: { SelectRendererState(D3DRS_STENCILPASS, D3DSTENCILOP_DECRSAT); break; }
            case RENDERER_MODULE_STENCIL_PASS_INVERT: { SelectRendererState(D3DRS_STENCILPASS, D3DSTENCILOP_INVERT); break; }
            case RENDERER_MODULE_STENCIL_PASS_INCREMENT: { SelectRendererState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR); break; }
            case RENDERER_MODULE_STENCIL_PASS_DECREMENT: { SelectRendererState(D3DRS_STENCILPASS, D3DSTENCILOP_DECR); break; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_BLEND_STATE:
        case RENDERER_MODULE_STATE_SELECT_BLEND_STATE_ALTERNATIVE:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_ALPHA_BLEND_SOURCE_INVERSE_SOURCE:
            {
                SelectRendererState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
                SelectRendererState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

                break;
            }
            case RENDERER_MODULE_ALPHA_BLEND_SOURCE_ONE:
            {
                SelectRendererState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
                SelectRendererState(D3DRS_DESTBLEND, D3DBLEND_ONE);

                break;
            }
            case RENDERER_MODULE_ALPHA_BLEND_ZERO_INVERSE_SOURCE:
            {
                SelectRendererState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
                SelectRendererState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

                break;
            }
            case RENDERER_MODULE_ALPHA_BLEND_DESTINATION_COLOR_ZERO:
            {
                SelectRendererState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
                SelectRendererState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

                break;
            }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_DEPTH_BUFFER_WRITE_STATE:
        case RENDERER_MODULE_STATE_SELECT_DEPTH_BUFFER_WRITE_STATE_ALTERNATIVE:
        {
            SelectRendererState(D3DRS_ZWRITEENABLE, (u32)value == 0 ? FALSE : TRUE);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_ACQUIRE_GUARD_BANDS:
        {
            if (value != NULL)
            {
                const s32 left = (s32)State.Device.Capabilities.GuardBandLeft;
                const s32 right = (s32)State.Device.Capabilities.GuardBandRight;
                const s32 top = (s32)State.Device.Capabilities.GuardBandTop;
                const s32 bottom = (s32)State.Device.Capabilities.GuardBandBottom;

                if (left != 0 && right != 0 && top != 0 && bottom != 0)
                {
                    RendererModuleGuardBands* output = (RendererModuleGuardBands*)value;

                    output->Left = left;
                    output->Right = right;
                    output->Top = top;
                    output->Bottom = bottom;

                    return RENDERER_MODULE_SUCCESS;
                }
            }

            return RENDERER_MODULE_FAILURE;
        }
        case RENDERER_MODULE_STATE_SELCT_ALPHA_FUNCTION:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_ALPHA_FUNCTION_NEVER:
            {
                RendererAlphaFunction = D3DCMP_NEVER;

                SelectRendererState(D3DRS_ALPHAFUNC, D3DCMP_NEVER);

                break;
            }
            case RENDERER_MODULE_ALPHA_FUNCTION_LESS:
            {
                RendererAlphaFunction = D3DCMP_LESS;

                SelectRendererState(D3DRS_ALPHAFUNC, D3DCMP_LESS);

                break;
            }
            case RENDERER_MODULE_ALPHA_FUNCTION_EQUAL:
            {
                RendererAlphaFunction = D3DCMP_EQUAL;

                SelectRendererState(D3DRS_ALPHAFUNC, D3DCMP_EQUAL);

                break;
            }
            case RENDERER_MODULE_ALPHA_FUNCTION_LESS_EQUAL:
            {
                RendererAlphaFunction = D3DCMP_LESSEQUAL;

                SelectRendererState(D3DRS_ALPHAFUNC, D3DCMP_LESSEQUAL);

                break;
            }
            case RENDERER_MODULE_ALPHA_FUNCTION_GREATER:
            {
                RendererAlphaFunction = D3DCMP_GREATER;

                SelectRendererState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

                break;
            }
            case RENDERER_MODULE_ALPHA_FUNCTION_NOT_EQUAL:
            {
                RendererAlphaFunction = D3DCMP_NOTEQUAL;

                SelectRendererState(D3DRS_ALPHAFUNC, D3DCMP_NOTEQUAL);

                break;
            }
            case RENDERER_MODULE_ALPHA_FUNCTION_GREATER_EQUAL:
            {
                RendererAlphaFunction = D3DCMP_GREATEREQUAL;

                SelectRendererState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

                break;
            }
            case RENDERER_MODULE_ALPHA_FUNCTION_ALWAYS:
            {
                RendererAlphaFunction = D3DCMP_ALWAYS;

                SelectRendererState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);

                break;
            }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_TEXTURE_ADDRESS_STATE_U:
        {
            if (!State.Device.Capabilities.IsTextureIndependentUVs) { return RENDERER_MODULE_FAILURE; }

            switch ((u32)value)
            {
            case RENDERER_MODULE_TEXTURE_ADDRESS_CLAMP: { SelectRendererTextureStage(stage, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP); break; }
            case RENDERER_MODULE_TEXTURE_ADDRESS_WRAP: { SelectRendererTextureStage(stage, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP); break; }
            case RENDERER_MODULE_TEXTURE_ADDRESS_MIRROR: { SelectRendererTextureStage(stage, D3DTSS_ADDRESSU, D3DTADDRESS_MIRROR); break; }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_TEXTURE_ADDRESS_STATE_V:
        {
            if (!State.Device.Capabilities.IsTextureIndependentUVs) { return RENDERER_MODULE_FAILURE; }

            switch ((u32)value)
            {
            case RENDERER_MODULE_TEXTURE_ADDRESS_CLAMP: { SelectRendererTextureStage(stage, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP); break; }
            case RENDERER_MODULE_TEXTURE_ADDRESS_WRAP: { SelectRendererTextureStage(stage, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP); break; }
            case RENDERER_MODULE_TEXTURE_ADDRESS_MIRROR: { SelectRendererTextureStage(stage, D3DTSS_ADDRESSV, D3DTADDRESS_MIRROR); break; }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_CLEAR_DEPTH_STATE:
        {
            RendererClearDepth = *(f32*)&value;

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_TEXTURE_COORDINATE_INDEX_STATE:
        {
            SelectRendererTextureStage(stage, D3DTSS_TEXCOORDINDEX, (DWORD)value);

            return RENDERER_MODULE_FAILURE;
        }
        case RENDERER_MODULE_STATE_SELECT_TEXTURE_MIN_FILTER_STATE:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_TEXTURE_FILTER_POINT: { SelectRendererTextureStage(stage, D3DTSS_MINFILTER, D3DTEXF_POINT); break; }
            case RENDERER_MODULE_TEXTURE_FILTER_LENEAR: { SelectRendererTextureStage(stage, D3DTSS_MINFILTER, D3DTEXF_LINEAR); break; }
            case RENDERER_MODULE_TEXTURE_FILTER_ANISOTROPY:
            {
                if (!State.Device.Capabilities.IsAnisotropyAvailable) { return RENDERER_MODULE_FAILURE; }

                SelectRendererTextureStage(stage, D3DTSS_MINFILTER, D3DTEXF_ANISOTROPIC);

                break;
            }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_TEXTURE_MAG_FILTER_STATE:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_TEXTURE_FILTER_POINT: { SelectRendererTextureStage(stage, D3DTSS_MAGFILTER, D3DTEXF_POINT); break; }
            case RENDERER_MODULE_TEXTURE_FILTER_LENEAR: { SelectRendererTextureStage(stage, D3DTSS_MAGFILTER, D3DTEXF_LINEAR); break; }
            case RENDERER_MODULE_TEXTURE_FILTER_ANISOTROPY:
            {
                if (!State.Device.Capabilities.IsAnisotropyAvailable) { return RENDERER_MODULE_FAILURE; }

                SelectRendererTextureStage(stage, D3DTSS_MAGFILTER, D3DTEXF_ANISOTROPIC);

                break;
            }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_VERTEX_MIN_DEPTH:
        {
            RendererMinVertexDepth = *(f32*)&value;

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_VERTEX_MAX_DEPTH:
        {
            RendererMaxVertexDepth = *(f32*)&value;

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_ACQUIRE_DEVICE_CAPABILITIES_DX8:
        {
            if (value == NULL) { return RENDERER_MODULE_FAILURE; }

            memcpy(value, &State.Device.Capabilities, sizeof(RendererModuleDeviceCapabilities8));

            return (addr)value;
        }
        case RENDERER_MODULE_STATE_SELECT_SOURCE_BLEND_STATE_DX8:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_BLEND_ONE: { SelectRendererState(D3DRS_SRCBLEND, D3DBLEND_ONE); break; }
            case RENDERER_MODULE_BLEND_ZERO: { SelectRendererState(D3DRS_SRCBLEND, D3DBLEND_ZERO); break; }
            case RENDERER_MODULE_BLEND_SOURCE_ALPHA: { SelectRendererState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA); break; }
            case RENDERER_MODULE_BLEND_INVERSE_SOURCE_ALPHA: { SelectRendererState(D3DRS_SRCBLEND, D3DBLEND_INVSRCALPHA); break; }
            case RENDERER_MODULE_BLEND_DESTINATION_ALPHA: { SelectRendererState(D3DRS_SRCBLEND, D3DBLEND_DESTALPHA); break; }
            case RENDERER_MODULE_BLEND_INVERSE_DESTINATION_ALPHA: { SelectRendererState(D3DRS_SRCBLEND, D3DBLEND_INVDESTALPHA); break; }
            case RENDERER_MODULE_BLEND_SOURCE_COLOR: { SelectRendererState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR); break; }
            case RENDERER_MODULE_BLEND_DESTINATION_COLOR: { SelectRendererState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR); break; }
            case RENDERER_MODULE_BLEND_INVERSE_SOURCE_COLOR: { SelectRendererState(D3DRS_SRCBLEND, D3DBLEND_INVSRCCOLOR); break; }
            case RENDERER_MODULE_BLEND_INVERSE_DESTINATION_COLOR: { SelectRendererState(D3DRS_SRCBLEND, D3DBLEND_INVDESTCOLOR); break; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_DESTINATION_BLEND_STATE_DX8:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_BLEND_ONE: { SelectRendererState(D3DRS_DESTBLEND, D3DBLEND_ONE); break; }
            case RENDERER_MODULE_BLEND_ZERO: { SelectRendererState(D3DRS_DESTBLEND, D3DBLEND_ZERO); break; }
            case RENDERER_MODULE_BLEND_SOURCE_ALPHA: { SelectRendererState(D3DRS_DESTBLEND, D3DBLEND_SRCALPHA); break; }
            case RENDERER_MODULE_BLEND_INVERSE_SOURCE_ALPHA: { SelectRendererState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA); break; }
            case RENDERER_MODULE_BLEND_DESTINATION_ALPHA: { SelectRendererState(D3DRS_DESTBLEND, D3DBLEND_DESTALPHA); break; }
            case RENDERER_MODULE_BLEND_INVERSE_DESTINATION_ALPHA: { SelectRendererState(D3DRS_DESTBLEND, D3DBLEND_INVDESTALPHA); break; }
            case RENDERER_MODULE_BLEND_SOURCE_COLOR: { SelectRendererState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR); break; }
            case RENDERER_MODULE_BLEND_DESTINATION_COLOR: { SelectRendererState(D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR); break; }
            case RENDERER_MODULE_BLEND_INVERSE_SOURCE_COLOR: { SelectRendererState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR); break; }
            case RENDERER_MODULE_BLEND_INVERSE_DESTINATION_COLOR: { SelectRendererState(D3DRS_DESTBLEND, D3DBLEND_INVDESTCOLOR); break; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_TEXTURE_FACTOR_DX8:
        {
            SelectRendererState(D3DRS_TEXTUREFACTOR, (DWORD)value);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_DESTINATION_BLEND_STATE:
        {
            if (value == NULL) { return RENDERER_MODULE_FAILURE; }

            const RendererModuleTextureStageBumpMappingMatrix* matrix = (RendererModuleTextureStageBumpMappingMatrix*)value;

            SelectRendererTextureStage(stage, D3DTSS_BUMPENVMAT00, *(DWORD*)&matrix->M00);
            SelectRendererTextureStage(stage, D3DTSS_BUMPENVMAT01, *(DWORD*)&matrix->M01);
            SelectRendererTextureStage(stage, D3DTSS_BUMPENVMAT10, *(DWORD*)&matrix->M10);
            SelectRendererTextureStage(stage, D3DTSS_BUMPENVMAT11, *(DWORD*)&matrix->M11);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_BUMP_MAPPING_LUMINANCE_SCALE_DX8:
        {
            SelectRendererTextureStage(stage, D3DTSS_BUMPENVLSCALE, (DWORD)value);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_BUMP_MAPPING_LUMINANCE_OFFSET_DX8:
        {
            SelectRendererTextureStage(stage, D3DTSS_BUMPENVLOFFSET, (DWORD)value);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_RENDER_PACKET_DX8:
        {
            BeginRendererScene();

            const RendererModulePacket* packet = (RendererModulePacket*)value;

            u32 count = 0;

            switch (packet->Type)
            {
            case RendererModulePrimitiveTypePointList: { count = packet->VertexCount; break; }
            case RendererModulePrimitiveTypeLineList: { count = packet->VertexCount / 2; break; }
            case RendererModulePrimitiveTypeLineStrip: { count = packet->VertexCount - 1; break; }
            case RendererModulePrimitiveTypeTriangleList: { count = packet->VertexCount / 3; break; }
            case RendererModulePrimitiveTypeTriangleStrip:
            case RendererModulePrimitiveTypeTriangleFan: { count = packet->VertexCount - 2; break; }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            State.DX.Device->SetVertexShader(packet->FVF);

            if (packet->Indexes == NULL || packet->IndexCount == 0)
            {
                result = State.DX.Device->DrawPrimitiveUP((D3DPRIMITIVETYPE)packet->Type,
                    count, packet->Vertexes, sizeof(RTLVX)) == D3D_OK;
            }
            else
            {
                result = State.DX.Device->DrawIndexedPrimitiveUP((D3DPRIMITIVETYPE)packet->Type,
                    0, packet->IndexCount + 1, count, packet->Indexes, D3DFMT_INDEX16,
                    packet->Vertexes, sizeof(RTLVX)) == D3D_OK;
            }

            if (result == RENDERER_MODULE_FAILURE) { return RENDERER_MODULE_FAILURE; }

            break;
        }
        case RENDERER_MODULE_STATE_SELECT_TEXTURE_COORDINATE_INDEX_STATE_DX8:
        {
            if (RendererCurrentShader & D3DFVF_TEX1)
            {
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_TEXCOORDINDEX, 0);
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_1, D3DTSS_TEXCOORDINDEX, 0);
            }
            else if (RendererCurrentShader & D3DFVF_TEX2)
            {
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_TEXCOORDINDEX, 0);
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_1, D3DTSS_TEXCOORDINDEX, 1);
            }

            return RENDERER_MODULE_FAILURE;
        }
        case RENDERER_MODULE_STATE_ACQUIRE_MAX_ANISOTROPY_DX8:
        {
            if (!State.Device.Capabilities.IsAnisotropyAvailable) { return RENDERER_MODULE_FAILURE; }

            *(u32*)value = State.Device.Capabilities.MaxAnisotropy;

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_MAX_ANISOTROPY_DX8:
        {
            if (!State.Device.Capabilities.IsAnisotropyAvailable) { return RENDERER_MODULE_FAILURE; }

            SelectRendererTextureStage(stage, D3DTSS_MAXANISOTROPY, Min(State.Device.Capabilities.MaxAnisotropy, (u32)value));

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_RENDERER_ACTIVE_LAMBDA_DX8:
        {
            if (value != NULL) { State.Lambdas.RendererActiveLambda = (RENDERERACTIVELAMBDA)value; }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_RENDERER_INACTIVE_LAMBDA_DX8:
        {
            if (value != NULL) { State.Lambdas.RendererInactiveLambda = (RENDERERINACTIVELAMBDA)value; }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_RENDERER_INACTIVE_LAMBDA_VALUE_DX8:
        {
            if (value != NULL) { State.Lambdas.RendererInactiveLambdaValue = value; }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_430_DX8:
        {
            ((u32*)value)[6] = 0; // TODO

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_431_DX8:
        {
            ((u32*)value)[6] = 1; // TODO

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_432_DX8:
        {
            ((u32*)value)[6] = 2; // TODO

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_433_DX8:
        {
            ((u32*)value)[6] = 3; // TODO

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_434_DX8:
        {
            ((u32*)value)[6] = 4; // TODO

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_435_DX8:
        {
            ((u32*)value)[6] = 5; // TODO

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_436_DX8:
        {
            ((u32*)value)[6] = 6; // TODO

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_437_DX8:
        {
            ((u32*)value)[6] = 7; // TODO

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_ACQUIRE_RENDERER_DEVICE_DX8:
        {
            if (value == NULL) { return RENDERER_MODULE_FAILURE; }

            *(IDirect3DDevice8**)value = State.DX.Device;

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_CLIPPING_STATE_DX8:
        {
            RenderPackets();

            State.DX.Device->SetRenderState(D3DRS_CLIPPING, (DWORD)value);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        default: { return SelectBasicRendererState(actual, value); }
        }

        SelectRendererStateValue(actual, value);

        return result;
    }

    // 0x600019f0
    // a.k.a. THRASH_settexture
    DLLAPI u32 STDCALLAPI SelectTexture(RendererTexture* tex)
    {
        RendererTexture* current = (RendererTexture*)AcquireState(RENDERER_MODULE_STATE_SELECT_TEXTURE);

        BeginRendererScene();
        RenderPackets();

        u32 palette = DEFAULT_TEXTURE_PALETTE_VALUE;

        if (tex == NULL)
        {
            for (u32 x = 0; x < State.Device.Capabilities.MaximumSimultaneousTextures; x++)
            {
                State.DX.Device->SetTexture(x, NULL);
            }
        }
        else
        {
            if ((u32)tex < 16) // TODO
            {
                State.DX.Device->SetTexture((u32)tex - 1, NULL);

                SelectRendererStateValue(RENDERER_MODULE_STATE_SELECT_TEXTURE, tex);

                return RENDERER_MODULE_SUCCESS;
            }

            if (current == tex)
            {
                SelectRendererStateValue(RENDERER_MODULE_STATE_SELECT_TEXTURE, tex);

                return RENDERER_MODULE_SUCCESS;
            }

            State.DX.Device->SetTexture(tex->Stage, tex->Texture);

            palette = tex->Palette;

            if (palette == INVALID_TEXTURE_PALETTE_VALUE)
            {
                SelectRendererStateValue(RENDERER_MODULE_STATE_SELECT_TEXTURE, tex);

                return RENDERER_MODULE_SUCCESS;
            }
        }

        State.DX.Device->SetCurrentTexturePalette(palette);

        SelectRendererStateValue(RENDERER_MODULE_STATE_SELECT_TEXTURE, tex);

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60003230
    // a.k.a. THRASH_setvideomode
    DLLAPI u32 STDCALLAPI SelectVideoMode(const u32 mode, const u32 pending, const u32 depth)
    {
        s32 actual = (s32)mode;

        if (actual < 0 || ModuleDescriptor.Capabilities.Count <= actual
            || !ModuleDescriptor.Capabilities.Capabilities[actual].IsActive)
        {
            actual = 1; // TODO
        }

        if (depth == 1) { State.DX.Surfaces.Bits = GRAPHICS_BITS_PER_PIXEL_16; } // TODO
        else if (depth == 2) { State.DX.Surfaces.Bits = GRAPHICS_BITS_PER_PIXEL_32; } // TODO
        else { State.DX.Surfaces.Bits = 0; }

        u32 result = RENDERER_MODULE_FAILURE;

        if (State.Lambdas.Lambdas.Execute != NULL)
        {
            if (GetWindowThreadProcessId(State.Window.Parent.HWND, NULL) != GetCurrentThreadId())
            {
                State.Window.HWND = NULL;
                State.Window.HWND = State.Lambdas.Lambdas.AcquireWindow();

                State.Mutexes.Device = CreateEventA(NULL, FALSE, FALSE, NULL);

                SetForegroundWindow(State.Window.HWND);
                PostMessageA(State.Window.HWND, RENDERER_MODULE_WINDOW_MESSAGE_INITIALIZE_DEVICE, actual, pending);

                result = WaitForSingleObject(State.Mutexes.Device, 10000) == WAIT_OBJECT_0;
            }
        }
        else
        {
            InitializeRendererDeviceExecute(0, State.Window.HWND, RENDERER_MODULE_WINDOW_MESSAGE_INITIALIZE_DEVICE, actual, pending, NULL);
        }

        BeginRendererScene();

        InitializeRendererModuleState(actual, pending, depth, ENVIRONMENT_SECTION_NAME);
        SelectBasicRendererState(RENDERER_MODULE_STATE_62, (void*)(DAT_6001dfd0 + 1));

        InitializeRendererState();

        SelectGameWindow(2); // TODO

        {
            RendererModuleWindowLock* lock = LockGameWindow();

            UnlockGameWindow(NULL);

            SelectRendererStateValue(RENDERER_MODULE_STATE_SELECT_WINDOW_LOCK_STATE, lock);
        }

        return result;
    }

    // 0x60001450
    // a.k.a. THRASH_sync
    DLLAPI u32 STDCALLAPI SyncGameWindow(const u32 type)
    {
        switch (type)
        {
        case RENDERER_MODULE_SYNC_NORMAL:
        {
            LockGameWindow();
            UnlockGameWindow(NULL);
            break;
        }
        case RENDERER_MODULE_SYNC_VBLANK:
        {
            if (State.DX.Device != NULL)
            {
                D3DRASTER_STATUS status;
                ZeroMemory(&status, sizeof(D3DRASTER_STATUS));

                while (State.DX.Device->GetRasterStatus(&status) == D3D_OK)
                {
                    if (status.InVBlank) { return RENDERER_MODULE_FAILURE; }
                }
            }

            break;
        }
        }

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60006d90
    // a.k.a. THRASH_talloc
    DLLAPI RendererTexture* STDCALLAPI AllocateTexture(const u32 width, const u32 height, const u32 format, const u32 palette, const u32 state)
    {
        if (width == 0 || height == 0
            || State.Textures.Formats[format] == D3DFMT_UNKNOWN || State.Textures.Illegal) { return NULL; }

        RendererTexture* tex = tex = AllocateRendererTexture();

        if (tex == NULL) { return NULL; }

        tex->Width = width;
        tex->Height = height;

        tex->PixelFormat = MAKEPIXELFORMAT(format);
        tex->TextureFormat = State.Textures.Formats[format];
        tex->PixelSize = PixelFormatSizes[format];

        tex->Stage = MAKETEXTURESTAGEVALUE(state);
        tex->MipMapCount = MAKETEXTUREMIPMAPVALUE(state) != 0 ? (MAKETEXTUREMIPMAPVALUE(state) + 1) : 0;

        tex->Texture = NULL;
        
        tex->Is16Bit = tex->PixelFormat == RENDERER_PIXEL_FORMAT_R5G5B5 || tex->PixelFormat == RENDERER_PIXEL_FORMAT_R4G4B4;
        tex->PaletteMode = palette;
        tex->MemoryType = RENDERER_MODULE_TEXTURE_LOCATION_SYSTEM_MEMORY;
        tex->Palette = INVALID_TEXTURE_PALETTE_VALUE;
        tex->Colors = 0;

        {
            const HRESULT result = InitializeRendererTexture(tex);

            if (result != D3D_OK)
            {
                ReleaseTexture(tex);

                if (result == D3DERR_OUTOFVIDEOMEMORY) { State.Textures.Illegal = TRUE; }

                return NULL;
            }
        }

        tex->Previous = State.Textures.Current;
        State.Textures.Current = tex;

        if (palette == RENDERER_MODULE_PALETTE_ACQUIRE) { tex->Palette = AcquireTexturePalette(); }

        return tex;
    }

    // 0x60006f20
    // a.k.a. THRASH_tfree
    DLLAPI u32 STDCALLAPI ReleaseTexture(RendererTexture* tex)
    {
        if (State.Textures.Current == NULL) { return RENDERER_MODULE_FAILURE; }

        RendererTexture* prev = NULL;
        RendererTexture* current = State.Textures.Current;
        RendererTexture* next = NULL;

        do
        {
            if (current == tex)
            {
                if (current == State.Textures.Current) { State.Textures.Current = current->Previous; }
                else if (next != NULL)
                {
                    if (current->Previous == NULL) { next->Previous = NULL; }
                    else { next->Previous = current->Previous; }
                }

                if (current->Texture != NULL)
                {
                    while (current->Texture->Release() != D3D_OK) { }

                    current->Texture = NULL;
                }

                if (current->Palette != INVALID_TEXTURE_PALETTE_VALUE)
                {
                    ReleaseTexturePalette(current->Palette);

                    current->Palette = INVALID_TEXTURE_PALETTE_VALUE;
                }

                DisposeRendererTexture(current);

                return RENDERER_MODULE_SUCCESS;
            }

            prev = current->Previous;
            next = current;
            current = prev;
        } while (prev != NULL);

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60007050
    // a.k.a. THRASH_treset
    DLLAPI u32 STDCALLAPI ResetTextures(void)
    {
        if (State.DX.Device == NULL) { return RENDERER_MODULE_FAILURE; }

        SelectTexture(NULL);

        if (State.Scene.IsActive) { AttemptRenderPackets(); }

        while (State.Textures.Current != NULL)
        {
            RendererTexture* tex = State.Textures.Current;

            State.Textures.Current = State.Textures.Current->Previous;

            ReleaseTexture(tex);
        }

        State.Textures.Current = NULL;

        State.Textures.Illegal = FALSE;

        BeginRendererScene();

        InitializeRenderState55();

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60006fb0
    // a.k.a. THRASH_tupdate
    DLLAPI RendererTexture* STDCALLAPI UpdateTexture(RendererTexture* tex, const u32* pixels, const u32* palette)
    {
        if (State.Lock.IsActive) { UnlockGameWindow(NULL); }

        if (tex != NULL && pixels != NULL) { return UpdateRendererTexture(tex, pixels, palette) ? tex : NULL; }

        return NULL;
    }

    // 0x60006ff0
    // a.k.a. THRASH_tupdaterect
    DLLAPI RendererTexture* STDCALLAPI UpdateTextureRectangle(RendererTexture* tex, const u32* pixels, const u32* palette, const s32 x, const s32 y, const s32 width, const s32 height, const s32 size, const s32 level)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }
    
    // 0x600016f0
    // a.k.a. THRASH_unlockwindow
    DLLAPI u32 STDCALLAPI UnlockGameWindow(const RendererModuleWindowLock* state)
    {
        if (State.Lock.IsActive && State.Lock.Surface != NULL)
        {
            if (State.Lock.Surface->UnlockRect() != D3D_OK) { return RENDERER_MODULE_FAILURE; }

            State.Lock.Surface = NULL;

            State.Lock.State.Data = NULL;
            State.Lock.State.Stride = 0;
            State.Lock.State.Format = RENDERER_PIXEL_FORMAT_NONE;
            State.Lock.State.Width = 0;
            State.Lock.State.Height = 0;

            State.Lock.IsActive = FALSE;

            if (State.Lambdas.Lambdas.LockWindow != NULL) { State.Lambdas.Lambdas.LockWindow(FALSE); }

            BeginRendererScene();
        }

        return RENDERER_MODULE_SUCCESS;
    }
    
    // 0x60001160
    // a.k.a. THRASH_window
    DLLAPI u32 STDCALLAPI SelectGameWindow(const u32 indx)
    {
        if (State.DX.Device == NULL) { return RENDERER_MODULE_FAILURE; }

        SelectRendererStateValue(RENDERER_MODULE_STATE_SELECT_GAME_WINDOW_INDEX, (void*)indx);

        if (State.Window.Index == indx) { return RENDERER_MODULE_SUCCESS; }

        IDirect3DSurface8* back = NULL;
        IDirect3DSurface8* target = NULL;

        if ((s32)indx < 0)
        {
            if (indx < MIN_WINDOW_INDEX) { return RENDERER_MODULE_FAILURE; }

            if ((State.Window.Count + MIN_WINDOW_INDEX) <= indx) { return RENDERER_MODULE_FAILURE; }

            IDirect3DSurface8* surface = State.Windows[indx].Surface->Surface;
            IDirect3DSurface8* stencil = State.Windows[indx].Stencil->Surface;

            if (surface == NULL) { return RENDERER_MODULE_FAILURE; }

            State.DX.Device->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &back);
            State.DX.Device->GetRenderTarget(&target);

            SelectObjectReferenceCount(back, 2);
            SelectObjectReferenceCount(target, 2);

            if (State.DX.Device->SetRenderTarget(surface, stencil) == D3D_OK)
            {
                State.Window.Index = indx;
                State.Window.Surface = surface;

                SelectObjectReferenceCount(surface, 1);
                SelectObjectReferenceCount(stencil, 1);

                return RENDERER_MODULE_SUCCESS;
            }
        }
        else
        {
            if ((MIN_WINDOW_INDEX - 1) < indx)
            {
                if ((State.Window.Count + MIN_WINDOW_INDEX) <= indx) { return RENDERER_MODULE_FAILURE; }

                IDirect3DSurface8* surface = State.Windows[indx].Surface->Surface;
                IDirect3DSurface8* stencil = State.Windows[indx].Stencil->Surface;

                if (surface == NULL) { return RENDERER_MODULE_FAILURE; }

                State.DX.Device->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &back);
                State.DX.Device->GetRenderTarget(&target);

                SelectObjectReferenceCount(back, 2);
                SelectObjectReferenceCount(target, 2);

                if (State.DX.Device->SetRenderTarget(surface, stencil) == D3D_OK)
                {
                    State.Window.Index = indx;
                    State.Window.Surface = surface;

                    SelectObjectReferenceCount(surface, 1);
                    SelectObjectReferenceCount(stencil, 1);

                    return RENDERER_MODULE_SUCCESS;
                }
            }

            if (State.DX.Surfaces.Surfaces[indx] == NULL && indx != 1) { return RENDERER_MODULE_FAILURE; } // TODO

            switch (indx)
            {
            case 2: // TODO
            {
                State.DX.Device->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &back);
                State.DX.Device->GetDepthStencilSurface(&target);

                SelectObjectReferenceCount(back, 2);
                SelectObjectReferenceCount(target, 2);

                if (State.DX.Device->SetRenderTarget(State.DX.Surfaces.Surfaces[2], State.DX.Surfaces.Surfaces[3]) == D3D_OK)
                {
                    State.Window.Index = 2; // TODO
                    State.Window.Surface = State.DX.Surfaces.Surfaces[2]; // TODO

                    SelectObjectReferenceCount(State.DX.Surfaces.Surfaces[2], 1); // TODO
                    SelectObjectReferenceCount(State.DX.Surfaces.Surfaces[3], 1); // TODO

                    return RENDERER_MODULE_SUCCESS;
                }

                break;
            }
            case 3: // TODO
            case 5: // TODO
            {
                State.Window.Index = indx;
                State.Window.Surface = State.DX.Surfaces.Surfaces[3]; // TODO

                return RENDERER_MODULE_SUCCESS;
            }
            default: { return RENDERER_MODULE_FAILURE; }
            }
        }

        SelectObjectReferenceCount(back, 1);
        SelectObjectReferenceCount(target, 1);

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60002f00
    // a.k.a. THRASH_writerect
    DLLAPI u32 STDCALLAPI WriteRectangle(const u32 x, const u32 y, const u32 width, const u32 height, const u32* pixels)
    {
        return WriteRectangles(x, y, width, height, pixels, 0);
    }

    // 0x60004b20
    // a.k.a. THRASH_writerect
    // NOTE: Never being called by the application.
    DLLAPI u32 STDCALLAPI WriteRectangles(const u32 x, const u32 y, const u32 width, const u32 height, const u32* pixels, const u32 stride)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }
}