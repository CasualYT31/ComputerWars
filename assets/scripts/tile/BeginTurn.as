void BeginTurnForTile(const Vector2&in tile, const Terrain&in terrain,
	const ArmyID currentArmy) {
	string terrainName = terrain.scriptName;

	if (terrainName == "CITY" || terrainName == "BASE" || terrainName == "AIRPORT"
		|| terrainName == "PORT" || terrainName == "HQ") {
		game.offsetFunds(currentArmy, 1000);

		const auto unit = game.getUnitOnTile(tile);
		const auto army = game.getTileOwner(tile);
		if (unit > 0 && army == currentArmy) {
			const auto unitType = game.getUnitType(unit);
			const string type = unitType.movementType.scriptName;

			// We only charge for *displayed* HP.
			// Any internal HP that also goes up can be awarded for free,
			// e.g. unit is at 10HP already but not exactly at 100 internal HP.
			if (type == "TREADS" || type == "TIRES" || type == "INFANTRY" ||
				type == "MECH" || type == "PIPELINE") {
				if (terrainName == "CITY" || terrainName == "BASE" ||
					terrainName == "HQ") {
					// Heal and replenish.
					const HP healed = game.healUnit(unit, getInternalHP(2));
					game.replenishUnit(unit);
				}
			}
			
			if (type == "AIR") {
				if (terrainName == "AIRPORT") {
					// Heal and replenish.
					const HP healed = game.healUnit(unit, getInternalHP(2));
					game.replenishUnit(unit);
				}
			}
			
			if (type == "SHIPS" || type == "TRANSPORT") {
				if (terrainName == "PORT") {
					// Heal and replenish.
					const HP healed = game.healUnit(unit, getInternalHP(2));
					game.replenishUnit(unit);
				}
			}
		}
	}
}