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
		formatUInt(terrainType.maxHP));
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