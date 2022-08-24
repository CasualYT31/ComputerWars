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

/**@file game.h
 * Declares the class which is used to manage a game of Computer Wars.
 */

#include "renderer.h"
#include "map.h"
#include "userinput.h"
#include "gui.h"

#pragma once

namespace awe {
	class game_engine;

	/**
	 * Class which represents a map with game logic and user input.
	 * @sa @c awe::map
	 */
	class game : sf::NonCopyable, public sfx::animated_drawable {
	public:
		/**
		 * Initialises the internal logger object.
		 * @param name The name to give this particular instantiation within the
		 *             log file. Defaults to "game."
		 * @sa    \c engine::logger
		 */
		game(const std::string& name = "game") noexcept;

		/**
		 * Sets up a game based on what @c map requires.
		 * @param file       Path to the binary file containing the map to play on.
		 * @param countries  Information on the countries to search through when
		 *                   reading country IDs from the map file.
		 * @param tiles      Information on the tile types to search through when
		 *                   reading tile type IDs from the map file.
		 * @param units      Information on the unit types to search through when
		 *                   reading unit type IDs from the map file.
		 * @param commanders Information on the commanders to search through when
		 *                   reading CO IDs from the map file.
		 * @param tile_sheet Pointer to the animated spritesheet to use for tiles.
		 * @param unit_sheet Pointer to the animated spritesheet to use for units.
		 * @param icon_sheet Pointer to the animated spritesheet to use for icons.
		 * @param co_sheet   Pointer to the animated spritesheet to use for COs.
		 * @param font       Pointer to the font to use with this map.
		 * @sa    @c awe::map::load()
		 */
		bool load(const std::string& file,
			const std::shared_ptr<awe::bank<awe::country>>& countries,
			const std::shared_ptr<awe::bank<awe::tile_type>>& tiles,
			const std::shared_ptr<awe::bank<awe::unit_type>>& units,
			const std::shared_ptr<awe::bank<awe::commander>>& commanders,
			const std::shared_ptr<sfx::animated_spritesheet>& tile_sheet,
			const std::shared_ptr<sfx::animated_spritesheet>& unit_sheet,
			const std::shared_ptr<sfx::animated_spritesheet>& icon_sheet,
			const std::shared_ptr<sfx::animated_spritesheet>& co_sheet,
			const std::shared_ptr<sf::Font>& font) noexcept;

		/**
		 * Saves the state of the map to the previously given binary file.
		 * Returns @c FALSE if the map hasn't been previously <tt>load()</tt>ed.
		 * @sa @c awe::map::save()
		 */
		bool save() noexcept;

		/**
		 * Destroys the map object.
		 */
		void quit() noexcept;

		/////////////////////
		// BEGIN INTERFACE //
		/////////////////////

		/**
		 * @sa @c awe::map::moveSelectedTileUp().
		 */
		void moveSelectedTileUp() noexcept;

		/**
		 * @sa @c awe::map::moveSelectedTileDown().
		 */
		void moveSelectedTileDown() noexcept;

		/**
		 * @sa @c awe::map::moveSelectedTileLeft().
		 */
		void moveSelectedTileLeft() noexcept;

		/**
		 * @sa @c awe::map::moveSelectedTileRight().
		 */
		void moveSelectedTileRight() noexcept;

		/**
		 * @sa @c awe::map::getSelectedTile().
		 */
		sf::Vector2u getSelectedTile() const noexcept;

		/**
		 * @sa @c awe::map::getUnitOnTile().
		 */
		awe::UnitID getUnitOnTile(const sf::Vector2u tile) const noexcept;

		/////////////////////
		//  END  INTERFACE //
		/////////////////////

		/**
		 * This drawable's \c animate() method.
		 * @param  target The target to render the map to.
		 * @return \c FALSE, for now.
		 */
		virtual bool animate(const sf::RenderTarget& target,
			const double scaling = 1.0) noexcept;
	private:
		/**
		 * This drawable's \c draw() method.
		 * @param target The target to render the map to.
		 * @param states The render states to apply to the map. Applying transforms
		 *               is perfectly valid and will not alter the internal
		 *               workings of the drawable.
		 * @sa @c awe::map::draw()
		 */
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		/**
		 * Internal logger object.
		 */
		mutable engine::logger _logger;

		/**
		 * Stores the map.
		 */
		std::unique_ptr<awe::map> _map = nullptr;
	};
}