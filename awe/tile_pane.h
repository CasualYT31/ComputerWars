/*Copyright 2019-2022 CasualYouTuber31 <naysar@protonmail.com>

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

/**@file tile_pane.h
 * Declares the class which allows the client to draw the tile pane of a tile.
 */

#include "tile.h"
#include "unit.h"

#pragma once

namespace awe {
	/**
	 * Class which represents a tile pane.
	 */
	class tile_pane : public sfx::animated_drawable {
	public:
		/**
		 * Sets the tile that this pane displays information on.
		 * @param tile The tile to draw information on.
		 */
		void setTile(const awe::tile& tile) noexcept;

		/**
		 * Adds a unit to the tile pane.
		 * @param unit The unit to add to the pane.
		 */
		void addUnit(const awe::unit& unit) noexcept;

		/**
		 * Clears the list of units whose information is displayed.
		 */
		void clearUnits() noexcept;

		/**
		 * This drawable's \c animate() method.
		 * @return Always returns \c TRUE.
		 */
		virtual bool animate(const sf::RenderTarget& target) noexcept;
	private:
		/**
		 * This drawable's \c draw() method.
		 * @param target The target to render the tile pane to.
		 * @param states The render states to apply to the pane. Applying
		 *               transforms is perfectly valid and will not alter the
		 *               internal workings of the drawable.
		 */
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		/**
		 * Pointer to the tile to draw information on.
		 */
		std::shared_ptr<const awe::tile> _tile = nullptr;

		/**
		 * Pointers to units to draw information on.
		 */
		std::vector<std::shared_ptr<const awe::unit>> _units;
	};
}