/**
 * @file ListBox.as
 * Defines the \c ListBox class.
 */

/**
 * Represents a listbox in a GUI.
 */
shared class ListBox : Widget {
    /**
     * Create the listbox.
     */
    ListBox() {
        super(ListBoxEngineName);
    }

    void addItem(const string&in caption, array<any>@ const vars = null) {
        ::addItem(this, caption, vars);
    }

    void clearItems() {
        ::clearItems(this);
    }

    void setSelectedItem(const uint64 i) {
        ::setSelectedItem(this, i);
    }

    int getSelectedItem() const {
        return ::getSelectedItem(this);
    }

    string getSelectedItemText() const {
        return ::getSelectedItemText(this);
    }
}
