/**
 * @file TreeView.as
 * Defines the \c TreeView class.
 */

/**
 * Represents a tree view in a GUI.
 */
shared class TreeView : Widget {
    /**
     * Create the tree view.
     */
    TreeView() {
        super(TreeViewEngineName);
    }

    void addItem(const array<string>@ const i) {
        ::addTreeViewItem(this, i);
    }

    void clearItems() {
        ::clearItems(this);
    }

    void setSelectedItem(const array<string>@ const item) {
        ::setSelectedItemTextHierarchy(this, item);
    }

    array<string>@ getSelectedItem() const {
        return ::getSelectedItemTextHierarchy(this);
    }
}
