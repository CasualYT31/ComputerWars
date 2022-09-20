void BeginTurnForTile(const Vector2&in tile, const Terrain&in terrain,
	const ArmyID currentArmy) {
	string terrainName = terrain.scriptName;

	if (terrainName == "CITY" || terrainName == "BASE" || terrainName == "AIRPORT"
		|| terrainName == "PORT" || terrainName == "HQ") {
		game.offsetFunds(currentArmy, 1000);
	}
}