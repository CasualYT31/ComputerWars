/**
 * @file TilePropertiesWindow.as
 * Defines code that manages the tile properties child window.
 */

/**
 * Defines constants that the \c TilePropertiesWindow class uses internally.
 */
namespace TilePropertiesWindowConstants {
    /**
     * The width of the tile properties window when it is docked.
     */
    const string DockedWidth = "200";
}

/**
 * Allows the user to view and edit the currently selected (using the tileinfo
 * control) tile.
 */
class TilePropertiesWindow : Observer, ChildWindow {
    /**
     * Sets up the tile properties window.
     */
    TilePropertiesWindow() {
        close();
        setText("tileprops");
        noTileSelectMessage.setText("pleaseselecttile");
        noTileSelectMessage.setPosition("50%", "50%");
        noTileSelectMessage.setOrigin(0.5, 0.5);
        add(noTileSelectMessage);
        add(tabs);
        refresh(any());
        // Register Closing signal now so that close() call doesn't try to access
        // edit indirectly during the game's initialisation.
        connectClosing(function(alwaysClose){ edit.deselectTile(); });
    }

    /**
     * Restore the tile properties window.
     * Always select the tile tab, if it exists.
     */
    void open(const string&in x, const string&in y) override {
        ChildWindow::open(x, y);
        isOpening = true;
        selectBestTabIfNoTabIsSelected();
        isOpening = false;
    }

    /**
     * Restores and docks the tile properties window to the left of its parent.
     */
    void dock() {
        setSize(TilePropertiesWindowConstants::DockedWidth, "100%");
        open("0%", "0%");
    }

    /**
     * We usually want to close this window immediately without emitting any
     * signal.
     */
    void close(const bool emitClosingSignal = false) override {
        ChildWindow::close(emitClosingSignal);
    }

    /**
     * Refreshes the tile properties window.
     */
    void refresh(any&in data) override {
        // Find the tile to display information on. If it can't be found, or if
        // it's out-of-bounds, disable and deselect all tabs.
        Vector2 tile;
        if (!data.retrieve(tile) || edit.map.isOutOfBounds(tile)) {
            tabs.setTabEnabled(0, false);
            tabs.setTabEnabled(1, false);
            tabs.deselectTab();
            return;
        }

        // Re-enable the tile tab and refresh the tile properties panel.
        tabs.setTabEnabled(0, true);
        tilePanel.refresh(tile);
        
        // If there is a unit on the tile, re-enable and refresh the unit panel.
        // If there is no unit, disable the unit panel and deselect it.
        const auto unit = edit.map.getUnitOnTile(tile);
        tabs.setTabEnabled(1, unit != NO_UNIT);
        if (unit == NO_UNIT) tabs.deselectTab();
        else unitPanel.refresh(unit);

        // If the window is closed, dock it. If it's minimised, restore it.
        selectBestTabIfNoTabIsSelected();
        if (isOpen()) restore(); else dock();
    }

    /**
     * If no tab is selected, this method will select the "best" one available.
     */
    private void selectBestTabIfNoTabIsSelected() {
        if (tabs.getSelectedTab() >= 0) {
            // Still play the select sound.
            if (!isOpening) edit.map.queuePlay("sound", "select");
            return;
        }
        if (tabs.getTabEnabled(1)) tabs.setSelectedTab(1);
        else if (tabs.getTabEnabled(0)) tabs.setSelectedTab(0);
    }

    /**
     * The tab container.
     */
    private TabContainer tabs;

    /**
     * The tile properties panel.
     */
    private TilePropertiesPanel tilePanel(tabs, "tiletab");

    /**
     * The tile properties panel.
     */
    private UnitPropertiesPanel unitPanel(tabs, "unittab");

    /**
     * Message that's visible when none of the panels are selected.
     */
    private Label noTileSelectMessage;

    /**
     * Prevents playing the select sound twice when opening this window in certain
     * cases.
     */
    private bool isOpening = false;
}

/**
 * The panel within the tile properties window that displays the properties of the
 * current tile.
 */
class TilePropertiesPanel {
    /**
     * Sets up the tile properties panel.
     * @param tabContainer The tab container to create the tab and panel in.
     * @param tabText      The text to apply to the new tab.
     */
    TilePropertiesPanel(TabContainer@ const tabContainer,
        const string&in tabText) {
        ::add(tabContainer.addTabAndPanel(tabText), base);

        // Setup the terrain picture.
        picture.setOrigin(0.5, 0.0);
        picture.setPosition("50%", "0%");
        pictureGroup.add(picture);
        base.add(pictureGroup);
        base.setRatioOfWidget(0, 3);

        // Setup the tile type long name.
        tileTypeName.setOrigin(0.5, 0.0);
        tileTypeName.setPosition("50%", "0%");
        tileTypeName.setTextSize(11);
        tileTypeName.setTextColour(Colour(125, 125, 125, 255));
        tileTypeName.setTextStyles("Italic");
        tileTypeNameGroup.add(tileTypeName);
        base.add(tileTypeNameGroup);
        base.setRatioOfWidget(1, 0.5);

        // Setup the terrain long name.
        terrainName.setOrigin(0.5, 0.0);
        terrainName.setPosition("50%", "0%");
        terrainName.setTextSize(24);
        terrainName.setTextStyles("Bold");
        terrainNameGroup.add(terrainName);
        base.add(terrainNameGroup);
        base.setRatioOfWidget(2, 0.7);

        // Setup the position.
        positionLayout.add(x);
        positionLayout.add(y);
        base.add(positionLayout);

        // Setup the defence.
        base.add(defence);
        base.setRatioOfWidget(4, 0.75);

        // Setup the HP.
        base.add(hpLayout);

        // Setup the owner.
        ownerLabel.setText("owner");
        ownerLabel.setOrigin(0.5, 0.5);
        ownerLabel.setPosition("65%", "50%");
        owner.setSize("", "35");
        owner.setOrigin(0.5, 0.5);
        owner.setPosition("50%", "50%");
        ownerLabelGroup.add(ownerLabel);
        ownerGroup.add(owner);
        ownerLayout.add(ownerLabelGroup);
        ownerLayout.add(ownerGroup);
        ownerLayout.setRatioOfWidget(0, 33);
        ownerLayout.setRatioOfWidget(1, 67);
        base.add(ownerLayout);

        connectSignalHandlers();
    }

    /**
     * Refreshes the tile properties panel.
     * @param tile The tile to display information on.
     */
    void refresh(const Vector2&in tile) {
        disconnectSignalHandlers();
        const auto tileType = edit.map.getTileType(tile);
        const auto tileOwner = edit.map.getTileOwner(tile);
        const auto terrain = tileType.type;
        picture.setSprite("tilePicture.normal",
            tileOwner == NO_ARMY ? terrain.iconName : terrain.picture(tileOwner));
        tileTypeName.setText("~" + tileType.scriptName);
        terrainName.setText(terrain.name);
        x.setText("~" + formatUInt(tile.x));
        y.setText("~" + formatUInt(tile.y));
        defence.setText("~" + formatUInt(terrain.defence));
        hpLayout.setEditBoxEnabled(terrain.maxHP > 0);
        hpLayout.setEditBoxText(uint(edit.map.getTileHP(tile)));
        hpLayout.setDynamicText(terrain.maxHP);
        if (tileOwner == NO_ARMY) owner.select(0);
        else owner.select(tileOwner + 1);
        connectSignalHandlers();
    }

    /**
     * Connects all the signal handlers that update the state of the map.
     */
    private void connectSignalHandlers() {
        hpLayout.connectEditBox(SingleSignalHandler(this.newHPSelected));
        owner.setCallback(CountryComboBoxCallback(this.newOwnerSelected));
    }

    /**
     * Disconnects all the signal handlers that update the state of the map.
     */
    private void disconnectSignalHandlers() {
        hpLayout.connectEditBox(null);
        owner.setCallback(null);
    }

    /**
     * When a new HP value has been selected, update the tile.
     */
    private void newHPSelected() {
        edit.setSelectedTileHP(HP(hpLayout.getEditBoxText()));
    }

    /**
     * When a new owner is selected, set it to the tile.
     * @param newOwner The ID of the new owner, or \c NO_ARMY if there will be no
     *                 owner.
     */
    private void newOwnerSelected(const ArmyID newOwner) {
        edit.setSelectedTileOwner(newOwner);
    }

    /**
     * The base layout.
     */
    private VerticalLayout base;

    /**
     * The group containing the terrain picture.
     */
    private Group pictureGroup;

    /**
     * The terrain picture.
     */
    private Picture picture;

    /**
     * The group containing the tile's script name.
     */
    private Group tileTypeNameGroup;

    /**
     * Displays the tile's script name.
     */
    private Label tileTypeName;

    /**
     * The group containing the tile's terrain name.
     */
    private Group terrainNameGroup;

    /**
     * The tile's terrain name.
     */
    private Label terrainName;

    /**
     * Contains the widgets that display the tile's position on the map.
     */
    private HorizontalLayout positionLayout;

    /**
     * The layout containing the tile's X position.
     */
    private ReadOnlyPropertyLayout x("~X");

    /**
     * The layout containing the tile's Y position.
     */
    private ReadOnlyPropertyLayout y("~Y");
    
    /**
     * Contains the tile's defence information.
     */
    private ReadOnlyPropertyLayout defence("def");

    /**
     * Contains the widgets that display the tile's HP.
     */
    private PropertyLayout hpLayout("hp");

    /**
     * Contains the widgets that display the tile's owner.
     */
    private HorizontalLayout ownerLayout;

    /**
     * Contains the owner label.
     */
    private Group ownerLabelGroup;

    /**
     * The owner label.
     */
    private Label ownerLabel;

    /**
     * Contains the widget that displays the owner of the tile.
     */
    private Group ownerGroup;

    /**
     * Allows the user to select the owner of the tile.
     */
    private CountryComboBox owner(true, 6,
        CountryComboBoxCallback(this.newOwnerSelected));
}

/**
 * Defines constants used by the \c UnitPropertiesPanel class internally.
 */
namespace UnitPropertiesPanelConstants {
    /**
     * The minimum height of the base layout.
     */
    const string MinimumHeight = "600";
}

/**
 * The panel within the tile properties window that displays the properties of the
 * unit on the current tile.
 */
class UnitPropertiesPanel {
    /**
     * Sets up the unit properties panel.
     * @param tabContainer The tab container to create the tab and panel in.
     * @param tabText      The text to apply to the new tab.
     */
    UnitPropertiesPanel(TabContainer@ const tabContainer,
        const string&in tabText) {
        ::add(tabContainer.addTabAndPanel(tabText), scrollablePanel);
        scrollablePanel.setVerticalScrollbarAmount(50);
        scrollablePanel.add(base);
        base.setSize("100%-" + formatFloat(scrollablePanel.getScrollbarWidth()),
            UnitPropertiesPanelConstants::MinimumHeight);

        // Setup the unit tree view.
        treeView.setSize("100%", "100%");
        treeView.connect(ItemSelected,
            SingleSignalHandler(this.treeViewItemSelected));
        treeViewGroup.add(treeView);
        base.add(treeViewGroup);
        base.setRatioOfWidget(0, 2.0);

        // Setup the unit picture.
        picture.setOrigin(0.5, 0.5);
        picture.setPosition("50%", "50%");
        pictureGroup.add(picture);
        base.add(pictureGroup);
        base.setRatioOfWidget(1, 2.0);

        // Setup the unit name and owner.
        unitName.setOrigin(0.5, 0.0);
        unitName.setPosition("50%", "0%");
        unitName.setTextSize(24);
        unitName.setTextStyles("Bold");
        unitNameGroup.add(unitName);
        base.add(unitNameGroup);
        base.add(unitOwner);

        // Setup dynamic information. FuelLayout widget name is used to calculate
        // how high ammo layouts should be.
        fuelLayout.setName("FuelLayout");
        base.add(hpLayout);
        base.add(fuelLayout);
        ammoLayoutFirstIndex = base.widgetCount();

        // Setup the waiting, capturing, and hiding layouts.
        base.add(waiting);
        base.add(capturing);
        base.add(hiding);

        // Setup the buttons.
        setupBitmapButton(@createAndLoadGroup, @createAndLoad, "createandload",
            "loadicon", SingleSignalHandler(this.createAndLoadUnit));
        setupBitmapButton(@deleteGroup, @delete, "deleteunit", "deleteicon",
            SingleSignalHandler(this.deleteUnit));
        base.add(createAndLoadGroup);
        base.add(deleteGroup);

        // Setup static information.
        base.add(loadLimit);
        base.add(movementType);
        base.add(movementPoints);
        base.add(vision);
        base.add(priceLayout);

        connectSignalHandlers();
    }

    /**
     * Refreshes the unit properties panel.
     * Repopulate the tree view. Then, select the first item. The signal handler
     * will do the rest.
     * @param unit The ID of the unit to display information on. Cannot be
     *             \c NO_UNIT.
     */
    void refresh(const UnitID unit) {
        treeView.clearItems();
        array<string> firstItem;
        populateTreeView(unit, array<string>(), @firstItem);
        treeView.setSelectedItem(firstItem);
    }

    /**
     * Connects all the signal handlers that update the state of the map.
     */
    private void connectSignalHandlers() {
        hpLayout.connectEditBox(SingleSignalHandler(this.hpChanged));
        fuelLayout.connectEditBox(SingleSignalHandler(this.fuelChanged));
        for (uint64 i = 0, len = ammoLayouts.length(); i < len; ++i)
            ammoLayouts[i].connectEditBoxMulti(
                MultiSignalHandler(this.ammoChanged));
        waiting.connectCheckBox(SingleSignalHandler(this.waitingChanged));
        capturing.connectCheckBox(SingleSignalHandler(this.capturingChanged));
        hiding.connectCheckBox(SingleSignalHandler(this.hidingChanged));
    }

    /**
     * Disconnects all the signal handlers that update the state of the map.
     */
    private void disconnectSignalHandlers() {
        hpLayout.connectEditBox(null);
        fuelLayout.connectEditBox(null);
        // Technically we shouldn't have to care about the ammo layouts here but
        // we should do it anyway for completeness.
        for (uint64 i = 0, len = ammoLayouts.length(); i < len; ++i)
            ammoLayouts[i].connectEditBoxMulti(null);
        waiting.connectCheckBox(null);
        capturing.connectCheckBox(null);
        hiding.connectCheckBox(null);
    }

    /**
     * Sets up a bitmap button.
     * @param group   Add the button to this group, and initialise its padding.
     * @param button  The button to set up.
     * @param text    The text to assign to the button.
     * @param icon    The icon to assign to the button.
     * @param handler When the button is pressed, invoke this handler.
     */
    private void setupBitmapButton(Group@ const group, BitmapButton@ const button,
        const string&in text, const string&in icon,
        SingleSignalHandler@ const handler) {
        button.setSize("100%", "100%");
        button.setText(text);
        button.setSprite("icon", icon);
        button.connect(Clicked, handler);
        group.add(button);
        group.setPadding("5");
    }

    /**
     * Recursively populate the tree view.
     * @param unit      The ID of the unit to create an item for.
     * @param hierarchy The hierarchy of this item's parent.
     * @param firstItem Handle to an array that will store the first item.
     */
    private void populateTreeView(const UnitID unit,
        array<string>@ const hierarchy, array<string>@ const firstItem) {
        // Item string format:
        // #ID UnitTypeShortName # HP # Fuel [Hidden]
        const auto item = "#" + formatUnitID(unit) + " " +
            translate(edit.map.getUnitType(unit).shortName) + " " +
            formatHP(edit.map.getUnitDisplayedHP(unit)) + " " + translate("hp") +
            " " + formatFuel(edit.map.getUnitFuel(unit)) + " " +
            translate("fuel") + (edit.map.isUnitHiding(unit) ? " " +
                translate("hidden") : "");
        if (firstItem.isEmpty()) firstItem.insertLast(item);
        hierarchy.insertLast(item);
        treeView.addItem(hierarchy);
        const auto loaded = edit.map.getLoadedUnits(unit);
        for (uint i = 0, len = loaded.length(); i < len; ++i)
            populateTreeView(loaded[i], hierarchy, firstItem);
        hierarchy.removeLast();
    }

    /**
     * A unit has been selected from the tree view, or it has been deselected.
     * In case of deselection, don't do anything.
     */
    private void treeViewItemSelected() {
        const auto selectedItem = treeView.getSelectedItem();
        if (selectedItem.isEmpty()) return;
        disconnectSignalHandlers();

        // Find ID of unit from the item text.
        const auto leaf = selectedItem[selectedItem.length() - 1];
        const auto hash = leaf.findFirst("#");
        const auto space = leaf.findFirst(" ", hash);
        const auto unit = parseUnitID(leaf.substr(hash + 1, space - hash - 1));
        currentUnit = unit;
        const auto unitType = edit.map.getUnitType(unit);
        const auto unitArmy = edit.map.getArmyOfUnit(unit);

        // Update picture, name, and owner.
        picture.setSprite("unitPicture", unitType.pictureSprite(unitArmy));
        unitName.setText(unitType.name);
        unitOwner.setText(country[country.scriptNames[unitArmy]].name);

        // Update HP and fuel.
        hpLayout.setEditBoxText(uint(edit.map.getUnitHP(unit)));
        hpLayout.setDynamicText(unitType.maxHP);
        fuelLayout.setEditBoxEnabled(!unitType.hasInfiniteFuel);
        if (unitType.hasInfiniteFuel) {
            fuelLayout.setEditBoxText("");
            fuelLayout.setDynamicText("inf");
        } else {
            fuelLayout.setEditBoxText(uint(edit.map.getUnitFuel(unit)));
            fuelLayout.setDynamicText(uint(unitType.maxFuel));
        }
        
        // Remove all ammo layouts that currently exist, but don't delete them. If
        // we do, there will be nasty crashes when we invoke ammoChanged(). I
        // guess it's more efficient that we don't keep deleting and readding the
        // same widgets every refresh.
        uint64 ammoLayoutCount = ammoLayouts.length();
        for (uint64 i = 0; i < ammoLayoutCount; ++i) base.remove(ammoLayouts[i]);

        // Add new ammo layouts based on the weapons of the current unit's type.
        ammoLayoutCount = unitType.weaponCount;
        for (uint64 i = 0; i < ammoLayoutCount; ++i) {
            const auto weapon = unitType.weapon(i);
            if (i >= ammoLayouts.length())
                ammoLayouts.insertLast(PropertyLayout(weapon.shortName));
            else
                ammoLayouts[i].setStaticText(weapon.shortName);
            ammoLayouts[i].setName(formatUInt(i));
            ammoLayouts[i].setEditBoxEnabled(!weapon.hasInfiniteAmmo);
            if (weapon.hasInfiniteAmmo) {
                ammoLayouts[i].setEditBoxText("");
                ammoLayouts[i].setDynamicText("inf");
            } else {
                ammoLayouts[i].setEditBoxText(
                    uint(edit.map.getUnitAmmo(unit, weapon.scriptName)));
                ammoLayouts[i].setDynamicText(uint(weapon.maxAmmo));
            }
            base.add(ammoLayouts[i]);
            base.setIndexOfWidget(base.widgetCount() - 1,
                ammoLayoutFirstIndex + i);
        }

        // Update toggle properties.
        waiting.setChecked(edit.map.isUnitWaiting(unit));
        capturing.setChecked(edit.map.isUnitCapturing(unit));
        hiding.setChecked(edit.map.isUnitHiding(unit));

        // If the unit has already met its load limit, disable the create button.
        createAndLoad.setEnabled(
            edit.map.getLoadedUnits(unit).length() < unitType.loadLimit);
        
        // Update static properties.
        loadLimit.setText("~" + formatUInt(unitType.loadLimit));
        movementType.setText(unitType.movementType.name);
        movementPoints.setText("~" + formatUInt(unitType.movementPoints));
        vision.setText("~" + formatUInt(unitType.vision));
        priceLayout.setText("price", { any(unitType.cost) });
        
        // Finally, adjust the height of the base layout based on the number of
        // ammo layouts there are.
        base.setSize("", UnitPropertiesPanelConstants::MinimumHeight +
            "+FuelLayout.height*" + formatUInt(ammoLayoutCount));
        connectSignalHandlers();
    }

    /**
     * The Create and Load Unit button has been pressed.
     */
    private void createAndLoadUnit() {
        if (selectedUnitType.type is null) {
            mapMaker.openMessageBox(null, { "ok" }, "pleaseselectunittype");
            return;
        }
        const auto currentUnitType = edit.map.getUnitType(currentUnit);
        if (!currentUnitType.canLoad[selectedUnitType.type.scriptName]) {
            mapMaker.openMessageBox(null, { "ok" }, "invalidunittypeforload",
                { any(translate(selectedUnitType.type.name)),
                    any(translate(currentUnitType.name)) });
            return;
        }
        edit.createAndLoadUnit(currentUnit, selectedUnitType.type);
    }

    /**
     * The Delete Unit button has been pressed.
     */
    private void deleteUnit() {
        edit.deleteUnit(currentUnit);
    }

    /**
     * Invoked when the current unit's HP has been changed by the user.
     */
    private void hpChanged() {
        edit.setUnitHP(currentUnit, HP(hpLayout.getEditBoxText()));
    }

    /**
     * Invoked when the current unit's fuel has been changed by the user.
     */
    private void fuelChanged() {
        edit.setUnitFuel(currentUnit, Fuel(fuelLayout.getEditBoxText()));
    }

    /**
     * Invoked when one of the current unit's ammos has been changed by the user.
     * @param editbox This widget's name is the ID of the \c ammoLayouts
     *                corresponding to the layout that contains the edit box that
     *                emitted the signal.
     * @param signal  Only respond to the \c ReturnOrUnfocused signal.
     */
    private void ammoChanged(const WidgetID editbox, const string&in signal) {
        if (signal != ReturnOrUnfocused) return;
        const auto ammoID = parseUInt(::getWidgetName(editbox));
        const auto temp = ammoLayouts[ammoID];
        edit.setUnitAmmo(currentUnit, ammoID, Ammo(temp.getEditBoxText()));
    }

    /**
     * Invoked when the current unit's waiting flag has been changed by the user.
     */
    private void waitingChanged() {
        edit.waitUnit(currentUnit, waiting.getChecked());
    }

    /**
     * Invoked when the current unit's capturing flag has been changed by the
     * user.
     */
    private void capturingChanged() {
        edit.unitCapturing(currentUnit, capturing.getChecked());
    }

    /**
     * Invoked when the current unit's hiding flag has been changed by the user.
     */
    private void hidingChanged() {
        edit.unitHiding(currentUnit, hiding.getChecked());
    }

    /**
     * Cache of the ID of the last selected unit.
     */
    private UnitID currentUnit;

    /**
     * ID of the panel attached to the tab container, that's managed by the
     * engine.
     */
    private WidgetID panelID;

    /**
     * The scrollabel panel the base layout will be added to.
     */
    private ScrollablePanel scrollablePanel;

    /**
     * The base layout.
     */
    private VerticalLayout base;

    /**
     * Contains the unit tree view.
     */
    private Group treeViewGroup;

    /**
     * The tree view containing the unit on the current tile, as well as all of
     * the units loaded onto it, directly or indirectly.
     */
    private TreeView treeView;

    /**
     * The group containing the unit picture.
     */
    private Group pictureGroup;

    /**
     * The unit picture.
     */
    private Picture picture;

    /**
     * The group containing the unit type's name.
     */
    private Group unitNameGroup;

    /**
     * The name of the type of unit.
     */
    private Label unitName;

    /**
     * Displays the unit's army.
     */
    private ReadOnlyPropertyLayout unitOwner("army");

    /**
     * Contains HP information.
     */
    private PropertyLayout hpLayout("hp");

    /**
     * Contains fuel information.
     */
    private PropertyLayout fuelLayout("fuel");

    /**
     * Stores the index that the first ammo layout should have in the base layout.
     */
    private uint64 ammoLayoutFirstIndex;

    /**
     * Contains layouts for the ammos of each weapon.
     */
    private array<PropertyLayout@> ammoLayouts;

    /**
     * Contains waiting information.
     */
    private TogglePropertyLayout waiting("waiting");

    /**
     * Contains capturing information.
     */
    private TogglePropertyLayout capturing("capturing");

    /**
     * Contains hiding information.
     */
    private TogglePropertyLayout hiding("hiding");

    /**
     * The group containing the Create and Load Unit button.
     */
    private Group createAndLoadGroup;

    /**
     * The Create and Load Unit button.
     */
    private BitmapButton createAndLoad;

    /**
     * The group containing the Delete Unit button.
     */
    private Group deleteGroup;

    /**
     * The Delete Unit button.
     */
    private BitmapButton delete;

    /**
     * Contains load limit information.
     */
    private ReadOnlyPropertyLayout loadLimit("loadlimit");

    /**
     * Contains movement type information.
     */
    private ReadOnlyPropertyLayout movementType("movement");

    /**
     * Contains movement points information.
     */
    private ReadOnlyPropertyLayout movementPoints("mp");

    /**
     * Contains vision information.
     */
    private ReadOnlyPropertyLayout vision("vision");

    /**
     * Contains price information.
     */
    private ReadOnlyPropertyLayout priceLayout("pricelabel");
}
