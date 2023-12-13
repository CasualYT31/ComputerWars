/**
 * @file PreviewMoveUnitMenu.as
 * The menu that allows the current army to preview a unit's move before comitting
 * to it.
 */

/**
 * The preview unit movement menu.
 */
class PreviewMoveUnitMenu : Menu, Group {
    /**
     * Set up the menu.
     */
    PreviewMoveUnitMenu() {
        add(previewCommands);
        ::add(ROOT_WIDGET, this);
    }

    /**
     * Invoked when the menu is opened.
     * @param oldMenu Handle to the menu that was open before this one.
     */
    void Open(Menu@ const oldMenu) {
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
            previewCommands.addCommand("join", "joinicon", function(){
                game.joinUnits(
                    game.map.getUnitOnTile(game.map.getSelectedTile()),
                    game.map.getSelectedUnit(),
                    game.map.closedList[game.map.closedList.length() - 1].g
                );
                setGUI("GameScreen");
            });
        } else {
            if (game.canLoad(unit, unitOnTile)) {
                previewCommands.addCommand("load", "loadicon", function(){
                    game.loadUnit(
                        game.map.getSelectedUnit(),
                        game.map.getUnitOnTile(game.map.getSelectedTile()),
                        game.map.closedList[game.map.closedList.length() - 1].g
                    );
                    // Deselect unit and remove unit previews!
                    game.selectUnit(NO_UNIT);
                    game.map.removeAllPreviewUnits();
                    setGUI("GameScreen");
                });
            } else {
                if (game.canAttack(unit, tile)) {
                    previewCommands.addCommand("fire", "attackicon", function(){
                        setGUI("SelectTargetMenu");
                    });
                }
                if (game.canCapture(unit, tile)) {
                    previewCommands.addCommand("capture", "captureicon",
                        function(){
                        game.moveUnitAndCapture();
                        setGUI("GameScreen");
                    });
                }
                // Launch Missile Silo.
                if (tileStructure !is null &&
                    tileStructure.scriptName == "MISSILESILO" &&
                    (unitType.scriptName == "INFANTRY" ||
                    unitType.scriptName == "MECH")) {
                    previewCommands.addCommand("launch", "attackicon", function(){
                        cast<ExplodePreviewMenu>(getMenu("ExplodePreviewMenu")).
                            Initialise(3, Vector2(0, 3), true, function(tile){
                                // If the launch is going ahead, then we need to
                                // make the Missile Silo empty. Since it's a
                                // structure, we can destroy it to achieve this.
                                game.map.destroyStructure(tile);
                                // Move the unit that is launching the silo.
                                game.moveUnit();
                            }
                        );
                        setGUI("ExplodePreviewMenu");
                    });
                }
                // Remember to ignore the APC itself when searching for adjacent
                // units: it hasn't moved yet, so the engine still thinks it's at
                // its original position.
                if (unitType.scriptName == "APC" &&
                    game.areThereDepletedArmyUnitsAdjacentTo(tile,
                        game.map.getArmyOfUnit(unit), { unit })) {
                    previewCommands.addCommand("supply", "replenishicon",
                        function(){
                        // Cache selected unit, as moveUnit() unselects it.
                        const auto selectedUnit = game.map.getSelectedUnit();
                        game.moveUnit();
                        game.APCReplenishUnits(selectedUnit);
                        setGUI("GameScreen");
                    });
                } else if (unitType.scriptName == "BLACKBOAT" &&
                    game.areThereDamagedOrDepletedArmyUnitsAdjacentTo(tile,
                        game.map.getArmyOfUnit(unit), { unit })) {
                    previewCommands.addCommand("repair", "repairicon", function(){
                        setGUI("RepairMenu");
                    });
                }
                if (game.canUnload(unit, tile)) {
                    previewCommands.addCommand("unload", "unloadicon", function(){
                        setGUI("UnloadUnitsMenu");
                    });
                }
                if (unitType.canHide) {
                    if (game.map.isUnitHiding(unit)) {
                        if (unitType.scriptName == "SUB") {
                            previewCommands.addCommand("surface", "unhideicon",
                                function(){
                                game.moveUnitHide(false);
                                setGUI("GameScreen");
                            });
                        } else {
                            previewCommands.addCommand("unhide", "unhideicon",
                                function(){
                                game.moveUnitHide(false);
                                setGUI("GameScreen");
                            });
                        }
                    } else {
                        if (unitType.scriptName == "SUB") {
                            previewCommands.addCommand("dive", "hideicon",
                                function(){
                                game.moveUnitHide(true);
                                setGUI("GameScreen");
                            });
                        } else {
                            previewCommands.addCommand("hide", "hideicon",
                                function(){
                                game.moveUnitHide(true);
                                setGUI("GameScreen");
                            });
                        }
                    }
                }
                if (unitType.scriptName == "BLACKBOMB") {
                    previewCommands.addCommand("explode", "attackicon",
                        function(){
                        cast<ExplodePreviewMenu>(getMenu("ExplodePreviewMenu")).
                            Initialise(5, Vector2(1, 3), false, function(tile){
                                // If the explosion is going ahead, then we need
                                // to delete the Black Bomb unit.
                                game.deleteUnit(game.map.getSelectedUnit());
                            }
                        );
                        setGUI("ExplodePreviewMenu");
                    });
                }
                if (unitType.scriptName == "OOZIUM" && unitOnTile != NO_UNIT &&
                    unitOnTile != unit) {
                    previewCommands.addCommand("fire", "attackicon", function(){
                        game.deleteUnit(game.map.getUnitOnTile(
                            game.map.getSelectedTile()));
                        game.moveUnit();
                        setGUI("GameScreen");
                    });
                } else {
                    previewCommands.addCommand("wait", "waiticon", function(){
                        game.moveUnit();
                        setGUI("GameScreen");
                    });
                }
            }
        }

        // Position the command menu differently depending on the quadrant of the
        // screen that the mouse is in.
        const Vector2f commandMenuSize = previewCommands.getFullSize();
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
        previewCommands.setPosition(formatInt(mouse.x + xOffset),
            formatInt(mouse.y + yOffset));
        setVisibility(true);
    }

    /**
     * Invoked when the menu is closed.
     * @param newMenu Handle to the menu that will be opened after this one.
     */
    void Close(Menu@ const newMenu) {
        setVisibility(false);
        previewCommands.removeAllCommands();
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
        if (bool(ui["back"])) {
            game.map.disableSelectedUnitRenderingEffects(false);
            game.map.removePreviewUnit(game.map.getSelectedUnit());
            setGUI("MoveUnitMenu");
            return;
        }
    }

    /**
     * The command menu widget.
     */
    private CommandWidget previewCommands("PreviewMoveUnitMenu", "100");
}
