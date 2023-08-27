/**
 * Holds names pertaining to each subwidget of each ArmyWidget.
 */
array<ArmyWidget> armyWidgets;

/**
 * Sets up the army panel of the detailed info menu.
 * @param  baseLayout The name of the widget in which the army panel will be
 *                    added.
 * @return The name of the ScrollablePanel added.
 */
string setUpArmyPanel(string baseLayout) {
	baseLayout += ".armyPanel";
	addWidget(ScrollablePanel, baseLayout);
    const auto ret = baseLayout;
	setWidgetSize(baseLayout, "33.333%", "100%");
	setHorizontalScrollbarAmount(baseLayout, 100);
    setVerticalScrollbarAmount(baseLayout, 50);
	baseLayout += ".day";
	addWidget(Label, baseLayout);
	setWidgetPosition(baseLayout, "2%", "2%");
	setWidgetTextSize(baseLayout, 32);
	setWidgetTextColour(baseLayout, Colour(255, 255, 255, 255));
	setWidgetTextOutlineColour(baseLayout, Colour(0, 0, 0, 255));
	setWidgetTextOutlineThickness(baseLayout, 3.0);
    return ret;
}

/**
 * Sets up the terrain panel of the detailed info menu.
 * @param  baseLayout The name of the widget in which the terrain panel will be
 *                    added.
 * @return The name of the ScrollablePanel added.
 */
string setUpTerrainPanel(string baseLayout) {
	baseLayout += ".terrainPanel";
	addWidget(ScrollablePanel, baseLayout);
    const auto ret = baseLayout;
	setWidgetSize(baseLayout, "33.333%", "100%");
	setGroupPadding(baseLayout, "5%");
	baseLayout += ".terrainLayout";
    const auto verticalLayout = baseLayout;
	addWidget(VerticalLayout, baseLayout);
	baseLayout += ".";

	addWidget(HorizontalLayout, baseLayout + "detailsAndPicture");
	addWidget(VerticalLayout, baseLayout + "detailsAndPicture.details");
	addWidget(Label, baseLayout + "detailsAndPicture.details.longName");
	addWidget(HorizontalLayout, baseLayout + "detailsAndPicture.details.defence");
	addWidget(Picture, baseLayout + "detailsAndPicture.details.defence.icon");
	setWidgetSprite(baseLayout + "detailsAndPicture.details.defence.icon", "icon",
		"defstar");
	addWidget(Label, baseLayout + "detailsAndPicture.details.defence.label");
	addWidget(HorizontalLayout, baseLayout + "detailsAndPicture.details.hp");
	addWidget(Picture, baseLayout + "detailsAndPicture.details.hp.icon");
	setWidgetSprite(baseLayout + "detailsAndPicture.details.hp.icon", "icon",
		"hp");
	addWidget(Label, baseLayout + "detailsAndPicture.details.hp.label");
	addWidget(Group, baseLayout + "detailsAndPicture.group");
	addWidget(Picture, baseLayout + "detailsAndPicture.group.picture");
	setWidgetPosition(baseLayout + "detailsAndPicture.group.picture", "100%",
        "50%");
	setWidgetOrigin(baseLayout + "detailsAndPicture.group.picture", 1.0, 0.5);

	addWidget(Label, baseLayout + "description");

	addWidget(VerticalLayout, baseLayout + "moveCosts");
    setWidgetRatioInLayout(verticalLayout, 0, 1.5f);
    return ret;
}

/**
 * Sets up the unit panel of the detailed info menu.
 * @param  baseLayout The name of the widget in which the unit panel will be
 *                    added.
 * @return The name of the ScrollablePanel added.
 */
string setUpUnitPanel(string baseLayout) {
	baseLayout += ".unitPanel";
	addWidget(ScrollablePanel, baseLayout);
    const auto ret = baseLayout;
	setWidgetSize(baseLayout, "33.333%", "100%");
	setGroupPadding(baseLayout, "5%");
	baseLayout += ".unitLayout";
	addWidget(VerticalLayout, baseLayout);
	string unitLayout = baseLayout + ".";
	string details = unitLayout + "details";

	addWidget(HorizontalLayout, details);
	details += ".";
	string stats = details + "stats";
	addWidget(VerticalLayout, stats);
	stats += ".";
	addWidget(Label, stats + "name");
	stats += "grid";
	addWidget(Grid, stats);
	stats += ".";
	addWidgetToGrid(Label, stats + "priceLabel", 0, 0);
	setWidgetText(stats + "priceLabel", "price", {any("")});
	addWidgetToGrid(Picture, stats + "fuelIcon", 1, 0);
	setWidgetSprite(stats + "fuelIcon", "icon", "fuel");
	addWidgetToGrid(Picture, stats + "ammoIcon", 2, 0);
	setWidgetSprite(stats + "ammoIcon", "icon", "ammo");
	addWidgetToGrid(Label, stats + "price", 0, 1);
	addWidgetToGrid(Label, stats + "fuel", 1, 1);
	addWidgetToGrid(Label, stats + "ammo", 2, 1);
	addWidgetToGrid(Picture, stats + "moveIcon", 0, 2);
	setWidgetSprite(stats + "moveIcon", "icon", "move");
	addWidgetToGrid(Picture, stats + "typeIcon", 1, 2);
	addWidgetToGrid(Picture, stats + "visionIcon", 2, 2);
	setWidgetSprite(stats + "visionIcon", "icon", "vision");
	addWidgetToGrid(Label, stats + "mp", 0, 3);
	addWidgetToGrid(Label, stats + "movement", 1, 3);
	addWidgetToGrid(Label, stats + "vision", 2, 3);
	stats.erase(stats.length() - 1);
	setWidgetAlignmentInGrid(stats, 0, 0, WidgetAlignment::Right);
	setWidgetAlignmentInGrid(stats, 1, 0, WidgetAlignment::Right);
	setWidgetAlignmentInGrid(stats, 2, 0, WidgetAlignment::Right);
	setWidgetAlignmentInGrid(stats, 0, 1, WidgetAlignment::Left);
	setWidgetAlignmentInGrid(stats, 1, 1, WidgetAlignment::Left);
	setWidgetAlignmentInGrid(stats, 2, 1, WidgetAlignment::Left);
	setWidgetAlignmentInGrid(stats, 0, 2, WidgetAlignment::Right);
	setWidgetAlignmentInGrid(stats, 1, 2, WidgetAlignment::Right);
	setWidgetAlignmentInGrid(stats, 2, 2, WidgetAlignment::Right);
	setWidgetAlignmentInGrid(stats, 0, 3, WidgetAlignment::Left);
	setWidgetAlignmentInGrid(stats, 1, 3, WidgetAlignment::Left);
	setWidgetAlignmentInGrid(stats, 2, 3, WidgetAlignment::Left);
	setWidgetRatioInLayout(details + "stats", 0, 1.0);
	setWidgetRatioInLayout(details + "stats", 1, 9.0);

	addWidget(VerticalLayout, details + "pictureAndRange");
	addWidget(Panel, details + "pictureAndRange.panel");
	addWidget(Picture, details + "pictureAndRange.panel.picture");
	setWidgetOrigin(details + "pictureAndRange.panel.picture", 1.0, 0.5);
	setWidgetPosition(details + "pictureAndRange.panel.picture", "100%", "50%");
	addWidget(HorizontalLayout, details + "pictureAndRange.rangeLayout");
	addWidget(Panel, details + "pictureAndRange.rangeLayout.panel");
	addWidget(Label, details + "pictureAndRange.rangeLayout.panel.label");
	setWidgetOrigin(details + "pictureAndRange.rangeLayout.panel.label", 1.0,
        0.5);
	setWidgetPosition(details + "pictureAndRange.rangeLayout.panel.label", "100%",
        "50%");
	setWidgetText(details + "pictureAndRange.rangeLayout.panel.label", "range");
	string ranges = details + "pictureAndRange.rangeLayout.ranges";
	addWidget(Grid, ranges);
	ranges += ".";
	addWidgetToGrid(Label, ranges + "lower", 0, 0);
	addWidgetToGrid(Picture, ranges + "icon", 0, 1);
	setWidgetSprite(ranges + "icon", "icon", "range");
	addWidgetToGrid(Label, ranges + "higher", 0, 2);
	ranges.erase(ranges.length() - 1);
	setWidgetAlignmentInGrid(ranges, 0, 0, WidgetAlignment::Centre);
	setWidgetAlignmentInGrid(ranges, 0, 1, WidgetAlignment::Centre);
	setWidgetAlignmentInGrid(ranges, 0, 2, WidgetAlignment::Centre);
	setWidgetRatioInLayout(details + "pictureAndRange", 0, 9.0);
	setWidgetRatioInLayout(details + "pictureAndRange", 1, 1.0);

	details.erase(details.length() - 1);
	setWidgetRatioInLayout(details, 0, 1.5);
	setWidgetRatioInLayout(details, 1, 1.0);

	addWidget(Label, unitLayout + "description");
    return ret;
}

/**
 * Sets up the entire detailed info menu.
 */
void DetailedInfoMenuSetUp() {
	string baseLayout = "DetailedInfoMenu.baseLayout";
	addWidget(HorizontalLayout, baseLayout);
	setWidgetSize(baseLayout, "100%", "100%");
	const auto armyPanel = setUpArmyPanel(baseLayout);
	const auto terrainPanel = setUpTerrainPanel(baseLayout);
	const auto unitPanel = setUpUnitPanel(baseLayout);
    setWidgetDirectionalFlow(armyPanel, "", "", unitPanel, terrainPanel);
    setWidgetDirectionalFlow(terrainPanel, "", "", armyPanel, unitPanel);
    setWidgetDirectionalFlow(unitPanel, "", "", terrainPanel, armyPanel);
    setWidgetDirectionalFlowStart(armyPanel);
}

/**
 * Determines which tile picture to present to the user.
 * @param  pos The location of the tile whose picture is to be shown.
 * @return The \c tilePicture.normal sprite to show.
 */
const string tilePicture(const Vector2&in pos) {
	const auto t = game.map.getTileType(pos).type;
	// If the tile is owned, attempt to retrieve the owned terrain picture.
	const auto ownerID = game.map.getTileOwner(pos);
	if (ownerID != NO_ARMY) {
		const auto owned = t.picture(game.map.getArmyCountry(ownerID).scriptName);
		if (!owned.isEmpty()) return owned;
	}
	// If it can't be retrieved, or if the tile isn't owned, present the icon
	// instead.
	return t.iconName;
}

/**
 * Retrieves all the details on the current armies and the current tile and
 * displays detailed information on them.
 */
void DetailedInfoMenuOpen() {
	// Setup army panel.
	setWidgetText("DetailedInfoMenu.baseLayout.armyPanel.day", "day",
		{ any(game.map.getDay()) });
	const auto armies = game.map.getArmyIDs();
	for (uint a = 0, armyCount = armies.length(); a < armyCount; a++) {
		armyWidgets.insertLast(ArmyWidget(
			"DetailedInfoMenu.baseLayout.armyPanel.army" + formatUInt(a)));
        armyWidgets[a].setAlignment(ArmyWidgetAlignment::Left);
		armyWidgets[a].update(armies[a]);
		setWidgetPosition(armyWidgets[a].panel, "2%", "2% + " +
			formatUInt(55 + (ARMYWIDGET_HEIGHT + 10) * a) + "px");
	}
	
	// Setup tile panel.
	const auto terrainType =
		game.map.getTileType(game.map.getSelectedTile()).type;
	string base = "DetailedInfoMenu.baseLayout.terrainPanel.terrainLayout.";
	setWidgetText(base + "detailsAndPicture.details.longName", terrainType.name);
	setWidgetText(base + "detailsAndPicture.details.defence.label", "~" +
		formatUInt(terrainType.defence));
	setWidgetText(base + "detailsAndPicture.details.hp.label", "~" +
		formatInt(game.map.getTileHP(game.map.getSelectedTile())) + " / " +
		formatUInt(terrainType.maxHP));
	if (terrainType.maxHP == 0) {
		setWidgetVisibility(base + "detailsAndPicture.details.hp", false);
	} else {
		setWidgetVisibility(base + "detailsAndPicture.details.hp", true);
	}
	setWidgetSprite(base + "detailsAndPicture.group.picture",
		"tilePicture.normal", tilePicture(game.map.getSelectedTile()));
	setWidgetText(base + "description", terrainType.description);
	base += "moveCosts.";
	const auto@ movementTypeNames = movement.scriptNames;
	const auto movementTypeCount = movement.length();
	for (uint64 moveID = 0; moveID < movementTypeCount; ++moveID) {
		if (terrainType.moveCost[movementTypeNames[moveID]] >= 0) {
			const auto movementType = movement[movementTypeNames[moveID]];
			const string costName = base + "move" + formatUInt(moveID);
			addWidget(HorizontalLayout, costName);
			addWidget(Picture, costName + ".icon");
			setWidgetSprite(costName + ".icon", "icon", movementType.iconName);
			addWidget(Label, costName + ".shortName");
			setWidgetText(costName + ".shortName", movementType.shortName);
			addWidget(Label, costName + ".cost");
			setWidgetText(costName + ".cost", "~" +
				terrainType.moveCost[movementTypeNames[moveID]]);
		}
	}

	// Setup unit panel.
	const string unitPanel =
		"DetailedInfoMenu.baseLayout.unitPanel.unitLayout.details";
	const auto unitID = game.map.getUnitOnTile(game.map.getSelectedTile());
	if (unitID > 0 &&
		game.map.isUnitVisible(unitID, game.map.getSelectedArmy())) {
		const auto unitType = game.map.getUnitType(unitID);
		const auto movementType = unitType.movementType;
		setWidgetText(unitPanel + ".stats.name", unitType.name);
		setWidgetText(unitPanel + ".stats.grid.price", "~" +
			formatUInt(unitType.cost));
		setWidgetText(unitPanel + ".stats.grid.fuel", "~" +
			formatInt(game.map.getUnitFuel(unitID)) + " / " +
			formatInt(unitType.maxFuel));
		// TODO-1 {
		if (unitType.weaponCount > 0 &&
			!unitType.weapon(0).hasInfiniteAmmo) {
			setWidgetText(unitPanel + ".stats.grid.ammo", "~" +
				formatInt(game.map.getUnitAmmo(unitID,
					unitType.weapon(0).scriptName)) + " / " +
				formatInt(unitType.weapon(0).maxAmmo));
		} else {
			setWidgetText(unitPanel + ".stats.grid.ammo", "~");
		}
		// }
		setWidgetText(unitPanel + ".stats.grid.mp", "~" +
			formatUInt(unitType.movementPoints));
		setWidgetSprite(unitPanel + ".stats.grid.typeIcon", "icon",
			movementType.iconName);
		setWidgetText(unitPanel + ".stats.grid.movement",
			movementType.shortName);
		setWidgetText(unitPanel + ".stats.grid.vision", "~" +
			formatUInt(unitType.vision));

		setWidgetSprite(unitPanel + ".pictureAndRange.panel.picture",
			"unitPicture", unitType.pictureSprite(
			game.map.getArmyCountry(game.map.getArmyOfUnit(unitID)).turnOrder));
		if (unitType.weaponCount > 0) {
			setWidgetText(unitPanel +
				".pictureAndRange.rangeLayout.ranges.lower",
				"~" + unitType.weapon(0).range.x);
			setWidgetText(unitPanel +
				".pictureAndRange.rangeLayout.ranges.higher",
				"~" + unitType.weapon(0).range.y);
		}

		setWidgetText(
			"DetailedInfoMenu.baseLayout.unitPanel.unitLayout.description",
			unitType.description);

		setWidgetVisibility("DetailedInfoMenu.baseLayout.unitPanel.unitLayout",
			true);
	} else {
		setWidgetVisibility("DetailedInfoMenu.baseLayout.unitPanel.unitLayout",
			false);
	}
}

/**
 * Removes all the dynamic widgets.
 */
void DetailedInfoMenuClose() {
	// Remove all army widgets.
	const uint armyCount = game.map.getArmyCount();
	for (uint a = 0; a < armyCount; a++) {
		armyWidgets[a].remove();
	}
	armyWidgets.removeRange(0, armyWidgets.length());

	// Remove all movement cost layouts.
	removeWidgetsFromContainer(
		"DetailedInfoMenu.baseLayout.terrainPanel.terrainLayout.moveCosts");
}

/**
 * Goes back to the previous menu if the user inputs the necessary control.
 * @param controls The control map given by the engine.
 */
void DetailedInfoMenuHandleInput(const dictionary controls) {
	if (bool(controls["info"]) || bool(controls["back"])) {
		setGUI(PREVIOUS_MENU);
	}
}
