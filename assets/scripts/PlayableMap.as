/**
 * @file PlayableMap.as
 * Defines most of the game logic.
 */

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
            map.disableMementos();
            map.setMapScalingFactor(_mapScalingFactor);
            setNormalCursorSprites();
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
     * The map scaling factor does not go above \c 5.0.
     */
    void zoomIn() {
        _mapScalingFactor += 1.0;
        if (_mapScalingFactor > 5.0) _mapScalingFactor = 5.0;
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
     * Reverts the cursor back to the normal sprites.
     */
    void setNormalCursorSprites() {
        map.setULCursorSprite("ulcursor");
        map.setURCursorSprite("urcursor");
        map.setLLCursorSprite("llcursor");
        map.setLRCursorSprite("lrcursor");
    }

    /**
     * Sets the cursor to have the attack cursor sprites.
     */
    void setAttackCursorSprites() {
        map.setULCursorSprite("ulattackcursor");
        map.setURCursorSprite("urattackcursor");
        map.setLLCursorSprite("llattackcursor");
        map.setLRCursorSprite("lrattackcursor");
    }

    /**
     * Sets the cursor to have the delete cursor sprites.
     */
    void setDeleteCursorSprites() {
        map.setULCursorSprite("deletecursor");
        map.setURCursorSprite("deletecursor");
        map.setLLCursorSprite("deletecursor");
        map.setLRCursorSprite("deletecursor");
    }

    /**
     * Updates a given damage label widget's position based on this map's current
     * cursor position.
     * @param widget The damage label widget to update.
     */
    void updateDamageWidgetPosition(DamageWidget@ widget) const {
        const auto rect = map.getCursorBoundingBox();
        const auto quadrant = map.getCursorQuadrant();
        switch (quadrant) {
        case Quadrant::LowerLeft:
            widget.updatePosition(MousePosition(rect.left + rect.width,
                rect.top), quadrant);
            break;
        case Quadrant::LowerRight:
            widget.updatePosition(MousePosition(rect.left, rect.top), quadrant);
            break;
        case Quadrant::UpperRight:
            widget.updatePosition(MousePosition(rect.left,
                rect.top + rect.height), quadrant);
            break;
        default: // UpperLeft:
            widget.updatePosition(MousePosition(rect.left + rect.width,
                rect.top + rect.height), quadrant);
        }
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
        if (_closedListEnabled) _updateMoveModeClosedList();
    }
    
    /**
     * Moves the cursor down one tile.
     * Also updates the selected unit's closed list.
     */
    void moveSelectedTileDown() {
        map.moveSelectedTileDown();
        if (_closedListEnabled) _updateMoveModeClosedList();
    }
    
    /**
     * Moves the cursor left one tile.
     * Also updates the selected unit's closed list.
     */
    void moveSelectedTileLeft() {
        map.moveSelectedTileLeft();
        if (_closedListEnabled) _updateMoveModeClosedList();
    }
    
    /**
     * Moves the cursor right one tile.
     * Also updates the selected unit's closed list.
     */
    void moveSelectedTileRight() {
        map.moveSelectedTileRight();
        if (_closedListEnabled) _updateMoveModeClosedList();
    }
    
    /**
     * Selects a tile based on a given pixel.
     * Also updates the selected unit's closed list.
     * @param pixel The tile underneath this pixel will be selected.
     */
    void setSelectedTileByPixel(const MousePosition&in pixel) {
        map.setSelectedTileByPixel(pixel);
        if (_closedListEnabled) _updateMoveModeClosedList();
    }

    /**
     * Disable closed list updates when selecting tiles.
     * @param flag \c TRUE if the closed list should be updated when moving the
     *             selection using \c moveSelectedTileXYZ() or
     *             \c setSelectedTileByPixel(), \c FALSE if not. Defaults to
     *             \c TRUE
     */
    void enableClosedList(const bool flag) {
        _closedListEnabled = flag;
    }

    ////////////////////////////////
    // TURN MANAGEMENT OPERATIONS //
    ////////////////////////////////
    /**
     * Ends the current turn.
     * @param deleteOldArmy If \c TRUE, this will delete the army who's ending
     *                      their turn, after the next army has been selected.
     *                      Their properties will not be transfered to any other
     *                      army.
     */
    void endTurn(const bool deleteOldArmy = false) {
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

        // 3. If configured to, delete the army ending their turn.
        if (deleteOldArmy) {
            map.deleteArmy(currentArmy);
        }
        currentArmy = nextArmy;

        // 4. Go through each of the current army's units and perform start of
        //    turn operations. Order each unit by their priority. Only do this if
        //    it's not the first day.
        if (map.getDay() > 1) {
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
        }

        // 5. Then, go through each of the current army's owned tiles.
        const auto armyTiles = map.getTilesOfArmy(currentArmy);
        for (uint i = 0, tileCount = armyTiles.length(); i < tileCount; ++i) {
            const Vector2 tile = armyTiles[i];
            _beginTurnForTile(tile, map.getTileType(tile).type, currentArmy);
        }

        // 6. Finally, perform extra operations on the army's units and tiles that
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

    /**
     * Delete an army.
     * This method has additional checking which ends the turn if the army was
     * having their turn at the time.
     * @param armyID            The ID of the army being deleted.
     * @param transferOwnership The ID of the army who will assume ownership of
     *                          all the deleted army's tiles. By default,
     *                          ownership is set to \c NO_ARMY, i.e. back to
     *                          neutral.
     */
    void deleteArmy(const ArmyID armyID, const ArmyID transferOwnership = NO_ARMY)
        {
        // Find all HQs belonging to the army and convert them to cities. Transfer
        // ownership of them, too.
        map.convertTiles(map.getTilesOfArmy(armyID), "06Ahq", "064city",
            transferOwnership);
        if (map.getSelectedArmy() == armyID) endTurn(true);
        else map.deleteArmy(armyID, transferOwnership);
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
    bool buyUnit(const UnitType@ type, const ArmyID army,
        const Vector2&in position) {
        if (map.getUnitOnTile(position) != NO_UNIT) {
            throw("Could not buy unit of type \"" + type.scriptName + "\" for "
                "army " + formatUInt(army) + " on tile " + position.toString() +
                ": this tile is occupied!");
        }
        Funds newFunds = map.getArmyFunds(army) - type.cost;
        if (newFunds < 0) return false;
        const auto unitID = map.createUnit(type.scriptName, army);
        if (unitID == NO_UNIT) {
            throw("Could not buy unit of type \"" + type.scriptName + "\" for "
                "army " + formatUInt(army) + " on tile " + position.toString() +
                ": this army ID is invalid!");
        }
        map.setUnitPosition(unitID, position);
        map.setArmyFunds(army, newFunds);
        map.replenishUnit(unitID, true);
        return true;
    }

    /**
     * Deletes a unit, and deletes its army if it has no units left.
     * @param  unitID ID of the unit to delete.
     * @return \c TRUE if the unit's army was deleted, \c FALSE otherwise.
     */
    bool deleteUnit(const UnitID unitID) {
        const auto unitsArmy = map.getArmyOfUnit(unitID);
        map.deleteUnit(unitID);
        const auto unitCount = map.getUnitsOfArmy(unitsArmy).length();
        if (unitCount == 0) {
            deleteArmy(unitsArmy);
            return true;
        }
        return false;
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
                    --hp;
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
     * Selects a unit for movement mode.
     * @param unit The ID of the unit to put into move mode. \c NO_UNIT should be
     *             given if the currently selected unit is to be deselected.
     */
    void selectUnit(const UnitID unit) {
        map.setSelectedUnit(unit);
        if (unit != NO_UNIT) {
            map.setAvailableTileShader(AvailableTileShader::Yellow);
            newClosedListNode(map.closedList, -1, map.getUnitPosition(unit), 0);
            map.disableSelectedUnitRenderingEffects(false);

            // Filter the available tiles down based on the unit's movement type,
            // movement points, and fuel.
            const auto tile = map.getUnitPosition(unit);
            const auto unitType = map.getUnitType(unit);
            const auto allTiles = map.getAvailableTiles(tile, 1,
                unitType.movementPoints);
            
            for (uint i = 0, length = allTiles.length(); i < length; ++i) {
                if (map.findPath(tile, allTiles[i], unitType.movementType,
                    unitType.movementPoints, map.getUnitFuel(unit),
                    map.getTeamOfUnit(unit), map.getArmyOfUnit(unit),
                    unitType.hasInfiniteFuel,
                    unitType.scriptName == "OOZIUM").length() > 0) {
                    map.addAvailableTile(allTiles[i]);
                }
            }
            map.addAvailableTile(tile);
        }
    }

    /**
     * Selects a unit so that its attack range can be calculated and shown.
     * In the future, this method should list the attack range of every weapon the
     * unit possesses, not just the first eligible one found.
     * @warning The client must ensure the closed list is disabled whilst the unit
     *          is still selected by this method!
     * @param   unitID The ID of the unit to show the attack range of. \c NO_UNIT
     *                 to show no attack range.
     */
    void showAttackRangeOfUnit(const UnitID unitID) {
        map.setSelectedUnit(unitID);
        if (unitID != NO_UNIT) {
            map.setAvailableTileShader(AvailableTileShader::Red);
            map.disableSelectedUnitRenderingEffects(false);

            // Filter the available tiles down based on the unit's movement type,
            // movement points, fuel, and first available weapon (the first weapon
            // with enough ammo, or the first weapon with infinite ammo). If there
            // is not an available weapon, then there will be no available tiles.
            map.clearAvailableTiles();
            const Weapon@ weaponType = null;
            const auto unitType = map.getUnitType(unitID);
            for (uint i = 0, len = unitType.weaponCount; i < len; ++i) {
                const auto weaponTypeTemp = unitType.weapon(i);
                if (weaponTypeTemp.hasInfiniteAmmo) {
                    @weaponType = weaponTypeTemp;
                    break;
                }
                if (map.getUnitAmmo(unitID, weaponTypeTemp.scriptName) > 0) {
                    @weaponType = weaponTypeTemp;
                    break;
                }
            }
            if (weaponType is null) return;

            const auto tile = map.getUnitPosition(unitID);
            array<Vector2> allTiles;
            // If the weapon can attack after moving, then first find all
            // available tiles separately. Then, feed each tile into an algorithm
            // that determines how many tiles the given weapon can attack from
            // that tile.
            // If the given weapon can't attack after moving, then we can simply
            // assign each tile as available that is within the weapon's range.
            if (weaponType.canAttackAfterMoving) {
                const auto moveableTiles = map.getAvailableTiles(tile, 1,
                    unitType.movementPoints);
                moveableTiles.insertLast(tile);
                const auto unitsArmy = map.getArmyOfUnit(unitID);
                array<Vector2> availableTiles;
                for (uint i = 0, len = moveableTiles.length(); i < len; ++i) {
                    // If there is a path to the available tile, AND it isn't
                    // occupied in any way EXCEPT if the unit occupying the tile
                    // is the unit itself, OR the occupying unit is not visible to
                    // the selected army, then it is considered in range.
                    const auto path = map.findPath(tile, moveableTiles[i],
                        unitType.movementType, unitType.movementPoints,
                        map.getUnitFuel(unitID), map.getTeamOfUnit(unitID),
                        map.getSelectedArmy(), unitType.hasInfiniteFuel,
                        unitType.scriptName == "OOZIUM");
                    // Army ID checks refer only to visible unit checks, so make
                    // sure to give in the CURRENT army to findPath(), and not the
                    // army who owns the unit.
                    if (path.length() > 0 &&
                        (map.getUnitOnTile(moveableTiles[i]) == NO_UNIT ||
                        map.getUnitOnTile(moveableTiles[i]) == unitID ||
                        !map.isUnitVisible(map.getUnitOnTile(moveableTiles[i]),
                            map.getSelectedArmy()))) {
                        availableTiles.insertLast(moveableTiles[i]);
                    }
                }
                // Now go through each available tile and see if the weapon can
                // attack tiles that go out from it.
                for (uint i = 0, len = availableTiles.length(); i < len; ++i) {
                    const auto availableTilesUnit =
                        map.getUnitOnTile(availableTiles[i]);
                    bool availableTileIsOccupied = availableTilesUnit != NO_UNIT;
                    // If the unit occupying the tile is the unit being queried,
                    // then consider it unoccupied.
                    if (availableTilesUnit == unitID) {
                        availableTileIsOccupied = false;
                    }
                    // If the unit is not visible to the selected army, then act
                    // as if it is not occupied.
                    if (availableTileIsOccupied &&
                        !map.isUnitVisible(availableTilesUnit,
                            map.getSelectedArmy())) {
                        availableTileIsOccupied = false;
                    }
                    // We can add the tiles in range, but only if the unit could
                    // theoretically move to the available tile first.
                    if (!availableTileIsOccupied) {
                        const auto tilesInRange = map.getAvailableTiles(
                            availableTiles[i], weaponType.range.x,
                            weaponType.range.y);
                        for (uint j = 0, len2 = tilesInRange.length(); j < len2;
                            ++j) {
                            allTiles.insertLast(tilesInRange[j]);
                        }
                    }
                }
            } else {
                allTiles = map.getAvailableTiles(tile, weaponType.range.x,
                    weaponType.range.y);
            }

            // Add each available tile. Don't add the available tile if the given
            // unit is occupying it.
            for (uint i = 0, length = allTiles.length(); i < length; ++i) {
                if (allTiles[i] != tile) map.addAvailableTile(allTiles[i]);
            }
        }
    }

    /**
     * Moves the currently selected unit along the chosen path and deselects the
     * unit.
     * @throws If a unit hasn't been selected.
     */
    void moveUnit() {
        const auto id = map.getSelectedUnit();
        if (id != NO_UNIT) {
            const auto node = map.closedList[map.closedList.length() - 1];
            map.burnUnitFuel(id, node.g);
            map.setUnitPosition(id, node.tile);
            map.waitUnit(id, true);
            selectUnit(NO_UNIT);
            map.removePreviewUnit(id);
        } else {
            throw("Cannot move a unit as no unit is currently selected!");
        }
    }

    /**
     * Moves the currently selected unit along the chosen path, deselects the
     * unit, and ensures it's either hidden or shown.
     * @param  hide \c TRUE to hide the unit, \c FALSE to show it.
     * @throws If a unit hasn't been selected. Note that no unit will be hidden or
     *         shown if this is the case.
     */
    void moveUnitHide(const bool hide) {
        const auto id = map.getSelectedUnit();
        moveUnit();
        map.unitHiding(id, hide);
    }

    /**
     * Checks if two units can join.
     * @param  stationary The ID of the stationary unit.
     * @param  moving     The ID of the unit that is being moved onto the
     *                    stationary one.
     * @return \c TRUE if the moving unit can join with the stationary one,
     *         \c FALSE otherwise.
     */
    bool canJoin(const UnitID stationary, const UnitID moving) const {
        /* Conditions:
        1. Both units must be of the same type.
        2. Both units must belong to the same army.
        3. Both units cannot have any units loaded onto them.
        4. The user-friendly HP of the stationary unit must be below its maximum
           user-friendly HP.
        5. The IDs of both the stationary unit and the moving unit cannot be
           NO_UNIT.
        6. A unit cannot join with itself.
        */
        return stationary != NO_UNIT && moving != NO_UNIT &&
            stationary != moving &&
            map.getUnitType(stationary).scriptName ==
                map.getUnitType(moving).scriptName &&
            map.getArmyOfUnit(stationary) == map.getArmyOfUnit(moving) &&
            map.getLoadedUnits(stationary).length() == 0 &&
            map.getLoadedUnits(moving).length() == 0 &&
            map.getUnitDisplayedHP(stationary) <
                GetDisplayedHP(map.getUnitType(stationary).maxHP);
    }

    /**
     * Joins two units, if they can be joined.
     * If the two units could not be joined, a detailed error will be logged.
     * @param stationary The ID of the stationary unit.
     * @param moving     The ID of the unit that is being moved onto the
     *                   stationary one.
     * @param burn       The units of fuel to burn from the moving unit before
     *                   performing the join.
     */
    void joinUnits(const UnitID stationary, const UnitID moving, const Fuel burn)
        {
        if (canJoin(stationary, moving)) {
            // First, burn the moving unit's fuel.
            map.burnUnitFuel(moving, burn);
            // Perform join.
            HP newHP = GetInternalHP(GetDisplayedHP(map.getUnitHP(stationary)) +
                GetDisplayedHP(map.getUnitHP(moving)));
            Fuel newFuel = map.getUnitFuel(stationary) + map.getUnitFuel(moving);
            const auto type = map.getUnitType(stationary);
            dictionary newAmmos;
            const auto weaponCount = type.weaponCount;
            for (uint64 w = 0; w < weaponCount; ++w) {
                const auto weaponType = type.weapon(w);
                newAmmos[weaponType.scriptName] =
                    map.getUnitAmmo(stationary, weaponType.scriptName) +
                    map.getUnitAmmo(moving, weaponType.scriptName);
                if (!weaponType.hasInfiniteAmmo &&
                    int(newAmmos[weaponType.scriptName]) > weaponType.maxAmmo) {
                    newAmmos[weaponType.scriptName] = weaponType.maxAmmo;
                }
            }
            if (GetDisplayedHP(newHP) > GetDisplayedHP(type.maxHP)) {
                map.offsetArmyFunds(map.getArmyOfUnit(stationary),
                    type.cost / GetDisplayedHP(type.maxHP) *
                    (GetDisplayedHP(newHP) - GetDisplayedHP(type.maxHP)));
                newHP = type.maxHP;
            }
            if (!type.hasInfiniteFuel && newFuel > type.maxFuel) {
                newFuel = type.maxFuel;
            }
            map.setUnitHP(stationary, newHP);
            if (!type.hasInfiniteFuel) map.setUnitFuel(stationary, newFuel);
            const auto@ weaponNames = newAmmos.getKeys();
            for (uint64 w = 0, len = weaponNames.length(); w < len; ++w) {
                const auto weaponName = weaponNames[w];
                if (!weapon[weaponName].hasInfiniteAmmo) {
                    map.setUnitAmmo(stationary, weaponName,
                        int(newAmmos[weaponName]));
                }
            }
            map.waitUnit(stationary, true);
            // For "efficiency," don't perform extra PlayableMap checking that
            // comes with deleteUnit() and just delete it directly.
            map.deleteUnit(moving);
        } else {
            error("Attempted to join moving unit " + formatUInt(moving) +
                " with stationary unit " + formatUInt(stationary) + ". Details:\n"
                "~~~Stationary unit~~~\nType: " +
                map.getUnitType(stationary).scriptName + "\nOwner: " +
                formatUInt(map.getArmyOfUnit(stationary)) + "\n# of Loaded "
                "Units: " + formatUInt(map.getLoadedUnits(stationary).length()) +
                "\nUser-friendly HP: " +
                formatInt(map.getUnitDisplayedHP(stationary)) + "\n"
                "~~~Moving unit~~~\nType: " +
                map.getUnitType(moving).scriptName + "\nOwner: " +
                formatUInt(map.getArmyOfUnit(moving)) + "\n# of Loaded "
                "Units: " + formatUInt(map.getLoadedUnits(moving).length()) +
                "\nUser-friendly HP: " +
                formatInt(map.getUnitDisplayedHP(moving)));
            
        }
    }
    
    /**
     * Checks if a unit can capture the given tile.
     * @param  unit The ID of the capturing unit.
     * @param  tile The tile which the unit is capturing.
     * @return \c TRUE if the unit can capture the tile, \c FALSE otherwise.
     */
    bool canCapture(const UnitID unit, const Vector2 tile) const {
        /* Conditions:
        1. The unit must be able to capture the terrain type that the tile is.
        2. The tile must be either owned by no one, or owned by an army on a
           team that is against the unit.
        3. The tile must be vacant, unless the given unit is already occupying it.
        */
        return unit != NO_UNIT && map.getUnitType(unit).canCapture[
                map.getTileType(tile).type.scriptName] &&
            (map.getTileOwner(tile) == NO_ARMY ||
            map.getTeamOfUnit(unit) != map.getArmyTeam(map.getTileOwner(tile))) &&
            (map.getUnitOnTile(tile) == NO_UNIT ||
                map.getUnitOnTile(tile) == unit);
    }

    /**
     * Performs \c moveUnit() on the currently selected unit, then captures the
     * destination tile using the selected unit.
     * Note that the game engine handles all cases where a capture is completely
     * interrupted, and in such cases, the tile's HP will be set back to maximum
     * and the unit will no longer be in capturing mode.
     * @throws If a unit hasn't been selected, or if the unit cannot capture the
     *         destination tile.
     */
    void moveUnitAndCapture() {
        const auto unit = map.getSelectedUnit();
        const auto tile = map.closedList[map.closedList.length() - 1].tile;
        if (canCapture(unit, tile)) {
            moveUnit();
            const auto newHP = map.getTileHP(tile) - map.getUnitDisplayedHP(unit);
            if (newHP <= 0) {
                if (map.getTileType(tile).type.scriptName == "HQ") {
                    deleteArmy(map.getTileOwner(tile), map.getArmyOfUnit(unit));
                } else {
                    map.setTileOwner(tile, map.getArmyOfUnit(unit));
                }
            } else {
                map.unitCapturing(unit, true);
                map.setTileHP(tile, newHP);
            }
        } else {
            throw("Unit with ID " + formatInt(unit) + " (\"" +
                map.getUnitType(unit).scriptName + "\") cannot capture tile " +
                tile.toString() + " (\"" + map.getTileType(tile).type.scriptName +
                " \").");
        }
    }

    /**
     * Checks if there are units adjacent to a given tile that belong to the same
     * army as the one given, and that have lost some fuel and/or ammo.
     * @param  tile        The tile to search from.
     * @param  army        The ID of the army to search with.
     * @param  ignoreUnits An array of units to ignore in the search.
     * @return \c TRUE if there is at least one unit that belongs to the given
     *         army, on a tile directly adjacent to the tile given, that has lost
     *         some fuel and/or ammo. \c FALSE otherwise.
     */
    bool areThereDepletedArmyUnitsAdjacentTo(const Vector2&in tile,
        const ArmyID army, const array<UnitID>@ ignoreUnits) const {
        const auto units = _findUnits(tile, 1, 1);
        for (uint i = 0, length = units.length(); i < length; ++i) {
            const auto unitID = units[i];
            if (ignoreUnits.find(unitID) >= 0) continue;
            if (map.getArmyOfUnit(unitID) == army) {
                const auto unitType = map.getUnitType(unitID);
                if ((!unitType.hasInfiniteFuel &&
                    map.getUnitFuel(unitID) < unitType.maxFuel)) {
                    return true;
                }
                // Loop through every weapon.
                const auto weaponCount = unitType.weaponCount;
                for (uint64 w = 0; w < weaponCount; ++w) {
                    const auto weaponType = unitType.weapon(w);
                    if (!weaponType.hasInfiniteAmmo &&
                        map.getUnitAmmo(unitID, weaponType.scriptName) <
                            weaponType.maxAmmo) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    /**
     * Finds the units adjacent to a given tile that belong to the same army as
     * the one given, and that have lost some \em displayed HP, and/or have lost
     * some fuel and/or ammo.
     * @param  tile        The tile to search from.
     * @param  army        The ID of the army to search with.
     * @param  ignoreUnits An array of units to ignore in the search.
     * @return A list of unit IDs that match the given criteria.
     */
    array<UnitID>@ findDamagedOrDepletedArmyUnitsAdjacentTo(const Vector2&in tile,
        const ArmyID army, const array<UnitID>@ ignoreUnits) const {
        const auto units = _findUnits(tile, 1, 1);
        array<UnitID> res;
        for (uint i = 0, length = units.length(); i < length; ++i) {
            const auto unitID = units[i];
            if (ignoreUnits.find(unitID) >= 0) continue;
            if (map.getArmyOfUnit(unitID) == army) {
                const auto unitType = map.getUnitType(unitID);
                if (map.getUnitDisplayedHP(unitID) <
                    GetDisplayedHP(int(unitType.maxHP))) {
                    res.insertLast(unitID);
                    continue;
                }
                if ((!unitType.hasInfiniteFuel &&
                    map.getUnitFuel(unitID) < unitType.maxFuel)) {
                    res.insertLast(unitID);
                    continue;
                }
                // Loop through every weapon.
                const auto weaponCount = unitType.weaponCount;
                for (uint64 w = 0; w < weaponCount; ++w) {
                    const auto weaponType = unitType.weapon(w);
                    if (!weaponType.hasInfiniteAmmo &&
                        map.getUnitAmmo(unitID, weaponType.scriptName) <
                            weaponType.maxAmmo) {
                        res.insertLast(unitID);
                        break;
                    }
                }
            }
        }
        return res;
    }

    /**
     * Checks if there are units adjacent to a given tile that belong to the same
     * army as the one given, and that have lost some \em displayed HP, and/or
     * have lost some fuel and/or ammo.
     * @param  tile        The tile to search from.
     * @param  army        The ID of the army to search with.
     * @param  ignoreUnits An array of units to ignore in the search.
     * @return \c TRUE if there is at least one unit that belongs to the given
     *         army, on a tile directly adjacent to the tile given, that has lost
     *         some displayed HP. \c FALSE otherwise.
     */
    bool areThereDamagedOrDepletedArmyUnitsAdjacentTo(const Vector2&in tile,
        const ArmyID army, const array<UnitID>@ ignoreUnits) const {
        return findDamagedOrDepletedArmyUnitsAdjacentTo(tile, army, ignoreUnits).
            length() > 0;
    }

    /**
     * APC replenish code.
     * @param unit The ID of the APC unit who is replenishing adjacent units, if
     *             they belong to the same army as the APC unit.
     */
    void APCReplenishUnits(const UnitID unit) {
        array<UnitID> units = _findUnits(map.getUnitPosition(unit), 1, 1);
        auto apcArmy = map.getArmyOfUnit(unit);
        for (uint i = 0; i < units.length(); ++i) {
            if (apcArmy == map.getArmyOfUnit(units[i]))
                map.replenishUnit(units[i]);
        }
    }

    /**
     * Checks if the first unit can load onto the second.
     * @param  load ID of the unit to load onto another.
     * @param  onto ID of the unit which is accepting the \c load unit.
     * @return \c TRUE if the load can be carried out, \c FALSE otherwise.
     */
    bool canLoad(const UnitID load, const UnitID onto) const {
        /* Conditions:
        1. The two units must belong to the same army.
        2. `onto` must be able to load units of `load`'s type.
        3. `onto` must have space for `load`.
        4. There are other conditions that are already covered by map.loadUnit().
        5. The IDs of both the stationary unit and the moving unit cannot be
           NO_UNIT.
        */
        if (load == NO_UNIT || onto == NO_UNIT) return false;
        const auto ontoType = map.getUnitType(onto);
        return map.getArmyOfUnit(load) == map.getArmyOfUnit(onto) &&
            ontoType.canLoad[map.getUnitType(load).scriptName] &&
            map.getLoadedUnits(onto).length() < ontoType.loadLimit;
    }

    /**
     * Loads the \c load unit onto the \c onto unit, if it can be carried out.
     * If the first unit could not be loaded onto the second, a detailed error
     * will be logged.
     * @param load ID of the unit being loaded onto \c onto.
     * @param onto ID of the unit accepting \c load.
     * @param fuel The fuel to burn from \c load.
     */
    void loadUnit(const UnitID load, const UnitID onto, const Fuel burn) {
        if (canLoad(load, onto)) {
            // canLoad() will automatically deal with any load limits imposed by
            // the type of the `onto` unit.
            map.burnUnitFuel(load, burn);
            map.loadUnit(load, onto);
        } else {
            const auto ontoType = map.getUnitType(onto);
            error("Attempted to load unit " + formatUInt(load) + " onto unit " +
                formatUInt(onto) + ". Details: \n" + "Unit " + formatUInt(load) +
                ":\nType: " + map.getUnitType(load).scriptName + "\nUnit " +
                formatUInt(onto) + ":\nType: " + ontoType.scriptName + "\nLoad "
                "Limit: " + formatUInt(ontoType.loadLimit) + "\nUnits Currently "
                "Loaded: " + formatUInt(map.getLoadedUnits(onto).length()));
        }
    }

    /**
     * Checks if a given unit can unload any of its units from a given tile.
     * @param  unit ID of the unit who will unload.
     * @param  from The tile from which the given unit will unload.
     * @return \c TRUE if at least one unload can be carried out, \c FALSE
     *         otherwise.
     */
    bool canUnload(const UnitID unit, const Vector2&in from) const {
        /* Conditions:
        1. The unit ID must not be NO_UNIT.
        2. The unit must have at least one unit loaded onto it.
        3. `from` must be vacant, unless the unit occupying the tile is the same
           as `unit`.
        4. At least one of the loaded units must be able to move to at least one
           of the tiles adjacent to `from`.
        5. `unit` cannot itself be loaded onto another unit.
        6. `from` is a tile which `unit` can unload from.
        */
        if (unit == NO_UNIT) return false;
        if (map.getUnitWhichContainsUnit(unit) != NO_UNIT) return false;
        if (!map.getUnitType(unit).canUnloadFrom[
            map.getTileType(from).type.scriptName]) {
            return false;
        }
        const auto loadedUnits = map.getLoadedUnits(unit);
        const auto loadedUnitsLength = loadedUnits.length();
        if (loadedUnitsLength == 0 ||
            (map.getUnitOnTile(from) != unit &&
            map.getUnitOnTile(from) != NO_UNIT))
            return false;
        const auto adjacentTiles = map.getAvailableTiles(from, 1, 1);
        for (uint i = 0; i < loadedUnitsLength; ++i) {
            const auto unitID = loadedUnits[i];
            const auto movementType = map.getUnitType(unitID).movementType;
            const auto unitTeam = map.getTeamOfUnit(unitID);
            for (uint j = 0, len = adjacentTiles.length(); j < len; ++j) {
                if (map.findPathForUnloadUnit(from, adjacentTiles[j],
                    movementType, map.getArmyOfUnit(unit), { unit }).length() > 0)
                    {
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * Checks if a given unit, that is loaded on another unit, can be unloaded
     * onto at least one of the given tiles.
     * @warning Does not count "unloads in progress" as occupied!
     * @param   fromUnit      ID of the unit who will unload.
     * @param   fromTile      The tile from which the given unit will unload.
     * @param   unloadingUnit ID of the unit that is being unloaded.
     * @param   toTile        The tiles to attempt to unload the unit to (no
     *                        unload will actually be attempted).
     * @return  \c TRUE if the unload operation can be carried out, \c FALSE
     *          otherwise. If \c toTile is \c NULL, then this method will return
     *          \c TRUE if \c unloadingUnit can unload to any of the tiles
     *          adjacent to \c fromTile.
     */
    bool canUnload(const UnitID fromUnit, const Vector2&in fromTile,
        const UnitID unloadingUnit, const array<Vector2>&in toTiles) const {
        /* Conditions:
        1. The unit IDs must not be NO_UNIT.
        2. `unloadingUnit` must be loaded onto `fromUnit`.
        3. None of `toTiles` can be equal to `fromTile`.
        4. `fromTile` and `toTiles` must all be vacant, unless any tile is
           occupied by `fromUnit`.
        5. `unloadingUnit` must be able to move to a `toTile` from `fromTile`
           (remembering to ignore `fromUnit`).
        6. `fromUnit` cannot itself be loaded onto another unit.
        7. `toTiles` cannot be empty.
        */
        if (fromUnit == NO_UNIT || unloadingUnit == NO_UNIT) return false;
        if (toTiles.length() == 0) return false;
        if (map.getUnitWhichContainsUnit(fromUnit) != NO_UNIT) return false;
        if (!map.getUnitType(fromUnit).
            canUnloadFrom[map.getTileType(fromTile).type.scriptName]) {
            return false;
        }
        if (!map.isUnitLoadedOntoUnit(unloadingUnit, fromUnit)) return false;
        const auto unitOnFromTile = map.getUnitOnTile(fromTile);
        if (unitOnFromTile != NO_UNIT && unitOnFromTile != fromUnit) return false;
        const auto unloadingUnitMoveType =
            map.getUnitType(unloadingUnit).movementType;
        const auto toTilesLength = toTiles.length();
        for (uint i = 0; i < toTilesLength; ++i) {
            const auto tile = toTiles[i];
            if (tile != fromTile && map.findPathForUnloadUnit(fromTile, tile,
                unloadingUnitMoveType, map.getArmyOfUnit(fromUnit),
                { fromUnit }).length() > 0) return true;
        }
        return false;
    }

    /**
     * Checks if the given Oozium unit can move onto a tile.
     * Does not check if the Oozium can perform a join. <b>It is assumed that the
     * Oozium can find a path to the given tile.</b>
     * @param  oozium The ID of the Oozium unit.
     * @param  toTile The tile to move the Oozium to.
     * @return \c TRUE if the given unit is an Oozium and it can move onto the
     *         given tile.
     */
    bool canOoziumMove(const UnitID oozium, const Vector2&in toTile) const {
        /* Conditions:
        1. The given unit exists, and is an Oozium.
        2. The tile is vacant,
        3. OR, the tile is occupied by a unit on an opposing team.
        */
        if (oozium == NO_UNIT) return false;
        if (map.getUnitType(oozium).scriptName != "OOZIUM") return false;
        const auto unitOnTile = map.getUnitOnTile(toTile);
        return unitOnTile == NO_UNIT ||
            map.getTeamOfUnit(unitOnTile) != map.getTeamOfUnit(oozium);
    }

    /**
     * Fills a dictionary with all the tiles which contain possible targets for
     * the given unit, attacking from the given tile.
     * The keys will be string forms of the positions of tiles containing targets
     * (whether it be tile, unit, or both), and the value will be the script name
     * of the weapon best suited to attack the given target with.\n
     * If a tile contains both a unit and tile target, then the unit target will/
     * must be chosen.
     * @warning This method will \em not clear the given dictionary before
     *          processing.
     * @param   result             Handle to the dictionary to write the results
     *                             to.
     * @param   attackingUnit      The ID of the unit who's attacking.
     * @param   fromTile           The tile from which the unit will attack.
     * @param   isCounterattacking If \c TRUE, \c attackingUnit is performing a
     *                             counterattack, and some extra checks need to be
     *                             carried out.
     */
    void findTilesWithTargets(dictionary@ result, const UnitID attackingUnit,
        const Vector2&in fromTile, const bool isCounterattacking = false) const {
        // No tiles can contain targets if there isn't a unit who is attacking.
        if (attackingUnit == NO_UNIT) return;
        // If `fromTile` isn't vacant and the unit on that tile isn't the
        // attacking unit, then the unit can't attack.
        if (map.getUnitOnTile(fromTile) != NO_UNIT &&
            map.getUnitOnTile(fromTile) != attackingUnit) return;
        const bool isMoving = map.getUnitPosition(attackingUnit) != fromTile;
        const auto unitType = map.getUnitType(attackingUnit);
        // Cycle through every weapon that the attacking unit possesses.
        for (uint64 weaponID = 0, weaponCount = unitType.weaponCount;
            weaponID < weaponCount; ++weaponID) {
            const auto weaponType = unitType.weapon(weaponID);
            // If the unit is moving as part of the attack, check if this weapon
            // can be used, and if not, check the next weapon.
            if (isMoving && !weaponType.canAttackAfterMoving) continue;
            // If this weapon hasn't got infinite ammo, and the ammo has run out,
            // this weapon can't be used, try the next one.
            if (!weaponType.hasInfiniteAmmo &&
                map.getUnitAmmo(attackingUnit, weaponType.scriptName) <= 0) {
                continue;
            }
            // Now, go through all available tiles, within the range configured
            // for the weapon, and see if the weapon can attack any units or tiles
            // within that range.
            const auto availableTiles = map.getAvailableTiles(fromTile,
                weaponType.range.x, weaponType.range.y);
            for (uint64 t = 0, tileCount = availableTiles.length();
                t < tileCount; ++t) {
                const auto tile = availableTiles[t];
                const auto tileStr = tile.toString();
                // If the attacker is counterattacking, then we need to perform
                // some more checks here, namely:
                // 1. If the target is directly next to the attacker, and this
                //    weapon cannot counterattack directly, then we must ignore
                //    this target.
                // 2. If the target is not directly next to the attacker, and this
                //    weapon cannot counterattack indirectly, then we must ignore
                //    this target.
                if (isCounterattacking) {
                    const auto distanceBetweenAttackerAndTarget =
                        Distance(fromTile, tile);
                    if (distanceBetweenAttackerAndTarget == 1 &&
                        !weaponType.canCounterattackDirectly) continue;
                    if (distanceBetweenAttackerAndTarget > 1 &&
                        !weaponType.canCounterattackIndirectly) continue;
                }
                // Prioritise units over tiles. Units can't be attacked if they
                // aren't visible to the attacking unit, or if they are on the
                // same team as the attacking unit.
                const auto defendingUnit = map.getUnitOnTile(tile);
                if (defendingUnit != NO_UNIT &&
                    map.getTeamOfUnit(attackingUnit) !=
                    map.getTeamOfUnit(defendingUnit) &&
                    map.isUnitVisible(defendingUnit,
                    map.getArmyOfUnit(attackingUnit))) {
                    // There is a unit that could be a target. Check if this
                    // weapon can attack this type of unit.
                    if (weaponType.canAttackUnit(
                        map.getUnitType(defendingUnit).scriptName,
                        map.isUnitHiding(defendingUnit))) {
                        // Add this tile as a target, but only if it wasn't added
                        // previously.
                        // If it was added previously, then it can only override
                        // the selected weapon if it deals more damage, or if it
                        // deals the same damage, but has infinite ammo, and the
                        // stored weapon has finite ammo.
                        if (result.exists(tileStr)) {
                            const auto defendingUnitTypeName =
                                map.getUnitType(defendingUnit).scriptName;
                            string storedWeaponName;
                            result.get(tileStr, storedWeaponName);
                            const auto storedWeapon =
                                unitType.weapon(storedWeaponName);
                            const auto baseDamageThisWeapon =
                                weaponType.getBaseDamageUnit(
                                    defendingUnitTypeName);
                            const auto baseDamageStoredWeapon =
                                storedWeapon.getBaseDamageUnit(
                                    defendingUnitTypeName);
                            if (baseDamageThisWeapon == baseDamageStoredWeapon) {
                                if (weaponType.hasInfiniteAmmo &&
                                    !storedWeapon.hasInfiniteAmmo) {
                                    result.set(tileStr, weaponType.scriptName);
                                }
                            } else if (baseDamageThisWeapon >
                                baseDamageStoredWeapon) {
                                result.set(tileStr, weaponType.scriptName);
                            }
                        } else {
                            result.set(tileStr, weaponType.scriptName);
                        }
                    }
                }
                // If there isn't a unit on this tile, or if the unit couldn't be
                // attacked, see if this weapon can attack this tile.
                if (weaponType.canAttackTerrain(
                    map.getTileType(tile).type.scriptName)) {
                    // Add this tile as a target, but only if it wasn't added
                    // previously, and if it is vacant.
                    // If it was added previously, then it can only override the
                    // selected weapon if it deals more damage, or if it deals the
                    // same damage, but has infinite ammo, and the stored weapon
                    // has finite ammo.
                    if (defendingUnit == NO_UNIT) {
                        if (result.exists(tileStr)) {
                            const auto terrainTypeName =
                                map.getTileType(tile).type.scriptName;
                            string storedWeaponName;
                            result.get(tileStr, storedWeaponName);
                            const auto storedWeapon =
                                unitType.weapon(storedWeaponName);
                            const auto baseDamageThisWeapon =
                                weaponType.getBaseDamageTerrain(terrainTypeName);
                            const auto baseDamageStoredWeapon =
                                storedWeapon.getBaseDamageTerrain(
                                    terrainTypeName);
                            if (baseDamageThisWeapon == baseDamageStoredWeapon) {
                                if (weaponType.hasInfiniteAmmo &&
                                    !storedWeapon.hasInfiniteAmmo) {
                                    result.set(tileStr, weaponType.scriptName);
                                }
                            } else if (baseDamageThisWeapon >
                                baseDamageStoredWeapon) {
                                result.set(tileStr, weaponType.scriptName);
                            }
                        } else {
                            result.set(tileStr, weaponType.scriptName);
                        }
                    }
                }
            }
        }
    }

    /**
     * Checks if a unit can attack from a given tile.
     * @param attackingUnit The ID of the unit who's attacking
     * @param fromTile      Attacking from this tile.
     */
    bool canAttack(const UnitID attackingUnit, const Vector2&in fromTile) const {
        dictionary t;
        findTilesWithTargets(t, attackingUnit, fromTile);
        return t.getSize() > 0;
    }

    /**
     * Calculates the base damage that one weapon deals to another tile on the
     * map.
     * This method takes into account defensive terrain, COM tower count, and the
     * HP of both the attacker and the defender, if the defender is a unit.
     * @param attacker   The ID of the unit who's attacking.
     * @param weaponName The name of the weapon the attacker is using.
     * @param defender   The tile who's defending. If the tile is being occupied
     *                   by a unit, then the unit will be the defender.
     * @param baseOnly   If \c TRUE is given, no luck modifiers will be applied.
     */
    int calculateDamage(const UnitID attacker, const string&in weaponName,
        const Vector2&in defender, bool baseOnly) {
        // Credit: https://awbw.fandom.com/wiki/Damage_Formula.
        const auto comTowerCount = map.countTilesBelongingToArmy(
            map.getArmyOfUnit(attacker), "COMTOWER");
        const auto defenderUnit = map.getUnitOnTile(defender);
        // Find base damage.
        int baseDamage = 0;
        const auto attackerWeapon = map.getUnitType(attacker).weapon(weaponName);
        const auto defenderTerrainType = map.getTileType(defender).type;
        int displayedHPOfDefender = 0;
        uint defenderDefenceRating = 0;
        if (defenderUnit != NO_UNIT) {
            // Unit is the defender.
            baseDamage = attackerWeapon.getBaseDamageUnit(
                map.getUnitType(defenderUnit).scriptName,
                map.isUnitHiding(defenderUnit));
            displayedHPOfDefender = map.getUnitDisplayedHP(defenderUnit);
            defenderDefenceRating = map.getUnitDefence(defenderUnit);
        } else {
            // Tile is the defender.
            baseDamage = attackerWeapon.getBaseDamageTerrain(
                defenderTerrainType.scriptName);
            displayedHPOfDefender = GetDisplayedHP(map.getTileHP(defender));
            defenderDefenceRating = defenderTerrainType.defence;
        }
        // Apply CO attack percentage here:
        // baseDamage = baseDamage * 100 / 100
        // Roll for luck.
        const int luck = ((baseOnly) ? (0) : (rand(9)));
        // Calculate attacker HP multiplier.
        double attackerDisplayedHPMultiplier =
            double(map.getUnitDisplayedHP(attacker)) / 10;
        // Calculate defender HP multiplier, apply CO defence percentage here:
        double defenderDisplayedHPMultiplier = double(200 - (100 +
            defenderDefenceRating * displayedHPOfDefender)) / 100;
        // Calculate total damage%.
        double totalDamage = (baseDamage + luck) * attackerDisplayedHPMultiplier *
            defenderDisplayedHPMultiplier;
        // Apply COM tower buffs.
        totalDamage += double(comTowerCount) * 10.0;
        // Round up to the nearest 0.05 interval.
        const double mod = totalDamage % 0.05;
        if (mod < 0.025) {
            totalDamage -= mod;
        } else {
            totalDamage += 0.05 - mod;
        }
        // Round down, i.e. floor(), i.e. cast to int.
        return int(totalDamage);
    }

    /**
     * Carries out an attack between two combatants.
     * A lot of attacking checks are assumed to be carried out first using
     * \c findTilesWithTargets() (such as ammo checks).\n
     * The attacker must have already been moved onto the tile it is attacking
     * from before making a call to this method!
     * @param targets  A dictionary of targets generated previously using
     *                 \c findTilesWithTargets().
     * @param attacker The ID of the unit who is performing the attack.
     * @param defender The tile defending itself. If there is a unit on this tile,
     *                 it will be selected as the defender instead of the tile.
     */
    void attack(const dictionary@ targets, const UnitID attacker,
        const Vector2&in defender) {
        // Find the weapon that should be used against the target. If it can't be
        // found, then we know the attack can't be carried out. Also, if the found
        // weapon can't attack the given target, then we must cancel the attack.
        const auto defenderStr = defender.toString();
        if (!targets.exists(defenderStr)) {
            error("Unit with ID " + formatUInt(attacker) + " cannot attack "
                "defender [on] tile " + defenderStr + " as it was not in the "
                "available list of targets!");
            return;
        }
        string weaponName;
        targets.get(defenderStr, weaponName);
        const auto attackerType = map.getUnitType(attacker);
        if (attackerType is null) {
            error("Could not retrieve type of unit with ID " +
                formatUInt(attacker) + ", cancelling the attack against defender "
                "[on] tile " + defenderStr + ".");
            return;
        }
        // Find out if the defender is a tile or a unit.
        const auto defenderTerrainTypeName =
            map.getTileType(defender).type.scriptName;
        const auto defenderUnit = map.getUnitOnTile(defender);
        const auto attackerWeapon = attackerType.weapon(weaponName);
        bool attackerIsAlive = true;
        if (defenderUnit != NO_UNIT) {
            // Defender is a unit.
            const auto defenderUnitIsHidden = map.isUnitHiding(defenderUnit);
            const auto defenderUnitType = map.getUnitType(defenderUnit);
            if (!attackerWeapon.canAttackUnit(defenderUnitType.scriptName,
                defenderUnitIsHidden)) {
                error("Unit with ID " + formatUInt(attacker) + " attempted to "
                    "attack unit with ID " + formatUInt(defenderUnit) + " using "
                    "weapon \"" + weaponName + "\", which cannot attack units of "
                    "type \"" + defenderTerrainTypeName + "\". The defending "
                    "unit was " + ((defenderUnitIsHidden) ? ("") : ("not ")) +
                    "hidden at the time of the call.");
                return;
            }
            // Carry out attack!
            const auto damage = calculateDamage(attacker, weaponName, defender,
                false);
            const auto newUnitHP = map.getUnitHP(defenderUnit) - damage;
            map.setUnitHP(defenderUnit, newUnitHP);
            // If unit HP has reached 0, do not counterattack and delete unit.
            // Otherwise, perform counterattack, if possible.
            if (newUnitHP <= 0) {
                deleteUnit(defenderUnit);
            } else {
                // First, see if the attacker is a potential target for the
                // defender.
                dictionary defenderTargets;
                findTilesWithTargets(defenderTargets, defenderUnit, defender,
                    true);
                const auto keys = defenderTargets.getKeys();
                const auto attackerPosition = map.getUnitPosition(attacker);
                for (uint i = 0, len = keys.length(); i < len; ++i) {
                    const auto tile = Vector2(keys[i]);
                    if (tile == attackerPosition) {
                        // Counterattack!
                        string defenderWeaponName;
                        defenderTargets.get(keys[i], defenderWeaponName);
                        const auto defendersDamage = calculateDamage(
                            defenderUnit, defenderWeaponName,
                            attackerPosition, false);
                        const auto newUnitHP2 = map.getUnitHP(attacker) -
                            defendersDamage;
                        map.setUnitHP(attacker, newUnitHP2);
                        // If the defending unit somehow killed its attacker, then
                        // delete it.
                        if (newUnitHP2 <= 0) {
                            deleteUnit(attacker);
                            attackerIsAlive = false;
                        }
                        // If the defending unit's weapon has finite ammo, remove
                        // 1.
                        if (!map.getUnitType(defenderUnit).
                            weapon(defenderWeaponName).hasInfiniteAmmo) {
                            map.setUnitAmmo(defenderUnit, defenderWeaponName,
                                map.getUnitAmmo(defenderUnit,
                                defenderWeaponName) - 1);
                        }
                    }
                }
            }
        } else {
            // Defender is a tile.
            if (!attackerWeapon.canAttackTerrain(defenderTerrainTypeName)) {
                error("Unit with ID " + formatUInt(attacker) + " attempted to "
                    "attack tile " + defenderStr + " using weapon \"" +
                    weaponName + "\", which cannot attack tiles of type \"" +
                    defenderTerrainTypeName + "\".");
                return;
            }
            // Carry out attack! Tiles never counterattack, nor do they have luck
            // rolls.
            const auto damage = calculateDamage(attacker, weaponName, defender,
                true);
            const auto newTileHP = map.getTileHP(defender) - damage;
            map.setTileHP(defender, newTileHP);
            // If the tile's HP has reached zero, invoke the helper method.
            if (newTileHP <= 0) {
                _tileHasBeenDestroyed(defender, map.getTileType(defender));
            }
        }
        // If the weapon used has finite ammo, and it is still alive, then remove
        // one from its ammo count.
        if (attackerIsAlive && !attackerWeapon.hasInfiniteAmmo) {
            map.setUnitAmmo(attacker, weaponName,
                map.getUnitAmmo(attacker, weaponName) - 1);
        }
    }

    /**
     * Damage all units within a given range by a given amount, but don't delete
     * them if they reach 0HP.
     * @param fromTile          The tile from which to measure the range.
     * @param lowRange          The number of tiles away from \c fromTile the
     *                          range begins.
     * @param highRange         The number of tiles the range goes on for.
     * @param displayedHP       The HP to remove from all units found in the
     *                          range, in \em displayed HP format.
     * @param unitTypesToIgnore An array of unit type script names. If a unit is
     *                          of this type, then it won't be damaged by this
     *                          method.
     */
    void damageUnitsInRange(const Vector2&in fromTile, const uint lowRange,
        const uint highRange, const HP displayedHP,
        const array<string>@ unitTypesToIgnore) {
        const auto units = _findUnits(fromTile, lowRange, highRange);
        for (uint i = 0, len = units.length(); i < len; ++i) {
            const auto unitID = units[i];
            if (unitTypesToIgnore.find(map.getUnitType(unitID).scriptName) >= 0) {
                continue;
            }
            auto newHP = map.getUnitHP(unitID) - GetInternalHP(displayedHP);
            if (newHP <= 0) newHP = 1;
            map.setUnitHP(unitID, newHP);
        }
    }

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
    private void _beginTurnForUnit(const UnitID unit, const UnitType@ type,
        const Vector2&in position) {
        string typeName = type.scriptName;

        if (typeName == "APC") {
            APCReplenishUnits(unit);

        } else if (typeName == "TCOPTER" || typeName == "BCOPTER") {
            map.burnUnitFuel(unit, 2);
        
        } else if (typeName == "FIGHTER" || typeName == "BOMBER" ||
            typeName == "STEALTH" || typeName == "BLACKBOMB") {
            if (typeName == "STEALTH" && map.isUnitHiding(unit)) {
                map.burnUnitFuel(unit, 8);
            } else {
                map.burnUnitFuel(unit, 5);
            }
        
        } else if (typeName == "BLACKBOAT" || typeName == "LANDER" ||
            typeName == "CRUISER" || typeName == "SUB" ||
            typeName == "BATTLESHIP" || typeName == "CARRIER") {
            if (typeName == "SUB" && map.isUnitHiding(unit)) {
                map.burnUnitFuel(unit, 5);
            } else {
                map.burnUnitFuel(unit, 1);
            }

            if (typeName == "CARRIER" || typeName == "CRUISER") {
                // Replenish the units that are loaded onto this one.
                const auto loadedUnits = map.getLoadedUnits(unit);
                const auto loadedUnitsLength = loadedUnits.length();
                for (uint i = 0; i < loadedUnitsLength; ++i) {
                    map.replenishUnit(loadedUnits[i]);
                }
            }
        }
    }

    /**
     * When a turn ends, the next army's tiles will be given to this method.
     * @param tile        The location of the tile.
     * @param terrain     The type of terrain this tile has.
     * @param currentArmy The ID of the army who owns the tile.
     */
    private void _beginTurnForTile(const Vector2&in tile,
        const Terrain@ terrain, const ArmyID currentArmy) {
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
            if (unitOnTile != NO_UNIT) {
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
                            map.replenishUnit(unitOnTile);
                        }
                    } else if (terrainType == "AIRPORT") {
                        if (unitType == "AIR") {
                            healUnit(unitOnTile, 2, currentArmy);
                            map.replenishUnit(unitOnTile);
                        }
                    } else if (terrainType == "PORT") {
                        if (unitType == "SHIPS" || unitType == "TRANSPORT") {
                            healUnit(unitOnTile, 2, currentArmy);
                            map.replenishUnit(unitOnTile);
                        }
                    }
                }
            }
        }

        // Plane and ship crash checks.
        const uint unitsSize = units.length();
        for (uint i = 0; i < unitsSize; ++i) {
            // If unit has infinite fuel, skip fuel checks.
            const auto unit = map.getUnitType(units[i]);
            if (unit.hasInfiniteFuel) continue;

            string type = unit.movementType.scriptName;
            if (type == "AIR" || type == "SHIPS" || type == "TRANSPORT") {
                if (map.getUnitFuel(units[i]) <= 0) deleteUnit(units[i]);
            }
        }
    }

    //////////////////////////////////
    // TILE AND UNIT SEARCH METHODS //
    //////////////////////////////////
    /**
     * Creates a list of units that are within the specified range of a given
     * tile, regardless of whether or not they are visible to any given army.
     * @param  position  The tile to begin the search from.
     * @param  startFrom The number of tiles away from the given tile to start on.
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
            if (unitID != NO_UNIT) ret.insertLast(unitID);
        }
        if (startFrom == 0) {
            const auto unitID = map.getUnitOnTile(position);
            if (unitID != NO_UNIT) ret.insertLast(unitID);
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
        const UnitType@);
    funcdef bool DEF2(Map@);
    private void _updateMoveModeClosedList() {
        // Declare the function used to revert back to the shortest path.
        DEF@ replaceWithShortestPath = function(pThis, tile, unitID, unitType){
            pThis.map.closedList.removeRange(0, pThis.map.closedList.length());
            Vector2 src = pThis.map.getUnitPreviewPosition(unitID);
            if (src == NO_POSITION) {
                // If the unloading unit doesn't have a location override yet,
                // then use the location override of the unit that it's loaded
                // onto.
                src = pThis.map.getUnitPreviewPosition(
                    pThis.map.getUnitWhichContainsUnit(unitID)
                );
            }
            const auto arr = pThis.map.findPath(
                src,
                tile,
                unitType.movementType,
                unitType.movementPoints,
                pThis.map.getUnitFuel(unitID),
                pThis.map.getTeamOfUnit(unitID),
                pThis.map.getArmyOfUnit(unitID),
                unitType.hasInfiniteFuel,
                unitType.scriptName == "OOZIUM"
            );
            for (uint i = 0, length = arr.length(); i < length; ++i) {
                pThis.newClosedListNode(pThis.map.closedList, -1, arr[i].tile,
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
        if (unitID != NO_UNIT && map.isAvailableTile(tile)) {
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
                        unitType.movementTypeScriptName];
                if (tentativeGScore <= map.getUnitFuel(unitID) &&
                    uint(tentativeGScore) <= unitType.movementPoints) {
                    newClosedListNode(map.closedList, -1, tile, tentativeGScore);

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
    void newClosedListNode(array<ClosedListNode>& arr, int64 index,
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

    //////////////////////////
    // MISC. HELPER METHODS //
    //////////////////////////
    /**
     * When a tile's HP reaches 0 after it has been attacked, this method will be
     * invoked.
     * @param tile The position of the tile that has ran out of HP.
     * @param type The tile type of the tile.
     */
    private void _tileHasBeenDestroyed(const Vector2&in tile,
        const TileType@ type) {
        // Pipe seams need to transform into plains.
        if (type.scriptName == "05Epipeseamhori") {
            map.setTileType(tile, "060plainseamhori");
        } else if (type.scriptName == "05Fpipeseamvert") {
            map.setTileType(tile, "061plainseamvert");
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

    //////////
    // DATA //
    //////////
    /**
     * The map scaling factor.
     */
    private float _mapScalingFactor = 2.0f;

    /**
     * Flag which determines if the \c _updateMoveModeClosedList() method should
     * be called.
     */
    private bool _closedListEnabled = true;
}
