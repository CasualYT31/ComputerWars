/**
 * @file 0CWM.as
 * Code used to read and write Computer Wars map files, specifically map files
 * stored using the first version of the CWM format.
 */

/* The following is pseudocode describing the structure of the format:
CWMFileVersion version;
string mapName;
string environmentScriptName;
uint32 mapWidth;
uint32 mapHeight;
uint32 selectedTileX;
uint32 selectedTileY;
Day currentDay;
bool fowEnabled;
uint32 scriptCount;
foreach script {
    string name;
    string code;
}
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
    string structureScriptName; // An empty string if this tile does not form part
                                // of a structure.
    int32 structureTileOffsetX;
    int32 structureTileOffsetY;
    bool structureIsDestroyed;
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
	 * Deserialises unit data from a binary stream.
	 * @param  stream   Handle to the binary stream to read from.
	 * @param  map      Handle to the map object to update.
	 * @param  tile     The tile to place the read unit onto.
	 * @param  loadOnto The ID of the unit to load the read unit onto. If
     *                  \c NO_UNIT is given, the read unit will not be loaded onto
     *                  any unit.
     * @throws You are permitted to throw exceptions to signify unrecoverable
     *         errors.
	 */
	bool LoadMapUnit(BinaryIStream@ stream, Map@ map, const Vector2&in tile,
		const UnitID loadOnto = NO_UNIT) {
		ArmyID unitOwner;
		stream.read(unitOwner);
		if (unitOwner == NO_ARMY) {
			return false;
		} else {
			string unitTypeScriptName;
			stream.read(unitTypeScriptName);
			const auto unitID = map.createUnit(unitTypeScriptName, unitOwner);
			if (unitID != NO_UNIT) {
				if (loadOnto != NO_UNIT) {
					map.loadUnit(unitID, loadOnto);
				} else {
					map.setUnitPosition(unitID, tile);
				}
				HP hp;
				stream.read(hp);
				map.setUnitHP(unitID, hp);
				Fuel fuel;
				stream.read(fuel);
				map.setUnitFuel(unitID, fuel);
				const auto type = unittype[unitTypeScriptName];
				for (uint64 i = 0, weaponCount = type.weaponCount;
					i < weaponCount; ++i) {
					Ammo ammo;
					stream.read(ammo);
					map.setUnitAmmo(unitID, type.weapon(i).scriptName, ammo);
				}
				bool isWaiting;
				stream.read(isWaiting);
				map.waitUnit(unitID, isWaiting);
				bool isCapturing;
				stream.read(isCapturing);
				map.unitCapturing(unitID, isCapturing);
				bool isHiding;
				stream.read(isHiding);
				map.unitHiding(unitID, isHiding);
				while (LoadMapUnit(stream, map, tile,
					((loadOnto != NO_UNIT) ? (loadOnto) : (unitID))));
			} else {
				throw("read above");
			}
			return true;
		}
	}

	/**
	 * Updates a map object based on the given binary stream's contents.
	 * The engine will clear the state of the map object before invoking this
	 * function. You will not be able to perform write operations.
	 * @param  stream Handle to the binary stream to read from.
	 * @param  map    Handle to the map object to update.
	 * @param  format A number indicating which format the binary stream's data is
	 *                expected to take.
     * @throws You are permitted to throw exceptions to signify unrecoverable
     *         errors.
	 */
	void LoadMap(BinaryIStream@ stream, Map@ map, uint8 format) {
		// For now, ignore the first four bytes. In the future, this will signify
		// the version of the file format used, but whilst I'm still working
		// towards a full release, it's not worth tracking differences in
		// versions.
		CWMFileVersion fileVersion; stream.read(fileVersion);
		string mapName;
		stream.read(mapName);
		map.setMapName(mapName);
        string environmentScriptName;
        stream.read(environmentScriptName);
        map.setEnvironment(environmentScriptName);
		uint32 width, height, selX, selY, armyCount;
		stream.read(width);
		stream.read(height);
		// Find the first tile type and use that. They will get replaced with
		// setTileType() later.
		string firstTileType =
			((tiletype.length() > 0) ? (tiletype.scriptNames[0]) : (""));
		// If we couldn't find one, then setMapSize() will cause a crash, so we
		// need to throw now.
		if (firstTileType == "")
			throw("No tile types were configured; failed to load map!");
		map.setMapSize(Vector2(width, height), firstTileType);
		stream.read(selX);
		stream.read(selY);
		map.setSelectedTile(Vector2(selX, selY));
		Day day;
		stream.read(day);
		map.setDay(day);
        bool fowEnabled;
        stream.read(fowEnabled);
        map.enableFoW(fowEnabled);
        uint32 scriptCount;
        stream.read(scriptCount);
        for (uint32 i = 0; i < scriptCount; ++i) {
            string scriptName, scriptCode;
            stream.read(scriptName);
            stream.read(scriptCode);
            map.addScriptFile(scriptName, scriptCode);
        }
        if (scriptCount > 0) map.buildScriptFiles();
		stream.read(armyCount);
		for (uint64 army = 0; army < armyCount; ++army) {
			string countryScriptName;
			stream.read(countryScriptName);
			if (map.createArmy(countryScriptName)) {
				const auto armyID = country[countryScriptName].turnOrder;
				TeamID team;
				stream.read(team);
				map.setArmyTeam(armyID, team);
				Funds funds;
				stream.read(funds);
				map.setArmyFunds(armyID, funds);
				string currentCOScriptName;
				stream.read(currentCOScriptName);
				string tagCOScriptName;
				stream.read(tagCOScriptName);
				map.setArmyCOs(armyID, currentCOScriptName, tagCOScriptName);
			} else {
				throw("read above");
			}
		}
		ArmyID currentArmy;
		stream.read(currentArmy);
        if (armyCount > 0) map.setSelectedArmy(currentArmy);
		for (uint32 y = 0; y < height; ++y) {
			for (uint32 x = 0; x < width; ++x) {
				Vector2 tilePos(x, y);
				string tileTypeScriptName;
				stream.read(tileTypeScriptName);
				if (map.setTileType(tilePos, tileTypeScriptName)) {
					HP tileHP;
					stream.read(tileHP);
					map.setTileHP(tilePos, tileHP);
					ArmyID tileOwner;
					stream.read(tileOwner);
					map.setTileOwner(tilePos, tileOwner);
                    string structureScriptName;
                    stream.read(structureScriptName);
                    int32 offsetX, offsetY;
                    stream.read(offsetX);
                    stream.read(offsetY);
                    bool destroyed;
                    stream.read(destroyed);
                    if (!structureScriptName.isEmpty()) {
                        map.setTileStructureData(tilePos, structureScriptName,
                            MousePosition(offsetX, offsetY), destroyed);
                    }
					LoadMapUnit(stream, map, tilePos);
				} else {
					throw("read above");
				}
			}
		}
	}

	/**
	 * Serialises unit data to a binary stream.
	 * @param  stream Handle to the binary stream to write to.
	 * @param  map    Handle to the map object to read from.
	 * @param  unit   The ID of the unit to write data of.
     * @throws You are permitted to throw exceptions to signify unrecoverable
     *         errors.
	 */
	void SaveMapUnit(BinaryOStream@ stream, Map@ map, const UnitID unit) {
		stream.write(map.getArmyOfUnit(unit));
		const auto type = map.getUnitType(unit);
		stream.write(type.scriptName);
		stream.write(map.getUnitHP(unit));
		stream.write(map.getUnitFuel(unit));
		for (uint64 i = 0, weaponCount = type.weaponCount; i < weaponCount; ++i) {
			stream.write(map.getUnitAmmo(unit, type.weapon(i).scriptName));
		}
		stream.write(map.isUnitWaiting(unit));
		stream.write(map.isUnitCapturing(unit));
		stream.write(map.isUnitHiding(unit));
		const auto loadedUnits = map.getLoadedUnits(unit);
		const auto loadedUnitCount = loadedUnits.length();
		for (uint i = 0; i < loadedUnitCount; ++i) {
			SaveMapUnit(stream, map, loadedUnits[i]);
		}
		stream.write(NO_ARMY);
	}

	/**
	 * Serialises a map object to a binary stream.
	 * The given stream will be empty.
	 * You are permitted to throw exceptions to signify unrecoverable errors.
	 * @param stream Handle to the binary stream to write to.
	 * @param map    Handle to the map object to update.
	 * @param format A number indicating which format the binary stream's data
	 *               must take.
	 */
	void SaveMap(BinaryOStream@ stream, Map@ map, uint8 format) {
		// For now, save the same file version every time. See LoadMap().
		stream.write(LATEST_CWM_FILE_VERSION);
		stream.write(map.getMapName());
        stream.write(map.getEnvironment().scriptName);
		const auto mapSize = map.getMapSize();
		stream.write(mapSize.x);
		stream.write(mapSize.y);
		const auto cursor = map.getSelectedTile();
		stream.write(cursor.x);
		stream.write(cursor.y);
		stream.write(map.getDay());
        stream.write(map.isFowEnabled());
        const auto scriptFileNames = map.getScriptNames();
        const uint scriptFileCount = scriptFileNames.length();
        stream.write(scriptFileCount);
        for (uint32 i = 0; i < scriptFileCount; ++i) {
            const string scriptName = scriptFileNames[i];
            stream.write(scriptName);
            stream.write(map.getScript(scriptName));
        }
		const auto armyCount = map.getArmyCount();
		stream.write(armyCount);
		const auto armyIDs = map.getArmyIDs();
		for (uint i = 0; i < armyCount; ++i) {
			const auto armyID = armyIDs[i];
			stream.write(map.getArmyCountry(armyID).scriptName);
			stream.write(map.getArmyTeam(armyID));
			stream.write(map.getArmyFunds(armyID));
			stream.write(map.getArmyCurrentCO(armyID));
			stream.write(map.getArmyTagCO(armyID));
		}
		stream.write(map.getSelectedArmy());
		for (uint32 y = 0; y < mapSize.y; ++y) {
			for (uint32 x = 0; x < mapSize.x; ++x) {
				Vector2 tilePos(x, y);
				stream.write(map.getTileType(tilePos).scriptName);
				stream.write(map.getTileHP(tilePos));
				stream.write(map.getTileOwner(tilePos));
                if (map.isTileAStructureTile(tilePos)) {
                    stream.write(map.getTileStructure(tilePos).scriptName);
                    const auto offset = map.getTileStructureOffset(tilePos);
                    const auto destroyed = map.isTileDestroyed(tilePos);
                    stream.write(offset.x);
                    stream.write(offset.y);
                    stream.write(destroyed);
                } else {
                    stream.write("");
                    stream.write(int32(0));
                    stream.write(int32(0));
                    stream.write(false);
                }
				const auto unitID = map.getUnitOnTile(tilePos);
				if (unitID != NO_UNIT) {
					SaveMapUnit(stream, map, unitID);
				} else {
					stream.write(NO_ARMY);
				}
			}
		}
	}
}
