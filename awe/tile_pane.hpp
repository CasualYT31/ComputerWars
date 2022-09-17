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
#include "unit_pane.h"
#include "language.h"

#pragma once

namespace awe {
	/**
	 * Class which represents a tile pane.
	 */
	class tile_pane : public sfx::animated_drawable {
	public:
		/**
		 * Defines the different types of tile pane that can be drawn.
		 * The pane can either be drawn onto the left side of a target, or onto the
		 * right side. These values are intended to be used with
		 * \c setGeneralLocation() to determine if the pane should be rounded on
		 * the left or right.
		 * @sa setGeneralLocation()
		 * @sa draw()
		 */
		enum class location {
			Left,
			Right
		};

		/**
		 * Sets up the static properties of the internal drawing objects.
		 */
		tile_pane() noexcept;

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
		 * Sets this pane's general location.
		 * The default location of a tile pane is left.
		 * @param location The location property of this tile pane.
		 * @sa    \c awe::tile_pane::location
		 */
		void setGeneralLocation(const awe::tile_pane::location& location) noexcept;

		/**
		 * Sets the icon spritesheet to use with this tile pane.
		 * @param sheet Pointer to the icon spritesheet to use with this tile pane.
		 */
		void setSpritesheet(
			const std::shared_ptr<const sfx::animated_spritesheet>& sheet)
			noexcept;

		/**
		 * Sets the font used with this tile pane.
		 * If \c nullptr is given, the call will be ignored.
		 * @param font Pointer to the font to use with this tile pane.
		 */
		void setFont(const std::shared_ptr<const sf::Font>& font) noexcept;

		/**
		 * Sets the language dictionary to use with this tile pane.
		 * If \c nullptr is given, the call will be ignored.
		 * @param dict Pointer to the dictionary to use with this tile pane.
		 */
		void setLanguageDictionary(
			const std::shared_ptr<engine::language_dictionary>& dict) noexcept;

		/**
		 * This drawable's \c animate() method.
		 * @return Always returns \c TRUE.
		 */
		virtual bool animate(const sf::RenderTarget& target,
			const double scaling = 1.0) noexcept;
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
		 * Calculates the points which are used to draw the curve on the edge of
		 * the pane.
		 * @warning Make sure to set \c _bg's size and position \em before calling
		 *          this method!
		 */
		std::vector<sf::Vector2f> _calculateCurvePoints() const noexcept;

		/**
		 * The icon spritesheet to use with this pane.
		 */
		std::shared_ptr<const sfx::animated_spritesheet> _icons = nullptr;

		/**
		 * The font to use with this pane.
		 */
		std::shared_ptr<const sf::Font> _font = nullptr;

		/**
		 * Pointer to the tile to draw information on.
		 */
		std::shared_ptr<const awe::tile> _tile = nullptr;

		/**
		 * The panes which will display information on units.
		 */
		std::vector<awe::unit_pane> _units;

		/**
		 * The main body of the pane.
		 */
		sf::RectangleShape _bg;

		/**
		 * The curve on the edge of the pane.
		 */
		sf::ConvexShape _rounded_bg;

		/**
		 * Defines the type of tile pane to draw.
		 */
		awe::tile_pane::location _location = awe::tile_pane::location::Left;

		/**
		 * The tile graphic.
		 */
		sfx::animated_sprite _tileIcon;

		/**
		 * The tile's short name.
		 */
		sf::Text _tileName;

		/**
		 * The tile's defence icon.
		 */
		sfx::animated_sprite _tileDefIcon;

		/**
		 * The tile's HP icon.
		 */
		sfx::animated_sprite _tileHPIcon;

		/**
		 * The tile's defence stat.
		 */
		sf::Text _tileDef;

		/**
		 * The tile's HP stat.
		 */
		sf::Text _tileHP;

		/**
		 * Pointer to the language dictionary to use.
		 */
		std::shared_ptr<engine::language_dictionary> _dict;
	};
}