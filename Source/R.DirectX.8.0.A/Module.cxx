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
#include "Module.hxx"
#include "RendererValues.hxx"
#include "Settings.hxx"

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
            State.DX.Surfaces.Window == State.DX.Surfaces.Surfaces[3]);
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
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60001810
    // a.k.a. THRASH_destroywindow
    // NOTE: Never being called by the application.
    DLLAPI u32 STDCALLAPI DestroyGameWindow(const u32 indx)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
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
        if (State.Settings.Cull == 1
            || ((u32)AcquireNormal((f32x3*)a, (f32x3*)b, (f32x3*)c) & 0x80000000) != State.Settings.Cull) // TODO
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
        // TODO NOT IMPLEMENTED

        return NULL;
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
        // TODO NOT IMPLEMENTED

        return NULL;
    }

    // 0x600013e0
    // a.k.a. THRASH_pageflip
    DLLAPI u32 STDCALLAPI ToggleGameWindow(void)
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

        return BeginRendererScene();
    }

    // 0x60002ed0
    // a.k.a. THRASH_readrect
    DLLAPI u32 STDCALLAPI ReadRectangle(const u32 x, const u32 y, const u32 width, const u32 height, u32* pixels)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
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
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60004bc0
    // a.k.a. THRASH_setstate
    DLLAPI u32 STDCALLAPI SelectState(const u32 state, void* value)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x600019f0
    // a.k.a. THRASH_settexture
    DLLAPI u32 STDCALLAPI SelectTexture(RendererTexture* tex)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
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
        if (type == 0) // TODO
        {
            LockGameWindow();
            UnlockGameWindow(NULL);
        }
        else if (type == 2) // TODO
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
        }

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60006d90
    // a.k.a. THRASH_talloc
    DLLAPI RendererTexture* STDCALLAPI AllocateTexture(const u32 width, const u32 height, const u32 format, const u32 options, const u32 state)
    {
        // TODO NOT IMPLEMENTED

        return NULL;
    }

    // 0x60006f20
    // a.k.a. THRASH_tfree
    DLLAPI u32 STDCALLAPI ReleaseTexture(RendererTexture* tex)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60007050
    // a.k.a. THRASH_treset
    DLLAPI u32 STDCALLAPI ResetTextures(void)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60006fb0
    // a.k.a. THRASH_tupdate
    DLLAPI RendererTexture* STDCALLAPI UpdateTexture(RendererTexture* tex, const u32* pixels, const u32* palette)
    {
        // TODO NOT IMPLEMENTED

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
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60001160
    // a.k.a. THRASH_window
    DLLAPI u32 STDCALLAPI SelectGameWindow(const u32 indx)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60002f00
    // a.k.a. THRASH_writerect
    DLLAPI u32 STDCALLAPI WriteRectangle(const u32 x, const u32 y, const u32 width, const u32 height, const u32* pixels)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
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