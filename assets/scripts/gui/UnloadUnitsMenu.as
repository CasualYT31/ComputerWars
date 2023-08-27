/**
 * @file UnloadUnitsMenu.as
 * Code that's run when the user is selecting units to unload.
 */

/**
 * Sets up the grid used to display available units to unload.
 */
void UnloadUnitsMenuSetUp() {
	addWidget(ScrollablePanel, "panel");
	setWidgetBackgroundColour("panel", Colour(0,0,0,128));
	setWidgetOrigin("panel", 0.5, 0.5);
	setWidgetPosition("panel", "50%", "50%");
	setWidgetSize("panel", "38%", "38%");
	addWidget(Grid, "panel.grid");
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
	addWidgetToGrid(Button, "panel.grid.cancel", 0, 0);
	setWidgetText("panel.grid.cancel", "cancelunload");
    setWidgetDirectionalFlowStart("panel.grid.cancel");
	addWidgetToGrid(Button, "panel.grid.proceed", 0, 1);
	setWidgetText("panel.grid.proceed", "proceedwithunload");
	addWidgetToGrid(Picture, "panel.grid.hpicon", 0, 2);
	setWidgetSprite("panel.grid.hpicon", "icon", "hp");
	addWidgetToGrid(Picture, "panel.grid.fuelicon", 0, 3);
	setWidgetSprite("panel.grid.fuelicon", "icon", "fuel");
	addWidgetToGrid(Picture, "panel.grid.ammoicon", 0, 4);
	setWidgetSprite("panel.grid.ammoicon", "icon", "ammo");

	// Go through all the units that are currently loaded on the selected unit.
	const auto loadedUnits =
		game.map.getLoadedUnits(UnloadUnitsMenuBaseUnloadUnit);
	const auto loadedUnitsLength = loadedUnits.length();
	for (uint i = 0; i < loadedUnitsLength; ++i) {
		const auto unitID = loadedUnits[i];
		const string name = formatUInt(unitID);
		const auto type = game.map.getUnitType(unitID);
		addWidgetToGrid(Picture, "panel.grid.icon" + name, i + 1, 0);
		setWidgetSprite("panel.grid.icon" + name, "unit",
			type.unitSprite(
				game.map.getArmyCountry(game.map.getArmyOfUnit(unitID)).turnOrder
		));
		// Update UnloadUnitsMenuHandleInput() if you change the name of this
		// widget!
		addWidgetToGrid(Button, "panel.grid.button" + name, i + 1, 1,
			"UnloadUnitsMenu_UnloadUnit");
		setWidgetText("panel.grid.button" + name, "unload");
		addWidgetToGrid(Label, "panel.grid.hp" + name, i + 1, 2);
		setWidgetText("panel.grid.hp" + name,
			"~" + formatInt(game.map.getUnitDisplayedHP(unitID)));
		setWidgetTextColour("panel.grid.hp" + name, Colour(255, 255, 255, 255));
		setWidgetTextOutlineColour("panel.grid.hp" + name, Colour(0,0,0,255));
		setWidgetTextOutlineThickness("panel.grid.hp" + name, 2.0);
		addWidgetToGrid(Label, "panel.grid.fuel" + name, i + 1, 3);
		setWidgetText("panel.grid.fuel" + name,
			"~" + formatInt(game.map.getUnitFuel(unitID)));
		setWidgetTextColour("panel.grid.fuel" + name, Colour(255, 255, 255, 255));
		setWidgetTextOutlineColour("panel.grid.fuel" + name, Colour(0,0,0,255));
		setWidgetTextOutlineThickness("panel.grid.fuel" + name, 2.0);
		addWidgetToGrid(Label, "panel.grid.ammo" + name, i + 1, 4);
		// TODO-1 {
		if (type.weaponCount > 0 && !type.weapon(0).hasInfiniteAmmo) {
			setWidgetText("panel.grid.ammo" + name,
				"~" + formatInt(game.map.getUnitAmmo(unitID,
					type.weapon(0).scriptName)));
		} else {
			setWidgetText("panel.grid.ammo" + name, "~");
		}
		// }
		setWidgetTextColour("panel.grid.ammo" + name, Colour(255, 255, 255, 255));
		setWidgetTextOutlineColour("panel.grid.ammo" + name, Colour(0,0,0,255));
		setWidgetTextOutlineThickness("panel.grid.ammo" + name, 2.0);
		if (game.map.isUnitHiding(unitID)) {
			addWidgetToGrid(Label, "panel.grid.ishiddenmsg" + name, i + 1, 5);
			setWidgetText("panel.grid.ishiddenmsg" + name, "hidden");
			setWidgetTextColour("panel.grid.ishiddenmsg" + name,
				Colour(255, 255, 255, 255));
			setWidgetTextOutlineColour("panel.grid.ishiddenmsg" + name,
				Colour(0,0,0,255));
			setWidgetTextOutlineThickness("panel.grid.ishiddenmsg" + name, 2.0);
		}
	}

    // Configure directional flow.
    const auto firstUnloadButton = loadedUnitsLength > 0 ? "panel.grid.button" +
            formatUInt(loadedUnits[0]) : "",
        lastUnloadButton = loadedUnitsLength > 0 ? "panel.grid.button" +
            formatUInt(loadedUnits[loadedUnitsLength - 1]) : "",
        firstUnloadIcon = loadedUnitsLength > 0 ? "panel.grid.icon" +
            formatUInt(loadedUnits[0]) : "",
        lastUnloadIcon = loadedUnitsLength > 0 ? "panel.grid.icon" +
            formatUInt(loadedUnits[loadedUnitsLength - 1]) : "",
        firstUnloadHP = loadedUnitsLength > 0 ? "panel.grid.hp" +
            formatUInt(loadedUnits[0]) : "",
        lastUnloadHP = loadedUnitsLength > 0 ? "panel.grid.hp" +
            formatUInt(loadedUnits[loadedUnitsLength - 1]) : "",
        firstUnloadFuel = loadedUnitsLength > 0 ? "panel.grid.fuel" +
            formatUInt(loadedUnits[0]) : "",
        lastUnloadFuel = loadedUnitsLength > 0 ? "panel.grid.fuel" +
            formatUInt(loadedUnits[loadedUnitsLength - 1]) : "",
        firstUnloadAmmo = loadedUnitsLength > 0 ? "panel.grid.ammo" +
            formatUInt(loadedUnits[0]) : "",
        lastUnloadAmmo = loadedUnitsLength > 0 ? "panel.grid.ammo" +
            formatUInt(loadedUnits[loadedUnitsLength - 1]) : "";
    if (firstUnloadButton.length() > 0)
        setWidgetDirectionalFlowStart(firstUnloadButton);
    setWidgetDirectionalFlow("panel.grid.cancel", lastUnloadIcon,
        firstUnloadIcon, "panel.grid.ammoicon", "panel.grid.proceed");
    setWidgetDirectionalFlow("panel.grid.proceed", lastUnloadButton,
        firstUnloadButton, "panel.grid.cancel", "panel.grid.hpicon");
    setWidgetDirectionalFlow("panel.grid.hpicon", lastUnloadHP,
        firstUnloadHP, "panel.grid.proceed", "panel.grid.fuelicon");
    setWidgetDirectionalFlow("panel.grid.fuelicon", lastUnloadFuel,
        firstUnloadFuel, "panel.grid.hpicon", "panel.grid.ammoicon");
    setWidgetDirectionalFlow("panel.grid.ammoicon", lastUnloadAmmo,
        firstUnloadAmmo, "panel.grid.fuelicon", "panel.grid.cancel");
    for (uint i = 0; i < loadedUnitsLength; ++i) {
        const auto FIRST = i == 0, LAST = i == loadedUnitsLength - 1;
        const auto id = formatUInt(loadedUnits[i]),
            prevID = FIRST ? formatUInt(loadedUnits[loadedUnitsLength - 1]) :
                formatUInt(loadedUnits[i - 1]),
            nextID = LAST ? formatUInt(loadedUnits[0]) :
                formatUInt(loadedUnits[i + 1]);
        const auto HIDDEN_MSG = widgetExists("panel.grid.ishiddenmsg" + id),
            PREV_HIDDEN_MSG = widgetExists("panel.grid.ishiddenmsg" + prevID),
            NEXT_HIDDEN_MSG = widgetExists("panel.grid.ishiddenmsg" + nextID);
        setWidgetDirectionalFlow("panel.grid.icon" + id,
            FIRST ? "panel.grid.cancel" : "panel.grid.icon" + prevID,
            LAST ? "panel.grid.cancel" : "panel.grid.icon" + nextID,
            HIDDEN_MSG ? "panel.grid.ishiddenmsg" + id : "panel.grid.ammo" + id,
            "panel.grid.button" + id);
        setWidgetDirectionalFlow("panel.grid.button" + id,
            FIRST ? "panel.grid.proceed" : "panel.grid.button" + prevID,
            LAST ? "panel.grid.proceed" : "panel.grid.button" + nextID,
            "panel.grid.icon" + id,
            "panel.grid.hp" + id);
        setWidgetDirectionalFlow("panel.grid.hp" + id,
            FIRST ? "panel.grid.hpicon" : "panel.grid.hp" + prevID,
            LAST ? "panel.grid.hpicon" : "panel.grid.hp" + nextID,
            "panel.grid.button" + id,
            "panel.grid.fuel" + id);
        setWidgetDirectionalFlow("panel.grid.fuel" + id,
            FIRST ? "panel.grid.fuelicon" : "panel.grid.fuel" + prevID,
            LAST ? "panel.grid.fuelicon" : "panel.grid.fuel" + nextID,
            "panel.grid.hp" + id,
            "panel.grid.ammo" + id);
        setWidgetDirectionalFlow("panel.grid.ammo" + id,
            FIRST ? "panel.grid.ammoicon" : "panel.grid.ammo" + prevID,
            LAST ? "panel.grid.ammoicon" : "panel.grid.ammo" + nextID,
            "panel.grid.fuel" + id,
            HIDDEN_MSG ? "panel.grid.ishiddenmsg" + id : "panel.grid.icon" + id);
        if (HIDDEN_MSG) {
            setWidgetDirectionalFlow("panel.grid.ishiddenmsg" + id,
                PREV_HIDDEN_MSG ? "panel.grid.ishiddenmsg" + prevID :
                    (FIRST ? "panel.grid.ammoicon" : "panel.grid.ammo" + prevID),
                NEXT_HIDDEN_MSG ? "panel.grid.ishiddenmsg" + nextID :
                    (LAST ? "panel.grid.ammoicon" : "panel.grid.ammo" + nextID),
                "panel.grid.ammo" + id,
                "panel.grid.icon" + id);
        }
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
 * @param controls         The control map given by the engine.
 * @param mouseInputs      Stores which controls are being triggered by the mouse.
 * @param previousPosition The previous mouse position.
 * @param currentPosition  The current mouse position.
 */
void UnloadUnitsMenuHandleInput(const dictionary controls,
    const dictionary mouseInputs, const MousePosition&in previousPosition,
    const MousePosition&in currentPosition) {
	// Prevent user from accidentally proceeding with unload if they haven't
	// selected any units to unload yet.
	if (game.map.getUnitPreviewsCount() == 1) {
		setWidgetEnabled("panel.grid.proceed", false);
	} else {
		setWidgetEnabled("panel.grid.proceed", true);
	}

	if (getWidgetVisibility("panel")) {
		if (bool(controls["back"])) {
			// Cancel whole unloading operation.
			UnloadUnitsMenu_cancel_MouseReleased();
			return;
		}
	} else {
		// Move selection around when selecting a tile for an unload, or cancel
		// unload operation.
		HandleCommonGameInput(controls, mouseInputs, previousPosition,
            currentPosition);
		if (bool(controls["select"]) || bool(controls["back"])) {
			// If the select control is being made by the mouse, and it is not
            // inside the map's graphic, then drop it.
            if (bool(mouseInputs["select"]) &&
                !game.map.getMapBoundingBox().contains(currentPosition)) return;

            if (bool(controls["select"])) {
				// Select the current tile for the currently unloading unit, but
				// only if it is an available tile.
				const auto selectedTile = game.map.getSelectedTile();
				if (game.map.isAvailableTile(selectedTile)) {
					// However! If the selected tile has a hidden unit on it, then
					// prevent the unload of that unit, but force any unloads in
					// progress to go ahead, and make the base unit wait.
					if (game.map.getUnitOnTile(selectedTile) > 0) {
						game.map.popSelectedUnit();
						UnloadUnitsMenu_proceed_MouseReleased();
						return;
					} else {
						game.map.addPreviewUnit(
							UnloadUnitsMenuCurrentlyUnloadingUnit, selectedTile);
						setWidgetText("panel.grid.button" +
							formatUInt(UnloadUnitsMenuCurrentlyUnloadingUnit),
							"load");
					}
				} else {
					// Don't cancel the unload operation if it's unavailable!
					return;
				}
			}
			game.map.popSelectedUnit();
			setWidgetVisibility("panel", true);
			return;
		}
	}
}

/**
 * Cancel the unload operation and go back to the command menu.
 */
void UnloadUnitsMenu_cancel_MouseReleased() {
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
void UnloadUnitsMenu_proceed_MouseReleased() {
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
	// Go back to the map, and always ensure the base panel is visible before
	// leaving (if we don't, then when a unit is unloaded onto a hidden unit, the
	// unload menu will be bugged out in the future).
	setWidgetVisibility("panel", true);
	setGUI("Map");
}

/**
 * Unload a unit.
 * @param widget The name of the Unload button widget.
 * @param signal The signal emitted by the button widget.
 */
void UnloadUnitsMenu_UnloadUnit(const string&in widget, const string&in signal) {
	if (signal == "MouseReleased") {
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
					game.map.getArmyOfUnit(UnloadUnitsMenuBaseUnloadUnit),
					{ UnloadUnitsMenuBaseUnloadUnit }).length() == 0) {
					continue;
				}
				game.map.addAvailableTile(adjacentTile);
			}
		}
	}
}
