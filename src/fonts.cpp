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

#include "fonts.h"

sfx::fonts::fonts(const std::string& name) noexcept : _logger(name) {}

sf::Font& sfx::fonts::operator[](const std::string& key) {
	if (_font.find(key) == _font.end()) {
		std::string msg = "Attempting to access font with key \"" + key + "\" which does not exist.";
		_logger.error(msg);
		throw std::invalid_argument(msg);
	}
	return _font.at(key);
}

bool sfx::fonts::_load(safe::json& j) noexcept {
	bool ret = true;
	nlohmann::json jj = j.nlohmannJSON();
	for (auto& i : jj.items()) {
		std::string path = i.value();
		if (!_font[i.key()].loadFromFile(path)) {
			_logger.error("Attempting to load font file \"{}\" which does not exist.", path);
			_font.erase(i.key());
			ret = false;
		}
		_fontpath[i.key()] = path;
	}
	return ret;
}

bool sfx::fonts::_save(nlohmann::json& j) noexcept {
	for (auto itr = _fontpath.begin(), enditr = _fontpath.end(); itr != enditr; itr++) {
		j[itr->first] = itr->second;
	}
	return true;
}