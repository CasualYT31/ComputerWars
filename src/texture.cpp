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

// CONSTRUCTION ZONE

sfx::animated_spritesheet::animated_spritesheet(const std::string& name) noexcept : _logger(name) {}

const sf::Texture& sfx::animated_spritesheet::accessTexture(unsigned int frameID) const {
	if (frameID >= _frames.size()) {
		std::string msg = "Attempted to access non-existent frame " + std::to_string(frameID) + ".";
		// _logger.error(msg);
		throw std::out_of_range(msg);
	} else {
		return _frames.at(frameID);
	}
}

sf::IntRect sfx::animated_spritesheet::accessSprite(unsigned int spriteID) const {
	if (spriteID >= _sprites.size()) {
		std::string msg = "Attempted to access non-existent sprite " + std::to_string(spriteID) + ".";
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

bool sfx::animated_spritesheet::_load(safe::json& j) noexcept {
	std::string temp_basepath;
	unsigned int frameCount = 1;
	j.apply(temp_basepath, { "path" });
	if (!j.inGoodState()) {
		_logger.error("No base path provided - aborting load attempt.");
		return false;
	}
	if (temp_basepath.find('.') == std::string::npos) {
		_logger.error("Invalid base path provided - '.' is required.");
		return false;
	}
	j.apply(frameCount, { "frames" }, &frameCount, true);
	if (frameCount == 0) {
		_logger.error("Cannot have 0 frames: assuming 1...");
		frameCount = 1;
	}
	j.apply(_framerate, { "framerate" }, &_framerate, true);

	_basepath = temp_basepath;
	nlohmann::json jj = j.nlohmannJSON();
	try {
		jj = jj["sprites"];
		for (std::size_t s = 0; s < jj.size(); s++) {
			//initial checks have passed, so the sprites vector can now be cleared
			if (s == 0) _sprites.clear();
			try {
				sf::IntRect t;
				t.left = jj[s][0];
				t.top = jj[s][1];
				t.width = jj[s][2];
				t.height = jj[s][3];
				_sprites.push_back(t);
			} catch (std::exception& e) {
				_logger.error("An error occurred when attempting to add sprite coordinates to the internal vector: {}.", e.what());
				_logger.write("The coordinates 0,0,0,0 will be given to sprite {}.", s);
				_sprites.push_back(sf::IntRect());
			}
		}
	} catch (std::exception& e) {
		_logger.error("An error occurred when attempting to read sprite coordinates: {}.", e.what());
		_logger.write("The coordinates 0,0,0,0 will be given to the single sprite.");
		_sprites.push_back(sf::IntRect());
	}

	return _loadImages(frameCount);
}

bool sfx::animated_spritesheet::_save(nlohmann::json& j) noexcept {
	j["path"] = _basepath;
	j["frames"] = getFrameCount();
	j["framerate"] = getFramerate();
	for (auto itr = _sprites.begin(), enditr = _sprites.end(); itr != enditr; itr++) {
		std::array<int, 4> rect;
		rect[0] = itr->left;
		rect[1] = itr->top;
		rect[2] = itr->width;
		rect[3] = itr->height;
		j["sprites"].push_back(rect);
	}
	return true;
}

bool sfx::animated_spritesheet::_loadImages(unsigned int expectedFrames) noexcept {
	_frames.clear();
	for (unsigned int i = 0; i < expectedFrames; i++) {
		std::string singlePath = _basepath;
		singlePath.insert(_basepath.find_last_of('.'), "_" + std::to_string(i));
		sf::Texture test;
		if (!test.loadFromFile(singlePath)) {
			if (i != expectedFrames) {
				_logger.error("{} image{} were loaded for this spritesheet object, expected {}!", i, ((i == 1) ? ("") : ("s")), expectedFrames);
				return false;
			} else {
				return true;
			}
		}
		_frames.push_back(test);
	}
}

sfx::animated_sprite::animated_sprite(std::shared_ptr<const sfx::animated_spritesheet> sheet, unsigned int sprite, const std::string& name) noexcept : _logger(name) {
	setSpritesheet(sheet);
	setSprite(sprite);
}

void sfx::animated_sprite::setSpritesheet(std::shared_ptr<const sfx::animated_spritesheet> sheet) noexcept {
	_sheet = sheet;
	if (!_sheet) _logger.error("No animated spritesheet has been assigned to this sprite.");
	_errored = false;
}

void sfx::animated_sprite::setSprite(unsigned int sprite) noexcept {
	_spriteID = sprite;
	_errored = false;
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
			if (_sheet->getFramerate() > 0.0 && _clock.getElapsedTime().asSeconds() >= 1.0 / _sheet->getFramerate()) {
				++(*this);
				_clock.restart();
			}
		}
		_sprite.setTexture(_sheet->accessTexture(_currentFrame));
		_sprite.setTextureRect(_sheet->accessSprite(_spriteID));
	} catch (std::out_of_range& e) {
		if (!_errored) {
			_logger.error("Attempted to access non-existent frame {} of sprite {}: {}", _currentFrame, _spriteID, e.what());
			_errored = true;
		}
		return true;
	}
	return getCurrentFrame() == 0;
}

unsigned int sfx::animated_sprite::getCurrentFrame() const noexcept {
	return _currentFrame;
}

unsigned int sfx::animated_sprite::setCurrentFrame(unsigned int newFrame) noexcept {
	if (!_sheet) return _currentFrame;
	auto old = _currentFrame;
	if (newFrame >= _sheet->getFrameCount()) newFrame = 0;
	_currentFrame = newFrame;
	return old;
}

unsigned int sfx::animated_sprite::operator++() noexcept {
	setCurrentFrame(getCurrentFrame() + 1);
	return getCurrentFrame();
}

unsigned int sfx::animated_sprite::operator++(int) noexcept {
	return setCurrentFrame(getCurrentFrame() + 1);
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

void sfx::animated_sprite::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(_sprite, states);
}

// END CONSTRUCTION ZONE