void MainMenuSetUp() {
	setBackground(200, 200, 200, 255, "MainMenu");
	addListBox("List", 5.0, 5.0, 200.0, 200.0);
	addListBoxItem("MainMenu.List", "opt1");
	addListBoxItem("MainMenu.List", "opt2");
	addListBoxItem("MainMenu.List", "opt3");
	addListBoxItem("List", "~Other...");
	addListBox("Lang", 500.0, 5.0, 200.0, 200.0);
	addListBoxItem("Lang", "lang1");
	addListBoxItem("Lang", "lang2");
}

void MainMenu_Lang_ItemSelected() {
	TEST_changeLanguage(getListBoxSelectedItem("Lang"));
}