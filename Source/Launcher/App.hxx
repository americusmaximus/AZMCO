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

#include "framework.h"

#define FILE_NAME_TEMPLATE _T("%s\\%s")

#define APP_FILE_NAME _T("mcity_d.exe")
#define INI_FILE_NAME _T("\\nofront.ini")
#define INI_TEMP_FILE_NAME _T("\\nofront.tmp")
#define INI_BACKUP_FILE_NAME _T("\\nofront.bak")
#define TEXT_FILE_NAME _T("\\mcitynofrontskinslist.txt")

#define TRACK_DIRECTORY_TEMPLATE _T("\\Data\\Tracks\\*.*")

#define SECTION_NAME_INTEGER_PROPERTY_VALUE_TEMPLATE _T("%s=%d")
#define SECTION_NAME_STRING_PROPERTY_VALUE_TEMPLATE _T("%s=%s")

#define CARS_SECTION_NAME _T("[cars]")
#define CARS_SECTION_PLAYER_ID_PROPERTY_NAME _T("playerID")
#define CARS_SECTION_SKIN_ID_PROPERTY_NAME _T("skinID")
#define CARS_SECTION_OPPONENT_ID_PROPERTY_NAME _T("opponentID")
#define CARS_SECTION_OPPONENT_SKIN_ID_PROPERTY_NAME _T("opponentSkinID")
#define CARS_SECTION_OPPONENT_COUNT_PROPERTY_NAME _T("numai")
#define CARS_SECTION_TRAFFIC_PROPERTY_NAME _T("traffic")

#define TRACK_SECTION_NAME _T("[track]")
#define TRACK_SECTION_INDEX_PROPERTY_NAME _T("index")
#define TRACK_SECTION_NIGHT_PROPERTY_NAME _T("night")
#define TRACK_SECTION_WEATHER_PROPERTY_NAME _T("weather")
#define TRACK_SECTION_MIRRORED_PROPERTY_NAME _T("mirrored")
#define TRACK_SECTION_BACKWARD_PROPERTY_NAME _T("backward")
#define TRACK_SECTION_LAPS_PROPERTY_NAME _T("laps")

#define GAME_SECTION_NAME _T("[game]")
#define GAME_SECTION_NO_DAMAGE_PROPERTY_NAME _T("nodamage")
#define GAME_SECTION_DIFFICULTY_PROPERTY_NAME _T("difficulty")
#define GAME_SECTION_RACE_MODE_PROPERTY_NAME _T("racemode")
#define GAME_SECTION_ROUND_COUNT_PROPERTY_NAME _T("rounds")
#define GAME_SECTION_HANDICAPPED_PROPERTY_NAME _T("handicapped")

#define CAR_LIST_SECTION_NAME _T("[carlist]")
#define CAR_LIST_ITEM_TEMAPLE _T("%s (#%d)")

#define ERROR_MESSAGE_ENTER_INTEGER _T("Please enter an integer.")
#define ERROR_MESSAGE_ENTER_INTEGER_0_25 _T("Please enter an integer between 0 and 25.")
#define ERROR_MESSAGE_ENTER_INTEGER_1_8 _T("Please enter an integer between 1 and 8.")

#define ERROR_MESSAGE_MISSING_APP_FILE _T("Unable to find file %s.\r\nPelase make sure the launcher is in the same directory as the game.")
#define ERROR_MESSAGE_MISSING_CONFIG_FILE _T("Unable to find file %s.\r\nYou must install Motor City Online Debug before running the launcher.")

#define ERROR_MESSAGE_UNABLE_CREATE_SETTINGS_FILE _T("Unable to update file nofront.ini")
#define ERROR_MESSAGE_UNABLE_CREATE_SETTINGS_FILE_COPY _T("Unable to create a backup copy of file nofront.bak")

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
            BOOL Same;

            int Car;
            int Skin;

            int Count;
            int Difficulty;
        } Opponents;

        CStringArray* Tracks;

        struct
        {
            CString* Track;

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

    enum SettingsSection : uint32_t
    {
        None = 0,
        Cars = 1,
        Track = 2,
        Game = 3
    };

    CStringArray* AcquireSettings(CString& path);
    CString* AlterSettings(CString& value);

    void Init(void);
    void InitializeCars(void);
    void InitializeSettings(void);
    void InitializeSkins(void);
    void InitializeTracks(void);

    void InitCarsSectionValue(CString& line);
    void InitTrackSectionValue(CString& line);
    void InitGameSectionValue(CString& line);

    BOOL SaveSettings(void);
    BOOL SaveSettingsCopy(void);
}