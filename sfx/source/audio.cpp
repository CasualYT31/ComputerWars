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

#include "audio.hpp"

sfx::audio::audio(const engine::logger::data& data) :
	json_script({data.sink, "json_script"}), _logger(data) {}

void sfx::audio::process() {
	while (!_queue.empty() && _queue.front()()) _queue.pop();
}

float sfx::audio::getVolume() const noexcept {
	return _volume;
}

void sfx::audio::setVolume(float newvolume) {
	_validateVolume(newvolume);
	_volume = newvolume;
	for (auto itr = _sound.begin(), enditr = _sound.end(); itr != enditr; ++itr) {
		itr->second.sound.setVolume(_volumeAfterOffset(itr->first));
	}
	for (auto itr = _music.begin(), enditr = _music.end(); itr != enditr; ++itr) {
		itr->second.music.setVolume(_volumeAfterOffset(itr->first));
	}
}

void sfx::audio::play(const std::string& name, const sf::Time& length) {
	if (name.empty()) return;
	if (_sound.find(name) != _sound.end()) {
		// Play the sound /*if not already playing*/.
		/*if (_sound[name].sound.getStatus() != sf::Sound::Playing)*/
			_sound[name].sound.play();
	} else if (_music.find(name) != _music.end()) {
		// If the music is in sound mode, play immediately like a sound.
		if (_music[name].soundMode) {
			_music[name].music.play();
			return;
		}
		// Queue a stop action, and then queue the play action.
		stop(length);
		_queue.push(std::bind([this](const std::string& name) -> bool {
			_music[name].music.play();
			_currentMusic = name;
			return true;
		}, name));
	} else {
		_logger.error("Audio object \"{}\" could not be found.", name);
	}
}

void sfx::audio::stop(const sf::Time& length) {
	_queue.push(std::bind([this](const sf::Time& length) -> bool {
		// If there is no music currently playing, do nothing.
		if (getCurrentMusic().empty()) return true;
		// If there is no fade out, or the fade out has ended, stop immediately.
		if (length.asMilliseconds() < 10 ||
			_music[getCurrentMusic()].music.getVolume() < 1.0) {
			_music[getCurrentMusic()].music.setVolume(
				_volumeAfterOffset(getCurrentMusic()));
			_music[getCurrentMusic()].music.stop();
			_currentMusic = "";
			return true;
		}
		// Otherwise, handle the fadeout.
		if (accumulatedDelta() >= length.asSeconds() / _granularity) {
			float newVolume = _music[getCurrentMusic()].music.getVolume() -
				_volumeAfterOffset(getCurrentMusic()) / _granularity;
			if (newVolume <= 1.0f) newVolume = 0.0f;
			_music[getCurrentMusic()].music.setVolume(newVolume);
			resetDeltaAccumulation();
		}
		return false;
	}, length));
}

void sfx::audio::stop(const std::string& name) {
	if (_music.find(name) != _music.end() && _music[name].soundMode) {
		_music[name].music.stop();
	} else if (_sound.find(name) != _sound.end()) {
		_sound[name].sound.stop();
	}
}

bool sfx::audio::_load(engine::json& j) {
	nlohmann::ordered_json jj = j.nlohmannJSON();
	while (!_queue.empty()) _queue.pop();
	_sound.clear();
	_music.clear();
	_currentMusic = "";

	for (auto& i : jj.items()) {
		std::string path;
		j.apply(path, { i.key(), "path" });
		if (!j.inGoodState()) {
			_logger.error("Audio object \"{}\" was not given a valid \"path\" "
				"value, in script \"{}\".", i.key(), getScriptPath());
			j.resetState();
			continue;
		}

		std::string type = "sound";
		j.apply(type, { i.key(), "type" }, true);
		if (type != "sound" && type != "music") {
			_logger.warning("Invalid type \"{}\" provided for audio object \"{}\" "
				"in script \"{}\", \"sound\" assumed.", type, i.key(),
				getScriptPath());
			type = "sound";
		}
		const bool soundMode = type == "sound" &&
			j.keysExist({ i.key(), "loopto" }) &&
			j.keysExist({ i.key(), "loopwhen" });
		if (type == "sound" && !soundMode) {
			_sound[i.key()].path = path;
			j.apply(_sound[i.key()].volumeOffset, { i.key(), "offset" }, true);
		} else if (type == "music" || soundMode) {
			_music[i.key()].path = path;
			j.apply(_music[i.key()].volumeOffset, { i.key(), "offset" }, true);
			j.apply(_music[i.key()].loopTo, { i.key(), "loopto" }, true);
			j.apply(_music[i.key()].loopWhen, { i.key(), "loopwhen" }, true);
			_music[i.key()].soundMode = soundMode;
		}
	}

	return _loadAudio();
}

bool sfx::audio::_loadAudio() {
	bool ret = true;
	for (auto itr = _sound.begin(), enditr = _sound.end(); itr != enditr; ++itr) {
		if (!itr->second.buffer.loadFromFile(itr->second.path)) {
			_logger.error("Audio file \"{}\" for sound object \"{}\" could not be "
				"loaded!", itr->second.path, itr->first);
			ret = false;
		} else {
			itr->second.sound.setBuffer(itr->second.buffer);
		}
	}
	for (auto itr = _music.begin(), enditr = _music.end(); itr != enditr; ++itr) {
		if (!itr->second.music.openFromFile(itr->second.path)) {
			_logger.error("Audio file \"{}\" for music object \"{}\" could not be "
				"loaded!", itr->second.path, itr->first);
			ret = false;
		} else {
			if (itr->second.loopTo < 0 && itr->second.loopWhen < 0) {
				itr->second.music.setLoop(false);
			} else if (itr->second.loopWhen < 0) {
				itr->second.music.setLoop(true);
				itr->second.music.setLoopPoints(sf::Music::TimeSpan(
					sf::milliseconds(itr->second.loopTo),
					itr->second.music.getDuration()
				));
			} else {
				itr->second.music.setLoop(true);
				itr->second.music.setLoopPoints(sf::Music::TimeSpan(
					sf::milliseconds(itr->second.loopTo),
					sf::milliseconds(itr->second.loopWhen - itr->second.loopTo)
				));
			}
		}
	}
	setVolume(getVolume());
	return ret;
}

void sfx::audio::_validateVolume(float& v) noexcept {
	if (v > 100.0f) {
		_logger.write("Volume value {} is too high: set to 100.0", v);
		v = 100.0f;
	}
	if (v < 0.0f) {
		_logger.write("Volume value {} is too low: set to 0.0", v);
		v = 0.0f;
	}
}

float sfx::audio::_volumeAfterOffset(const std::string& name) const {
	if (_sound.find(name) != _sound.end()) {
		if (getVolume() < 1.0) return 0.0;
		float vol = getVolume() + _sound.at(name).volumeOffset;
		if (vol < 1.0) vol = 1.0;
		if (vol > 100.0) vol = 100.0;
		return vol;
	} else if (_music.find(name) != _music.end()) {
		if (getVolume() < 1.0) return 0.0;
		float vol = getVolume() + _music.at(name).volumeOffset;
		if (vol < 1.0) vol = 1.0;
		if (vol > 100.0) vol = 100.0;
		return vol;
	}
	return 0.0;
}

sfx::audios::audios(const engine::logger::data& data) :
	resource_pool<sfx::audio>({ data.sink, "json_script" }, "audio object") {}

bool sfx::audios::_load(engine::json& j) {
	PoolType pool;
	bool ret = true;
	nlohmann::ordered_json jj = j.nlohmannJSON();
	for (auto& i : jj.items()) {
		pool.emplace(i.key(), std::make_shared<sfx::audio>(
			engine::logger::data{ _logger.getData().sink,
			_logger.getData().name + "_" + i.key() }));
		pool[i.key()]->load(i.value());
		if (!pool[i.key()]->inGoodState()) ret = false;
	}
	_pool = std::move(pool);
	return ret;
}

bool sfx::audios::_save(nlohmann::ordered_json& j) {
	for (const auto& pair : _pool) j[pair.first] = pair.second->getScriptPath();
	return true;
}
