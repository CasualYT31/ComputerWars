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

#include "texture.h"

sfx::spritesheet::spritesheet(const std::string& name) noexcept : _logger(name) {}

std::string sfx::spritesheet::getBasePath() const noexcept {
	return _basepath;
}

std::string sfx::spritesheet::getFormat() const noexcept {
	return _format;
}

sf::Texture sfx::spritesheet::getFrame(sfx::FrameIndex frame, sfx::SpriteKey sprite) noexcept {
	if (_frame.size() == 0) {
		//_logger.error("There were no frames loaded.");
		return sf::Texture();
	}
	if (frame >= _frame.size()) {
		//_logger.write("The frame index {} is too large, last frame in the vector ({}) used instead", frame, _frame.size() - 1);
		frame = (sfx::FrameIndex) _frame.size() - 1;
	}
	if (_frame[frame].size() == 0) {
		//_logger.error("There were no sprites loaded.");
		return sf::Texture();
	}
	if (sprite >= _frame[frame].size()) {
		//_logger.write("The sprite index {} is too large, last sprite in the vector ({}) used instead", sprite, _frame[frame].size() - 1);
		sprite = (sfx::SpriteKey) _frame[frame].size() - 1;
	}
	return _frame[frame][sprite];
}

sf::IntRect sfx::spritesheet::getSprite(sfx::SpriteKey sprite) noexcept {
	if (_sprite.size() == 0) {
		return sf::IntRect();
	}
	if (sprite >= _sprite.size()) {
		//_logger.write("The sprite index {} is too large, last sprite in the vector ({}) used instead", sprite, _sprite.size() - 1);
		sprite = (sfx::SpriteKey) _sprite.size() - 1;
	}
	return _sprite[sprite];
}

float sfx::spritesheet::getFrameRate() const noexcept {
	return _framerate;
}

unsigned int sfx::spritesheet::getFrames() const noexcept {
	return _frames;
}

sfx::FrameIndex sfx::spritesheet::getCurrentFrame() const noexcept {
	return _currentFrame;
}

sfx::FrameIndex sfx::spritesheet::setCurrentFrame(sfx::FrameIndex frame) noexcept {
	auto old = _currentFrame;
	if (frame >= _frame.size()) frame = 0;
	_currentFrame = frame;
	return old;
}

void sfx::spritesheet::resetCurrentFrame() noexcept {
	setCurrentFrame(0);
	_hasNotBeenDrawn = true;
}

sf::Texture sfx::spritesheet::operator[](sfx::SpriteKey sprite) noexcept {
	if (_hasNotBeenDrawn) {
		_clock.restart();
		_hasNotBeenDrawn = false;
	} else if (getFrameRate() > 0.0 && _clock.getElapsedTime().asSeconds() >= (1 / getFrameRate())) {
		_clock.restart();
		++(*this);
	}
	return this->getFrame(getCurrentFrame(), sprite);
}

sfx::FrameIndex sfx::spritesheet::operator++() noexcept {
	setCurrentFrame(getCurrentFrame() + 1);
	return getCurrentFrame();
}

sfx::FrameIndex sfx::spritesheet::operator++(int) noexcept {
	return setCurrentFrame(getCurrentFrame() + 1);
}

sfx::FrameIndex sfx::spritesheet::operator--() noexcept {
	if ((int)getCurrentFrame() - 1 < 0) {
		setCurrentFrame(getFrames() - 1);
	} else {
		setCurrentFrame(getCurrentFrame() - 1);
	}
	return getCurrentFrame();
}

sfx::FrameIndex sfx::spritesheet::operator--(int) noexcept {
	auto oldCurrentFrame = getCurrentFrame();
	--(*this);
	return oldCurrentFrame;
}

bool sfx::spritesheet::_load(safe::json& j) noexcept {
	j.apply(_basepath, { "basepath" });
	if (!j.inGoodState()) {
		_logger.error("Fatal error: invalid base path.");
		return false;
	}
	j.apply(_format, { "format" }, &_format, true);
	j.apply(_frames, { "frames" }, &_frames, true);
	j.apply(_framerate, { "framerate" }, &_framerate, true);

	nlohmann::json jj = j.nlohmannJSON();
	try {
		jj = jj["sprites"];
		for (std::size_t s = 0; s < jj.size(); s++) {
			//initial checks have passed, so the sprite vector can now be cleared of its default value, ready for new input
			if (!s) _sprite.clear();
			try {
				sf::IntRect t;
				t.left = jj[s][0];
				t.top = jj[s][1];
				t.width = jj[s][2];
				t.height = jj[s][3];
				_sprite.push_back(t);
			}
			catch (std::exception & e) {
				_logger.error("An error occurred when attempting to add sprite coordinates to the internal vector: {}.", e.what());
				_logger.write("The coordinates 0,0,0,0 will be given to sprite {}.", s);
				_sprite.push_back(sf::IntRect());
			}
		}
	}
	catch (std::exception & e) {
		_logger.error("An error occurred when attempting to read sprite coordinates: {}.", e.what());
		_logger.write("The coordinates 0,0,0,0 will be given to the single sprite.");
		_sprite.push_back(sf::IntRect());
	}

	return _loadTextures();
}

bool sfx::spritesheet::_save(nlohmann::json& j) noexcept {
	j["basepath"] = _basepath;
	j["format"] = _format;
	j["frames"] = _frames;
	j["framerate"] = _framerate;
	for (auto itr = _sprite.begin(), enditr = _sprite.end(); itr != enditr; itr++) {
		std::array<int, 4> rect;
		rect[0] = itr->left;
		rect[1] = itr->top;
		rect[2] = itr->width;
		rect[3] = itr->height;
		j["sprites"].push_back(rect);
	}
	return true;
}

bool sfx::spritesheet::_loadTextures() noexcept {
	_frame.clear();
	bool ret = true;
	for (unsigned int f = 0; f < _frames; f++) {
		std::string path = getBasePath() + "_" + std::to_string(f) + "." + getFormat();
		std::vector<sf::Texture> temp;
		for (unsigned int s = 0; s < _sprite.size(); s++) {
			sf::Texture t;
			if (t.loadFromFile(path, _sprite[s])) {
				temp.push_back(t);
			} else {
				_logger.error("Failed to load texture from \"{}\".", path);
				ret = false;
			}
		}
		_frame.push_back(temp);
	}
	return ret;
}