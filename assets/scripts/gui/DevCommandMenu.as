/**
 * @file DevCommandMenu.as
 * Contains the menu which allows the developer to run code on-the-fly.
 */

/**
 * Adds the widgets to the developer command menu.
 */
void DevCommandMenuSetUp() {
	addWidget("EditBox", "Code");
	setWidgetSize("Code", "100%", "30px");
	setWidgetOrigin("Code", 0.0, 1.0);
	setWidgetPosition("Code", "0%", "100%");
	setWidgetTextSize("Code", 22);

	addWidget("Button", "Back");
	setWidgetText("Back", "back");
	setWidgetSize("Back", "12.5%", "30px");
	setWidgetOrigin("Back", 1.0, 1.0);
	setWidgetPosition("Back", "100%", "DevCommandMenu.Code.top");
	setWidgetTextSize("Back", 22);
	
	addWidget("Label", "Error");
	setWidgetTextColour("Error", Colour(128, 0, 0, 255));
	setWidgetTextOutlineColour("Error", Colour(255, 255, 255, 255));
	setWidgetTextOutlineThickness("Error", 2.0);
	setWidgetOrigin("Error", 0.0, 1.0);
	setWidgetPosition("Error", "0%", "DevCommandMenu.Code.top");
	setWidgetSize("Error", "87.5%", "30px");
	setWidgetTextSize("Error", 22);
}

/**
 * Whenever the developer command menu opens, always set the focus to the edit
 * box.
 */
void DevCommandMenuOpen() {
	setWidgetFocus("Code");
}

/**
 * When the user clicks the Back button, go back to the \c Map menu.
 */
void DevCommandMenu_Back_Pressed() {
	setGUI("Map");
}

/**
 * When the user hits enter on the edit box, execute the code!
 */
void DevCommandMenu_Code_ReturnKeyPressed() {
	string result = execute(getWidgetText("Code"));
	setWidgetText("Error", "~" + result);
	if (result.isEmpty()) {
		setGUI("Map");
	}
}