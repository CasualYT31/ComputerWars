/**
 * @file ComboBox.as
 * Defines the \c ComboBox class.
 */

/**
 * Represents a combobox in a GUI.
 */
shared class ComboBox : Widget {
    /**
     * Create the combobox.
     */
    ComboBox() {
        super(ComboBoxEngineName);
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

    void setItemsToDisplay(const uint64 i) {
        ::setItemsToDisplay(this, i);
    }

    uint64 getItemCount() const {
        return ::getItemCount(this);
    }
}
