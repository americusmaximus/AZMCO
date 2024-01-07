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
        AppState.Cars = new CArray<Car*, Car*>();

        InitializeCars();
        InitializeSkins();

        AppState.Difficulties = new CStringArray();

        AppState.Difficulties->Add(_T("Easy"));
        AppState.Difficulties->Add(_T("Medium"));
        AppState.Difficulties->Add(_T("Hard"));

        AppState.Tracks = new CStringArray();

        InitializeTracks();
    }

    void InitializeCars(void)
    {
        TCHAR szDirectory[MAX_PATH] = _T("");
        GetCurrentDirectory(MAX_PATH - 1, szDirectory);

        CString path(szDirectory);
        path.Append(INI_FILE_NAME);

        CStdioFile file;

        if (file.Open(path.GetString(), CFile::modeRead))
        {
            BOOL found = FALSE;
            CString line;

            while (file.ReadString(line))
            {
                CString trimmed = line.Trim();

                if (!found)
                {
                    if (trimmed.CompareNoCase(CAR_LIST_SECTION_NAME) == 0)
                    {
                        found = TRUE;

                        continue;
                    }
                }
                else if(!trimmed.IsEmpty() && trimmed.GetAt(0) != _T('['))
                {
                    int pos = trimmed.Find(_T('='), 0);

                    if (pos >= 0)
                    {
                        CString idString = trimmed.Left(pos);
                        CString valueString = trimmed.Right(trimmed.GetLength() - pos - 1);

                        int id = 0;
                        int count = _stscanf_s(idString, _T("%d"), &id);

                        if (count != 1) { continue; }

                        Car* car = new Car();

                        car->ID = id;
                        car->Name = new CString(valueString);
                        car->Skins = new CArray<Skin*, Skin*>();

                        Skin* skin = new Skin();

                        skin->ID = 0;
                        skin->Name = new CString(_T("Random"));

                        car->Skins->Add(skin);

                        AppState.Cars->Add(car);
                    }
                }
            }

            file.Close();
        }
    }

    void InitializeSkins(void)
    {
        // TODO
    }

    void InitializeTracks(void)
    {
        TCHAR szDirectory[MAX_PATH] = _T("");
        GetCurrentDirectory(MAX_PATH - 1, szDirectory);

        CString path(szDirectory);
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