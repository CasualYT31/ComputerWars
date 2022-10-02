/**
 * Holds options that will override options stored in a map file when loading the
 * map file.
 * If, for example, an army's ID isn't stored as a key in the \c currentCOs
 * dictionary, then that means whatever is stored in the map file will be used. If
 * an army ID is used as a key, then whatever is stored in the map file will be
 * overridden by the value after the map has loaded.
 */
class GameOptions {
	/**
	 * Stores current/primary CO overrides.
	 * The key is the \c ArmyID of the army whose current CO is to be overridden,
	 * converted into a string. Each value will be a \c string which stores the
	 * script name of the CO to override with.
	 */
	dictionary currentCOs;
	
	/**
	 * Stores tag/secondary CO overrides.
	 * The key is the \c ArmyID of the army whose tag CO is to be overridden,
	 * converted into a string. Each value will be a \c string which stores the
	 * script name of the CO to override with. Can be a blank string to represent
	 * "no tag CO."
	 */
	dictionary tagCOs;
	
	/**
	 * Stores team overrides.
	 * The key is the \c ArmyID of the army whose team is to be overridden,
	 * converted into a string. Each value will be a \c TeamID.
	 */
	dictionary teams;
}

/**
 * Represents a map with game logic attached.
 * <b>Note that all the HP values that are <em>given</em> to and <em>received</em>
 * from this class are user-friendly.</b>
 */
class PlayableMap {
	//////////////////
	// CONSTRUCTION //
	//////////////////
	/**
	 * Default constructor.
	 */
	PlayableMap() {}

	/**
	 * Constructs a playable map from a previously loaded map.
	 * @param mapToPlayOn The map to play on.
	 */
	PlayableMap(Map@ mapToPlayOn) {
		if (mapToPlayOn is null) {
			error("An invalid Map handle was given to the constructor of "
				"PlayableMap; the game will crash soon!");
		} else {
			@map = mapToPlayOn;
			setNormalCursorSprite();
		}
	}

	/**
	 * Overrides some of the map's data with other values.
	 * @param opts The data to override with.
	 * @sa    @c GameOptions.
	 */
	void overrideWithOptions(const GameOptions&in opts) {
		// Override current COs, and each of their paired tag COs, if they have
		// any.
		array<string>@ currentCOKeys = opts.currentCOs.getKeys();
		array<string>@ tagCOKeys = opts.tagCOs.getKeys();
		for (uint i = 0, keys = currentCOKeys.length(); i < keys; ++i) {
			string val;
			opts.currentCOs.get(currentCOKeys[i], val);
			if (opts.tagCOs.exists(currentCOKeys[i])) {
				string val2;
				opts.tagCOs.get(currentCOKeys[i], val2);
				map.setArmyCOs(parseUInt(currentCOKeys[i]), val, val2);
			} else {
				map.setArmyCurrentCO(parseUInt(currentCOKeys[i]), val);
			}
		}
		// Override any tag COs that didn't have a current CO override.
		for (uint i = 0, keys = tagCOKeys.length(); i < keys; ++i) {
			if (!opts.currentCOs.exists(tagCOKeys[i])) {
				string val2;
				opts.tagCOs.get(tagCOKeys[i], val2);
				map.setArmyTagCO(parseUInt(tagCOKeys[i]), val2);
			}
		}
		// Override teams.
		array<string>@ teamKeys = opts.teams.getKeys();
		for (uint i = 0, keys = teamKeys.length(); i < keys; ++i) {
			TeamID team;
			opts.teams.get(teamKeys[i], team);
			map.setArmyTeam(parseUInt(teamKeys[i]), team);
		}

	}

	////////////////////////
	// DRAWING OPERATIONS //
	////////////////////////
	/**
	 * Increases the map scaling factor by \c 1.0.
	 * The map scaling factor does not go above \c 3.0.
	 */
	void zoomIn() {
		_mapScalingFactor += 1.0;
		if (_mapScalingFactor > 3.0) _mapScalingFactor = 3.0;
		map.setMapScalingFactor(_mapScalingFactor);
	}
	
	/**
	 * Decreases the map scaling factor by \c 1.0.
	 * The map scaling factor does not go below \c 1.0.
	 */
	void zoomOut() {
		_mapScalingFactor -= 1.0;
		if (_mapScalingFactor < 1.0) _mapScalingFactor = 1.0;
		map.setMapScalingFactor(_mapScalingFactor);
	}

	/**
	 * Reverts the cursor back to the normal sprite.
	 */
	void setNormalCursorSprite() {
		map.setCursorSprite("cursor");
	}

	///////////////////////////////
	// TILE SELECTION OPERATIONS //
	///////////////////////////////
	/**
	 * Moves the cursor up one tile.
	 * Also updates the selected unit's closed list.
	 */
	void moveSelectedTileUp() {
		map.moveSelectedTileUp();
		_updateMoveModeClosedList();
	}
	
	/**
	 * Moves the cursor down one tile.
	 * Also updates the selected unit's closed list.
	 */
	void moveSelectedTileDown() {
		map.moveSelectedTileDown();
		_updateMoveModeClosedList();
	}
	
	/**
	 * Moves the cursor left one tile.
	 * Also updates the selected unit's closed list.
	 */
	void moveSelectedTileLeft() {
		map.moveSelectedTileLeft();
		_updateMoveModeClosedList();
	}
	
	/**
	 * Moves the cursor right one tile.
	 * Also updates the selected unit's closed list.
	 */
	void moveSelectedTileRight() {
		map.moveSelectedTileRight();
		_updateMoveModeClosedList();
	}
	
	/**
	 * Selects a tile based on a given pixel.
	 * Also updates the selected unit's closed list.
	 * @param pixel The tile underneath this pixel will be selected.
	 */
	void setSelectedTileByPixel(const MousePosition pixel) {
		map.setSelectedTileByPixel(pixel);
		_updateMoveModeClosedList();
	}

	////////////////////////////////
	// TURN MANAGEMENT OPERATIONS //
	////////////////////////////////
	/**
	 * Ends the current turn.
	 */
	void endTurn() {
		// 1. Ensure that each of the current army's units are no longer waiting.
		auto currentArmy = map.getSelectedArmy();
		const auto previousArmyUnits = map.getUnitsOfArmy(currentArmy);
		for (uint i = 0, count = previousArmyUnits.length(); i < count; ++i) {
			map.waitUnit(previousArmyUnits[i], false);
		}

		// 2. Update the current army to the next one in the list, and increment
		//    the day if necessary.
		const auto nextArmy = map.getNextArmy();
		if (currentArmy >= nextArmy) {
			map.setDay(map.getDay() + 1);
		}
		map.setSelectedArmy(nextArmy);
		currentArmy = nextArmy;

		// 3. Go through each of the current army's units and perform start of
		//    turn operations. Order each unit by their priority.
		const auto armyUnitsByPriority =
			map.getUnitsOfArmyByPriority(currentArmy);
		for (uint i = 0, priorityGroups = armyUnitsByPriority.length();
			i < priorityGroups; ++i) {
			for (uint j = 0, unitCount = armyUnitsByPriority[i].length();
			j < unitCount; ++j) {
				const auto unit = armyUnitsByPriority[i][j];
				_beginTurnForUnit(unit, map.getUnitType(unit),
					map.getUnitPosition(unit));
			}
		}

		// 4. Then, go through each of the current army's owned tiles.
		const auto armyTiles = map.getTilesOfArmy(currentArmy);
		for (uint i = 0, tileCount = armyTiles.length(); i < tileCount; ++i) {
			const Vector2 tile = armyTiles[i];
			_beginTurnForTile(tile, map.getTileType(tile).type, currentArmy);
		}

		// 5. Finally, perform extra operations on the army's units and tiles that
		//    need to be executed after the others. We shall get the lists again.
		//    This allows steps 3 and 4 to add or remove units or tiles if it so
		//    chooses.
		const auto finalUnits = map.getUnitsOfArmy(currentArmy);
		const auto finalTiles = map.getTilesOfArmy(currentArmy);
		_beginTurnForArmy(currentArmy, finalUnits, finalTiles);
	}

	/**
	 * Tags the current army's COs before ending the turn.
	 */
	void tagCOs() {
		map.tagArmyCOs(map.getSelectedArmy());
		endTurn();
	}

	////////////////////////////////
	// UNIT MANAGEMENT OPERATIONS //
	////////////////////////////////
	/**
	 * Buys a unit, gives it to the given army, and places it at the given
	 * location.
	 * @param  type     The type of unit to create.
	 * @param  army     The ID of the army who will own this unit, and the ID of
	 *                  the army who will be charged.
	 * @param  position The tile to place the unit on.
	 * @return \c TRUE if the army could afford the unit and the unit was created,
	 *         \c FALSE, otherwise.
	 * @throws If the unit could not be created. This will either be because the
	 *         army ID wasn't valid, or the given position was occupied.
	 */
	bool buyUnit(const UnitType&in type, const ArmyID army,
		const Vector2&in position) {
		if (map.getUnitOnTile(position) > 0) {
			throw("Could not buy unit of type \"" + type.scriptName + "\" for "
				"army " + formatUInt(army) + " on tile " + position.toString() +
				": this tile is occupied!");
		}
		Funds newFunds = map.getArmyFunds(army) - type.cost;
		if (newFunds < 0) return false;
		const auto unitID = map.createUnit(type.ID, army);
		if (unitID == 0) {
			throw("Could not buy unit of type \"" + type.scriptName + "\" for "
				"army " + formatUInt(army) + " on tile " + position.toString() +
				": this army ID is invalid!");
		}
		map.setUnitPosition(unitID, position);
		map.setArmyFunds(army, newFunds);
		map.setUnitHP(unitID, type.maxHP);
		if (!type.hasInfiniteFuel) {
			map.setUnitFuel(unitID, type.maxFuel);
		}
		if (!type.hasInfiniteAmmo) {
			map.setUnitAmmo(unitID, type.maxAmmo);
		}
		return true;
	}

	/**
	 * Adds HP to a unit.
	 * Ensures that a unit's HP does not exceed its maximum. Also charges the
	 * specified army for the heal.
	 * @param  unit The ID of the unit to heal.
	 * @param  hp   The user-friendly HP amount to heal by.
	 * @param  army The ID of the army to charge for the heal. If \c NO_ARMY is
	 *              given, no army will be charged. If an invalid ID is given, no
	 *              army will be charged, but the invalid parameter will be
	 *              logged, and the given unit's HP will only be healed up to the
	 *              maximum of its own HP (i.e. any "decimal" HP damage will be
	 *              healed but the visible HP won't increase).
	 * @throws If HP was at or below 0, or if the given unit ID was invalid.
	 */
	void healUnit(const UnitID unit, HP hp, const ArmyID army) {
		if (hp <= 0) {
			throw("HP given to the function was " + formatInt(hp) + ", which is "
				"invalid!");
		}
		const auto type = map.getUnitType(unit);
		HP newHP = map.getUnitHP(unit) + GetInternalHP(hp);
		if (uint(newHP) > type.maxHP) {
			hp -= GetDisplayedHP(newHP - type.maxHP);
			newHP - HP(type.maxHP);
		}
		if (army == NO_ARMY) {
			map.setUnitHP(unit, newHP);
		} else {
			// See if the army can afford the heal. If not, attempt to heal 1
			// less. If that doesn't work, keep going until we hit 0. At which
			// point, simply heal the unit back to "full health" for that HP (e.g.
			// set an internal HP of 57 to 60) and don't charge anything.
			const Funds currentFunds = map.getArmyFunds(army);
			while (true) {
				Funds charge = type.cost / GetDisplayedHP(type.maxHP) * hp;
				if (hp <= 0) {
					// Get internal HP. Then, convert it to user-friendly HP.
					// Finally, converting it back into internal HP should return
					// the full HP amount for the unit's current HP.
					map.setUnitHP(unit,
						GetInternalHP(GetDisplayedHP(map.getUnitHP(unit))));
					break;
				} else if (charge > currentFunds) {
					hp -= 1;
					newHP -= GetInternalHP(1);
				} else {
					map.setUnitHP(unit, newHP);
					map.setArmyFunds(army, currentFunds - charge);
					break;
				}
			}
		}
	}

	/**
	 * Replenishes a unit.
	 * @param  unit The ID of the unit to replenish.
	 * @throws If the given unit ID was invalid.
	 */
	void replenishUnit(const UnitID unit) {
		const auto type = map.getUnitType(unit);
		if (!type.hasInfiniteFuel) {
			map.setUnitFuel(unit, type.maxFuel);
		}
		if (!type.hasInfiniteAmmo) {
			map.setUnitAmmo(unit, type.maxAmmo);
		}
	}

	/**
	 * Selects a unit for movement mode.
	 * @param unit The ID of the unit to put into move mode. \c 0 should be given
	 *             if the currently selected unit is to be deselected.
	 */
	void selectUnit(const UnitID unit) {
		map.setSelectedUnit(unit);
		if (unit > 0) {
			map.setAvailableTileShader(AvailableTileShader::Yellow);
			_newClosedListNode(map.closedList, -1, map.getUnitPosition(unit), 0);
			map.renderUnitAtDestination(false);

			// Filter the available tiles down based on the unit's movement type,
			// movement points, and fuel.
			const auto tile = map.getUnitPosition(unit);
			const auto unitType = map.getUnitType(unit);
			const auto allTiles = map.getAvailableTiles(tile, 1,
				unitType.movementPoints);
			
			for (uint i = 0, length = allTiles.length(); i < length; ++i) {
				if (map.findPath(tile, allTiles[i], unitType.movementType,
					unitType.movementPoints, map.getUnitFuel(unit),
					map.getTeamOfUnit(unit)).length() > 0) {
					map.addAvailableTile(allTiles[i]);
				}
			}
			map.addAvailableTile(tile);
		}
	}

	/**
	 * Moves the currently selected unit along the chosen path and deselects the
	 * unit.
	 * @throws If a unit hasn't been selected.
	 */
	void moveUnit() {
		const auto id = map.getSelectedUnit();
		if (id > 0) {
			const auto node = map.closedList[map.closedList.length() - 1];
			map.burnUnitFuel(id, node.g);
			map.setUnitPosition(id, node.tile);
			map.waitUnit(id, true);
			selectUnit(0);
		} else {
			throw("Cannot move a unit as no unit is currently selected!");
		}
	}
	
	/////////
	// MAP //
	/////////
	/**
	 * The map.
	 * @warning Although read-write access to the map is given here, it is assumed
	 *          that the state of the map will be changed via \c PlayableMap's
	 *          operations wherever possible. Also, do not update the handle
	 *          itself. I would have made it constant if I could.
	 */
	Map@ map;

	/////////////////////////////
	// END TURN HELPER METHODS //
	/////////////////////////////
	/**
	 * When a turn ends, the next army's units will be given to this method.
	 * They are given in order of their turn start priority. The order of units
	 * with the same priority is not specified.
	 * @param unit     The ID of the unit.
	 * @param type     The type of unit that is identified by \c unit.
	 * @param position The position of the unit.
	 */
	private void _beginTurnForUnit(const UnitID unit, const UnitType&in type,
		const Vector2&in position) {
		string typeName = type.scriptName;

		if (typeName == "APC") {
			array<UnitID> units = _findUnits(position, 1, 1);
			auto apcArmy = map.getArmyOfUnit(unit);
			for (uint i = 0; i < units.length(); ++i) {
				if (apcArmy == map.getArmyOfUnit(units[i]))
					replenishUnit(units[i]);
			}

		} else if (typeName == "TCOPTER" || typeName == "BCOPTER") {
			map.burnUnitFuel(unit, 2);
		
		} else if (typeName == "FIGHTER" || typeName == "BOMBER" ||
			typeName == "STEALTH" || typeName == "BLACKBOMB") {
			map.burnUnitFuel(unit, 5);
		
		} else if (typeName == "BLACKBOAT" || typeName == "LANDER" ||
			typeName == "CRUISER" || typeName == "SUB" ||
			typeName == "BATTLESHIP" || typeName == "CARRIER") {
			map.burnUnitFuel(unit, 1);

		}
	}

	/**
	 * When a turn ends, the next army's tiles will be given to this method.
	 * @param tile        The location of the tile.
	 * @param terrain     The type of terrain this tile has.
	 * @param currentArmy The ID of the army who owns the tile.
	 */
	private void _beginTurnForTile(const Vector2&in tile,
		const Terrain&in terrain, const ArmyID currentArmy) {
		string terrainName = terrain.scriptName;

		if (terrainName == "CITY" || terrainName == "BASE" ||
			terrainName == "AIRPORT" || terrainName == "PORT" ||
			terrainName == "HQ") {
			map.offsetArmyFunds(currentArmy, 1000);
		}
	}

	/**
	 * Then an army's turn begins, all of their units and tiles get given to this
	 * method.
	 * @param currentArmy The ID of the army who owns the units and tiles.
	 * @param units       The array of units that belong to the current army.
	 * @param tiles       The array of tiles that belong to the current army.
	 */
	private void _beginTurnForArmy(const ArmyID currentArmy,
		const array<UnitID>@ units, const array<Vector2>@ tiles) {
		// Property heal and replenish code.
		const uint tilesSize = tiles.length();
		for (uint i = 0; i < tilesSize; ++i) {
			const UnitID unitOnTile = map.getUnitOnTile(tiles[i]);
			if (unitOnTile > 0) {
				if (map.getArmyOfUnit(unitOnTile) == currentArmy) {
					// Heal and replenish the unit based on the tile and unit's
					// movement type.
					const string unitType =
						map.getUnitType(unitOnTile).movementType.scriptName;
					const string terrainType =
						map.getTileType(tiles[i]).type.scriptName;
					if (terrainType == "CITY" || terrainType == "BASE" ||
						terrainType == "HQ") {
						if (unitType == "TREAD" || unitType == "TIRES" ||
							unitType == "INFANTRY" || unitType == "MECH" ||
							unitType == "PIPELINE") {
							healUnit(unitOnTile, 2, currentArmy);
							replenishUnit(unitOnTile);
						}
					} else if (terrainType == "AIRPORT") {
						if (unitType == "AIR") {
							healUnit(unitOnTile, 2, currentArmy);
							replenishUnit(unitOnTile);
						}
					} else if (terrainType == "PORT") {
						if (unitType == "SHIPS" || unitType == "TRANSPORT") {
							healUnit(unitOnTile, 2, currentArmy);
							replenishUnit(unitOnTile);
						}
					}
				}
			}
		}

		// Plane and ship crash checks.
		const uint unitsSize = units.length();
		for (uint i = 0; i < unitsSize; ++i) {
			// If unit has infinite fuel, skip fuel checks.
			const UnitType unit = map.getUnitType(units[i]);
			if (unit.hasInfiniteFuel) continue;

			string type = unit.movementType.scriptName;
			if (type == "AIR" || type == "SHIPS" || type == "TRANSPORT") {
				if (map.getUnitFuel(units[i]) <= 0) map.deleteUnit(units[i]);
			}
		}
	}

	//////////////////////////////////
	// TILE AND UNIT SEARCH METHODS //
	//////////////////////////////////
	/**
	 * Creates a list of units that are within the specified range of a given
	 * tile.
	 * @param  position  The tile to begin the search from.
	 * @param  startFrom The number of tiles away from the given tile to start on
	 *                   will always be at least \c 1.
	 * @param  endAt     The number of tiles away from the given tile where the
	 *                   search will end.
	 * @return An array of unit IDs.
	 */
	private array<UnitID>@ _findUnits(const Vector2&in position,
		const uint startFrom, const uint endAt) const {
		array<UnitID> ret;
		const auto tiles = map.getAvailableTiles(position, startFrom, endAt);
		for (uint i = 0, tileCount = tiles.length(); i < tileCount; ++i) {
			const auto unitID = map.getUnitOnTile(tiles[i]);
			if (unitID > 0) ret.insertLast(unitID);
		}
		return ret;
	}

	/**
	 * Updates the selected unit's closed list, if a unit is selected.
	 * If in move mode, and the selection has changed, and the selection is of an
	 * available tile, then attempt to append the currently selected tile to the
	 * closed list. If the path won't work, then deduce the shortest path and use
	 * that instead.
	 */
	funcdef void DEF(PlayableMap@, const Vector2&in, const UnitID,
		const UnitType&in);
	funcdef bool DEF2(Map@);
	private void _updateMoveModeClosedList() {
		// Declare the function used to revert back to the shortest path.
		DEF@ replaceWithShortestPath = function(pThis, tile, unitID, unitType){
			pThis.map.closedList.removeRange(0, pThis.map.closedList.length());
			const auto arr = pThis.map.findPath(
				pThis.map.getUnitPosition(unitID),
				tile,
				unitType.movementType,
				unitType.movementPoints,
				pThis.map.getUnitFuel(unitID),
				pThis.map.getTeamOfUnit(unitID)
			);
			for (uint i = 0, length = arr.length(); i < length; ++i) {
				pThis._newClosedListNode(pThis.map.closedList, -1, arr[i].tile,
					arr[i].g);
			}
			pThis.map.regenerateClosedListSprites();
		};
		// Checks if the distance between any two adjacent tiles in the closed
		// list is more than one.
		// If any distance between any two adjacent tiles in the closed list is
		// more than one, then we need to fix up the list. The easiest way to do
		// this is to just find the shortest path. The closed list can get into
		// this state if the mouse is moved very quickly, for example.
		DEF2@ isPathDisjoint = function(map){
			for (uint i = 1, length = map.closedList.length(); i < length; ++i) {
				if (Distance(map.closedList[i - 1].tile,
					map.closedList[i].tile) > 1) {
					return true;
				}
			}
			return false;
		};

		// Find data.
		const auto tile = map.getSelectedTile();
		const auto unitID = map.getSelectedUnit();
		if (unitID > 0 && map.isAvailableTile(tile)) {
			const auto unitType = map.getUnitType(unitID);

			// If the selection has changed...
			if (tile != map.closedList[map.closedList.length() - 1].tile) {

				if (isPathDisjoint(map)) {
					replaceWithShortestPath(this, tile, unitID, unitType);
					return;
				}

				// If a tile has been selected that's already in the closed list,
				// then simply remove all tiles from the closed list that come
				// after it.
				for (uint i = 0, length = map.closedList.length();
					i < length; ++i) {
					if (map.closedList[i].tile == tile && i < length - 1) {
						map.closedList.removeRange(i + 1, length - i - 1);
						map.regenerateClosedListSprites();
						return;
					}
				}

				// Attempt to append the currently selected tile to the end of the
				// closed list. If it can't be done, then just find the shortest
				// path and use that instead (we already know it should be
				// traversable since it's an available tile).
				const int tentativeGScore =
					map.closedList[map.closedList.length() - 1].g +
					map.getTileType(tile).type.moveCost[
						unitType.movementTypeIndex];
				if (tentativeGScore <= map.getUnitFuel(unitID) &&
					uint(tentativeGScore) <= unitType.movementPoints) {
					_newClosedListNode(map.closedList, -1, tile, tentativeGScore);

					// Because we've just appended a new node without checking it
					// thoroughly, the user could have moved their cursor off the
					// available tiles and joined back into the available tiles
					// again to a form a completely disjointed path. So we need to
					// carry out distance checking again!
					if (isPathDisjoint(map)) {
						replaceWithShortestPath(this, tile, unitID, unitType);
					} else {
						map.regenerateClosedListSprites();
					}
				} else {
					replaceWithShortestPath(this, tile, unitID, unitType);
				}
			}
		}
	}

	/**
	 * Inserts a new \c ClosedListNode into an array of <tt>ClosedListNode</tt>s.
	 * @param arr   Reference to the array to insert the node into.
	 * @param index If <= 0, will insert the new element at the specified index.
	 *              If < 0, will append the new element.
	 * @param tile  The tile to give to the new node.
	 * @param g     The G score to give to the new node.
	 */
	void _newClosedListNode(array<ClosedListNode>& arr, int64 index,
		const Vector2&in tile, const int g) {
		if (index < 0) {
			arr.insertLast(ClosedListNode());
			index = int64(arr.length() - 1);
		} else {
			arr.insertAt(uint(index), ClosedListNode());
		}
		arr[uint(index)].tile = tile;
		arr[uint(index)].g = g;
	}

	//////////
	// DATA //
	//////////
	/**
	 * The map scaling factor.
	 */
	private float _mapScalingFactor = 2.0;
}