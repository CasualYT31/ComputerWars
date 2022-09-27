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
	 * @param panelName The full name of the layout which will contain all of the
	 *                  other widgets.
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
		matchWidgetSizeToSprite(layout + ".curve", true);
		// Default to left alignment.
		_dontReorder = true;
		setAlignment(TileWidgetAlignment::Left);
		_dontReorder = false;
		_fixTileWidgetSize();
	}

	/**
	 * Choose which alignment to use.
	 * Reverses the panels within the layout and switches the curve picture used.
	 * @param align Which alignment to use.
	 */
	void setAlignment(const TileWidgetAlignment align) {
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
		if (!_dontReorder) {
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
		const TileType tileType = game.getTileType(tilePos);
		const Terrain terrainType = game.getTerrainOfTile(tilePos);
		const ArmyID tileOwner = game.getTileOwner(tilePos);
		const UnitID unitID = game.getUnitOnTile(tilePos);
		array<UnitID> unitIDs;
		array<UnitType> unitTypes;
		if (unitID > 0) {
			unitIDs = game.getLoadedUnits(unitID);
			unitIDs.insertAt(0, unitID);
			for (uint i = 0; i < unitIDs.length(); ++i) {
				unitTypes.insertLast(game.getUnitType(unitIDs[i]));
			}
		}

		// If there are more units than there are allocated unit panels, then
		// allocate more.
		if (unitIDs.length() > _panels.length() - 1) {
			for (uint i = 0; i < unitIDs.length() - (_panels.length() - 1); ++i) {
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

		_updateTilePanel(tilePos, tileType, terrainType, tileOwner);
		_updateUnitPanels(unitIDs, unitTypes);
	}

	/**
	 * Updates the tile property panel with tile information.
	 */
	private void _updateTilePanel(const Vector2&in tilePos,
		const TileType&in tileType, const Terrain&in terrainType,
		const ArmyID tileOwner) {
		if (tileOwner == NO_ARMY) {
			_panels[0].setIcon("tile.normal",
				tileType.neutralTileSprite);
		} else {
			_panels[0].setIcon("tile.normal",
				tileType.ownedTileSprite[game.getArmyCountry(tileOwner).ID]);
		}
		_panels[0].setName(terrainType.shortName);
		_panels[0].setPropertyText(0, "~" + formatInt(game.getTileHP(tilePos)));
		_panels[0].setPropertyText(1, "~" + formatUInt(terrainType.defence));
		_panels[0].setPropertyVisibility(0, terrainType.maxHP > 0);
	}

	/**
	 * Updates the unit property panels with information on each unit.
	 */
	private void _updateUnitPanels(const array<UnitID>@ unitIDs,
		const array<UnitType>@ unitTypes) {
		_panelsThatAreShowing = 1;
		for (uint i = 1; i < unitIDs.length() + 1; ++i) {
			++_panelsThatAreShowing;
			const UnitID unitID = unitIDs[i - 1];
			const UnitType unitType = unitTypes[i - 1];
			_panels[i].setIcon("unit", unitType.unitSprite[
				game.getArmyCountry(game.getArmyOfUnit(unitID)).ID
			]);
			_panels[i].setName(unitType.shortName);
			_panels[i].setPropertyText(0, "~" +
				formatInt(game.getUnitHP(unitID)));
			_panels[i].setPropertyText(1, "~" +
				formatInt(game.getUnitFuel(unitID)));
			_panels[i].setPropertyText(2, "~" +
				formatInt(game.getUnitAmmo(unitID)));
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
	 * Internal flag used by the constructor to prevent setAlignment() from
	 * reordering the panels.
	 */
	private bool _dontReorder = true;

	/**
	 * The ratio that the curve picture widget should have.
	 */
	private float _CURVE_RATIO = 0.2167;






	// \/ REWRITE \/ //


	/**
	 * Configures a tile widget to display information on a given tile.
	 * @param tilePos The location of the tile on the current map to display
	 *                information on.
	 * @param align   The alignment to set the tile widget to. Falls back on left
	 *                alignment in cases where an invalid alignment was given.
	 *
	void update(const Vector2&in tilePos, const TileWidgetAlignment align) {
		if (_firstUpdateCall) {
			_firstUpdateCall = false;
			// Add all the widgets for the first time.
			_addWidgets(tilePos, align);
		} else if (_prevTilePos != tilePos || _prevAlign != align) {
			// Add all the widgets if the tile or alignment has changed.
			_removePanels();
			_addWidgets(tilePos, align);
		}

		// Update tracker variables.
		_prevTilePos = tilePos;
		_prevAlign = align;

		// Generate an appropriately ordered list of unit IDs.
		array<UnitID> unitIDs;
		const UnitID unitOnTile = game.getUnitOnTile(tilePos);
		if (unitOnTile > 0) {
			unitIDs.insertLast(unitOnTile);
			array<UnitID> loaded = game.getLoadedUnits(unitOnTile);
			for (uint i = 0; i < loaded.length(); ++i) {
				unitIDs.insertLast(loaded[i]);
			}
			if (align == TileWidgetAlignment::Right) {
				unitIDs.reverse();
			}
		}
		
		// Set panel properties.
		_setTilePanel(tilePos);
		// for (uint i = 0; i < unitIDs.length(); ++i) {
		// 	_setUnitPanel(i, unitIDs[i]);
		// }

		// Fix the size of the tile widget.
		// 13 here is the unscaled width of the curve picture.
		setWidgetSize(layout, "13+" + formatUInt(1 + _unitPanels.length()) +
			"*" + PROPERTY_PANEL_WIDTH, PROPERTY_PANEL_HEIGHT);
	}

	/**
	 * Updates a unit panel's properties.
	 * @param panel  The 0-based index identifying the unit panel to update.
	 * @param unitID The ID of the unit to display information on.
	 *
	private void _setUnitPanel(const uint panel, const UnitID unitID) {
		const Unit unitType = game.getUnitType(unitID);
		_unitPanels[panel].setIcon("unit", unitType.unitSprite[
			game.getArmyCountry(game.getArmyOfUnit(unitID)).ID
		]);
		_unitPanels[panel].setName(unitType.shortName);
		uint index = 0;
		_unitPanels[panel].setProperty(index++, "~" +
			formatInt(game.getUnitHP(unitID)), "icon", "hp");
		_unitPanels[panel].setProperty(index++, "~" +
			formatInt(game.getUnitFuel(unitID)), "icon", "fuel");
		_unitPanels[panel].setProperty(index++, "~" +
			formatInt(game.getUnitAmmo(unitID)), "icon", "ammo");
	}

	/**
	 * Updates the tile panel's properties.
	 * @param tilePos Location of the tile to display information on.
	 *
	private void _setTilePanel(const Vector2&in tilePos) {
		const Tile tileType = game.getTileType(tilePos);
		const Terrain terrainType = game.getTerrainOfTile(tilePos);
		const ArmyID tileOwner = game.getTileOwner(tilePos);
		if (tileOwner == NO_ARMY) {
			_tilePanel.setIcon("tile.normal", tileType.neutralTileSprite);
		} else {
			_tilePanel.setIcon("tile.normal",
				tileType.ownedTileSprite[game.getArmyCountry(tileOwner).ID]);
		}
		_tilePanel.setName(terrainType.shortName);
		uint index = 0;
		if (terrainType.maxHP > 0) {
			_tilePanel.setProperty(index++, "~" +
				formatInt(game.getTileHP(tilePos)), "icon", "hp");
		}
		_tilePanel.setProperty(index++, "~" + formatUInt(terrainType.defence),
			"icon", "defstar");
	}

	/**
	 * Adds all the widgets necessary to display information on a given tile.
	 * Also sets static properties, such as the picture that the curve panel
	 * displays.
	 *
	private void _addWidgets(const Vector2&in tilePos,
		const TileWidgetAlignment align) {
		const UnitID unitID = game.getUnitOnTile(tilePos);
		// Add widgets in a different order depending on the alignment.
		if (align == TileWidgetAlignment::Right) {
			_addCurve(align);
			_addLoadedUnitPropertyPanels(unitID, align);
			_addUnitPropertyPanel(unitID);
			_addTilePropertyPanel();
		} else {
			_addTilePropertyPanel();
			_addTilePropertyPanel(".2");
			_addTilePropertyPanel(".3");
			// _addUnitPropertyPanel(unitID);
			// _addLoadedUnitPropertyPanels(unitID, align);
			_addCurve(align);
		}
	}

	/**
	 * Adds the tile property panel.
	 *
	private void _addTilePropertyPanel(const string&in name = ".tile") {
		_tilePanel = PropertyPanel(layout + name);
	}
	
	/**
	 * Adds the unit property panel.
	 * @param unitID The ID of the unit to display.
	 *
	private void _addUnitPropertyPanel(const UnitID unitID) {
		if (unitID > 0) {
			_unitPanels.insertLast(PropertyPanel(layout + ".unit" +
				formatUInt(unitID)));
		}
	}

	/**
	 * Adds the loaded unit property panels.
	 * @param unitID The ID of the unit who has the loaded units on it.
	 * @param align  The alignment of the tile widget.
	 *
	private void _addLoadedUnitPropertyPanels(const UnitID unitID,
		const TileWidgetAlignment align) {
		if (unitID > 0) {
			const array<UnitID> loadedIDs = game.getLoadedUnits(unitID);
			for (int64 i = (align == TileWidgetAlignment::Right ?
				loadedIDs.length() - 1 : 0);
				(align == TileWidgetAlignment::Right ?
								i >= 0 : i < loadedIDs.length());
				(align == TileWidgetAlignment::Right ?
								--i : ++i)) {
				_unitPanels.insertLast(PropertyPanel(layout + ".unit" +
					formatUInt(loadedIDs[i])));
			}
		}
	}
	
	/**
	 * Adds the curve picture panel.
	 *
	private void _addCurve(const TileWidgetAlignment align) {
		addWidget("Picture", layout + ".curve");
		if (align == TileWidgetAlignment::Right) {
			setWidgetSprite(layout + ".curve", "icon", "curveRightAlign");
		} else {
			setWidgetSprite(layout + ".curve", "icon", "curveLeftAlign");
		}
		matchWidgetSizeToSprite(layout + ".curve", true);
	}
	
	/**
	 * Removes all of the widgets within the layout.
	 *
	private void _removePanels() {
		_unitPanels.removeRange(0, _unitPanels.length());
		removeWidgetsFromContainer(layout);
	}

	/**
	 * Used to force widget creation upon a first call to update().
	 *
	private bool _firstUpdateCall = true;

	/**
	 * The tile previously given to update().
	 *
	private Vector2 _prevTilePos;

	/**
	 * The alignment previously given to update().
	 *
	private TileWidgetAlignment _prevAlign;

	/**
	 * Holds the property panel displaying the tile.
	 *
	private PropertyPanel _tilePanel;

	/**
	 * Holds the property panels displaying each unit.
	 *
	private array<PropertyPanel> _unitPanels;*/
}