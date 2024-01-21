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
     * @param oldMenu     Handle to the menu that was open before this one.
     * @param oldMenuName Name of the type of the \c Menu object.
     */
    void Open(Menu@ const oldMenu, const string&in oldMenuName) {
        setVisibility(true);
    }

    /**
     * Invoked when the menu is closed.
     * @param newMenu     Handle to the menu that will be opened after this one.
     * @param newMenuName Name of the type of the \c Menu object.
     */
    void Close(Menu@ const newMenu, const string&in newMenuName) {
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
            game.map.queuePlay("sound", "back");
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
                        // Set the moving unit's preview location now. If there is a
                        // trap, it will be removed using game.moveUnit().
                        game.map.addPreviewUnit(unitID, selectedTile);
                        const auto obstructionIndex =
                            game.map.scanPath(game.map.closedList, unitID);
                        if (obstructionIndex >= 0) {
                            // Trim closed list and move unit.
                            const auto trapUnit = game.map.getUnitOnTile(
                                game.map.closedList[obstructionIndex].tile);
                            hideTrapUnit(trapUnit);
                            game.map.closedList.removeRange(obstructionIndex,
                                game.map.closedList.length() - obstructionIndex);
                            game.map.animateMoveUnit(unitID, game.map.closedList);
                            game.moveUnit();
                            game.map.queueCode(AnimationCode(this.showTrapUnit),
                                any(trapUnit));
                            game.animateTrap(unitID);
                            game.map.queueCode(function(_) {
                                setGUI("GameScreen");
                            });
                        } else {
                            game.map.disableSelectedUnitRenderingEffects(true);
                            if (game.map.closedList.length() >= 2)
                                game.map.animateMoveUnit(unitID,
                                    game.map.closedList);
                            game.map.queueCode(function(_) {
                                setGUI("PreviewMoveUnitMenu");
                            });
                        }
                        return;
                    }
                }
            }
        }
    }

    /**
     * Hides the trap unit until the move unit animation has completed.
     * @param trapUnit The ID of the trap unit.
     */
    private void hideTrapUnit(const UnitID trapUnit) {
        game.map.addPreviewUnit(trapUnit, NO_POSITION);
    }

    /**
     * Shows the trap unit.
     * @param unitID Must contain the ID of the trap unit.
     */
    private void showTrapUnit(any@ const unitID) {
        UnitID trapUnit = NO_UNIT;
        if (unitID.retrieve(trapUnit)) game.map.removePreviewUnit(trapUnit);
    }
}
