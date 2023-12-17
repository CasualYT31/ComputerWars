/**
 * @file CountryComboBox.as
 * Defines a widget that can be used to select a country.
 */

/**
 * If a country has been selected from the combobox, a callback of this signature
 * will be invoked.
 * @param <tt>const ArmyID</tt> The \c ArmyID corresponding to the country chosen.
 */
funcdef void CountryComboBoxCallback(const ArmyID);

/**
 * Allows the user to select a country.
 * Made up of a picture widget and a combobox widget.
 */
class CountryComboBox : Group {
    /**
     * Sets up the country combobox.
     */
    CountryComboBox(const bool includeNeutral, const uint64 itemsToDisplay,
        CountryComboBoxCallback@ const callback) {
        // Setup the icon.
        icon.setOrigin(0.5, 0.5);
        icon.setPosition("50%", "50%");
        iconGroup.add(icon);

        // Setup the combobox.
        @listCallback = callback;
        resetList(includeNeutral);
        list.connect(ItemSelected, SingleSignalHandler(this.listItemSelected));
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
     * Clears the combobox and refills it with countries.
     * Automatically selects the first item, if there is one.
     * @param neutral \c TRUE if the list should start with "Neutral." \c FALSE if
     *                this item should not be included.
     * @param filter  A list of turn order IDs of countries to exclude from the
     *                list.
     */
    void resetList(const bool neutral, const array<ArmyID>@ const filter = {}) {
        neutralPresent = neutral;
        list.clearItems();
        if (neutralPresent) list.addItem("neutral");
        const auto countryScriptNames = country.scriptNames;
        for (uint64 c = 0, len = countryScriptNames.length(); c < len; ++c)
            if (filter.find(ArmyID(c)) < 0)
                list.addItem(country[countryScriptNames[c]].name);
        if (list.getItemCount() > 0) list.setSelectedItem(0);
    }

    /**
     * When the user selects a country, update the icon, then invoke the callback,
     * if one was given.
     * Clears the icon, and emits \c NO_ARMY, when there is no item selected.
     */
    private void listItemSelected() {
        const auto item = list.getSelectedItem();
        ArmyID armyID = NO_ARMY;
        if (item >= 0) {
            if (neutralPresent) {
                if (item == 0) armyID = NO_ARMY;
                else armyID = ArmyID(item - 1);
            } else armyID = ArmyID(item);
        }
        if (armyID == NO_ARMY) icon.setSprite("", "");
        else
            icon.setSprite("icon", country[country.scriptNames[armyID]].iconName);
        if (listCallback !is null) listCallback(armyID);
    }

    /**
     * Set to the last value given for \c resetList()'s \c neutral parameter. 
     */
    private bool neutralPresent = false;

    /**
     * Handle to the callback to invoke when a country has been selected.
     */
    private CountryComboBoxCallback@ listCallback;

    /**
     * The group containing the country icon.
     */
    private Group iconGroup;

    /**
     * Depicts the country's icon.
     */
    private Picture icon;

    /**
     * The group containing the country list.
     */
    private Group listGroup;

    /**
     * The country list.
     */
    private ComboBox list;
}
