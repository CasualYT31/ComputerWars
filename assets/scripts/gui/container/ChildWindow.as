/**
 * @file ChildWindow.as
 * Defines the \c ChildWindow class.
 */

/**
 * Represents a child window in a GUI.
 */
shared class ChildWindow : Container {
    /**
     * Create the child window.
     * Should not be called by subclasses that represent a subtype of this widget.
     * E.g. \c MessageBox.
     */
    ChildWindow() {
        super(ChildWindowEngineName);
    }

    /**
     * Create the child window.
     * Should be called by subclasses that represent a subtype of this widget.
     * E.g. \c MessageBox.
     * @param widgetType The name of the type of container to create.
     */
    ChildWindow(const string&in widgetType) {
        super(widgetType);
    }

    void connectClosing(ChildWindowClosingSignalHandler@ const h) {
        connectChildWindowClosingSignalHandler(this, h);
    }

    // Don't forget to override minimise, maximise stuff in subclasses such as
    // MessageBox with empty implementation... Not sure if I can delete them
    // in those classes altogether?
}
