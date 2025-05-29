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

#include "DXS.hxx"

// Always allocate 2048 x 2048 x 32 BPP

#define IAMGE_BPP           4
#define IMAGE_DIMS          2048
#define MAX_SURFACE_SIZE    (IMAGE_DIMS * IMAGE_DIMS * IAMGE_BPP)
#define MAX_SURFACE_PITCH   (IMAGE_DIMS * IAMGE_BPP)

HRESULT __stdcall GetDesc(DXS* self, D3DSURFACE_DESC* desc) {
    if (self == NULL || desc == NULL) { return D3DERR_INVALIDCALL; }

    CopyMemory(desc, &self->Desc, sizeof(D3DSURFACE_DESC));

    return D3D_OK;
}

HRESULT __stdcall LockRect(DXS* self, D3DLOCKED_RECT* desc, CONST RECT* rect, DWORD)
{
    if (self == NULL || desc == NULL) { return D3DERR_INVALIDCALL; }

    desc->Pitch = MAX_SURFACE_PITCH;

    desc->pBits = rect == NULL
        ? self->Surface
        : (LPVOID)((addr)(self->Surface) + (addr)(rect->left * IAMGE_BPP + rect->top * MAX_SURFACE_PITCH));

    return D3D_OK;
}

HRESULT __stdcall UnlockRect(DXS*) { return D3D_OK; }

static DXSSelf Self = {
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    (GETDESCACTION)GetDesc, (LOCKRECTACTION)LockRect, (UNLOCKRECTACTION)UnlockRect
};

DXS* AllocateSurface(D3DSURFACE_DESC* desc)
{
    if (desc == NULL) { return NULL; }

    DXS* surface = (DXS*)malloc(sizeof(DXS));

    if (surface == NULL) { return NULL; }

    ZeroMemory(surface, sizeof(DXS));

    surface->Self = &Self;
    CopyMemory(&surface->Desc, desc, sizeof(D3DSURFACE_DESC));

    surface->Surface = malloc(MAX_SURFACE_SIZE);
    
    if (surface->Surface == NULL)
    {
        free(surface);

        return NULL;
    }

    ZeroMemory(surface->Surface, MAX_SURFACE_SIZE);

    return surface;
}

void ReleaseSurface(DXS* surface)
{
    if (surface == NULL) { return; }

    if (surface->Surface != NULL) { free(surface->Surface); }

    free(surface);
}