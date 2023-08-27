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
 * Stores the calc label text size.
 */
const uint BASEMENU_CALC_LABEL_TEXT_SIZE = 16;

/**
 * Stores the button text size.
 */
const uint BASEMENU_BUTTON_TEXT_SIZE = 12;

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
	addWidget(ScrollablePanel, panelName);
	setWidgetVisibility(panelName, false);
	setWidgetSize(panelName, "60%", "60%");
	setWidgetOrigin(panelName, 0.5, 0.5);
	setWidgetPosition(panelName, "50%", "50%");
	setHorizontalScrollbarPolicy(panelName, ScrollbarPolicy::Never);
	const auto unitTypeScriptNames = unittype.scriptNames;
	const uint unitTypeCount = unitTypeScriptNames.length();
	array<string> unitsMatchingMovement;
	for (uint i = 0; i < unitTypeCount; ++i) {
		const auto type = unittype[unitTypeScriptNames[i]];
		if (movementTypeNames.find(type.movementType.scriptName) >= 0) {
			unitsMatchingMovement.insertLast(unitTypeScriptNames[i]);
		}
	}
    array<array<string>> widgetNames;
    for (uint i = 0; i < columnCount; ++i) widgetNames.insertLast({});
	for (uint i = 0, len = unitsMatchingMovement.length(); i < len; ++i) {
		const auto type = unittype[unitsMatchingMovement[i]];
		string widget = panelName + "." + type.scriptName;
		addWidget(BitmapButton, widget, "BaseMenuHandleSignal");
        widgetNames[i % columnCount].insertLast(widget);
		setWidgetTextSize(widget, BASEMENU_BUTTON_TEXT_SIZE);
		// Rounding errors that will need addressing in the future.
		setWidgetSize(widget, formatFloat(100.0 / double(columnCount)) + "%",
			formatUInt(HEIGHT) + "px");
		const string x = formatFloat(100 / double(columnCount) *
			(i % columnCount)) + "%";
		const string y = formatUInt(uint(i / columnCount)
			* HEIGHT) + "px";
			setWidgetPosition(widget, x, y);
		setWidgetText(widget, "~" + translate(type.name) + " (" +
			translate("price", {any(type.cost)}) + ")");
	}
    // Now organise directional flow.
    for (uint col = 0, cols = widgetNames.length(); col < cols; ++col) {
        for (uint row = 0, rows = widgetNames[col].length(); row < rows; ++row) {
            const auto up = row == 0 ? widgetNames[col][rows - 1] :
                widgetNames[col][row - 1];
            const auto down = row == rows - 1 ? widgetNames[col][0] :
                widgetNames[col][row + 1];
            string right;
            // If there isn't a unit to the immediate right, pick the unit that is
            // on the row above, if possible.
            if (col < cols - 1) {
                if (widgetNames[col + 1].length() <= row) {
                    if (row > 0) {
                        right = widgetNames[col + 1][row - 1];
                    }
                }
            }
            if (right.length() == 0) {
                right = col == cols - 1 ? widgetNames[0][row] :
                    widgetNames[col + 1][row];
            }
            string left;
            // If there isn't a unit to the immediate left, pick the unit that is
            // on the row above, if possible.
            if (col == 0) {
                if (widgetNames[cols - 1].length() <= row) {
                    if (row > 0) {
                        left = widgetNames[cols - 1][row - 1];
                    }
                }
            }
            if (left.length() == 0) {
                left = col == 0 ? widgetNames[cols - 1][row] :
                    widgetNames[col - 1][row];
            }
            setWidgetDirectionalFlow(
                widgetNames[col][row], up, down, left, right);
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
	
	addWidget(Panel, "calc");
	setWidgetOrigin("calc", 0.5, 1.0);
	// They're all taking up the same size, so it shouldn't matter which panel
	// we're basing the position on.
	setWidgetPosition("calc", "50%", "BaseMenu.ground.top");
	setWidgetSize("calc", "60%", "30px");
	addWidget(Label, "calc.label");
	setWidgetSize("calc.label", "100%", "100%");
	setWidgetTextSize("calc.label", BASEMENU_CALC_LABEL_TEXT_SIZE);
	setWidgetTextColour("calc.label", Colour(255, 255, 255, 255));
	setWidgetTextOutlineColour("calc.label", Colour(0, 0, 0, 255));
	setWidgetTextOutlineThickness("calc.label", 1.5);
	setWidgetTextAlignment("calc.label", HorizontalAlignment::Centre,
		VerticalAlignment::Centre);
}

/**
 * When this menu is opened, one of the panels has to be shown. This function
 * shows one of the panels.
 * @param  panelName         The name of the panel to show.
 * @param  movementTypeNames An array of movement types. If a unit has a movement
 *                           type from this list, then it can be built using this
 *                           panel.
 * @return The name of the first unit widget.
 */
string PanelOpen(const string&in panelName,
    const array<string>@ movementTypeNames) {
	const auto country =
		game.map.getArmyCountry(game.map.getSelectedArmy()).scriptName;
	const auto unitTypeNames = unittype.scriptNames;
	const uint unitTypeCount = unittype.length();
    string ret;
	for (uint i = 0; i < unitTypeCount; ++i) {
		const auto type = unittype[unitTypeNames[i]];
		if (movementTypeNames.find(type.movementType.scriptName) >= 0) {
            if (ret.length() == 0) ret = panelName + "." + type.scriptName;
			setWidgetSprite(panelName + "." + type.scriptName, "unit",
				type.unitSprite(country));
		}
	}
	setWidgetVisibility(panelName, true);
    return ret;
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
        setWidgetDirectionalFlowSelection(PanelOpen("ground", GROUND_UNITS));
	} else if (terrain == "AIRPORT") {
		setWidgetDirectionalFlowSelection(PanelOpen("air", AIR_UNITS));
	} else if (terrain == "PORT") {
		setWidgetDirectionalFlowSelection(PanelOpen("sea", SEA_UNITS));
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
	const auto type = unittype[widgetName.substr(widgetName.findLast(".") + 1)];
	if (signal == "MouseReleased") {
		const ArmyID army = game.map.getSelectedArmy();
		if (game.buyUnit(type, army, game.map.getSelectedTile())) {
			setGUI("Map");
		}
	} else if (signal == "MouseEntered") {
		const ArmyID army = game.map.getSelectedArmy();
		const Funds current = game.map.getArmyFunds(army);
		const Funds cost = type.cost;
		const Funds result = current - cost;
		setWidgetText("calc.label", "~" + translate("price", {any(current)}) +
            " - " + translate("price", {any(cost)}) + " = " +
            translate("price", {any(result)}));
	}
}
