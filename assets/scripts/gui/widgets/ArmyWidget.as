/**
 * @file ArmyWidget.as
 * Declares the army widget type.
 */

/**
 * The different alignment options available to army widgets.
 */
enum ArmyWidgetAlignment {
	Left,
	Right
}

/**
 * Stores the height of an ArmyWidget.
 */
const uint ARMYWIDGET_HEIGHT = 60;

/**
 * Stores the radius of an ArmyWidget's border.
 */
const float ARMYWIDGET_RADIUS = ARMYWIDGET_HEIGHT / 2.0;

/**
 * Used to create the widgets that represent an army panel in-game.
 */
class ArmyWidget {
	/**
	 * Doesn't create any widgets.
	 */
	ArmyWidget() {}

	/**
	 * Creates the widgets that represent an army panel.
	 * Defaults to left alignment.
	 * @param panelName    The full name of the panel which contains all of the
	 *                     other widgets.
	 * @param transparency The alpha value to assign to the panel's colour.
	 */
	ArmyWidget(const string&in panelName,
		const uint8 transparency = 255) explicit {
		alpha = transparency;

		panel = panelName;
		addWidget("Panel", panel);
		setWidgetSize(panel, "250px", formatUInt(ARMYWIDGET_HEIGHT) + "px");
		setWidgetBorderSize(panel, 2.0);
		setWidgetBorderRadius(panel, ARMYWIDGET_RADIUS);
		setWidgetBorderColour(panel, Colour(0,0,0,alpha));

		team = panelName + ".team";
		addWidget("Label", team);
		setWidgetTextSize(team, 32);
		setWidgetTextColour(team, Colour(255,255,255,255));
		setWidgetTextOutlineColour(team, Colour(0,0,0,255));
		setWidgetTextOutlineThickness(team, 2.0);

		currentCO = panelName + ".currentCO";
		addWidget("Picture", currentCO);

		tagCO = panelName + ".tagCO";
		addWidget("Picture", tagCO);

		funds = panelName + ".funds";
		addWidget("Label", funds);
		setWidgetTextColour(funds, Colour(255,255,255,255));
		setWidgetTextOutlineColour(funds, Colour(0,0,0,255));
		setWidgetTextOutlineThickness(funds, 2.0);

		setAlignment(ArmyWidgetAlignment::Left);
	}

	/**
	 * Removes the widgets from the menu.
	 */
	void remove() {
		removeWidget(panel);
	}

	/**
	 * Configures an army widget to display information on a given army.
	 * @param armyID The ID of the army on the current map to display information
	 *               on.
	 */
	void update(const ArmyID armyID) {
		Colour colour = game.getArmyCountry(armyID).colour;
		colour.a = alpha;
		setWidgetBackgroundColour(panel, colour);

		TeamID teamID = game.getArmyTeam(armyID);
		setWidgetText(team, "~" + formatUInt(teamID));

		Commander currentCommander = game.getArmyCurrentCO(armyID);
		setWidgetSprite(currentCO, "co", currentCommander.iconName);

		if (game.tagCOIsPresent(armyID)) {
			Commander tagCommander = game.getArmyTagCO(armyID);
			setWidgetSprite(tagCO, "co", tagCommander.iconName);
			setWidgetVisibility(tagCO, true);
		} else {
			setWidgetVisibility(tagCO, false);
		}

		setWidgetText(funds, "~G. " + formatInt(game.getArmyFunds(armyID)));
	}

	/**
	 * Configures an army widget to be either left-aligned or right-aligned.
	 * This method will reposition the panel, as well as everything inside the
	 * panel, but the panel itself can be repositioned safely after the call.
	 * @param alignment The alignment. If an invalid value is given, an error will
	 *                  be logged and the widget will not be changed.
	 */
	void setAlignment(const ArmyWidgetAlignment alignment) {
		const float coX = ARMYWIDGET_RADIUS + 5;
		switch (alignment) {
		case ArmyWidgetAlignment::Left:
			setWidgetPosition(panel, "0px", "0px");
			setWidgetPosition(team, "9px", "6px");
			setWidgetPosition(currentCO, formatFloat(coX) + "px", "5px");
			setWidgetPosition(tagCO, formatFloat(coX) + "px", "30px");
			setWidgetPosition(funds, "140px", "5px");
			break;
		case ArmyWidgetAlignment::Right:
			break;
		default:
			error("Attempted to set the ArmyWidget \"" + panel + "\"'s alignment "
				"to " + alignment + ", which is invalid.");
		}
	}

	/**
	 * Holds the name of the panel widget.
	 */
	string panel;

	/**
	 * Holds the name of the team ID label widget.
	 */
	string team;

	/**
	 * Holds the name of the current CO's picture widget.
	 */
	string currentCO;

	/**
	 * Holds the name of the tag CO's picture widget.
	 */
	string tagCO;

	/**
	 * Holds the name of the funds label widget.
	 */
	string funds;

	/**
	 * Holds the alpha that's been assigned to the panel.
	 */
	uint8 alpha;
}