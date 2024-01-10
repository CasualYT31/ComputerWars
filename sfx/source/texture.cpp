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

#include "texture.hpp"

sfx::delta_timer sfx::animated_spritesheet::_globalTimer = {};

sfx::animated_spritesheet::animated_spritesheet(const engine::logger::data& data) :
	json_script({ data.sink, "json_script" }), _logger(data) {}

const sf::Texture& sfx::animated_spritesheet::getTexture() const noexcept {
	return _texture;
}

std::size_t sfx::animated_spritesheet::getFrameCount(
	const std::string& sprite) const noexcept {
	try {
		return _data.at(sprite).frames.size();
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
		const std::vector<sf::IntRect>& frames = _data.at(sprite).frames;
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
		const std::vector<sf::Time>& frames = _data.at(sprite).durations;
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

std::vector<sf::Time> sfx::animated_spritesheet::getFrameDurations(
	const std::string& sprite) const {
	try {
		return _data.at(sprite).durations;
	} catch (const std::out_of_range&) {
		/*
		_logger.error("Error whilst attempting to retrieve the durations of "
			"sprite \"{}\": the sprite does not exist!", sprite);
			*/
	}
	return {};
}

sf::Vector2f sfx::animated_spritesheet::getSpriteOffset(const std::string& sprite)
	const {
	try {
		return _data.at(sprite).offset;
	} catch (const std::out_of_range&) {
		/*
		_logger.error("Error whilst attempting to retrieve the offset of sprite "
			"\"{}\": the sprite does not exist!", sprite); */
	}
	return {};
}

bool sfx::animated_spritesheet::doesSpriteHaveGlobalFrameID(
	const std::string& sprite) const {
	try {
		return _data.at(sprite).globalFrameCounter;
	} catch (const std::out_of_range&) {
		/*
		_logger.error("Error whilst attempting to retrieve the globalFrameCounter "
			"property of sprite \"{}\": the sprite does not exist!", sprite); */
	}
	return false;
}

std::size_t sfx::animated_spritesheet::getSpriteGlobalFrameID(
	const std::string& sprite) const {
	try {
		return _data.at(sprite).globalFrameID;
	} catch (const std::out_of_range&) {
		/*
		_logger.error("Error whilst attempting to retrieve the globalFrameID "
			"property of sprite \"{}\": the sprite does not exist!", sprite); */
	}
	return false;
}

/**
 * Increments a sprite's frame counter.
 * @param frameDuration The duration of the sprite's current frame.
 * @param frameCount    The number of frames the sprite has.
 * @param timer         The delta timer used to acquire accumulated delta.
 * @param frameID       The frame counter to increment.
 * @param deltaOffset   If \c nullptr, the \c delta_timer's accumulated delta will
 *                      be reset. If a pointer is given, it will be assigned to the
 *                      delta that's been accumulated so far. It will be assumed
 *                      that the timer is not being reset, so the value of the
 *                      float will be used to offset the accumulated delta, causing
 *                      a reset local to a given sprite.
 * @return \c TRUE if the frame ID has wrapped at least once, or is about to wrap,
 *         \c FALSE otherwise.
 */
static bool incrementFrameID(const std::vector<sf::Time>& frameDurations,
	const std::size_t frameCount, sfx::delta_timer& timer, std::size_t& frameID,
	float* deltaOffset = nullptr) {
	bool frameWrap = false;
	if (frameDurations[frameID].asMilliseconds() > 0) {
		const float accumulatedDelta = timer.accumulatedDelta();
		const float offset = deltaOffset ? *deltaOffset : 0.f;
		float delta = accumulatedDelta - offset;
		while (delta >= frameDurations[frameID].asSeconds()) {
			delta -= frameDurations[frameID].asSeconds();
			if (++frameID >= frameCount) {
				frameID = 0;
				frameWrap = true;
			}
		}
		// If we are at the last frame, and we're almost about to wrap, then
		// prematurely end the animation to prevent showing the first frame for a
		// single frame.
		if (frameID == frameCount - 1 &&
			frameDurations[frameID].asSeconds() - delta <= 0.05f) {
			frameWrap = true;
		}
		if (deltaOffset) *deltaOffset = accumulatedDelta - delta;
		else timer.resetDeltaAccumulation(delta);
	}
	return frameWrap;
}

void sfx::animated_spritesheet::updateGlobalFrameIDs() {
	for (auto& data : _data) {
		if (!data.second.globalFrameCounter) continue;
		incrementFrameID(data.second.durations, data.second.frames.size(),
			_globalTimer, data.second.globalFrameID,
			&data.second.accumulatedDeltaOffset);
	}
}

std::size_t sfx::animated_spritesheet::heightOfTallestFrame() const {
	return _tallestSpriteHeight;
}

void sfx::animated_spritesheet::setPathOverride(const std::string& path) {
	_pathOverride = path;
}

bool sfx::animated_spritesheet::_load(engine::json& j) {
	// Firstly, load the spritesheet.
	std::string imgpath;
	if (_pathOverride) {
		imgpath = *_pathOverride;
		_logger.write("Path override has been issued: \"{}\"", imgpath);
	} else {
		j.apply(imgpath, { "path" });
		if (!j.inGoodState()) {
			_logger.error("No path to a spritesheet graphic has been provided - "
				"aborting load attempt.");
			return false;
		}
	}
	if (!_texture.loadFromFile(imgpath)) {
		_logger.error("Couldn't load image file \"{}\" - aborting load attempt.",
			imgpath);
		return false;
	}
	// Secondly, go through all the sprites and store the info.
	_data.clear();
	_tallestSpriteHeight = 0;
	nlohmann::ordered_json jj = j.nlohmannJSON();
	try {
		for (auto& i : jj["sprites"].items()) {
			// Ignore any key-value pairs where the value is not an object.
			if (i.value().is_object()) {
				sfx::animated_spritesheet::sprite_data data;
				// Go through the frames.
				for (std::size_t f = 0; f < i.value()["frames"].size(); f++) {
					sf::IntRect rect;
					rect.left = i.value()["frames"][f][0];
					rect.top = i.value()["frames"][f][1];
					rect.width = i.value()["frames"][f][2];
					rect.height = i.value()["frames"][f][3];
					data.frames.push_back(rect);
					if (rect.height > static_cast<int>(_tallestSpriteHeight))
						_tallestSpriteHeight = rect.height;
				}
				// Go through the durations.
				if (i.value()["durations"].is_array()) {
					for (std::size_t f = 0; f < i.value()["durations"].size();
						f++) {
						sf::Time time;
						time = sf::milliseconds(i.value()["durations"][f]);
						data.durations.push_back(time);
					}
					// Report a warning if the frame count did not match the
					// duration count.
					if (data.frames.size() != data.durations.size()) {
						_logger.warning("The number of frames for sprite \"{}\" "
							"was {} and the number of durations was {}.", i.key(),
							data.frames.size(), data.durations.size());
					}
				} else data.durations.resize(data.frames.size(),
					sf::milliseconds(i.value()["durations"]));
				// Read the sprite's offset, if one was given.
				if (i.value().contains("offset")) {
					std::array<float, 2> offset;
					j.applyArray(offset, { "sprites", i.key(), "offset" });
					j.resetState();
					data.offset = sf::Vector2f(offset.at(0), offset.at(1));
				}
				// Does every copy of this sprite animate in sync?
				if (i.value().contains("globalframeid")) {
					j.apply(data.globalFrameCounter,
						{ "sprites", i.key(), "globalframeid" });
					j.resetState();
				}
				// Store the sprite's data.
				_data[i.key()] = std::move(data);
			}
		}
	} catch (const nlohmann::json::exception& e) {
		_logger.error("There was an error whilst attempting to read sprite info: "
			"{}.", e);
	}
	return true;
}

bool sfx::animated_spritesheet::_save(nlohmann::ordered_json& j) noexcept {
	return false;
}

sfx::animated_spritesheets::animated_spritesheets(const engine::logger::data& data)
	: resource_pool<sfx::animated_spritesheet>({ data.sink, "json_script" },
		"spritesheet") {}

void sfx::animated_spritesheets::updateGlobalFrameIDs() {
	for (auto& sheet : _pool) sheet.second->updateGlobalFrameIDs();
}

bool sfx::animated_spritesheets::_load(engine::json& j) {
	PoolType sheets;
	bool ret = true;
	nlohmann::ordered_json jj = j.nlohmannJSON();
	for (auto& i : jj.items()) {
		sheets[i.key()] = std::make_shared<sfx::animated_spritesheet>(
			engine::logger::data{ _logger.getData().sink,
				_logger.getData().name + "_" + i.key() });
		if (i.value().is_string()) {
			sheets[i.key()]->load(i.value());
		} else if (i.value().is_object()) {
			if (i.value().find("json") == i.value().end()) {
				_logger.error("Could not load {} with name \"{}\" as its object "
					"value did not contain the \"json\" key.", _objectType,
					i.key());
				sheets.erase(i.key());
				ret = false;
				continue;
			} else if (i.value().find("path") == i.value().end()) {
				_logger.error("Could not load {} with name \"{}\" as its object "
					"value did not contain the \"path\" key.", _objectType,
					i.key());
				sheets.erase(i.key());
				ret = false;
				continue;
			}
			sheets[i.key()]->setPathOverride(i.value()["path"]);
			sheets[i.key()]->load(i.value()["json"]);
		} else {
			_logger.error("Could not load {} with name \"{}\" as its value was of "
				"unrecognised type \"{}\".", _objectType, i.key(),
				i.value().type_name());
			sheets.erase(i.key());
			ret = false;
			continue;
		}
		if (!sheets[i.key()]->inGoodState()) ret = false;
	}
	_pool = std::move(sheets);
	return ret;
}

bool sfx::animated_spritesheets::_save(nlohmann::ordered_json& j) {
	for (const auto& pair : _pool) j[pair.first] = pair.second->getScriptPath();
	return true;
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
	if (!_sheet || _spriteID.empty()) return;
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
	if (!_sheet || !_sheet->doesSpriteExist(_spriteID)) return true;
	try {
		bool ret = false;
		if (_sheet->doesSpriteHaveGlobalFrameID(_spriteID)) {
			_currentFrame = _sheet->getSpriteGlobalFrameID(_spriteID);
		} else {
			ret = incrementFrameID(_sheet->getFrameDurations(_spriteID),
				_sheet->getFrameCount(_spriteID), *this, _currentFrame);
		}
		_sprite.setTexture(_sheet->getTexture());
		_sprite.setTextureRect(_sheet->getFrameRect(_spriteID, _currentFrame));
		return ret;
	} catch (const std::out_of_range& e) {
		if (!_errored) {
			_logger.error("Attempted to access non-existent frame {} of sprite {}"
				": {}", _currentFrame, _spriteID, e);
			_errored = true;
		}
		return true;
	}
}

void sfx::animated_sprite::draw(sf::RenderTarget& target,
	sf::RenderStates states) const {
	if (!_sheet || _spriteID.empty()) return;
	states.transform.translate(_sheet->getSpriteOffset(_spriteID));
	target.draw(_sprite, states);
}
