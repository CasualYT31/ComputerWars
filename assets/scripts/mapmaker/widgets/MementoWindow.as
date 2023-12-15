/**
 * @file MementoWindow.as
 * Defines the class representing the window used to list and select mementos.
 */

/**
 * Allows the user to view and select mementos.
 */
class MementoWindow : ChildWindow {
    /**
     * Sets up the memento child window.
     */
    MementoWindow() {
        setText("mementowindow");
        setSize("200", "400");
        close(false);

        listBoxGroup.setPadding("5");
        add(listBoxGroup);
        
        listBox.setSize("100%", "100%");
        listBoxGroup.add(listBox);
    }

    /**
     * Restore the memento window.
     */
    void open(const string&in x = "100", const string&in y = "50") override {
        ChildWindow::open(x, y);
    }

    /**
     * We usually want to close this window immediately without emitting any
     * signal.
     */
    void close(const bool emitClosingSignal = false) override {
        ChildWindow::close(emitClosingSignal);
    }

    /**
     * Refreshes the memento window.
     */
    void refresh() {
        listBox.disconnectAll();
        listBox.clearItems();
        if (edit is null) return;
        const auto mementos = edit.map.getMementos(selected);
        for (uint i = 0, len = mementos.length(); i < len; ++i)
            listBox.addItem(mementos[i]);
        listBox.setSelectedItem(selected);
        listBox.connect(MouseReleased,
            SingleSignalHandler(this.listBoxMouseReleased));
    }

    /**
     * When the user selects a memento, undo or redo as appropriate.
     */
    private void listBoxMouseReleased() {
        if (edit is null) return;
        int item = listBox.getSelectedItem();
        uint64 itemCast = item;
        if (item >= 0 && selected != itemCast) {
            if (itemCast > selected) edit.undo(itemCast - selected - 1);
            else edit.redo(selected - itemCast - 1);
            selected = itemCast;
        }
    }

    /**
     * The group in which the memento listbox is placed.
     */
    private Group listBoxGroup;

    /**
     * The listbox in which all mementos are listed.
     */
    private ListBox listBox;

    /**
     * The previously selected item in the listbox widget.
     */
    private uint64 selected;
}
