/**
 * @file UnloadUnitsMenu.as
 * Code that's run when the user is selecting units to unload.
 */

/**
 * Sets up the grid used to display available units to unload.
 */
void UnloadUnitsMenuSetUp() {
	addWidget("ScrollablePanel", "panel");
	setWidgetBackgroundColour("panel", Colour(0,0,0,128));
	setWidgetOrigin("panel", 0.5, 0.5);
	setWidgetPosition("panel", "50%", "50%");
	setWidgetSize("panel", "33%", "33%");
	addWidget("Grid", "panel.grid");
}

/**
 * Cache the originally selected tile to use for available tile calculations.
 */
Vector2 UnloadUnitsMenuBaseUnloadTile;

/**
 * Cache the originally selected unit to use for \c findPath calculations.
 */
UnitID UnloadUnitsMenuBaseUnloadUnit;

/**
 * The ID of the unit that is being currently unloaded.
 */
UnitID UnloadUnitsMenuCurrentlyUnloadingUnit;

/**
 * Add all the widgets to the grid, based on the unit that is currently selected.
 */
void UnloadUnitsMenuOpen() {
	// Cache the originally selected tile.
	UnloadUnitsMenuBaseUnloadTile = game.map.getSelectedTile();
	UnloadUnitsMenuBaseUnloadUnit = game.map.getSelectedUnit();

	// Add the header widgets.
	addWidgetToGrid("Button", "panel.grid.cancel", 0, 0);
	setWidgetText("panel.grid.cancel", "cancelunload");
	addWidgetToGrid("Button", "panel.grid.proceed", 0, 1);
	setWidgetText("panel.grid.proceed", "proceedwithunload");
	addWidgetToGrid("Picture", "panel.grid.hpicon", 0, 2);
	setWidgetSprite("panel.grid.hpicon", "icon", "hp");
	addWidgetToGrid("Picture", "panel.grid.fuelicon", 0, 3);
	setWidgetSprite("panel.grid.fuelicon", "icon", "fuel");
	addWidgetToGrid("Picture", "panel.grid.ammoicon", 0, 4);
	setWidgetSprite("panel.grid.ammoicon", "icon", "ammo");

	// Go through all the units that are currently loaded on the selected unit.
	const auto loadedUnits =
		game.map.getLoadedUnits(UnloadUnitsMenuBaseUnloadUnit);
	const auto loadedUnitsLength = loadedUnits.length();
	for (uint i = 0; i < loadedUnitsLength; ++i) {
		const auto unitID = loadedUnits[i];
		const string name = formatUInt(unitID);
		addWidgetToGrid("Picture", "panel.grid.icon" + name, i + 1, 0);
		setWidgetSprite("panel.grid.icon" + name, "unit",
			game.map.getUnitType(unitID).unitSprite[
				game.map.getArmyCountry(game.map.getArmyOfUnit(unitID)).ID
			]);
		// Update UnloadUnitsMenuHandleInput() if you change the name of this
		// widget!
		addWidgetToGrid("Button", "panel.grid.button" + name, i + 1, 1,
			"UnloadUnitsMenu_UnloadUnit");
		setWidgetText("panel.grid.button" + name, "unload");
		addWidgetToGrid("Label", "panel.grid.hp" + name, i + 1, 2);
		setWidgetText("panel.grid.hp" + name,
			"~" + formatInt(game.map.getUnitDisplayedHP(unitID)));
		setWidgetTextColour("panel.grid.hp" + name, Colour(255, 255, 255, 255));
		setWidgetTextOutlineColour("panel.grid.hp" + name, Colour(0,0,0,255));
		setWidgetTextOutlineThickness("panel.grid.hp" + name, 2.0);
		addWidgetToGrid("Label", "panel.grid.fuel" + name, i + 1, 3);
		setWidgetText("panel.grid.fuel" + name,
			"~" + formatInt(game.map.getUnitFuel(unitID)));
		setWidgetTextColour("panel.grid.fuel" + name, Colour(255, 255, 255, 255));
		setWidgetTextOutlineColour("panel.grid.fuel" + name, Colour(0,0,0,255));
		setWidgetTextOutlineThickness("panel.grid.fuel" + name, 2.0);
		addWidgetToGrid("Label", "panel.grid.ammo" + name, i + 1, 4);
		setWidgetText("panel.grid.ammo" + name,
			"~" + formatInt(game.map.getUnitAmmo(unitID)));
		setWidgetTextColour("panel.grid.ammo" + name, Colour(255, 255, 255, 255));
		setWidgetTextOutlineColour("panel.grid.ammo" + name, Colour(0,0,0,255));
		setWidgetTextOutlineThickness("panel.grid.ammo" + name, 2.0);
	}
}

/**
 * Remove all widgets from the grid.
 */
void UnloadUnitsMenuClose() {
	removeWidgetsFromContainer("panel.grid");
}

/**
 * Allows the user to cancel an unload operation.
 * @param controls The control map given by the engine.
 */
void UnloadUnitsMenuHandleInput(const dictionary controls) {
	if (getWidgetVisibility("panel")) {
		if (bool(controls["back"])) {
			// Cancel whole unloading operation.
			UnloadUnitsMenu_cancel_Pressed();
			return;
		}
	} else {
		// Move selection around when selecting a tile for an unload, or cancel
		// unload operation.
		HandleCommonGameInput(controls);
		if (bool(controls["select"]) || bool(controls["back"])) {
			if (bool(controls["select"])) {
				// Select the current tile for the currently unloading unit, but
				// only if it is an available tile.
				const auto selectedTile = game.map.getSelectedTile();
				if (game.map.isAvailableTile(selectedTile)) {
					game.map.addPreviewUnit(UnloadUnitsMenuCurrentlyUnloadingUnit,
						selectedTile);
				} else {
					// Don't cancel the unload operation if it's unavailable!
					return;
				}
			}
			game.map.popSelectedUnit();
			setWidgetText("panel.grid.button" +
				formatUInt(UnloadUnitsMenuCurrentlyUnloadingUnit), "load");
			setWidgetVisibility("panel", true);
			return;
		}
	}
}

/**
 * Cancel the unload operation and go back to the command menu.
 */
void UnloadUnitsMenu_cancel_Pressed() {
	// Remove all of the preview units that were added by this menu.
	const auto loadedUnits =
		game.map.getLoadedUnits(UnloadUnitsMenuBaseUnloadUnit);
	const auto loadedUnitsLength = loadedUnits.length();
	for (uint i = 0; i < loadedUnitsLength; ++i) {
		const auto loadedUnit = loadedUnits[i];
		if (game.map.isPreviewUnit(loadedUnit)) {
			game.map.removePreviewUnit(loadedUnit);
		}
	}
	// Force the selection to go back to the originally selected unit (as the
	// currently selected tile could be moved whilst in this menu, and this menu
	// relies on the selected tile being on the original unit as the menu opens).
	game.map.setSelectedTile(UnloadUnitsMenuBaseUnloadTile);
	setGUI("PreviewMoveUnitMenu");
}

/**
 * Proceed with the unload operation.
 */
void UnloadUnitsMenu_proceed_Pressed() {
	// Move the base unit as normal.
	game.moveUnit();
	// Set the locations of all the unloaded units.
	const auto loadedUnits =
		game.map.getLoadedUnits(UnloadUnitsMenuBaseUnloadUnit);
	const auto loadedUnitsLength = loadedUnits.length();
	for (uint i = 0; i < loadedUnitsLength; ++i) {
		const auto unitID = loadedUnits[i];
		if (game.map.isPreviewUnit(unitID)) {
			game.map.unloadUnit(unitID, UnloadUnitsMenuBaseUnloadUnit,
				game.map.getUnitPreviewPosition(unitID));
			game.map.waitUnit(unitID, true);
			game.map.removePreviewUnit(unitID);
		}
	}
	// Go back to the map.
	setGUI("Map");
}

/**
 * Unload a unit.
 * @param widget The name of the Unload button widget.
 * @param signal The signal emitted by the button widget.
 */
void UnloadUnitsMenu_UnloadUnit(const string&in widget, const string&in signal) {
	if (signal == "Pressed") {
		const auto index = widget.findLastNotOf("0123456789");
		if (index < 0) {
			error("Digit not found in widget name \"" + widget + "\", could not "
				"extract unit ID. Won't allow the unload of this unit!");
			return;
		}
		UnloadUnitsMenuCurrentlyUnloadingUnit =
			parseUInt(widget.substr(index + 1));
		if (game.map.isPreviewUnit(UnloadUnitsMenuCurrentlyUnloadingUnit)) {
			// Remove the unit from the map.
			game.map.removePreviewUnit(UnloadUnitsMenuCurrentlyUnloadingUnit);
			setWidgetText(widget, "unload");
		} else {
			setWidgetVisibility("panel", false);
			const auto unitMoveType = game.map.getUnitType(
				UnloadUnitsMenuCurrentlyUnloadingUnit).movementType;
			// Select unit.
			game.map.pushSelectedUnit(UnloadUnitsMenuCurrentlyUnloadingUnit);
			game.map.setAvailableTileShader(AvailableTileShader::Yellow);
			game.newClosedListNode(game.map.closedList, -1,
				UnloadUnitsMenuBaseUnloadTile, 0);
			// Filter adjacent tiles.
			const auto loadedUnits =
				game.map.getLoadedUnits(UnloadUnitsMenuBaseUnloadUnit);
			const auto loadedUnitsLength = loadedUnits.length();
			const auto adjacentTiles =
				game.map.getAvailableTiles(UnloadUnitsMenuBaseUnloadTile, 1, 1);
			const auto adjacentTilesLength = adjacentTiles.length();
			for (uint i = 0; i < adjacentTilesLength; ++i) {
				const auto adjacentTile = adjacentTiles[i];
				// Remove tiles that are already occupied by an unload in
				// progress.
				{
					bool continueLoop = false;
					for (uint j = 0; j < loadedUnitsLength; ++j) {
						if (game.map.tileHasPreviewUnit(adjacentTile,
							loadedUnits[j])) {
							continueLoop = true;
							break;
						}
					}
					if (continueLoop) continue;
				}
				// Remove tiles that the unit cannot be unloaded onto.
				if (game.map.findPathForUnloadUnit(UnloadUnitsMenuBaseUnloadTile,
					adjacentTile, unitMoveType,
					{ UnloadUnitsMenuBaseUnloadUnit }).length() == 0) {
					continue;
				}
				game.map.addAvailableTile(adjacentTile);
			}
		}
	}
}
