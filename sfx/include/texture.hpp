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

/**@file texture.hpp
 * Defines classes dedicated to drawing animated sprites.
 * \c animated_spritesheet loads a series of images and dissects them into separate
 * sprites, each image representing a frame of that sprite. \c animated_sprite
 * accesses a sprite within an \c animated_spritesheet and draws it.
 */

#pragma once

#include "renderer.hpp"
#include "resourcepool.hpp"

namespace sfx {
	/**
	 * This class stores a spritesheet and all of its sprites' properties.
	 */
	class animated_spritesheet : public engine::json_script {
	public:
		/**
		 * Initialises the internal logger object.
		 * @param data The data to initialise the logger object with.
		 * @sa    \c engine::logger
		 */
		animated_spritesheet(const engine::logger::data& data);

		/**
		 * Determines whether or not a sprite with a given ID exists in this sheet.
		 * @param  sprite The ID of the sprite to search for.
		 * @return \c TRUE if a sprite exists with this ID, \c FALSE otherwise.
		 */
		inline bool doesSpriteExist(const std::string& sprite) const {
			return _data.find(sprite) != _data.end();
		}

		/**
		 * Retrieves the entire spritesheet graphic, or a sprite's separate texture
		 * graphic if it's repeated.
		 * @param  sprite The sprite whose texture is to be queried.
		 * @param  frame  The 0-based ID of the frame to retrieve the texture of.
		 * @return A reference to the texture object storing the spritesheet, if
		 *         the given sprite is not repeated. If it is, a reference to the
		 *         texture with just the specified frame is returned.
		 */
		const sf::Texture& getTexture(const std::string& sprite,
			const std::size_t frame) const noexcept;

		/**
		 * Retrieves the frame count of a given sprite.
		 * @warning Logging has been disabled in this method.
		 * @param   sprite The sprite to retrieve the frame count of.
		 * @return  The number of frames the sprite has. \c 0 if the given sprite
		 *          does not exist (an error will also be logged in this case).
		 */
		std::size_t getFrameCount(const std::string& sprite) const noexcept;

		/**
		 * Retrieves one of a sprite's frame's bounding rectangles.
		 * @warning Logging has been disabled in this method.
		 * @param   sprite The sprite whose frame is to be queried.
		 * @param   frame  The 0-based ID of the frame to retrieve the bounding box
		 *                 of.
		 * @return  The width, height, and upper-left location of the given
		 *          sprite's frame in the spritesheet texture. A blank rect will be
		 *          returned if either the sprite or the frame of the sprite did
		 *          not exist (an error will also be logged in this case). In the
		 *          case where the given sprite is repeated, <tt>{ 0, 0,
		 *          frameWidth, frameHeight }</tt> will be returned, since the
		 *          frame is stored in its own texture and has no need to remember
		 *          where it was in the original spritesheet texture.
		 */
		sf::IntRect getFrameRect(const std::string& sprite,
			const std::size_t frame) const;

		/**
		 * Retrieves the intended duration that a given frame of a given sprite is
		 * to remain visible for.
		 * @warning Logging has been disabled in this method.
		 * @param   sprite The sprite whose frame is to be queried.
		 * @param   frame  The 0-based ID of the frame to retrieve the duration of.
		 * @return  The duration of the frame. A duration of \c 0 will be returned
		 *          if either the sprite or the frame of the sprite did not exist
		 *          (an error will also be logged in this case).
		 */
		sf::Time getFrameDuration(const std::string& sprite,
			const std::size_t frame) const;

		/**
		 * Retrieves the configured durations of all frames of a sprite.
		 * @warning Logging has been disabled in this method.
		 * @param   sprite The sprite whose durations are to be returned.
		 * @return  The durations of each frame of this sprite. Empty vector if
		 *          there was an error.
		 */
		std::vector<sf::Time> getFrameDurations(const std::string& sprite) const;

		/**
		 * Finds out if the given sprite is configured to repeat.
		 * @warning Logging has been disabled in this method.
		 * @param   sprite The sprite whose repeated flag is to be returned.
		 * @return  \c TRUE if this sprite repeats, \c FALSE if it does not.
		 */
		bool isSpriteRepeated(const std::string& sprite) const;

		/**
		 * Retrieves the offset intended to be applied to a sprite as it is being
		 * drawn.
		 * @warning Logging has been disabled in this method.
		 * @param   sprite The sprite whose offset is to be queried.
		 * @return  The offset to apply to the sprite. <tt>(0, 0)</tt> will be
		 *          returned if the given sprite did not exist (an error will also
		 *          be logged in this case).
		 */
		sf::Vector2f getSpriteOffset(const std::string& sprite) const;

		/**
		 * Should every copy of this sprite animate in sync?
		 * @warning Logging has been disabled in this method.
		 * @param   sprite The sprite to query.
		 * @return  \c TRUE if every copy of this sprite should base its frame ID
		 *          on the same frame counter, \c FALSE if each copy should run on
		 *          its own frame counter.
		 */
		bool doesSpriteHaveGlobalFrameID(const std::string& sprite) const;

		/**
		 * Gets the sprite's global frame ID.
		 * @warning Logging has been disabled in this method.
		 * @param   sprite The sprite to query.
		 * @return  The current frame ID that every copy of this sprite should
		 *          currently be showing. \c 0 if this sprite does not run on a
		 *          global frame counter.
		 */
		std::size_t getSpriteGlobalFrameID(const std::string& sprite) const;

		/**
		 * Goes through every global frame counter in this spritesheet and
		 * increments them based on a static delta timer.
		 */
		void updateGlobalFrameIDs();

		/**
		 * Returns the height of the tallest frame in this spritesheet.
		 * @return The height, in pixels, of the tallest frame in the entire
		 *         spritesheet.
		 */
		std::size_t heightOfTallestFrame() const;

		/**
		 * Sets this spritesheet's path override.
		 * @param path The path which points to the image to load when calling
		 *             \c load(). The path in the JSON script will be ignored.
		 */
		void setPathOverride(const std::string& path);
	private:
		/**
		 * The JSON load method for this class.
		 * There must be two key-value pairs defined in the base object:
		 * <ul><li>\c path - The path of the image file which stores the
		 *         spritesheet to load.</li>
		 *     <li>\c sprites - An object storing the properties of each sprite.
		 *         </li></ul>
		 * Within \c sprites, there should be a list of key-value pairs. Each key
		 * defines the name of each sprite, and each value is an object containing
		 * the following key-object pairs:
		 * <ul><li>\c frames - An array of bounding rectangles. Each bounding
		 *         rectangle is represented by a four element array, i.e.
		 *         [X,Y,W,H].</li>
		 *     <li>\c durations - An array of integers. Each integer represents the
		 *         duration of time which the corresponding frame should last for
		 *         on screen, in milliseconds. OR: an integer, which stores the
		 *         duration of time which every frame should last for, in ms.</li>
		 *     <li>\c offset - An array that must contain two floats, [X, Y]. This
		 *         will store offsets along the X and Y axes that the sprite will
		 *         always be drawn with. If this array isn't provided, then an
		 *         offset of [0, 0] is assumed.</li>
		 *     <li>\c globalframeid - A bool. By default, it is \c false and does
		 *         not have to be specified. If \c true is given, however, the game
		 *         engine will make sure all copies of this sprite animate in
		 *         sync.</li>
		 *     <li>\c repeated - A bool. By default, it is \c false and does not
		 *         have to be specified. If \c true is given, however, each frame
		 *         of the sprite is <a target="_blank" href="https://www.sfml-dev.org/documentation/2.5.1/classsf_1_1Texture.php#aaa87d1eff053b9d4d34a24c784a28658">
		 *         repeated</a>.</li></ul>
		 * @param  j The \c engine::json object representing the contents of the
		 *           loaded script which this method reads.
		 * @return If the spritesheet graphic could not be loaded, \c FALSE is
		 *         returned and the state of the object is not changed. Otherwise
		 *         \c TRUE is returned and the old state is cleared, even if there
		 *         was an error loading sprite info.
		 * @safety No guarantee.
		 */
		bool _load(engine::json& j);

		/**
		 * The internal logger object.
		 */
		mutable engine::logger _logger;

		/**
		 * All sprites that animate in sync will increment their frame IDs based on
		 * this clock.
		 */
		static sfx::delta_timer _globalTimer;

		/**
		 * The texture storing the spritesheet.
		 */
		sf::Texture _texture;

		/**
		 * Information pertaining to each sprite.
		 */
		struct sprite_data {
			/// The bounding rectangle of each frame of the sprite.
			std::vector<sf::IntRect> frames;

			/// The durations of each frame of the sprite.
			std::vector<sf::Time> durations;

			/// If the sprite is configured to repeat, each frame will need to be
			/// stored in their own texture. If this sprite doesn't repeat, this
			/// vector will be empty.
			std::vector<sf::Texture> textures;

			/// The offset that is applied to this sprite.
			sf::Vector2f offset;

			/// Does this sprite run on a global frame counter?
			bool globalFrameCounter = false;

			/// The global frame counter for this sprite.
			std::size_t globalFrameID = 0;

			/// Each frame counter must have its own accumulated delta since
			/// resetting the static timer's accumulated delta would interfere with
			/// other sprites. To achieve this, an offset is applied to the timer's
			/// accumulated delta to effectively make it start from \c 0 when
			/// required.
			float accumulatedDeltaOffset = 0.0f;
		};

		/// Stores information on every sprite in this sheet.
		std::unordered_map<std::string, sprite_data> _data;

		/**
		 * Caches the height of the tallest sprite/frame in this spritesheet.
		 */
		std::size_t _tallestSpriteHeight = 0;

		/**
		 * If this spritesheet has a path override, it will be used instead of any
		 * paths that are read when <tt>load()</tt>ing.
		 */
		std::optional<std::string> _pathOverride;
	};

	/**
	 * A collection of \c animated_spritesheet objects.
	 */
	class animated_spritesheets :
		public engine::resource_pool<sfx::animated_spritesheet> {
	public:
		/**
		 * Initialises the internal logger object.
		 * @param data The data to initialise the logger object with.
		 * @sa    \c engine::logger
		 */
		animated_spritesheets(const engine::logger::data& data);

		/**
		 * Goes through every global frame counter in every spritesheet and
		 * increments them based on a static delta timer.
		 */
		void updateGlobalFrameIDs();
	private:
		/**
		 * The JSON load method for this class.
		 * Within the root object, there are simply a list of key-string pairs,
		 * with the keys defining the names of the spritesheets, and the string
		 * values containing the paths to the JSON scripts to pass to the \c load()
		 * method of each \c animated_spritesheet.\n
		 * The strings can also be objects that contain the \c json and \c path
		 * keys. If this is the case, \c path is set as the spritesheet's path
		 * override, and \c json is the path of the JSON file to \c load() with.
		 * @param  j The \c engine::json object representing the contents of the
		 *           loaded script which this method reads.
		 * @return \c TRUE if every spritesheet could be loaded successfully,
		 *         \c FALSE if even one of the \c load() methods returned \c FALSE.
		 * @safety No guarantee.
		 */
		bool _load(engine::json& j);

		/**
		 * The JSON save method for this class.
		 * Simply rewrites the spritesheet list.
		 * @param  j The \c nlohmann::ordered_json object representing the JSON
		 *           script which this method writes to.
		 * @return Always returns \c TRUE.
		 */
		bool _save(nlohmann::ordered_json& j);
	};

	/**
     * This class represents an animated sprite.
     * It is intended to be used with \c animated_spritesheet to manage the
     * animation of an already loaded sprite. Multiple \c animated_sprite objects
     * can draw the same sprite, with animations starting at different times if so
     * desired.
     * @sa sfx::animated_drawable
     */
	class animated_sprite : public sfx::animated_drawable {
	public:
		/**
		 * Initialises the internal logger object.
		 * \c _sheet will be initialised with \c nullptr, so it should be set later
		 * on with \c setSpritesheet() if this constructor is used.
		 * @param data The data to initialise the logger object with. Defaults to
		 *             the data that represents "no logger."
		 * @sa    \c engine::logger
		 */
		animated_sprite(const engine::logger::data& data = { nullptr, "" });

		/**
		 * Constructs an animated sprite and initialises the internal logger
		 * object.
		 * @param sheet  A pointer to an \c animated_spritesheet object containing
		 *               the sprite to animate.
		 * @param sprite The name of the sprite from the given sheet which is to be
		 *               animated/drawn.
		 * @param data   The data to initialise the logger object with. Defaults to
		 *               the data that represents "no logger."
		 * @sa    \c engine::logger
		 */
		animated_sprite(std::shared_ptr<const sfx::animated_spritesheet> sheet,
			const std::string& sprite, const engine::logger::data& data =
			{ nullptr, "" });

		/**
		 * Sets a new \c animated_spritesheet to this animated sprite.
		 * Passing \c nullptr will reset the internal sprite's texture rect to
		 * <tt>IntRect(0,0,0,0)</tt> - this can be used to effectively remove an
		 * animated sprite whilst keeping the object alive. This method also
		 * updates \c _hasNotBeenDrawn and \c _currentFrame so that the animated
		 * sprite will start from the beginning of the animation.
		 * @param  sheet A pointer to the sheet to assign to this sprite.
		 * @safety No guarantee.
		 */
		void setSpritesheet(
			std::shared_ptr<const sfx::animated_spritesheet> sheet);

		/**
		 * Retrieves the spritesheet used with this sprite.
		 * @return Pointer to the spritesheet used with this sprite. \c nullptr if
		 *         no spritesheet has been assigned.
		 */
		std::shared_ptr<const sfx::animated_spritesheet> getSpritesheet() const;

		/**
		 * Updates the sprite to animate and draw with this object.
		 * This method also updates \c _hasNotBeenDrawn and \c _currentFrame so
		 * that the animated sprite will start from the beginning of the animation.
		 * \n The call will be ignored if the given sprite was already assigned.
		 * @param  sprite The name of the sprite to animate and draw.
		 * @safety Strong guarantee.
		 */
		void setSprite(const std::string& sprite);

		/**
		 * Retrieves the name of the sprite assigned to this object.
		 * @return The name of the sprite to animate and draw with this object.
		 */
		std::string getSprite() const;

		/**
		 * Retrieves the current frame ID.
		 * @return The ID of the current frame.
		 */
		std::size_t getCurrentFrame() const noexcept;

		/**
		 * Sets the current frame.
		 * This method shouldn't be called unless you have a static sprite and you
		 * wish to choose the frame to display. If you wish to animate the sprite
		 * instead, please use \c animate(). If a frame ID outside of the range of
		 * frames is provided, \c _currentFrame will be set to \c 0. In the event
		 * \c _sheet is \c NULL, no calculations will be carried out.
		 * @param newFrame The ID of the new current frame.
		 * @sa    \c animate()
		 */
		void setCurrentFrame(std::size_t newFrame) noexcept;

		/**
		 * The prefix frame increment operator.
		 * Increments \c _currentFrame by \c 1 using \c setCurrentFrame(). If
		 * incrementing from the last frame, \c _currentFrame will wrap round to
		 * the first frame. If \c _sheet is \c NULL, \c _currentFrame is returned
		 * and no other calculations are carried out.
		 * @return The new value of \c _currentFrame.
		 * @sa     \c setCurrentFrame()
		 */
		std::size_t operator++() noexcept;

		/**
		 * The postfix frame increment operator.
		 * Increments \c _currentFrame by \c 1 using \c setCurrentFrame().
		 * @return The old value of \c _currentFrame.
		 * @sa     \c setCurrentFrame()
		 */
		std::size_t operator++(int) noexcept;

		/**
		 * The prefix frame decrement operator.
		 * Decrements \c _currentFrame by \c 1 using \c setCurrentFrame(). If
		 * decrementing from the first frame, \c _currentFrame will wrap round to
		 * the last frame. If \c _sheet is \c NULL, \c _currentFrame is returned
		 * and no other calculations are carried out.
		 * @return The new value of \c _currentFrame.
		 * @sa     \c setCurrentFrame()
		 */
		std::size_t operator--() noexcept;

		/**
		 * The postfix frame decrement operator.
		 * Decrements \c _currentFrame by \c 1 using \c operator--().
		 * @return The old value of \c _currentFrame.
		 * @sa     \c setCurrentFrame()
		 */
		std::size_t operator--(int) noexcept;

		/**
		 * If this animated sprite is using a sprite that repeats, its size can be
		 * defined using this method.
		 * A repeated animated sprite's default size will match the current frame's
		 * size, meaning no frame will ever seem to repeat by default.\n
		 * If this animated sprite is not repeated, this texture rect will not be
		 * used.
		 * @param size The size, in pixels, of the repeated sprite.
		 */
		inline void setRepeatedSize(const sf::Vector2i& size) {
			_repeatedSize = size;
		}

		/**
		 * Will always ensure that, if this animated sprite is rendering a repeated
		 * sprite, its size matches each frame's size exactly.
		 * This is the default behaviour of repeated sprites.
		 */
		inline void clearRepeatedSize() {
			_repeatedSize = std::nullopt;
		}

		/**
		 * Gets the current size of the sprite with scaling applied.
		 * @return The size of the internal sprite, specifically, the texture rect
		 *         width and height, at the time of calling, with scaling applied.
		 */
		inline sf::Vector2f getSize() const {
			const auto size = getUnscaledSize();
			return sf::Vector2f(size.x * _sprite.getScale().x,
				size.y * _sprite.getScale().y);
		}

		/**
		 * Gets the current size of the sprite without scaling applied.
		 * @return The size of the internal sprite, specifically, the texture rect
		 *         width and height, at the time of calling.
		 */
		inline sf::Vector2f getUnscaledSize() const {
			return sf::Vector2f(static_cast<float>(_sprite.getTextureRect().width),
				static_cast<float>(_sprite.getTextureRect().height));
		}

		/**
		 * Sets the position of the internal sprite.
		 * @param  newPosition The new position, in pixels, of the sprite.
		 * @safety No guarantee.
		 */
		inline void setPosition(const sf::Vector2f& newPosition) {
			_sprite.setPosition(newPosition);
		}

		/**
		 * Gets the position of the internal sprite.
		 * @return The position of the sprite, in pixels.
		 */
		inline sf::Vector2f getPosition() const {
			return getPositionWithoutOffset() + getOffset();
		}

		/**
		 * Gets the position of the internal sprite without the offset stored in
		 * the spritesheet.
		 * @return The position of the sprite, in pixels.
		 */
		inline sf::Vector2f getPositionWithoutOffset() const {
			return _sprite.getPosition();
		}

		/**
		 * Gets the offset of the sprite that's currently set to this object.
		 * @return The offset of the sprite as configured in the spritesheet. If
		 *         there is no sheet, an empty vector will be returned.
		 */
		inline sf::Vector2f getOffset() const {
			return _sheet ? _sheet->getSpriteOffset(_spriteID) : sf::Vector2f();
		}

		/**
		 * Sets the origin of the internal sprite.
		 * @param newOrigin The new origin of the sprite.
		 */
		inline void setOrigin(const sf::Vector2f& newOrigin) {
			_sprite.setOrigin(newOrigin);
		}

		/**
		 * Gets the origin of the internal sprite.
		 * @return The origin of the sprite.
		 */
		inline sf::Vector2f getOrigin() const {
			return _sprite.getOrigin();
		}

		/**
		 * Sets the rotation of the internal sprite.
		 * @param  newRotation The new rotation, in degrees, of the sprite.
		 * @safety No guarantee.
		 */
		inline void setRotation(const float newRotation) {
			_sprite.setRotation(newRotation);
		}

		/**
		 * Gets the rotation of the internal sprite.
		 * @return The rotation of the sprite, in degrees.
		 */
		inline float getRotation() const {
			return _sprite.getRotation();
		}

		/**
		 * Sets the scaling of the internal sprite.
		 * @param  factors The new scaling of the sprite.
		 */
		inline void setScale(const sf::Vector2f& factors) {
			_sprite.setScale(factors);
		}

		/**
		 * Gets the scaling of the internal sprite.
		 * @return The scaling factors of the sprite.
		 */
		inline sf::Vector2f getScale() const {
			return _sprite.getScale();
		}

		/**
		 * This drawable's \c animate() method.
		 * This method will look up the assigned sprite's details (such as frame
		 * duration) and will advance the current frame based on these details.
		 * This method may advance multiple frames if the time between \c animate()
		 * calls is long enough.\n
		 * This method also assigns the necessary texture and texture rectangle to
		 * the internal \c sf::Sprite object, so it must be called even if a
		 * non-animated spritesheet is being used so that the object will render
		 * properly. If no sheet was given, this method will return \c TRUE and
		 * will not perform any other operations. If the sprite name
		 * (client-assigned) or current frame ID (internal) do not refer to a valid
		 * sprite or frame, an error will be logged only once. If the spritesheet
		 * or sprite ID is changed after this time, another error will be logged if
		 * any IDs are still invalid.
		 * @return \c TRUE if the last frame has ended (<b>this only happens with
		 *         sprites that do not have a global frame ID</b>), or if \c _sheet
		 *         is \c NULL, or if there was an error in retrieving the sprite
		 *         information, \c FALSE otherwise.
		 * @safety No guarantee.
		 */
		bool animate(const sf::RenderTarget& target) final;
	private:
		/**
		 * This drawable's \c draw() method.
		 * Simply draws \c _sprite to the screen: to actually assign the correct
		 * sprite graphic to this internal sprite object, please call \c animate()
		 * first!
		 * @param  target The target to render the animated sprite to.
		 * @param  states The render states to apply to the sprite. Applying
		 *                transforms is perfectly valid and will not alter the
		 *                internal workings of the drawable.
		 * @safety No guarantee.
		 */
		void draw(sf::RenderTarget& target, sf::RenderStates states) const final;

		/**
		 * The internal logger object.
		 */
		mutable engine::logger _logger;

		/**
		 * Pointer to an unalterable \c animated_spritesheet object.
		 */
		std::shared_ptr<const sfx::animated_spritesheet> _sheet = nullptr;

		/**
		 * The name of the sprite from \c _sheet to animate and draw.
		 */
		std::string _spriteID;

		/**
		 * The internal sprite object.
		 */
		sf::Sprite _sprite;

		/**
		 * The current frame.
		 */
		std::size_t _currentFrame = 0;

		/**
		 * If present, will override the sprite's texture rect's width and height,
		 * only if it is repeating.
		 */
		std::optional<sf::Vector2i> _repeatedSize;

		/**
		 * Flag tracking \c animate() errors so they don't spam the log file.
		 */
		bool _errored = false;
	};
}
