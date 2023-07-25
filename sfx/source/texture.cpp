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

#include "texture.hpp"

sfx::animated_spritesheet::animated_spritesheet(const engine::logger::data& data) :
	json_script({ data.sink, "json_script" }), _logger(data) {}

const sf::Texture& sfx::animated_spritesheet::getTexture() const noexcept {
	return _texture;
}

std::size_t sfx::animated_spritesheet::getFrameCount(
	const std::string& sprite) const noexcept {
	try {
		return _frames.at(sprite).size();
	} catch (const std::out_of_range&) {
		/*
		_logger.error("Error whilst attempting to retrieve the frame count of the "
			"sprite \"{}\": it does not exist!", sprite); */
	}
	return 0;
}

sf::IntRect sfx::animated_spritesheet::getFrameRect(const std::string& sprite,
	const std::size_t frame) const {
	try {
		const std::vector<sf::IntRect>& frames = _frames.at(sprite);
		try {
			return frames.at(frame);
		} catch (const std::out_of_range&) {
			/*
			_logger.error("Error whilst attempting to retrieve the rect of frame "
				"{} of the sprite \"{}\": the frame does not exist!", frame,
				sprite); */
		}
	} catch (const std::out_of_range&) {
		/*
		_logger.error("Error whilst attempting to retrieve the rect of frame {} "
			"of the sprite \"{}\": the sprite does not exist!", frame, sprite); */
	}
	return sf::IntRect();
}

sf::Time sfx::animated_spritesheet::getFrameDuration(const std::string& sprite,
	const std::size_t frame) const {
	try {
		const std::vector<sf::Time>& frames = _durations.at(sprite);
		try {
			return frames.at(frame);
		} catch (const std::out_of_range&) {
			/*
			_logger.error("Error whilst attempting to retrieve the duration of "
				"frame {} of the sprite \"{}\": the frame does not exist!", frame,
				sprite); */
		}
	} catch (const std::out_of_range&) {
		/*
		_logger.error("Error whilst attempting to retrieve the duration of frame "
			"{} of the sprite \"{}\": the sprite does not exist!", frame, sprite);
			*/
	}
	return sf::Time();
}

sf::Vector2f sfx::animated_spritesheet::getSpriteOffset(const std::string& sprite)
	const {
	try {
		return _offsets.at(sprite);
	} catch (const std::out_of_range&) {
		/*
		_logger.error("Error whilst attempting to retrieve the offset of sprite "
			"\"{}\": the sprite does not exist!", sprite); */
	}
	return {};
}

bool sfx::animated_spritesheet::_load(engine::json& j) {
	// Firstly, load the spritesheet.
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
	// Secondly, go through all the sprites and store the info.
	_frames.clear();
	_durations.clear();
	nlohmann::ordered_json jj = j.nlohmannJSON();
	try {
		for (auto& i : jj["sprites"].items()) {
			// Ignore any key-value pairs where the value is not an object.
			if (i.value().is_object()) {
				// Go through the frames.
				for (std::size_t f = 0; f < i.value()["frames"].size(); f++) {
					sf::IntRect rect;
					rect.left = i.value()["frames"][f][0];
					rect.top = i.value()["frames"][f][1];
					rect.width = i.value()["frames"][f][2];
					rect.height = i.value()["frames"][f][3];
					_frames[i.key()].push_back(rect);
				}
				// Go through the durations.
				for (std::size_t f = 0; f < i.value()["durations"].size(); f++) {
					sf::Time time;
					time = sf::milliseconds(i.value()["durations"][f]);
					_durations[i.key()].push_back(time);
				}
				// Report a warning if the frame count did not match the duration
				// count.
				if (_frames[i.key()].size() != _durations[i.key()].size()) {
					_logger.warning("The number of frames for sprite \"{}\" was "
						"{} and the number of durations was {}.", i.key(),
						_frames[i.key()].size(), _durations[i.key()].size());
				}
				// Read the sprite's offset, if one was given.
				_offsets[i.key()] = sf::Vector2f();
				if (i.value().contains("offset")) {
					std::array<float, 2> offset;
					j.applyArray(offset, { "sprites", i.key(), "offset" });
					j.resetState();
					_offsets[i.key()] = sf::Vector2f(offset.at(0), offset.at(1));
				}
			}
		}
	} catch (const nlohmann::json::exception& e) {
		_logger.error("There was an error whilst attempting to read sprite info: "
			"{}.", e.what());
	}
	return true;
}

bool sfx::animated_spritesheet::_save(nlohmann::ordered_json& j) noexcept {
	return false;
}

sfx::animated_sprite::animated_sprite(const engine::logger::data& data) :
	_logger(data) {}

sfx::animated_sprite::animated_sprite(
	std::shared_ptr<const sfx::animated_spritesheet> sheet,
	const std::string& sprite, const engine::logger::data& data) : _logger(data) {
	setSpritesheet(sheet);
	setSprite(sprite);
}

void sfx::animated_sprite::setSpritesheet(
	std::shared_ptr<const sfx::animated_spritesheet> sheet) {
	_sheet = sheet;
	if (!_sheet) _sprite.setTextureRect(sf::IntRect(0, 0, 0, 0));
	_errored = false;
	_currentFrame = 0;
}

std::shared_ptr<const sfx::animated_spritesheet>
	sfx::animated_sprite::getSpritesheet() const {
	return _sheet;
}

void sfx::animated_sprite::setSprite(const std::string& sprite) {
	if (sprite != _spriteID) {
		_spriteID = sprite;
		_errored = false;
		_currentFrame = 0;
	}
}

std::string sfx::animated_sprite::getSprite() const {
	return _spriteID;
}

std::size_t sfx::animated_sprite::getCurrentFrame() const noexcept {
	return _currentFrame;
}

void sfx::animated_sprite::setCurrentFrame(std::size_t newFrame) noexcept {
	if (!_sheet) return;
	if (newFrame >= _sheet->getFrameCount(_spriteID)) newFrame = 0;
	_currentFrame = newFrame;
}

std::size_t sfx::animated_sprite::operator++() noexcept {
	setCurrentFrame(getCurrentFrame() + 1);
	return getCurrentFrame();
}

std::size_t sfx::animated_sprite::operator++(int) noexcept {
	auto old = getCurrentFrame();
	setCurrentFrame(getCurrentFrame() + 1);
	return old;
}

std::size_t sfx::animated_sprite::operator--() noexcept {
	if (!_sheet) return _currentFrame;
	if ((int)_currentFrame - 1 < 0) {
		setCurrentFrame(_sheet->getFrameCount(_spriteID) - 1);
	} else {
		setCurrentFrame(getCurrentFrame() - 1);
	}
	return getCurrentFrame();
}

std::size_t sfx::animated_sprite::operator--(int) noexcept {
	auto old = getCurrentFrame();
	--(*this);
	return old;
}

bool sfx::animated_sprite::animate(const sf::RenderTarget& target) {
	if (!_sheet) return true;
	try {
		if (_sheet->getFrameDuration(_spriteID, _currentFrame).asMilliseconds()
			> 0) {
			float delta = accumulatedDelta();
			while (delta >=
				_sheet->getFrameDuration(_spriteID, _currentFrame).asSeconds()) {
				delta -=
					_sheet->getFrameDuration(_spriteID, _currentFrame).asSeconds();
				++(*this);
			}
			resetDeltaAccumulation(delta);
		}
		_sprite.setTexture(_sheet->getTexture());
		_sprite.setTextureRect(_sheet->getFrameRect(_spriteID, _currentFrame));
		return getCurrentFrame() == _sheet->getFrameCount(_spriteID) - 1;
	} catch (const std::out_of_range& e) {
		if (!_errored) {
			_logger.error("Attempted to access non-existent frame {} of sprite {}"
				": {}", _currentFrame, _spriteID, e.what());
			_errored = true;
		}
		return true;
	}
}

void sfx::animated_sprite::draw(sf::RenderTarget& target, sf::RenderStates states)
	const {
	if (_sheet) states.transform.translate(_sheet->getSpriteOffset(_spriteID));
	target.draw(_sprite, states);
}
