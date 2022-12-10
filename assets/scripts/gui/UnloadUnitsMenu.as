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

string namestr(uint i, uint counter = 0) {
	if (counter == i) {
		return "p" + formatUInt(counter);
	} else {
		return "p" + formatUInt(counter) + "." + namestr(i, counter + 1);
	}
}

/**
 * Add all the widgets to the grid, based on the unit that is currently selected.
 */
void UnloadUnitsMenuOpen() {
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
	// TODO: disable unload button if the unit cannot be unloaded.
	const auto unloadFromTile = game.map.getSelectedTile();
	const auto unloadFromUnit = game.map.getSelectedUnit();
	const auto loadedUnits = game.map.getLoadedUnits(unloadFromUnit);
	const auto loadedUnitsLength = loadedUnits.length();
	for (uint i = 0; i < loadedUnitsLength; ++i) {
		const string name = formatUInt(i);
		const auto unitID = loadedUnits[i];
		addWidgetToGrid("Picture", "panel.grid.icon" + name, i + 1, 0);
		setWidgetSprite("panel.grid.icon" + name, "unit",
			game.map.getUnitType(unitID).unitSprite[
				game.map.getArmyCountry(game.map.getArmyOfUnit(unitID)).ID
			]);
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
	if (bool(controls["back"])) {
		UnloadUnitsMenu_cancel_Pressed();
		return;
	}
}

/**
 * Cancel the unload operation and go back to the command menu.
 */
void UnloadUnitsMenu_cancel_Pressed() {
	setGUI("PreviewMoveUnitMenu");
}

/**
 * Unload a unit.
 */
void UnloadUnitsMenu_UnloadUnit(const string&in widget, const string&in signal) {
	info(widget);
}
