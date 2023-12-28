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

#include "pch.h"

namespace App
{
    LauncherAppState AppState;

    void Init(void)
    {
        AppState.Cars = new CStringArray();
        AppState.Skins = new CStringArray();

        AppState.Difficulties = new CStringArray();

        AppState.Difficulties->Add(_T("Easy"));
        AppState.Difficulties->Add(_T("Medium"));
        AppState.Difficulties->Add(_T("Hard"));

        AppState.Tracks = new CStringArray();

        InitializeTracks();
    }

    void InitializeTracks(void)
    {
        TCHAR szDirectory[MAX_PATH] = _T("");
        GetCurrentDirectory(MAX_PATH - 1, szDirectory);

        CString path = CString(szDirectory);
        path.Append(TRACK_DIRECTORY_TEMPLATE);

        CFileFind finder;

        BOOL found = finder.FindFile(path);

        while (found)
        {
            found = finder.FindNextFile();

            if (!finder.IsDots() && !finder.IsHidden())
            {
                AppState.Tracks->Add(finder.GetFileName());
            }
        }

        finder.Close();
    }
}