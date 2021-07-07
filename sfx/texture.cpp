/*Copyright 2019-2021 CasualYouTuber31 <naysar@protonmail.com>

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

sfx::animated_spritesheet::animated_spritesheet(const std::string& name) noexcept :
	_logger(name) {}

const sf::Texture& sfx::animated_spritesheet::accessTexture(unsigned int frameID)
	const {
	if (frameID >= _frames.size()) {
		std::string msg = "Attempted to access non-existent frame " +
			std::to_string(frameID) + ".";
		// _logger.error(msg);
		throw std::out_of_range(msg);
	} else {
		return _frames.at(frameID);
	}
}

sf::IntRect sfx::animated_spritesheet::accessSprite(unsigned int spriteID) const {
	if (spriteID >= _sprites.size()) {
		std::string msg = "Attempted to access non-existent sprite " +
			std::to_string(spriteID) + ".";
		// _logger.error(msg);
		throw std::out_of_range(msg);
	} else {
		return _sprites.at(spriteID);
	}
}

double sfx::animated_spritesheet::getFramerate() const noexcept {
	return _framerate;
}

unsigned long long sfx::animated_spritesheet::getFrameCount() const noexcept {
	return _frames.size();
}

bool sfx::animated_spritesheet::_load(engine::json& j) noexcept {
	std::string temp_basepath;
	unsigned int frameCount = 1;
	j.apply(temp_basepath, { "path" });
	if (!j.inGoodState()) {
		_logger.error("No base path provided - aborting load attempt.");
		return false;
	}
	if (temp_basepath.find('.') == std::string::npos) {
		_logger.error("Invalid base path provided - '.###' file format extension "
			"is required.");
		return false;
	}
	j.apply(frameCount, { "frames" }, &frameCount, true);
	if (frameCount == 0) {
		_logger.warning("Cannot have 0 frames: assuming 1...");
		frameCount = 1;
	}
	j.apply(_framerate, { "framerate" }, &_framerate, true);
	if (_framerate < 0.0) _framerate = 0.0;

	_basepath = temp_basepath;
	_sprites.clear();
	nlohmann::ordered_json jj = j.nlohmannJSON();
	try {
		jj = jj["sprites"];
		for (std::size_t s = 0; s < jj.size(); s++) {
			try {
				sf::IntRect t;
				t.left = jj[s][0];
				t.top = jj[s][1];
				t.width = jj[s][2];
				t.height = jj[s][3];
				_sprites.push_back(t);
			} catch (std::exception& e) {
				_logger.warning("An error occurred when attempting to add sprite "
					"coordinates to the internal vector: {}.", e.what());
				_logger.write("The coordinates 0,0,0,0 will be given to sprite "
					"{}.", s);
				_sprites.push_back(sf::IntRect());
			}
		}
	} catch (std::exception& e) {
		_logger.warning("An error occurred when attempting to read sprite "
			"coordinates: {}.", e.what());
		_logger.write("The coordinates 0,0,0,0 will be given to the single "
			"sprite.");
		_sprites.push_back(sf::IntRect());
	}

	return _loadImages(frameCount);
}

bool sfx::animated_spritesheet::_save(nlohmann::ordered_json& j) noexcept {
	j["path"] = _basepath;
	j["frames"] = getFrameCount();
	j["framerate"] = getFramerate();
	for (auto& itr : _sprites) {
		std::array<int, 4> rect;
		rect[0] = itr.left;
		rect[1] = itr.top;
		rect[2] = itr.width;
		rect[3] = itr.height;
		j["sprites"].push_back(rect);
	}
	return true;
}

bool sfx::animated_spritesheet::_loadImages(unsigned int expectedFrames) noexcept {
	_frames.clear();
	bool result = true;
	for (unsigned int i = 0; i < expectedFrames; i++) {
		std::string singlePath = _basepath;
		singlePath.insert(_basepath.find_last_of('.'), "_" + std::to_string(i));
		sf::Texture test;
		if (!test.loadFromFile(singlePath)) {
			_logger.error("Couldn't load image file \"{}\".", singlePath);
			result = false;
		}
		_frames.push_back(test);
	}
	return result;
}

sfx::animated_sprite::animated_sprite(const std::string& name) noexcept :
	_logger(name) {}

sfx::animated_sprite::animated_sprite(
	std::shared_ptr<const sfx::animated_spritesheet> sheet, unsigned int sprite,
	const std::string& name) noexcept : _logger(name) {
	setSpritesheet(sheet);
	setSprite(sprite);
}

void sfx::animated_sprite::setSpritesheet(
	std::shared_ptr<const sfx::animated_spritesheet> sheet) noexcept {
	_sheet = sheet;
	if (!_sheet) _sprite.setTextureRect(sf::IntRect(0,0,0,0));
	_errored = false;
	_hasNotBeenDrawn = true;
	_currentFrame = 0;
}

void sfx::animated_sprite::setSprite(unsigned int sprite) noexcept {
	_spriteID = sprite;
	_errored = false;
	_hasNotBeenDrawn = true;
	_currentFrame = 0;
}

unsigned int sfx::animated_sprite::getSprite() const noexcept {
	return _spriteID;
}

bool sfx::animated_sprite::animate(const sf::RenderTarget& target) noexcept {
	if (!_sheet) return true;
	try {
		if (_hasNotBeenDrawn) {
			_hasNotBeenDrawn = false;
			_clock.restart();
		} else {
			if (_sheet->getFramerate() > 0.0 &&
				_clock.getElapsedTime().asSeconds() >= 1.0 /
				_sheet->getFramerate()) {
				++(*this);
				_clock.restart();
			}
		}
		_sprite.setTexture(_sheet->accessTexture(_currentFrame));
		_sprite.setTextureRect(_sheet->accessSprite(_spriteID));
		return getCurrentFrame() == _sheet->getFrameCount() - 1;
	} catch (std::out_of_range& e) {
		if (!_errored) {
			_logger.error("Attempted to access non-existent frame {} of sprite {}"
				": {}", _currentFrame, _spriteID, e.what());
			_errored = true;
		}
		return true;
	}
}

unsigned int sfx::animated_sprite::getCurrentFrame() const noexcept {
	return _currentFrame;
}

void sfx::animated_sprite::setCurrentFrame(unsigned int newFrame) noexcept {
	if (!_sheet) return;
	if (newFrame >= _sheet->getFrameCount()) newFrame = 0;
	_currentFrame = newFrame;
}

unsigned int sfx::animated_sprite::operator++() noexcept {
	setCurrentFrame(getCurrentFrame() + 1);
	return getCurrentFrame();
}

unsigned int sfx::animated_sprite::operator++(int) noexcept {
	auto old = getCurrentFrame();
	setCurrentFrame(getCurrentFrame() + 1);
	return old;
}

unsigned int sfx::animated_sprite::operator--() noexcept {
	if (!_sheet) return _currentFrame;
	if ((int)_currentFrame - 1 < 0) {
		setCurrentFrame(_sheet->getFrameCount() - 1);
	} else {
		setCurrentFrame(getCurrentFrame() - 1);
	}
	return getCurrentFrame();
}

unsigned int sfx::animated_sprite::operator--(int) noexcept {
	auto old = getCurrentFrame();
	--(*this);
	return old;
}

sf::Vector2f sfx::animated_sprite::getSize() const noexcept {
	return sf::Vector2f((float)_sprite.getTextureRect().width,
		(float)_sprite.getTextureRect().height);
}

void sfx::animated_sprite::setPosition(const sf::Vector2f& newPosition) noexcept {
	_sprite.setPosition(newPosition);
}

sf::Vector2f sfx::animated_sprite::getPosition() const noexcept {
	return _sprite.getPosition();
}

void sfx::animated_sprite::draw(sf::RenderTarget& target, sf::RenderStates states)
	const {
	target.draw(_sprite, states);
}