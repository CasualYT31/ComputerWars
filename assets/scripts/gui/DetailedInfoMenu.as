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
	baseLayout += ".terrainLayout";
	addWidget("VerticalLayout", baseLayout);
	baseLayout += ".";

	addWidget("HorizontalLayout", baseLayout + "detailsAndPicture");
	addWidget("VerticalLayout", baseLayout + "detailsAndPicture.details");
	addWidget("Label", baseLayout + "detailsAndPicture.details.longName");
	addWidget("VerticalLayout",
		baseLayout + "detailsAndPicture.details.defence");
	addWidget("Picture",
		baseLayout + "detailsAndPicture.details.defence.icon");
	addWidget("Label",
		baseLayout + "detailsAndPicture.details.defence.label");
	addWidget("VerticalLayout",
		baseLayout + "detailsAndPicture.details.hp");
	addWidget("Picture",
		baseLayout + "detailsAndPicture.details.hp.icon");
	addWidget("Label",
		baseLayout + "detailsAndPicture.details.hp.label");
	addWidget("Picture", baseLayout + "detailsAndPicture.picture");

	addWidget("Label", baseLayout + "description");

	// Add in movement types.
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

// Retrieves all the details on the current armies and the current tile and
// displays detailed information on them.
void DetailedInfoMenuOpen(const string&in previous) {
	previousMenu = previous;
	const uint armyCount = game.getArmyCount();
	for (uint a = 0; a < armyCount; a++) {
		armyWidgets.insertLast(ArmyWidget(
			"DetailedInfoMenu.baseLayout.armyPanel.army" + formatUInt(a)));
		armyWidgets[a].update(a);
		setWidgetPosition(armyWidgets[a].panel, "2%", "2% + " +
			formatUInt((ARMYWIDGET_HEIGHT + 10) * a) + "px");
	}
}

/**
 * Deletes all ArmyWidgets.
 */
void DetailedInfoMenuClose() {
	const uint armyCount = game.getArmyCount();
	for (uint a = 0; a < armyCount; a++) {
		armyWidgets[a].remove();
	}
	armyWidgets.removeRange(0, armyWidgets.length());
}

void DetailedInfoMenuHandleInput(const dictionary controls) {
	if (bool(controls["info"]) || bool(controls["back"])) {
		setGUI(previousMenu);
	}
}