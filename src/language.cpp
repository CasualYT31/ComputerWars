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

#include "language.h"

std::stringstream i18n::expand_string::_sstream = std::stringstream();
char i18n::expand_string::_varchar = '#';

i18n::expand_string::expand_string() noexcept {}

char i18n::expand_string::getVarChar() noexcept {
	return _varchar;
}

void i18n::expand_string::setVarChar(const char varchar) noexcept {
	_varchar = varchar;
}

// there is likely a cleaner way of implementing this for future reference
// I'm not sure why I didn't just grab the string contents directly using str()...
std::string i18n::expand_string::insert(const std::string& original) noexcept {
	_sstream << original;
	std::string finalString;
	std::getline(_sstream, finalString, '\0');
	_sstream.str(std::string());
	_sstream.clear();
	return finalString;
}

i18n::language_dictionary::language_dictionary(const std::string& name) noexcept : _logger(name) {}

bool i18n::language_dictionary::addLanguage(const std::string& id, const std::string& path) noexcept {
	if (id == _currentLanguage) {
		_logger.warning("Attempted to replace the script path of the current language \"{}\".", id);
		return false;
	} else if (id == "") {
		_logger.warning("Attempted to add a script path with a blank language ID.");
		return false;
	} else {
		_languageFiles[id] = path;
		return true;
	}
}

bool i18n::language_dictionary::removeLanguage(const std::string& id) noexcept {
	if (_languageFiles.find(id) == _languageFiles.end()) {
		_logger.warning("Attempted to remove non-existent language script path \"{}\".", id);
		return false;
	} else if (id == _currentLanguage) {
		_logger.warning("Attempted to remove current language script path \"{}\".", id);
		return false;
	} else {
		_languageFiles.erase(id);
		return true;
	}
}

bool i18n::language_dictionary::setLanguage(const std::string& id) noexcept {
	if (id != "" && _languageFiles.find(id) == _languageFiles.end()) {
		_logger.warning("Attempted to switch to non-existent string map \"{}\".", id);
		return false;
	} else {
		if (id == "") {
			_languageMap = nullptr;
			_currentLanguage = "";
			return true;
		}
		std::unique_ptr<i18n::language_dictionary::language> newMap = nullptr;
		try {
			newMap = std::make_unique<i18n::language_dictionary::language>("language_" + id);
		} catch (std::bad_alloc& e) {
			_logger.error("Failed to allocate memory for the string map of langauage \"{}\": {}", id, e.what());
			return false;
		}
		newMap->load(_languageFiles[id]);
		if (newMap->inGoodState()) {
			_languageMap = std::move(newMap);
			_currentLanguage = id;
			return true;
		} else {
			_logger.error("Failed to load string map script for language \"{}\".", id);
			return false;
		}
	}
}

std::string i18n::language_dictionary::getLanguage() const noexcept {
	return _currentLanguage;
}

bool i18n::language_dictionary::_load(safe::json& j) noexcept {
	std::string buffer = "";
	// firstly, load language scripts
	_languageFiles.clear();
	_currentLanguage = "";
	nlohmann::json jj = j.nlohmannJSON();
	for (auto& i : jj.items()) {
		if (i.key() != "lang" && i.key() != "") {
			j.apply(buffer, { i.key() });
			if (j.inGoodState()) {
				addLanguage(i.key(), buffer);
			} else {
				j.resetState();
			}
		}
	}
	// lastly, load current language
	j.apply(buffer, { "lang" }, &_currentLanguage);
	if (!j.inGoodState()) {
		return false;
	} else {
		return setLanguage(buffer);
	}
}

bool i18n::language_dictionary::_save(nlohmann::json& j) noexcept {
	j["lang"] = _currentLanguage;
	for (auto itr = _languageFiles.begin(), enditr = _languageFiles.end(); itr != enditr; itr++) {
		j[itr->first] = itr->second;
	}
	return true;
}

i18n::language_dictionary::language::language(const std::string& name) noexcept : _logger(name) {}

bool i18n::language_dictionary::language::_load(safe::json& j) noexcept {
	_strings.clear();
	nlohmann::json jj = j.nlohmannJSON();
	for (auto& i : jj.items()) {
		j.apply(_strings[i.key()], { i.key() });
		if (!j.inGoodState()) {
			_strings.erase(i.key());
			j.resetState();
		}
	}
	return true;
}

bool i18n::language_dictionary::language::_save(nlohmann::json& j) noexcept {
	for (auto itr = _strings.begin(), enditr = _strings.end(); itr != enditr; itr++) {
		j[itr->first] = itr->second;
	}
	return true;
}