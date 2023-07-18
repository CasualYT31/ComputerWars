/*Copyright 2019-2023 CasualYouTuber31 <naysar@protonmail.com>

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

/**@file language.hpp
 * Classes used for string translation and manipulation.
 * This file declares two classes, \c expand_string and \c language_dictionary.
 * \c language_dictionary stores a map of maps of string pairs which can be
 * accessed based on two keys: a language ID and a string ID. \c expand_string is a
 * small suite of functions used to insert variables into strings.
 */

#pragma once

#include "safejson.hpp"
#include <sstream>
#include <memory>

namespace engine {
	/**
	 * This class contains a small suite of functions used to insert variables into
	 * strings.
	 * Each function is static, as well as all the data members. This class cannot
	 * be instantiated. A class approach was chosen since there needed to be
	 * shared, private data between the two versions of \c insert().
	 * @warning This class is \b not thread safe! Don't attempt to change the var
	 *          char during a call to \c insert().
	 */
	class expand_string {
	public:
		/**
		 * Inserts variables into a given string.
		 * This version of the \c insert() method scans through every character of
		 * a given string in search of <em>var chars</em>. When a single var char
		 * is found within a string, a variable given in the call to \c insert() is
		 * inserted to replace the var char, and a recursive call is performed. If
		 * there are no more variables left, then the second version of \c insert()
		 * is called.\n
		 * If two var chars are found next to each other, then one var char is
		 * inserted into the final string and the other is discarded. Once this is
		 * carried out, character scanning continues to the character after the
		 * last of the two var chars: this means that, if three var chars are given
		 * in a row, then the first two will be replaced with one var char, and the
		 * last var char will be replaced by a variable.\n
		 * If more variables are given than var chars in the string, surplus
		 * variables will be ignored. If less variables are given than var chars in
		 * the string, surplus var chars will be treated as normal characters by
		 * the second version of \c insert().\n
		 * Since an STL string stream is used internally as an output, most
		 * standard variable types will work (numeric, string, chars, etc.).
		 * @tparam T        The type of the first variable given.
		 * @tparam Ts       The types of subsequent variables, if any are given.
		 * @param  original The string to scan.
		 * @param  value    The variable to insert in this call.
		 * @param  values   Subsequent variables in the queue.
		 * @return The final string, with variables inserted.
		 * @safety Strong guarantee: if an exception is thrown, internal state will
		 *         be reset and future calls won't be affected.
		 * @sa     insert()
		 * @sa     setVarChar()
		 */
		template<typename T, typename... Ts>
		static std::string insert(const std::string& original, T value,
			Ts... values);

		/**
		 * Dumps the given string to the internal stream and returns the final
		 * string.
		 * This version of \c insert() is the one called when no variables are
		 * given for insertion. This is usually the final call in the recursive
		 * chain, but it can also be the only call in the \c insert() chain. The
		 * given string is dumped to the string stream, and then the entire stream
		 * is output to a string object, which is then returned. This function also
		 * resets the string stream, ready for another call to \c insert(). All
		 * characters of the given string are treated the same: the entire string
		 * is simply dumped to the stream.
		 * @param  original The string to dump to the internal stream: usually the
		 *                  tail end of a larger string.
		 * @return The final string as stored by the stream.
		 * @safety Strong guarantee: if an exception is thrown, internal state will
		 *         be reset and future calls won't be affected.
		 * @sa     insert<T,Ts>()
		 */
		static std::string insert(const std::string& original);

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
		expand_string() noexcept = default;
	private:
		/**
		 * The string stream which is used to piece together the final string.
		 * @sa insert<T,Ts>()
		 */
		static std::unique_ptr<std::stringstream> _sstream;

		/**
		 * Stores the var char.
		 */
		static char _varchar;
	};

	/**
	 * Class used to translate strings during runtime.
	 * This class works by loading a variety of JSON scripts. The first contains a
	 * list of JSON scripts and their language IDs, forming the language map. Each
	 * of these scripts contains a list of string pairs, with the keys IDing
	 * strings of a particular language, such as English or German. This forms the
	 * string map. The same key should be given to equivalent strings so that they
	 * can be accessed in the same way by the client.
	 * @sa _load()
	 */
	class language_dictionary : public engine::json_script {
	public:
		/**
		 * Initialises the internal logger object.
		 * @param data The data to initialise the logger object with.
		 * @sa    \c engine::logger
		 */
		language_dictionary(const engine::logger::data& data) noexcept;

		/**
		 * Adds a path to a language's string map script.
		 * The language is only added if the given ID was non-blank and was not the
		 * ID of the current language. If the ID of an existing language was given,
		 * the old path is replaced with the new path.\n
		 * Please see \c engine::language_dictionary::language::_load() for a rundown
		 * of the format this JSON script is to have.\n
		 * @remark It is advised to instead load all language script paths at once
		 *         using the \c load() function inherited from
		 *         \c engine::json_script.
		 * @param  id   The ID to give to this language. It should ideally take the
		 *              format "ENG_US", "DE_DEU", etc.
		 * @param  path The path of the JSON script to load.
		 * @return \c TRUE if adding the language went successfully,
		 *         \c FALSE otherwise (if \c id was blank, for example). If
		 *         \c FALSE is returned, a warning will be logged.
		 */
		bool addLanguage(const std::string& id, const std::string& path) noexcept;

		/**
		 * Removes a language path from the internal collection.
		 * If the given ID could not identify a language script path at the time of
		 * calling, or if it was the ID of the current language, then the method
		 * will fail and warnings will be logged.
		 * @param  id The ID of the language to remove.
		 * @return \c TRUE if removal was successful, \c FALSE otherwise.
		 */
		bool removeLanguage(const std::string& id) noexcept;

		/**
		 * Updates the current language.
		 * This class has been setup to only access the string map of one language
		 * at a time. This method is used to change the language to access, dubbed
		 * "the current language." If set to an empty string, the \c operator()
		 * method will instead return the given string with variables inserted. The
		 * name of the freshly allocated \c language object given in the logger
		 * will be "language_ID".
		 * @param  id The ID of the language to set.
		 * @return \c TRUE if switching was successful, \c FALSE if the given ID
		 *         could not identify a loaded language, or if loading the script
		 *         failed.
		 * @sa     operator()
		 * @sa     getLanguage()
		 */
		bool setLanguage(const std::string& id) noexcept;

		/**
		 * Retrieves the ID of the current language.
		 * @return The ID of the current language.
		 * @sa     setLanguage()
		 */
		std::string getLanguage() const noexcept;

		/**
		 * Retrieves a string from the current language's string map.
		 * This method looks up the current language's string map and searches for
		 * a string with the given key. This key is also known as the "native
		 * string." It uses the \c expand_string::insert() function to insert
		 * variables into the string found in the string map. Please see the
		 * \c engine::language_dictionary::language::get() method for more
		 * information.\n
		 * If the string map of the current language is \c NULL, and yet this
		 * method still attempts to access it, a fatal error will be logged and
		 * "<fatal>" will be returned.
		 * @tparam Ts           The types of the variables to insert into the
		 *                      language string, if any are given.
		 * @param  nativeString A string key which uniquely identifies a string in
		 *                      the current language's string map.
		 * @param  values       The variables to insert into the language string.
		 * @return The translated string.
		 */
		template<typename... Ts>
		std::string operator()(const std::string& nativeString, Ts... values)
			noexcept;
	private:
		/**
		 * The JSON load method for this class.
		 * Before loading, the language map is \b cleared, even if the method
		 * returns \c FALSE.\n
		 * The JSON script loaded by this class must have the following format:\n
		 * <ul><li>There should only be string values, no objects or any other
		 * types are permitted. If they exist a warning will be reported and the
		 * value will be ignored.</li>
		 * <li>There is a special key, "lang", which is paired with the current
		 * language ID as it was when last saved.</li>
		 * <li>All other keys are language IDs, and <em>their</em> values should be
		 * paths to JSON scripts detailing that language's string map.</li>
		 * <li>A blank key and its value will be completely ignored.</li></ul>\n
		 * Please see \c engine::language_dictionary::language::_load() for more
		 * details on the format of these string map JSON scripts.
		 * @param  j The \c engine::json object representing the contents
		 *           of the loaded script which this method reads.
		 * @return \c TRUE if the current language as dictated by the
		 *         \c lang key could be loaded, \c FALSE if not, or
		 *         if the current language ID could not identify a language.
		 */
		bool _load(engine::json& j) noexcept;

		/**
		 * The JSON save method for this class.
		 * Please see \c _load() for a detailed summary of
		 * the format of JSON script that this method produces.
		 * @param  j The \c nlohmann::ordered_json object representing
		 *           the JSON script which this method writes to.
		 * @return Always returns \c TRUE.
		 */
		bool _save(nlohmann::ordered_json& j) noexcept;

		/**
		 * This class represents the string map of a single language.
		 * This is a private, nested class, meaning that only the
		 * \c language_dictionary class can access it.
		 */
		class language : public engine::json_script {
		public:
			/**
			 * The character which tells \c get() not to translate a given string.
			 * Set to '~'.
			 */
			static const char TRANSLATION_OVERRIDE;

			/**
			 * Initialises the internal logger object.
			 * @param data The data to initialise the logger object with.
			 * @sa    \c engine::logger
			 */
			language(const engine::logger::data& data) noexcept;

			/**
			 * Accesses a string stored within the string map.
			 * In addition, this method uses the \c expand_string::insert()
			 * method to insert variables into the string found.
			 * If a string with the given key could not be found, then
			 * "<error>" is returned and an error is logged.
			 * "<error>" can be amended by the
			 * \c engine::language_dictionary::operator() method if any of
			 * its characters is set as a var char (see \c engine::expand_string).
			 * However, if \c nativeString begins with the character stored within
			 * \c TRANSLATION_OVERRIDE, that first character is removed from
			 * \c nativeString and then the final \c nativeString is returned. If
			 * this character is given, no variables will be inserted.
			 * @tparam Ts           The types of the variables to insert into
			 *                      the language string, if any are given.
			 * @param  nativeString The key uniquely identifying the language
			 *                      string to extract. Also called the "native
			 *                      string."
			 * @param  values       The variables to insert into the language
			 *                      string.
			 * @return The final language string.
			 */
			template<typename... Ts>
			std::string get(const std::string& nativeString, Ts... values) noexcept;
		private:
			/**
			 * The JSON load method for this class.
			 * The script should contain values of a string type only. Any other
			 * types of values will be ignored. The key of a key-value pair defines
			 * the native string which the client provides in its call to \c get().
			 * It does not have to contain any var chars and it does not have to be
			 * numbers: it can be anything, including an empty string. The value
			 * defines the corresponding language string. Each string map JSON
			 * script should contain the same list of keys, but they should have
			 * different values, depending on the language the script is supposed
			 * to store.
			 * @warning Before loading the string map is \b cleared.
			 * @param   j The \c engine::json object representing the contents of
			 *            the loaded script which this method reads.
			 * @return  Always returns \c TRUE.
			 */
			bool _load(engine::json& j) noexcept;

			/**
			 * The JSON save method for this class.
			 * Please see \c _load() for a detailed summary of the format of JSON
			 * script that this method produces.
			 * @param  j The \c nlohmann::ordered_json object representing the JSON
			 *           script which this method writes to.
			 * @return Always returns \c TRUE.
			*/
			bool _save(nlohmann::ordered_json& j) noexcept;

			/**
			 * The string map.
			 * A string map is a collection of strings, each one having a unique
			 * native string key identifying them.
			 */
			std::unordered_map<std::string, std::string> _strings;

			/**
			 * The internal logger object.
			 */
			mutable engine::logger _logger;
		};

		/**
		 * The collection of language scripts that are available for this
		 * dictionary object.
		 */
		std::unordered_map<std::string, std::string> _languageFiles;

		/**
		 * A pointer to a dynamically-allocated language map object, holding the
		 * string map of the current language.
		 */
		std::unique_ptr<engine::language_dictionary::language> _languageMap =
			nullptr;

		/**
		 * Stores the ID of the current language.
		 * @sa setLanguage()
		 */
		std::string _currentLanguage = "";

		/**
		 * The internal logger object.
		 */
		mutable engine::logger _logger;
	};
}

#include "tpp/language.tpp"
