/**
 * @file GameScreen.as
 * The root menu of gameplay.
 */

/**
 * The root menu of gameplay.
 */
class GameScreen : Menu, Group {
    /**
     * Set up the menu.
     */
    GameScreen() {
        // Setup the root group.
        ::add(ROOT_WIDGET, this);
    }

    /**
     * Invoked when the menu is opened.
     * @param oldMenu Handle to the menu that was open before this one.
     */
    void Open(Menu@ const oldMenu) {
        // This menu can only be opened from the main menu.
        @game = cast<MainMenu>(oldMenu).getGameMap();
        setVisibility(true);
    }

    /**
     * Invoked when the menu is closed.
     * @param newMenu Handle to the menu that will be opened after this one.
     */
    void Close(Menu@ const newMenu) {
        setVisibility(false);
        @game = null;
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
    }

    /**
     * Handle to the map that's currently being played.
     */
    private PlayableMap@ game;
}
