/**
 * @file CurrentlySelectedTileGroup.as
 * Defines code that manages the instances of \c CurrentlySelectedTile groups.
 */

namespace CurrentlySelectedTileType {
    /**
     * Signature of the callback that is invoked when a new tile type is selected.
     */
    funcdef void TileTypeSelectionUpdatedCallback();

    /**
     * A list of all the \c CurrentlySelectedTile widgets that have been added.
     */
    array<string> currentlySelectedTileWidgetList;

    /**
     * A list of all the callbacks that have been given to \c AddWidget().
     */
    array<TileTypeSelectionUpdatedCallback@> callbacks;

    /**
     * The currently selected tile type.
     */
    const TileType@ mapMakerSelectedTileType;

    /**
     * The currently selected owner.
     */
    string mapMakerSelectedOwner;

    /**
     * Adds a \c CurrentlySelectedTile widget to the given parent and registers it
     * with \c currentlySelectedTileWidgetList.
     * @param parent   Name of the parent to add the widget to.
     * @param callback Code to run when the tile type selection is updated.
     */
    void AddWidget(const string&in parent,
        TileTypeSelectionUpdatedCallback@ callback = null) {
        const auto layout =
            parent + (parent.isEmpty() ? "" : ".") + "CurSelTileLayout";
        addWidget("HorizontalLayout", layout);
        setWidgetSize(layout, "100%", "100%");
        setSpaceBetweenWidgets(layout, 10.0f);

        const auto leftGroup = layout + ".IconGroup";
        addWidget("Group", leftGroup);
        
        const auto tile = leftGroup + ".TileIcon";
        addWidget("Picture", tile);
        setWidgetOrigin(tile, 1.0f, 0.5f);
        setWidgetPosition(tile, "100%", "50%");

        const auto rightGroup = layout + ".NameGroup";
        addWidget("Group", rightGroup);

        const auto name = rightGroup + ".TileName";
        addWidget("Label", name);
        setWidgetOrigin(name, 0.0f, 0.5f);
        setWidgetPosition(name, "0%", "50%");

        currentlySelectedTileWidgetList.insertLast(layout);
        if (!(callback is null)) callbacks.insertLast(@callback);
    }

    /**
     * Update the currently selected tile type.
     * @param scriptName The script name of the tile to select.
     */
    void Set(const string&in scriptName) {
        @mapMakerSelectedTileType = tiletype[scriptName];
        _Set();
    }

    /**
     * Update the currently selected owner.
     * @param scriptName The script name of the owner to select.
     */
    void SetOwner(const string&in scriptName) {
        mapMakerSelectedOwner = scriptName;
        _Set();
    }

    /**
     * Updates all registered \c CurrentlySelectedTile widgets and invokes all
     * callbacks.
     */
    void _Set() {
        if (mapMakerSelectedTileType is null) return;
        for (uint i = 0, len = currentlySelectedTileWidgetList.length(); i < len;
            ++i) {
            const auto layout = currentlySelectedTileWidgetList[i];
            if (mapMakerSelectedOwner.isEmpty()) {
                setWidgetSprite(layout + ".IconGroup.TileIcon", "tile.normal",
                    mapMakerSelectedTileType.neutralTileSprite);
            } else {
                setWidgetSprite(layout + ".IconGroup.TileIcon", "tile.normal",
                    mapMakerSelectedTileType.ownedTileSprite(
                        mapMakerSelectedOwner));
            }
            setWidgetText(layout + ".NameGroup.TileName",
                mapMakerSelectedTileType.type.name);
        }
        for (uint i = 0, len = callbacks.length(); i < len; ++i) callbacks[i]();
    }

    /**
     * Retrieve the currently selected tile type.
     * @return Handle to the tile type selected.
     */
    const TileType@ Get() {
        return mapMakerSelectedTileType;
    }

    /**
     * Retrieve the currently selected owner.
     * @return Reference to the currently selected owner.
     */
    const string& GetOwner() {
        return mapMakerSelectedOwner;
    }
}