/**
 * This is intended to be used purely for debugging, so I've not made any effort
 * to make this script scalable.
 */

void NewGameOptionsSetUp() {
	setBackground("", 128, 128, 128, 255);
	addWidget("Button", "GoBack");
	setWidgetText("GoBack", "~Back");
	setWidgetPosition("GoBack", "5%", "5%");
	addWidget("Button", "Play");
	setWidgetText("Play", "~Play");
	setWidgetPosition("Play", "95%", "95%");
	setWidgetOrigin("Play", 1.0, 1.0);
	addWidget("Button", "DeletePlay");
	setWidgetText("DeletePlay", "~Delete & Play");
	setWidgetPosition("DeletePlay", "5%", "95%");
	setWidgetOrigin("DeletePlay", 0.0, 1.0);

	addWidget("Grid", "COs");
	setWidgetOrigin("COs", 0.5, 0.5);
	setWidgetPosition("COs", "50%", "50%");
	setWidgetSize("COs", "50%", "200px");

	addWidgetToGrid("Label", "COs.OSCurrentLabel", 0, 0);
	addWidgetToGrid("Label", "COs.BMCurrentLabel", 1, 0);
	addWidgetToGrid("Label", "COs.GECurrentLabel", 2, 0);
	addWidgetToGrid("Label", "COs.YCCurrentLabel", 3, 0);
	setWidgetText("COs.OSCurrentLabel", "~OS Current CO");
	setWidgetText("COs.BMCurrentLabel", "~BM Current CO");
	setWidgetText("COs.GECurrentLabel", "~GE Current CO");
	setWidgetText("COs.YCCurrentLabel", "~YC Current CO");

	addWidgetToGrid("Listbox", "COs.OSCurrentList", 0, 1);
	addWidgetToGrid("Listbox", "COs.BMCurrentList", 1, 1);
	addWidgetToGrid("Listbox", "COs.GECurrentList", 2, 1);
	addWidgetToGrid("Listbox", "COs.YCCurrentList", 3, 1);
	setWidgetSize("COs.OSCurrentList", "100px", "50px");
	setWidgetSize("COs.BMCurrentList", "100px", "50px");
	setWidgetSize("COs.GECurrentList", "100px", "50px");
	setWidgetSize("COs.YCCurrentList", "100px", "50px");
	addItem("COs.OSCurrentList", "~" + commander["NOCO"].name);
	addItem("COs.OSCurrentList", "~" + commander["JAKE"].name);
	addItem("COs.BMCurrentList", "~" + commander["NOCO"].name);
	addItem("COs.BMCurrentList", "~" + commander["JAKE"].name);
	addItem("COs.GECurrentList", "~" + commander["NOCO"].name);
	addItem("COs.GECurrentList", "~" + commander["JAKE"].name);
	addItem("COs.YCCurrentList", "~" + commander["NOCO"].name);
	addItem("COs.YCCurrentList", "~" + commander["JAKE"].name);

	addWidgetToGrid("Label", "COs.OSTagLabel", 0, 2);
	addWidgetToGrid("Label", "COs.BMTagLabel", 1, 2);
	addWidgetToGrid("Label", "COs.GETagLabel", 2, 2);
	addWidgetToGrid("Label", "COs.YCTagLabel", 3, 2);
	setWidgetText("COs.OSTagLabel", "~OS Tag CO");
	setWidgetText("COs.BMTagLabel", "~BM Tag CO");
	setWidgetText("COs.GETagLabel", "~GE Tag CO");
	setWidgetText("COs.YCTagLabel", "~YC Tag CO");

	addWidgetToGrid("Listbox", "COs.OSTagList", 0, 3);
	addWidgetToGrid("Listbox", "COs.BMTagList", 1, 3);
	addWidgetToGrid("Listbox", "COs.GETagList", 2, 3);
	addWidgetToGrid("Listbox", "COs.YCTagList", 3, 3);
	setWidgetSize("COs.OSTagList", "100px", "50px");
	setWidgetSize("COs.BMTagList", "100px", "50px");
	setWidgetSize("COs.GETagList", "100px", "50px");
	setWidgetSize("COs.YCTagList", "100px", "50px");
	addItem("COs.OSTagList", "~" + commander["NOCO"].name);
	addItem("COs.OSTagList", "~" + commander["JAKE"].name);
	addItem("COs.BMTagList", "~" + commander["NOCO"].name);
	addItem("COs.BMTagList", "~" + commander["JAKE"].name);
	addItem("COs.GETagList", "~" + commander["NOCO"].name);
	addItem("COs.GETagList", "~" + commander["JAKE"].name);
	addItem("COs.YCTagList", "~" + commander["NOCO"].name);
	addItem("COs.YCTagList", "~" + commander["JAKE"].name);

	/*
	Original map file has these COs:
	OS: Jake, NOCO
	BM: NOCO
	GE: JAKE, JAKE
	YC: NOCO, NOCO
	*/
	// setSelectedItem("COs.OSCurrentList", 1);
	// setSelectedItem("COs.BMCurrentList", 0);
	// setSelectedItem("COs.GECurrentList", 1);
	// setSelectedItem("COs.YCCurrentList", 0);
	// setSelectedItem("COs.OSTagList", 0);
	// setSelectedItem("COs.GETagList", 1);
	// setSelectedItem("COs.YCTagList", 0);
}

void NewGameOptions_GoBack_MouseReleased() {
	setGUI("MainMenu");
}

void NewGameOptions_DeletePlay_MouseReleased() {
	// Delete any existing copy of Island X before playing.
	filesystem fs;
	fs.changeCurrentPath("./map");
	fs.deleteFile("islandxcopy.cwm");
	NewGameOptions_Play_MouseReleased();
}

void NewGameOptions_Play_MouseReleased() {
	filesystem fs;
	fs.changeCurrentPath("./map");
	fs.copyFile("islandx.cwm", "islandxcopy.cwm");
	// Setup the game options, then load the map.
	GameOptions opts;
	auto osCurrent = getSelectedItem("COs.OSCurrentList");
	auto bmCurrent = getSelectedItem("COs.BMCurrentList");
	auto geCurrent = getSelectedItem("COs.GECurrentList");
	auto ycCurrent = getSelectedItem("COs.YCCurrentList");
	auto osTag = getSelectedItem("COs.OSTagList");
	auto bmTag = getSelectedItem("COs.BMTagList");
	auto geTag = getSelectedItem("COs.GETagList");
	auto ycTag = getSelectedItem("COs.YCTagList");
	if (osCurrent >= 0) opts.setCurrentCO(0, osCurrent);
	if (bmCurrent >= 0) opts.setCurrentCO(1, bmCurrent);
	if (geCurrent >= 0) opts.setCurrentCO(2, geCurrent);
	if (ycCurrent >= 0) opts.setCurrentCO(3, ycCurrent);
	if (osTag >= 0) {
		opts.setTagCO(0, osTag);
	} else {
		opts.setNoTagCO(0, true);
	}
	if (bmTag >= 0) {
		opts.setTagCO(1, bmTag);
	} else {
		opts.setNoTagCO(1, true);
	}
	if (geTag >= 0) {
		opts.setTagCO(2, geTag);
	} else {
		opts.setNoTagCO(2, true);
	}
	if (ycTag >= 0) {
		opts.setTagCO(3, ycTag);
	} else {
		opts.setNoTagCO(3, true);
	}
	loadMap("map/islandxcopy.cwm", "Map", opts);
}