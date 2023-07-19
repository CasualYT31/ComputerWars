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

/**@file fonts.hpp
 * Defines the \c fonts class.
 */

#pragma once

#include "safejson.hpp"
#include "SFML/Graphics/Font.hpp"

namespace sfx {
	/**
	 * This class is used as a JSON-configured font collection.
	 */
	class fonts : public engine::json_script {
	public:
		/**
		 * Initialises the internal logger object.
		 * @param data The data to initialise the logger object with.
		 * @sa    \c engine::logger
		 */
		fonts(const engine::logger::data& data);

		/**
		 * Accesses a previously loaded \c sf::Font object.
		 * If a non-existent font is given, an error will be logged.\n
		 * The \c font object may technically be amended but no edits will be saved
		 * in any way via \c save().
		 * @param  key The string name of the font which was given in the JSON
		 *             script.
		 * @return The pointer to the \c sf::Font object, or \c nullptr if the font
		 *         didn't exist.
		 * @safety Strong guarantee.
		 */
		std::shared_ptr<sf::Font> operator[](const std::string& key) const;

		/**
		 * Accesses a previously loaded font file path.
		 * If a non-existent font is given, an error will be logged.
		 * @param  key The string name of the font which was given in the JSON
		 *             script.
		 * @return The file path of the font, or an empty string if the font didn't
		 *         exist.
		 * @safety Strong guarantee.
		 */
		std::string getFontPath(const std::string& key) const;
	private:
		/**
		 * The JSON load method for this class.
		 * The root object is to contain a list of key-string value pairs only.
		 * The key defines the name of the font object, and the value defines the
		 * absolute or relative path of the font file to load. If a font file could
		 * not be loaded, it won't be included in the internal fonts map.
		 * @param  j The \c engine::json object representing the contents of the
		 *           loaded script which this method reads.
		 * @return \c TRUE if all font files could be loaded, \c FALSE if at least
		 *         one could not be loaded.
		 * @safety Strong guarantee.
		 */
		bool _load(engine::json& j);

		/**
		 * The JSON save method for this class.
		 * Please see \c _load() for a detailed summary of the format of JSON
		 * script that this method produces.
		 * @param  j The \c nlohmann::ordered_json object representing the JSON
		 *           script which this method writes to.
		 * @return Always returns \c TRUE.
		 * @safety Strong guarantee.
		 */
		bool _save(nlohmann::ordered_json& j);

		/**
		 * The collection of font objects.
		 */
		std::unordered_map<std::string, std::shared_ptr<sf::Font>> _font;

		/**
		 * The collection of font file paths corresponding to each font object.
		 */
		std::unordered_map<std::string, std::string> _fontpath;

		/**
		 * The internal logger object.
		 */
		mutable engine::logger _logger;
	};
}
