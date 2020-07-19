/*Copyright 2020 CasualYouTuber31 <naysar@protonmail.com>

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

/**@file texture.h
 * Defines classes dedicated to drawing animated sprites.
 * \c animated_spritesheet loads a series of images and dissects them into separate sprites,
 * each image representing a frame of that sprite.
 * \c animated_sprite accesses a sprite within an \c animated_spritesheet and draws it.
 */

#pragma once

#include "renderer.h"

// for documentation on the sfx namespace, please see renderer.h
namespace sfx {
	/**
	 * This class stores an animated spritesheet.
	 */
	class animated_spritesheet : public safe::json_script {
	public:
		/**
		 * Initialises the internal logger object.
		 * @param name The name to give this particular instantiation within the log file. Defaults to "spritesheet."
		 * @sa    \c global::logger
		 */
		animated_spritesheet(const std::string& name = "spritesheet") noexcept;

		/**
		 * Accesses a reference to an entire frame.
		 * @param  frameID The 0-based ID of the frame to access.
		 * @return A reference to the \c Texture object storing the frame.
		 */
		const sf::Texture& accessTexture(unsigned int frameID) const;

		/**
		 * Accesses the bounding rectangle of a sprite.
		 * This bounding rectangle (X and Y coordinates, as well as dimensions) remains consistent across all frames.
		 * @param  spriteID THe 0-based ID of the sprite whose bounding rectangle is required.
		 * @return The bounding rectangle of the sprite.
		 */
		sf::IntRect accessSprite(unsigned int spriteID) const;

		/**
		 * Retrieves the frame rate of this animated spritesheet.
		 * This frame rate is consistent across all sprites.
		 * @return The frame rate, in frames per second.
		 */
		double getFramerate() const noexcept;

		/**
		 * Retrieves the number of frames in this spritesheet.
		 * Each sprite has the same number of frames.
		 * @return The number of frames.
		 */
		unsigned long long getFrameCount() const noexcept;
	private:
		/**
		 * The JSON load method for this class.
		 * There are four key-value pairs that should be defined:
		 * <ul><li>\c path - <em>Required</em> - The path to find all the image files, each representing a single frame. All files must have extensions.</li>
		 * <li>\c frames - Defaults to 1 (unsigned) - The number of frames this spritesheet should contain.</li>
		 * <li>\c framerate - Defaults to 60.0 - The frame rate, in frames per second. Negative values will be turned into \c 0.0.</li>
		 * <li>\c sprites - An array of bounding rectangles representing each sprite.</li></ul>
		 * \c path is to have the following format: <tt>folders/image_name.png</tt> (or any other valid image format).
		 * \c _loadImages() then inserts frame IDs into the base path to produce the final list of image files to load.
		 * For example, if there are four frames, the following image files will be generated:
		 * <ol><li><tt>folders/image_name<b>_0</b>.png</tt></li>
		 * <li><tt>folders/image_name<b>_1</b>.png</tt></li>
		 * <li><tt>folders/image_name<b>_2</b>.png</tt></li>
		 * <li><tt>folders/image_name<b>_3</b>.png</tt></li></ol>
		 * \c sprites is to contain an array of four-element arrays:
		 * <ol><li>The first element of each inner array is to define the X coordinate of the bounding rectangle.</li>
		 * <li>The second element of each inner array is to define the Y coorindate of the bounding rectangle.</li>
		 * <li>The third element of each inner array is to define the width of the bounding rectangle.</li>
		 * <li>The fourth element of each inner array is to define the height of the bounding rectangle.</li></ol>
		 * The first array will contain the bounding rectangle of the first sprite, which has an ID of \c 0.
		 * The second array will contain the bounding rectangle of the second sprite, with an ID of \c 1, etc.
		 * This method clears both the \c _frames (indirectly) and \c _sprites vectors if a valid \c path value was given.
		 * @param  j The \c safe::json object representing the contents of the loaded script which this method reads.
		 * @return The result of \c _loadImages(). \c FALSE if an invalid \c path value was given.
		 */
		virtual bool _load(safe::json& j) noexcept;

		/**
		 * The JSON save method for this class.
		 * Please see \c _load() for a detailed summary of the format of JSON script that this method produces.
		 * @param  j The \c nlohmann::json object representing the JSON script which this method writes to.
		 * @return Always returns \c TRUE.
		 */
		virtual bool _save(nlohmann::json& j) noexcept;

		/**
		 * Loads each frame into memory during the call to \c load().
		 * Please see _load() for a summary of how each image file should be named.
		 * This method clears the \c _frames vector.
		 * @param  expectedFrames The number of frames to expect.
		 * @return \c TRUE if all expected frames could be loaded, \c FALSE if at least one couldn't.
		 */
		bool _loadImages(unsigned int expectedFrames) noexcept;

		/**
		 * The internal logger object.
		 * @todo Perhaps make most logger objects mutable?
		 */
		mutable global::logger _logger;

		/**
		 * The base file path of the images to load.
		 */
		std::string _basepath;

		/**
		 * The frame rate of the spritesheet.
		 */
		double _framerate = 60.0;

		/**
		 * Stores each frame in memory.
		 * The size of this vector is used to calculate the number of frames.
		 */
		std::vector<sf::Texture> _frames;

		/**
		 * Stores each sprite's bounding rectangle.
		 */
		std::vector<sf::IntRect> _sprites;
	};

	class animated_sprite : public sfx::animated_drawable {
	public:
		animated_sprite(std::shared_ptr<const sfx::animated_spritesheet> sheet, unsigned int sprite, const std::string& name = "sprite") noexcept;
		void setSpritesheet(std::shared_ptr<const sfx::animated_spritesheet> sheet) noexcept;
		void setSprite(unsigned int sprite) noexcept;
		unsigned int getSprite() const noexcept;
		virtual bool animate(const sf::RenderTarget& target) noexcept;
		unsigned int getCurrentFrame() const noexcept;
		unsigned int setCurrentFrame(unsigned int newFrame) noexcept;
		unsigned int operator++() noexcept; // prefix
		unsigned int operator++(int) noexcept; // postfix
		unsigned int operator--() noexcept;
		unsigned int operator--(int) noexcept;
	private:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		std::shared_ptr<const sfx::animated_spritesheet> _sheet;
		unsigned int _spriteID;
		sf::Sprite _sprite;

		mutable global::logger _logger;
		unsigned int _currentFrame = 0;
		bool _hasNotBeenDrawn = true;
		bool _errored = false;
		sf::Clock _clock;
	};
}