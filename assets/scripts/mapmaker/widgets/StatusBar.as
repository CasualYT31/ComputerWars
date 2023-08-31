/**
 * @file StatusBar.as
 * Defines the code that manages the status bars of the map maker.
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
        addWidget(Panel, statusBar);
        setWidgetSize(statusBar, "100%", height);
        setWidgetPosition(statusBar, "50%", "100%");
        setWidgetOrigin(statusBar, 0.5f, 1.0f);

        statusBarLayout = statusBar + ".StatusBarLayout";
        addWidget(HorizontalLayout, statusBarLayout);
    }

    /**
     * Clears the status bar.
     */
    void clear() {
        for (uint i = 0, len = labels.length(); i < len; ++i)
            setWidgetText(labels[i], "~");
    }

    /**
     * Adds a label to the end of a status bar.
     * @param  widgetName The short name of the label widget.
     * @param  ratio      The ratio in width between the new label and all the
     *                    other labels. If every label has the default ratio, each
     *                    label will be of equal width.
     * @return The long name of the label widget.
     */
    string addLabel(const string&in widgetName, const float ratio = 1.0f) {
        const auto group = statusBarLayout + "." + widgetName + "Group";
        addWidget(Group, group);
        setWidgetRatioInLayout(statusBarLayout,
            getWidgetCount(statusBarLayout) - 1, ratio);

        const auto name = group + "." + widgetName;
        addWidget(Label, name);
        labels.insertLast(name);
        return name;
    }

    /**
     * Updates a label's text.
     * @param labelLongName The long name of the label in the Status Bar.
     * @param text          The text to apply to the label.
     * @param vars          The variables to insert into the \c text.
     */
    void setLabel(const string&in labelLongName, const string&in text,
        array<any>@ const vars = null) {
        setWidgetText(labelLongName, text, vars);
    }

    /// The name of the status bar's \c HorizontalLayout widget.
    private string statusBarLayout;

    /// The name of every label added to the Status Bar.
    private array<string> labels;
}

/**
 * The Map Maker's main Status Bar.
 */
class MainStatusBar {
    /**
     * Sets up the status bar.
     * @param parent The parent of the status bar.
     * @param height The height of the status bar.
     */
    void setUp(const string&in parent, const string&in height) {
        statusBar.setUp(parent, height);
        undoActionLabel = statusBar.addLabel("UndoAction");
        redoActionLabel = statusBar.addLabel("RedoAction");
        zoomLabel = statusBar.addLabel("Zoom");
        xyLabel = statusBar.addLabel("XY");
    }

    /**
     * Clears the status bar.
     */
    void clear() {
        statusBar.clear();
    }

    /**
     * Sets the text of the undo action label.
     * @param label The new text.
     */
    void setUndoAction(const string&in label) {
        statusBar.setLabel(undoActionLabel, label);
    }

    /**
     * Sets the text of the redo action label.
     * @param label The new text.
     */
    void setRedoAction(const string&in label) {
        statusBar.setLabel(redoActionLabel, label);
    }

    /**
     * Sets the X and Y location of the currently selected tile to the XY label.
     * @param pos The new X, Y location.
     */
    void setTileXY(const Vector2&in pos) {
        statusBar.setLabel(xyLabel, "~" + pos.toString());
    }

    /**
     * Sets the map scaling factor to the Zoom label.
     * @param factor The map scaling factor.
     */
    void setZoom(const float factor) {
        statusBar.setLabel(zoomLabel, "zoom", { any(factor) });
    }

    /// The Status Bar.
    private StatusBarWidget statusBar;

    /// The name of the undo action \c Label widget.
    private string undoActionLabel;

    /// The name of the redo action \c Label widget.
    private string redoActionLabel;

    /// The name of the X and Y /c Label widget.
    private string xyLabel;

    /// The name of the zoom \c Label widget.
    private string zoomLabel;
}

/**
 * The Scripts Window's Status Bar.
 */
class ScriptsStatusBar {
    /**
     * Sets up the status bar.
     * @param parent The parent of the status bar.
     * @param height The height of the status bar.
     */
    void setUp(const string&in parent, const string&in height) {
        statusBar.setUp(parent, height);
        compilerResultLabel = statusBar.addLabel("CompilerResult", 0.8f);
        caretLabel = statusBar.addLabel("CaretPosition", 0.2f);
    }

    /**
     * Clears the status bar.
     */
    void clear() {
        statusBar.clear();
    }

    /**
     * Sets the text of the compiler result label.
     * @param result The new text.
     */
    void setCompilerResult(const string&in result) {
        statusBar.setLabel(compilerResultLabel, "~" + result);
    }

    /**
     * Sets the text of the caret position label.
     * @param line The line number.
     * @param col  The column number.
     */
    void setCaretPosition(const uint64 line, const uint64 col) {
        statusBar.setLabel(caretLabel, "caret", { any(line), any(col) });
    }

    /// The Status Bar.
    private StatusBarWidget statusBar;

    /// The name of the compiler result \c Label widget.
    private string compilerResultLabel;

    /// The name of the caret position \c Label widget.
    private string caretLabel;
}
