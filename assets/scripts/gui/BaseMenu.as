/**
 * Holds a list of ground unit movement types.
 */
const array<string> GROUND_UNITS = {
	"TREAD",
	"TIRES",
	"INFANTRY",
	"MECH",
	"PIPELINE"
};

/**
 * Holds a list of air unit movement types.
 */
const array<string> AIR_UNITS = {
	"AIR"
};

/**
 * Holds a list of naval unit movement types.
 */
const array<string> SEA_UNITS = {
	"SHIPS",
	"TRANSPORT"
};

/**
 * Sets up the panel shown for a particular type of production tile.
 * @param panelName         The name to give to the root panel widget.
 * @param movementTypeNames An array of movement types. If a unit has a movement
 *                          type from this list, then it can be built using this
 *                          panel.
 * @param columnCount       The number of columns in this panel.
 */
void PanelSetUp(const string&in panelName, const array<string>@ movementTypeNames,
	const uint columnCount) {
	const uint HEIGHT = 30;
	addWidget("ScrollablePanel", panelName);
	setWidgetVisibility(panelName, false);
	setWidgetSize(panelName, "60%", "60%");
	setWidgetOrigin(panelName, 0.5, 0.5);
	setWidgetPosition(panelName, "50%", "50%");
	setHorizontalScrollbarPolicy(panelName, ScrollbarPolicy::Never);
	const uint unitTypeCount = unittype.length();
	uint firstUnitTypeID = 0;
	bool firstIDSet = false;
	for (uint i = 0; i < unitTypeCount; ++i) {
		const UnitType type = unittype[i];
		if (movementTypeNames.find(type.movementType.scriptName) >= 0) {
			if (!firstIDSet) {
				firstIDSet = true;
				firstUnitTypeID = i;
			}
			string widget = panelName + "." + type.scriptName;
			addWidget("BitmapButton", widget, "BaseMenuHandleSignal");
			setWidgetTextSize(widget, 16);
			// Rounding errors that will need addressing in the future.
			setWidgetSize(widget, formatFloat(100.0 / double(columnCount)) + "%",
				formatUInt(HEIGHT) + "px");
			const string x = formatFloat(100 / double(columnCount) *
				((i - firstUnitTypeID) % columnCount)) + "%";
			const string y = formatUInt(uint((i - firstUnitTypeID) / columnCount)
				* HEIGHT) + "px";
				setWidgetPosition(widget, x, y);
			setWidgetText(widget, "~" + translate(type.name) + " (G. " +
				formatUInt(type.cost) + ")");
		}
	}
}

/**
 * Sets up the menu which allows users to produce units.
 */
void BaseMenuSetUp() {
	PanelSetUp("ground", GROUND_UNITS, 2);
	PanelSetUp("air", AIR_UNITS, 1);
	PanelSetUp("sea", SEA_UNITS, 1);
	
	addWidget("Panel", "calc");
	setWidgetOrigin("calc", 0.5, 1.0);
	// They're all taking up the same size, so it shouldn't matter which panel
	// we're basing the position on.
	setWidgetPosition("calc", "50%", "BaseMenu.ground.top");
	setWidgetSize("calc", "60%", "30px");
	addWidget("Label", "calc.label");
	setWidgetSize("calc.label", "100%", "100%");
	setWidgetTextSize("calc.label", 20);
	setWidgetTextColour("calc.label", Colour(255, 255, 255, 255));
	setWidgetTextOutlineColour("calc.label", Colour(0, 0, 0, 255));
	setWidgetTextOutlineThickness("calc.label", 1.5);
	setWidgetTextAlignment("calc.label", HorizontalAlignment::Centre,
		VerticalAlignment::Top);
}

/**
 * When this menu is opened, one of the panels has to be shown. This function
 * shows one of the panels.
 * @param panelName         The name of the panel to show.
 * @param movementTypeNames An array of movement types. If a unit has a movement
 *                          type from this list, then it can be built using this
 *                          panel.
 */
void PanelOpen(const string&in panelName, const array<string>@ movementTypeNames)
	{
	const BankID country = game.map.getArmyCountry(game.map.getSelectedArmy()).ID;
	const uint unitTypeCount = unittype.length();
	for (uint i = 0; i < unitTypeCount; ++i) {
		const UnitType type = unittype[i];
		if (movementTypeNames.find(type.movementType.scriptName) >= 0) {
			setWidgetSprite(panelName + "." + type.scriptName, "unit",
				type.unitSprite[country]);
		}
	}
	setWidgetVisibility(panelName, true);
}

/**
 * When an appropriate base, airport, or port is selected, this menu is opened.
 */
void BaseMenuOpen() {
	setWidgetVisibility("ground", false);
	setWidgetVisibility("air", false);
	setWidgetVisibility("sea", false);
	const string terrain =
		game.map.getTileType(game.map.getSelectedTile()).type.scriptName;
	if (terrain == "BASE") {
		PanelOpen("ground", GROUND_UNITS);
	} else if (terrain == "AIRPORT") {
		PanelOpen("air", AIR_UNITS);
	} else if (terrain == "PORT") {
		PanelOpen("sea", SEA_UNITS);
	}
}

/**
 * When the menu is closed, the calculation label is cleared.
 */
void BaseMenuClose() {
	setWidgetText("calc.label", "~");
}

/**
 * Allows the user to leave the menu without building anything.
 * @param controls Control map given by the engine.
 */
void BaseMenuHandleInput(const dictionary controls) {
	if (bool(controls["back"])) {
		setGUI("Map");
	}
}

/**
 * Handles all signals emitted by button widgets in this menu.
 * Handles clicking of a button, and when the mouse enters a button.
 * @param widgetName The name of the button that emitted the signal.
 * @param signal     The signal emitted. Use this to determine if a button was
 *                   clicked or if the mouse entered a button.
 */
void BaseMenuHandleSignal(const string&in widgetName, const string&in signal) {
	const UnitType type =
		unittype[widgetName.substr(widgetName.findLast(".") + 1)];
	if (signal == "Clicked") {
		const ArmyID army = game.map.getSelectedArmy();
		if (game.buyUnit(type, army, game.map.getSelectedTile())) {
			setGUI("Map");
		}
	} else if (signal == "MouseEntered") {
		const ArmyID army = game.map.getSelectedArmy();
		const Funds current = game.map.getArmyFunds(army);
		const Funds cost = type.cost;
		const Funds result = current - cost;
		setWidgetText("calc.label", "~G. " + formatInt(current) + " - G. " +
			formatInt(cost) + " = G. " + formatInt(result));
	}
}
