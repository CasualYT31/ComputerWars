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
	/**
	 * Represents a map of string pairs which hold native language strings and their corresponding translations.
	 * This class was designed to simplify the localisation effort for software projects in terms of language translation.
	 * Programmers can create \c language objects which accept a JSON script, written by translators, containing string key-value pairs.
	 * The key holds the "native language" string, the value holds the equivolent "foreign language" string.
	 * The programmer then replaces native language strings in the code with languageObjectName("native string").
	 * This allows the client to switch out native or foreign language strings by simply calling \c toNativeLanguage().
	 * In order to cohesively support multiple foreign languages, the \c translation class should be used.
	 * @sa translation
	 */
	class language : public safe::json_script {
	public:
		/**
		 * Typedef represening a map of string pairs.
		 * The key represents the native language string, the value represents the foreign language string.
		 * @todo Perhaps this should be made into its own class.
		 */
		typedef std::unordered_map<std::string, std::string> Dictionary;

		/**
		 * Character which represents a variable to insert into the string.
		 * Whenever a variable needs to be inserted into a native or foreign language string, this character should be included.
		 * If the character needs to be output instead of a variable, then two of these characters, one directly after the other, should be typed.
		 * @todo This should be taken out of this class and made into its own: see \c _dumpToStream().
		 */
		static const char VAR_CHAR = '#';

		/**
		 * Constructor which initialises the internal logger object.
		 * @param name The name to given this \c language object in the log file.
		 * @sa    logger.logger()
		 */
		language(const std::string& name = "language") noexcept;

		/**
		 * Translates a string.
		 * This operator accepts a string and an optional list of variables to insert into the string.
		 * The string given must be the native language string.
		 * <tt>VAR_CHAR</tt>s are read from left to right, and are replaced with any given variables, starting with the one given first, then proceeding.
		 * If more variables were given than <tt>VAR_CHAR</tt>s, the extra variables are ignored.
		 * If more <tt>VAR_CHAR</tt>s were given than variables, the extra <tt>VAR_CHAR</tt>s are preserved in the resulting string.
		 * @tparam Ts         The types of the objects within the parameter pack \c values.
		 * @param  baseString The native language string to process.
		 * @param  values     The variables to optionally insert into the native language string. Can be none.
		 * @return The string after translation and variable processing.
		 * @sa     VAR_CHAR
		 * @sa     _dumpToStream()
		 */
		template<typename... Ts>
		std::string operator()(const std::string& baseString, Ts... values) noexcept;

		/**
		 * Switches the language mode of this object.
		 * This method is used to switch between returning he native or foreign language string when the operator()() method is called.
		 * @param newval \c TRUE if the native language string should be returned, \c FALSE if the foreign language string should be returned.
		 */
		void toNativeLanguage(bool newval) noexcept;

		/**
		 * Tests if the object is in native language mode.
		 * @return \c TRUE if the object is in native language mode, \c FALSE if it's in the foreign language mode.
		 */
		bool inNativeLanguage() const noexcept;
	private:
		/**
		 * Configures the class with a JSON script.
		 * The root object must hold a set of string key-value pairs.
		 * The key must hold the native language string: this is the string that is used in the program code.
		 * The value must hold the foreign language equivalent of the native language string.
		 * If any value is a non-string, an error will be logged and \c FALSE will be returned: the \c language object can be used but not all strings are guaranteed to be loaded.
		 * The internal Dictionary will only be updated if loading was completely successful. The internal Dictionary will be wiped before being updated.
		 * @param  j The \c safe::json object which contains the contents of the loaded script.
		 * @return \c TRUE upon success, \c FALSE if loading failed.
		 * @sa     VAR_CHAR
		 * @sa     json._load()
		 * @sa     _save()
		 */
		virtual bool _load(safe::json& j) noexcept;

		/**
		 * Exports the class' data into a JSON script.
		 * See _load() for a description of the resulting format of the JSON script.
		 * @param  j The object to write this object's data to.
		 * @return \c TRUE.
		 * @sa     json._save()
		 * @sa     _load()
		 */
		virtual bool _save(nlohmann::json& j) noexcept;

		/**
		 * Determines which string should be returned via the operator()() method.
		 * If in native language mode, the method will simply return the string it is given.
		 * If in foreign language mode, the corresponding foreign language string will be returned using the native language string given.
		 * If it could be found, it is returned. If it could not be found, an error string is returned and it is logged.
		 * @param  baseString The native language string to translate.
		 * @return Either the native or foreign language string depending on the language mode at the time of calling, or an error string if an error occurred.
		 * @remark Yes, the string is technically duplicated via this method if in native language mode.
		 *         However, assuming that the result is used for output only, this copy only exists temporarily.
		 */
		std::string _calculateString(const std::string& baseString) noexcept;

		/**
		 * Parses a string and inserts variables into it.
		 * This method is called if at least one variable is given in a call to the operator()() method.
		 * Characters are individually checked. If one of them is a lone \c VAR_CHAR, \c value is inserted into the string, and a recursive call is performed.
		 * Since the \c value object is inserted via a \c stringstream object, it must be able to be output to such an object using the \c operator<<() method on a \c stringstream.
		 * @todo   This method should be made into its own separate function. There may well be use for this elsewhere.
		 * @tparam T  The type of the first object in the parameter pack.
		 * @tparam Ts The types of the remaining objects in the parameter pack, if any exist.
		 * @param  transString The string to process: can be either native or foreign.
		 * @param  value       The variable to insert into \c transString if a \c VAR_CHAR is found.
		 * @param  values      The remaining variables to insert into \c transString. Can be zero.
		 * @sa     _dumpToStream()
		 */
		template<typename T, typename... Ts>
		void _dumpToStream(const std::string& transString, T value, Ts... values) noexcept;

		/**
		 * Writes the given string to an internal string stream object.
		 * This method is called if no variables are given in a call to the operator()() method.
		 * This method is also automatically called if all given variables have been inserted.
		 * @param transString The string to process: can be either native or foreign.
		 * @sa    _dumpToString<T,Ts>()
		 */
		void _dumpToStream(const std::string& transString) noexcept;

		/**
		 * The string pair map holding native language strings and their corresponding foreign language strings.
		 */
		Dictionary _dict;

		/**
		 * Tracks the language mode of the object.
		 * If \c TRUE, the object is in native language mode, if \c FALSE, the object is in foreign language mode.
		 */
		bool _baseLanguage = true;

		/**
		 * The string stream used for string processing internally.
		 * @sa _dumpToStream()
		 */
		std::stringstream _sstr;

		/**
		 * The logger object.
		 */
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