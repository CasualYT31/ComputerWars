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

#pragma once

#include "safejson.h"
#include "sfml/Graphics.hpp"

namespace sfx {
	class spritesheet : public safe::json_script {
	public:
		spritesheet(const std::string& name = "spritesheet") noexcept;

		std::string getBasePath() const noexcept;
		std::string getFormat() const noexcept;
		sf::Texture getFrame(unsigned int frame = 0, unsigned int sprite = 0) noexcept;
		sf::IntRect getSprite(unsigned int sprite = 0) noexcept;
		float getFrameRate() const noexcept;
		unsigned int getFrames() const noexcept;
		unsigned int getCurrentFrame() const noexcept;
		unsigned int setCurrentFrame(unsigned int frame) noexcept;
		void resetCurrentFrame() noexcept;

		sf::Texture operator[](unsigned int sprite) noexcept;

		unsigned int operator++() noexcept;
		unsigned int operator++(int) noexcept;
		unsigned int operator--() noexcept;
		unsigned int operator--(int) noexcept;
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

		unsigned int _currentFrame = 0;
		sf::Clock _clock;
		bool _hasNotBeenDrawn = true;
		global::logger _logger;
	};
}