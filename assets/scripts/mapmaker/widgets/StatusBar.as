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

        statusBarLayout = statusBar + ".StatusBarLayout";
        addWidget("HorizontalLayout", statusBarLayout);

        undoActionLabel = _addLabel("UndoAction");
        redoActionLabel = _addLabel("RedoAction");
        zoomLabel = _addLabel("Zoom");
        xyLabel = _addLabel("XY");
    }

    /**
     * Clears the status bar.
     */
    void clear() {
        setWidgetText(undoActionLabel, "~");
        setWidgetText(redoActionLabel, "~");
        setWidgetText(xyLabel, "~");
        setWidgetText(zoomLabel, "~");
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

    /**
     * Sets the X and Y location of the currently selected tile to the XY label.
     * @param pos The new X, Y location.
     */
    void setTileXY(const Vector2&in pos) {
        setWidgetText(xyLabel, "~" + pos.toString());
    }

    /**
     * Sets the map scaling factor to the Zoom label.
     * @param factor The map scaling factor.
     */
    void setZoom(const float factor) {
        setWidgetText(zoomLabel, "zoom", { any(factor) });
    }

    /**
     * Adds a label to a status bar.
     * @param  widgetName The short name of the label widget.
     * @return The long name of the label widget.
     */
    private string _addLabel(const string&in widgetName) {
        const auto group = statusBarLayout + "." + widgetName + "Group";
        addWidget("Group", group);

        const auto name = group + "." + widgetName;
        addWidget("Label", name);
        return name;
    }

    /// The name of the status bar's \c HorizontalLayout widget.
    private string statusBarLayout;

    /// The name of the undo action \c Label widget.
    private string undoActionLabel;

    /// The name of the redo action \c Label widget.
    private string redoActionLabel;

    /// The name of the X and Y /c Label widget.
    private string xyLabel;

    /// The name of the zoom \c Label widget.
    private string zoomLabel;
}
