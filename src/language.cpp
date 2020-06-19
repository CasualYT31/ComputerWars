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


// CONSTRUCTION ZONE

std::stringstream i18n::expand_string::_sstream = std::stringstream();
char i18n::expand_string::_varchar = '#';

i18n::expand_string::expand_string() noexcept {}

char i18n::expand_string::getVarChar() noexcept {
	return _varchar;
}

void i18n::expand_string::setVarChar(const char varchar) noexcept {
	_varchar = varchar;
}

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
	i18n::language_dictionary::language newStringMap("language_" + id);
	newStringMap.load(path);
	if (newStringMap.inGoodState()) {
		_dictionary[id] = newStringMap;
	} else {
		_logger.error("Failed to add new language object \"{}\".", id);
	}
	return newStringMap.inGoodState();
}

bool i18n::language_dictionary::removeLanguage(const std::string& id) noexcept {
	if (_dictionary.find(id) == _dictionary.end()) {
		_logger.write("Attempted to remove non-existent language object \"{}\".", id);
		return false;
	} else if (id == _currentLanguage) {
		_logger.write("Attempted to remove current language object \"{}\".", id);
		return false;
	} else {
		_dictionary.erase(id);
		return true;
	}
}

bool i18n::language_dictionary::setLanguage(const std::string& id) noexcept {
	if (id != "" && _dictionary.find(id) == _dictionary.end()) {
		_logger.write("Attempted to switch to non-existent language object \"{}\".", id);
		return false;
	} else {
		_currentLanguage = id;
		return true;
	}
}

std::string i18n::language_dictionary::getLanguage() const noexcept {
	return _currentLanguage;
}

bool i18n::language_dictionary::_load(safe::json& j) noexcept {
	std::string curlang = _currentLanguage;
	j.apply(curlang, { "lang" });
	if (!j.inGoodState()) {
		return false;
	} else {
		return setLanguage(curlang);
	}
}

bool i18n::language_dictionary::_save(nlohmann::json& j) noexcept {
	j["lang"] = _currentLanguage;
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

// END OF CONSTRUCTION ZONE





i18n::language::language(const std::string& name) noexcept : _logger(name) {}

void i18n::language::toNativeLanguage(bool newval) noexcept {
	_baseLanguage = newval;
}

bool i18n::language::inNativeLanguage() const noexcept {
	return _baseLanguage;
}

void i18n::language::_dumpToStream(const std::string& transString) noexcept {
	_sstr << transString;
}

std::string i18n::language::_calculateString(const std::string& baseString) noexcept {
	if (_baseLanguage) {
		return baseString;
	} else {
		std::string ret;
		try {
			ret = _dict.at(baseString);
		}
		catch (std::exception & e) {
			ret = e.what();
			_logger.error("Could not find foreign language string with the native langauge string \"{}\", in script {}", baseString, getScriptPath());
		}
		return ret;
	}
}

bool i18n::language::_load(safe::json& j) noexcept {
	i18n::language::Dictionary newmap;
	nlohmann::json jj = j.nlohmannJSON();

	for (auto& i : jj.items()) {
		j.apply<std::string>(newmap[i.key()], { i.key() });

		if (!j.inGoodState()) {
			std::string value;
			_sstr << i.value();
			std::getline(_sstr, value);
			_sstr.str(std::string()); _sstr.clear();

			_logger.error("State of the safe::json object became unhealthy in a language object: \"{}\": {}. Value is not a string, in script {}", i.key(), value, getScriptPath());
			return false;
		}
	}

	_dict.clear();
	_dict = newmap;
	return true;
}

bool i18n::language::_save(nlohmann::json& j) noexcept {
	for (i18n::language::Dictionary::iterator itr = _dict.begin(), enditr = _dict.end(); itr != enditr; itr++) {
		j[itr->first] = itr->second;
	}
	return true;
}

i18n::translation::LanguageObjectList i18n::translation::_langs = i18n::translation::LanguageObjectList();
std::string i18n::translation::_curlang = "";
std::string i18n::translation::_path = ".";

i18n::translation::translation() noexcept {}

void i18n::translation::addLanguageObject(const std::string& name, i18n::language* obj) noexcept {
	if (obj) _langs[name] = obj;
}

void i18n::translation::removeLanguageObject(const std::string& name) noexcept {
	_langs.erase(name);
}

void i18n::translation::setLanguage(const std::string& lang) noexcept {
	if (lang != getLanguage()) {
		for (auto itr = _langs.begin(), enditr = _langs.end(); itr != enditr; itr++) {
			if (lang == "") {
				itr->second->toNativeLanguage(true);
			} else {
				itr->second->load(getLanguageScriptPath() + "/" + lang + "_" + itr->first + ".json");
				if (itr->second->inGoodState()) itr->second->toNativeLanguage(false);
			}
		}
		if (inGoodState()) _curlang = lang;
	}
}

void i18n::translation::setLanguageScriptPath(const std::string& path) noexcept {
	_path = path;
}

std::string i18n::translation::getLanguageScriptPath() noexcept {
	return _path;
}

std::string i18n::translation::getLanguage() noexcept {
	return _curlang;
}

bool i18n::translation::inGoodState() noexcept {
	bool good = true;
	for (auto itr = _langs.begin(), enditr = _langs.end(); itr != enditr; itr++) {
		if (itr->second) good &= itr->second->inGoodState();
		if (!good) break;
	}
	return good;
}

i18n::language* i18n::translation::getLanguageObject(const std::string& name) noexcept {
	if (_langs.find(name) == _langs.end()) {
		return nullptr;
	} else {
		return _langs[name];
	}
}