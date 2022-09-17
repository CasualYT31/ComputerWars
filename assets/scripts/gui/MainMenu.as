void MainMenuSetUp() {
	setGlobalFont("AW2");
	setBackground("", 200, 200, 200, 0);
	addWidget("ListBox", "FileSelect");
	setWidgetSize("FileSelect", "50%", "50%");
	setWidgetPosition("FileSelect", "25%", "50%");
	setWidgetOrigin("FileSelect", 0.25, 0.5);
	addWidget("Button", "NewGame");
	setWidgetPosition("NewGame", "10%", "10%");
	setWidgetText("NewGame", "~New Game");
}

void MainMenuOpen() {
	// Collect a list of available map files and add them to the listbox.
	clearItems("FileSelect");
	filesystem cwd;
	cwd.changeCurrentPath("./map");
	array<string> files = cwd.getFiles();
	for (uint f = 0; f < files.length(); f++) {
		// Only look for *.cwm files!
		if (files[f].findLast(".") >= 0) {
			if (files[f].substr(files[f].findLast(".")) == ".cwm") {
				addItem("FileSelect", "~" + files[f]);
			}
		}
	}
}

void MainMenu_FileSelect_MouseReleased() {
	string item = getSelectedItemText("FileSelect");
	loadMap("map/" + item, "Map");
}

void MainMenu_NewGame_MouseReleased() {
	setGUI("NewGameOptions");
}