/**
 * @file ConfirmTileTypeWindow.as
 * Defines the window that lets the user confirm their choice of tile type.
 */

/**
 * Allows the user to confirm if they wish to carry out an operation with the tile
 * type they currently have selected.
 */
class ConfirmTileTypeWindow : Observer, ButtonsWindow {
    /**
     * Sets up the confirmation child window.
     * @param beforeTileSelected The text to display in the subtext before a tile
     *                           type has been selected.
     * @param afterTileSelected  The text to display in the subtext when a tile
     *                           type is selected.
     * @param yes                Code to execute when the yes button is clicked.
     * @param no                 Code to execute when the no button is clicked.
     */
    ConfirmTileTypeWindow(const string&in beforeTileSelected,
        const string&in afterTileSelected, SingleSignalHandler@ const yes,
        SingleSignalHandler@ const no) {
        // Setup child window.
        super({ ButtonsWindowButton("no", no),
            ButtonsWindowButton("yes", yes) });
        refresh();
        setText("alert");
        setSize("250", "175");
        setResizable(false);
        setTitleButtons(TitleButton::None);
        close();

        // Setup the body text.
        body.setSize("100%", "100%");
        body.setTextMaximumWidth(body.getFullSize().x);
        bodyGroup.add(body);

        // Setup the subtext.
        subtext.setText(beforeTileSelected);
        subtextGroup.add(subtext);

        // Setup the selected object widget.
        @tileType = selectedTileType.widgetFactory();
        tileTypeGroup.add(tileType);

        // Setup the base layout.
        baseLayout.add(bodyGroup);
        baseLayout.add(subtextGroup);
        baseLayout.add(tileTypeGroup);
        baseLayout.setRatioOfWidget(0, 1.25);
        clientArea.add(baseLayout);
    }

    /**
     * Ensures the selected object widget is detached to allow for it to be
     * deleted.
     */
    ~ConfirmTileTypeWindow() {
        selectedTileType.detach(tileType);
    }

    /**
     * Restore the confirmation window.
     * @param bodyText The text to apply to the body label. Leave empty if you
     *                 don't want to change the text.
     */
    void open(const string&in bodyText = "", const string&in x = "50",
        const string&in y = "35") {
        if (!bodyText.isEmpty()) body.setText(bodyText);
        ChildWindow::open(x, y);
    }

    /**
     * We usually want to close this window immediately without emitting any
     * signal.
     */
    void close(const bool emitClosingSignal = false) override {
        ChildWindow::close(emitClosingSignal);
    }

    /**
     * The selected tile type was changed.
     * If a tile type has been selected, enable the yes button.
     */
    void refresh(any&in data = any()) override {
        buttons[1].setEnabled(selectedTileType.type !is null);
    }

    /**
     * The base layout added directly to the client area.
     */
    private VerticalLayout baseLayout;

    /**
     * The group containing the body text.
     */
    private Group bodyGroup;

    /**
     * The main text of the window.
     */
    private Label body;

    /**
     * The group containing the subtext.
     */
    private Group subtextGroup;

    /**
     * Text describing what the selected tile type will be used for in more
     * detail.
     */
    private Label subtext;

    /**
     * The group containing the tile type \c SelectedObjectWidget.
     */
    private Group tileTypeGroup;

    /**
     * Displays the currently selected tile type.
     */
    private SelectedObjectWidget@ tileType;
}
