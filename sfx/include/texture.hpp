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

/**@file texture.hpp
 * Defines classes dedicated to drawing animated sprites.
 * \c animated_spritesheet loads a series of images and dissects them into separate
 * sprites, each image representing a frame of that sprite. \c animated_sprite
 * accesses a sprite within an \c animated_spritesheet and draws it.
 */

#pragma once

#include "renderer.hpp"

namespace sfx {
	/**
	 * This class stores a spritesheet and all of its sprites' properties.
	 */
	class animated_spritesheet : public engine::json_script {
	public:
		/**
		 * Initialises the internal logger object.
		 * @param name The name to give this particular instantiation within the
		 *             log file. Defaults to "spritesheet."
		 * @sa    \c engine::logger
		 */
		animated_spritesheet(const std::string& name = "spritesheet") noexcept;

		/**
		 * Retrieves the entire spritesheet graphic.
		 * @return A reference to the texture object storing the spritesheet.
		 */
		const sf::Texture& getTexture() const noexcept;

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
		 *          not exist (an error will also be logged in this case).
		 */
		sf::IntRect getFrameRect(const std::string& sprite,
			const std::size_t frame) const noexcept;

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
			const std::size_t frame) const noexcept;

		/**
		 * Retrieves the offset intended to be applied to a sprite as it is being
		 * drawn.
		 * @warning Logging has been disabled in this method.
		 * @param   sprite The sprite whose offset is to be queried.
		 * @return  The offset to apply to the sprite. <tt>(0, 0)</tt> will be
		 *          returned if the given sprite did not exist (an error will also
		 *          be logged in this case).
		 */
		sf::Vector2f getSpriteOffset(const std::string& sprite) const noexcept;
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
		 *         on screen, in milliseconds.</li>
		 *     <li>\c offset - An array that must contain two floats, [X, Y]. This
		 *         will store offsets along the X and Y axes that the sprite will
		 *         always be drawn with. If this array isn't provided, then an
		 *         offset of [0, 0] is assumed.</li></ul>
		 * @param  j The \c engine::json object representing the contents of the
		 *           loaded script which this method reads.
		 * @return If the spritesheet graphic could not be loaded, \c FALSE is
		 *         returned and the state of the object is not changed. Otherwise
		 *         \c TRUE is returned and the old state is cleared, even if there
		 *         was an error loading sprite info.
		 */
		bool _load(engine::json& j) noexcept;

		/**
		 * The JSON save method for this class.
		 * This class does not have the ability to be saved.
		 * @param  j The \c nlohmann::ordered_json object representing the JSON
		 *           script which this method writes to.
		 * @return Always returns \c FALSE.
		 */
		bool _save(nlohmann::ordered_json& j) noexcept;

		/**
		 * The internal logger object.
		 */
		mutable engine::logger _logger;

		/**
		 * The texture storing the spritesheet.
		 */
		sf::Texture _texture;

		/**
		 * The bounding rectangles of each frame of all of the sprites.
		 */
		std::unordered_map<std::string, std::vector<sf::IntRect>> _frames;

		/**
		 * The durations of each frame of all of the sprites.
		 */
		std::unordered_map<std::string, std::vector<sf::Time>> _durations;

		/**
		 * The offsets that are to be applied to each sprite.
		 */
		std::unordered_map<std::string, sf::Vector2f> _offsets;
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
		 * @param  name The name to give this particular instantiation within the
		 *              log file. Defaults to "sprite."
		 * @sa     \c engine::logger
		 */
		animated_sprite(const std::string& name = "sprite") noexcept;

		/**
		 * Constructs an animated sprite and initialises the internal logger
		 * object.
		 * @param  sheet  A pointer to an \c animated_spritesheet object containing
		 *                the sprite to animate.
		 * @param  sprite The name of the sprite from the given sheet which is to
		 *                be animated/drawn.
		 * @param  name   The name to give this particular instantiation within the
		 *                log file. Defaults to "sprite."
		 * @sa     \c engine::logger
		 */
		animated_sprite(std::shared_ptr<const sfx::animated_spritesheet> sheet,
			const std::string& sprite, const std::string& name = "sprite")
			noexcept;

		/**
		 * Sets a new \c animated_spritesheet to this animated sprite.
		 * Passing \c nullptr will reset the internal sprite's texture rect to
		 * <tt>IntRect(0,0,0,0)</tt> - this can be used to effectively remove an
		 * animated sprite whilst keeping the object alive. This method also
		 * updates \c _hasNotBeenDrawn and \c _currentFrame so that the animated
		 * sprite will start from the beginning of the animation.
		 * @param sheet A pointer to the sheet to assign to this sprite.
		 */
		void setSpritesheet(std::shared_ptr<const sfx::animated_spritesheet> sheet)
			noexcept;

		/**
		 * Retrieves the spritesheet used with this sprite.
		 * @return Pointer to the spritesheet used with this sprite. \c nullptr if
		 *         no spritesheet has been assigned.
		 */
		std::shared_ptr<const sfx::animated_spritesheet> getSpritesheet() const
			noexcept;

		/**
		 * Updates the sprite to animate and draw with this object.
		 * This method also updates \c _hasNotBeenDrawn and \c _currentFrame so
		 * that the animated sprite will start from the beginning of the animation.
		 * \n The call will be ignored if the given sprite was already assigned.
		 * @param sprite The name of the sprite to animate and draw.
		 */
		void setSprite(const std::string& sprite) noexcept;

		/**
		 * Retrieves the name of the sprite assigned to this object.
		 * @return The name of the sprite to animate and draw with this object.
		 */
		std::string getSprite() const noexcept;

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
		 * Retrieves the current frame ID.
		 * @return The ID of the current frame.
		 */
		std::size_t getCurrentFrame() const noexcept;

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
		 * Gets the current size of the sprite.
		 * @return The size of the internal sprite, specifically, the texture rect
		 *         width and height, at the time of calling.
		 */
		sf::Vector2f getSize() const noexcept;

		/**
		 * Sets the position of the internal sprite.
		 * @param newPosition The new position, in pixels, of the sprite.
		 */
		void setPosition(const sf::Vector2f& newPosition) noexcept;

		/**
		 * Gets the position of the internal sprite.
		 * @return The position of the sprite, in pixels.
		 */
		sf::Vector2f getPosition() const noexcept;

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
		 * @return \c TRUE if the current frame is the last frame, or if \c _sheet
		 *         is \c NULL, or if there was an error in retrieving the sprite
		 *         information, \c FALSE otherwise.
		 */
		virtual bool animate(const sf::RenderTarget& target,
			const double scaling = 1.0) noexcept;
	private:
		/**
		 * This drawable's \c draw() method.
		 * Simply draws \c _sprite to the screen: to actually assign the correct
		 * sprite graphic to this internal sprite object, please call \c animate()
		 * first!
		 * @param target The target to render the animated sprite to.
		 * @param states The render states to apply to the sprite. Applying
		 *               transforms is perfectly valid and will not alter the
		 *               internal workings of the drawable.
		 */
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

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
		 * Flag tracking \c animate() errors so they don't spam the log file.
		 */
		bool _errored = false;
	};
}
