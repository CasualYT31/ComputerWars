/*Copyright 2019-2021 CasualYouTuber31 <naysar@protonmail.com>

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
 * @file typedef.h
 * Defines a series of typedefs specific to Computer Wars. These were added so that
 * the sizes of these values could be easily amended after implementation.
 */

#pragma once

#include "sfml/Config.hpp"

namespace awe {
	/**
	 * Typedef representing a team ID.
	 */
	typedef sf::Uint8 TeamID;

	/**
	 * Typedef representing a generic bank ID.
	 */
	typedef sf::Uint32 BankID;

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
	 * \c 0 should be reserved.
	 * @warning Do \b not make this a signed type!
	 * @sa      awe::map::_findUnitID()
	 */
	typedef sf::Uint32 UnitID;

	/**
	 * Typedef representing an army ID.
	 * \c UINT32_MAX should be reserved, even if it isn't officially in bank.h...
	 * @warning Do \b not make this a signed type!
	 * @sa      awe::army::NO_ARMY
	 */
	typedef awe::BankID ArmyID;
}