/**
 * @file ArmyWidget.as
 * Declares the army widget type.
 */

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
		setWidgetBackgroundColour(panelName, Colour());
		addWidget("Picture", panelName + ".currentCO");
		addWidget("Picture", panelName + ".tagCO");
		addWidget("Label", panelName + ".funds");
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