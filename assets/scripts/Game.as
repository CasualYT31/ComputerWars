void tileHasBeenSelected(Vector2 tile) {
	if (getUnitOnTile(tile) == 0) {
		setGUI("MapMenu");
	}
}