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

/**@file game.hpp
 * Declares the class which is used to manage a game of Computer Wars.
 */

#include "renderer.hpp"
#include "map.hpp"
#include "userinput.hpp"
#include "gui.hpp"
#include "options.hpp"

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
		 * Sets the script engine to use with this game.
		 * @param scripts Pointer to the script engine.
		 */
		void setScripts(const std::shared_ptr<engine::scripts>& scripts) noexcept;

		/**
		 * Sets up a game based on what @c map requires.
		 * When calling \c load(), \c game will check if it has a pointer to game
		 * options. If it does, it will use those options to replace values that
		 * are stored in the map file. E.g., setting an army's CO/s for a new game,
		 * and then not providing any options when loading the map again, as those
		 * COs will have been saved to the map by then.
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
		 * @param dict       Pointer to the language dictionary to use with this
		 *                   map.
		 * @param options    Reference to a set of game options to apply to this
		 *                   game. This method <b>will not</b> release this
		 *                   reference! \c nullptr can be given.
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
			const std::shared_ptr<sf::Font>& font,
			const std::shared_ptr<engine::language_dictionary>& dict,
			awe::game_options* options) noexcept;

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
		 * @throws std::runtime_error if no map is currently loaded.
		 * @sa     @c awe::map::moveSelectedTileUp().
		 */
		void moveSelectedTileUp();

		/**
		 * @throws std::runtime_error if no map is currently loaded.
		 * @sa     @c awe::map::moveSelectedTileDown().
		 */
		void moveSelectedTileDown();

		/**
		 * @throws std::runtime_error if no map is currently loaded.
		 * @sa     @c awe::map::moveSelectedTileLeft().
		 */
		void moveSelectedTileLeft();

		/**
		 * @throws std::runtime_error if no map is currently loaded.
		 * @sa     @c awe::map::moveSelectedTileRight().
		 */
		void moveSelectedTileRight();

		/**
		 * @throws std::runtime_error if no map is currently loaded.
		 * @sa     @c awe::map::getSelectedTile().
		 */
		sf::Vector2u getSelectedTile() const;

		/**
		 * @throws std::runtime_error if no map is currently loaded.
		 * @sa     @c awe::map::getUnitOnTile().
		 */
		awe::UnitID getUnitOnTile(const sf::Vector2u tile) const;

		/**
		 * Increases the map scaling factor by @c 1.0.
		 * The map scaling factor does not go above @c 3.0.
		 * @throws std::runtime_error if no map is currently loaded.
		 * @sa     @c awe::map::setMapScalingFactor().
		 */
		void zoomIn();

		/**
		 * Decreases the map scaling factor by @c 1.0.
		 * The map scaling factor does not go below @c 1.0.
		 * @throws std::runtime_error if no map is currently loaded.
		 * @sa     @c awe::map::setMapScalingFactor().
		 */
		void zoomOut();

		/**
		 * @throws std::runtime_error if no map is currently loaded.
		 * @sa     @c awe::map::setSelectedTileByPixel().
		 */
		void setSelectedTileByPixel(const sf::Vector2i pixel);

		/**
		 * @throws std::runtime_error if no map is currently loaded.
		 * @sa     @c awe::map::getTileSize().
		 */
		sf::Vector2u getTileSize() const;

		/**
		 * @throws std::runtime_error if no map is currently loaded, or if the
		 *                            current CO couldn't be retrieved.
		 * @sa     @c awe::map::getArmyCurrentCO().
		 */
		const awe::commander getArmyCurrentCO(const awe::ArmyID army) const;

		/**
		 * @throws std::runtime_error if no map is currently loaded, or if the
		 *                            tag CO couldn't be retrieved.
		 * @sa     @c awe::map::getArmyTagCO().
		 */
		const awe::commander getArmyTagCO(const awe::ArmyID army) const;

		/**
		 * @throws std::runtime_error if no map is currently loaded, or if the
		 *                            country couldn't be retrieved.
		 * @sa     @c awe::map::getArmyCountry().
		 */
		const awe::country getArmyCountry(const awe::ArmyID army) const;

		/**
		 * @throws std::runtime_error if no map is currently loaded, or if the
		 *                            funds couldn't be retrieved.
		 * @sa     @c awe::map::getArmyFunds().
		 */
		awe::Funds getArmyFunds(const awe::ArmyID army) const;

		/**
		 * @throws std::runtime_error if no map is currently loaded.
		 * @sa     @c awe::map::tagCOIsPresent().
		 */
		bool tagCOIsPresent(const awe::ArmyID army) const;

		/**
		 * @throws std::runtime_error if no map is currently loaded.
		 * @sa     @c awe::map::getArmyCount().
		 */
		std::size_t getArmyCount() const;

		/**
		 * @throws std::runtime_error if no map is currently loaded, or if the tile
		 *                            location given was out of range.
		 * @sa     @c awe::map::getTileType().
		 */
		const awe::tile_type getTileType(const sf::Vector2u& pos) const;

		/**
		 * @throws std::runtime_error if no map is currently loaded, or if the tile
		 *                            location given was out of range.
		 * @sa     @c awe::map::getTileType().
		 */
		const awe::terrain getTerrainOfTile(const sf::Vector2u& pos) const;

		/**
		 * @throws std::runtime_error if no map is currently loaded.
		 * @sa     @c awe::map::getTileOwner().
		 */
		awe::ArmyID getTileOwner(const sf::Vector2u& pos) const;

		/**
		 * @throws std::runtime_error if no map is currently loaded.
		 * @sa     @c awe::map::getTileHP().
		 */
		awe::HP getTileHP(const sf::Vector2u& pos) const;

		/**
		 * @throws std::runtime_error if no map is currently loaded, or if the unit
		 *                            ID was invalid.
		 * @sa     @c awe::map::getUnitType().
		 */
		const awe::unit_type getUnitType(const awe::UnitID id) const;

		/**
		 * @throws std::runtime_error if no map is currently loaded, or if the unit
		 *                            ID was invalid.
		 * @sa     @c awe::map::getArmyOfUnit().
		 */
		awe::ArmyID getArmyOfUnit(const awe::UnitID id) const;

		/**
		 * @throws std::runtime_error if no map is currently loaded.
		 * @sa     @c awe::map::getUnitHP().
		 */
		awe::HP getUnitHP(const awe::UnitID id) const;

		/**
		 * @throws std::runtime_error if no map is currently loaded.
		 * @sa     @c awe::map::getUnitFuel().
		 */
		awe::Fuel getUnitFuel(const awe::UnitID id) const;

		/**
		 * @throws std::runtime_error if no map is currently loaded.
		 * @sa     @c awe::map::getUnitAmmo().
		 */
		awe::Ammo getUnitAmmo(const awe::UnitID id) const;

		/**
		 * @throws std::runtime_error if no map is currently loaded.
		 * @sa     @c awe::map::getLoadedUnits().
		 */
		CScriptArray* getLoadedUnits(const awe::UnitID id) const;

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
		 * Lets the \c game class create arrays to pass to the scripts.
		 */
		std::shared_ptr<engine::scripts> _scripts = nullptr;

		/**
		 * Stores the map.
		 */
		std::unique_ptr<awe::map> _map = nullptr;

		/**
		 * Stores the current map scaling factor.
		 */
		float _mapScalingFactor = 2.0f;
	};
}