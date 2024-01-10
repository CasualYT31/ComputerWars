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

#include "fonts.hpp"

sfx::fonts::fonts(const engine::logger::data& data) :
	resource_pool<sf::Font>({data.sink, "json_script"}, "font") {}

std::string sfx::fonts::getFontPath(const std::string& key) const {
	if (_fontpath.find(key) == _fontpath.end()) {
		_logger.error("Attempting to access {} file path with key \"{}\" which "
			"does not exist.", _objectType, key);
		return "";
	}
	return _fontpath.at(key);
}

bool sfx::fonts::_load(engine::json& j) {
	PoolType fonts;
	std::unordered_map<std::string, std::string> fontpaths;
	bool ret = true;
	nlohmann::ordered_json jj = j.nlohmannJSON();
	for (auto& i : jj.items()) {
		std::string path = i.value();
		fonts[i.key()] = std::make_shared<sf::Font>();
		if (!fonts[i.key()]->loadFromFile(path)) {
			_logger.error("Could not load {} file \"{}\".", _objectType, path);
			fonts.erase(i.key());
			ret = false;
		}
		fontpaths[i.key()] = path;
	}
	_pool = std::move(fonts);
	_fontpath = std::move(fontpaths);
	return ret;
}

bool sfx::fonts::_save(nlohmann::ordered_json& j) {
	for (const auto& path : _fontpath) j[path.first] = path.second;
	return true;
}
