/**
 * @file EditableMap.as
 * Defines most of the map maker logic.
 */

/**
 * An object that observes the state of an \c EditableMap and is notified of
 * changes.
 */
interface Observer {
    /**
     * When the map changes state, it should call this method on observers.
     * @param data Optional data passed to the observer. Left empty unless
     *             otherwise specified in \c Subject documentation.
     */
    void refresh(any&in data = any());
}

/**
 * Defines the different types of observers of \c EditableMap.
 */
enum Subject {
    /// Map property changes.
    Properties,
    /// Tile changes.
    /// Passes in a \c Vector2 which locates the tile that's been selected using
    /// the tileinfo control (and whose information has been updated). Passes in
    /// an empty \c any object when it's being deselected.
    Tiles,
    /// Army changes.
    Armies,
    /// Script changes.
    Scripts,
    /// Misc. changes related to the main status bar.
    /// Passes in a \c float which stores the map scaling factor.
    Status,
    /// The number of subjects.
    Count
}

/**
 * Each map operation that can be performed in or on \c EditableMap.
 * @sa \c OPERATION.
 */
enum Operation {
    // Internal.
    MAP_PROPS,
    CREATE_ARMY_SCRIPT,
    DELETE_ARMY_SCRIPT,
    TILE_TYPE_AND_OWNER,
    CREATE_UNIT_SCRIPT,
    CREATE_LOAD_UNIT,
    PAINT_STRUCTURE,
    FIX_TILES,
    MAP_SIZE,
    RECT_FILL_TILES,
    TILE_OWNER,
    
    // External.
    PAINT_TERRAIN_TOOL,
    PAINT_TILE_TOOL,
    PAINT_UNIT_TOOL,
    DELETE_UNIT_TOOL,
    RECT_PAINT_TERRAIN_TOOL,
    RECT_PAINT_TILE_TOOL,
    RECT_PAINT_UNIT_TOOL,
    RECT_DELETE_UNIT_TOOL
}

/**
 * Maps \c Operation values to translation keys.
 * @sa \c Operation.
 */
const array<string> OPERATION = {
    // Internal.
    "OPERATION_mapprops",
    "OPERATION_createarmy",
    "OPERATION_deletearmy",
    "OPERATION_tiletypeandowner",
    "OPERATION_createunit",
    "OPERATION_createloadunit",
    "OPERATION_paintstructure",
    "OPERATION_fixtiles",
    "OPERATION_mapsize",
    "OPERATION_rectfilltiles",
    "OPERATION_tileowner",

    // External.
    "OPERATION_paintterraintool",
    "OPERATION_painttiletool",
    "OPERATION_paintunittool",
    "OPERATION_deleteunittool",
    "OPERATION_rectpaintterraintool",
    "OPERATION_rectpainttiletool",
    "OPERATION_rectpaintunittool",
    "OPERATION_rectdeleteunittool"
};

/**
 * Represents a map with game logic attached.
 * <b>Note that all the HP values that are <em>given</em> to and <em>received</em>
 * from this class are internal.</b>
 */
class EditableMap {
    //////////////////
    // CONSTRUCTION //
    //////////////////
    /**
     * Constructs an editable map from a previously loaded map.
     * @param mapToEdit    The map to edit.
     */
    EditableMap(Map@ mapToEdit) {
        observers.resize(Subject::Count);
        if (mapToEdit is null) {
            error("An invalid Map handle was given to the constructor of "
                "EditableMap; the game will crash soon!");
        } else {
            @map = mapToEdit;
            map.enableAnimations(false);
            map.enablePeriodic(false);
            map.alwaysShowHiddenUnits(true);
            map.setMapScalingFactor(_mapScalingFactor, false);
            map.setULAdditionalCursorSprite("ulanglebracket");
            map.setURAdditionalCursorSprite("uranglebracket");
            map.setLLAdditionalCursorSprite("llanglebracket");
            map.setLRAdditionalCursorSprite("lranglebracket");
            setNormalCursorSprites();
            _updateStatusBar();
        }
    }

    /**
     * Sets an observer.
     * @param s The type of data to observe.
     * @param o The observer.
     */
    void setObserver(const Subject s, Observer@ const o) {
        @observers[s] = o;
    }

    ////////////////////////
    // MEMENTO OPERATIONS //
    ////////////////////////
    /**
     * Undo an operation.
     * Also updates all windows.
     * @param additionalUndos The number of additional undos to make.
     */
    void undo(const uint64 additionalUndos = 0) {
        map.undo(additionalUndos);
        refreshTileProps();
        _updateArmyProps();
        _updateMapProps();
        _updateStatusBar();
        _updateScriptsWindow();
    }
    
    /**
     * Redo an operation.
     * Also updates all windows.
     * @param additionalRedos The number of additional redos to make.
     */
    void redo(const uint64 additionalRedos = 0) {
        map.redo(additionalRedos);
        refreshTileProps();
        _updateArmyProps();
        _updateMapProps();
        _updateStatusBar();
        _updateScriptsWindow();
    }

    //////////////////////////
    // SCRIPTING OPERATIONS //
    //////////////////////////
    /**
     * Adds a new script to the map file, or updates an existing one.
     * @param name The name of the script file.
     * @param code The code of the script file.
     */
    void addScriptFile(const string&in name, const string&in code) {
        map.addScriptFile(name, code);
        _updateScriptsWindow();
    }

    /**
     * Same as \c addScriptFile(), except the scripts window will not be
     * refreshed, and a memento will not be created (but the state will still be
     * marked as changed).
     * If a non-existent script is given, an error will be logged and no new
     * script will be added.
     */
    void updateScriptFile(const string&in name, const string&in code) {
        if (!map.doesScriptExist(name)) {
            error("Attempted to update non-existent script file \"" + name +
                "\"!");
            return;
        }
        map.disableMementos();
        map.addScriptFile(name, code);
        map.enableMementos("");
        map.markChanged();
    }
    
    /**
     * Renames an existing script.
     * @param oldName The name of the existing script file.
     * @param newName The new name to assign to it.
     */
    void renameScriptFile(const string&in oldName, const string&in newName) {
        map.renameScriptFile(oldName, newName);
        _updateScriptsWindow();
    }

    /**
     * Removes a script from the map file.
     * @param name The name of the script file.
     */
    void removeScriptFile(const string&in name) {
        map.removeScriptFile(name);
        _updateScriptsWindow();
    }

    /**
     * Builds the scripts and caches the last known result message.
     * The result message won't update with mementos but I don't really care and I
     * think it's intuitive enough.
     * @return \c TRUE if the build was successful, \c FALSE if not.
     */
    bool buildScriptFiles() {
        const bool success = map.buildScriptFiles().isEmpty();
        _updateScriptsWindow();
        return success;
    }

    /// @return The last known build result, an empty string if there hasn't been
    ///         a build yet.
    string getLastKnownBuildResult() const {
        const auto lastResult = map.getLastKnownBuildResult();
        if (lastResult.isEmpty())
            return translate("success");
        else
            return lastResult;
    }

    ////////////////////////
    // DRAWING OPERATIONS //
    ////////////////////////
    /**
     * Increases the map scaling factor by \c 1.0.
     * The map scaling factor does not go above \c 5.0.
     */
    void zoomIn() {
        _mapScalingFactor += 1.0;
        if (_mapScalingFactor > 5.0) _mapScalingFactor = 5.0;
        map.setMapScalingFactor(_mapScalingFactor);
        _updateStatusBar();
    }
    
    /**
     * Decreases the map scaling factor by \c 1.0.
     * The map scaling factor does not go below \c 1.0.
     */
    void zoomOut() {
        _mapScalingFactor -= 1.0;
        if (_mapScalingFactor < 1.0) _mapScalingFactor = 1.0;
        map.setMapScalingFactor(_mapScalingFactor);
        _updateStatusBar();
    }

    /**
     * Reverts the cursor back to the normal sprites.
     */
    void setNormalCursorSprites() {
        map.setULCursorSprite("ulcursor");
        map.setURCursorSprite("urcursor");
        map.setLLCursorSprite("llcursor");
        map.setLRCursorSprite("lrcursor");
    }

    ///////////////////////////////
    // TILE SELECTION OPERATIONS //
    ///////////////////////////////
    /**
     * Moves the cursor up one tile.
     */
    void moveSelectedTileUp() {
        map.moveSelectedTileUp();
        _updateStatusBar();
    }
    
    /**
     * Moves the cursor down one tile.
     */
    void moveSelectedTileDown() {
        map.moveSelectedTileDown();
        _updateStatusBar();
    }
    
    /**
     * Moves the cursor left one tile.
     */
    void moveSelectedTileLeft() {
        map.moveSelectedTileLeft();
        _updateStatusBar();
    }
    
    /**
     * Moves the cursor right one tile.
     */
    void moveSelectedTileRight() {
        map.moveSelectedTileRight();
        _updateStatusBar();
    }
    
    /**
     * Selects a tile based on a given pixel.
     * @param pixel The tile underneath this pixel will be selected.
     */
    void setSelectedTileByPixel(const MousePosition&in pixel) {
        map.setSelectedTileByPixel(pixel);
        _updateStatusBar();
    }

    ////////////////////
    // MAP OPERATIONS //
    ////////////////////
    /**
     * Updates a map's properties.
     * @param mapName The name of the map.
     * @param day     The current day of the map.
     */
    void setMapProperties(const string&in mapName, const Day day) {
        if (mapName == map.getMapName() && day == map.getDay()) return;
        DisableMementos token(map, OPERATION[Operation::MAP_PROPS]);
        map.setMapName(mapName);
        map.setDay(day);
    }

    /**
     * Resizes a map.
     * @param mapSize  The new size of the map.
     * @param tileType The type of tile to create if the map is getting larger.
     * @param army     The owner to assign to new tiles.
     */
    void setMapSize(const Vector2&in mapSize, const string&in tileType,
        const ArmyID army = NO_ARMY) {
        if (mapSize == map.getMapSize()) return;
        DisableMementos token(map, OPERATION[Operation::MAP_SIZE]);
        _createArmyIfNonExistent(army);
        map.setMapSize(mapSize, tileType, army);
        refreshTileProps();
        _updateStatusBar();
    }

    /**
     * Fills a map with a given tile type.
     * @param type  The type of tile to fill with.
     * @param owner The owner to assign to each of the tiles.
     */
    void fillMap(const TileType@ const type, const string&in owner) {
        map.fillMap(type.scriptName,
            owner.isEmpty() ? NO_ARMY : country[owner].turnOrder);
        refreshTileProps();
    }

    /**
     * Changes the tiles within a given rectangle to a given type, and gives them
     * to the specified army.
     * @sa \c Map::rectangleFillTiles().
     */
    void rectangleFillTiles(const Vector2&in start, const Vector2&in end,
        const string&in type, const ArmyID owner = NO_ARMY) {
        DisableMementos token(map, OPERATION[Operation::RECT_FILL_TILES]);
        _createArmyIfNonExistent(owner);
        map.rectangleFillTiles(start, end, type, owner);
        refreshTileProps();
    }
    
    /**
     * Changes the tiles within a given rectangle to a given type of terrain, and
     * gives them to the specified army.
     */
    void rectangleFillTiles(const Vector2&in start, const Vector2&in end,
        const Terrain@ const type, const string&in owner) {
        const auto tiles = map.getTilesInArea(start, end);
        for (uint i = 0, len = tiles.length(); i < len; ++i)
            setTerrain(tiles[i], type, owner);
        refreshTileProps();
    }

    /**
     * Creates fully replenished units on the tiles within a given rectangle.
     * @sa \c Map::rectangleFillUnits().
     */
    void rectangleFillUnits(const Vector2&in start, const Vector2&in end,
        const string&in type, const ArmyID army) {
        map.rectangleFillUnits(start, end, type, army);
        refreshTileProps();
        _updateArmyProps();
    }

    /**
     * Deletes all units on the tiles within a given rectangle.
     * @return The number of units deleted.
     * @sa     \c Map::rectangleDeleteUnits().
     */
    uint64 rectangleDeleteUnits(const Vector2&in start, const Vector2&in end) {
        const auto count = map.rectangleDeleteUnits(start, end);
        refreshTileProps();
        _updateArmyProps();
        return count;
    }

    /**
     * Trawls through every tile on the map and updates their types using
     * \c setTerrain().
     */
    void fixTiles() {
        DisableMementos token(map, OPERATION[Operation::FIX_TILES]);
        for (uint y = 0, h = map.getMapSize().y; y < h; ++y) {
            for (uint x = 0, w = map.getMapSize().x; x < w; ++x) {
                updateTerrain(Vector2(x, y));
            }
        }
    }

    ////////////////////////////////
    // ARMY MANAGEMENT OPERATIONS //
    ////////////////////////////////
    /**
     * Creates an army.
     * @param country The turn-order ID of the country to assign to the new army.
     */
    void createArmy(const ArmyID country) {
        DisableMementos token(map, OPERATION[Operation::CREATE_ARMY_SCRIPT]);
        const auto successful =
            map.createArmy(::country.scriptNames[uint64(country)]);
        // Assign the first CO in the commander bank as this army's current CO.
        if (successful) map.setArmyCurrentCO(country, commander.scriptNames[0]);
        if (successful && map.getArmyCount() == 1) {
            // If the first army has been created, automatically select them.
            map.setSelectedArmy(map.getArmyIDs()[0]);
        } else if (!successful) {
            token.discard();
        }
        _updateArmyProps();
    }

    /**
     * Deletes an army.
     * @param army The ID of the army to delete.
     */
    void deleteArmy(const ArmyID army) {
        DisableMementos token(map, OPERATION[Operation::DELETE_ARMY_SCRIPT]);
        // Delete all HQ structures, but leave other structures intact. They will
        // be disowned automatically later.
        const auto hqTiles = map.getTilesOfArmy(army, { "HQ" });
        for (uint64 i = 0, len = hqTiles.length(); i < len; ++i)
            map.deleteStructure(hqTiles[i]);
        // If we are deleting the current army, select the next one.
        const auto getNextArmy = map.getSelectedArmy() == army;
        const auto nextArmy =
            map.getArmyCount() == 1 ? NO_ARMY : map.getNextArmy();
        map.deleteArmy(army);
        if (getNextArmy) map.setSelectedArmy(nextArmy);
        _updateArmyProps();
        refreshTileProps();
    }

    /**
     * Updates an army's team.
     * @param army    The ID of the army to update.
     * @param newTeam The new team to assign to the army.
     */
    void setArmyTeam(const ArmyID army, const TeamID newTeam) {
        map.setArmyTeam(army, newTeam);
        _updateArmyProps();
    }

    /**
     * Updates an army's funds.
     * @param  army     The ID of the army to update.
     * @param  newFunds The new funds to assign to the army.
     * @return The actual funds assigned to the army.
     */
    Funds setArmyFunds(const ArmyID army, Funds newFunds) {
        if (newFunds < 0) newFunds = 0;
        map.setArmyFunds(army, newFunds);
        _updateArmyProps();
        return newFunds;
    }

    /**
     * Updates an army's current CO.
     * @param army The ID of the army to update.
     * @param co   The script name of the current CO to assign to the army.
     */
    void setArmyCurrentCO(const ArmyID army, const string&in co) {
        map.setArmyCurrentCO(army, co);
        _updateArmyProps();
    }

    /**
     * Updates an army's tag CO.
     * @param army The ID of the army to update.
     * @param co   The script name of the tag CO to assign to the army.
     */
    void setArmyTagCO(const ArmyID army, const string&in co) {
        map.setArmyTagCO(army, co);
        _updateArmyProps();
    }

    /**
     * Update's which army is having their turn.
     * @param army The ID of the army to set as the current one.
     */
     void setSelectedArmy(const ArmyID army) {
        map.setSelectedArmy(army);
        _updateArmyProps();
     }

    ////////////////////////////////
    // TILE MANAGEMENT OPERATIONS //
    ////////////////////////////////
    /**
     * Updates a given tile to a given type and assigns a new owner to it.
     * @param tileToChange The position of the tile to update.
     * @param toType       Handle to the tile type to assign to the tile.
     * @param newOwner     Script name of the new owner. Empty if neutral.
     */
    void setTile(const Vector2&in tileToChange, const TileType@ const toType,
        const string&in newOwner) {
        const auto fromType = map.getTileType(tileToChange);
        const auto oldOwnerID = map.getTileOwner(tileToChange);
        const auto newOwnerID =
            newOwner.isEmpty() ? NO_ARMY : country[newOwner].turnOrder;

        array<Vector2>@ changingTiles;
        if (fromType.scriptName != toType.scriptName) {
            DisableMementos token(map, OPERATION[Operation::TILE_TYPE_AND_OWNER]);
            @changingTiles = map.querySetTileTypeChangedTiles(tileToChange);
            map.setTileType(tileToChange, toType.scriptName);
            _createArmyIfNonExistent(newOwnerID);
            map.setTileOwner(tileToChange, newOwnerID);
            // If this tile is a non-paintable structure, setup its data now.
            const auto structureName =
                map.getTileTypeStructure(toType.scriptName);
            if (!structureName.isEmpty()) {
                map.setTileStructureData(tileToChange, structureName,
                    MousePosition(0, 0), false);
            }
        } else if (oldOwnerID != newOwnerID) {
            DisableMementos token(map, OPERATION[Operation::TILE_TYPE_AND_OWNER]);
            @changingTiles = array<Vector2>();
            changingTiles.insertLast(tileToChange);
            _createArmyIfNonExistent(newOwnerID);
            map.setTileOwner(tileToChange, newOwnerID);
        }
        _updateTileProps(changingTiles);
    }

    /**
     * Determines what tile type to set the given tile to based on its surrounding
     * tiles.
     * Does nothing if the given tile already is the terrain given.
     * @param tileToChange           The position of the tile to update.
     * @param toType                 Handle to the terrain that the tile will
     *                               have.
     * @param newOwner               Script name of the new owner. Empty if
     *                               neutral.
     * @param updateSurroundingTiles If \c TRUE, the surrounding tiles may also
     *                               be changed.
     */
    void setTerrain(Vector2 tileToChange, const Terrain@ const toType,
        const string&in newOwner, const bool updateSurroundingTiles = true) {
        const auto newTileType = awe::DetermineTileType(
            _getTileTypeOfTile(tileToChange.x - 1, tileToChange.y - 1),
            _getTileTypeOfTile(tileToChange.x, tileToChange.y - 1),
            _getTileTypeOfTile(tileToChange.x + 1, tileToChange.y - 1),
            _getTileTypeOfTile(tileToChange.x - 1, tileToChange.y),
            toType,
            _getTileTypeOfTile(tileToChange.x + 1, tileToChange.y),
            _getTileTypeOfTile(tileToChange.x - 1, tileToChange.y + 1),
            _getTileTypeOfTile(tileToChange.x, tileToChange.y + 1),
            _getTileTypeOfTile(tileToChange.x + 1, tileToChange.y + 1)
        );
        if (newTileType.isEmpty()) {
            if (toType.primaryTileType !is null)
                setTile(tileToChange, toType.primaryTileType, newOwner);
        } else setTile(tileToChange, tiletype[newTileType], newOwner);
        if (!updateSurroundingTiles) return;
        // Now go through each of the surrounding tiles and update their types.
        --tileToChange.x;
        --tileToChange.y;
        for (uint y = 0; y < 3; ++y) {
            for (uint x = 0; x < 3; ++x) {
                if (!map.isOutOfBounds(tileToChange)) {
                    const auto tileOwner = map.getTileOwner(tileToChange);
                    setTerrain(tileToChange, map.getTileType(tileToChange).type,
                        tileOwner == NO_ARMY ? "" :
                            country.scriptNames[tileOwner], false);
                }
                ++tileToChange.x;
            }
            tileToChange.x -= 3;
            ++tileToChange.y;
        }
    }

    /**
     * Calls \c setTerrain() on a given tile to update its type, as well as the
     * tiles that surround it.
     * @param tile The tile to update.
     */
    void updateTerrain(const Vector2&in tile) {
        const auto tileOwner = map.getTileOwner(tile);
        setTerrain(tile, map.getTileType(tile).type, tileOwner == NO_ARMY ? "" :
            country.scriptNames[tileOwner], true);
    }

    /**
     * Get the tile type of a tile without logging an error if the operation
     * fails.
     * @param  x The X coordinate of the tile.
     * @param  y The Y coordinate of the tile.
     * @return If the tile is in bounds, a handle to its tile type. \c null if the
     *         tile is out-of-bounds.
     */
    private const TileType@ const _getTileTypeOfTile(const uint x,
        const uint y) const {
        const auto tile = Vector2(x, y);
        if (map.isOutOfBounds(tile)) return null;
        return map.getTileType(tile);
    }

    /**
     * Set the HP of the tile that is being displayed in the properties window.
     * @param newHP The new HP value to set.
     */
    void setSelectedTileHP(HP newHP) {
        const auto max = map.getTileType(tilePropsTile).type.maxHP;
        if (newHP < 0) newHP = 0;
        else if (newHP > HP(max)) newHP = HP(max);
        map.setTileHP(tilePropsTile, newHP);
        refreshTileProps();
    }

    /**
     * Set the owner of the tile that is being displayed in the properties window.
     * @param newOwner The ID of the new owner.
     */
    void setSelectedTileOwner(const ArmyID newOwner) {
        if (newOwner == map.getTileOwner(tilePropsTile)) return;
        DisableMementos token(map, OPERATION[Operation::TILE_OWNER]);
        _createArmyIfNonExistent(newOwner);
        map.setTileOwner(tilePropsTile, newOwner);
        refreshTileProps();
    }

    ////////////////////////////////
    // UNIT MANAGEMENT OPERATIONS //
    ////////////////////////////////
    /**
     * Deletes a given tile's unit, if there is any, and creates a fully
     * replenished unit in its place.
     * If the deleted unit was the last unit of its army, then the army will be
     * deleted. And if the army of the new unit doesn't have a unit yet, the army
     * will be created first.
     * @param unitPosition The position of the new unit.
     * @param unitType     Handle to the unit type to assign to the new unit.
     * @param army         Script name of the new unit's army.
     */
    void createUnit(const Vector2&in unitPosition, const UnitType@ const unitType,
        const string&in army) {
        const auto oldUnit = map.getUnitOnTile(unitPosition);
        const auto oldUnitType =
            oldUnit == NO_UNIT ? null : map.getUnitType(oldUnit);
        const auto oldUnitArmyID =
            oldUnit == NO_UNIT ? NO_ARMY : map.getArmyOfUnit(oldUnit);
        const auto armyID = country[army].turnOrder;

        if (oldUnit == NO_UNIT || oldUnitType.scriptName != unitType.scriptName ||
            oldUnitArmyID != armyID) {
            DisableMementos token(map, OPERATION[Operation::CREATE_UNIT_SCRIPT]);
            // Delete existing unit, and its army if appropriate.
            deleteUnit(oldUnit);

            // Create new unit, after creating its army if it doesn't exist yet.
            if (!map.isArmyPresent(armyID)) createArmy(armyID);
            const auto newUnit = map.createUnit(unitType.scriptName, armyID);
            map.setUnitPosition(newUnit, unitPosition);
            map.replenishUnit(newUnit, true);
            map.waitUnit(newUnit, false);

            _updateTileProps(unitPosition);
        }
    }
    /**
     * Creates a unit, and loads it onto another unit.
     * The created unit will have the same army as the unit it will be loaded
     * onto.
     * @param loadedOnto The ID of the unit to load the new one on to.
     * @param unitType   Handle to the unit type to assign to the new unit.
     */
    void createAndLoadUnit(const UnitID loadedOnto,
        const UnitType@ const unitType) {
        DisableMementos token(map, OPERATION[Operation::CREATE_LOAD_UNIT]);
        const auto army = map.getArmyOfUnit(loadedOnto);
        const auto newUnit = map.createUnit(unitType.scriptName, army);
        map.replenishUnit(newUnit, true);
        map.waitUnit(newUnit, false);
        map.loadUnit(newUnit, loadedOnto);
        const auto parentOfLoadedOnto =
            map.getUnloadedUnitWhichContainsUnit(loadedOnto);
        _updateTileProps(map.getUnitPosition(parentOfLoadedOnto));
    }

    /**
     * Deletes a given unit.
     * If \c NO_UNIT is given, don't do anything.
     * @param unit ID of the unit to delete.
     */
    void deleteUnit(const UnitID unit) {
        if (unit != NO_UNIT) {
            const auto parentUnit = map.getUnloadedUnitWhichContainsUnit(unit);
            const auto parentUnitTile = map.getUnitPosition(parentUnit);
            const auto unitArmyID = map.getArmyOfUnit(unit);
            map.deleteUnit(unit);
            _updateTileProps(parentUnitTile);
        }
    }

    /**
     * Updates a unit's HP.
     * @param  unit  The ID of the unit to update.
     * @param  newHP The new HP to assign to the unit.
     * @return The actual HP assigned to the unit.
     */
    HP setUnitHP(const UnitID unit, HP newHP) {
        if (unit == NO_UNIT) return 0;
        const auto max = map.getUnitType(unit).maxHP;
        if (newHP < 1) newHP = 1;
        else if (newHP > HP(max)) newHP = HP(max);
        map.setUnitHP(unit, newHP);
        refreshTileProps();
        return newHP;
    }

    /**
     * Updates a unit's fuel.
     * @param  unit    The ID of the unit to update.
     * @param  newFuel The new fuel to assign to the unit.
     * @return The actual fuel assigned to the unit.
     */
    Fuel setUnitFuel(const UnitID unit, Fuel newFuel) {
        if (unit == NO_UNIT) return 0;
        const auto max = map.getUnitType(unit).maxFuel;
        if (newFuel < 0) newFuel = 0;
        else if (newFuel > Fuel(max)) newFuel = Fuel(max);
        map.setUnitFuel(unit, newFuel);
        refreshTileProps();
        return newFuel;
    }

    /**
     * Updates a unit's weapon's ammo.
     * @param  unit    The ID of the unit to update.
     * @param  weapon  The index of the weapon to update.
     * @param  newAmmo The new ammo to assign to the unit.
     * @return The actual ammo assigned to the unit.
     */
    Ammo setUnitAmmo(const UnitID unit, const uint64 weapon, Ammo newAmmo) {
        if (unit == NO_UNIT) return 0;
        const auto weaponType = map.getUnitType(unit).weapon(weapon);
        const auto max = weaponType.maxAmmo;
        if (newAmmo < 0) newAmmo = 0;
        else if (newAmmo > Ammo(max)) newAmmo = Ammo(max);
        map.setUnitAmmo(unit, weaponType.scriptName, newAmmo);
        refreshTileProps();
        return newAmmo;
    }

    /**
     * Updates the unit's waiting flag.
     * @param unit The ID of the unit to update.
     * @param wait \c TRUE if the unit should be waiting, \c FALSE if not.
     */
    void waitUnit(const UnitID unit, const bool wait) {
        map.waitUnit(unit, wait);
        refreshTileProps();
    }

    /**
     * Updates the unit's capturing flag.
     * @param unit    The ID of the unit to update.
     * @param capture \c TRUE if the unit should be capturing, \c FALSE if not.
     */
    void unitCapturing(const UnitID unit, const bool capture) {
        map.unitCapturing(unit, capture);
        refreshTileProps();
    }

    /**
     * Updates the unit's hiding flag.
     * @param unit The ID of the unit to update.
     * @param hide \c TRUE if the unit should be hiding, \c FALSE if not.
     */
    void unitHiding(const UnitID unit, const bool hide) {
        map.unitHiding(unit, hide);
        const auto type = map.getUnitType(unit);
        map.queuePlay("sound", hide ? type.hideSound : type.unhideSound);
        refreshTileProps();
    }

    ////////////////////////
    // STRUCTURE HANDLING //
    ////////////////////////
    /**
     * Clears the available tiles and adds new ones based on the tile offsets of a
     * given structure, and the currently selected tile in the map.
     * Not based on the selected tile in this \c EditableMap.\n
     * If any tiles lie outside of the map's size, the red shader will be used.
     * Otherwise, the yellow shader is used.
     * @param structure The structure to pull the tiles from. \c null can be used
     *                  to clear the available tiles only.
     */
    void setStructurePaintedTiles(const Structure@ const structure) {
        map.clearAvailableTiles();
        if (structure is null) return;
        const auto rootTile = map.getSelectedTile();
        map.addAvailableTile(rootTile);
        AvailableTileShader shader = AvailableTileShader::Yellow;
        const auto mapSize = map.getMapSize();
        for (uint64 i = 0, len = structure.dependentTileCount; i < len; ++i) {
            // Integer overflow will handle tiles going above the map or to the
            // left of the map. A map should never be over 4 billion tiles in
            // either axis anyway.
            const auto tile = rootTile + structure.dependentTileOffset[i];
            if (tile.x >= mapSize.x || tile.y >= mapSize.y)
                shader = AvailableTileShader::Red;
            else
                map.addAvailableTile(tile);
        }
        map.setAvailableTileShader(shader);
    }

    /**
     * Paints a structure from a given tile.
     * If the structure can't fit when painted from the given tile, no changes
     * will be made.
     * @param fromTile      The location of the root tile of the structure.
     * @param structureType The structure to build.
     * @param owner         The script name of the army that's owning the
     *                      structure if it's not destroyed.
     * @param destroyed     \c TRUE if this structure is to be destroyed, \c FALSE
     *                      if this structure is to be normal/functional.
     */
    void paintStructure(const Vector2&in fromTile,
        const Structure@ const structureType, const string&in owner,
        const bool destroyed) {
        if (!map.canStructureFit(fromTile, structureType.scriptName)) return;
        DisableMementos token(map, OPERATION[Operation::PAINT_STRUCTURE]);
        // Set each tile's type. Delete all units occupying structure tiles unless
        // the structure says otherwise.
        const auto name = structureType.scriptName;
        const bool keepUnits = structureType.keepUnitsWhenPainted;
        array<Vector2>@ changingTiles =
            map.querySetTileTypeChangedTiles(fromTile);
        map.setTileType(fromTile, destroyed ?
            structureType.rootDestroyedTileType : structureType.rootTileType);
        if (!keepUnits) deleteUnit(map.getUnitOnTile(fromTile));
        map.setTileStructureData(fromTile, name, MousePosition(0, 0), destroyed);
        for (uint64 i = 0, len = structureType.dependentTileCount; i < len; ++i) {
            const auto offset = structureType.dependentTileOffset[i];
            const Vector2 tile = fromTile + offset;
            auto depChangingTiles = map.querySetTileTypeChangedTiles(tile);
            for (uint64 j = 0, lenj = depChangingTiles.length(); j < lenj; ++j)
                changingTiles.insertLast(depChangingTiles[j]);
            map.setTileType(tile, destroyed ?
                structureType.dependentDestroyedTileType[i] :
                structureType.dependentTileType[i]);
            if (!keepUnits) deleteUnit(map.getUnitOnTile(tile));
            map.setTileStructureData(tile, name, offset, destroyed);
        }
        // Set the owner of the root tile if not destroyed.
        if (!destroyed) {
            const auto ownerID = owner.isEmpty() ? NO_ARMY :
                country[owner].turnOrder;
            _createArmyIfNonExistent(ownerID);
            map.setTileOwner(fromTile, ownerID);
        }
        // Structure tiles could influence the types of tiles surrounding them, so
        // feed them all to setTerrain().
        for (uint i = 0, len = changingTiles.length(); i < len; ++i)
            updateTerrain(changingTiles[i]);
        _updateTileProps(changingTiles);
    }

    ///////////////////////////////////////////////
    // TILE AND ARMY PROPERTIES WINDOWS HANDLING //
    ///////////////////////////////////////////////
    /**
     * Display tile properties on the given tile.
     * @param tile The tile to display information on.
     */
    void selectTile(const Vector2&in tile) {
        tilePropsTile = tile;
        tilePropsTileSet = true;
        map.setAdditionallySelectedTile(tile);
        _updateTileProps(tile);
    }

    /**
     * Clear tile properties from the \c TilePropertiesWindow.
     */
    void deselectTile() {
        if (!tilePropsTileSet) return;
        tilePropsTileSet = false;
        map.clearAdditionallySelectedTile();
        _refresh(Subject::Tiles);
    }

    /**
     * Allows external code to refresh the Tile Properties window without changing
     * the selected tile.
     */
    void refreshTileProps() {
        _updateTileProps(tilePropsTile);
    }

    /**
     * Passes one tile to <tt>_updateTileProps(const array<Vector2>@ const)</tt>.
     */
    private void _updateTileProps(const Vector2&in tileThatIsChanging) {
        _updateTileProps({ tileThatIsChanging });
    }

    /**
     * Updates the linked \c TilePropertiesWindow to ensure it is always
     * displaying the correct information.
     * Ensures to disable future calls to this method if \c tilePropsTile becomes
     * out-of-bounds, until a new tile is selected via \c selectTile().
     * @param tilesThatAreChanging The tiles that are changing.
     */
    private void _updateTileProps(
        const array<Vector2>@ const tilesThatAreChanging) {
        if (!tilePropsTileSet) return;
        if (tilesThatAreChanging !is null &&
            tilesThatAreChanging.find(tilePropsTile) >= 0)
                _refresh(Subject::Tiles, any(tilePropsTile));
        if (map.isOutOfBounds(tilePropsTile)) deselectTile();
    }

    /**
     * Notifies the armies observer of a change.
     */
    private void _updateArmyProps() {
        _refresh(Subject::Armies);
    }

    /**
     * Notifies the map properties observer of a change.
     */
    private void _updateMapProps() {
        _refresh(Subject::Properties);
    }

    /**
     * Notifies the status observer of a change.
     */
    private void _updateStatusBar() {
        _refresh(Subject::Status, any(_mapScalingFactor));
    }

    /**
     * Notifies the scripts observer of a change.
     */
    private void _updateScriptsWindow() {
        _refresh(Subject::Scripts);
    }

    /**
     * Notify an observer of a change.
     * @param s    The observer to notify.
     * @param data The data to given to the observer.
     */
    private void _refresh(const Subject s, const any&in data = any()) {
        if (observers[s] !is null) observers[s].refresh(data);
    }

    /**
     * Create the given army if it doesn't exist already, and update the army
     * properties window.
     * @param army The ID of the army to add. If \c NO_ARMY, nothing will change.
     */
    private void _createArmyIfNonExistent(const ArmyID army) {
        if (army != NO_ARMY && !map.isArmyPresent(army)) createArmy(army);
    }

    /////////
    // MAP //
    /////////
    /**
     * The map.
     * @warning Although read-write access to the map is given here, it is assumed
     *          that the state of the map will be changed via \c EditableMap's
     *          operations wherever possible. Also, do not update the handle
     *          itself. I would have made it constant if I could.
     */
    Map@ map;

    //////////
    // DATA //
    //////////
    /**
     * The observers that monitor map changes.
     */
    private array<Observer@> observers;

    /**
     * The currently selected tile used to fill the \c TilePropertiesWindow.
     */
    private Vector2 tilePropsTile;

    /**
     * Does \c tilePropsTile contains a valid value?
     */
    private bool tilePropsTileSet = false;

    /**
     * The map scaling factor.
     */
    private float _mapScalingFactor = 2.0f;
}
