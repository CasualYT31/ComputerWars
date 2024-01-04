/**
 * @file UnloadUnitGrid.as
 * Defines the widget that represents a grid containing details of units that can
 * be unloaded.
 */

/**
 * Grid displaying units that can be unloaded.
 */
class UnloadUnitGrid : Grid {
    /**
     * Sets up the grid.
     * @param proceedHandler When the proceed button is pressed, invoke this
     *                       handler.
     * @param cancelHandler  When the cancel button is pressed, invoke this
     *                       handler.
     */
    UnloadUnitGrid(SingleSignalHandler@ const proceedHandler,
        SingleSignalHandler@ const cancelHandler) {
        // Setup the buttons.
        cancel.setText("cancelunload");
        cancel.connect(MouseReleased, cancelHandler);
        proceed.setText("proceedwithunload");
        proceed.connect(MouseReleased, proceedHandler);

        // Setup the icons.
        hpIcon.setSprite("icon", "hp");
        fuelIcon.setSprite("icon", "fuel");
        ammoIcon.setSprite("icon", "ammo");

        // Setup the grid.
        add(cancel, 0, 0);
        add(proceed, 0, 1);
        add(hpIcon, 0, 2);
        add(fuelIcon, 0, 3);
        add(ammoIcon, 0, 4);
    }

    /**
     * Adds a unit row to the grid.
     * @param unitID        The ID of the unit to display in the new row.
     * @param unloadHandler When the unload button emits a signal, this handler
     *                      will be invoked.
     */
    void addUnit(const UnitID unitID, MultiSignalHandler@ const unloadHandler) {
        units.resize(units.length() + 1);
        @units[units.length() - 1] = UnloadUnitRow(this, unitID, unloadHandler);
    }

    /**
     * Removes every unit row from the grid.
     */
    void removeUnits() {
        for (uint i = 0, len = units.length(); i < len; ++i)
            units[i].cleanUp();
        units.resize(0);
    }

    /**
     * Reinitialises the directional flow within the grid.
     */
    void setupDirectionalFlow() {
        const auto unitCount = units.length();
        const bool unitPresent = unitCount > 0;
        const auto first = unitPresent ? units[0] : null,
            last = unitPresent ? units[unitCount - 1] : null;
        const Widget@ const firstIcon = first is null ? null : @first.icon,
            firstButton = first is null ? null : @first.unload,
            firstHP = first is null ? null : @first.hp,
            firstFuel = first is null ? null : @first.fuel,
            firstAmmo = first is null ? null : @first.ammo,
            lastIcon = last is null ? null : @last.icon,
            lastButton = last is null ? null : @last.unload,
            lastHP = last is null ? null : @last.hp,
            lastFuel = last is null ? null : @last.fuel,
            lastAmmo = last is null ? null : @last.ammo;

        cancel.setDirectionalFlow(lastIcon, firstIcon, ammoIcon, proceed);
        proceed.setDirectionalFlow(lastButton, firstButton, cancel, hpIcon);
        hpIcon.setDirectionalFlow(lastHP, firstHP, proceed, fuelIcon);
        fuelIcon.setDirectionalFlow(lastFuel, firstFuel, hpIcon, ammoIcon);
        ammoIcon.setDirectionalFlow(lastAmmo, firstAmmo, fuelIcon, cancel);

        for (uint i = 0; i < unitCount; ++i) {
            UnloadUnitRow@ const p = (i > 0 ? units[i - 1] : null),
                n = (i < unitCount - 1 ? units[i + 1] : null);
            array<Widget@> up, down;
            if (p is null)
                up = { cancel, proceed, hpIcon, fuelIcon, ammoIcon, null };
            else
                up = { p.icon, p.unload, p.hp, p.fuel, p.ammo, p.hidden };
            if (n is null)
                down = { cancel, proceed, hpIcon, fuelIcon, ammoIcon, null };
            else
                down = { n.icon, n.unload, n.hp, n.fuel, n.ammo, n.hidden };
            units[i].setupDirectionalFlow(up, down);
        }
    }

    /**
     * Enables or disables the proceed button.
     * @param enabled \c TRUE to enable, \c FALSE to disable.
     */
    void proceedEnabled(const bool enabled) {
        proceed.setEnabled(enabled);
    }

    /**
     * Grabs the ID of the widget that should be the directional flow start widget
     * if this grid is the main widget in a menu.
     * @return Either the first unload button's ID or the ID of the cancel button,
     *         if there are no unload buttons.
     */
    WidgetID getMainWidgetID() const {
        return units.length() == 0 ? cancel.ID : units[0].unload.ID;
    }

    /**
     * The cancel button.
     */
    private Button cancel;

    /**
     * The proceed button.
     */
    private Button proceed;

    /**
     * The HP icon that's displayed in the header of the grid.
     */
    private Picture hpIcon;

    /**
     * The fuel icon that's displayed in the header of the grid.
     */
    private Picture fuelIcon;

    /**
     * The ammo icon that's displayed in the header of the grid.
     */
    private Picture ammoIcon;

    /**
     * Each row in the grid.
     */
    private array<UnloadUnitRow@> units;
}

/**
 * A row within an \c UnloadUnitGrid.
 */
class UnloadUnitRow {
    /**
     * Sets up a row and adds it to the bottom of an \c UnloadUnitGrid widget.
     * @param grid          The grid to add the row to.
     * @param unitID        The ID of the unit to display in the new row.
     * @param unloadHandler When the unload button emits a signal, this handler
     *                      will be invoked.
     */
    UnloadUnitRow(UnloadUnitGrid@ const grid, const UnitID unitID,
        MultiSignalHandler@ const unloadHandler) {
        @pGrid = grid;
        const auto type = game.map.getUnitType(unitID);

        // Setup the icon widget.
        icon.setSprite("unit", type.unitSprite(
            game.map.getArmyCountry(game.map.getArmyOfUnit(unitID)).turnOrder));
        
        // Setup the unload button.
        unload.setName(formatUnitID(unitID));
        unload.setText("unload");
        unload.connect(unloadHandler);

        // Setup the labels.
        setupLabel(hp, "~" + formatInt(game.map.getUnitDisplayedHP(unitID)));
        setupLabel(fuel, "~" + formatInt(game.map.getUnitFuel(unitID)));
        // TODO-1 {
        setupLabel(ammo, (type.weaponCount > 0 && !type.weapon(0).hasInfiniteAmmo)
            ? ("~" + formatInt(game.map.getUnitAmmo(unitID,
                type.weapon(0).scriptName))) : ("~"));
        // }
        if (game.map.isUnitHiding(unitID)) {
            @hidden = Label();
            setupLabel(hidden, "hidden");
        }

        // Add widgets to the grid.
        const auto row = grid.getRowCount();
        grid.add(icon, row, 0);
        grid.add(unload, row, 1);
        grid.add(hp, row, 2);
        grid.add(fuel, row, 3);
        grid.add(ammo, row, 4);
        if (hidden !is null) grid.add(hidden, row, 5);
    }

    /**
     * Removes the row from the grid.
     * To be called when the row is being released.
     */
    void cleanUp() {
        pGrid.remove(icon);
        pGrid.remove(unload);
        pGrid.remove(hp);
        pGrid.remove(fuel);
        pGrid.remove(ammo);
        if (hidden !is null) pGrid.remove(hidden);
    }

    /**
     * Sets up directional flow for this row.
     * @param up   The row of widgets, starting from the first column, that are
     *             above this row.
     * @param down The row of widgets, starting from the first column, that are
     *             below this row.
     */
    void setupDirectionalFlow(const array<Widget@>@ const up,
        const array<Widget@>@ const down) {
        // AngelScript crashes here if we do not upcast.
        icon.setDirectionalFlow(up[0], down[0],
            (hidden is null ? cast<Widget@>(ammo) : cast<Widget@>(hidden)),
            unload);
        unload.setDirectionalFlow(up[1], down[1], icon, hp);
        hp.setDirectionalFlow(up[2], down[2], unload, fuel);
        fuel.setDirectionalFlow(up[3], down[3], hp, ammo);
        // If we don't cast here, AngelScript throws a build error.
        // Looks like the above case is supposed to, too.
        ammo.setDirectionalFlow(up[4], down[4], fuel,
            (hidden is null ? cast<Widget@>(icon) : cast<Widget@>(hidden)));
        if (hidden is null) return;
        const Widget@ const upWidget = (up[5] is null ? up[4] : up[5]),
            downWidget = (down[5] is null ? down[4] : down[5]);
        hidden.setDirectionalFlow(upWidget, downWidget, ammo, icon);
    }

    /**
     * Sets up a label.
     * @param label Points to the label to set up.
     * @param text  The caption to set to the label.
     */
    private void setupLabel(Label@ const label, const string&in text) {
        label.setText(text);
        label.setTextColour(White);
        label.setTextOutlineColour(Black);
        label.setTextOutlineThickness(2.0);
    }

    /**
     * Points to the grid this row is within.
     */
    private Grid@ pGrid;

    /**
     * The unit icon.
     */
    Picture icon;

    /**
     * The unload button.
     */
    Button unload;

    /**
     * The HP of the unit.
     */
    Label hp;

    /**
     * The fuel of the unit.
     */
    Label fuel;

    /**
     * The ammo of the unit.
     */
    Label ammo;

    /**
     * If the unit is hidden, this label will be displayed.
     */
    Label@ hidden;
}
