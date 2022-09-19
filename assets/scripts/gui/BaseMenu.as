/**
 * The height of each unit button.
 */
const uint HEIGHT = 30;

void BaseMenuSetUp() {
	addWidget("ScrollablePanel", "panel");
	setWidgetSize("panel", "60%", "60%");
	setWidgetOrigin("panel", 0.5, 0.5);
	setWidgetPosition("panel", "50%", "50%");
	setHorizontalScrollbarPolicy("panel", ScrollbarPolicy::Never);
	const uint unitTypeCount = unit.length();
	for (uint i = 0; i < unitTypeCount; ++i) {
		const Unit type = unit[i];
		string movementType = type.movementType.scriptName;
		if (movementType == "TREAD" || movementType == "TIRES" ||
			movementType == "INFANTRY" || movementType == "MECH" ||
			movementType == "PIPELINE") {
			string widget = "panel." + type.scriptName;
			addWidget("BitmapButton", widget, "BaseMenuHandleSignal");
			setWidgetTextSize(widget, 16);
			setWidgetSize(widget, "50%", formatUInt(HEIGHT) + "px");
			const string y =
				formatUInt(uint(i / 2) * HEIGHT) + "px";
			if ((i + 1) % 2 == 0) {
				setWidgetOrigin(widget, 1.0, 0.0);
				setWidgetPosition(widget, "100%", y);
			} else {
				setWidgetPosition(widget, "0%", y);
			}
			setWidgetText(widget, "~" + translate(type.name) + " (G. " +
				formatUInt(type.cost) + ")");
		}
	}
	
	addWidget("Panel", "calc");
	setWidgetOrigin("calc", 0.5, 1.0);
	setWidgetPosition("calc", "50%", "BaseMenu.panel.top");
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

void BaseMenuOpen() {
	const BankID country = game.getArmyCountry(game.getCurrentArmy()).ID;
	const uint unitTypeCount = unit.length();
	for (uint i = 0; i < unitTypeCount; ++i) {
		const Unit type = unit[i];
		string movementType = type.movementType.scriptName;
		if (movementType == "TREAD" || movementType == "TIRES" ||
			movementType == "INFANTRY" || movementType == "MECH" ||
			movementType == "PIPELINE") {
			setWidgetSprite("panel." + type.scriptName, "unit",
				type.unitSprite[country]);
		}
	}
}

void BaseMenuHandleInput(const dictionary controls) {
	if (bool(controls["back"])) {
		setGUI("Map");
	}
}

void BaseMenuHandleSignal(const string&in widgetName, const string&in signal) {
	const Unit type = unit[widgetName.substr(widgetName.findLast(".") + 1)];
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