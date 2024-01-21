/**
 * @file MainMenu.as
 * Code that handles the main menu.
 */

/**
 * Represents the main menu.
 * This version of the main menu simply lists the map files currently stored in
 * the assets folder.
 */
class MainMenu : Menu, Group {
    /**
     * Set up the menu.
     * Also sets up the root widget and performs other one-time setup tasks.
     */
    MainMenu() {
        // Perform GUI initialisation.
        setWidgetInheritedFont(ROOT_WIDGET, "AW2");
        setDirectionalFlowAngleBracketSprite("UL", "icon", "ulanglebracket");
        setDirectionalFlowAngleBracketSprite("UR", "icon", "uranglebracket");
        setDirectionalFlowAngleBracketSprite("LL", "icon", "llanglebracket");
        setDirectionalFlowAngleBracketSprite("LR", "icon", "lranglebracket");

        // Setup the listbox.
        mapFiles.setSize("50%", "50%");
        mapFiles.setPosition("25%", "50%");
        mapFiles.setOrigin(0.25, 0.5);
        mapFiles.setDirectionalFlow(openMapMaker, openMapMaker, openMapMaker,
            openMapMaker);
        mapFiles.connect(MouseReleased,
            SingleSignalHandler(this.mapFilesMouseReleased));
        add(mapFiles);

        // Setup the map maker button.
        openMapMaker.setPosition("90%", "10%");
        openMapMaker.setOrigin(1.0, 0.0);
        openMapMaker.setText("~Map Maker");
        openMapMaker.setDirectionalFlow(mapFiles, mapFiles, mapFiles, mapFiles);
        openMapMaker.connect(Clicked,
            // function(){ transitionToGUI("MapMaker"); });
            function(){ setGUI("MapMaker"); });
        add(openMapMaker);

        // Setup the root group.
        setWidgetDirectionalFlowStart("MainMenu", mapFiles);
        ::add(ROOT_WIDGET, this);
    }

    /**
     * Invoked when the menu is opened.
     * @param oldMenu     Handle to the menu that was open before this one.
     * @param oldMenuName Name of the type of the \c Menu object.
     */
    void Open(Menu@ const oldMenu, const string&in oldMenuName) {
        // Collect a list of available map files and add them to the listbox.
        mapFiles.clearItems();
        filesystem cwd;
        cwd.changeCurrentPath("./map");
        array<string> files = cwd.getFiles();
        // Only look for *.cwm files!
        for (uint f = 0; f < files.length(); ++f)
            if (files[f].findLast(".") >= 0)
                if (files[f].substr(files[f].findLast(".")) == ".cwm")
                    mapFiles.addItem("~" + files[f]);
        setVisibility(true);
        play("music", "mainmenu", 0.0);
    }

    /**
     * Invoked when the menu is closed.
     * @param newMenu     Handle to the menu that will be opened after this one.
     * @param newMenuName Name of the type of the \c Menu object.
     */
    void Close(Menu@ const newMenu, const string&in newMenuName) {
        setVisibility(false);
        stop("music", 0.0);
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
     * When the user selects a file, open the game screen after setting the map
     * file's path to it.
     */
    private void mapFilesMouseReleased() {
        cast<GameScreen>(getMenu("GameScreen")).loadThisMapWhenNextOpened(
            "map/" + mapFiles.getSelectedItemText());
        // transitionToGUI("GameScreen");
        setGUI("GameScreen");
    }

    /**
     * The listbox that lists all the available map files.
     */
    private ListBox mapFiles;

    /**
     * Opens the map maker menu.
     */
    private Button openMapMaker;
}
