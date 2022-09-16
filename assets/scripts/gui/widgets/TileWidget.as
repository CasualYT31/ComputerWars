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
	 * Doesn't create any widgets.
	 */
	TileWidget() {}

	/**
	 * Creates the widgets that represent a tile panel.
	 * Defaults to left alignment.
	 * @param panelName The full name of the layout which contains all of the
	 *                  other widgets.
	 */
	TileWidget(const string&in layoutName)
		explicit {
		layout = layoutName;
		addWidget("HorizontalLayout", layoutName);
		setSpaceBetweenWidgets(layoutName, NO_SPACE);
	}

	/**
	 * Removes the widgets from the menu.
	 */
	void remove() {
		removeWidget(layout);
	}

	/**
	 * Configures a tile widget to display information on a given tile.
	 * @param tilePos The location of the tile on the current map to display
	 *                information on.
	 * @param align   The alignment to set the tile widget to. Falls back on left
	 *                alignment in cases where an invalid alignment was given.
	 */
	void update(const Vector2&in tilePos, const TileWidgetAlignment align) {
		removeWidgetsFromContainer(layout);
		_panelCount = 0;
		switch (align) {
		case TileWidgetAlignment::Left:
			_addTilePropertyPanel(tilePos);
			_addUnitPropertyPanels(tilePos, align);
			_addCurve(align);
			break;
		case TileWidgetAlignment::Right:
			_addCurve(align);
			_addUnitPropertyPanels(tilePos, align);
			_addTilePropertyPanel(tilePos);
			break;
		default:
			warn("Attempted to set an alignment of " + formatInt(align) + " to "
				"a TileWidget \"" + layout + "\", which is invalid. Falling back "
				"on a left alignment.");
			update(tilePos, TileWidgetAlignment::Left);
			return;
		}
		// Fix sizes.
		setWidgetSize(layout, "13+" +
			formatUInt(_panelCount) + "*" + PROPERTY_PANEL_WIDTH,
			PROPERTY_PANEL_HEIGHT);
	}

	/**
	 * Name of the tile's base layout widget.
	 */
	string layout;

	private uint _panelCount = 0;

	private void _addTilePropertyPanel(const Vector2&in pos) {
		const Tile tileType = game.getTileType(pos);
		const Terrain terrainType = game.getTerrainOfTile(pos);
		PropertyPanel panel(layout + ".tile");
		++_panelCount;
		const auto armyID = game.getTileOwner(pos);
		if (armyID == NO_ARMY) {
			panel.setIcon("tile.normal", tileType.neutralTileSprite);
		} else {
			panel.setIcon("tile.normal",
				tileType.ownedTileSprite[game.getArmyCountry(armyID).ID]);
		}
		panel.setName(terrainType.shortName);
		uint index = 0;
		if (terrainType.maxHP > 0) {
			panel.setProperty(index++, "~" + formatInt(game.getTileHP(pos)),
				"icon", "hp");
		}
		panel.setProperty(index++, "~" + formatUInt(terrainType.defence), "icon",
			"defstar");
	}

	private void _addUnitPropertyPanels(const Vector2&in pos,
		const TileWidgetAlignment align) {
		const UnitID unitID = game.getUnitOnTile(pos);
		if (unitID > 0) _addUnitsPropertyPanel(unitID, align, true);
	}

	private void _addUnitsPropertyPanel(const UnitID id,
		const TileWidgetAlignment align, const bool loaded) {
		const Unit unitType = game.getUnitType(id);
		PropertyPanel panel(layout + ".unit" + formatUInt(id));
		++_panelCount;
		panel.setIcon("unit", unitType.unitSprite[
			game.getArmyCountry(game.getArmyOfUnit(id)).ID
		]);
		panel.setName(unitType.shortName);
		uint index = 0;
		panel.setProperty(index++, "~" +
			formatInt(getDisplayedHP(game.getUnitHP(id))), "icon", "hp");
		panel.setProperty(index++, "~" +
			formatInt(game.getUnitFuel(id)), "icon", "fuel");
		panel.setProperty(index++, "~" +
			formatInt(game.getUnitAmmo(id)), "icon", "ammo");
		if (loaded) {
			const array<UnitID> units = game.getLoadedUnits(id);
			for (int64 i =
				(align == TileWidgetAlignment::Right ? units.length() - 1 :
					0);
				(align == TileWidgetAlignment::Right ? i >= 0 :
					i < units.length());
				(align == TileWidgetAlignment::Right ? --i :
					++i)) {
				_addUnitsPropertyPanel(units[i], align, false);
			}
		}
	}

	private void _addCurve(const TileWidgetAlignment align) {
		addWidget("Picture", layout + ".curve");
		if (align == TileWidgetAlignment::Left) {
			setWidgetSprite(layout + ".curve", "icon", "curveLeftAlign");
		} else if (align == TileWidgetAlignment::Right) {
			setWidgetSprite(layout + ".curve", "icon", "curveRightAlign");
		}
		matchWidgetSizeToSprite(layout + ".curve", true);
	}
}