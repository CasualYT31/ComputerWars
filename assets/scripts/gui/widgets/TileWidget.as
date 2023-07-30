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
 * Used to create the widgets that represent a tile panel in-game.
 */
class TileWidget {
	/**
	 * Name of the tile widget's base layout.
	 */
	string layout;

	/**
	 * Doesn't create any widgets.
	 */
	TileWidget() {}

	/**
	 * Creates the layout that the property panels will be added to.
	 * @param layoutName The full name of the layout which will contain all of the
	 *                   other widgets.
	 */
	TileWidget(const string&in layoutName) explicit {
		layout = layoutName;
		addWidget("HorizontalLayout", layout);
		setSpaceBetweenWidgets(layout, NO_SPACE);
		// Setup the property panels that will always exist, plus any widgets
		// within those panels that will always exist.
		_panels.insertLast(PropertyPanel(layout + ".tile"));
		_panels[0].setPropertyIcon(0, "icon", "hp");
		_panels[0].setPropertyVisibility(0, false);
		_panels[0].setPropertyIcon(1, "icon", "defstar");
		addWidget("Picture", layout + ".curve");
		// Default to left alignment.
		_alignment = TileWidgetAlignment::Left;
		setAlignment(TileWidgetAlignment::Left);
		_fixTileWidgetSize();
	}

	/**
	 * Choose which alignment to use.
	 * Reverses the panels within the layout and switches the curve picture used.
	 * @param align Which alignment to use.
	 */
	void setAlignment(const TileWidgetAlignment align) {
		bool dontReorder = align == _alignment;
		_alignment = align;
		// Check alignment and fix.
		switch (align) {
		case TileWidgetAlignment::Left:
		case TileWidgetAlignment::Right:
			break;
		default:
			warn("Attempted to set tile widget \"" + layout + "\"'s alignment to " +
				formatInt(align) + ", which is invalid. Falling back on left "
				"alignment.");
			setAlignment(TileWidgetAlignment::Left);
			return;
		}
		// Reverse panel order.
		// N.B.: there will be # of panels + 1 (curve) to deal with.
		if (!dontReorder) {
			for (uint i = 0; i < uint((_panels.length() + 1) / 2); ++i) {
				setWidgetIndexInContainer(layout, i, _panels.length() - i);
				setWidgetIndexInContainer(layout, _panels.length() - 1 - i, i);
			}
		}
		// Adjust curve.
		switch (align) {
		case TileWidgetAlignment::Left:
			setWidgetSprite(layout + ".curve", "icon", "curveLeftAlign");
			setWidgetRatioInLayout(layout, 0, 1.0);
			setWidgetRatioInLayout(layout, _panels.length(), _CURVE_RATIO);
			break;
		case TileWidgetAlignment::Right:
			setWidgetSprite(layout + ".curve", "icon", "curveRightAlign");
			setWidgetRatioInLayout(layout, _panels.length(), 1.0);
			setWidgetRatioInLayout(layout, 0, _CURVE_RATIO);
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
		if (unitID > 0 &&
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
		if (unitIDs.length() > _panels.length() - 1) {
			for (uint i = 0, until = unitIDs.length() - (_panels.length() - 1);
				i < until; ++i) {
				// Set current curve widget's ratio back to 1.
				switch (_alignment) {
				case TileWidgetAlignment::Left:
					setWidgetRatioInLayout(layout, _panels.length(), 1.0);
					break;
				case TileWidgetAlignment::Right:
					setWidgetRatioInLayout(layout, 0, 1.0);
					break;
				}
				// Append new property panel.
				string panelName = layout + ".unit" +
					formatUInt(_panels.length() - 1);
				PropertyPanel panel = PropertyPanel(panelName);
				panel.setPropertyIcon(0, "icon", "hp");
				panel.setPropertyIcon(1, "icon", "fuel");
				panel.setPropertyIcon(2, "icon", "ammo");
				_panels.insertLast(panel);
				// Move new panel depending on alignment, and reassign curve ratio.
				switch (_alignment) {
				case TileWidgetAlignment::Left:
					setWidgetIndex(panelName, _panels.length() - 1);
					setWidgetRatioInLayout(layout, _panels.length(), _CURVE_RATIO);
					break;
				case TileWidgetAlignment::Right:
					setWidgetIndex(panelName, 1);
					setWidgetRatioInLayout(layout, 0, _CURVE_RATIO);
					break;
				}
			}
		}

		_updateTilePanel(tilePos);
		_updateUnitPanels(unitIDs, unitTypes);
	}

	/**
	 * Updates the tile property panel with tile information.
	 */
	private void _updateTilePanel(const Vector2&in tilePos) {
        // If the given tile is out of bounds, update with default information.
        if (game.map.isOutOfBounds(tilePos)) {
            _panels[0].setIcon("", "");
            _panels[0].setName("~");
            _panels[0].setPropertyText(0, "~");
            _panels[0].setPropertyText(1, "~");
            _panels[0].setPropertyVisibility(0, false);
        } else {
            const auto tileType = game.map.getTileType(tilePos);
            const auto terrainType = tileType.type;
            const ArmyID tileOwner = game.map.getTileOwner(tilePos);
            if (tileOwner == NO_ARMY) {
                _panels[0].setIcon("tile.normal",
                    tileType.neutralTileSprite);
            } else {
                _panels[0].setIcon("tile.normal",
                    tileType.ownedTileSprite(
                        game.map.getArmyCountry(tileOwner).turnOrder));
            }
            _panels[0].setName(terrainType.shortName);
            _panels[0].setPropertyText(0,
                "~" + formatInt(game.map.getTileHP(tilePos)));
            _panels[0].setPropertyText(1,
                "~" + formatUInt(terrainType.defence));
            _panels[0].setPropertyVisibility(0, terrainType.maxHP > 0);
        }
	}

	/**
	 * Updates the unit property panels with information on each unit.
	 */
	private void _updateUnitPanels(const array<UnitID>@ unitIDs,
		const array<const UnitType@>@ unitTypes) {
		_panelsThatAreShowing = 1;
		for (uint i = 1; i < unitIDs.length() + 1; ++i) {
			++_panelsThatAreShowing;
			const UnitID unitID = unitIDs[i - 1];
			const UnitType@ unitType = unitTypes[i - 1];
			_panels[i].setIcon("unit", unitType.unitSprite(
				game.map.getArmyCountry(game.map.getArmyOfUnit(unitID)).turnOrder
			));
			_panels[i].setName(unitType.shortName);
			_panels[i].setPropertyText(0, "~" +
				formatInt(game.map.getUnitDisplayedHP(unitID)));
			_panels[i].setPropertyText(1, "~" +
				formatInt(game.map.getUnitFuel(unitID)));
			// TODO-1 {
			if (unitType.weaponCount > 0 && !unitType.weapon(0).hasInfiniteAmmo) {
				_panels[i].setPropertyText(2, "~" +
					formatInt(game.map.getUnitAmmo(unitID,
						unitType.weapon(0).scriptName)));
			} else {
				_panels[i].setPropertyText(2, "~0");
			}
			// }
			setWidgetVisibility(_panels[i].layout, true);
			switch (_alignment) {
			case TileWidgetAlignment::Left:
				setWidgetRatioInLayout(layout, i, 1.0);
				break;
			case TileWidgetAlignment::Right:
				setWidgetRatioInLayout(layout, _panels.length() - i, 1.0);
				break;
			}
		}
		// For any remaining unit panels, hide them.
		if (unitIDs.length() < _panels.length() - 1) {
			for (uint i = unitIDs.length() + 1; i < _panels.length(); ++i) {
				setWidgetVisibility(_panels[i].layout, false);
				switch (_alignment) {
				case TileWidgetAlignment::Left:
					setWidgetRatioInLayout(layout, i, 0.0);
					break;
				case TileWidgetAlignment::Right:
					setWidgetRatioInLayout(layout, _panels.length() - i, 0.0);
					break;
				}
			}
		}
		_fixTileWidgetSize();
	}

	/**
	 * Fix the widget size.
	 */
	private void _fixTileWidgetSize() {
		// 13 here is the unscaled width of the curve picture.
		setWidgetSize(layout, "13+" + formatUInt(_panelsThatAreShowing) +
			"*" + PROPERTY_PANEL_WIDTH, PROPERTY_PANEL_HEIGHT);
	}

	/**
	 * Holds the property panels that are currently allocated.
	 */
	private array<PropertyPanel> _panels;

	/**
	 * The alignment of the tile widget.
	 */
	private TileWidgetAlignment _alignment;

	/**
	 * Tracks the number of panels that are showing to the user.
	 */
	private uint _panelsThatAreShowing = 1;

	/**
	 * The ratio that the curve picture widget should have.
	 */
	private float _CURVE_RATIO = 0.2167;
}
