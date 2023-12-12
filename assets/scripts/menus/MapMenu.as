/**
 * @file MapMenu.as
 * The main command menu shown when selecting a vacant tile.
 */

/**
 * The main map menu.
 */
class MapMenu : Menu, Group {
    /**
     * Set up the menu.
     */
    MapMenu() {
        // Setup the root group.
        mapMenuCommands.setPosition("50%", "10px");
        mapMenuCommands.setOrigin(0.5, 0.0);
        add(mapMenuCommands);
        ::add(ROOT_WIDGET, this);
    }

    /**
     * Invoked when the menu is opened.
     * @param oldMenu Handle to the menu that was open before this one.
     */
    void Open(Menu@ const oldMenu) {
        mapMenuCommands.addCommand("save", "saveicon", function(){
            game.map.save();
            setGUI("GameScreen");
        });
        mapMenuCommands.addCommand("delete", "deleteicon", function(){
            setGUI("DeleteUnitMenu");
        });
        mapMenuCommands.addCommand("yield", "yieldicon", function(){
            game.endTurn(true);
            setGUI("GameScreen");
        });
        if (game.map.tagCOIsPresent(game.map.getSelectedArmy())) {
            mapMenuCommands.addCommand("tag", "tagicon", function(){
                game.tagCOs();
                setGUI("GameScreen");
            });
        }
        mapMenuCommands.addCommand("endturn", "endturnicon", function(){
            game.endTurn();
            setGUI("GameScreen");
        });
        mapMenuCommands.addCommand("quit", "quiticon", function(){
            quitMap();
        });
        setVisibility(true);
    }

    /**
     * Invoked when the menu is closed.
     * @param newMenu Handle to the menu that will be opened after this one.
     */
    void Close(Menu@ const newMenu) {
        setVisibility(false);
        mapMenuCommands.removeAllCommands();
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
        if (bool(ui["back"])) setGUI("GameScreen");
    }

    /**
     * The command menu widget.
     */
    private CommandWidget mapMenuCommands("MapMenu");
}
