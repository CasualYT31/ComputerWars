/**
 * @file ScriptsWindow.as
 * Window allowing you to edit a map's scripts.
 */

/**
 * Represents the window used to edit map scripts.
 */
class ScriptsWindow {
    /**
     * Sets up the scripts window.
     * @param parent The container to add the scripts window to.
     */
    void setUp(const string&in parent) {
        window = parent + ".ScriptsWindow";
        addWidget(ChildWindow, window);
        setWidgetText(window, "scripts");
        close();

        const auto menu = window + ".Menu";
        addWidget(MenuBar, menu);

        addMenu(menu, "file");

        tabContainer = window + ".TabContainer";
        addWidget(TabContainer, tabContainer);
        setWidgetSize(tabContainer, "100%", "100%-2*" +
            formatFloat(getWidgetFullSize(menu).y) + "px");
        setWidgetPosition(tabContainer, "50%", "50%");
        setWidgetOrigin(tabContainer, 0.5f, 0.5f);

        const auto test = addTabAndPanel(tabContainer, "~Script File Name.as");
        setWidgetBackgroundColour(test, Colour(180, 180, 180, 255));

        addWidget(TextArea, test + ".TextArea");
        setWidgetSize(test + ".TextArea", "100%", "100%");
        setHorizontalScrollbarPolicy(test + ".TextArea",
            ScrollbarPolicy::Automatic);

        statusBar.setUp(window, formatFloat(getWidgetFullSize(menu).y));
    }

    /// Opens and/or restores the scripts window to a known location.
    void restore() {
        if (isChildWindowOpen(window)) {
            setWidgetPosition(window, "25px", "25px");
        } else {
            openChildWindow(window, "25px", "25px");
            refresh();
        }
    }

    /// If the window is open, refresh it with up-to-date data.
    void refresh() {
        if (isChildWindowOpen(window)) {
        }
    }

    /// Closes the scripts window.
    void close() {
        closeChildWindow(window);
    }

    /// Caches the name of the \c ChildWindow.
    private string window;

    /// Caches the name of the \c TabContainer.
    private string tabContainer;

    /// The status bar.
    StatusBarWidget statusBar;
}
