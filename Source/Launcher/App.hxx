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

#include "framework.h"

#define FILE_NAME_TEMPLATE _T("%s\\%s")

#define APP_FILE_NAME _T("mcity_d.exe")
#define INI_FILE_NAME _T("\\nofront.ini")
#define INI_TEMP_FILE_NAME _T("\\nofront.tmp")
#define INI_BACKUP_FILE_NAME _T("\\nofront.bak")
#define TEXT_FILE_NAME _T("\\mcitynofrontskinslist.txt")

#define TRACK_DIRECTORY_TEMPLATE _T("\\Data\\Tracks\\*.*")

#define CAR_LIST_SECTION_NAME _T("[carlist]")
#define CAR_LIST_ITEM_TEMAPLE _T("%s (#%d)")

#define ERROR_MESSAGE_MISSING_APP_FILE _T("Unable to find file %s.\r\nPelase make sure the launcher is in the same directory as the game.")
#define ERROR_MESSAGE_MISSING_CONFIG_FILE _T("Unable to find file %s.\r\nYou must install Motor City Online Debug before running the launcher.")

namespace App
{
    struct Skin
    {
        int ID;
        CString* Name;
    };

    struct Car
    {
        int ID;
        CString* Name;
        CArray<Skin*, Skin*>* Skins;
    };

    struct LauncherAppState
    {
        CArray<Car*, Car*>* Cars;

        struct
        {
            int Car;

            int Skin;
            int Mode;
        } Player;

        CStringArray* Difficulties;

        struct
        {
            BOOL SameAsPlayer;

            int Car;
            int Skin;
            int Count;
            int Difficulty;
        } Opponents;

        CStringArray* Tracks;

        struct
        {
            LPCSTR Track;
            int Laps;

            BOOL Night;
            BOOL Weather;
            BOOL NoDamage;
            BOOL Mirrored;
            BOOL Backward;
            BOOL Traffic;
        } Track;
    };

    extern LauncherAppState AppState;

    void Init(void);
    void InitializeCars(void);
    void InitializeSkins(void);
    void InitializeTracks(void);
}