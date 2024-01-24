/**
 * @file GameScreen.as
 * The root menu of gameplay.
 */

/**
 * Input handling code common to both Map and MoveUnitMenu.
 * @param  controls         The control map given by the engine.
 * @param  mouseInputs      Stores which controls are being triggered by the
 *                          mouse.
 * @param  previousPosition The previous mouse position.
 * @param  currentPosition  The current mouse position.
 * @return \c TRUE if this function updated the selected tile, \c FALSE if not.
 */
bool HandleCommonGameInput(const dictionary@ controls,
    const dictionary@ mouseInputs, const MousePosition&in previousPosition,
    const MousePosition&in currentPosition) {
    bool ret = false;

    // Handle mouse input. Ignore the mouse if the game doesn't have focus.
    if (currentPosition != INVALID_MOUSE) {
        // Ignore the mouse if it's outside of the window.
        Vector2 windowSize = getWindowSize();
        if (currentPosition.x >= 0 && currentPosition.y >= 0
            && currentPosition.x <= int(windowSize.x)
            && currentPosition.y <= int(windowSize.y)) {
            // Only consider the mouse if it has moved.
            if (currentPosition != previousPosition) {
                game.setSelectedTileByPixel(currentPosition);
                ret = true;
            }
        }
    }

    // Handle controls.
    if (bool(controls["up"])) {
        game.moveSelectedTileUp();
        ret = true;
    } else if (bool(controls["down"])) {
        game.moveSelectedTileDown();
        ret = true;
    } else if (bool(controls["left"])) {
        game.moveSelectedTileLeft();
        ret = true;
    } else if (bool(controls["right"])) {
        game.moveSelectedTileRight();
        ret = true;
    }
    if (bool(controls["zoomout"])) {
        game.zoomOut();
    }
    if (bool(controls["zoomin"])) {
        game.zoomIn();
    }

    // Make sure this stays at the bottom.
    if (bool(controls["info"])) {
        setGUI("DetailedInfoMenu");
    }
    return ret;
}

/**
 * The root menu of gameplay.
 */
class GameScreen : Menu, Group {
    /**
     * Set up the menu.
     */
    GameScreen() {
        armyGroup.setSize("50%", "");
        armyGroup.add(armyWidget);
        add(tileWidget);
        add(armyGroup);
        ::add(ROOT_WIDGET, this);
        hideWidgets();
    }

    /**
     * Invoked when the menu is opened.
     * @param oldMenu     Handle to the menu that was open before this one.
     * @param oldMenuName Name of the type of the \c Menu object.
     */
    void Open(Menu@ const oldMenu, const string&in oldMenuName) {
        if (!mapPath.isEmpty()) {
            @game = PlayableMap(loadMap(mapPath, PLAYABLE_MAP_TYPENAME));
            if (game.map is null)
                error("Failed to load map, game will crash soon!");
            mapPath = "";
        }
        setVisibility(true);
    }

    /**
     * Invoked when the menu is closed.
     * @param newMenu     Handle to the menu that will be opened after this one.
     * @param newMenuName Name of the type of the \c Menu object.
     */
    void Close(Menu@ const newMenu, const string&in newMenuName) {
        setVisibility(false);
        hideWidgets();
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
        showWidgets();

        // If the dev key was hit, open the debug menu.
        if (bool(ui["dev"])) {
            if (cancelDevInput) {
                cancelDevInput = false;
            } else {
                setGUI("DevCommandMenu");
                return;
            }
        }

        HandleCommonGameInput(ui, mouse, previousMouse, currentMouse);
        const auto cursorOnLeft = game.map.isCursorOnLeftSide();
        
        // Update army widget.
        armyWidget.update(game.map.getOverriddenSelectedArmy());
        if (!cursorOnLeft) {
            armyWidget.setAlignment(ArmyWidgetAlignment::Left);
            armyWidget.setOrigin(0.0, 0.0);
        } else {
            armyWidget.setAlignment(ArmyWidgetAlignment::Right);
            armyWidget.setOrigin(1.0, 0.0);
        }

        // Update tile widget.
        const auto selectedTile = game.map.getSelectedTile();
        tileWidget.update(selectedTile);
        if (!cursorOnLeft) {
            tileWidget.setAlignment(TileWidgetAlignment::Left);
            tileWidget.setOrigin(0.0, 1.0);
            tileWidget.setPosition("0%", "100%");
        } else {
            tileWidget.setAlignment(TileWidgetAlignment::Right);
            tileWidget.setOrigin(1.0, 1.0);
            tileWidget.setPosition("100%", "100%");
        }

        // Update the army and tile widget sizes and positions.
        auto scale = armyGroup.getFullSize().x / ArmyWidgetConstants::Width;
        if (scale > 1.0) scale = 1.0;
        armyWidget.setScale(scale, scale);
        tileWidget.setScale(scale, scale);
        if (cursorOnLeft) {
            armyGroup.setAutoLayout(AutoLayout::Right);
            armyWidget.setPosition("100%+" +
                formatFloat(ArmyWidgetConstants::Radius * scale) + "+" +
                formatFloat(ArmyWidgetConstants::BorderSize * scale),
                "0%-" + formatFloat(ArmyWidgetConstants::BorderSize * scale));
        } else {
            armyGroup.setAutoLayout(AutoLayout::Left);
            armyWidget.setPosition("0%-" +
                formatFloat(ArmyWidgetConstants::Radius * scale) + "-" +
                formatFloat(ArmyWidgetConstants::BorderSize * scale),
                "0%-" + formatFloat(ArmyWidgetConstants::BorderSize * scale));
        }

        // If the user is holding the correct control, calculate the attack range
        // of the unit on the currently selected tile and display it. Otherwise,
        // allow a unit to be selected.
        const auto currentArmy = game.map.getSelectedArmy();
        const auto unitID = game.map.getUnitOnTile(selectedTile);
        game.showAttackRangeOfUnit(NO_UNIT);
        if (bool(ui["range"])) {
            game.enableClosedList(false);
            if (unitID != NO_UNIT &&
                game.map.isUnitVisible(unitID, currentArmy)) {
                game.showAttackRangeOfUnit(unitID);
                if (bool(ui["rangesound"]))
                    game.map.queuePlay("sound", "select.unit");
            } else if (game.showAttackRangeOfTile(selectedTile) &&
                bool(ui["rangesound"]))
                game.map.queuePlay("sound", "select.unit");
        } else if (bool(ui["select"])) {
            // If the select control is being made by the mouse, and it is not
            // inside the map's graphic, then drop it.
            if (bool(mouse["select"]) &&
                !game.map.getMapBoundingBox().contains(currentMouse)) return;

            if (unitID == NO_UNIT) {
                game.map.queuePlay("sound", "select");
                if (!game.map.isOutOfBounds(selectedTile)) {
                    ArmyID owner = game.map.getTileOwner(selectedTile);
                    string type =
                        game.map.getTileType(selectedTile).type.scriptName;
                    if (owner == currentArmy) {
                        if (type == "BASE" || type == "AIRPORT" ||
                            type == "PORT") {
                            setGUI("BaseMenu");
                            return;
                        }
                    }
                }
                setGUI("MapMenu");
                return;
            } else if (game.map.isUnitWaiting(unitID) ||
                !game.map.isUnitVisible(unitID, currentArmy)) {
                game.map.queuePlay("sound", "select");
                setGUI("MapMenu");
                return;
            } else if (game.map.getArmyOfUnit(unitID) ==
                game.map.getSelectedArmy() && !game.map.isUnitWaiting(unitID)) {
                game.selectUnit(unitID);
                game.map.queuePlay("sound", "select.unit");
                game.enableClosedList(true);
                game.map.removeAllPreviewUnits();
                setGUI("MoveUnitMenu");
                return;
            }
        }
    }

    /**
     * Work-around for an issue where pressing enter on code that doesn't throw an
     * exception causes the DevCommandMenu to reopen immediately.
     */
    void cancelNextDevInput() {
        cancelDevInput = true;
    }

    /**
     * Shows the army and tile widgets.
     */
    void showWidgets(any@ const _ = null) {
        armyWidget.setVisibility(true);
        tileWidget.setVisibility(true);
    }

    /**
     * Hides the army and tile widgets.
     */
    void hideWidgets() {
        armyWidget.setVisibility(false);
        tileWidget.setVisibility(false);
    }

    /**
     * To be called before opening this menu when you want to opena map and use it
     * to initialise \c game.
     */
    void loadThisMapWhenNextOpened(const string&in path) {
        mapPath = path;
    }

    /**
     * Group used to scale down the army and tile widgets if the army widget won't
     * fit in it.
     */
    private Group armyGroup;

    /**
     * Displays information on the current army.
     */
    private ArmyWidget armyWidget(128);

    /**
     * Displays information on the current tile.
     */
    private TileWidget tileWidget;

    /**
     * Used to make sure that the enter key doesn't trigger the dev command menu
     * to reappear immediately after sending a command.
     */
    private bool cancelDevInput = false;

    /**
     * The map to load when this menu is next opened.
     */
    private string mapPath;
}
