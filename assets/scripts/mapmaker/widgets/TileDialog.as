/**
 * @file TileDialog.as
 * Defines code that manages the \c TileDialog window.
 */

namespace tile_dialog_internal {
    /**
     * The default width of the \c TileDialog.
     */
    string DEFAULT_WIDTH;

    /**
     * The name of the owner combobox.
     */
    string COMBOBOX;

    /**
     * The name of the owner icon.
     */
    string ICON;

    /**
     * List of tile buttons which have owned tile sprites.
     * Keyed on \c BitmapButton widget name, values store corresponding
     * <tt>const tiletype@</tt> handles.
     */
    dictionary BUTTONS_WITH_OWNED_SPRITES;
}

/**
 * Opens a \c TileDialog and docks it to the left of its parent.
 * @param dialog Name of the \c TileDialog to dock.
 */
void DockTileDialog(const string&in dialog) {
    openChildWindow(dialog, "100%-" + tile_dialog_internal::DEFAULT_WIDTH, "0");
    setWidgetSize(dialog, tile_dialog_internal::DEFAULT_WIDTH, "100%");
}

/**
 * Refreshes the tiles grid with a new owner.
 * @param owner The script name of the owner whose tile sprites to use. If neutral
 *              is desired, provide an empty string.
 */
void RefreshTileDialogGrid(const string&in owner = "") {
    const auto@ btns = tile_dialog_internal::BUTTONS_WITH_OWNED_SPRITES.getKeys();
    for (uint i = 0, len = btns.length(); i < len; ++i) {
        const auto key = btns[i];
        const TileType@ val;
        tile_dialog_internal::BUTTONS_WITH_OWNED_SPRITES.get(btns[i], @val);
        setWidgetSprite(key, "tile.normal", (owner.isEmpty() ?
            val.neutralTileSprite : val.ownedTileSprite(owner)));
    }
}

/**
 * Sets up the \c TileDialog group and window.
 * @param  parent The parent for the \c TileDialog.
 * @return The name of the \c TileDialog's \c ChildWindow.
 */
string TileDialogSetUp(const string&in parent = "") {
    const auto window = parent + (parent.isEmpty() ? "" : ".") + "TileDialog";
    addWidget("ChildWindow", window);
    setWidgetText(window, "tiledialog");

    // CurrentlySelectedTileWidget //

    const auto currentTileGroup = window + ".CurrentTileGroup";
    const auto GROUP_HEIGHT = "35px";
    addWidget("Group", currentTileGroup);
    setWidgetSize(currentTileGroup, "100%", GROUP_HEIGHT);
    setWidgetOrigin(currentTileGroup, 0.5f, 0.0f);
    setWidgetPosition(currentTileGroup, "50%", "0%");

    CurrentlySelectedTileType::AddWidget(currentTileGroup);

    // ScrollablePanel //

    const auto scrollablePanel = window + ".ScrollablePanel";
    addWidget("ScrollablePanel", scrollablePanel);
    setWidgetSize(scrollablePanel, "100%", "100%-" + GROUP_HEIGHT + "-" +
        GROUP_HEIGHT);
    setWidgetPosition(scrollablePanel, "50%", "50%");
    setWidgetOrigin(scrollablePanel, 0.5f, 0.5f);
    setVerticalScrollbarAmount(scrollablePanel, 25);

    const auto grid = scrollablePanel + ".Grid";
    addWidget("Grid", grid);
    const uint COLUMNS = 6;
    const auto@ tileScriptNames = tiletype.scriptNames;
    float WIDTH_OF_CHILD_WINDOW = 45.0f;
    for (uint t = 0, typeCount = tileScriptNames.length(); t < typeCount; ++t) {
        const auto tileName = tileScriptNames[t];
        const auto@ tile = tiletype[tileName];
        const auto ROW = uint(t / COLUMNS), COL = t % COLUMNS;
        const auto btn = grid + "." + tileName;
        addWidgetToGrid("BitmapButton", btn, ROW, COL,
            "TileDialogHandleBitmapButtonSignal");
        setWidgetSprite(btn, "tile.normal", tile.neutralTileSprite);
        setWidgetAlignmentInGrid(grid, ROW, COL, WidgetAlignment::Centre);
        if (tile.hasOwnedTiles)
            tile_dialog_internal::BUTTONS_WITH_OWNED_SPRITES.set(btn, @tile);
        if (ROW == 0) WIDTH_OF_CHILD_WINDOW += getWidgetFullSize(btn).x;
    }

    // SelectOwner widgets //

    const auto selectOwnerGroup = window + ".SelectOwnerGroup";
    const auto GROUP_PADDING = "5px";
    addWidget("Group", selectOwnerGroup);
    setWidgetSize(selectOwnerGroup, "100%", GROUP_HEIGHT);
    setWidgetOrigin(selectOwnerGroup, 0.5f, 1.0f);
    setWidgetPosition(selectOwnerGroup, "50%", "100%");

    const auto ownerIconGroup = selectOwnerGroup + ".OwnerIconGroup";
    addWidget("Group", ownerIconGroup);
    setWidgetSize(ownerIconGroup, GROUP_HEIGHT, "100%");
    setGroupPadding(ownerIconGroup, "5px");

    tile_dialog_internal::ICON = ownerIconGroup + ".OwnerIcon";
    addWidget("Picture", tile_dialog_internal::ICON);
    setWidgetOrigin(tile_dialog_internal::ICON, 0.5f, 0.5f);
    setWidgetPosition(tile_dialog_internal::ICON, "50%", "50%");

    const auto ownerComboboxGroup = selectOwnerGroup + ".OwnerComboboxGroup";
    addWidget("Group", ownerComboboxGroup);
    setWidgetSize(ownerComboboxGroup, "100%-" + GROUP_HEIGHT, "100%");
    setWidgetPosition(ownerComboboxGroup, GROUP_HEIGHT, "0px");
    setGroupPadding(ownerComboboxGroup, "5px");

    tile_dialog_internal::COMBOBOX = ownerComboboxGroup + ".OwnerCombobox";
    addWidget("ComboBox", tile_dialog_internal::COMBOBOX);
    setWidgetSize(tile_dialog_internal::COMBOBOX, "100%", "100%");
    addItem(tile_dialog_internal::COMBOBOX, "neutral");
    setSelectedItem(tile_dialog_internal::COMBOBOX, 0);
    const auto@ countryScriptNames = country.scriptNames;
    for (uint c = 0, len = countryScriptNames.length(); c < len; ++c) {
        addItem(tile_dialog_internal::COMBOBOX,
            country[countryScriptNames[c]].name);
    }

    // Calculate how wide the window has to be to hide the horizontal scrollbar by
    // default.
    tile_dialog_internal::DEFAULT_WIDTH = formatFloat(WIDTH_OF_CHILD_WINDOW +
        5.0f);
    DockTileDialog(window);
    return window;
}

/**
 * Signal handler for tile buttons.
 * @param widgetName Full name of the button.
 * @param signal     The signal that was emitted.
 */
void TileDialogHandleBitmapButtonSignal(const string&in widgetName,
    const string&in signal) {
    if (signal == "Pressed") {
        CurrentlySelectedTileType::Set(
            widgetName.substr(widgetName.findLast(".") + 1));
    }
}

/**
 * Updates the \c OwnerIcon with the icon of each country.
 */
void MapMakerMenu_OwnerCombobox_ItemSelected() {
    const auto itemID = getSelectedItem(tile_dialog_internal::COMBOBOX);
    if (itemID <= 0) {
        clearWidgetSprite(tile_dialog_internal::ICON);
        RefreshTileDialogGrid();
        CurrentlySelectedTileType::SetOwner("");
    } else {
        const auto c = country.scriptNames[itemID - 1];
        setWidgetSprite(tile_dialog_internal::ICON, "icon", country[c].iconName);
        RefreshTileDialogGrid(c);
        CurrentlySelectedTileType::SetOwner(c);
    }
}
