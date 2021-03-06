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

#include "audio.h"

sfx::audio::audio(const std::string& name) noexcept : _logger(name) {}

float sfx::audio::getVolume() const noexcept {
	return _volume;
}

void sfx::audio::setVolume(float newvolume) noexcept {
	_validateVolume(newvolume);
	_volume = newvolume;
	for (auto itr = _sound.begin(), enditr = _sound.end(); itr != enditr; itr++) {
		itr->second.sound.setVolume(_volumeAfterOffset(itr->first, getVolume()));
	}
	for (auto itr = _music.begin(), enditr = _music.end(); itr != enditr; itr++) {
		itr->second.music.setVolume(_volumeAfterOffset(itr->first, getVolume()));
	}
}

void sfx::audio::play(std::string name) noexcept {
	if (name == "") name = getCurrentMusic();
	if (name == "") return;
	if (_sound.find(name) != _sound.end()) {
		// play the sound /*if not already playing*/
		/*if (_sound[name].sound.getStatus() != sf::Sound::Playing)*/
			_sound[name].sound.play();
	} else if (_music.find(name) != _music.end()) {
		// play the music if not already playing
		// also stop the currently playing music if there is one
		// if paused, resume
		if (_music[name].music.getStatus() == sf::Music::Paused) {
			_music[name].music.play();
		} else if (_music[name].music.getStatus() == sf::Music::Stopped) {
			if (getCurrentMusic() != "") stop();
			_music[name].music.play();
			_currentMusic = name;
		}
	} else {
		_logger.error("Audio object \"{}\" could not be found.", name);
	}
}

void sfx::audio::stop(const std::string& name) noexcept {
	if (_sound.find(name) != _sound.end()) {
		_sound[name].sound.stop();
	} else {
		if (getCurrentMusic() != "") {
			_music[getCurrentMusic()].music.stop();
			_currentMusic = "";
		}
	}
}

void sfx::audio::pause(const std::string& name) noexcept {
	if (_sound.find(name) != _sound.end()) {
		_sound[name].sound.pause();
	} else {
		if (getCurrentMusic() != "") _music[getCurrentMusic()].music.pause();
	}
}

bool sfx::audio::fadeout(sf::Time length) noexcept {
	if (getCurrentMusic() == "") return true;
	if (!_fadingOut) {
		_clock.restart();
		_fadingOut = true;
		_currentGranularity = _granularity;
		_currentVolume = _volume;
	}
	if (_clock.getElapsedTime().asSeconds() >=
		length.asSeconds() / _currentGranularity) {
		_music[getCurrentMusic()].music.setVolume(
			_music[getCurrentMusic()].music.getVolume() -
			_volumeAfterOffset(getCurrentMusic(), _currentVolume) /
			_currentGranularity
		);
		_clock.restart();
	}
	if (length.asMilliseconds() < 10 ||
		_music[getCurrentMusic()].music.getVolume() < 1.0) {
		std::string temp = getCurrentMusic();
		stop();
		_music[temp].music.setVolume(_volumeAfterOffset(temp, getVolume()));
		_fadingOut = false;
		return true;
	}
	return !_fadingOut;
}

float sfx::audio::getGranularity() const noexcept {
	return _granularity;
}

void sfx::audio::setGranularity(float newval) noexcept {
	_granularity = newval;
}

std::string sfx::audio::getCurrentMusic() const noexcept {
	return _currentMusic;
}

void sfx::audio::_validateVolume(float& v) noexcept {
	if (v > 100.0) {
		_logger.write("Volume value {} is too high: set to 100.0", v);
		v = 100.0;
	}
	if (v < 0.0) {
		_logger.write("Volume value {} is too low: set to 0.0", v);
		v = 0.0;
	}
}

float sfx::audio::_volumeAfterOffset(const std::string& name,
	const float baseVolume) const noexcept {
	if (_sound.find(name) != _sound.end()) {
		if (baseVolume < 1.0) return 0.0;
		float vol = baseVolume + _sound.at(name).volumeOffset;
		if (vol < 1.0) vol = 1.0;
		if (vol > 100.0) vol = 100.0;
		return vol;
	} else if (_music.find(name) != _music.end()) {
		if (baseVolume < 1.0) return 0.0;
		float vol = baseVolume + _music.at(name).volumeOffset;
		if (vol < 1.0) vol = 1.0;
		if (vol > 100.0) vol = 100.0;
		return vol;
	}
	return 0.0;
}

bool sfx::audio::_load(engine::json& j) noexcept {
	j.apply(_volume, { "volume" }, true);
	nlohmann::ordered_json jj = j.nlohmannJSON();
	_sound.clear();
	_music.clear();
	_currentMusic = "";
	_fadingOut = false;

	for (auto& i : jj.items()) {
		if (i.key() != "volume") {
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
				_logger.warning("Invalid type \"{}\" provided for audio object "
					"\"{}\" in script \"{}\", \"sound\" assumed.", type, i.key(),
					getScriptPath());
				type = "sound";
			}

			if (type == "sound") {
				_sound[i.key()].path = path;
				j.apply(_sound[i.key()].volumeOffset, { i.key(), "offset" }, true);
			} else if (type == "music") {
				_music[i.key()].path = path;
				j.apply(_music[i.key()].volumeOffset, { i.key(), "offset" }, true);
				j.apply(_music[i.key()].loopTo, { i.key(), "loopto" }, true);
				j.apply(_music[i.key()].loopWhen, { i.key(), "loopwhen" }, true);
			}
		}
	}

	return _loadAudio();
}

bool sfx::audio::_save(nlohmann::ordered_json& j) noexcept {
	j["volume"] = _volume;
	for (auto itr = _sound.begin(), enditr = _sound.end(); itr != enditr; itr++) {
		j[itr->first]["type"] = "sound";
		j[itr->first]["path"] = itr->second.path;
		j[itr->first]["offset"] = itr->second.volumeOffset;
	}
	for (auto itr = _music.begin(), enditr = _music.end(); itr != enditr; itr++) {
		j[itr->first]["type"] = "music";
		j[itr->first]["path"] = itr->second.path;
		j[itr->first]["offset"] = itr->second.volumeOffset;
		j[itr->first]["loopto"] = itr->second.loopTo;
		j[itr->first]["loopwhen"] = itr->second.loopWhen;
	}
	return true;
}

bool sfx::audio::_loadAudio() noexcept {
	bool ret = true;
	for (auto itr = _sound.begin(), enditr = _sound.end(); itr != enditr; itr++) {
		if (!itr->second.buffer.loadFromFile(itr->second.path)) {
			_logger.error("Audio file \"{}\" for sound object \"{}\" could not be "
				"loaded!", itr->second.path, itr->first);
			ret = false;
		} else {
			itr->second.sound.setBuffer(itr->second.buffer);
		}
	}
	for (auto itr = _music.begin(), enditr = _music.end(); itr != enditr; itr++) {
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