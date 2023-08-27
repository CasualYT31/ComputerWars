/*Copyright 2019-2023 CasualYouTuber31 <naysar@protonmail.com>

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

/**
 * @file typedef.hpp
 * Defines a series of typedefs specific to Computer Wars. These were added so that
 * the sizes of these values could be easily amended after implementation.
 */

#pragma once

#include "sfml/Config.hpp"
#include "angelscript.h"
#include <memory>
#include <limits>
#include "docgen.h"

namespace awe {
	/**
	 * Typedef representing a team ID.
	 */
	typedef sf::Uint8 TeamID;

	/**
	 * Typedef representing funds.
	 * I decided to make this signed because fund calculations will result in
	 * negative values more often than not, and they should be handled properly.
	 * This reasoning can be applied to pretty much all of my signed int typedefs.
	 */
	typedef sf::Int32 Funds;

	/**
	 * Typedef representing a tile's or unit's HP.
	 */
	typedef sf::Int32 HP;

	/**
	 * Typedef representing a unit's fuel.
	 */
	typedef sf::Int32 Fuel;

	/**
	 * Typedef representing a unit's ammo.
	 */
	typedef sf::Int32 Ammo;

	/**
	 * Typedef representing a unit ID.
	 */
	typedef sf::Uint32 UnitID;

	/**
	 * Reserved value representing no unit.
	 * Assigned the minimum value of an \c awe::UnitID.
	 */
	constexpr awe::UnitID NO_UNIT = std::numeric_limits<awe::UnitID>::min();

	/**
	 * The ID of the first unit that'll be created.
	 */
	constexpr awe::UnitID ID_OF_FIRST_UNIT = NO_UNIT + 1;

	/**
	 * Typedef representing an army ID.
	 * @warning Do \b not make this a signed type!
	 * @sa      @c awe::army::NO_ARMY
	 */
	typedef sf::Uint32 ArmyID;

	/**
	 * Reserved value representing no army.
	 * Assigned the maximum value of the unsigned type of \c awe::ArmyID.
	 */
	constexpr awe::ArmyID NO_ARMY = std::numeric_limits<awe::ArmyID>::max();

	/**
	 * Typedef representing a day number.
	 */
	typedef sf::Uint32 Day;

	/**
	 * Registers game-related typedefs with a given AngelScript engine, if any of
	 * them haven't already been registered.
	 * @safety No guarantee.
	 */
	void RegisterGameTypedefs(asIScriptEngine* engine,
		const std::shared_ptr<DocumentationGenerator>& document);
}
