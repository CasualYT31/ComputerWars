/**
 * @file 0CWM.as
 * Code used to read and write Computer Wars map files, specifically map files
 * stored using the first version of the CWM format.
 */

/* The following is pseudocode describing the structure of the format:
CWMFileVersion version;
string mapName;
uint32 mapWidth;
uint32 mapHeight;
uint32 selectedTileX;
uint32 selectedTileY;
Day currentDay;
uint32 armyCount;
foreach army {
	string countryScriptName;
	TeamID;
	Funds;
	// Will be blank strings if there isn't e.g. a tag CO.
	string currentCOScriptName;
	string tagCOScriptName;
}
ArmyID currentArmy;
foreach tile {
	// Tiles are read and written from the uppermost leftmost tile, to the
	// rightmost tile, and then from top to bottom.
	string tileTypeScriptName;
	HP; // E.g. either city's capture points or cannon's HP.
	ArmyID tileOwner;
	ArmyID unitOwner;
	if (unitOwner != NO_ARMY) {
		string unitTypeScriptName;
		HP;
		Fuel;
		foreach weapon belonging to a unit {
			// In the same order as found when using getWeaponByIndex().
			Ammo;
		}
		bool isWaiting;
		bool isCapturing;
		bool isHiding;
		if there are loaded units {
			while (READ(ArmyID loadedUnitOwner) != NO_ARMY) {
				goto "string unitTypeScriptName";
			}
		}
	}
}
*/

/**
 * Contains code relating to the Computer Wars map file format.
 */
namespace cwm {
	/**
	 * Represents a CWM file format version.
	 */
	typedef uint32 CWMFileVersion;

	/**
	 * Version number of the CWM format representing the very first version.
	 * \c 1297564416 is the 32-bit integer value representing "[NUL]", "C", "W",
	 * and "M". Adding a number of up to 255 to this value's first byte allows
	 * different versions of the CWM format to be checked for easily.
	 */
	const CWMFileVersion FIRST_CWM_FILE_VERSION = 1297564416;

	/**
	 * The latest version of the CWM format.
	 */
	const CWMFileVersion LATEST_CWM_FILE_VERSION = 1297564416;

	/**
	 * Deserialises unit data from a binary file.
	 * You are permitted to throw exceptions to signify unrecoverable errors.
	 * @param file     Handle to the binary file to read from.
	 * @param map      Handle to the map object to update.
	 * @param tile     The tile to place the read unit onto.
	 * @param loadOnto The ID of the unit to load the read unit onto. If \c 0 is
	 *                 given, the read unit will not be loaded onto any unit.
	 */
	bool LoadMapUnit(BinaryFile@ file, Map@ map, const Vector2&in tile,
		const UnitID loadOnto = 0) {
		ArmyID unitOwner;
		file.read(unitOwner);
		if (unitOwner == NO_ARMY) {
			return false;
		} else {
			string unitTypeScriptName;
			file.read(unitTypeScriptName);
			const auto unitID = map.createUnit(unitTypeScriptName, unitOwner);
			if (unitID > 0) {
				// Setting the unit's position needs to come before, at least,
				// reading the capturing property. This is because, if a unit is
				// capturing something, setting its position will try to set the
				// unit's old tile's HP to its max. But the unit doesn't have a
				// previous position, causing a read access violation. Maybe I
				// should at least add a safe guard in the engine code for this
				// case...
				if (loadOnto > 0) {
					map.loadUnit(unitID, loadOnto);
				} else {
					map.setUnitPosition(unitID, tile);
				}
				HP hp;
				file.read(hp);
				map.setUnitHP(unitID, hp);
				Fuel fuel;
				file.read(fuel);
				map.setUnitFuel(unitID, fuel);
				const auto type = unittype[unitTypeScriptName];
				for (uint64 i = 0, weaponCount = type.weaponCount;
					i < weaponCount; ++i) {
					Ammo ammo;
					file.read(ammo);
					map.setUnitAmmo(unitID, type.weapon(i).scriptName, ammo);
				}
				bool isWaiting;
				file.read(isWaiting);
				map.waitUnit(unitID, isWaiting);
				bool isCapturing;
				file.read(isCapturing);
				map.unitCapturing(unitID, isCapturing);
				bool isHiding;
				file.read(isHiding);
				map.unitHiding(unitID, isHiding);
				while (LoadMapUnit(file, map, tile,
					((loadOnto > 0) ? (loadOnto) : (unitID))));
			} else {
				throw("read above");
			}
			return true;
		}
	}

	/**
	 * Loads a binary file and updates a map object based on the file's contents.
	 * The engine will clear the state of the map object before invoking this
	 * function. You will not be able to perform write operations.\n
	 * You are permitted to throw exceptions to signify unrecoverable errors.
	 * @param file   Handle to the binary file to read from.
	 * @param map    Handle to the map object to update.
	 * @param format A number indicating which format the binary file's data is
	 *               expected to take.
	 */
	void LoadMap(BinaryFile@ file, Map@ map, uint8 format) {
		// For now, ignore the first four bytes. In the future, this will signify
		// the version of the file format used, but whilst I'm still working
		// towards a full release, it's not worth tracking differences in
		// versions.
		CWMFileVersion fileVersion; file.read(fileVersion);
		string mapName;
		file.read(mapName);
		map.setMapName(mapName);
		uint32 width, height, selX, selY, armyCount;
		file.read(width);
		file.read(height);
		// Find the first tile type and use that. They will get replaced with
		// setTileType() later.
		string firstTileType =
			((tiletype.length() > 0) ? (tiletype.scriptNames[0]) : (""));
		// If we couldn't find one, then setMapSize() will cause a crash, so we
		// need to throw now.
		if (firstTileType == "")
			throw("No tile types were configured; failed to load map!");
		map.setMapSize(Vector2(width, height), firstTileType);
		file.read(selX);
		file.read(selY);
		map.setSelectedTile(Vector2(selX, selY));
		Day day;
		file.read(day);
		map.setDay(day);
		file.read(armyCount);
		for (uint64 army = 0; army < armyCount; ++army) {
			string countryScriptName;
			file.read(countryScriptName);
			if (map.createArmy(countryScriptName)) {
				const auto armyID = country[countryScriptName].turnOrder;
				TeamID team;
				file.read(team);
				map.setArmyTeam(armyID, team);
				Funds funds;
				file.read(funds);
				map.setArmyFunds(armyID, funds);
				string currentCOScriptName;
				file.read(currentCOScriptName);
				string tagCOScriptName;
				file.read(tagCOScriptName);
				map.setArmyCOs(armyID, currentCOScriptName, tagCOScriptName);
			} else {
				throw("read above");
			}
		}
		ArmyID currentArmy;
		file.read(currentArmy);
        if (armyCount > 0) map.setSelectedArmy(currentArmy);
		for (uint32 y = 0; y < height; ++y) {
			for (uint32 x = 0; x < width; ++x) {
				Vector2 tilePos(x, y);
				string tileTypeScriptName;
				file.read(tileTypeScriptName);
				if (map.setTileType(tilePos, tileTypeScriptName)) {
					HP tileHP;
					file.read(tileHP);
					map.setTileHP(tilePos, tileHP);
					ArmyID tileOwner;
					file.read(tileOwner);
					map.setTileOwner(tilePos, tileOwner);
					LoadMapUnit(file, map, tilePos);
				} else {
					throw("read above");
				}
			}
		}
	}

	/**
	 * Serialises unit data to a binary file.
	 * You are permitted to throw exceptions to signify unrecoverable errors.
	 * @param file Handle to the binary file to write to.
	 * @param map  Handle to the map object to read from.
	 * @param unit The ID of the unit to write data of.
	 */
	void SaveMapUnit(BinaryFile@ file, Map@ map, const UnitID unit) {
		file.write(map.getArmyOfUnit(unit));
		const auto type = map.getUnitType(unit);
		file.write(type.scriptName);
		file.write(map.getUnitHP(unit));
		file.write(map.getUnitFuel(unit));
		for (uint64 i = 0, weaponCount = type.weaponCount; i < weaponCount; ++i) {
			file.write(map.getUnitAmmo(unit, type.weapon(i).scriptName));
		}
		file.write(map.isUnitWaiting(unit));
		file.write(map.isUnitCapturing(unit));
		file.write(map.isUnitHiding(unit));
		const auto loadedUnits = map.getLoadedUnits(unit);
		const auto loadedUnitCount = loadedUnits.length();
		for (uint i = 0; i < loadedUnitCount; ++i) {
			SaveMapUnit(file, map, loadedUnits[i]);
		}
		file.write(NO_ARMY);
	}

	/**
	 * Serialises a map object to a binary file.
	 * The given file will be empty. You will not be able to perform read
	 * operations.\n
	 * You are permitted to throw exceptions to signify unrecoverable errors.
	 * @param file   Handle to the binary file to write to.
	 * @param map    Handle to the map object to update.
	 * @param format A number indicating which format the binary file's data must
	 *               take.
	 */
	void SaveMap(BinaryFile@ file, Map@ map, uint8 format) {
		// For now, save the same file version every time. See LoadMap().
		file.write(LATEST_CWM_FILE_VERSION);
		file.write(map.getMapName());
		const auto mapSize = map.getMapSize();
		file.write(mapSize.x);
		file.write(mapSize.y);
		const auto cursor = map.getSelectedTile();
		file.write(cursor.x);
		file.write(cursor.y);
		file.write(map.getDay());
		const auto armyCount = map.getArmyCount();
		file.write(armyCount);
		const auto armyIDs = map.getArmyIDs();
		for (uint i = 0; i < armyCount; ++i) {
			const auto armyID = armyIDs[i];
			file.write(map.getArmyCountry(armyID).scriptName);
			file.write(map.getArmyTeam(armyID));
			file.write(map.getArmyFunds(armyID));
			file.write(map.getArmyCurrentCO(armyID));
			file.write(map.getArmyTagCO(armyID));
		}
		file.write(map.getSelectedArmy());
		for (uint32 y = 0; y < mapSize.y; ++y) {
			for (uint32 x = 0; x < mapSize.x; ++x) {
				Vector2 tilePos(x, y);
				file.write(map.getTileType(tilePos).scriptName);
				file.write(map.getTileHP(tilePos));
				file.write(map.getTileOwner(tilePos));
				const auto unitID = map.getUnitOnTile(tilePos);
				if (unitID > 0) {
					SaveMapUnit(file, map, unitID);
				} else {
					file.write(NO_ARMY);
				}
			}
		}
	}
}
