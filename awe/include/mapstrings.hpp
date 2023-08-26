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

/**@file mapstrings.hpp
 * Declares the class which is used to store strings used with every \c map object.
 */

#pragma once

#include "logger.hpp"
#include "safejson.hpp"
#include "language.hpp"

namespace awe {
	/**
	 * Stores translation keys that are to be used with every \c map object.
	 * The keys that are stored primarily refer to names of operations. They are
	 * used to give mementos names that the scripts can retrieve and display.
	 */
	class map_strings : public engine::json_script {
	public:
		/**
		 * List of all the map operations.
		 */
		enum class operation {
			LOAD_MAP,
			MAP_NAME,
			MAP_SIZE,
			RECT_FILL_TILES,
			RECT_FILL_UNITS,
			RECT_DEL_UNITS,
			DAY,
			CREATE_ARMY,
			DELETE_ARMY,
			ARMY_TEAM,
			ARMY_FUNDS,
			ARMY_COS,
			TAG_COS,
			CREATE_UNIT,
			DELETE_UNIT,
			UNIT_POSITION,
			UNIT_HP,
			UNIT_FUEL,
			UNIT_AMMO,
			UNIT_REPLENISH,
			UNIT_WAIT,
			UNIT_CAPTURE,
			UNIT_HIDE,
			UNIT_LOAD,
			UNIT_UNLOAD,
			TILE_TYPE,
			TILE_HP,
			TILE_OWNER,
			SELECT_ARMY,
			count
		};

		/**
		 * List of all the map operation keys.
		 */
		static constexpr std::array OPERATION = {
			"loadmap",
			"mapname",
			"mapsize",
			"rectfilltiles",
			"rectfillunits",
			"rectdelunits",
			"day",
			"createarmy",
			"deletearmy",
			"armyteam",
			"armyfunds",
			"armycos",
			"tagcos",
			"createunit",
			"deleteunit",
			"unitposition",
			"unithp",
			"unitfuel",
			"unitammo",
			"unitreplenish",
			"unitwait",
			"unitcapture",
			"unithide",
			"unitload",
			"unitunload",
			"tiletype",
			"tilehp",
			"tileowner",
			"selectarmy",
		};

		/**
		 * The string used when accessing a translation key failed.
		 */
		static constexpr const char* ERROR_STRING = "<mapstrings error>";

		/**
		 * Initialises the internal logger object.
		 * @param data The data to initialise the logger object with.
		 * @sa    \c engine::logger
		 */
		map_strings(const engine::logger::data& data);

		/**
		 * Access a map operation's translation key.
		 * @param  op The map operation.
		 * @return The translation key, or \c ERROR if it could not be accessed.
		 */
		std::string operator[](operation op);
	private:
		/**
		 * The JSON load method for this class.
		 * The root object is to contain a list of key-string value pairs only.
		 * The key defines the name of the map operation to give a translation key
		 * to, and the value defines the translation key of the name to give to the
		 * map operation. If a key doesn't name a map operation, it and its value
		 * will be ignored.
		 * @param  j The \c engine::json object representing the contents of the
		 *           loaded script which this method reads.
		 * @return \c TRUE if all map operations were given names, \c FALSE if at
		 *         least one was not given.
		 * @safety Strong guarantee, if an exception is thrown or if this method
		 *         returns \c FALSE.
		 */
		bool _load(engine::json& j) override;

		/**
		 * The JSON save method for this class.
		 * Please see \c _load() for a detailed summary of the format of JSON
		 * script that this method produces.
		 * @param  j The \c nlohmann::ordered_json object representing the JSON
		 *           script which this method writes to.
		 * @return Always returns \c TRUE.
		 * @safety Strong guarantee.
		 */
		bool _save(nlohmann::ordered_json& j) override;

		/**
		 * The internal logger object.
		 */
		mutable engine::logger _logger;

		/**
		 * Maps operations to their translation keys.
		 */
		std::unordered_map<std::string, std::string> _strings;
	};
}
