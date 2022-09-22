void MoveUnitMenuSetUp() {}

void MoveUnitMenuHandleInput(const dictionary controls) {
	HandleCommonGameInput(controls);
	if (bool(controls["back"])) {
		game.disableMoveMode();
		setGUI("Map");
		return;
	}
}