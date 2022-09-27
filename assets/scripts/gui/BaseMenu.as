const array<string> GROUND_UNITS = {
	"TREAD",
	"TIRES",
	"INFANTRY",
	"MECH",
	"PIPELINE"
};

const array<string> AIR_UNITS = {
	"AIR"
};

const array<string> SEA_UNITS = {
	"SHIPS",
	"TRANSPORT"
};

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

void BaseMenuSetUp() {
	PanelSetUp("ground", GROUND_UNITS, 2);
	PanelSetUp("air", AIR_UNITS, 1);
	PanelSetUp("sea", SEA_UNITS, 1);
	
	addWidget("Panel", "calc");
	setWidgetOrigin("calc", 0.5, 1.0);
	// They're all taking up the same size, so it shouldn't matter which panel
	// we're passing the position on.
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

void PanelOpen(const string&in panelName, const array<string>@ movementTypeNames)
	{
	const BankID country = game.getArmyCountry(game.getCurrentArmy()).ID;
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

void BaseMenuOpen() {
	setWidgetVisibility("ground", false);
	setWidgetVisibility("air", false);
	setWidgetVisibility("sea", false);
	const string terrain =
		game.getTerrainOfTile(game.getSelectedTile()).scriptName;
	if (terrain == "BASE") {
		PanelOpen("ground", GROUND_UNITS);
	} else if (terrain == "AIRPORT") {
		PanelOpen("air", AIR_UNITS);
	} else if (terrain == "PORT") {
		PanelOpen("sea", SEA_UNITS);
	}
}

void BaseMenuClose() {
	setWidgetText("calc.label", "~");
}

void BaseMenuHandleInput(const dictionary controls) {
	if (bool(controls["back"])) {
		setGUI("Map");
	}
}

void BaseMenuHandleSignal(const string&in widgetName, const string&in signal) {
	const UnitType type =
		unittype[widgetName.substr(widgetName.findLast(".") + 1)];
	if (signal == "Clicked") {
		if (game.buyUnit(type.ID)) setGUI("Map");
	} else if (signal == "MouseEntered") {
		const ArmyID army = game.getCurrentArmy();
		const Funds current = game.getArmyFunds(army);
		const Funds cost = type.cost;
		const Funds result = current - cost;
		setWidgetText("calc.label", "~G. " + formatInt(current) + " - G. " +
			formatInt(cost) + " = G. " + formatInt(result));
	}
}