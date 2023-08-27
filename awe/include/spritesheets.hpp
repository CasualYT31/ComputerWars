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
 * @file spritesheets.hpp
 * Declares the structure used to hold spritesheet pointers.
 */

#pragma once

#include "texture.hpp"

namespace awe {
	/**
	 * Structure used to hold all the spritesheets required by the engine.
	 */
	struct spritesheets {
		/**
		 * Tests all the pointers to ensure they aren't empty.
		 * @param  errstring The error string within \c _initChecks(). If any
		 *                   pointers are invalid, their names will be added to
		 *                   this string.
		 * @safety Basic guarantee.
		 */
		void test(std::string& errstring) const;

		/**
		 * Goes through every global frame counter in every spritesheet and
		 * increments them based on a static delta timer.
		 */
		void updateGlobalFrameIDs();

		/**
		 * Pointer to the CO spritesheet.
		 */
		std::shared_ptr<sfx::animated_spritesheet> CO;

		/**
		 * A collection of unit spritesheets.
		 */
		struct units {
			/**
			 * Pointer to the unit spritesheet used when a unit is idle.
			 */
			std::shared_ptr<sfx::animated_spritesheet> idle;
		};

		/**
		 * Pointer to the tile picture spritesheets.
		 */
		std::shared_ptr<awe::spritesheets::units> unit;

		/**
		 * A collection of tile spritesheets.
		 */
		struct tiles {
			/**
			 * Pointer to the tile spritesheet used in normal weather.
			 */
			std::shared_ptr<sfx::animated_spritesheet> normal;
		};

		/**
		 * Pointer to the tile spritesheets.
		 */
		std::shared_ptr<awe::spritesheets::tiles> tile;

		/**
		 * Pointer to the unit picture spritesheet.
		 */
		std::shared_ptr<sfx::animated_spritesheet> unitPicture;

		/**
		 * A collection of tile picture spritesheets.
		 */
		struct tile_pictures {
			/**
			 * Pointer to the tile picture spritesheet used in normal weather.
			 */
			std::shared_ptr<sfx::animated_spritesheet> normal;
		};

		/**
		 * Pointer to the tile picture spritesheets.
		 */
		std::shared_ptr<awe::spritesheets::tile_pictures> tilePicture;

		/**
		 * Pointer to the icon spritesheet.
		 */
		std::shared_ptr<sfx::animated_spritesheet> icon;

		/**
		 * Pointer to the GUI spritesheet.
		 */
		std::shared_ptr<sfx::animated_spritesheet> GUI;
	};
}
