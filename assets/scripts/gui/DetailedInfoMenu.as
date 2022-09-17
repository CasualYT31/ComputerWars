/**
 * Holds names pertaining to each subwidget of each ArmyWidget.
 */
array<ArmyWidget> armyWidgets;

void setUpArmyPanel(string baseLayout) {
	baseLayout += ".armyPanel";
	addWidget("ScrollablePanel", baseLayout);
	setWidgetSize(baseLayout, "33.333%", "100%");
	setHorizontalScrollbarAmount(baseLayout, 100);
}

void setUpTerrainPanel(string baseLayout) {
	baseLayout += ".terrainPanel";
	addWidget("ScrollablePanel", baseLayout);
	setWidgetSize(baseLayout, "33.333%", "100%");
	setGroupPadding(baseLayout, "5%");
	baseLayout += ".terrainLayout";
	addWidget("VerticalLayout", baseLayout);
	baseLayout += ".";

	addWidget("HorizontalLayout", baseLayout + "detailsAndPicture");
	addWidget("VerticalLayout", baseLayout + "detailsAndPicture.details");
	addWidget("Label", baseLayout + "detailsAndPicture.details.longName");
	addWidget("HorizontalLayout",
		baseLayout + "detailsAndPicture.details.defence");
	addWidget("Picture",
		baseLayout + "detailsAndPicture.details.defence.icon");
	setWidgetSprite(baseLayout + "detailsAndPicture.details.defence.icon", "icon",
		"defstar");
	addWidget("Label",
		baseLayout + "detailsAndPicture.details.defence.label");
	addWidget("HorizontalLayout",
		baseLayout + "detailsAndPicture.details.hp");
	addWidget("Picture",
		baseLayout + "detailsAndPicture.details.hp.icon");
	setWidgetSprite(baseLayout + "detailsAndPicture.details.hp.icon", "icon",
		"hp");
	addWidget("Label",
		baseLayout + "detailsAndPicture.details.hp.label");
	addWidget("Group", baseLayout + "detailsAndPicture.group");
	addWidget("Picture", baseLayout + "detailsAndPicture.group.picture");
	setWidgetPosition(baseLayout + "detailsAndPicture.group.picture",
		"100%", "50%");
	setWidgetOrigin(baseLayout + "detailsAndPicture.group.picture", 1.0, 0.5);

	addWidget("Label", baseLayout + "description");

	addWidget("VerticalLayout", baseLayout + "moveCosts");
}

void setUpUnitPanel(string baseLayout) {
	baseLayout += ".unitPanel";
	addWidget("ScrollablePanel", baseLayout);
	setWidgetSize(baseLayout, "33.333%", "100%");
	setGroupPadding(baseLayout, "5%");
	baseLayout += ".unitLayout";
	addWidget("VerticalLayout", baseLayout);
	string unitLayout = baseLayout + ".";
	string details = unitLayout + "details";

	addWidget("HorizontalLayout", details);
	details += ".";
	string stats = details + "stats";
	addWidget("VerticalLayout", stats);
	stats += ".";
	addWidget("Label", stats + "name");
	stats += "grid";
	addWidget("Grid", stats);
	stats += ".";
	addWidgetToGrid("Label", stats + "priceLabel", 0, 0);
	setWidgetText(stats + "priceLabel", "~G.");
	addWidgetToGrid("Picture", stats + "fuelIcon", 1, 0);
	setWidgetSprite(stats + "fuelIcon", "icon", "fuel");
	addWidgetToGrid("Picture", stats + "ammoIcon", 2, 0);
	setWidgetSprite(stats + "ammoIcon", "icon", "ammo");
	addWidgetToGrid("Label", stats + "price", 0, 1);
	addWidgetToGrid("Label", stats + "fuel", 1, 1);
	addWidgetToGrid("Label", stats + "ammo", 2, 1);
	addWidgetToGrid("Picture", stats + "moveIcon", 0, 2);
	setWidgetSprite(stats + "moveIcon", "icon", "move");
	addWidgetToGrid("Picture", stats + "typeIcon", 1, 2);
	addWidgetToGrid("Picture", stats + "visionIcon", 2, 2);
	setWidgetSprite(stats + "visionIcon", "icon", "vision");
	addWidgetToGrid("Label", stats + "mp", 0, 3);
	addWidgetToGrid("Label", stats + "movement", 1, 3);
	addWidgetToGrid("Label", stats + "vision", 2, 3);
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

	addWidget("VerticalLayout", details + "pictureAndRange");
	addWidget("Panel", details + "pictureAndRange.panel");
	addWidget("Picture", details + "pictureAndRange.panel.picture");
	setWidgetOrigin(
		details + "pictureAndRange.panel.picture", 1.0, 0.5);
	setWidgetPosition(
		details + "pictureAndRange.panel.picture", "100%", "50%");
	addWidget("HorizontalLayout", details + "pictureAndRange.rangeLayout");
	addWidget("Panel", details + "pictureAndRange.rangeLayout.panel");
	addWidget("Label", details + "pictureAndRange.rangeLayout.panel.label");
	setWidgetOrigin(
		details + "pictureAndRange.rangeLayout.panel.label", 1.0, 0.5);
	setWidgetPosition(
		details + "pictureAndRange.rangeLayout.panel.label", "100%", "50%");
	setWidgetText(details + "pictureAndRange.rangeLayout.panel.label", "range");
	string ranges = details + "pictureAndRange.rangeLayout.ranges";
	addWidget("Grid", ranges);
	ranges += ".";
	addWidgetToGrid("Label", ranges + "lower", 0, 0);
	addWidgetToGrid("Picture", ranges + "icon", 0, 1);
	setWidgetSprite(ranges + "icon", "icon", "range");
	addWidgetToGrid("Label", ranges + "higher", 0, 2);
	ranges.erase(ranges.length() - 1);
	setWidgetAlignmentInGrid(ranges, 0, 0, WidgetAlignment::Centre);
	setWidgetAlignmentInGrid(ranges, 0, 1, WidgetAlignment::Centre);
	setWidgetAlignmentInGrid(ranges, 0, 2, WidgetAlignment::Centre);
	setWidgetRatioInLayout(details + "pictureAndRange", 0, 9.0);
	setWidgetRatioInLayout(details + "pictureAndRange", 1, 1.0);

	details.erase(details.length() - 1);
	setWidgetRatioInLayout(details, 0, 1.5);
	setWidgetRatioInLayout(details, 1, 1.0);

	addWidget("Label", unitLayout + "description");
}

void DetailedInfoMenuSetUp() {
	string baseLayout = "DetailedInfoMenu.baseLayout";
	addWidget("HorizontalLayout", baseLayout);
	setWidgetSize(baseLayout, "100%", "100%");
	setUpArmyPanel(baseLayout);
	setUpTerrainPanel(baseLayout);
	setUpUnitPanel(baseLayout);
}

// Remembers the previous menu so that it can go back to it.
string previousMenu;

/**
 * Determines which tile picture to present to the user.
 * @param  pos The location of the tile whose picture is to be shown.
 * @return The \c tilePicture.normal sprite to show.
 */
const string tilePicture(const Vector2&in pos) {
	const Terrain t = game.getTerrainOfTile(pos);
	// If the tile is owned, attempt to retrieve the owned terrain picture.
	const auto ownerID = game.getTileOwner(pos);
	if (ownerID != NO_ARMY) {
		const auto owned = t.picture[game.getArmyCountry(ownerID).ID];
		if (!owned.isEmpty()) {
			return owned;
		}
	}
	// If it can't be retrieved, or if the tile isn't owned, present the icon
	// instead.
	return t.iconName;
}

/**
 * Retrieves all the details on the current armies and the current tile and
 * displays detailed information on them.
 */
void DetailedInfoMenuOpen(const string&in previous) {
	previousMenu = previous;

	// Setup army panel.
	const uint armyCount = game.getArmyCount();
	for (uint a = 0; a < armyCount; a++) {
		armyWidgets.insertLast(ArmyWidget(
			"DetailedInfoMenu.baseLayout.armyPanel.army" + formatUInt(a)));
		armyWidgets[a].update(a);
		setWidgetPosition(armyWidgets[a].panel, "2%", "2% + " +
			formatUInt((ARMYWIDGET_HEIGHT + 10) * a) + "px");
	}
	
	// Setup tile panel.
	const Terrain terrainType = game.getTerrainOfTile(game.getSelectedTile());
	string base = "DetailedInfoMenu.baseLayout.terrainPanel.terrainLayout.";
	setWidgetText(base + "detailsAndPicture.details.longName", terrainType.name);
	setWidgetText(base + "detailsAndPicture.details.defence.label", "~" +
		formatUInt(terrainType.defence));
	setWidgetText(base + "detailsAndPicture.details.hp.label", "~" +
		formatInt(game.getTileHP(game.getSelectedTile())) + " / " +
		formatUInt(terrainType.maxHP));
	if (terrainType.maxHP == 0) {
		setWidgetVisibility(base + "detailsAndPicture.details.hp", false);
	} else {
		setWidgetVisibility(base + "detailsAndPicture.details.hp", true);
	}
	setWidgetSprite(base + "detailsAndPicture.group.picture",
		"tilePicture.normal", tilePicture(game.getSelectedTile()));
	setWidgetText(base + "description", terrainType.description);
	base += "moveCosts.";
	for (BankID moveID = 0; moveID < movement.length(); ++moveID) {
		if (terrainType.moveCost[moveID] >= 0) {
			const Movement movementType = movement[moveID];
			const string costName = base + "move" + formatUInt(moveID);
			addWidget("HorizontalLayout", costName);
			addWidget("Picture", costName + ".icon");
			setWidgetSprite(costName + ".icon", "icon", movementType.iconName);
			addWidget("Label", costName + ".shortName");
			setWidgetText(costName + ".shortName", movementType.shortName);
			addWidget("Label", costName + ".cost");
			setWidgetText(costName + ".cost", "~" + terrainType.moveCost[moveID]);
		}
	}

	// Setup unit panel.
	const string unitPanel =
		"DetailedInfoMenu.baseLayout.unitPanel.unitLayout.details";
	const auto unitID = game.getUnitOnTile(game.getSelectedTile());
	if (unitID > 0) {
		const Unit unitType = game.getUnitType(unitID);
		const Movement movementType = unitType.movementType;
		setWidgetText(unitPanel + ".stats.name", unitType.name);
		setWidgetText(unitPanel + ".stats.grid.price", "~" +
			formatUInt(unitType.cost));
		setWidgetText(unitPanel + ".stats.grid.fuel", "~" +
			formatInt(game.getUnitFuel(unitID)) + " / " +
			formatInt(unitType.maxFuel));
		setWidgetText(unitPanel + ".stats.grid.ammo", "~" +
			formatInt(game.getUnitAmmo(unitID)) + " / " +
			formatInt(unitType.maxAmmo));
		setWidgetText(unitPanel + ".stats.grid.mp", "~" +
			formatUInt(unitType.movementPoints));
		setWidgetSprite(unitPanel + ".stats.grid.typeIcon", "icon",
			movementType.iconName);
		setWidgetText(unitPanel + ".stats.grid.movement",
			movementType.shortName);
		setWidgetText(unitPanel + ".stats.grid.vision", "~" +
			formatUInt(unitType.vision));

		setWidgetSprite(unitPanel + ".pictureAndRange.panel.picture",
			"unitPicture", unitType.pictureSprite[
			game.getArmyCountry(game.getArmyOfUnit(unitID)).ID]);
		setWidgetText(unitPanel + ".pictureAndRange.rangeLayout.ranges.lower",
			"~" + unitType.lowerRange);
		setWidgetText(unitPanel + ".pictureAndRange.rangeLayout.ranges.higher",
			"~" + unitType.higherRange);

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
 * Deletes all the dynamic widgets.
 */
void DetailedInfoMenuClose() {
	// Remove all army widgets.
	const uint armyCount = game.getArmyCount();
	for (uint a = 0; a < armyCount; a++) {
		armyWidgets[a].remove();
	}
	armyWidgets.removeRange(0, armyWidgets.length());

	// Remove all movement cost layouts.
	removeWidgetsFromContainer(
		"DetailedInfoMenu.baseLayout.terrainPanel.terrainLayout.moveCosts");
}

void DetailedInfoMenuHandleInput(const dictionary controls) {
	if (bool(controls["info"]) || bool(controls["back"])) {
		setGUI(previousMenu);
	}
}