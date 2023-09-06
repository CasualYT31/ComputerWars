/**
 * @file PreviewMoveUnitMenu.as
 * Code that's run when the user is previewing a unit move operation.
 */

/**
 * Stores the commands available for the moving unit.
 */
CommandWidget PreviewCommands;

/**
 * Sets up the menu that is shown whilst previewing a unit move operation.
 */
void PreviewMoveUnitMenuSetUp() {
    PreviewCommands = CommandWidget("Menu", "100px");
}

/**
 * Positions the command menu relative to the current mouse position.
 * Also adds all the buttons to the command menu.
 */
void PreviewMoveUnitMenuOpen() {
    // Gather data.
    const auto unit = game.map.getSelectedUnit();
    const auto unitType = game.map.getUnitType(unit);
    const auto tile = game.map.getSelectedTile();
    const auto tileType = game.map.getTileType(tile);
    const auto tileStructure = game.map.isTileAStructureTile(tile) ?
        game.map.getTileStructure(tile) : null;
    const auto unitOnTile = game.map.getUnitOnTile(tile);

    // Add commands here.
    if (game.canJoin(unitOnTile, unit)) {
        PreviewCommands.addCommand("Join", "join", "joinicon");
    } else {
        if (game.canLoad(unit, unitOnTile)) {
            PreviewCommands.addCommand("Load", "load", "loadicon");
        } else {
            if (game.canAttack(unit, tile)) {
                PreviewCommands.addCommand("Fire", "fire", "attackicon");
            }
            if (game.canCapture(unit, tile)) {
                PreviewCommands.addCommand("Capture", "capture", "captureicon");
            }
            // Launch Missile Silo.
            if (tileStructure !is null &&
                tileStructure.scriptName == "MISSILESILO" &&
                (unitType.scriptName == "INFANTRY" ||
                unitType.scriptName == "MECH")) {
                PreviewCommands.addCommand("Launch", "launch", "attackicon");
            }
            // Remember to ignore the APC itself when searching for adjacent
            // units: it hasn't moved yet, so the engine still thinks it's at its
            // original position.
            if (unitType.scriptName == "APC" &&
                game.areThereDepletedArmyUnitsAdjacentTo(tile,
                    game.map.getArmyOfUnit(unit), { unit })) {
                PreviewCommands.addCommand("Supply", "supply", "replenishicon");
            } else if (unitType.scriptName == "BLACKBOAT" &&
                game.areThereDamagedOrDepletedArmyUnitsAdjacentTo(tile,
                    game.map.getArmyOfUnit(unit), { unit })) {
                PreviewCommands.addCommand("Repair", "repair", "repairicon");
            }
            if (game.canUnload(unit, tile)) {
                PreviewCommands.addCommand("Unload", "unload", "unloadicon");
            }
            if (unitType.canHide) {
                if (game.map.isUnitHiding(unit)) {
                    if (unitType.scriptName == "SUB") {
                        PreviewCommands.addCommand("Show", "surface",
                            "unhideicon");
                    } else {
                        PreviewCommands.addCommand("Show", "unhide",
                            "unhideicon");
                    }
                } else {
                    if (unitType.scriptName == "SUB") {
                        PreviewCommands.addCommand("Hide", "dive", "hideicon");
                    } else {
                        PreviewCommands.addCommand("Hide", "hide", "hideicon");
                    }
                }
            }
            if (unitType.scriptName == "BLACKBOMB") {
                PreviewCommands.addCommand("Explode", "explode", "attackicon");
            }
            if (unitType.scriptName == "OOZIUM" && unitOnTile != NO_UNIT &&
                unitOnTile != unit) {
                PreviewCommands.addCommand("OoziumFire", "fire", "attackicon");
            } else {
                PreviewCommands.addCommand("Wait", "wait", "waiticon");
            }
        }
    }

    // Position the command menu differently depending on the quadrant of the
    // screen that the mouse is in.
    const Vector2f commandMenuSize = getWidgetFullSize(PreviewCommands.layout);
    int xOffset = 10;
    if (mousePosition().x >= int(getWindowSize().x / 2)) {
        xOffset = xOffset - int(commandMenuSize.x) - 20;
    }
    int yOffset = 10;
    if (mousePosition().y >= int(getWindowSize().y / 2)) {
        yOffset = yOffset - int(commandMenuSize.y) - 20;
    }
    auto mouse = scaledMousePosition();
    if (mouse == INVALID_MOUSE) {
        mouse.x = 0;
        mouse.y = 0;
    }
    setWidgetPosition(PreviewCommands.layout, formatInt(mouse.x + xOffset) + "px",
        formatInt(mouse.y + yOffset) + "px");
}

/**
 * Removes all buttons from the command menu.
 */
void PreviewMoveUnitMenuClose() {
    PreviewCommands.removeAllCommands();
}

/**
 * Allows the user to cancel a unit move operation.
 * @param controls The control map given by the engine.
 */
void PreviewMoveUnitMenuHandleInput(const dictionary controls) {
    if (bool(controls["back"])) {
        game.map.disableSelectedUnitRenderingEffects(false);
        game.map.removePreviewUnit(game.map.getSelectedUnit());
        setGUI("MoveUnitMenu");
        return;
    }
}

/**
 * Moves a unit and makes it wait at its new destination.
 */
void PreviewMoveUnitMenu_Wait_MouseReleased() {
    game.moveUnit();
    setGUI("Map");
}

/**
 * Joins two units together.
 */
void PreviewMoveUnitMenu_Join_MouseReleased() {
    game.joinUnits(
        game.map.getUnitOnTile(game.map.getSelectedTile()),
        game.map.getSelectedUnit(),
        game.map.closedList[game.map.closedList.length() - 1].g
    );
    setGUI("Map");
}

/**
 * Moves a unit and make it capture a property.
 */
void PreviewMoveUnitMenu_Capture_MouseReleased() {
    game.moveUnitAndCapture();
    setGUI("Map");
}

/**
 * Resupplies adjacent units and makes the selected unit wait.
 */
void PreviewMoveUnitMenu_Supply_MouseReleased() {
    // Cache selected unit, as moveUnit() unselects it.
    const auto selectedUnit = game.map.getSelectedUnit();
    game.moveUnit();
    game.APCReplenishUnits(selectedUnit);
    setGUI("Map");
}

/**
 * Loads the moving unit onto the stationary one.
 */
void PreviewMoveUnitMenu_Load_MouseReleased() {
    game.loadUnit(
        game.map.getSelectedUnit(),
        game.map.getUnitOnTile(game.map.getSelectedTile()),
        game.map.closedList[game.map.closedList.length() - 1].g
    );
    // Deselect unit and remove unit previews!
    game.selectUnit(NO_UNIT);
    game.map.removeAllPreviewUnits();
    setGUI("Map");
}

/**
 * Allows the user to select units to unload.
 */
void PreviewMoveUnitMenu_Unload_MouseReleased() {
    setGUI("UnloadUnitsMenu");
}

/**
 * Move a unit and unhide it.
 */
void PreviewMoveUnitMenu_Show_MouseReleased() {
    game.moveUnitHide(false);
    setGUI("Map");
}

/**
 * Move a unit and hide it.
 */
void PreviewMoveUnitMenu_Hide_MouseReleased() {
    game.moveUnitHide(true);
    setGUI("Map");
}

/**
 * Allows the user to select a target for an attack.
 */
void PreviewMoveUnitMenu_Fire_MouseReleased() {
    setGUI("SelectTargetMenu");
}

/**
 * Allows the user to attack with an Oozium.
 * The unit occupying the selected tile is deleted, and the Oozium moves to the
 * selected tile.
 */
void PreviewMoveUnitMenu_OoziumFire_MouseReleased() {
    game.deleteUnit(game.map.getUnitOnTile(game.map.getSelectedTile()));
    PreviewMoveUnitMenu_Wait_MouseReleased();
}

/**
 * Allows the user to preview a Black Bomb explosion.
 */
void PreviewMoveUnitMenu_Explode_MouseReleased() {
    EXPLODE_PREVIEW_MENU_DISPLAYED_HP_TO_DEAL = 5;
    EXPLODE_PREVIEW_MENU_RANGE.x = 1;
    EXPLODE_PREVIEW_MENU_RANGE.y = 3;
    EXPLODE_PREVIEW_MENU_ALLOW_TILE_SELECTION_CHANGE = false;
    @EXPLODE_PREVIEW_MENU_CALLBACK = function(){
        // If the explosion is going ahead, then we need to delete the Black Bomb
        // unit.
        game.deleteUnit(game.map.getSelectedUnit());
    };
    setGUI("ExplodePreviewMenu");
}

/**
 * Allows the user to select a unit to repair.
 */
void PreviewMoveUnitMenu_Repair_MouseReleased() {
    setGUI("RepairMenu");
}

/**
 * Allows the user to preview launching a Missile Silo.
 */
void PreviewMoveUnitMenu_Launch_MouseReleased() {
    EXPLODE_PREVIEW_MENU_DISPLAYED_HP_TO_DEAL = 3;
    EXPLODE_PREVIEW_MENU_RANGE.x = 0;
    EXPLODE_PREVIEW_MENU_RANGE.y = 3;
    EXPLODE_PREVIEW_MENU_ALLOW_TILE_SELECTION_CHANGE = true;
    @EXPLODE_PREVIEW_MENU_CALLBACK = function(){
        // If the launch is going ahead, then we need to make the Missile Silo
        // empty. Since it's a structure, we can destroy it to achieve this.
        game.map.destroyStructure(EXPLODE_PREVIEW_MENU_SELECTED_TILE);
        // Move the unit that is launching the silo.
        game.moveUnit();
    };
    setGUI("ExplodePreviewMenu");
}
