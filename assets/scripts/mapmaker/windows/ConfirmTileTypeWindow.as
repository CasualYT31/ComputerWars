/**
 * @file ConfirmTileTypeWindow.as
 * Window used to confirm whether an operation should be carried out with the
 * currently selected tile type.
 */

/**
 * Represents the \c ConfirmTileTypeWindow.
 */
class ConfirmTileTypeWindow {
    /**
     * Sets up the \c ConfirmTileTypeWindow.
     * @param name               The name to give to the \c ChildWindow.
     * @param beforeTileSelected The text to display in the tile type label before
     *                           a tile type has been selected.
     * @param afterTileSelected  The text to display in the tile type label after
     *                           a tile type has been selected.
     * @param yes                The signal handler to connect to the \c Yes
     *                           button.
     * @param no                 The signal handler to connect to the \c No
     *                           button.
     */
    void setUp(const string&in name, const string&in beforeTileSelected,
        const string&in afterTileSelected, SignalHandler@ const yes,
        SignalHandler@ const no) {
        // ChildWindow.
        window = name;
        addWidget("ChildWindow", window);
        setWidgetText(window, "alert");
        setWidgetSize(window, "250px", "175px");
        setWidgetResizable(window, false);
        setChildWindowTitleButtons(window, TitleButton::None);
        closeChildWindow(window);
    
        // Base layout.
        const auto baseLayout = window + ".BaseLayout";
        addWidget("VerticalLayout", baseLayout);
        setWidgetSize(baseLayout, "100%", "100%-30px");
        setGroupPadding(baseLayout, "5px");

        // The main label.
        const auto textGroup = baseLayout + ".TextGroup";
        addWidget("Group", textGroup);

        text = textGroup + ".Text";
        addWidget("Label", text);
        setWidgetSize(text, "100%", "100%");
        setWidgetTextMaximumWidth(text, getWidgetFullSize(text).x);

        // The CurrentlySelectedTileType widget and label.
        const auto tileLayout = baseLayout + ".TileLayout";
        addWidget("VerticalLayout", tileLayout);

        const auto tileTextGroup = tileLayout + ".TextGroup";
        addWidget("Group", tileTextGroup);

        tileText = tileTextGroup + ".Text";
        addWidget("Label", tileText);
        setWidgetText(tileText, beforeTileSelected);
        tileTextAfterTileSelected = afterTileSelected;

        const auto tileGroup = tileLayout + ".TileGroup";
        addWidget("Group", tileGroup);

        CurrentlySelectedTileType.addWidget(tileGroup + ".CurSelTileLayout",
            CurrentlySelectedObject::Callback(this.enableYesButton));

        // Yes and No buttons.
        awe::addButtonsToParent(window, {
            awe::ParentButton("ConfirmTileTypeYes", "yes", yes),
            awe::ParentButton("ConfirmTileTypeNo", "no", no)
        }, 50, 25);
        yesButton = window + ".ConfirmTileTypeYes";
        setWidgetEnabled(yesButton, false);
    }

    /// Ensure the window is open and visible.
    /// @param mainText The text to assign to the main \c Label. Leave blank to
    ///                 not change the text.
    void restore(const string&in mainText = "") {
        if (!mainText.isEmpty()) setWidgetText(text, mainText);
        openChildWindow(window, "50px", "35px");
    }

    /// Closes the window.
    void close() { closeChildWindow(window); }

    /// Make sure to enable the Yes button when a tile type is selected.
    private void enableYesButton() {
        setWidgetEnabled(yesButton, true);
        setWidgetText(tileText, tileTextAfterTileSelected);
    }

    /// The name of the \c ChildWindow.
    private string window;
    string windowName { get const { return window; } }

    /// The name of the main \c Label.
    private string text;

    /// The name of the currently selected tile type \c Label.
    private string tileText;

    /// The string to set to \c tileText once a tile type has been selected.
    private string tileTextAfterTileSelected;

    /// The name of the Yes button.
    private string yesButton;
}
