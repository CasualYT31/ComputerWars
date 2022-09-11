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
	 * Creates the widgets that represent an army panel.
	 * @param panelName The full name of the panel which contains all of the
	 *                  other widgets.
	 */
	ArmyWidget(const string&in panelName) explicit {
		addWidget("Panel", panelName);
		// setWidgetBackgroundColour(panelName, );
		addWidget("Picture", panelName + ".currentCO");
		addWidget("Picture", panelName + ".tagCO");
		addWidget("Label", panelName + ".funds");
	}

	/**
	 * Configures an army widget to display information on a given army.
	 * @param armyID The ID of the army on the current map to display information
	 *               on.
	 */
	void update(const uint armyID) {
	}

	/**
	 * Configures an army widget to be either left-aligned or right-aligned.
	 */
	void setAlignment(const ArmyWidgetAlignment alignment) {
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