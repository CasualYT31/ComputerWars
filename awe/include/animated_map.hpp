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

/**@file animated_map.hpp
 * Declares the class that is used to display a map.
 */

#pragma once

#include "map.hpp"

namespace awe {
	/**
	 * Graphically displays a \c map object by responding to notifications that are
	 * sent to it by that object.
	 */
	class animated_map : public engine::observer, public sfx::animated_drawable {
	public:
		/**
		 * Represents quadrants of a rectangle such as a render target.
		 */
		enum class quadrant {
			UpperLeft,
			UpperRight,
			LowerLeft,
			LowerRight
		};

		/**
		 * The list of shaders that can be applied to the tiles that are available.
		 */
		enum class available_tile_shader {
			None,
			Yellow,
			Red
		};

		/**
		 * Initialises the shaders and other drawing objects.
		 * @warning You must add the \c animated_map as an observer of \c map
		 *          manually!
		 * @param   map  Pointer to the map object this animated map will render.
		 * @param   data The data to initialise the logger object with.
		 * @sa      \c engine::logger
		 */
		animated_map(const std::shared_ptr<awe::map>& map,
			const engine::logger::data& data);

		/**
		 * When there's a change in the map, this method will be called.
		 */
		void update(const int type, const std::any& data) override final;

		///////////////////
		// TARGET & VIEW //
		///////////////////
		/**
		 * Sets the target used with this map.
		 * @param target Pointer to the target this map will be drawn on.
		 */
		void setTarget(const std::shared_ptr<sf::RenderTarget>& target) noexcept;

		/**
		 * Retrieves the tile that is at the given pixel.
		 * If \c setTarget() hasn't been called this method will not produce valid
		 * results.
		 * @param  pixel The pixel within the current render target to query.
		 * @return The tile's position.
		 */
		sf::Vector2u getTileAtPixel(const sf::Vector2i& pixel);

		/**
		 * Calculates where the map's bounding rectangle is relative to the screen.
		 * If you are using this to check if the mouse position is within the map's
		 * graphic, you may have to be aware that \c INVALID_MOUSE stores
		 * <tt>(-1, -1)</tt>, so if the map is larger than the screen that may be
		 * considered within the map graphic! If you are pairing that check with a
		 * \c isMouseButtonTriggeringControl() check, though, you won't have to
		 * worry as all mouse buttons are ignored if the mouse position is
		 * \c INVALID_MOUSE.
		 * @warning This method assumes each tile is \c awe::tile::MIN_WIDTH in
		 *          width! A more complicated implementation would take variable
		 *          widths into consideration, but it's not something I'd ever use
		 *          so I'm not wasting time on it.
		 * @return  The bounding rectangle of the map, in pixel coordinates.
		 */
		sf::IntRect getMapBoundingBox() const;

		/////////////
		// OPTIONS //
		/////////////
		/**
		 * Sets the amount by which the map is scaled.
		 * If it is detected that a value at or below \c 0.0f is given, an error
		 * will be logged and \c _scaling will not be changed.
		 * @param factor The factor by which to scale the map.
		 */
		void setMapScalingFactor(const float factor);

		/**
		 * Used to always draw units that are hidden from the perspective of the
		 * current army.
		 * @param alwaysShow \c TRUE if all hidden units should be shown, \c FALSE
		 *                   to not override a unit's hidden flag.
		 */
		void alwaysShowHiddenUnits(const bool alwaysShow) noexcept;

		/**
		 * Sets the shader to use for available tiles.
		 * @param shader The shader to use.
		 */
		void setAvailableTileShader(const available_tile_shader shader);

		/**
		 * Gets the shader currently used for available tiles.
		 * @return The shader in use.
		 */
		available_tile_shader getAvailableTileShader() const;

		/////////////
		// CURSORS //
		/////////////
		/**
		 * Sets which sprites from the icon spritesheet to use as the main cursor
		 * when its in different quadrants of the screen.
		 * If \c _sheet_icon isn't \c nullptr, and a given sprite doesn't exist in
		 * that sheet, then a warning will be logged, but the assignment will go
		 * ahead. This warning will not be logged for empty sprite keys.
		 * @warning Must be called, or else the cursor will not show up!
		 * @param   ul The animated sprite to show when the cursor is within the
		 *             UL quadrant of the render target.
		 * @param   ur The animated sprite to show when the cursor is within the
		 *             UR quadrant of the render target.
		 * @param   ll The animated sprite to show when the cursor is within the
		 *             LL quadrant of the render target.
		 * @param   lr The animated sprite to show when the cursor is within the
		 *             LR quadrant of the render target.
		 */
		void setCursorSprites(const std::string& ul, const std::string& ur,
			const std::string& ll, const std::string& lr);

		/**
		 * Sets which sprites from the icon spritesheet to use as the corners of
		 * the additionally selected tile cursor.
		 * If \c _sheet_icon isn't \c nullptr, and a given sprite doesn't exist in
		 * that sheet, then a warning will be logged, but the assignment will go
		 * ahead. This warning will not be logged for empty sprite keys.
		 * @warning Must be called, or else the cursor will not show up!
		 * @param   ul The animated sprite to use as the UL corner of the cursor.
		 * @param   ur The animated sprite to use as the UR corner of the cursor.
		 * @param   ll The animated sprite to use as the LL corner of the cursor.
		 * @param   lr The animated sprite to use as the LR corner of the cursor.
		 */
		void setAdditionalCursorSprites(const std::string& ul,
			const std::string& ur, const std::string& ll, const std::string& lr);

		/**
		 * Determines if the cursor is on the left or right side of the target.
		 * @return \c TRUE if the UL of the cursor graphic is on the left half of
		 *         the target, \c FALSE if it is on the right half of the target,
		 *         or if \c _target is \c nullptr.
		 */
		bool isCursorOnLeftSide() const;

		/**
		 * Determines if the cursor is on the top or bottom side of the target.
		 * @return \c TRUE if the UL of the cursor graphic is on the top half of
		 *         the target, \c FALSE if it is on the bottom half of the target,
		 *         or if \c _target is \c nullptr.
		 */
		bool isCursorOnTopSide() const;

		/**
		 * Determines which quadrant of the render target the cursor is in.
		 * @return Where the UL of the cursor sprite is determined to be.
		 */
		quadrant getCursorQuadrant() const;

		/**
		 * Calculates where the cursor's bounding rectangle is relative to the GUI.
		 * @return The bounding rectangle of the cursor, in GUI coordinates.
		 */
		sf::IntRect getCursorBoundingBox() const;

		//////////////////////
		// RECTANGLE SELECT //
		//////////////////////
		/**
		 * Determines which tile the selection tile graphic should be drawn from.
		 * @param tile The tile from which to draw the outline rectangle.
		 * @sa    \c setRectangleSelectionEnd().
		 */
		void setRectangleSelectionStart(const sf::Vector2u& tile);

		/**
		 * Determines which tile the selection tile graphic should be drawn to.
		 * @param tile The tile to which the outline rectangle should be drawn.
		 * @sa    \c setRectangleSelectionStart().
		 */
		void setRectangleSelectionEnd(const sf::Vector2u& tile);

		/**
		 * Clears the rectangle selection.
		 */
		void removeRectangleSelection();

		/**
		 * Retrieves the previously set start tile of the rectangle selection.
		 * @return The tile from which the rectangle selection is currently being
		 *         drawn, or <tt>(0, 0)</tt> if there isn't any being drawn.
		 */
		sf::Vector2u getRectangleSelectionStart() const;

		/**
		 * Retrieves the previously set end tile of the rectangle selection.
		 * @return The tile to which the rectangle selection is currently being
		 *         drawn, or <tt>(0, 0)</tt> if there isn't any being drawn.
		 */
		sf::Vector2u getRectangleSelectionEnd() const;

		//////////////////
		// SPRITESHEETS //
		//////////////////
		/**
		 * Sets the spritesheet used for drawing tiles.
		 * @param sheet Pointer to the animated spritesheet to use for tiles.
		 */
		void setTileSpritesheet(
			const std::shared_ptr<sfx::animated_spritesheet>& sheet);

		/**
		 * Sets the spritesheet used for drawing units.
		 * @param sheet Pointer to the animated spritesheet to use for units.
		 */
		void setUnitSpritesheet(
			const std::shared_ptr<sfx::animated_spritesheet>& sheet);

		/**
		 * Sets the spritesheet used for drawing map icons.
		 * @param sheet Pointer to the animated spritesheet to use for icons.
		 */
		void setIconSpritesheet(
			const std::shared_ptr<sfx::animated_spritesheet>& sheet);

		/**
		 * Sets the spritesheet used for drawing COs.
		 * @param sheet Pointer to the animated spritesheet to use for COs.
		 */
		void setCOSpritesheet(
			const std::shared_ptr<sfx::animated_spritesheet>& sheet);

		/////////////////////
		// ADDITIONAL DATA //
		/////////////////////
		/**
		 * Sets the GUI engine to pull the GUI scaling factor from.
		 * @param gui Pointer to the GUI engine to use with this map.
		 */
		void setGUI(const std::shared_ptr<sfx::gui>& gui);

		/**
		 * Sets the language dictionary to use with this object.
		 * @param dict Pointer to the dictionary to use with this object.
		 */
		void setLanguageDictionary(
			const std::shared_ptr<engine::language_dictionary>& dict);

		/**
		 * Sets the fonts used with this map.
		 * @param font Pointer to the fonts to use with this map.
		 */
		void setFonts(const std::shared_ptr<sfx::fonts>& fonts);

		/**
		 * This drawable's \c animate() method.
		 * @param  target The target to render the map to.
		 * @return \c FALSE, for now.
		 */
		bool animate(const sf::RenderTarget& target) final;
	private:
		/**
		 * This drawable's \c draw() method.
		 * Here is what will be rendered to the given target, in order:
		 * <ol><li>The map's tiles that are within the visible portion, starting
		 *         from the top row down.</li>
		 *     <li>All the units that are on the visible tiles, if they are to be
		 *         on the map according to \c isUnitOnMap(). This includes their
		 *         flashing icons.</li>
		 *     <li>The cursor, which will be drawn at the selected tile.</li>
		 *     <li>An information pane describing the selected army: their COs,
		 *         their funds, their country, and their power meters.</li>
		 *     <li>An information pane describing the selected tile: its type and
		 *         HP, and the unit's information, if there is a unit on the tile.
		 *     </li></ol>
		 * @param target The target to render the map to.
		 * @param states The render states to apply to the map. Applying transforms
		 *               is perfectly valid and will not alter the internal
		 *               workings of the drawable.
		 */
		void draw(sf::RenderTarget& target, sf::RenderStates states) const final;
		
		/**
		 * Internal logger object.
		 */
		mutable engine::logger _logger;

		/**
		 * The map object.
		 */
		std::shared_ptr<awe::map> _map;

		//////////////////////
		// INTERNAL CLASSES //
		//////////////////////
		/**
		 * The graphical representation of an \c awe::closed_list_node.
		 */
		struct closed_list_node {
			/**
			 * The icon of the closed list node.
			 */
			sfx::animated_sprite sprite;
		};

		/**
		 * The graphical representation of an \c awe::map::selected_unit.
		 */
		struct selected_unit {
			/**
			 * The shader to apply to all available tiles.
			 */
			available_tile_shader availableTileShader =
				available_tile_shader::None;

			/**
			 * Used to temporarily disable rendering effects without deselecting
			 * the unit.
			 */
			bool disableRenderingEffects = false;

			/**
			 * Used to disable greyed out shading on units if they are on an
			 * available tile.
			 */
			bool disableShaderForAvailableUnits = false;
		};

		/**
		 * The graphical representation of an \c awe::tile.
		 */
		class tile : public sfx::animated_drawable {
		public:
			/**
			 * Initialises the tile sprite.
			 */
			tile(const std::shared_ptr<const awe::tile_type>& type,
				const awe::ArmyID owner,
				const std::shared_ptr<sfx::animated_spritesheet>& sheet);

			/**
			 * Updates this tile's graphics based on the given type and owner.
			 */
			void setType(const std::shared_ptr<const awe::tile_type>& type,
				const awe::ArmyID owner);

			/**
			 * This drawable's \c animate() method.
			 * Simply calls the internal sprite's \c animate() method.
			 * @return The return value of <tt>animated_sprite</tt>'s \c animate()
			 *         call.
			 */
			bool animate(const sf::RenderTarget& target) final;
		private:
			/**
			 * This drawable's \c draw() method.
			 * Simply draws \c _sprite to the screen.
			 * @param target The target to render the tile to.
			 * @param states The render states to apply to the tile. Applying
			 *               transforms is perfectly valid and will not alter the
			 *               internal workings of the drawable.
			 */
			void draw(sf::RenderTarget& target,
				sf::RenderStates states) const final;

			/**
			 * The sprite of this tile.
			 */
			sfx::animated_sprite _sprite;
		};

		/**
		 * The graphical representation of an \c awe::unit.
		 */
		class unit : public sfx::animated_drawable {
		public:
			/**
			 * This drawable's \c animate() method.
			 * Simply calls the internal sprite's \c animate() method.
			 * @return The return value of <tt>animated_sprite</tt>'s \c animate()
			 *         call.
			 */
			bool animate(const sf::RenderTarget& target) final;
		private:
			/**
			 * This drawable's \c draw() method.
			 * Simply draws \c _sprite to the screen.
			 * @param target The target to render the tile to.
			 * @param states The render states to apply to the tile. Applying
			 *               transforms is perfectly valid and will not alter the
			 *               internal workings of the drawable.
			 */
			void draw(sf::RenderTarget& target,
				sf::RenderStates states) const final;

			/**
			 * The sprite of this unit.
			 */
			sfx::animated_sprite _sprite;
		};

		///////////////////
		// TARGET & VIEW //
		///////////////////
		/**
		 * The target set via \c setTarget().
		 */
		std::shared_ptr<sf::RenderTarget> _target;

		/**
		 * The view applied to the target whilst drawing.
		 */
		sf::View _view;

		/**
		 * The amount to offset the view by in the X direction.
		 */
		std::optional<float> _viewOffsetX;

		/**
		 * The amount to offset the view by in the Y direction.
		 */
		std::optional<float> _viewOffsetY;

		/////////////
		// OPTIONS //
		/////////////
		/**
		 * The scaling factor to apply to the map when drawing.
		 */
		float _scaling = 1.0f;

		/**
		 * If set to \c TRUE, all units are drawn, regardless of whether they are
		 * hidden to the current army or not.
		 */
		bool _alwaysShowHiddenUnits = false;

		///////////////////
		// TILES & UNITS //
		///////////////////
		/**
		 * The tile sprites.
		 */
		std::vector<std::vector<tile>> _tiles;

		/**
		 * The unit sprites.
		 */
		std::unordered_map<awe::UnitID, unit> _units;

		/////////////
		// CURSORS //
		/////////////
		/**
		 * The animated sprite representing the cursor.
		 */
		sfx::animated_sprite _cursor;

		/**
		 * The ID of the sprite to use for the cursor when it is in the UL corner
		 * of the screen.
		 */
		std::string _ulCursorSprite;

		/**
		 * The ID of the sprite to use for the cursor when it is in the UR corner
		 * of the screen.
		 */
		std::string _urCursorSprite;

		/**
		 * The ID of the sprite to use for the cursor when it is in the LL corner
		 * of the screen.
		 */
		std::string _llCursorSprite;

		/**
		 * The ID of the sprite to use for the cursor when it is in the LR corner
		 * of the screen.
		 */
		std::string _lrCursorSprite;

		/**
		 * The UL corner of the additionally selected tile cursor.
		 */
		sfx::animated_sprite _additionallySelectedTileCursorUL;

		/**
		 * The UR corner of the additionally selected tile cursor.
		 */
		sfx::animated_sprite _additionallySelectedTileCursorUR;

		/**
		 * The LL corner of the additionally selected tile cursor.
		 */
		sfx::animated_sprite _additionallySelectedTileCursorLL;

		/**
		 * The LR corner of the additionally selected tile cursor.
		 */
		sfx::animated_sprite _additionallySelectedTileCursorLR;

		//////////////////////
		// RECTANGLE SELECT //
		//////////////////////
		/**
		 * The start of the rectangle tile selection.
		 */
		std::optional<sf::Vector2u> _startOfRectSel;

		/**
		 * The end of the rectangle tile selection.
		 */
		std::optional<sf::Vector2u> _endOfRectSel;

		/**
		 * The rectangle selection graphic.
		 */
		sf::RectangleShape _rectangle;

		/////////////
		// SHADERS //
		/////////////
		/**
		 * Yellow tiles shader.
		 */
		sf::Shader _availableTileShader;

		/**
		 * Red tiles shader.
		 */
		sf::Shader _attackableTileShader;

		/**
		 * Unavilable tiles shader.
		 */
		sf::Shader _unavailableTileShader;

		//////////////////
		// SPRITESHEETS //
		//////////////////
		/**
		 * Spritesheet used with all tiles.
		 */
		std::shared_ptr<sfx::animated_spritesheet> _sheet_tile = nullptr;

		/**
		 * Spritesheet used with all units.
		 */
		std::shared_ptr<sfx::animated_spritesheet> _sheet_unit = nullptr;

		/**
		 * Spritesheet used with all map icons.
		 */
		std::shared_ptr<sfx::animated_spritesheet> _sheet_icon = nullptr;

		/**
		 * Spritesheet used with all armies.
		 */
		std::shared_ptr<sfx::animated_spritesheet> _sheet_co = nullptr;

		/////////////////////
		// ADDITIONAL DATA //
		/////////////////////
		/**
		 * GUI engine.
		 */
		std::shared_ptr<sfx::gui> _gui;

		/**
		 * The language dictionary.
		 */
		std::shared_ptr<engine::language_dictionary> _dict;

		/**
		 * The fonts collection.
		 */
		std::shared_ptr<sfx::fonts> _fonts;
	};
}
