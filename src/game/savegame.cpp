//       _________ __                 __
//      /   _____//  |_____________ _/  |______     ____  __ __  ______
//      \_____  \\   __\_  __ \__  \\   __\__  \   / ___\|  |  \/  ___/
//      /        \|  |  |  | \// __ \|  |  / __ \_/ /_/  >  |  /\___ |
//     /_______  /|__|  |__|  (____  /__| (____  /\___  /|____//____  >
//             \/                  \/          \//_____/            \/
//  ______________________                           ______________________
//                        T H E   W A R   B E G I N S
//         Stratagus - A free fantasy real time strategy game engine
//
/**@name savegame.cpp - Save game. */
//
//      (c) Copyright 2001-2015 by Lutz Sammer, Andreas Arens and Andrettin
//
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; only version 2 of the License.
//
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
//      02111-1307, USA.
//

//@{

/*----------------------------------------------------------------------------
--  Includes
----------------------------------------------------------------------------*/

#include "stratagus.h"

#include "game.h"

#include "actions.h"
#include "ai.h"
#include "iolib.h"
#include "map.h"
#include "missile.h"
#include "parameters.h"
#include "player.h"
#include "replay.h"
#include "spells.h"
#include "trigger.h"
#include "ui.h"
#include "unit.h"
#include "unit_manager.h"
#include "unittype.h"
#include "upgrade.h"
#include "version.h"

#include <ctime>

extern void StartMap(const std::string &filename, bool clean);


/*----------------------------------------------------------------------------
--  Variables
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
--  Functions
----------------------------------------------------------------------------*/

fs::path ExpandPath(const std::string &path)
{
	if (path[0] == '~') {
		fs::path newpath = Parameters::Instance.GetUserDirectory();
		if (!GameName.empty()) {
			newpath /= GameName;
		}
		return newpath / path.substr(1);
	} else {
		return fs::path(StratagusLibPath) / path;
	}
}

/**
** Get the save directory and create dirs if needed
*/
static fs::path GetSaveDir()
{
	fs::path dir(Parameters::Instance.GetUserDirectory());
	if (!GameName.empty()) {
		dir /= GameName;
	}
	dir /= "save";
	fs::create_directories(dir);
	return dir;
}

/**
**  Save a game to file.
**
**  @param filename  File name to be stored.
**  @return  -1 if saving failed, 0 if all OK
**
**  @note  Later we want to store in a more compact binary format.
*/
int SaveGame(const std::string &filename)
{
	CFile file;
	fs::path fullpath(GetSaveDir());

	fullpath /= filename;
	if (file.open(fullpath.string().c_str(), CL_WRITE_GZ | CL_OPEN_WRITE) == -1) {
		ErrorPrint("Can't save to '%s'\n", filename.c_str());
		return -1;
	}

	time_t now;
	char dateStr[64];

	time(&now);
	const struct tm *timeinfo = localtime(&now);
	strftime(dateStr, sizeof(dateStr), "%c", timeinfo);

	// Load initial level // Without units
	file.printf("local oldCreateUnit = CreateUnit\n");
	file.printf("local oldSetResourcesHeld = SetResourcesHeld\n");
	file.printf("local oldSetTile = SetTile\n");
	file.printf("function CreateUnit() end\n");
	file.printf("function SetResourcesHeld() end\n");
	file.printf("function SetTile() end\n");
	file.printf("Load(\"%s\")\n", Map.Info.Filename.c_str());
	file.printf("CreateUnit = oldCreateUnit\n");
	file.printf("SetResourcesHeld = oldSetResourcesHeld\n");
	file.printf("SetTile = oldSetTile\n");
	//
	// Parseable header
	//
	file.printf("SavedGameInfo({\n");
	file.printf("---  \"comment\", \"Generated by Stratagus Version " VERSION "\",\n");
	file.printf("---  \"comment\", \"Visit https://github.com/Wargus/stratagus for more information\",\n");
	file.printf("---  \"type\",    \"%s\",\n", "single-player");
	file.printf("---  \"date\",    \"%s\",\n", dateStr);
	file.printf("---  \"map\",     \"%s\",\n", Map.Info.Description.c_str());
	file.printf("---  \"media-version\", \"%s\"", "Undefined");
	file.printf("---  \"engine\",  {%d, %d, %d},\n",
				StratagusMajorVersion, StratagusMinorVersion, StratagusPatchLevel);
	file.printf("  SyncHash = %d, \n", SyncHash);
	file.printf("  SyncRandSeed = %d, \n", SyncRandSeed);
	file.printf("  SaveFile = \"%s\"\n", CurrentMapPath);
	file.printf("\n---  \"preview\", \"%s.pam\",\n", filename.c_str());
	file.printf("} )\n\n");

	// FIXME: probably not the right place for this
	file.printf("GameCycle = %lu\n", GameCycle);

	file.printf("SetGodMode(%s)\n", GodMode ? "true" : "false");

	SaveUnitTypes(file);
	SaveUpgrades(file);
	SavePlayers(file);
	Map.Save(file);
	UnitManager->Save(file);
	SaveUserInterface(file);
	SaveAi(file);
	SaveSelections(file);
	SaveGroups(file);
	SaveMissiles(file);
	SaveReplayList(file);
	SaveGameSettings(file);
	// FIXME: find all state information which must be saved.
	const std::string s = SaveGlobal(Lua);
	if (!s.empty()) {
		file.printf("-- Lua state\n\n %s\n", s.c_str());
	}
	SaveTriggers(file); //Triggers are saved in SaveGlobal, so load it after Global
	file.close();
	return 0;
}

/**
**  Delete save game
**
**  @param filename  Name of file to delete
*/
void DeleteSaveGame(const std::string &filename)
{
	// Security check
	if (filename.find_first_of("/\\") != std::string::npos) {
		return;
	}

	fs::path fullpath = GetSaveDir() / filename;
	if (!fs::remove(fullpath)) {
		ErrorPrint("delete failed for '%s'", fullpath.u8string().c_str());
	}
}

void StartSavedGame(const std::string &filename)
{
	SaveGameLoading = true;
	CleanPlayers();
	LoadGame(ExpandPath(filename));

	StartMap(filename, false);
	//SetDefaultTextColors(nc, rc);
}

//@}
