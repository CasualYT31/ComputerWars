/**
 * @file MessageBox.as
 * Defines the \c MessageBox class.
 */

/**
 * Represents a message box in a GUI.
 */
shared class MessageBox : ChildWindow {
    /**
     * Create the message box.
     */
    MessageBox() {
        super(MessageBoxEngineName);
    }

    void setStrings(const string&in t1, array<any>@ const v1, const string&in t2,
        array<any>@ const v2) {
        setMessageBoxStrings(this, t1, v1, t2, v2);
    }

    void addButton(const string&in t, array<any>@ const v = null) {
        addMessageBoxButton(this, t, v);
    }
    protected void open(const string&in, const string&in) override {}

    uint64 getPressedButton() const {
        return getLastSelectedButton(this);
    }
}
