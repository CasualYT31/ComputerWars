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
	 * @param panelName The full name of the panel which contains all of the
	 *                  other widgets.
	 */
	ArmyWidget(const string&in panelName) explicit {
		panel = panelName;
		addWidget("Panel", panel);
		setWidgetSize(panel, "200px", "50px");
		setWidgetBorderSize(panel, 2.0f);
		setWidgetBorderRadius(panel, 25.0);

		currentCO = panelName + ".currentCO";
		addWidget("Picture", currentCO);

		tagCO = panelName + ".tagCO";
		addWidget("Picture", tagCO);

		funds = panelName + ".funds";
		addWidget("Label", funds);

		setAlignment(ArmyWidgetAlignment::Left);
	}

	/**
	 * Configures an army widget to display information on a given army.
	 * @param armyID The ID of the army on the current map to display information
	 *               on.
	 */
	void update(const ArmyID armyID) {
		setWidgetBackgroundColour(panel, game.getArmyCountry(armyID).colour);

		Commander currentCommander = game.getArmyCurrentCO(armyID);
		info(currentCommander.iconName);
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
		switch (alignment) {
		case ArmyWidgetAlignment::Left:
			setWidgetPosition(panel, "0px", "0px");
			setWidgetPosition(currentCO, "30px", "5px");
			setWidgetPosition(tagCO, "30px", "30px");
			setWidgetPosition(funds, "150px", "5px");
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
}