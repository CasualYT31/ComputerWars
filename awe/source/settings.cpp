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

#include "settings.hpp"

awe::settings::settings(const engine::logger::data& data,
	const std::shared_ptr<sfx::audios>& audios,
	const std::shared_ptr<engine::language_dictionary>& dictionary) :
	json_script({ data.sink, "json_script" }), _logger(data), _audios(audios),
	_dictionary(dictionary) {
	assert(_audios && _dictionary);
}

void awe::settings::setAnimationPreset(const awe::animation_preset p) noexcept {
	_animationPreset = p;
}

awe::animation_preset awe::settings::getAnimationPreset() const noexcept {
	return _animationPreset;
}

bool awe::settings::_load(engine::json& j) {
	for (auto& audio : *_audios) {
		float volume = 100.f;
		j.apply(volume, { "volumes", audio.first });
		if (!j.inGoodState()) {
			_logger.warning("Could not find volume for audio object \"{}\", "
				"assigning default volume of {}.", audio.first, volume);
			j.resetState();
		}
		audio.second->setVolume(volume);
	}
	std::string language;
	j.apply(language, { "language" });
	if (!j.inGoodState() || language.empty()) {
		_logger.warning("Could not find selected language, leaving to default "
			"\"{}\".{}", _dictionary->getLanguage(), (j.inGoodState() ?
				" You cannot have an empty language setting." : ""));
		j.resetState();
	} else if (!_dictionary->setLanguage(language)) {
		_logger.warning("Could not set the current language to \"{}\", leaving to "
			"default \"{}\".", language, _dictionary->getLanguage());
	}
	unsigned int preset = static_cast<unsigned int>(_animationPreset);
	j.apply(preset, { "preset" }, true);
	if (preset < static_cast<unsigned int>(awe::animation_preset::Count))
		_animationPreset = static_cast<awe::animation_preset>(preset);
	else
		_logger.warning("Stored preset value {} is higher than the number of "
			"presets available, {}.", preset,
			static_cast<unsigned int>(awe::animation_preset::Count));
	return true;
}

bool awe::settings::_save(nlohmann::ordered_json& j) {
	for (const auto& audio : *_audios)
		j["volumes"][audio.first] = audio.second->getVolume();
	j["language"] = _dictionary->getLanguage();
	j["preset"] = static_cast<unsigned int>(_animationPreset);
	return true;
}
