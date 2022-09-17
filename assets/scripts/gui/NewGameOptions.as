void NewGameOptionsSetUp() {
	addWidget("Button", "GoBack");
	setWidgetText("GoBack", "~Back");
	setWidgetPosition("GoBack", "5%", "5%");
}

void NewGameOptions_GoBack_MouseReleased() {
	setGUI(PREVIOUS_MENU);
}