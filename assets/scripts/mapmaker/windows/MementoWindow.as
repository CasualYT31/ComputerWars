/**
 * @file MementoWindow.as
 * Defines the class representing the window used to list and select mementos.
 */

/**
 * Allows the user to view and select mementos.
 */
class MementoWindow {
    /**
     * Sets up the memento child window.
     * @param parent The parent of the child window.
     */
    void setUp(const string&in parent) {
        window = parent + ".MementoWindow";
        addWidget("ChildWindow", window);
        setWidgetText(window, "mementowindow");
        setWidgetSize(window, "200px", "400px");
        closeChildWindow(window);

        const auto listboxGroup = window + ".MementoGroup";
        addWidget("Group", listboxGroup);
        setGroupPadding(listboxGroup, "5px");

        listbox = listboxGroup + ".MementoList";
        addWidget("ListBox", listbox);
        setWidgetSize(listbox, "100%", "100%");
    }

    /// Restores the window.
    void restore() {
        openChildWindow(window, "100px", "50px");
    }

    /// Refreshes the window.
    void refresh() {
        if (edit !is null) {
            clearItems(listbox);
            const auto mementos = edit.map.getMementos(selected);
            for (uint i = 0, len = mementos.length(); i < len; ++i)
                addItem(listbox, mementos[i]);
            disconnectSignalHandlers({ listbox });
            setSelectedItem(listbox, selected);
            connectSignalHandler(listbox,
                SignalHandler(this.listboxSignalHandler));
        }
    }

    /**
     * When an item in the listbox is selected, undo or redo as appropriate.
     * @param widgetName The full name of the \c ListBox.
     * @param signalName The name of the signal emitted.
     */
    private void listboxSignalHandler(const string&in widgetName,
        const string&in signalName) {
        if (edit is null) return;
        if (signalName == "ItemSelected") {
            int item = getSelectedItem(widgetName);
            uint64 itemCast = item;
            if (item >= 0 && selected != itemCast) {
                if (itemCast > selected) edit.undo(itemCast - selected - 1);
                else edit.redo(selected - itemCast - 1);
                selected = itemCast;
            }
        }
    }

    /// Caches the name of the \c ChildWindow widget.
    private string window;

    /// Caches the name of the \c ListBox widget.
    private string listbox;

    /// The previously selected item in the \c ListBox widget.
    private uint64 selected;
}
