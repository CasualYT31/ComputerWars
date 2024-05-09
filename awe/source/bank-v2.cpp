/*Copyright 2019-2024 CasualYouTuber31 <naysar@protonmail.com>

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "bank-v2.hpp"

std::function<void(awe::overrides&)> awe::overrides::_factory = {};

DEFINE_POD_5(awe, particle_data, "ParticleData",
	std::string, sheet,
	std::string, spriteID,
	float, density,
	sf::Vector2f, vector,
	sf::Time, respawnDelay
);

DEFINE_POD_3(awe, root_structure_tile, "RootStructureTile",
	std::string, tile,
	std::string, destroyed,
	std::string, deleted
);

DEFINE_POD_4(awe, dependent_structure_tile, "DependentStructureTile",
	std::string, tile,
	sf::Vector2i, offset,
	std::string, destroyed,
	std::string, deleted
);

DEFINE_POD_7(awe, unit_sprite_info, "UnitSpriteInfo",
	std::string, idleSheet,
	std::string, upSheet,
	std::string, downSheet,
	std::string, leftSheet,
	std::string, rightSheet,
	std::string, selectedSheet,
	std::string, sprite
);

DEFINE_POD_5(awe, unit_sound_info, "UnitSoundInfo",
	std::string, move,
	std::string, destroy,
	std::string, moveHidden,
	std::string, hide,
	std::string, unhide
);

DEFINE_POD_3(awe, weapon_damage, "WeaponDamage",
	std::string, target,
	sf::Int32, damage,
	sf::Int32, damageWhenHidden
);

awe::ArmyID awe::country::_turnOrderCounter = 0;
