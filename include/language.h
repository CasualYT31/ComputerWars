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
 * Classes used for string translation and manipulation.
 * This file declares two classes, \c expand_string and \c language_dictionary.
 * \c language_dictionary stores a map of maps of string pairs which can be accessed based on two keys: a language ID and a string ID.
 * \c expand_string is a small suite of functions used to insert variables into strings.
 */

#pragma once

#include "safejson.h"
#include <sstream>

/**
 * The \c i18n namespace contains internationalisation-based classes.
 */
namespace i18n {
	/**
	 * This class contains a small suite of functions used to insert variables into strings.
	 * Each function is static, as well as all the data members. This class cannot be instantiated.
	 * A class approach was chosen since there needed to be shared, private data between the two versions of insert().
	 * @warning This class is *not* thread safe! Don't attempt to change the var char during a call to insert().
	 */
	class expand_string {
	public:
		/**
		 * Inserts variables into a given string.
		 * This version of the insert() function scans through every character of a given string in search of <em>var chars</em>.
		 * When a single var char is found within a string, a variable given in the call to insert() is inserted instead of the var char,
		 * and a recursive call is performed. If there are no more variables left, then the second version of insert() is called.
		 * If two var chars are found next to each other, then one var char is inserted into the final string and the other is discarded.
		 * Once this is done, character scanning continues to the character after the last of the two var chars: this means that, if three
		 * var chars are given in a row, then the first two will be replaced with one var char, and the last var char will be replaced by
		 * a variable. If more variables are given than var chars in the string, surplus variables will be ignored.
		 * If less variables are given than var chars in the string, surplus var chars will be treated as normal characters by the second
		 * version of insert().
		 * Since an STL string stream is used internally as an output, most standard variable types will work (numeric, string, chars, etc.).
		 * @tparam T        The type of the first variable given.
		 * @tparam Ts       The types of subsequent variables, if any are given.
		 * @param  original The string to scan.
		 * @param  value    The variable to insert in this call.
		 * @param  values   Subsequent variables in the queue.
		 * @return The final string, with variables inserted.
		 * @sa     insert()
		 * @sa     setVarChar()
		 */
		template<typename T, typename... Ts>
		static std::string insert(const std::string& original, T value, Ts... values) noexcept;

		/**
		 * Dumps the given string to the internal stream and returns the final string.
		 * This version of insert() is the one called when no variables are given for insertion. This is usually the final call in the
		 * recursive chain, but it can also be the only call in the insert() chain. The given string is dumped to the string stream,
		 * and then the entire stream is output to a string object, which is then returned. This function also resets the string stream,
		 * ready for another call to insert().
		 * All characters of the given string are treated the same: the entire string is simply dumped to the stream.
		 * @param  original The string to dump to the internal stream: usually the tail end of a larger string.
		 * @return The final string as stored by the stream.
		 * @sa     insert<T,Ts>()
		 */
		static std::string insert(const std::string& original) noexcept;

		/**
		 * Retrieves the var char.
		 * @return The var char, defaults to '#'.
		 * @sa     setVarChar()
		 */
		static char getVarChar() noexcept;

		/**
		 * Updates the var char.
		 * @param varchar The new var char.
		 * @sa    getVarChar()
		 */
		static void setVarChar(const char varchar) noexcept;
	protected:
		/**
		 * This class cannot be instantiated by the client.
		 */
		expand_string() noexcept;
	private:
		/**
		 * The string stream which is used to piece together the final string.
		 * @sa insert<T,Ts>()
		 */
		static std::stringstream _sstream;

		/**
		 * Stores the var char.
		 */
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