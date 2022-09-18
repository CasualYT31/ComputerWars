void BeginTurnForUnit(const UnitID unit, const Unit&in type,
	const Vector2&in position) {
	if (type.scriptName == "APC") {
		array<UnitID> units = game.getAdjacentUnits(position);
		auto apcArmy = game.getArmyOfUnit(unit);
		for (uint i = 0; i < units.length(); ++i) {
			if (apcArmy == game.getArmyOfUnit(units[i]))
				game.replenishUnit(units[i]);
		}
	}
}