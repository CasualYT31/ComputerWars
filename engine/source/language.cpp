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

#include "language.hpp"

std::unique_ptr<std::stringstream> engine::expand_string::_sstream = nullptr;
char engine::expand_string::_varchar = '#';

char engine::expand_string::getVarChar() noexcept {
	return _varchar;
}

void engine::expand_string::setVarChar(const char varchar) noexcept {
	_varchar = varchar;
}

std::string engine::expand_string::insert(const std::string& original) {
	if (_sstream) {
		*_sstream << original;
		std::string finalString = _sstream->str();
		_sstream = nullptr;
		return finalString;
	} else {
		// No variables were inserted, so just return the original string.
		return original;
	}
}

engine::language_dictionary::language_dictionary(
	const engine::logger::data& data) :
	engine::json_script({data.sink, "json_script"}), _logger(data) {}

bool engine::language_dictionary::addLanguage(const std::string& id,
	const std::string& path) {
	if (id == _currentLanguage) {
		_logger.warning("Attempted to replace the script path of the current "
			"language \"{}\".", id);
		return false;
	} else if (id == "") {
		_logger.warning("Attempted to add a script path with a blank language "
			"ID.");
		return false;
	} else {
		_languageFiles[id] = path;
		return true;
	}
}

bool engine::language_dictionary::removeLanguage(const std::string& id) {
	if (_languageFiles.find(id) == _languageFiles.end()) {
		_logger.warning("Attempted to remove non-existent language script path "
			"\"{}\".", id);
		return false;
	} else if (id == _currentLanguage) {
		_logger.warning("Attempted to remove current language script path \"{}\".",
			id);
		return false;
	} else {
		_languageFiles.erase(id);
		return true;
	}
}

bool engine::language_dictionary::setLanguage(const std::string& id) {
	if (id != "" && _languageFiles.find(id) == _languageFiles.end()) {
		_logger.warning("Attempted to switch to non-existent string map \"{}\".",
			id);
		return false;
	} else {
		if (id == "") {
			_currentLanguage = "";
			_languageMap = nullptr;
			return true;
		}
		std::unique_ptr<engine::language_dictionary::language> newMap =
			std::make_unique<engine::language_dictionary::language>(
				engine::logger::data{ _logger.getData().sink, "language_" + id }
			);
		newMap->load(_languageFiles[id]);
		if (newMap->inGoodState()) {
			_currentLanguage = id;
			_languageMap.swap(newMap);
			return true;
		} else {
			_logger.error("Failed to load string map script for language \"{}\".",
				id);
			return false;
		}
	}
}

std::string engine::language_dictionary::getLanguage() const {
	return _currentLanguage;
}

bool engine::language_dictionary::_load(engine::json& j) {
	std::string buffer;
	// Firstly, load language scripts.
	_languageFiles.clear();
	_currentLanguage.clear();
	nlohmann::ordered_json jj = j.nlohmannJSON();
	for (auto& i : jj.items()) {
		if (!i.key().empty()) {
			j.apply(buffer, { i.key() });
			if (j.inGoodState()) {
				addLanguage(i.key(), buffer);
			} else {
				j.resetState();
			}
		}
	}
	// Lastly, set the current language to the first in the list.
	if (jj.begin() != jj.end()) {
		return setLanguage(jj.begin().key());
	} else {
		_logger.error("There were no languages defined in the JSON file!");
		return false;
	}
}

bool engine::language_dictionary::_save(nlohmann::ordered_json& j) {
	return false;
}

engine::language_dictionary::language::language(const engine::logger::data& data) :
	engine::json_script({ data.sink, "json_script" }), _logger(data) {}

bool engine::language_dictionary::language::_load(engine::json& j) {
	std::unordered_map<std::string, std::string> strings;
	nlohmann::ordered_json jj = j.nlohmannJSON();
	for (const auto& i : jj.items()) {
		j.apply(strings[i.key()], { i.key() });
		if (!j.inGoodState()) {
			strings.erase(i.key());
			j.resetState();
		}
	}
	_strings = std::move(strings);
	return true;
}

bool engine::language_dictionary::language::_save(nlohmann::ordered_json& j) {
	for (const auto& itr : _strings) j[itr.first] = itr.second;
	return true;
}

const char engine::language_dictionary::language::TRANSLATION_OVERRIDE = '~';
