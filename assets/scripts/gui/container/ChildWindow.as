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

    void setText(const string&in t, array<any>@ const vars = null) {
        ::setWidgetText(this, t, vars);
    }

    void setPositionLocked(const bool l) {
        ::setWidgetPositionLocked(this, l);
    }

    void setResizable(const bool r) {
        ::setWidgetResizable(this, r);
    }

    void setTitleButtons(const uint b) {
        setChildWindowTitleButtons(this, b);
    }

    float getTitleBarHeight() const {
        return ::getTitleBarHeight(this);
    }

    void connectClosing(ChildWindowClosingSignalHandler@ const h) {
        connectChildWindowClosingSignalHandler(this, h);
    }

    void open(const string&in x, const string&in y) {
        ::openChildWindow(this, x, y);
    }

    bool isOpen() const {
        return ::isChildWindowOpen(this);
    }

    void close(const bool emitClosingSignal = true) {
        if (emitClosingSignal) ::closeChildWindowAndEmitSignal(this);
        else ::closeChildWindow(this);
    }

    void restore() {
        ::restoreChildWindow(this);
    }
}
