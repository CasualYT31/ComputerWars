void BeginTurnForArmy(const array<UnitID>@ units, const array<Vector2>@ tiles) {
	const uint size = units.length();
	for (uint i = 0; i < size; ++i) {
		// If unit has infinite fuel, skip fuel checks.
		const Unit unit = game.getUnitType(units[i]);
		if (unit.hasInfiniteFuel) continue;

		string type = unit.movementType.scriptName;
		if (type == "AIR" || type == "SHIPS" || type == "TRANSPORT") {
			if (game.getUnitFuel(units[i]) <= 0) game.deleteUnit(units[i]);
		}
	}
}