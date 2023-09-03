/**
 * @file EditableMap.as
 * Defines most of the map maker logic.
 */

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
    
    // External.
    PAINT_TILE_TOOL,
    PAINT_UNIT_TOOL,
    DELETE_UNIT_TOOL,
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

    // External.
    "OPERATION_painttiletool",
    "OPERATION_paintunittool",
    "OPERATION_deleteunittool",
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
     * @param tPropsWindow The \c TilePropertiesWindow to link up with this
     *                     \c EditableMap.
     * @param aPropsWindow The \c ArmyPropertiesWindow to link up with this
     *                     \c EditableMap.
     * @param mPropsWindow The \c MapPropertiesWindow to link up with this
     *                     \c EditableMap.
     * @param sBar         The \c MainStatusBar to link up with this
     *                     \c EditableMap.
     * @param sWindow      The \c ScriptsWindow to link up with this
     *                     \c EditableMap.
     */
    EditableMap(Map@ mapToEdit, TilePropertiesWindow@ tPropsWindow,
        ArmyPropertiesWindow@ aPropsWindow, MapPropertiesWindow@ mPropsWindow,
        MainStatusBar@ sBar, ScriptsWindow@ sWindow) {
        if (mapToEdit is null) {
            error("An invalid Map handle was given to the constructor of "
                "EditableMap; the game will crash soon!");
        } else if (tPropsWindow is null) {
            error("An invalid TilePropertiesWindow handle was given to the "
                "constructor of EditableMap; the game will crash soon!");
        } else if (aPropsWindow is null) {
            error("An invalid ArmyPropertiesWindow handle was given to the "
                "constructor of EditableMap; the game will crash soon!");
        } else if (mPropsWindow is null) {
            error("An invalid MapPropertiesWindow handle was given to the "
                "constructor of EditableMap; the game will crash soon!");
        } else if (sBar is null) {
            error("An invalid MainStatusBar handle was given to the constructor "
                "of EditableMap; the game will crash soon!");
        } else if (sWindow is null) {
            error("An invalid ScriptsWindow handle was given to the constructor "
                "of EditableMap; the game will crash soon!");
        } else {
            @map = mapToEdit;
            @tilePropsWindow = tPropsWindow;
            @armyPropsWindow = aPropsWindow;
            @mapPropsWindow = mPropsWindow;
            @statusBar = sBar;
            @scriptsWindow = sWindow;
            map.enablePeriodic(false);
            map.alwaysShowHiddenUnits(true);
            map.setMapScalingFactor(_mapScalingFactor);
            setNormalCursorSprites();
            _updateStatusBar();
        }
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
     * refreshed, and a memento will not be created.
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
        map.setMapSize(mapSize, tileType, army);
        refreshTileProps();
        _updateStatusBar();
    }

    /**
     * Changes the tiles within a given rectangle to a given type, and gives them
     * to the specified army.
     * @sa \c Map::rectangleFillTiles().
     */
    void rectangleFillTiles(const Vector2&in start, const Vector2&in end,
        const string&in type, const ArmyID owner = NO_ARMY) {
        map.rectangleFillTiles(start, end, type, owner);
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
        // Find all HQs belonging to the army and convert them to neutral cities.
        map.convertTiles(map.getTilesOfArmy(army), "06Ahq", "064city", NO_ARMY);
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
     * Updates an army's funds.
     * @param  army     The ID of the army to update.
     * @param  newFunds The new funds to assign to the army.
     * @return The actual funds assigned to the army.
     */
    Funds setArmyFunds(const ArmyID army, Funds newFunds) {
        if (newFunds < 0) newFunds = 0;
        map.setArmyFunds(army, newFunds);
        return newFunds;
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

        if (fromType.scriptName != toType.scriptName) {
            DisableMementos token(map, OPERATION[Operation::TILE_TYPE_AND_OWNER]);
            map.setTileType(tileToChange, toType.scriptName);
            map.setTileOwner(tileToChange, newOwnerID);
        } else if (oldOwnerID != newOwnerID) {
            DisableMementos token(map, OPERATION[Operation::TILE_TYPE_AND_OWNER]);
            map.setTileOwner(tileToChange, newOwnerID);
        }
        
        _updateTileProps(tileToChange);
    }

    /**
     * Set the HP of the tile that is being displayed in the properties window.
     * @param  newHP The new HP value to set.
     * @return The actual HP assigned.
     */
    HP setSelectedTileHP(HP newHP) {
        const auto max = map.getTileType(tilePropsTile).type.maxHP;
        if (newHP < 0) newHP = 0;
        else if (newHP > HP(max)) newHP = HP(max);
        map.setTileHP(tilePropsTile, newHP);
        return newHP;
    }

    /**
     * Set the owner of the tile that is being displayed in the properties window.
     * @param newOwner The ID of the new owner.
     */
    void setSelectedTileOwner(const ArmyID newOwner) {
        map.setTileOwner(tilePropsTile, newOwner);
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
        const auto oldUnitType = oldUnit == 0 ? null : map.getUnitType(oldUnit);
        const auto oldUnitArmyID =
            oldUnit == 0 ? NO_ARMY : map.getArmyOfUnit(oldUnit);
        const auto armyID = country[army].turnOrder;

        if (oldUnit == 0 || oldUnitType.scriptName != unitType.scriptName ||
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
     * If \c 0 is given, don't do anything.
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
        return newAmmo;
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
        map.clearAvailableTiles();
        map.addAvailableTile(tile);
        _updateTileProps(tile);
    }

    /**
     * Clear tile properties from the \c TilePropertiesWindow.
     */
    void deselectTile() {
        if (!tilePropsTileSet) return;
        tilePropsTileSet = false;
        map.clearAvailableTiles();
        if (tilePropsWindow !is null) tilePropsWindow.deselect();
    }

    /**
     * Allows external code to refresh the Tile Properties window without changing
     * the selected tile.
     */
    void refreshTileProps() {
        _updateTileProps(tilePropsTile);
    }

    /**
     * Updates the linked \c TilePropertiesWindow to ensure it is always
     * displaying the correct information.
     * Ensures to disable future calls to this method if \c tilePropsTile becomes
     * out-of-bounds, until a new tile is selected via \c selectTile().
     * @param tileThatIsChanging The tile that is changing.
     */
    private void _updateTileProps(const Vector2&in tileThatIsChanging) {
        if (!tilePropsTileSet) return;
        if (tileThatIsChanging == tilePropsTile)
            tilePropsWindow.refresh(tilePropsTile);
        if (map.isOutOfBounds(tilePropsTile)) deselectTile();
    }

    /**
     * Updates the linked \c ArmyPropertiesWindow to ensure it is always
     * displaying the correct information.
     */
    private void _updateArmyProps() {
        armyPropsWindow.refresh();
    }

    /**
     * Updates the linked \c MapPropertiesWindow to ensure it is always
     * displaying the correct information.
     */
    private void _updateMapProps() {
        mapPropsWindow.refresh();
    }

    /**
     * Update the linked \c MainStatusBar to ensure it is always displaying the
     * correct information.
     * Note that the undo and redo action labels must be handled separately as
     * the C++ itself game engine may be respnsible for changing the state of the
     * mementos.
     */
    private void _updateStatusBar() {
        statusBar.setTileXY(map.getSelectedTile());
        statusBar.setZoom(_mapScalingFactor);
    }

    /**
     * Updates the linked \c ScriptsWindow to ensure it is always displaying the
     * correct information.
     */
    private void _updateScriptsWindow() {
        scriptsWindow.refresh();
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
     * The \c TilePropertiesWindow to link up with this \c EditableMap.
     */
    private TilePropertiesWindow@ tilePropsWindow;
    
    /**
     * The \c ArmyPropertiesWindow to link up with this \c EditableMap.
     */
    private ArmyPropertiesWindow@ armyPropsWindow;

    /**
     * The \c MapPropertiesWindow to link up with this \c EditableMap.
     */
    private MapPropertiesWindow@ mapPropsWindow;

    /**
     * The \c MainStatusBar to link up with this \c EditableMap.
     */
    private MainStatusBar@ statusBar;

    /**
     * The \c ScriptsWindow to link up with this \c EditableMap.
     */
    private ScriptsWindow@ scriptsWindow;

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
