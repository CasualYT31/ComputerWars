void MainMenuSetUp() {
	setBackground(200, 200, 200, 255, "MainMenu");
	addListBox("FileSelect", 5.0, 5.0, 200.0, 100.0);
	// Collect a list of available map files and add them to the listbox.
	filesystem cwd;
	cwd.changeCurrentPath("./assets/map");
	array<string> files = cwd.getFiles();
	for (uint f = 0; f < files.length(); f++) {
		// Only look for *.cwm files!
		if (files[f].findLast(".") >= 0) {
			if (files[f].substr(files[f].findLast(".")) == ".cwm") {
				addListBoxItem("FileSelect", "~" + files[f]);
			}
		}
	}
}

void MainMenu_FileSelect_ItemSelected() {
	string item = getListBoxSelectedItem("FileSelect");
	info(item);
}

void MapSetUp() {}