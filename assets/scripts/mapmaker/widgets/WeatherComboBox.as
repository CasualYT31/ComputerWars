/**
 * @file WeatherComboBox.as
 * Defines a widget that can be used to select a weather.
 */

/**
 * If a weather has been selected from the combobox, a callback of this signature
 * will be invoked.
 * @param <tt>const string&in</tt> The script name of the weather chosen.
 */
funcdef void WeatherComboBoxCallback(const string&in);

/**
 * Allows the user to select a weather.
 * Made up of a picture widget and a combobox widget.
 */
class WeatherComboBox : Group {
    /**
     * Sets up the weather combobox.
     */
    WeatherComboBox(const uint64 itemsToDisplay,
        WeatherComboBoxCallback@ const callback) {
        // Setup the icon.
        icon.setOrigin(0.5, 0.5);
        icon.setPosition("50%", "50%");
        iconGroup.add(icon);

        // Setup the combobox.
        list.connect(ItemSelected, SingleSignalHandler(this.listItemSelected));
        const auto weatherScriptNames = weather.scriptNames;
        for (uint64 c = 0, len = weatherScriptNames.length(); c < len; ++c)
            list.addItem(weather[weatherScriptNames[c]].name);
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
    void setCallback(WeatherComboBoxCallback@ const cb) {
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
     * Returns the index of the weather selected.
     * @return The 0-based index of the weather selected, or < 0 if no weather is
     *         selected.
     */
    int getSelected() const {
        return list.getSelectedItem();
    }

    /**
     * When the user selects a weather, update the icon, then invoke the callback,
     * if one was given.
     * Clears the icon, and emits an empty string, when there is no item selected.
     */
    private void listItemSelected() {
        const auto item = list.getSelectedItem();
        string selectedWeather;
        if (item >= 0) {
            selectedWeather = weather.scriptNames[uint(item)];
        }
        if (selectedWeather.isEmpty()) icon.setSprite("", "");
        else icon.setSprite("icon", weather[selectedWeather].iconName);
        if (listCallback !is null) listCallback(selectedWeather);
    }

    /**
     * Handle to the callback to invoke when a weather has been selected.
     */
    private WeatherComboBoxCallback@ listCallback;

    /**
     * The group containing the weather icon.
     */
    private Group iconGroup;

    /**
     * Depicts the weather's icon.
     */
    private Picture icon;

    /**
     * The group containing the weather list.
     */
    private Group listGroup;

    /**
     * The weather list.
     */
    private ComboBox list;
}
