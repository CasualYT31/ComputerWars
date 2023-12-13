/**
 * @file UnloadUnitsMenu.as
 * The menu that allows the current army to select units to unload.
 */

/**
 * The unit unload menu.
 */
class UnloadUnitsMenu : Menu, Group {
    /**
     * Set up the menu.
     */
    UnloadUnitsMenu() {
        panel.setBackgroundColour(Colour(0, 0, 0, 128));
        panel.setOrigin(0.5, 0.5);
        panel.setPosition("50%", "50%");
        panel.setSize("38%", "38%");
        panel.add(grid);
        add(panel);
        ::add(ROOT_WIDGET, this);
    }

    /**
     * Invoked when the menu is opened.
     * @param oldMenu Handle to the menu that was open before this one.
     */
    void Open(Menu@ const oldMenu) {
        selectedTileCache = game.map.getSelectedTile();
        selectedUnitCache = game.map.getSelectedUnit();

        // Add a row for each of the loaded units.
        const auto loadedUnits = game.map.getLoadedUnits(selectedUnitCache);
        const auto loadedUnitsLength = loadedUnits.length();
        for (uint i = 0; i < loadedUnitsLength; ++i) grid.addUnit(loadedUnits[i],
            MultiSignalHandler(this.unloadButtonSignalHandler));

        setWidgetDirectionalFlowStart("UnloadUnitsMenu", grid.getMainWidgetID());
        grid.setupDirectionalFlow();
        setVisibility(true);
    }

    /**
     * Invoked when the menu is closed.
     * @param newMenu Handle to the menu that will be opened after this one.
     */
    void Close(Menu@ const newMenu) {
        setVisibility(false);
        grid.removeUnits();
    }

    /**
     * Called periodically whilst the menu is open.
     * @param ui            Maps from control names to bools, where \c TRUE is
     *                      stored if a control is currently being triggered.
     * @param mouse         Maps from control names to bools, where \c TRUE is
     *                      stored if a control is currently being triggered by
     *                      the mouse.
     * @param previousMouse The position of the mouse during the previous
     *                      iteration of the game loop.
     * @param currentMouse  The position of the mouse right now.
     */
    void Periodic(const dictionary ui, const dictionary mouse,
        const MousePosition&in previousMouse,
        const MousePosition&in currentMouse) {
        // Prevent user from accidentally proceeding with unload if they haven't
        // selected any units to unload yet.
        grid.proceedEnabled(game.map.getUnitPreviewsCount() > 1);

        if (panel.getVisibility()) {
            if (bool(ui["back"])) {
                cancelUnload();
                return;
            }
        } else {
            // Move selection around when selecting a tile for an unload, or
            // cancel the unload operation.
            HandleCommonGameInput(ui, mouse, previousMouse, currentMouse);
            if (bool(ui["select"]) || bool(ui["back"])) {
                // If the select control is being made by the mouse, and it is not
                // inside the map's graphic, then drop it.
                if (bool(mouse["select"]) &&
                    !game.map.getMapBoundingBox().contains(currentMouse)) return;

                if (bool(ui["select"])) {
                    // Select the current tile for the currently unloading unit,
                    // but only if it is an available tile.
                    const auto selectedTile = game.map.getSelectedTile();
                    if (game.map.isAvailableTile(selectedTile)) {
                        // However! If the selected tile has a hidden unit on it,
                        // then prevent the unload of that unit, but force any
                        // unloads in progress to go ahead, and make the base unit
                        // wait.
                        if (game.map.getUnitOnTile(selectedTile) != NO_UNIT) {
                            game.map.popSelectedUnit();
                            proceedWithUnload();
                            return;
                        } else {
                            game.map.addPreviewUnit(currentlyUnloadingUnit,
                                selectedTile);
                            setWidgetText(currentlyUnloadingUnitsUnloadButton,
                                "load");
                        }
                    } else {
                        // Don't cancel the unload operation if it's unavailable!
                        return;
                    }
                }
                game.map.popSelectedUnit();
                panel.setVisibility(true);
                return;
            }
        }
    }

    /**
     * To be called when the unload operation is to proceed.
     */
    private void proceedWithUnload() {
        // Move the base unit as normal.
        game.moveUnit();
        // Set the locations of all the unloaded units.
        const auto loadedUnits = game.map.getLoadedUnits(selectedUnitCache);
        const auto loadedUnitsLength = loadedUnits.length();
        for (uint i = 0; i < loadedUnitsLength; ++i) {
            const auto unitID = loadedUnits[i];
            if (game.map.isPreviewUnit(unitID)) {
                game.map.unloadUnit(unitID, selectedUnitCache,
                    game.map.getUnitPreviewPosition(unitID));
                game.map.waitUnit(unitID, true);
                game.map.removePreviewUnit(unitID);
            }
        }
        // Go back to the game screen, and always ensure the base panel is visible
        // before leaving (if we don't, then when a unit is unloaded onto a hidden
        // unit, the unload menu will be bugged out in the future).
        panel.setVisibility(true);
        setGUI("GameScreen");
    }

    /**
     * To be called when the unload operation is cancelled.
     */
    private void cancelUnload() {
        // Remove all of the preview units that were added by this menu.
        const auto loadedUnits = game.map.getLoadedUnits(selectedUnitCache);
        const auto loadedUnitsLength = loadedUnits.length();
        for (uint i = 0; i < loadedUnitsLength; ++i) {
            const auto loadedUnit = loadedUnits[i];
            if (game.map.isPreviewUnit(loadedUnit))
                game.map.removePreviewUnit(loadedUnit);
        }
        // Force the selection to go back to the originally selected unit (as the
        // currently selected tile could be moved whilst in this menu, and this
        // menu relies on the selected tile being on the original unit as the menu
        // opens).
        game.map.setSelectedTile(selectedTileCache);
        setGUI("PreviewMoveUnitMenu");
    }

    /**
     * Unload a unit.
     * @param w The ID of the Unload button widget.
     * @param s The signal emitted by the button widget.
     */
    private void unloadButtonSignalHandler(const WidgetID w, const string&in s) {
        if (s != MouseReleased) return;
        currentlyUnloadingUnitsUnloadButton = w;
        currentlyUnloadingUnit = parseUnitID(getWidgetName(w));
        if (game.map.isPreviewUnit(currentlyUnloadingUnit)) {
            // Remove the unit from the map.
            game.map.removePreviewUnit(currentlyUnloadingUnit);
            setWidgetText(w, "unload");
        } else {
            panel.setVisibility(false);
            const auto unitMoveType = game.map.getUnitType(
                currentlyUnloadingUnit).movementType;
            // Select unit.
            game.map.pushSelectedUnit(currentlyUnloadingUnit);
            game.map.setAvailableTileShader(AvailableTileShader::Yellow);
            game.newClosedListNode(game.map.closedList, -1, selectedTileCache, 0);
            // Filter adjacent tiles.
            const auto loadedUnits = game.map.getLoadedUnits(selectedUnitCache);
            const auto loadedUnitsLength = loadedUnits.length();
            const auto adjacentTiles =
                game.map.getAvailableTiles(selectedTileCache, 1, 1);
            const auto adjacentTilesLength = adjacentTiles.length();
            for (uint i = 0; i < adjacentTilesLength; ++i) {
                const auto adjacentTile = adjacentTiles[i];
                // Remove tiles that are already occupied by an unload in
                // progress.
                {
                    bool continueLoop = false;
                    for (uint j = 0; j < loadedUnitsLength; ++j) {
                        if (game.map.tileHasPreviewUnit(adjacentTile,
                            loadedUnits[j])) {
                            continueLoop = true;
                            break;
                        }
                    }
                    if (continueLoop) continue;
                }
                // Remove tiles that the unit cannot be unloaded onto.
                if (game.map.findPathForUnloadUnit(selectedTileCache,
                    adjacentTile, unitMoveType,
                    game.map.getArmyOfUnit(selectedUnitCache),
                    { selectedUnitCache }).length() == 0) continue;
                game.map.addAvailableTile(adjacentTile);
            }
        }
    }

    /**
     * Cache of the tile that was selected when the menu was opened.
     */
    private Vector2 selectedTileCache;

    /**
     * Cache the originally selected unit to use for \c findPath calculations.
     */
    private UnitID selectedUnitCache;

    /**
     * The ID of the unit that is currently being unloaded.
     */
    private UnitID currentlyUnloadingUnit;

    /**
     * The ID of the unload button corresponding to the unit that is currently
     * being unloaded.
     */
    private WidgetID currentlyUnloadingUnitsUnloadButton;

    /**
     * The panel in which all of the units available for unload are displayed.
     */
    private ScrollablePanel panel;

    /**
     * The grid of units that can be unloaded.
     */
    private UnloadUnitGrid grid(SingleSignalHandler(this.proceedWithUnload),
        SingleSignalHandler(this.cancelUnload));
}
