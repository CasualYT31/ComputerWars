/**
 * @file MenuBar.as
 * Defines the \c MenuBar class.
 */

/**
 * Represents a menu bar in a GUI.
 * The interface for this class is limiting but suits my purposes for now.
 */
shared class MenuBar : Widget {
    /**
     * Create the menu bar.
     */
    MenuBar() {
        super(MenuBarEngineName);
    }

    void add(const string&in n) {
        ::addMenu(this, n);
    }

    MenuItemID addItem(const string&in i) {
        return ::addMenuItem(this, i);
    }

    MenuItemID getSelectedItem() const {
        return ::getLastSelectedMenuItem(this);
    }
}
