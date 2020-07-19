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
 * 
 */

#pragma once

#include "renderer.h"

namespace sfx {
	// CONSTRUCTION ZONE

	class animated_spritesheet : public safe::json_script {
	public:
		animated_spritesheet(const std::string& name = "spritesheet") noexcept;
		const sf::Texture& accessTexture(unsigned int frameID) const;
		sf::IntRect accessSprite(unsigned int spriteID) const;

		double getFramerate() const noexcept;
		unsigned long long getFrameCount() const noexcept;
	private:
		virtual bool _load(safe::json& j) noexcept;
		virtual bool _save(nlohmann::json& j) noexcept;
		bool _loadImages(unsigned int expectedFrames) noexcept;

		// todo: make loggers in most places mutable
		mutable global::logger _logger;
		std::string _basepath;
		double _framerate = 60.0;
		std::vector<sf::Texture> _frames; // size represents number of frames
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

	// END CONSTRUCTION ZONE


	typedef unsigned int FrameIndex;
	typedef unsigned int SpriteKey;

	class spritesheet : public safe::json_script {
	public:
		spritesheet(const std::string& name = "spritesheet") noexcept;

		std::string getBasePath() const noexcept;
		std::string getFormat() const noexcept;
		sf::Texture getFrame(FrameIndex frame = 0, SpriteKey sprite = 0) noexcept;
		sf::IntRect getSprite(SpriteKey sprite = 0) noexcept;
		float getFrameRate() const noexcept;
		unsigned int getFrames() const noexcept;
		FrameIndex getCurrentFrame() const noexcept;
		FrameIndex setCurrentFrame(FrameIndex frame) noexcept;
		void resetCurrentFrame() noexcept;

		sf::Texture operator[](SpriteKey sprite) noexcept;

		FrameIndex operator++() noexcept;
		FrameIndex operator++(int) noexcept;
		FrameIndex operator--() noexcept;
		FrameIndex operator--(int) noexcept;
	private:
		virtual bool _load(safe::json& j) noexcept;
		virtual bool _save(nlohmann::json& j) noexcept;
		bool _loadTextures() noexcept;

		std::string _basepath;
		std::string _format = "png";
		std::vector<std::vector<sf::Texture>> _frame; //_frame[frame][sprite]
		std::vector<sf::IntRect> _sprite = { sf::IntRect() };
		float _framerate = 60.0;
		unsigned int _frames = 1;

		FrameIndex _currentFrame = 0;
		sf::Clock _clock;
		bool _hasNotBeenDrawn = true;
		global::logger _logger;
	};
}