/*Copyright 2019-2024 CasualYouTuber31 <naysar@protonmail.com>

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

/**@file bank-v2.hpp
 * This header file declares classes used to store static game information specific
 * to Computer Wars.
 * Static game information describes information that doesn't change during
 * execution. This includes, but is not limited to:
 * <ul><li>Types of weather.</li>
 * <li>The countries available.</li>
 * <li>Movement types.</li>
 * <li>Types of terrain.</li>
 * <li>Types of units.</li>
 * <li>Commanders available.</li></ul>
 */

#pragma once

#include <unordered_set>
#include "boost/call_traits.hpp"
#include "script.hpp"

#define GAME_PROPERTY_COUNT 10

#define BANK_OVERRIDE_FIELD(n, i) \
	static_assert(i >= 0 && i < GAME_PROPERTY_COUNT, \
		"i must be within the game property count!"); \
	inline field_overrides& n(const std::string& newValue) { \
		_overrides[i] = newValue; \
		return *this; \
	} \
	inline std::string& n() { \
		return _overrides[i]; \
	}

namespace awe {
	class field_overrides {
		std::array<std::string, GAME_PROPERTY_COUNT> _overrides;
	public:
		inline std::string& operator[](const std::size_t i) {
			return _overrides[i];
		}
		inline const std::string& operator[](const std::size_t i) const {
			return _overrides.at(i);
		}
		friend bool operator==(const awe::field_overrides& lhs, const awe::field_overrides& rhs);
		BANK_OVERRIDE_FIELD(weapon, 9)
		BANK_OVERRIDE_FIELD(unitType, 8)
		BANK_OVERRIDE_FIELD(terrain, 7)
		BANK_OVERRIDE_FIELD(tileType, 6)
		BANK_OVERRIDE_FIELD(structure, 5)
		BANK_OVERRIDE_FIELD(movementType, 4)
		BANK_OVERRIDE_FIELD(country, 3)
		BANK_OVERRIDE_FIELD(environment, 2)
		BANK_OVERRIDE_FIELD(weather, 1)
		BANK_OVERRIDE_FIELD(commander, 0)
	};

	inline bool operator==(const awe::field_overrides& lhs, const awe::field_overrides& rhs) {
		return lhs._overrides == rhs._overrides;
	}
}

namespace std {
	template<> struct hash<awe::field_overrides> {
		std::size_t operator()(awe::field_overrides const& f) const noexcept {
			std::hash<std::string> hasher;
			std::size_t seed = 0;
			for (std::size_t i = 0; i < GAME_PROPERTY_COUNT; ++i)
				seed ^= hasher(f[i]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			return seed;
		}
	};
}

namespace awe {

	// T = type of field.
	// N = depth of the hierarchy desired. 1 = just CO, 2 = CO, then weather, etc.
	//                                     0 = no overrides.
	template<typename T, std::size_t N>
	class property_field {
		static_assert(N < GAME_PROPERTY_COUNT,
			"N must be within the game property count!");
	public:
		inline property_field(engine::json& j,
			const engine::json::KeySequence& keys) {
			for (auto& i : _scriptNamesWithOverrides) i.insert("");
			// operator[] automatically creates an empty string for us;
			// we can always rely on the default value existing.
			j.apply(_values[{}], keys, true);
		}
		inline T& operator[](const field_overrides& overrides) {
			return _values[overrides];
		}
		inline typename boost::call_traits<T>::const_reference operator[](
			const field_overrides& overrides) const {
			try {
				return _values.at(overrides);
			} catch (const std::out_of_range&) {
				try {
					return _values.at(_sanitiseFieldOverrides(overrides));
				} catch (const std::out_of_range&) {
					return _values.at({});
				}
			}
		}
	private:
		field_overrides _sanitiseFieldOverrides(
			const field_overrides& overrides) const {
			field_overrides result;
			for (std::size_t i = 0; i < N; ++i)
				if (_scriptNamesWithOverrides[i].count(overrides[i]) > 0)
					result[i] = overrides[i];
			return result;
		}
		std::array<std::unordered_set<std::string>, N> _scriptNamesWithOverrides;
		std::unordered_map<field_overrides, T> _values;
	};

	//class script_name_field {
	//public:
	//	script_name_field(const std::string& scriptName);
	//	static void Register(const std::string& type, asIScriptEngine* engine) {
	//		engine->RegisterObjectMethod(type.c_str(), // < may not need this if we will use macros.
	//			"const string& longName(const Overrides&in) const",
	//			asMETHODPR(country_long_name, operator(), (const field_overrides&) const, const std::string&),
	//			asCALL_THISCALL, nullptr, asOFFSET(country, longName), false);
	//	}

	//private:
	//	std::string _scriptName;
	//};

	class country {
	public:
		country(const std::string& scriptName, engine::json& j, engine::logger& logger)
			: longName(j, logger)
		{
			// Extra processing of all fields will go here.
		}
		static void Register(const std::string& type, asIScriptEngine* engine) {
			country_long_name::Register(type, engine);
		}
		class country_long_name {
		public:
			// Initialises the default value (i.e. no overrides, from JSON).
			country_long_name(engine::json& j, engine::logger& logger) :
				_longName(j, { "longName" })
			{
				// Extra processing of the long name field will go here.
			}
			// Register const access for the scripts.
			static void Register(const std::string& type, asIScriptEngine* engine) {
				engine->RegisterObjectMethod(type.c_str(), // < may not need this if we will use macros (think I meant the c_str() call).
					"const string& longName(const Overrides&in) const",
					asMETHODPR(country_long_name, operator(), (const field_overrides&) const, const std::string&),
					asCALL_THISCALL, nullptr, asOFFSET(country, longName), false);
			}
			// Used to add overrides: available only to the game_engine.
			std::string& operator()(const field_overrides& overrides = {}) {
				return _longName[overrides];
			}
			// Retrieve the default, or overridden value.
			const std::string& operator()(const field_overrides& overrides = {}) const {
				return _longName[overrides];
			}
		private:
			property_field<std::string, 2> _longName;
		} longName;
		// Usage example:
		// Add or replace override for JAKE:
		// longName[country_override().commander("JAKE")] = "JAKE'S override"
		// Retrieve override for JAKE [non-const and const]:
		// longName[country_override().commander("JAKE")]
	private:
	};
}
