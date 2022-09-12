void MainMenuSetUp() {
	setBackground("", 200, 200, 200, 0);
	addWidget("ListBox", "FileSelect");
	setWidgetPosition("FileSelect", "5px", "5px");
	setWidgetSize("FileSelect", "200px", "200px");
}

void MainMenuOpen(const string&in prev) {
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