/**
 * @file SelectTargetMenu.as
 * The menu that allows the current army to select a target when attacking.
 */

/**
 * The unit attacking menu.
 */
class SelectTargetMenu : Menu, Group {
    /**
     * Set up the menu.
     */
    SelectTargetMenu() {
        damageWidget.setVisibility(false);
        add(damageWidget);
        ::add(ROOT_WIDGET, this);
    }

    /**
     * Invoked when the menu is opened.
     * @param oldMenu     Handle to the menu that was open before this one.
     * @param oldMenuName Name of the type of the \c Menu object.
     */
    void Open(Menu@ const oldMenu, const string&in oldMenuName) {
        // When the menu is opened, the currently selected unit will be push
        // selected again, and its available targets will be added to the
        // available tiles list.
        game.enableClosedList(false);
        tilesWithTargets.deleteAll();
        @attackingUnitType = game.map.getUnitType(game.map.getSelectedUnit());
        game.findTilesWithTargets(tilesWithTargets, game.map.getSelectedUnit(),
            game.map.getSelectedTile());
        game.map.pushSelectedUnit(game.map.getSelectedUnit());
        const auto keys = tilesWithTargets.getKeys();
        for (uint i = 0, len = keys.length(); i < len; ++i)
            game.map.addAvailableTile(Vector2(keys[i]));
        game.map.setAvailableTileShader(AvailableTileShader::Red);
        game.map.disableShaderForAvailableUnits(true);
        game.setAttackCursorSprites();
        damageWidget.setVisibility(false);
        setVisibility(true);
    }

    /**
     * Invoked when the menu is closed.
     * @param newMenu     Handle to the menu that will be opened after this one.
     * @param newMenuName Name of the type of the \c Menu object.
     */
    void Close(Menu@ const newMenu, const string&in newMenuName) {
        game.enableClosedList(true);
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

        // Update the damage label widget's position based on the position of the
        // map cursor.
        const auto rect = game.map.getCursorBoundingBox();
        const auto quadrant = game.map.getCursorQuadrant();
        string x, y;
        float originX = 0.0, originY = 0.0;
        switch (quadrant) {
        case Quadrant::LowerLeft:
            x = formatInt(rect.left + rect.width);
            y = formatInt(rect.top);
            originX = 0.0;
            originY = 1.0;
            break;
        case Quadrant::LowerRight:
            x = formatInt(rect.left);
            y = formatInt(rect.top);
            originX = 1.0;
            originY = 1.0;
            break;
        case Quadrant::UpperRight:
            x = formatInt(rect.left);
            y = formatInt(rect.top + rect.height);
            originX = 1.0;
            originY = 0.0;
            break;
        default: // Quadrant::UpperLeft:
            x = formatInt(rect.left + rect.width);
            y = formatInt(rect.top + rect.height);
            originX = 0.0;
            originY = 0.0;
        }
        damageWidget.setPosition(x, y);
        damageWidget.setOrigin(originX, originY);


        // If the currently selected tile is hovering over a target, find the base
        // damage the selected weapon deals to the target and update the damage
        // label.
        const auto selectedTile = game.map.getSelectedTile();
        if (game.map.isAvailableTile(selectedTile)) {
            string weaponName;
            tilesWithTargets.get(selectedTile.toString(), weaponName);
            int baseDamage = game.calculateDamage(game.map.getSelectedUnit(),
                weaponName, selectedTile, true);
            damageWidget.setDamage(baseDamage);
            damageWidget.setVisibility(true);
        } else {
            damageWidget.setVisibility(false);
        }

        if (bool(ui["back"])) {
            game.map.popSelectedUnit();
            setGUI("PreviewMoveUnitMenu");
        } else if (bool(ui["select"])) {
            // If the select control is being made by the mouse, and it is not
            // inside the map's graphic, then drop it.
            if (bool(mouse["select"]) &&
                !game.map.getMapBoundingBox().contains(currentMouse)) return;

            // Ignore the selection if a non-available tile was selected.
            if (game.map.isAvailableTile(selectedTile)) {
                game.map.popSelectedUnit();
                // Perform move operation.
                const auto attackingUnit = game.map.getSelectedUnit();
                game.moveUnit();
                // Perform attack.
                game.attack(tilesWithTargets, attackingUnit, selectedTile);
                setGUI("GameScreen");
            }
        }
    }

    /**
     * The damage label widget.
     */
    private DamageWidget damageWidget;

    /**
     * A set of tiles that contain targets.
     * @sa \c PlayableMap::findTilesWithTargets().
     */
    private dictionary tilesWithTargets;

    /**
     * Points to the type of the attacking unit.
     */
    private const UnitType@ attackingUnitType;
}
