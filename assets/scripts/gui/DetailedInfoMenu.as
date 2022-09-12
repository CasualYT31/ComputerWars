/**
 * Holds names pertaining to each subwidget of the ArmyWidget.
 */
ArmyWidget armyWidget;

void setUpArmyPanel(string baseLayout) {
	baseLayout += ".armyPanel";
	addWidget("Panel", baseLayout);
	setWidgetSize(baseLayout, "33.33%", "100%");
	baseLayout += ".";
	armyWidget = ArmyWidget(baseLayout + "army1");
}

void setUpTerrainPanel(string baseLayout) {
	baseLayout += ".terrainPanel";
	addWidget("ScrollablePanel", baseLayout);
	setWidgetSize(baseLayout, "33.33%", "100%");
}

void setUpUnitPanel(string baseLayout) {
	baseLayout += ".unitPanel";
	addWidget("ScrollablePanel", baseLayout);
	setWidgetSize(baseLayout, "33.33%", "100%");
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
	armyWidget.update(0);
}

void DetailedInfoMenuHandleInput(const dictionary controls) {
	if (bool(controls["info"]) || bool(controls["back"])) {
		setGUI(previousMenu);
	}
}