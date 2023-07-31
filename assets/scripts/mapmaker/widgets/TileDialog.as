/**
 * @file TileDialog.as
 * Defines code that manages the \c TileDialog window.
 */

namespace tile_dialog_internal {
    /**
     * The name of the owner combobox.
     */
    string COMBOBOX;

    /**
     * The name of the owner icon.
     */
    string ICON;

    /**
     * The name of the \c ScrollablePanel that contains the wrap.
     */
    string PANEL;

    /**
     * The name of the \c HorizontalWrap that contains the buttons.
     */
    string WRAP;

    /**
     * Width of each tile button.
     */
    const float BUTTON_WIDTH = 35.0f;

    /**
     * HEIGHT of each tile button.
     */
    const float BUTTON_HEIGHT = 35.0f;

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
    const auto defaultWidth = formatFloat(
        tile_dialog_internal::BUTTON_WIDTH * 6.0f +
        getScrollbarWidth(tile_dialog_internal::PANEL) + 2.0f
    );
    openChildWindow(dialog, "100%-" + defaultWidth, "0");
    setWidgetSize(dialog, defaultWidth, "100%");
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

    tile_dialog_internal::PANEL = window + ".ScrollablePanel";
    addWidget("ScrollablePanel", tile_dialog_internal::PANEL);
    setWidgetSize(tile_dialog_internal::PANEL, "100%", "100%-" + GROUP_HEIGHT +
        "-" + GROUP_HEIGHT);
    setWidgetPosition(tile_dialog_internal::PANEL, "50%", "50%");
    setWidgetOrigin(tile_dialog_internal::PANEL, 0.5f, 0.5f);
    setVerticalScrollbarAmount(tile_dialog_internal::PANEL, 25);
    setHorizontalScrollbarPolicy(tile_dialog_internal::PANEL,
        ScrollbarPolicy::Never);

    tile_dialog_internal::WRAP = tile_dialog_internal::PANEL + ".TileDialogWrap";
    addWidget("HorizontalWrap", tile_dialog_internal::WRAP);
    const auto@ tileScriptNames = tiletype.scriptNames;
    for (uint t = 0, len = tileScriptNames.length(); t < len; ++t) {
        const auto tileName = tileScriptNames[t];
        const auto@ tile = tiletype[tileName];
        const auto btn = tile_dialog_internal::WRAP + "." + tileName;
        addWidget("BitmapButton", btn, "TileDialogHandleBitmapButtonSignal");
        setWidgetSprite(btn, "tile.normal", tile.neutralTileSprite);
        // TODO-4: We should really find the tallest tile sprite and base the
        // button sizes on that.
        setWidgetSize(btn, formatFloat(tile_dialog_internal::BUTTON_WIDTH),
            formatFloat(tile_dialog_internal::BUTTON_HEIGHT));
        if (tile.hasOwnedTiles)
            tile_dialog_internal::BUTTONS_WITH_OWNED_SPRITES.set(btn, @tile);
    }
    MapMakerMenu_TileDialogWrap_MouseEntered();

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

    DockTileDialog(window);
    return window;
}

/**
 * When the tile dialog window has changed size, we need to update the Wrap's
 * height.
 * Would've loved to register this with the \c ChildWindow's SizeChanged signal,
 * but it's super slow as it gets triggered whilst the window is still being
 * dragged.
 */
void MapMakerMenu_TileDialogWrap_MouseEntered() {
    // Figure out how many rows of buttons there are and resize the wrap to fit.
    const auto widthOfPanel = getWidgetFullSize(tile_dialog_internal::PANEL).x;
    auto columns = widthOfPanel / tile_dialog_internal::BUTTON_WIDTH;
    if (columns < 1.0f) columns = 1.0f;
    const auto rows =
        ceil(getWidgetCount(tile_dialog_internal::WRAP) / floor(columns));
    setWidgetSize(tile_dialog_internal::WRAP,
        "100%-" + getScrollbarWidth(tile_dialog_internal::PANEL),
        formatFloat(rows * tile_dialog_internal::BUTTON_HEIGHT)
    );
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
