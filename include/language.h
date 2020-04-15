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
#include <sstream>

namespace i18n {
	class language : public safe::json_script {
	public:
		typedef std::unordered_map<std::string, std::string> Dictionary;

		static const char VAR_CHAR = '#';

		language(const std::string& name = "language") noexcept;

		template<typename... Ts>
		std::string operator()(const std::string& baseString, Ts... values) noexcept;

		void toNativeLanguage(bool newval) noexcept;
		bool inNativeLanguage() const noexcept;
	private:
		virtual bool _load(safe::json& j) noexcept;
		virtual bool _save(nlohmann::json& j) noexcept;

		std::string _calculateString(const std::string& baseString) noexcept;

		template<typename T, typename... Ts>
		void _dumpToStream(const std::string& transString, T value, Ts... values) noexcept;
		void _dumpToStream(const std::string& transString) noexcept;

		Dictionary _dict;
		bool _baseLanguage = true;
		std::stringstream _sstr;
		global::logger _logger;
	};

	class translation {
	public:
		typedef std::unordered_map<std::string, i18n::language*> LanguageObjectList;

		static void addLanguageObject(const std::string& name, i18n::language* obj) noexcept;
		static i18n::language* getLanguageObject(const std::string& name) noexcept;
		static void removeLanguageObject(const std::string& name) noexcept;

		static void setLanguage(const std::string& lang) noexcept;
		static std::string getLanguage() noexcept;

		static void setLanguageScriptPath(const std::string& path) noexcept;
		static std::string getLanguageScriptPath() noexcept;

		static bool inGoodState() noexcept;
	protected:
		translation() noexcept;
	private:
		static LanguageObjectList _langs;
		static std::string _curlang;
		static std::string _path;
	};
}

template<typename... Ts>
std::string i18n::language::operator()(const std::string& baseString, Ts... values) noexcept {
	if (sizeof...(Ts)) {
		_sstr.str(std::string()); _sstr.clear();
		_dumpToStream(_calculateString(baseString), values...);
		std::string finalString;
		std::getline(_sstr, finalString, '\0');
		return finalString;
	} else {
		return _calculateString(baseString);
	}
}

template<typename T, typename... Ts>
void i18n::language::_dumpToStream(const std::string& transString, T value, Ts... values) noexcept {
	for (std::size_t c = 0; c < transString.length(); c++) {
		if (transString[c] == VAR_CHAR) {
			if (c < transString.length() - 1 && transString[c + 1] == VAR_CHAR) {
				//if two VAR_CHARs appear in succession
				//then one is printed, and the next one is ignored
				_sstr << VAR_CHAR; //print...
				c++; //ignore...
			} else { //a variable needs to be inserted
				_sstr << value;
				return _dumpToStream(transString.substr(c + 1), values...);
			}
		} else {
			_sstr << transString[c];
		}
	}
}