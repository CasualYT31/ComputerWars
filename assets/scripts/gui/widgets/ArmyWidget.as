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
 * Stores the width of an ArmyWidget.
 */
const uint ARMYWIDGET_WIDTH = 250;

/**
 * Stores the height of an ArmyWidget.
 */
const uint ARMYWIDGET_HEIGHT = 60;

/**
 * Stores the radius of an ArmyWidget's border.
 */
const float ARMYWIDGET_RADIUS = ARMYWIDGET_HEIGHT / 2.0;

/**
 * Stores the size of an ArmyWidget's border.
 */
const float ARMYWIDGET_BORDER_SIZE = 2.0;

/**
 * Stores the text size of the team ID label.
 */
const uint ARMYWIDGET_TEAMID_TEXT_SIZE = 24;

/**
 * Stores the text size of the funds label.
 */
const uint ARMYWIDGET_FUNDS_TEXT_SIZE = 14;

/**
 * Stores the X offset of the team ID label.
 */
const string ARMYWIDGET_X_OFFSET_TEAMID = "3px";

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
		setWidgetSize(panel, formatUInt(ARMYWIDGET_WIDTH) + "px",
			formatUInt(ARMYWIDGET_HEIGHT) + "px");
		setWidgetBorderSize(panel, ARMYWIDGET_BORDER_SIZE);
		setWidgetBorderRadius(panel, ARMYWIDGET_RADIUS);
		setWidgetBorderColour(panel, Colour(0,0,0,alpha));

		team = panelName + ".team";
		addWidget("Label", team);
		setWidgetTextSize(team, ARMYWIDGET_TEAMID_TEXT_SIZE);
		setWidgetTextColour(team, Colour(255,255,255,255));
		setWidgetTextOutlineColour(team, Colour(0,0,0,255));
		setWidgetTextOutlineThickness(team, 2.0);

		currentCO = panelName + ".currentCO";
		addWidget("Picture", currentCO);

		tagCO = panelName + ".tagCO";
		addWidget("Picture", tagCO);

		funds = panelName + ".funds";
		addWidget("Label", funds);
		setWidgetTextSize(funds, ARMYWIDGET_FUNDS_TEXT_SIZE);
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
        // If the army doesn't exist, update with default values.
        if (game.map.isArmyPresent(armyID)) {
            Colour colour = game.map.getArmyCountry(armyID).colour;
            colour.a = alpha;
            setWidgetBackgroundColour(panel, colour);

            TeamID teamID = game.map.getArmyTeam(armyID);
            setWidgetText(team, "~" + formatUInt(teamID));

            string currentCommanderStr = game.map.getArmyCurrentCO(armyID);
            if (currentCommanderStr.isEmpty()) {
                setWidgetVisibility(currentCO, false);
            } else {
                const auto currentCommander = commander[currentCommanderStr];
                setWidgetSprite(currentCO, "co", currentCommander.iconName);
                setWidgetVisibility(currentCO, true);
            }

            if (game.map.tagCOIsPresent(armyID)) {
                const auto tagCommander =
                    commander[game.map.getArmyTagCO(armyID)];
                setWidgetSprite(tagCO, "co", tagCommander.iconName);
                setWidgetVisibility(tagCO, true);
            } else {
                setWidgetVisibility(tagCO, false);
            }

            setWidgetText(funds, "~G. " +
                formatInt(game.map.getArmyFunds(armyID)));
        } else {
            setWidgetBackgroundColour(panel, Colour(255, 255, 255, alpha));
            setWidgetText(team, "~");
            setWidgetVisibility(currentCO, false);
            setWidgetVisibility(tagCO, false);
            setWidgetText(funds, "~");
        }
	}

	/**
	 * Configures an army widget to be either left-aligned or right-aligned.
	 * This method will reposition everything inside the panel, but the panel
     * itself will not be repositioned.
	 * @param alignment The alignment. If an invalid value is given, an error will
	 *                  be logged and the widget will not be changed.
	 */
	void setAlignment(const ArmyWidgetAlignment alignment) {
		const float coX = ARMYWIDGET_RADIUS + 5;
		switch (alignment) {
		case ArmyWidgetAlignment::Left:
			setWidgetOrigin(team, 0.0f, 0.5f);
			setWidgetPosition(team, ARMYWIDGET_X_OFFSET_TEAMID,
                formatFloat(ARMYWIDGET_RADIUS) + "px");
			setWidgetOrigin(currentCO, 0.0f, 0.0f);
			setWidgetPosition(currentCO, formatFloat(coX) + "px", "5px");
			setWidgetOrigin(tagCO, 0.0f, 0.0f);
			setWidgetPosition(tagCO, formatFloat(coX) + "px", "30px");
			setWidgetOrigin(funds, 0.0f, 0.0f);
			setWidgetPosition(funds, "140px", "5px");
			break;
		case ArmyWidgetAlignment::Right:
			setWidgetOrigin(team, 1.0f, 0.5f);
			setWidgetPosition(team, "100%-" + ARMYWIDGET_X_OFFSET_TEAMID,
                formatFloat(ARMYWIDGET_RADIUS) + "px");
			setWidgetOrigin(currentCO, 1.0f, 0.0f);
			setWidgetPosition(currentCO, "100%-" + formatFloat(coX) + "px",
                "5px");
			setWidgetOrigin(tagCO, 1.0f, 0.0f);
			setWidgetPosition(tagCO, "100%-" + formatFloat(coX) + "px", "30px");
			setWidgetOrigin(funds, 1.0f, 0.0f);
			setWidgetPosition(funds, "100%-140px", "5px");
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
