void MapMenuSetUp() {
	addVerticalLayout("Menu", "(parent.innersize-size)/2", "5px", "25%", "10%");
	// The vertical layout container will resize and reposition all of the buttons.
	addBitmapButton("MapMenu.Menu.Save", 0, 0, 0, 0);
	setBitmapButtonSprite("MapMenu.Menu.Save", "saveicon");
	setBitmapButtonText("MapMenu.Menu.Save", "Save");
	addBitmapButton("MapMenu.Menu.Quit", 0, 0, 0, 0);
	setBitmapButtonSprite("MapMenu.Menu.Quit", "quiticon");
	setBitmapButtonText("MapMenu.Menu.Quit", "Quit");
}

void MapMenu_Save_Clicked() {
	saveMap();
	setGUI("Map");
}

void MapMenu_Quit_Clicked() {
	quitMap();
	// quitMap() will switch to a different menu for us in the future.
	setGUI("MainMenu");
}