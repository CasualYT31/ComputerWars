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
#include "sfml/Audio.hpp"

namespace sfx {
	struct sound {
		sf::SoundBuffer buffer;
		sf::Sound sound;
		std::string path;
		float volumeOffset = 0.0;
	};

	struct music {
		sf::Music music;
		std::string path;
		sf::Int32 loopTo = 0;
		sf::Int32 loopWhen = -1;
		float volumeOffset = 0.0;
	};

	class audio : public safe::json_script {
	public:
		audio(const std::string& name = "audio") noexcept;

		float getMusicVolume() const noexcept;
		float setMusicVolume(float newvolume) noexcept;
		float getSoundVolume() const noexcept;
		float setSoundVolume(float newvolume) noexcept;

		void play(std::string name = "") noexcept;
		void stop(const std::string& name = "") noexcept;
		void pause(const std::string& name = "") noexcept;

		bool fadeout(sf::Time length = sf::seconds(1.0)) noexcept;
		float getGranularity() const noexcept;
		float setGranularity(float newval) noexcept;

		std::string getCurrentMusic() const noexcept;
	private:
		virtual bool _load(safe::json& j) noexcept;
		virtual bool _save(nlohmann::json& j) noexcept;
		bool _loadAudio() noexcept;
		void _validateVolume(float& v) noexcept;
		float _volumeAfterOffset(const std::string& name) const noexcept;

		global::logger _logger;
		std::unordered_map<std::string, sfx::sound> _sound;
		std::unordered_map<std::string, sfx::music> _music;
		std::string _currentMusic = "";
		float _soundVolume = 50.0;
		float _musicVolume = 50.0;

		bool _fadingOut = false;
		sf::Clock _clock;
		float _granularity = 100.0;
	};
}