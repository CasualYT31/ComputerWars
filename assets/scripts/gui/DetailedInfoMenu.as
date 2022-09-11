void setUpArmyPanel(string baseLayout) {
	baseLayout += ".armyPanel";
	addWidget("Panel", baseLayout);
	baseLayout += ".";
	ArmyWidget armyWidget(baseLayout + "army1");
	armyWidget.update(0);
}

void setUpTerrainPanel(string baseLayout) {
	baseLayout += ".";
	addWidget("ScrollablePanel", baseLayout + "terrainPanel");
}

void setUpUnitPanel(string baseLayout) {
	baseLayout += ".";
	addWidget("ScrollablePanel", baseLayout + "unitPanel");
}

void DetailedInfoMenuSetUp() {
	string baseLayout = "baseLayout";
	addWidget("HorizontalLayout", baseLayout);
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
}

void DetailedInfoMenuHandleInput(const dictionary controls) {
	if (bool(controls["info"]) || bool(controls["back"])) {
		setGUI(previousMenu);
	}
}