/**
 * @file DeleteUnitMenu.as
 * The menu that allows the current army to delete its units.
 */

/**
 * The unit deletion menu.
 */
class DeleteUnitMenu : Menu, Group {
    /**
     * Set up the menu.
     */
    DeleteUnitMenu() {
        ::add(ROOT_WIDGET, this);
    }

    /**
     * Invoked when the menu is opened.
     * @param oldMenu Handle to the menu that was open before this one.
     */
    void Open(Menu@ const oldMenu) {
        setVisibility(true);
        game.setDeleteCursorSprites();
    }

    /**
     * Invoked when the menu is closed.
     * @param newMenu Handle to the menu that will be opened after this one.
     */
    void Close(Menu@ const newMenu) {
        game.setNormalCursorSprites();
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
        auto unitID = game.map.getUnitOnTile(game.map.getSelectedTile());
        if (bool(ui["select"]) && unitID != NO_UNIT &&
            game.map.getArmyOfUnit(unitID) == game.map.getSelectedArmy()) {
            // If the select control is being made by the mouse, and it is not
            // inside the map's graphic, then drop it.
            if (bool(mouse["select"]) &&
                !game.map.getMapBoundingBox().contains(currentMouse)) return;

            if (game.deleteUnit(unitID)) {
                setGUI("GameScreen");
            }
        } else if (bool(ui["back"])) {
            setGUI("GameScreen");
        }
    }
}
