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

/**@file fonts.h
 * Defines the \c fonts class.
 * @todo Better error reporting needs to be considered for operator[] - perhaps throw an exception instead of returning?
 */

#pragma once

#include "safejson.h"
#include "sfml/Graphics/Font.hpp"

 // for documentation on the sfx namespace, please see renderer.h
namespace sfx {
	/**
	 * This class is used as a JSON-configured font collection.
	 * @sa _load()
	 */
	class fonts : public safe::json_script {
	public:
		/**
		 * Initialises the internal logger object.
		 * @param name The name to give this particular instantiation within the log file. Defaults to "fonts."
		 * @sa    \c global::logger
		 */
		fonts(const std::string& name = "fonts") noexcept;

		/**
		 * Accesses a previously loaded \c sf::Font object.
		 * If an non-existent font is given, an error will be logged. In addition to this, a reference to a dummy sf::Font object will be returned.
		 * The font object may technically be amended but no edits will be saved in any way via \c save().
		 * @warning Since this operator returns a reference, a new set of fonts should not be loaded so long as
		 *          the client uses the font object pointed to by this reference!
		 * @param   key The string name of the font which was given in the JSON script.
		 * @return  The reference to the \c sf::Font object.
		 */
		sf::Font& operator[](const std::string& key) noexcept;
	private:
		/**
		 * The JSON load method for this class.
		 * The root object is to contain a list of key-string value pairs only.
		 * The key defines the name of the font object, and the value defines the absolute or relative path of the font file to load.
		 * @param  j The \c safe::json object representing the contents of the loaded script which this method reads.
		 * @return \c TRUE if all font files could be loaded, \c FALSE if at least one could not be loaded.
		 */
		virtual bool _load(safe::json& j) noexcept;

		/**
		 * The JSON save method for this class.
		 * Please see \c _load() for a detailed summary of the format of JSON script that this method produces.
		 * @param  j The \c nlohmann::json object representing the JSON script which this method writes to.
		 * @return Always returns \c TRUE.
		 */
		virtual bool _save(nlohmann::json& j) noexcept;

		/**
		 * The dummy font object which is returned in case \c operator[]() was called with an invalid key.
		 */
		sf::Font _errorfont;

		/**
		 * The collection of font objects.
		 */
		std::unordered_map<std::string, sf::Font> _font;

		/**
		 * The collection of font file paths corresponding to each font object.
		 */
		std::unordered_map<std::string, std::string> _fontpath;

		/**
		 * The internal logger object.
		 */
		global::logger _logger;
	};
}