void MainMenuSetUp() {
	setBackground(200, 200, 200, 255, "MainMenu");
	addListBox("FileSelect", 5.0, 5.0, 200.0, 100.0);
	addListBoxItem("FileSelect", "Moon Isle");
	addListBoxItem("FileSelect", "Island X");
}

void MainMenu_FileSelect_ItemSelected() {
	string item = getListBoxSelectedItem("FileSelect");
	info(item);
}