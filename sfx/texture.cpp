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

unsigned int sfx::animated_spritesheet::getFrameCount() const noexcept {
	return static_cast<unsigned int>(_frames.size());
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
	j.apply(frameCount, { "frames" }, true);
	if (frameCount == 0) {
		_logger.warning("Cannot have 0 frames: assuming 1...");
		frameCount = 1;
	}
	j.apply(_framerate, { "framerate" }, true);
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

//
// CONSTRUCTION ZONE
//

sfx::new_animated_spritesheet::new_animated_spritesheet(const std::string& name)
	noexcept : _logger(name) {}

const sf::Texture& sfx::new_animated_spritesheet::getTexture() const noexcept {
	return _texture;
}

std::size_t sfx::new_animated_spritesheet::getFrameCount(
	const std::string& sprite) const noexcept {
	try {
		return _frames.at(sprite).size();
	} catch (std::out_of_range&) {
		_logger.error("Error whilst attempting to retrieve the frame count of the "
			"sprite \"{}\": it does not exist!", sprite);
	}
	return 0;
}

sf::IntRect sfx::new_animated_spritesheet::getFrameRect(const std::string& sprite,
	const std::size_t frame) const noexcept {
	try {
		const std::vector<sf::IntRect>& frames = _frames.at(sprite);
		try {
			return frames.at(frame);
		} catch (std::out_of_range&) {
			_logger.error("Error whilst attempting to retrieve the rect of frame "
				"{} of the sprite \"{}\": the frame does not exist!", frame,
				sprite);
		}
	} catch (std::out_of_range&) {
		_logger.error("Error whilst attempting to retrieve the rect of frame {} "
			"of the sprite \"{}\": the sprite does not exist!", frame, sprite);
	}
	return sf::IntRect();
}

sf::Time sfx::new_animated_spritesheet::getFrameDuration(const std::string& sprite,
	const std::size_t frame) const noexcept {
	try {
		const std::vector<sf::Time>& frames = _durations.at(sprite);
		try {
			return frames.at(frame);
		} catch (std::out_of_range&) {
			_logger.error("Error whilst attempting to retrieve the duration of "
				"frame {} of the sprite \"{}\": the frame does not exist!", frame,
				sprite);
		}
	} catch (std::out_of_range&) {
		_logger.error("Error whilst attempting to retrieve the duration of frame "
			"{} of the sprite \"{}\": the sprite does not exist!", frame, sprite);
	}
	return sf::Time();
}

bool sfx::new_animated_spritesheet::_load(engine::json& j) noexcept {
	// firstly, load the spritesheet
	std::string imgpath;
	j.apply(imgpath, { "path" });
	if (!j.inGoodState()) {
		_logger.error("No path to a spritesheet graphic has been provided - "
			"aborting load attempt.");
		return false;
	}
	if (!_texture.loadFromFile(imgpath)) {
		_logger.error("Couldn't load image file \"{}\" - aborting load attempt.",
			imgpath);
		return false;
	}
	// secondly, go through all the sprites and store the info
	_frames.clear();
	_durations.clear();
	nlohmann::ordered_json jj = j.nlohmannJSON();
	try {
		for (auto& i : jj["sprites"].items()) {
			// ignore any key-value pairs where the value is not an object
			if (i.value().is_object()) {
				// go through the frames
				for (std::size_t f = 0; f < i.value()["frames"].size(); f++) {
					sf::IntRect rect;
					rect.left = i.value()["frames"][f][0];
					rect.top = i.value()["frames"][f][1];
					rect.width = i.value()["frames"][f][2];
					rect.height = i.value()["frames"][f][3];
					_frames[i.key()].push_back(rect);
				}
				// go through the durations
				for (std::size_t f = 0; f < i.value()["durations"].size(); f++) {
					sf::Time time;
					time = sf::milliseconds(i.value()["durations"][f]);
					_durations[i.key()].push_back(time);
				}
				// report a warning if the frame count did not match the duration
				// count
				if (_frames[i.key()].size() != _durations[i.key()].size()) {
					_logger.warning("The number of frames for sprite \"{}\" was "
						"{} and the number of durations was {}.", i.key(),
						_frames[i.key()].size(), _durations[i.key()].size());
				}
			}
		}
	} catch (nlohmann::json::exception& e) {
		_logger.error("There was an error whilst attempting to read sprite info: "
			"{}.", e.what());
	}
	return true;
}

bool sfx::new_animated_spritesheet::_save(nlohmann::ordered_json& j) noexcept {
	return false;
}

sfx::new_animated_sprite::new_animated_sprite(const std::string& name) noexcept :
	_logger(name) {}

sfx::new_animated_sprite::new_animated_sprite(
	std::shared_ptr<const sfx::new_animated_spritesheet> sheet, const std::string& sprite,
	const std::string& name) noexcept : _logger(name) {
	setSpritesheet(sheet);
	setSprite(sprite);
}

void sfx::new_animated_sprite::setSpritesheet(
	std::shared_ptr<const sfx::new_animated_spritesheet> sheet) noexcept {
	_sheet = sheet;
	if (!_sheet) _sprite.setTextureRect(sf::IntRect(0, 0, 0, 0));
	_errored = false;
	_currentFrame = 0;
}

void sfx::new_animated_sprite::setSprite(const std::string& sprite) noexcept {
	_spriteID = sprite;
	_errored = false;
	_currentFrame = 0;
}

std::string sfx::new_animated_sprite::getSprite() const noexcept {
	return _spriteID;
}

bool sfx::new_animated_sprite::animate(const sf::RenderTarget& target) noexcept {
	if (!_sheet) return true;
	try {
		if (_sheet->getFrameDuration(_spriteID, _currentFrame).asMilliseconds() > 0) {
			float delta = accumulatedDelta();
			while (delta >= _sheet->getFrameDuration(_spriteID, _currentFrame).asSeconds()) {
				delta -= _sheet->getFrameDuration(_spriteID, _currentFrame).asSeconds();
				++(*this);
			}
			resetDeltaAccumulation(delta);
		}
		_sprite.setTexture(_sheet->getTexture());
		_sprite.setTextureRect(_sheet->getFrameRect(_spriteID,_currentFrame));
		return getCurrentFrame() == _sheet->getFrameCount(_spriteID) - 1;
	} catch (std::out_of_range& e) {
		if (!_errored) {
			_logger.error("Attempted to access non-existent frame {} of sprite {}"
				": {}", _currentFrame, _spriteID, e.what());
			_errored = true;
		}
		return true;
	}
}

unsigned int sfx::new_animated_sprite::getCurrentFrame() const noexcept {
	return _currentFrame;
}

void sfx::new_animated_sprite::setCurrentFrame(unsigned int newFrame) noexcept {
	if (!_sheet) return;
	if (newFrame >= _sheet->getFrameCount(_spriteID)) newFrame = 0;
	_currentFrame = newFrame;
}

unsigned int sfx::new_animated_sprite::operator++() noexcept {
	setCurrentFrame(getCurrentFrame() + 1);
	return getCurrentFrame();
}

unsigned int sfx::new_animated_sprite::operator++(int) noexcept {
	auto old = getCurrentFrame();
	setCurrentFrame(getCurrentFrame() + 1);
	return old;
}

unsigned int sfx::new_animated_sprite::operator--() noexcept {
	if (!_sheet) return _currentFrame;
	if ((int)_currentFrame - 1 < 0) {
		setCurrentFrame(_sheet->getFrameCount(_spriteID) - 1);
	} else {
		setCurrentFrame(getCurrentFrame() - 1);
	}
	return getCurrentFrame();
}

unsigned int sfx::new_animated_sprite::operator--(int) noexcept {
	auto old = getCurrentFrame();
	--(*this);
	return old;
}

sf::Vector2f sfx::new_animated_sprite::getSize() const noexcept {
	return sf::Vector2f((float)_sprite.getTextureRect().width,
		(float)_sprite.getTextureRect().height);
}

void sfx::new_animated_sprite::setPosition(const sf::Vector2f& newPosition) noexcept {
	_sprite.setPosition(newPosition);
}

sf::Vector2f sfx::new_animated_sprite::getPosition() const noexcept {
	return _sprite.getPosition();
}

void sfx::new_animated_sprite::draw(sf::RenderTarget& target, sf::RenderStates states)
const {
	target.draw(_sprite, states);
}

//
// END OF ZONE
//

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
	_currentFrame = 0;
}

void sfx::animated_sprite::setSprite(unsigned int sprite) noexcept {
	_spriteID = sprite;
	_errored = false;
	_currentFrame = 0;
}

unsigned int sfx::animated_sprite::getSprite() const noexcept {
	return _spriteID;
}

bool sfx::animated_sprite::animate(const sf::RenderTarget& target) noexcept {
	if (!_sheet) return true;
	try {
		if (_sheet->getFramerate() > 0.0) {
			auto framesToAdvance = static_cast<std::uint64_t>(
				static_cast<double>(accumulatedDelta()) /
				(1.0 / _sheet->getFramerate()));
			if (framesToAdvance) resetDeltaAccumulation();
			for (std::uint64_t x = 0; x < framesToAdvance; x++) ++(*this);
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