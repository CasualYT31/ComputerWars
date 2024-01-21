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
     * @param oldMenu     Handle to the menu that was open before this one.
     * @param oldMenuName Name of the type of the \c Menu object.
     */
    void Open(Menu@ const oldMenu, const string&in oldMenuName) {
        mapMenuCommands.addCommand("save", "saveicon", function(){
            game.map.save();
            game.map.queuePlay("sound", "save");
            setGUI("GameScreen");
        }, "icon", "savestart");
        mapMenuCommands.addCommand("delete", "deleteicon", function(){
            setGUI("DeleteUnitMenu");
        });
        mapMenuCommands.addCommand("yield", "yieldicon", function(){
            auto gameScreen = cast<GameScreen>(getMenu("GameScreen"));
            gameScreen.hideWidgets();
            game.deleteArmy(game.map.getSelectedArmy());
            game.map.queueCode(AnimationCode(gameScreen.showWidgets));
            setGUI("GameScreen");
        });
        visualIndex = mapMenuCommands.addCommand("", "visualicon",
            SingleSignalHandler(this.selectNextPreset));
        updateTextOfVisualCommand(getSelectedMapAnimationPreset());
        if (game.map.tagCOIsPresent(game.map.getSelectedArmy())) {
            mapMenuCommands.addCommand("tag", "tagicon", function(){
                auto gameScreen = cast<GameScreen>(getMenu("GameScreen"));
                gameScreen.hideWidgets();
                game.tagCOs();
                game.map.queueCode(AnimationCode(gameScreen.showWidgets));
                setGUI("GameScreen");
            });
        }
        mapMenuCommands.addCommand("endturn", "endturnicon", function(){
            auto gameScreen = cast<GameScreen>(getMenu("GameScreen"));
            gameScreen.hideWidgets();
            game.endTurn();
            game.map.queueCode(AnimationCode(gameScreen.showWidgets));
            setGUI("GameScreen");
        });
        mapMenuCommands.addCommand("quit", "quiticon", function(){
            quitMap("MainMenu");
        });
        setVisibility(true);
    }

    /**
     * Invoked when the menu is closed.
     * @param newMenu     Handle to the menu that will be opened after this one.
     * @param newMenuName Name of the type of the \c Menu object.
     */
    void Close(Menu@ const newMenu, const string&in newMenuName) {
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
        if (bool(ui["back"])) {
            game.map.queuePlay("sound", "back");
            setGUI("GameScreen");
        }
    }

    /**
     * Selects the next animation preset in the internal list.
     */
    private void selectNextPreset() {
        updateTextOfVisualCommand(selectNextMapAnimationPreset());
        saveGlobalSettings();
    }

    /**
     * Sets the visual command button's text to the given preset's name.
     * @param preset The preset to display the name of.
     */
    private void updateTextOfVisualCommand(const AnimationPreset preset) {
        mapMenuCommands.setCommandText(visualIndex, presetTexts[uint(preset)]);
    }

    /**
     * The command menu widget.
     */
    private CommandWidget mapMenuCommands("MapMenu", "50%");

    /**
     * The index of the Visual command.
     */
    private uint visualIndex = 0;

    /**
     * Maps \c AnimationPreset values to their corresponding names.
     */
    private array<string> presetTexts = {
        "visuala",
        "visualb",
        "visualc",
        "visuald",
        "novisual",
        "debugvisual"
    };
}
