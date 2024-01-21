/**
 * @file EnvironmentComboBox.as
 * Defines a widget that can be used to select an environment.
 */

/**
 * If an environment has been selected from the combobox, a callback of this
 * signature will be invoked.
 * @param <tt>const string&in</tt> The script name of the environment chosen.
 */
funcdef void EnvironmentComboBoxCallback(const string&in);

/**
 * Allows the user to select an environment.
 * Made up of a picture widget and a combobox widget.
 */
class EnvironmentComboBox : Group {
    /**
     * Sets up the environment combobox.
     */
    EnvironmentComboBox(const uint64 itemsToDisplay,
        EnvironmentComboBoxCallback@ const callback) {
        // Setup the icon.
        icon.setOrigin(0.5, 0.5);
        icon.setPosition("50%", "50%");
        iconGroup.add(icon);

        // Setup the combobox.
        list.connect(ItemSelected, SingleSignalHandler(this.listItemSelected));
        const auto environmentScriptNames = environment.scriptNames;
        for (uint64 c = 0, len = environmentScriptNames.length(); c < len; ++c)
            list.addItem(environment[environmentScriptNames[c]].name);
        if (list.getItemCount() > 0) select(0);
        @listCallback = callback;
        list.setItemsToDisplay(itemsToDisplay);
        list.setSize("100%", "100%");
        listGroup.add(list);

        // Setup the groups.
        iconGroup.setPadding("5");
        iconGroup.setSize("parent.height", "");
        iconGroup.setAutoLayout(AutoLayout::Left);
        add(iconGroup);
        listGroup.setPadding("0", "5", "5", "5");
        listGroup.setAutoLayout(AutoLayout::Fill);
        add(listGroup);
    }

    /**
     * Updates the callback.
     * @param cb Points to the callback to use instead of the one currently
     *           stored.
     */
    void setCallback(EnvironmentComboBoxCallback@ const cb) {
        @listCallback = cb;
    }

    /**
     * Select an item from the combobox.
     * @param i The 0-based index of the item to select.
     */
    void select(const uint64 i) {
        list.setSelectedItem(i);
    }

    /**
     * Returns the index of the environment selected.
     * @return The 0-based index of the environment selected, or < 0 if no
     *         environment is selected.
     */
    int getSelected() const {
        return list.getSelectedItem();
    }

    /**
     * When the user selects an environment, update the icon, then invoke the
     * callback, if one was given.
     * Clears the icon, and emits an empty string, when there is no item selected.
     */
    private void listItemSelected() {
        const auto item = list.getSelectedItem();
        string selectedEnvironment;
        if (item >= 0) {
            selectedEnvironment = environment.scriptNames[uint(item)];
        }
        if (selectedEnvironment.isEmpty()) icon.setSprite("", "");
        else icon.setSprite("icon", environment[selectedEnvironment].iconName);
        if (listCallback !is null) listCallback(selectedEnvironment);
    }

    /**
     * Handle to the callback to invoke when an environment has been selected.
     */
    private EnvironmentComboBoxCallback@ listCallback;

    /**
     * The group containing the environment icon.
     */
    private Group iconGroup;

    /**
     * Depicts the environment's icon.
     */
    private Picture icon;

    /**
     * The group containing the environment list.
     */
    private Group listGroup;

    /**
     * The environment list.
     */
    private ComboBox list;
}
