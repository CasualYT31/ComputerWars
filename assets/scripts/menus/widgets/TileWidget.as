/**
 * @file TileWidget.as
 * Declares the tile widget type.
 */

/**
 * The different alignment options available to tile widgets.
 */
enum TileWidgetAlignment {
    Left,
    Right
}

/**
 * Defines constants that the \c TileWidget class uses internally.
 */
namespace TileWidgetConstants {
    /**
     * Stores the width of an ArmyWidget.
     */
    const uint Width = 250;

    /**
     * Stores the width of an ArmyWidget as a string.
     */
    const string WidthStr = formatUInt(Width);

    /**
     * The ratio that the curve picture widget should have.
     */
    const float CurveRatio = 0.2167;

    /**
     * The unscaled width of the curve sprite.
     */
    const string UnscaledCurvePictureWidth = "13";
}

/**
 * Used to create the widgets that represent a tile panel in-game.
 */
class TileWidget : HorizontalLayout {
    /**
     * Creates the layout that the property panels will be added to.
     */
    TileWidget() {
        setSpaceBetweenWidgets(NO_SPACE);
        // Setup the property panels that will always exist, plus any widgets
        // within those panels that will always exist.
        panels.resize(1);
        panels[0].setPropertyIcon(0, "icon", "hp");
        panels[0].setPropertyVisibility(0, false);
        panels[0].setPropertyIcon(1, "icon", "defstar");
        add(panels[0]);
        add(curve);
        // Default to left alignment.
        alignment = TileWidgetAlignment::Left;
        setAlignment(TileWidgetAlignment::Left);
        fixTileWidgetSize();
    }

    /**
     * Choose which alignment to use.
     * Reverses the panels within the layout and switches the curve picture used.
     * @param align Which alignment to use.
     */
    void setAlignment(const TileWidgetAlignment align) {
        bool doNotReorder = align == alignment;
        alignment = align;
        // Check alignment and fix.
        switch (align) {
        case TileWidgetAlignment::Left:
        case TileWidgetAlignment::Right:
            break;
        default:
            warn("Attempted to set tile widget \"" + ID + "\"'s alignment to " +
                formatInt(align) + ", which is invalid. Falling back on left "
                "alignment.");
            setAlignment(TileWidgetAlignment::Left);
            return;
        }
        // Reverse panel order.
        // N.B.: there will be # of panels + 1 (curve) to deal with.
        if (!doNotReorder) {
            for (uint i = 0; i < uint((panels.length() + 1) / 2); ++i) {
                setIndexOfWidget(i, panels.length() - i);
                setIndexOfWidget(panels.length() - 1 - i, i);
            }
        }
        // Adjust curve.
        switch (align) {
        case TileWidgetAlignment::Left:
            curve.setSprite("icon", "curveLeftAlign");
            setRatioOfWidget(0, 1.0);
            setRatioOfWidget(panels.length(), TileWidgetConstants::CurveRatio);
            break;
        case TileWidgetAlignment::Right:
            curve.setSprite("icon", "curveRightAlign");
            setRatioOfWidget(panels.length(), 1.0);
            setRatioOfWidget(0, TileWidgetConstants::CurveRatio);
            break;
        }
    }

    /**
     * Updates a tile widget with information on a given tile.
     * @param tilePos The location of the tile to display information on.
     */
    void update(const Vector2&in tilePos) {
        // Gather information.
        const UnitID unitID = game.map.getUnitOnTile(tilePos);
        array<UnitID> unitIDs;
        array<const UnitType@> unitTypes;
        if (unitID != NO_UNIT &&
            game.map.isUnitVisible(unitID, game.map.getSelectedArmy())) {
            // Right now, information on a tile's unit/s are not displayed if
            // `unitID` isn't visible. The hiding status of loaded units is
            // ignored. If you wish to change this behaviour (i.e. hide loaded
            // units from the tile widget if they are hidden), then you will have
            // to remove the `isUnitOnMap()` check from within `isUnitVisible()`
            // (if this is kept in, loaded units will always be considered
            // hidden).
            unitIDs = game.map.getLoadedUnits(unitID);
            unitIDs.insertAt(0, unitID);
            for (uint i = 0; i < unitIDs.length(); ++i) {
                unitTypes.resize(unitTypes.length() + 1);
                @unitTypes[unitTypes.length() - 1] =
                    game.map.getUnitType(unitIDs[i]);
            }
        }

        // If there are more units than there are allocated unit panels, then
        // allocate more.
        if (unitIDs.length() > panels.length() - 1) {
            for (uint i = 0, until = unitIDs.length() - (panels.length() - 1);
                i < until; ++i) {
                // Set current curve widget's ratio back to 1.
                switch (alignment) {
                case TileWidgetAlignment::Left:
                    setRatioOfWidget(panels.length(), 1.0);
                    break;
                case TileWidgetAlignment::Right:
                    setRatioOfWidget(0, 1.0);
                    break;
                }
                // Append new property panel.
                panels.resize(panels.length() + 1);
                panels[panels.length() - 1].setPropertyIcon(0, "icon", "hp");
                panels[panels.length() - 1].setPropertyIcon(1, "icon", "fuel");
                panels[panels.length() - 1].setPropertyIcon(2, "icon", "ammo");
                add(panels[panels.length() - 1]);
                // Move new panel depending on alignment, and reassign curve
                // ratio.
                switch (alignment) {
                case TileWidgetAlignment::Left:
                    panels[panels.length() - 1].setIndex(panels.length() - 1);
                    setRatioOfWidget(panels.length(),
                        TileWidgetConstants::CurveRatio);
                    break;
                case TileWidgetAlignment::Right:
                    panels[panels.length() - 1].setIndex(1);
                    setRatioOfWidget(0, TileWidgetConstants::CurveRatio);
                    break;
                }
            }
        }

        _updateTilePanel(tilePos);
        _updateUnitPanels(unitIDs, unitTypes);
    }

    /**
     * Updates the tile property panel with tile information.
     * @param tilePos The position of the tile to display information on.
     */
    private void _updateTilePanel(const Vector2&in tilePos) {
        // If the given tile is out of bounds, update with default information.
        if (game.map.isOutOfBounds(tilePos)) {
            panels[0].setIcon("", "");
            panels[0].setName("~");
            panels[0].setPropertyText(0, "~");
            panels[0].setPropertyText(1, "~");
            panels[0].setPropertyVisibility(0, false);
        } else {
            const auto tileType = game.map.getTileType(tilePos);
            const auto terrainType = tileType.type;
            const ArmyID tileOwner = game.map.getTileOwner(tilePos);
            if (tileOwner == NO_ARMY) {
                panels[0].setIcon(game.map.getEnvironmentSpritesheet(),
                    tileType.neutralTileSprite);
            } else {
                panels[0].setIcon(game.map.getEnvironmentSpritesheet(),
                    tileType.ownedTileSprite(
                        game.map.getArmyCountry(tileOwner).turnOrder));
            }
            panels[0].setName(terrainType.shortName);
            panels[0].setPropertyText(0,
                "~" + formatInt(game.map.getTileHP(tilePos)));
            panels[0].setPropertyText(1, "~" + formatUInt(terrainType.defence));
            panels[0].setPropertyVisibility(0, terrainType.maxHP > 0);
        }
    }

    /**
     * Updates the unit property panels with information on each unit.
     * @param unitIDs   The IDs of the units to display information on.
     * @param unitTypes The unit types corresponding to each \c unitIDs element.
     */
    private void _updateUnitPanels(const array<UnitID>@ unitIDs,
        const array<const UnitType@>@ unitTypes) {
        panelsThatAreShowing = 1;
        for (uint i = 1; i < unitIDs.length() + 1; ++i) {
            ++panelsThatAreShowing;
            const UnitID unitID = unitIDs[i - 1];
            const UnitType@ unitType = unitTypes[i - 1];
            panels[i].setIcon("unit", unitType.unitSprite(
                game.map.getArmyCountry(game.map.getArmyOfUnit(unitID)).turnOrder
            ));
            panels[i].setName(unitType.shortName);
            panels[i].setPropertyText(0, "~" +
                formatInt(game.map.getUnitDisplayedHP(unitID)));
            panels[i].setPropertyText(1, "~" +
                formatInt(game.map.getUnitFuel(unitID)));
            // TODO-1 {
            if (unitType.weaponCount > 0 && !unitType.weapon(0).hasInfiniteAmmo) {
                panels[i].setPropertyText(2, "~" + formatInt(game.map.getUnitAmmo(
                    unitID, unitType.weapon(0).scriptName)));
            } else {
                panels[i].setPropertyText(2, "~0");
            }
            // }
            panels[i].setVisibility(true);
            switch (alignment) {
            case TileWidgetAlignment::Left:
                setRatioOfWidget(i, 1.0);
                break;
            case TileWidgetAlignment::Right:
                setRatioOfWidget(panels.length() - i, 1.0);
                break;
            }
        }
        // For any remaining unit panels, hide them.
        if (unitIDs.length() < panels.length() - 1) {
            for (uint i = unitIDs.length() + 1; i < panels.length(); ++i) {
                panels[i].setVisibility(false);
                switch (alignment) {
                case TileWidgetAlignment::Left:
                    setRatioOfWidget(i, 0.0);
                    break;
                case TileWidgetAlignment::Right:
                    setRatioOfWidget(panels.length() - i, 0.0);
                    break;
                }
            }
        }
        fixTileWidgetSize();
    }

    /**
     * Fix the tile widget's size.
     */
    private void fixTileWidgetSize() {
        setSize(TileWidgetConstants::UnscaledCurvePictureWidth + "+" +
            formatUInt(panelsThatAreShowing) + "*" +
            PropertyPanelConstants::WidthStr, PropertyPanelConstants::HeightStr);
    }

    /**
     * The picture widget that displays the curve of the tile widget.
     */
    private Picture curve;

    /**
     * Holds the property panels that are currently allocated.
     * @warning Avoid using \c insertLast(). The engine crashes when attempting to
     *          copy \c PropertyPanel objects (assumption).
     */
    private array<PropertyPanel> panels;

    /**
     * The alignment of the tile widget.
     */
    private TileWidgetAlignment alignment;

    /**
     * Tracks the number of panels that are showing to the user.
     */
    private uint panelsThatAreShowing = 1;
}
