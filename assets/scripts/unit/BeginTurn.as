void BeginTurnForUnit(const UnitID unit, const UnitType&in type,
	const Vector2&in position) {
	string typeName = type.scriptName;

	if (typeName == "APC") {
		array<UnitID> units = game.getAdjacentUnits(position, 1, 1);
		auto apcArmy = game.getArmyOfUnit(unit);
		for (uint i = 0; i < units.length(); ++i) {
			if (apcArmy == game.getArmyOfUnit(units[i]))
				game.replenishUnit(units[i]);
		}

	} else if (typeName == "TCOPTER" || typeName == "BCOPTER") {
		game.burnFuel(unit, 2);
	
	} else if (typeName == "FIGHTER" || typeName == "BOMBER" ||
		typeName == "STEALTH" || typeName == "BLACKBOMB") {
		game.burnFuel(unit, 5);
	
	} else if (typeName == "BLACKBOAT" || typeName == "LANDER" ||
		typeName == "CRUISER" || typeName == "SUB" ||
		typeName == "BATTLESHIP" || typeName == "CARRIER") {
		game.burnFuel(unit, 1);

	}
}