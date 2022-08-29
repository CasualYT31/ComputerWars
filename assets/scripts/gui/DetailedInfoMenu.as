void DetailedInfoMenuSetUp() {}

void DetailedInfoMenuHandleInput(const dictionary controls) {
	if (bool(controls["info"]) || bool(controls["back"])) {
		setGUI("Map");
	}
}