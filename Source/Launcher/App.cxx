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

#include "pch.h"

namespace App
{
    LauncherAppState AppState;

    void Init(void)
    {
        InitializeSettings();

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

    CStringArray* AcquireSettings(CString& path)
    {
        CStringArray* result = new CStringArray();

        CStdioFile file;

        if (file.Open(path.GetString(), CFile::modeRead))
        {
            CString line;

            while (file.ReadString(line)) { result->Add(line); }

            file.Close();
        }

        return result;
    }

    void InitializeCars(void)
    {
        TCHAR szDirectory[MAX_PATH] = _T("");
        GetCurrentDirectory(MAX_PATH - 1, szDirectory);

        CString path(szDirectory);
        path.Append(INI_FILE_NAME);

        BOOL found = FALSE;
        CStringArray* lines = AcquireSettings(path);

        for (uint32_t x = 0; x < lines->GetCount(); x++)
        {
            CString line = ((CString)lines->GetAt(x)).Trim();

            if (!found)
            {
                if (line.CompareNoCase(CAR_LIST_SECTION_NAME) == 0) { found = TRUE; }
            }
            else
            {
                if (line.GetAt(0) == _T('[')) { break; }

                if (!line.IsEmpty())
                {
                    const int pos = line.Find(_T('='), 0);

                    if (pos >= 0)
                    {
                        CString idString = line.Left(pos);
                        CString valueString = line.Right(line.GetLength() - pos - 1);

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
        }

        delete lines;
    }

    void InitializeSettings(void)
    {
        TCHAR szDirectory[MAX_PATH] = _T("");
        GetCurrentDirectory(MAX_PATH - 1, szDirectory);

        CString path(szDirectory);
        path.Append(INI_FILE_NAME);

        SettingsSection section = SettingsSection::None;
        CStringArray* lines = AcquireSettings(path);

        for (uint32_t x = 0; x < lines->GetCount(); x++)
        {
            CString line = CString(lines->GetAt(x)).Trim();

            if (line.GetAt(0) == _T('['))
            {
                if (line.CompareNoCase(CARS_SECTION_NAME) == 0) { section = SettingsSection::Cars; continue; }
                else if (line.CompareNoCase(TRACK_SECTION_NAME) == 0) { section = SettingsSection::Track; continue; }
                else if (line.CompareNoCase(GAME_SECTION_NAME) == 0) { section = SettingsSection::Game; continue; }
                else { section = SettingsSection::None; continue; }
            }

            switch (section)
            {
            case App::Cars: { InitCarsSectionValue(line); break; }
            case App::Track: { InitTrackSectionValue(line); break; }
            case App::Game: { InitGameSectionValue(line); break; }
            }
        }

        delete lines;
    }

    void InitializeSkins(void)
    {
        TCHAR szDirectory[MAX_PATH] = _T("");
        GetCurrentDirectory(MAX_PATH - 1, szDirectory);

        CString path(szDirectory);
        path.Append(TEXT_FILE_NAME);

        CStringArray* lines = AcquireSettings(path);

        for (uint32_t x = 0; x < lines->GetCount(); x++)
        {
            CString line = CString(lines->GetAt(x)).Trim();

            const int first = line.Find(_T(' '), 0);

            if (first == -1) { continue; }

            int car = 0;
            int skin = 0;

            // Car
            {
                CString carString = line.Left(first).Trim();

                const int count = _stscanf_s(carString, _T("%d"), &car);

                if (count != 1) { continue; }
            }

            line = line.Right(line.GetLength() - first - 1).Trim();

            const int second = line.Find(_T(' '), first + 1);

            if (second == -1) { continue; }

            // Skin
            {
                CString skinString = line.Left(second).Trim();

                const int count = _stscanf_s(skinString, _T("%d"), &skin);

                if (count != 1) { continue; }
            }

            if (car == 0 || skin == 0) { continue; }

            Skin* skn = new Skin();

            skn->ID = skin;
            skn->Name = new CString(line.Right(line.GetLength() - line.Find(_T(' '))).Trim());

            for (uint32_t x = 0; x < AppState.Cars->GetCount(); x++)
            {
                Car* c = AppState.Cars->GetAt(x);

                if (c->ID == car) { c->Skins->Add(skn); break; }
            }
        }

        delete lines;
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

    void InitCarsSectionValue(CString& line)
    {
        const int pos = line.FindOneOf(_T(" ="));

        if (pos == -1) { return; }

        CString name = line.Left(pos);
        CString value = line.Right(line.GetLength() - max(line.ReverseFind(_T(' ')), line.ReverseFind(_T('='))) - 1);

        int number = 0;

        {
            const int count = _stscanf_s(value, _T("%d"), &number);

            if (count != 1) { return; }
        }

        if (name.CompareNoCase(CARS_SECTION_PLAYER_ID_PROPERTY_NAME) == 0)
        {
            AppState.Player.Car = number;

            AppState.Opponents.Same = AppState.Player.Car == AppState.Opponents.Car && AppState.Player.Skin == AppState.Opponents.Skin;
        }
        else if (name.CompareNoCase(CARS_SECTION_SKIN_ID_PROPERTY_NAME) == 0)
        {
            AppState.Player.Skin = number;

            AppState.Opponents.Same = AppState.Player.Car == AppState.Opponents.Car && AppState.Player.Skin == AppState.Opponents.Skin;
        }
        else if (name.CompareNoCase(CARS_SECTION_OPPONENT_ID_PROPERTY_NAME) == 0)
        {
            AppState.Opponents.Car = number;

            AppState.Opponents.Same = AppState.Player.Car == AppState.Opponents.Car && AppState.Player.Skin == AppState.Opponents.Skin;
        }
        else if (name.CompareNoCase(CARS_SECTION_OPPONENT_SKIN_ID_PROPERTY_NAME) == 0)
        {
            AppState.Opponents.Skin = number;

            AppState.Opponents.Same = AppState.Player.Car == AppState.Opponents.Car && AppState.Player.Skin == AppState.Opponents.Skin;
        }
        else if (name.CompareNoCase(CARS_SECTION_OPPONENT_COUNT_PROPERTY_NAME) == 0) { AppState.Opponents.Count = number; }
        else if (name.CompareNoCase(CARS_SECTION_TRAFFIC_PROPERTY_NAME) == 0) { AppState.Track.Traffic = number != 0; }
    }

    void InitTrackSectionValue(CString& line)
    {
        const int pos = line.FindOneOf(_T(" ="));

        if (pos == -1) { return; }

        CString name = line.Left(pos);
        CString value = line.Right(line.GetLength() - max(line.ReverseFind(_T(' ')), line.ReverseFind(_T('='))) - 1);

        int number = 0;

        if (name.CompareNoCase(TRACK_SECTION_INDEX_PROPERTY_NAME) != 0)
        {
            const int count = _stscanf_s(value, _T("%d"), &number);

            if (count != 1) { return; }
        }

        if (name.CompareNoCase(TRACK_SECTION_INDEX_PROPERTY_NAME) == 0) { AppState.Track.Track = new CString(value); }
        else if (name.CompareNoCase(TRACK_SECTION_NIGHT_PROPERTY_NAME) == 0) { AppState.Track.Night = number != 0; }
        else if (name.CompareNoCase(TRACK_SECTION_WEATHER_PROPERTY_NAME) == 0) { AppState.Track.Weather = number != 0; }
        else if (name.CompareNoCase(TRACK_SECTION_MIRRORED_PROPERTY_NAME) == 0) { AppState.Track.Mirrored = number != 0; }
        else if (name.CompareNoCase(TRACK_SECTION_BACKWARD_PROPERTY_NAME) == 0) { AppState.Track.Backward = number != 0; }
        else if (name.CompareNoCase(TRACK_SECTION_LAPS_PROPERTY_NAME) == 0) { AppState.Track.Laps = max(1, min(8, number)); }
    }

    void InitGameSectionValue(CString& line)
    {
        const int pos = line.FindOneOf(_T(" ="));

        if (pos == -1) { return; }

        CString name = line.Left(pos);
        CString value = line.Right(line.GetLength() - max(line.ReverseFind(_T(' ')), line.ReverseFind(_T('='))) - 1);

        int number = 0;

        {
            const int count = _stscanf_s(value, _T("%d"), &number);

            if (count != 1) { return; }
        }

        if (name.CompareNoCase(GAME_SECTION_NO_DAMAGE_PROPERTY_NAME) == 0) { AppState.Track.NoDamage = number != 0; }
        else if (name.CompareNoCase(GAME_SECTION_DIFFICULTY_PROPERTY_NAME) == 0) { AppState.Opponents.Difficulty = number; }
        else if (name.CompareNoCase(GAME_SECTION_RACE_MODE_PROPERTY_NAME) == 0) { AppState.Player.Mode = max(0, min(25, number)); }
    }

    CString* AlterSettings(CString& value)
    {
        CString* result = new CString(value);

        const BOOL indx = result->Find(_T('='), 0);

        if (indx < 0) { return result; }

        CString name = result->Left(indx);

        if (name.CompareNoCase(CARS_SECTION_PLAYER_ID_PROPERTY_NAME) == 0)
        {
            result->Format(SECTION_NAME_INTEGER_PROPERTY_VALUE_TEMPLATE, CARS_SECTION_PLAYER_ID_PROPERTY_NAME, AppState.Player.Car);
        }
        else if (name.CompareNoCase(CARS_SECTION_SKIN_ID_PROPERTY_NAME) == 0)
        {
            result->Format(SECTION_NAME_INTEGER_PROPERTY_VALUE_TEMPLATE, CARS_SECTION_SKIN_ID_PROPERTY_NAME, AppState.Player.Skin);
        }
        else if (name.CompareNoCase(CARS_SECTION_OPPONENT_ID_PROPERTY_NAME) == 0)
        {
            result->Format(SECTION_NAME_INTEGER_PROPERTY_VALUE_TEMPLATE, CARS_SECTION_OPPONENT_ID_PROPERTY_NAME, AppState.Opponents.Car);
        }
        else if (name.CompareNoCase(CARS_SECTION_OPPONENT_SKIN_ID_PROPERTY_NAME) == 0)
        {
            result->Format(SECTION_NAME_INTEGER_PROPERTY_VALUE_TEMPLATE, CARS_SECTION_OPPONENT_SKIN_ID_PROPERTY_NAME, AppState.Opponents.Skin);
        }
        else if (name.CompareNoCase(CARS_SECTION_OPPONENT_COUNT_PROPERTY_NAME) == 0)
        {
            result->Format(SECTION_NAME_INTEGER_PROPERTY_VALUE_TEMPLATE, CARS_SECTION_OPPONENT_COUNT_PROPERTY_NAME, AppState.Opponents.Count);
        }
        else if (name.CompareNoCase(CARS_SECTION_TRAFFIC_PROPERTY_NAME) == 0)
        {
            result->Format(SECTION_NAME_INTEGER_PROPERTY_VALUE_TEMPLATE, CARS_SECTION_TRAFFIC_PROPERTY_NAME, AppState.Track.Traffic);
        }
        else if (name.CompareNoCase(TRACK_SECTION_INDEX_PROPERTY_NAME) == 0)
        {
            result->Format(SECTION_NAME_STRING_PROPERTY_VALUE_TEMPLATE, TRACK_SECTION_INDEX_PROPERTY_NAME, AppState.Track.Track->GetString());
        }
        else if (name.CompareNoCase(TRACK_SECTION_NIGHT_PROPERTY_NAME) == 0)
        {
            result->Format(SECTION_NAME_INTEGER_PROPERTY_VALUE_TEMPLATE, TRACK_SECTION_NIGHT_PROPERTY_NAME, AppState.Track.Night);
        }
        else if (name.CompareNoCase(TRACK_SECTION_WEATHER_PROPERTY_NAME) == 0)
        {
            result->Format(SECTION_NAME_INTEGER_PROPERTY_VALUE_TEMPLATE, TRACK_SECTION_WEATHER_PROPERTY_NAME, AppState.Track.Weather);
        }
        else if (name.CompareNoCase(TRACK_SECTION_MIRRORED_PROPERTY_NAME) == 0)
        {
            result->Format(SECTION_NAME_INTEGER_PROPERTY_VALUE_TEMPLATE, TRACK_SECTION_MIRRORED_PROPERTY_NAME, AppState.Track.Mirrored);
        }
        else if (name.CompareNoCase(TRACK_SECTION_BACKWARD_PROPERTY_NAME) == 0)
        {
            result->Format(SECTION_NAME_INTEGER_PROPERTY_VALUE_TEMPLATE, TRACK_SECTION_BACKWARD_PROPERTY_NAME, AppState.Track.Backward);
        }
        else if (name.CompareNoCase(TRACK_SECTION_LAPS_PROPERTY_NAME) == 0)
        {
            result->Format(SECTION_NAME_INTEGER_PROPERTY_VALUE_TEMPLATE, TRACK_SECTION_LAPS_PROPERTY_NAME, AppState.Track.Laps);
        }
        else if (name.CompareNoCase(GAME_SECTION_NO_DAMAGE_PROPERTY_NAME) == 0)
        {
            result->Format(SECTION_NAME_INTEGER_PROPERTY_VALUE_TEMPLATE, GAME_SECTION_NO_DAMAGE_PROPERTY_NAME, AppState.Track.NoDamage);
        }
        else if (name.CompareNoCase(GAME_SECTION_DIFFICULTY_PROPERTY_NAME) == 0)
        {
            result->Format(SECTION_NAME_INTEGER_PROPERTY_VALUE_TEMPLATE, GAME_SECTION_DIFFICULTY_PROPERTY_NAME, AppState.Opponents.Difficulty);
        }
        else if (name.CompareNoCase(GAME_SECTION_RACE_MODE_PROPERTY_NAME) == 0)
        {
            result->Format(SECTION_NAME_INTEGER_PROPERTY_VALUE_TEMPLATE, GAME_SECTION_RACE_MODE_PROPERTY_NAME, AppState.Player.Mode);
        }

        return result;
    }

    BOOL SaveSettings(void)
    {
        TCHAR szDirectory[MAX_PATH] = _T("");
        GetCurrentDirectory(MAX_PATH - 1, szDirectory);

        CString original(szDirectory);
        original.Append(INI_FILE_NAME);

        CStringArray* lines = AcquireSettings(original);

        CString copy(szDirectory);
        copy.Append(INI_TEMP_FILE_NAME);

        CFile file;
        BOOL result = FALSE;

        if (file.Open(copy, CFile::modeCreate | CFile::modeWrite))
        {
            for (uint32_t x = 0; x < lines->GetCount(); x++)
            {
                CString* str = AlterSettings(CString(lines->GetAt(x)).Trim());

                {
                    file.Write(CT2CA(str->GetString(), CP_ACP), str->GetLength());
                    file.Write("\r\n", 2);
                }

                delete str;
            }

            file.Close();

            result = CopyFile(copy.GetString(), original.GetString(), FALSE);

            if (result) { result = DeleteFile(copy.GetString()); }
        }

        delete lines;

        return result;
    }

    BOOL SaveSettingsCopy(void)
    {
        TCHAR szDirectory[MAX_PATH] = _T("");
        GetCurrentDirectory(MAX_PATH - 1, szDirectory);

        CString original(szDirectory);
        original.Append(INI_FILE_NAME);

        CString backup(szDirectory);
        backup.Append(INI_BACKUP_FILE_NAME);

        return CopyFile(original.GetString(), backup.GetString(), FALSE);
    }
}