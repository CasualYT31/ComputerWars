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

#include <sstream>
#include <unordered_set>
#include "boost/call_traits.hpp"
#include "script.hpp"

#define GAME_PROPERTY_COUNT 10

#define BANK_OVERRIDE_FIELD(n, i) \
	static_assert(i >= 0 && i < GAME_PROPERTY_COUNT, \
		"i must be within the game property count!"); \
	inline overrides& n(const std::string& newValue) { \
		_overrides[i] = newValue; \
		return *this; \
	} \
	inline std::string& n() { \
		return _overrides[i]; \
	}

namespace awe {
	class overrides {
		std::array<std::string, GAME_PROPERTY_COUNT> _overrides;
	public:
		inline std::string& operator[](const std::size_t i) {
			return _overrides[i];
		}
		inline const std::string& operator[](const std::size_t i) const {
			return _overrides.at(i);
		}
		friend bool operator==(const awe::overrides& lhs, const awe::overrides& rhs);
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

	inline bool operator==(const awe::overrides& lhs, const awe::overrides& rhs) {
		return lhs._overrides == rhs._overrides;
	}
}

namespace std {
	template<> struct hash<awe::overrides> {
		std::size_t operator()(awe::overrides const& f) const noexcept {
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
		inline typename boost::call_traits<T>::reference operator[](
			const overrides& overrides) {
			return _values[overrides];
		}
		inline typename boost::call_traits<T>::const_reference operator[](
			const overrides& overrides) const {
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
		overrides _sanitiseFieldOverrides(const overrides& o) const {
			overrides result;
			for (std::size_t i = 0; i < N; ++i)
				if (_scriptNamesWithOverrides[i].count(o[i]) > 0)
					result[i] = o[i];
			return result;
		}
		std::array<std::unordered_set<std::string>, N> _scriptNamesWithOverrides;
		std::unordered_map<overrides, T> _values;
	};

	// Used to map C++ types to the AS types that are returned from game property
	// field accessors. "const" is applied automatically by the PROPERTY() macro,
	// but any references must be added to the AS type if applicable. If a mapping
	// hasn't been defined here for a given C++ type, and you try to create a field
	// of that type, the code will fail to compile with an error:
	//     'value': is not a member of 'awe::AngelScriptType<T>'
	// where T will be the type you gave.
	template<typename T> struct AngelScriptType {};
	template<> struct AngelScriptType<std::string> {
		static constexpr char* const value = "string&";
	};
}

/* Generates code necessary to define a field in a game property class.
	cc: C++ game property class the field belongs to.
	ac: String literal containing the game property class's AngelScript typename.
	 n: The name of the property in C++ and AngelScript.
	ct: The C++ type of the property, without qualifiers.
	 i: Depth of the hierarchy desired (see awe::property_field).
	 e: Extra processing that's applied to the property. Can be nothing.
*/
#define PROPERTY(cc, ac, n, ct, i, e) class n##_ { \
	awe::property_field<ct, i> _##n; \
public: \
	n##_(engine::json& j, engine::logger& logger) : _##n(j, { #n }) { e } \
	static void Register(asIScriptEngine* engine) { \
		std::stringstream builder; \
		builder << "const "; \
		builder << awe::AngelScriptType<ct>::value; \
		builder << " " #n "(const Overrides&in) const"; \
		engine->RegisterObjectMethod(ac, builder.str().c_str(), \
			asMETHODPR(n##_, operator(), (const awe::overrides&) const, \
				typename boost::call_traits<ct>::const_reference), \
			asCALL_THISCALL, nullptr, asOFFSET(cc, n), false); \
	} \
	typename boost::call_traits<ct>::reference operator()( \
		const awe::overrides& overrides = {}) { \
		return _##n[overrides]; \
	} \
	typename boost::call_traits<ct>::const_reference operator()( \
		const awe::overrides& overrides = {}) const { \
		return _##n[overrides]; \
	} \
} n;

// Building blocks of a game property class.
#define GAME_PROPERTY_DECLARE(cc) class cc { \
	std::string _scriptName; \
public: \
	cc(const std::string& scriptName, engine::json& j, engine::logger& logger) : \
		_scriptName(scriptName),

#define GAME_PROPERTY_REGISTER(cc, ac, e) { e } \
	static void Register(asIScriptEngine* engine) { \
		engine->RegisterObjectMethod(ac, "const string& scriptName() const", \
			asMETHOD(cc, scriptName), asCALL_THISCALL);

#define GAME_PROPERTY_SCRIPTNAME() } \
	inline const std::string& scriptName() const { return _scriptName; }

/* Generates a game property class with 1 field.
	Unfortunately, I can't come up with a cleaner solution to support variable
	numbers of fields in macros beyond manually defining each N-field macro. To
	simplify the process, I've written a Python script that can generate them.
	cc: C++ name of the game property type.
	ac: String literal containing the typename to give this game property in AS.
	 i: Depth of the hierarchy desired for every field (see awe::property_field).
	p1: The name of the first field.
	t1: The C++ type of the first field, without qualifiers.
	 e: Extra processing that's applied to every field. Can be nothing.
*/
#define GAME_PROPERTY_1(cc, ac, i, p1, t1, e) \
	GAME_PROPERTY_DECLARE(cc) \
        p1(j, logger) \
    GAME_PROPERTY_REGISTER(cc, ac, e) \
        p1##_::Register(engine); \
    GAME_PROPERTY_SCRIPTNAME() \
    PROPERTY(cc, ac, p1, t1, i) \
};

namespace awe {
	GAME_PROPERTY_1(country, "Country", 2, longName, std::string, )
}
