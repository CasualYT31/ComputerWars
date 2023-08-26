/**
 * @file StatusBar.as
 * Defines the code that manages the status bar of the map maker.
 */

/**
 * Represents a status bar.
 */
class StatusBarWidget {
    /**
     * Sets up the status bar.
     * @param parent The parent of the status bar. It will fill up the width of
     *               the parent, and will be anchored to the bottom.
     * @param height The height of the status bar.
     */
    void setUp(const string&in parent, const string&in height) {
        const auto statusBar = parent + ".StatusBar";
        addWidget("Panel", statusBar);
        setWidgetSize(statusBar, "100%", height);
        setWidgetPosition(statusBar, "50%", "100%");
        setWidgetOrigin(statusBar, 0.5f, 1.0f);

        const auto statusBarLayout = statusBar + ".StatusBarLayout";
        addWidget("HorizontalLayout", statusBarLayout);

        const auto undoActionLabelGroup = statusBarLayout + ".UndoActionGroup";
        addWidget("Group", undoActionLabelGroup);

        undoActionLabel = undoActionLabelGroup + ".UndoAction";
        addWidget("Label", undoActionLabel);

        const auto redoActionLabelGroup = statusBarLayout + ".RedoActionGroup";
        addWidget("Group", redoActionLabelGroup);

        redoActionLabel = redoActionLabelGroup + ".RedoAction";
        addWidget("Label", redoActionLabel);
    }

    /**
     * Sets the text of the undo action label.
     * @param label The new text.
     */
    void setUndoAction(const string&in label) {
        setWidgetText(undoActionLabel, label);
    }

    /**
     * Sets the text of the redo action label.
     * @param label The new text.
     */
    void setRedoAction(const string&in label) {
        setWidgetText(redoActionLabel, label);
    }

    /// The name of the undo action \c Label widget.
    private string undoActionLabel;

    /// The name of the redo action \c Label widget.
    private string redoActionLabel;
}
