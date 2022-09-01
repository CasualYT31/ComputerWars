/**
 * Adds an army widget to a given container.
 * Here are the widgets that are created in this function:
 * Widget Type  --- Full Name
 * Panel        --- baseName
 * |	Picture --- baseName.currentCO
 * |	Picture --- baseName.tagCO
 * |	Label   --- baseName.funds
 * You can
 * @param baseName The full name of the army widget.
 */
void addArmyWidget(const string&in baseName) {
	addWidget("Panel", baseLayout + "army1");
	baseLayout += "army1";
	setWidgetBackgroundColour(baseLayout, );
	baseLayout += ".";
	addWidget("Picture", baseLayout + "currentCO");
	addWidget("Picture", baseLayout + "tagCO");
	addWidget("Label", baseLayout + "funds");
}

void setUpArmyPanel(string baseLayout) {
	baseLayout += ".armyPanel";
	addWidget("ScrollablePanel", baseLayout);
	baseLayout += ".";

	// Refactor later: just draws the first army.
	/*
	Panel // rounded rectangle with radius of half of the height of the Panel,
	|	  // bg is colour of army.
	|	Picture // current CO
	|	Picture // tag CO
	|	Label   // funds
	*/

	addWidget("Panel", baseLayout + "army1");
	baseLayout += "army1.";
	addWidget("Picture", baseLayout + "currentCO");
	addWidget("Picture", baseLayout + "tagCO");
	addWidget("Label", baseLayout + "funds");
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