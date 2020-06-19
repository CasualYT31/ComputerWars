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

/**@file language.h
 * Classes used for string translation.
 * This file declares two classes, \c language and \c translation.
 * \c language stores a map of string pairs which can be accessed.
 * The string retrieved is dependant on if the \c language object is configured to retrieve the \e native or \e foreign language string.
 * The \c translation singleton class is used to manipulate multiple \c language objects simulataneously.
 * @todo I wonder if a better architecture could be figured out here: it's best to avoid singleton classes wherever possible. Look into creation design patterns.
 *       I've got the basic building blocks in place: I just have to make it so that the client uses \e one class instead of \e two somehow.
 */

#pragma once

#include "safejson.h"
#include <sstream>

namespace i18n {
	class expand_string {
	public:
		template<typename T, typename... Ts>
		static std::string insert(const std::string& original, T value, Ts... values) noexcept;
		static std::string insert(const std::string& original) noexcept;
		static char getVarChar() noexcept;
		static void setVarChar(const char varchar) noexcept;
	protected:
		expand_string() noexcept;
	private:
		static std::stringstream _sstream;
		static char _varchar;
	};

	class language_dictionary : public safe::json_script {
	public:
		language_dictionary(const std::string& name = "dictionary") noexcept;
		bool addLanguage(const std::string& id, const std::string& path) noexcept;
		bool removeLanguage(const std::string& id) noexcept;
		bool setLanguage(const std::string& id) noexcept;
		std::string getLanguage() const noexcept;
		template<typename... Ts>
		std::string operator()(const std::string& nativeString, Ts... values) noexcept;
	private:
		virtual bool _load(safe::json& j) noexcept;
		virtual bool _save(nlohmann::json& j) noexcept;
		class language : public safe::json_script {
		public:
			language(const std::string& name = "language") noexcept;
			template<typename... Ts>
			std::string get(const std::string& nativeString, Ts... values) noexcept;
		private:
			virtual bool _load(safe::json& j) noexcept;
			virtual bool _save(nlohmann::json& j) noexcept;
			global::logger _logger;
			std::unordered_map<std::string, std::string> _strings;
		};
		std::unordered_map<std::string, i18n::language_dictionary::language> _dictionary;
		std::string _currentLanguage = "";
		global::logger _logger;
	};
}

template<typename T, typename... Ts>
std::string i18n::expand_string::insert(const std::string& original, T value, Ts... values) noexcept {
	for (std::size_t c = 0; c < original.length(); c++) {
		if (original[c] == _varchar) {
			if (c < original.length() - 1 && original[c + 1] == _varchar) {
				// if two varchars appear in succession,
				// then one is printed, and the next one is ignored
				_sstream << _varchar; // print...
				c++; // ignore...
			} else {
				_sstream << value;
				return insert(original.substr(c + 1), values...);
			}
		} else {
			_sstream << original[c];
		}
	}
	// execution enters this point if more variables were given than varchars
	// in which case, we've reached the end of the original string, so retrieve the results and return them
	return insert("");
}

template<typename... Ts>
std::string i18n::language_dictionary::language::get(const std::string& nativeString, Ts... values) noexcept {
	if (_strings.find(nativeString) == _strings.end()) {
		_logger.error("Native string \"{}\" does not exist in this string map.", nativeString);
		return "<error>";
	} else {
		return i18n::expand_string::insert(_strings.at(nativeString), values...);
	}
}

template<typename... Ts>
std::string i18n::language_dictionary::operator()(const std::string& nativeString, Ts... values) noexcept {
	if (_currentLanguage == "") return nativeString;
	return _dictionary.at(_currentLanguage).get(nativeString, values...);
}