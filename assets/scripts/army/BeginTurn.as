void BeginTurnForArmy(const ArmyID currentArmy, const array<UnitID>@ units,
	const array<Vector2>@ tiles) {
	// Property heal and replenish code.
	const uint tilesSize = tiles.length();
	for (uint i = 0; i < tilesSize; ++i) {
		const UnitID unitOnTile = game.getUnitOnTile(tiles[i]);
		if (unitOnTile > 0) {
			if (game.getArmyOfUnit(unitOnTile) == currentArmy) {
				// Heal and replenish the unit based on the tile and unit's
				// movement type.
				const string unitType =
					game.getUnitType(unitOnTile).movementType.scriptName;
				const string terrainType =
					game.getTerrainOfTile(tiles[i]).scriptName;
				if (terrainType == "CITY" || terrainType == "BASE" ||
					terrainType == "HQ") {
					if (unitType == "TREAD" || unitType == "TIRES" ||
						unitType == "INFANTRY" || unitType == "MECH" ||
						unitType == "PIPELINE") {
						game.healUnit(unitOnTile, 2);
						game.replenishUnit(unitOnTile);
					}
				} else if (terrainType == "AIRPORT") {
					if (unitType == "AIR") {
						game.healUnit(unitOnTile, 2);
						game.replenishUnit(unitOnTile);
					}
				} else if (terrainType == "PORT") {
					if (unitType == "SHIPS" || unitType == "TRANSPORT") {
						game.healUnit(unitOnTile, 2);
						game.replenishUnit(unitOnTile);
					}
				}
			}
		}
	}

	// Plane and ship crash checks.
	const uint unitsSize = units.length();
	for (uint i = 0; i < unitsSize; ++i) {
		// If unit has infinite fuel, skip fuel checks.
		const UnitType unit = game.getUnitType(units[i]);
		if (unit.hasInfiniteFuel) continue;

		string type = unit.movementType.scriptName;
		if (type == "AIR" || type == "SHIPS" || type == "TRANSPORT") {
			if (game.getUnitFuel(units[i]) <= 0) game.deleteUnit(units[i]);
		}
	}
}