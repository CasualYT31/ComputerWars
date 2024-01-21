/**
 * @file DevCommandMenu.as
 * Contains the menu which allows the developer to run code on-the-fly.
 */

/**
 * The main map menu.
 */
class DevCommandMenu : Menu, Group {
    /**
     * Set up the menu.
     */
    DevCommandMenu() {
        // Setup the edit box.
        code.setName("DevCommandMenuCode");
        code.setSize("100%", "30");
        code.setOrigin(0.0, 1.0);
        code.setPosition("0%", "100%");
        code.setTextSize(22);
        code.connect(ReturnKeyPressed,
            SingleSignalHandler(this.codeReturnKeyPressed));
        add(code);

        // Setup the back button.
        back.setText("back");
        back.setSize("12.5%", "30");
        back.setOrigin(1.0, 1.0);
        back.setPosition("100%", "DevCommandMenuCode.top");
        back.setTextSize(22);
        back.connect(Clicked, function(){ setGUI("GameScreen"); });
        add(back);

        // Setup the error label.
        error.setTextColour(Colour(128, 0, 0, 255));
        error.setTextOutlineColour(White);
        error.setTextOutlineThickness(2.0);
        error.setOrigin(0.0, 1.0);
        error.setPosition("0%", "DevCommandMenuCode.top");
        error.setSize("87.5%", "30");
        error.setTextSize(16);
        add(error);

        // Setup the info label.
        info.setTextColour(Black);
        info.setTextOutlineColour(White);
        info.setTextOutlineThickness(2.0);
        info.setTextSize(32);
        info.setPosition("2%", "2%");
        add(info);

        // Setup the root group.
        ::add(ROOT_WIDGET, this);
    }

    /**
     * Invoked when the menu is opened.
     * @param oldMenu     Handle to the menu that was open before this one.
     * @param oldMenuName Name of the type of the \c Menu object.
     */
    void Open(Menu@ const oldMenu, const string&in oldMenuName) {
        const auto tile = game.map.getSelectedTile();
        const auto unit = game.map.getUnitOnTile(tile);
        string msg = "~Selected Tile: " + tile.toString() + "\n"
            "Selected Unit ID: " + formatUInt(game.map.getUnitOnTile(tile));
        if (unit != NO_UNIT) {
            const auto loadedUnits = game.map.getLoadedUnits(unit);
            for (uint i = 0, len = loadedUnits.length(); i < len; ++i) {
                msg += "\nLoaded Unit #" + formatUInt(i + 1) + ": ID " +
                    formatUInt(loadedUnits[i]) + " Type " +
                    game.map.getUnitType(loadedUnits[i]).scriptName;
            }
        }
        info.setText(msg);
        // Parent container must be made visible first before EditBox can properly
        // receive focus.
        setVisibility(true);
        code.setFocus();
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
        const MousePosition&in currentMouse) {}
    
    /**
     * When the user hits enter whilst their focus is on the edit box, run code.
     */
    private void codeReturnKeyPressed() {
        auto result = execute(code.getText());
        error.setText("~" + result);
        if (result.isEmpty()) {
            cast<GameScreen>(getMenu("GameScreen")).cancelNextDevInput();
            setGUI("GameScreen");
        }
    }

    /**
     * The edit box in which the user writes code.
     */
    private EditBox code;

    /**
     * The back button.
     */
    private Button back;

    /**
     * When an error occurs, it will be written to this label.
     */
    private Label error;

    /**
     * Information on the current tile will be written to this label.
     */
    private Label info;
}
