/*Copyright 2019-2022 CasualYouTuber31 <naysar@protonmail.com>

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

#include "fonts.hpp"

sfx::fonts::fonts(const std::string& name) noexcept : _logger(name) {}

std::shared_ptr<sf::Font> sfx::fonts::operator[](const std::string& key) const
	noexcept {
 	if (_font.find(key) == _font.end()) {
		_logger.error("Attempting to access font with key \"{}\" which does not "
			"exist.", key);
		return nullptr;
	}
	return _font.at(key);
}

std::string sfx::fonts::getFontPath(const std::string& key) const noexcept {
	if (_fontpath.find(key) == _fontpath.end()) {
		_logger.error("Attempting to access font file path with key \"{}\" which "
			"does not exist.", key);
		return "";
	}
	return _fontpath.at(key);
}

bool sfx::fonts::_load(engine::json& j) noexcept {
	_font.clear();
	_fontpath.clear();
	bool ret = true;
	nlohmann::ordered_json jj = j.nlohmannJSON();
	for (auto& i : jj.items()) {
		std::string path = i.value();
		_font[i.key()] = std::make_shared<sf::Font>();
		if (!_font[i.key()]->loadFromFile(path)) {
			_logger.error("Attempting to load font file \"{}\" which does not "
				"exist.", path);
			_font.erase(i.key());
			ret = false;
		}
		_fontpath[i.key()] = path;
	}
	return ret;
}

bool sfx::fonts::_save(nlohmann::ordered_json& j) noexcept {
	for (auto& path : _fontpath) {
		j[path.first] = path.second;
	}
	return true;
}
