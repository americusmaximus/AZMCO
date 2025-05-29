/*
Copyright (c) 2025 Americus Maximus

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

#include "Base.hxx"

#include <d3d8.h>

struct DXS;

typedef HRESULT(__stdcall * QUERYINTERFACEACTION)(DXS*, REFIID, void**);
typedef ULONG(__stdcall * ADDREFACTION)(DXS*);
typedef ULONG(__stdcall * RELEASEACTION)(DXS*);

typedef HRESULT(__stdcall * GETDEVICEACTION)(DXS*, void**);
typedef HRESULT(__stdcall * SETPRIVATEDATAACTION)(DXS*, REFGUID, const void*, DWORD, DWORD);
typedef HRESULT(__stdcall * GETPRIVATEDATAACTION)(DXS*, REFGUID, void*, DWORD*);
typedef HRESULT(__stdcall * FREEPRIVATEDATAACTION)(DXS*, REFGUID);
typedef HRESULT(__stdcall * GETCONTAINERACTION)(DXS*, REFIID, void**);
typedef HRESULT(__stdcall * GETDESCACTION)(DXS*, D3DSURFACE_DESC*);
typedef HRESULT(__stdcall * LOCKRECTACTION)(DXS*, D3DLOCKED_RECT*, CONST RECT*, DWORD);
typedef HRESULT(__stdcall * UNLOCKRECTACTION)(DXS*);

struct DXSSelf
{
    /*** IUnknown ***/

    QUERYINTERFACEACTION    QueryInterface;
    ADDREFACTION            AddRef;
    RELEASEACTION           Release;

    /*** IDirect3DSurface8 ***/

    GETDEVICEACTION         GetDevice;
    SETPRIVATEDATAACTION    SetPrivateData;
    GETPRIVATEDATAACTION    GetPrivateData;
    FREEPRIVATEDATAACTION   FreePrivateData;
    GETCONTAINERACTION      GetContainer;
    GETDESCACTION           GetDesc;
    LOCKRECTACTION          LockRect;
    UNLOCKRECTACTION        UnlockRect;
};

struct DXS
{
    DXSSelf*        Self;
    D3DSURFACE_DESC Desc;
    LPVOID          Surface;
};

DXS* AllocateSurface(D3DSURFACE_DESC*);
void ReleaseSurface(DXS*);