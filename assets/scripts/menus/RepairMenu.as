/**
 * @file RepairMenu.as
 * The menu that allows the current army to select a unit to repair.
 */

/**
 * The unit repair menu.
 */
class RepairMenu : Menu, Group {
    /**
     * Set up the menu.
     */
    RepairMenu() {
        ::add(ROOT_WIDGET, this);
    }

    /**
     * Invoked when the menu is opened.
     * @param oldMenu     Handle to the menu that was open before this one.
     * @param oldMenuName Name of the type of the \c Menu object.
     */
    void Open(Menu@ const oldMenu, const string&in oldMenuName) {
        // When this menu is opened, push select the current unit (Black Boat),
        // and add the available units to the available tile list. Also disable
        // the closed list.
        game.enableClosedList(false);
        const auto selectedUnit = game.map.getSelectedUnit();
        game.map.pushSelectedUnit(selectedUnit);
        game.map.setAvailableTileShader(AvailableTileShader::Yellow);
        game.map.disableShaderForAvailableUnits(true);
        const auto availableUnits = game.findDamagedOrDepletedArmyUnitsAdjacentTo(
            game.map.getSelectedTile(), game.map.getArmyOfUnit(selectedUnit),
            { selectedUnit });
        for (uint i = 0, len = availableUnits.length(); i < len; ++i)
            game.map.addAvailableTile(
                game.map.getUnitPosition(availableUnits[i]));
        setVisibility(true);
    }

    /**
     * Invoked when the menu is closed.
     * @param newMenu     Handle to the menu that will be opened after this one.
     * @param newMenuName Name of the type of the \c Menu object.
     */
    void Close(Menu@ const newMenu, const string&in newMenuName) {
        game.enableClosedList(true);
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
            game.map.popSelectedUnit();
            game.map.queuePlay("sound", "back");
            setGUI("PreviewMoveUnitMenu");
        } else if (bool(ui["select"])) {
            // If the select control is being made by the mouse, and it is not
            // inside the map's graphic, then drop it.
            if (bool(mouse["select"]) &&
                !game.map.getMapBoundingBox().contains(currentMouse)) return;

            if (game.map.isAvailableTile(game.map.getSelectedTile())) {
                // Move black boat unit.
                const auto blackBoatsArmy =
                    game.map.getArmyOfUnit(game.map.getSelectedUnit());
                game.map.popSelectedUnit();
                game.moveUnit();
                // Perform repair and replenish.
                const auto unitToRepair =
                    game.map.getUnitOnTile(game.map.getSelectedTile());
                game.animateRepair(unitToRepair);
                game.map.replenishUnit(unitToRepair);
                game.healUnit(unitToRepair, 1, blackBoatsArmy);
                // Go back to the game screen.
                setGUI("GameScreen");
            }
        }
    }
}
