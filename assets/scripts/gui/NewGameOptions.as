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

	addWidget("VerticalLayout", "Teams");
	setWidgetOrigin("Teams", 1.0, 0.5);
	setWidgetPosition("Teams", "100%", "50%");
	setWidgetSize("Teams", "10%", "200px");
	addWidget("EditBox", "Teams.OS");
	setWidgetDefaultText("Teams.OS", "~OS's Team");
	addWidget("EditBox", "Teams.BM");
	setWidgetDefaultText("Teams.BM", "~BM's Team");
	addWidget("EditBox", "Teams.GE");
	setWidgetDefaultText("Teams.GE", "~GE's Team");
	addWidget("EditBox", "Teams.YC");
	setWidgetDefaultText("Teams.YC", "~YC's Team");

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
	const string menu = PlayMap("map/islandxcopy.cwm");
	if (!menu.isEmpty()) {
		GameOptions opts;
		auto osCurrent = getSelectedItem("COs.OSCurrentList");
		auto bmCurrent = getSelectedItem("COs.BMCurrentList");
		auto geCurrent = getSelectedItem("COs.GECurrentList");
		auto ycCurrent = getSelectedItem("COs.YCCurrentList");
		auto osTag = getSelectedItem("COs.OSTagList");
		auto bmTag = getSelectedItem("COs.BMTagList");
		auto geTag = getSelectedItem("COs.GETagList");
		auto ycTag = getSelectedItem("COs.YCTagList");
		auto osTeam = getWidgetText("Teams.OS");
		auto bmTeam = getWidgetText("Teams.BM");
		auto geTeam = getWidgetText("Teams.GE");
		auto ycTeam = getWidgetText("Teams.YC");

		if (osCurrent >= 0) {
			opts.currentCOs.set("0", commander[osCurrent].scriptName);
		}
		if (bmCurrent >= 0) {
			opts.currentCOs.set("1", commander[bmCurrent].scriptName);
		}
		if (geCurrent >= 0) {
			opts.currentCOs.set("2", commander[geCurrent].scriptName);
		}
		if (ycCurrent >= 0) {
			opts.currentCOs.set("3", commander[ycCurrent].scriptName);
		}

		if (osTag >= 0) {
			opts.tagCOs.set("0", commander[osTag].scriptName);
		} else {
			opts.tagCOs.set("0", "");
		}
		if (bmTag >= 0) {
			opts.tagCOs.set("1", commander[bmTag].scriptName);
		} else {
			opts.tagCOs.set("1", "");
		}
		if (geTag >= 0) {
			opts.tagCOs.set("2", commander[geTag].scriptName);
		} else {
			opts.tagCOs.set("2", "");
		}
		if (ycTag >= 0) {
			opts.tagCOs.set("3", commander[ycTag].scriptName);
		} else {
			opts.tagCOs.set("3", "");
		}

		if (osTeam.length() > 0) {
			opts.teams.set("0", parseUInt(osTeam));
		}
		if (bmTeam.length() > 0) {
			opts.teams.set("1", parseUInt(bmTeam));
		}
		if (geTeam.length() > 0) {
			opts.teams.set("2", parseUInt(geTeam));
		}
		if (ycTeam.length() > 0) {
			opts.teams.set("3", parseUInt(ycTeam));
		}
		game.overrideWithOptions(opts);
		setGUI(menu);
	}
}