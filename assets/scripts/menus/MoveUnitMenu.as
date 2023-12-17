/**
 * @file MoveUnitMenu.as
 * The menu that allows the current army to move one of its units.
 */

/**
 * The unit movement menu.
 */
class MoveUnitMenu : Menu, Group {
    /**
     * Set up the menu.
     */
    MoveUnitMenu() {
        ::add(ROOT_WIDGET, this);
    }

    /**
     * Invoked when the menu is opened.
     * @param oldMenu Handle to the menu that was open before this one.
     */
    void Open(Menu@ const oldMenu) {
        setVisibility(true);
    }

    /**
     * Invoked when the menu is closed.
     * @param newMenu Handle to the menu that will be opened after this one.
     */
    void Close(Menu@ const newMenu) {
        setVisibility(false);
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
        HandleCommonGameInput(ui, mouse, previousMouse, currentMouse);
        if (bool(ui["back"])) {
            game.selectUnit(NO_UNIT);
            setGUI("GameScreen");
            return;
        } else if (bool(ui["select"])) {
            // If the select control is being made by the mouse, and it is not
            // inside the map's graphic, then drop it.
            if (bool(mouse["select"]) &&
                !game.map.getMapBoundingBox().contains(currentMouse)) return;

            // Only allow selection if an available tile was selected.
            const auto selectedTile = game.map.getSelectedTile();
            const auto closedListLength = game.map.closedList.length();
            for (uint i = 0; i < closedListLength; ++i) {
                if (game.map.closedList[i].tile == selectedTile) {
                    const auto unitID = game.map.getSelectedUnit();
                    const auto otherUnit = game.map.getUnitOnTile(selectedTile);
                    if (otherUnit == NO_UNIT || otherUnit == unitID ||
                        game.canJoin(otherUnit, unitID) ||
                        game.canLoad(unitID, otherUnit) ||
                        !game.map.isUnitVisible(otherUnit,
                            game.map.getArmyOfUnit(unitID)) ||
                        game.canOoziumMove(unitID, selectedTile)) {
                        const auto obstructionIndex =
                            game.map.scanPath(game.map.closedList, unitID);
                        if (obstructionIndex >= 0) {
                            // Trim closed list and move unit.
                            game.map.closedList.removeRange(obstructionIndex,
                                game.map.closedList.length() - obstructionIndex);
                            game.moveUnit();
                            setGUI("GameScreen");
                        } else {
                            game.map.disableSelectedUnitRenderingEffects(true);
                            game.map.addPreviewUnit(unitID, selectedTile);
                            setGUI("PreviewMoveUnitMenu");
                        }
                        return;
                    }
                }
            }
        }
    }
}