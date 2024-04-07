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
#include "SFML/System/Vector2.hpp"
#include "SFML/System/Time.hpp"
#include "script.hpp"
#include "typedef.hpp"

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
	/**
	 * Data used to setup \c awe::random_particles::data objects.
	 * @sa \c awe::random_particles::data.
	 */
	struct particle_data {
		std::string sheet;
		std::string spriteID;
		float density;
		sf::Vector2f vector;
		sf::Time respawnDelay;
	};
}

namespace awe {
	template<typename T> struct Serialisable {};
	template<> struct Serialisable<std::string> {
		static void fromJSON(std::string& value, engine::json& j,
			const engine::json::KeySequence& keys) {
			j.apply(value, keys, true);
		}
	};
	template<> struct Serialisable<sf::Color> {
		static void fromJSON(sf::Color& value, engine::json& j,
			const engine::json::KeySequence& keys) {
			j.applyColour(value, keys, true);
		}
	};
	template<> struct Serialisable<std::vector<particle_data>> {
		static void fromJSON(std::vector<particle_data>& value, engine::json& j,
			const engine::json::KeySequence& keys) {
			nlohmann::ordered_json p;
			if (j.keysExist({ "particles" }, &p) &&
				p.is_array() && !p.empty() && p.at(0).is_object()) {
				value.reserve(p.size());
				for (const auto& particle : p) {
					value.emplace_back();
					if (particle.contains("sheet") &&
						particle["sheet"].is_string())
						value.back().sheet = particle["sheet"];
					if (particle.contains("sprite") &&
						particle["sprite"].is_string())
						value.back().spriteID = particle["sprite"];
					const nlohmann::ordered_json t = static_cast<std::size_t>(0);
					const nlohmann::ordered_json testFloat = 0.0f;
					if (particle.contains("density") &&
						engine::json::equalType(testFloat, particle["density"]))
						value.back().density = particle["density"];
					if (particle.contains("vectorx") &&
						engine::json::equalType(testFloat, particle["vectorx"]))
						value.back().vector.x = particle["vectorx"];
					if (particle.contains("vectory") &&
						engine::json::equalType(testFloat, particle["vectory"]))
						value.back().vector.y = particle["vectory"];
					if (particle.contains("respawndelay") &&
						engine::json::equalType(t, particle["respawndelay"]))
						value.back().respawnDelay =
							sf::milliseconds(particle["respawndelay"]);
				}
			}
		}
	};

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
			// We can always rely on the default value existing in a blank state so
			// long as T is default constructible.
			awe::Serialisable<T>::fromJSON(_values[{}], j, keys);
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
	// hasn't been defined here for a given C++ type, fields of that type won't be
	// registered with the script interface. This may be handy in cases where the
	// data is in a complex format that's not worth adding to the interface
	// (especially if the scripts won't need that data).
	template<typename T> struct AngelScriptType {
		static constexpr char* const value = "";
	};
	template<> struct AngelScriptType<std::string> {
		static constexpr char* const value = "string&";
	};
	template<> struct AngelScriptType<sf::Color> {
		static constexpr char* const value = "Colour&";
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
		if constexpr (awe::AngelScriptType<ct>::value[0] == '\0') return; \
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
	cc(const std::string& scriptName, engine::json& j, engine::logger& logger) :

#define GAME_PROPERTY_REGISTER(cc, ac, e) _scriptName(scriptName) { e } \
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
	e1: Extra processing that's applied to the first field.
	 e: Extra processing that's applied to every field after every field has been
	    processed. Can be nothing.
	 a: Append extra code to the end of the class.
*/

#define GAME_PROPERTY_1(cc, ac, i, p1, t1, e1, e, a) \
    GAME_PROPERTY_DECLARE(cc) \
        p1(j, logger), \
    GAME_PROPERTY_REGISTER(cc, ac, e) \
        p1##_::Register(engine); \
    GAME_PROPERTY_SCRIPTNAME() \
    PROPERTY(cc, ac, p1, t1, i, e1) \
    a \
};

#define GAME_PROPERTY_4(cc, ac, i, p1, t1, e1, p2, t2, e2, p3, t3, e3, p4, t4, e4, e, a) \
    GAME_PROPERTY_DECLARE(cc) \
        p1(j, logger), \
        p2(j, logger), \
        p3(j, logger), \
        p4(j, logger), \
    GAME_PROPERTY_REGISTER(cc, ac, e) \
        p1##_::Register(engine); \
        p2##_::Register(engine); \
        p3##_::Register(engine); \
        p4##_::Register(engine); \
    GAME_PROPERTY_SCRIPTNAME() \
    PROPERTY(cc, ac, p1, t1, i, e1) \
    PROPERTY(cc, ac, p2, t2, i, e2) \
    PROPERTY(cc, ac, p3, t3, i, e3) \
    PROPERTY(cc, ac, p4, t4, i, e4) \
    a \
};

#define GAME_PROPERTY_5(cc, ac, i, p1, t1, e1, p2, t2, e2, p3, t3, e3, p4, t4, e4, p5, t5, e5, e, a) \
    GAME_PROPERTY_DECLARE(cc) \
        p1(j, logger), \
        p2(j, logger), \
        p3(j, logger), \
        p4(j, logger), \
        p5(j, logger), \
    GAME_PROPERTY_REGISTER(cc, ac, e) \
        p1##_::Register(engine); \
        p2##_::Register(engine); \
        p3##_::Register(engine); \
        p4##_::Register(engine); \
        p5##_::Register(engine); \
    GAME_PROPERTY_SCRIPTNAME() \
    PROPERTY(cc, ac, p1, t1, i, e1) \
    PROPERTY(cc, ac, p2, t2, i, e2) \
    PROPERTY(cc, ac, p3, t3, i, e3) \
    PROPERTY(cc, ac, p4, t4, i, e4) \
    PROPERTY(cc, ac, p5, t5, i, e5) \
    a \
};

#define GAME_PROPERTY_6(cc, ac, i, p1, t1, e1, p2, t2, e2, p3, t3, e3, p4, t4, e4, p5, t5, e5, p6, t6, e6, e, a) \
    GAME_PROPERTY_DECLARE(cc) \
        p1(j, logger), \
        p2(j, logger), \
        p3(j, logger), \
        p4(j, logger), \
        p5(j, logger), \
        p6(j, logger), \
    GAME_PROPERTY_REGISTER(cc, ac, e) \
        p1##_::Register(engine); \
        p2##_::Register(engine); \
        p3##_::Register(engine); \
        p4##_::Register(engine); \
        p5##_::Register(engine); \
        p6##_::Register(engine); \
    GAME_PROPERTY_SCRIPTNAME() \
    PROPERTY(cc, ac, p1, t1, i, e1) \
    PROPERTY(cc, ac, p2, t2, i, e2) \
    PROPERTY(cc, ac, p3, t3, i, e3) \
    PROPERTY(cc, ac, p4, t4, i, e4) \
    PROPERTY(cc, ac, p5, t5, i, e5) \
    PROPERTY(cc, ac, p6, t6, i, e6) \
    a \
};

#define GAME_PROPERTY_7(cc, ac, i, p1, t1, e1, p2, t2, e2, p3, t3, e3, p4, t4, e4, p5, t5, e5, p6, t6, e6, p7, t7, e7, e, a) \
    GAME_PROPERTY_DECLARE(cc) \
        p1(j, logger), \
        p2(j, logger), \
        p3(j, logger), \
        p4(j, logger), \
        p5(j, logger), \
        p6(j, logger), \
        p7(j, logger), \
    GAME_PROPERTY_REGISTER(cc, ac, e) \
        p1##_::Register(engine); \
        p2##_::Register(engine); \
        p3##_::Register(engine); \
        p4##_::Register(engine); \
        p5##_::Register(engine); \
        p6##_::Register(engine); \
        p7##_::Register(engine); \
    GAME_PROPERTY_SCRIPTNAME() \
    PROPERTY(cc, ac, p1, t1, i, e1) \
    PROPERTY(cc, ac, p2, t2, i, e2) \
    PROPERTY(cc, ac, p3, t3, i, e3) \
    PROPERTY(cc, ac, p4, t4, i, e4) \
    PROPERTY(cc, ac, p5, t5, i, e5) \
    PROPERTY(cc, ac, p6, t6, i, e6) \
    PROPERTY(cc, ac, p7, t7, i, e7) \
    a \
};

#define GAME_PROPERTY_10(cc, ac, i, p1, t1, e1, p2, t2, e2, p3, t3, e3, p4, t4, e4, p5, t5, e5, p6, t6, e6, p7, t7, e7, p8, t8, e8, p9, t9, e9, p10, t10, e10, e, a) \
    GAME_PROPERTY_DECLARE(cc) \
        p1(j, logger), \
        p2(j, logger), \
        p3(j, logger), \
        p4(j, logger), \
        p5(j, logger), \
        p6(j, logger), \
        p7(j, logger), \
        p8(j, logger), \
        p9(j, logger), \
        p10(j, logger), \
    GAME_PROPERTY_REGISTER(cc, ac, e) \
        p1##_::Register(engine); \
        p2##_::Register(engine); \
        p3##_::Register(engine); \
        p4##_::Register(engine); \
        p5##_::Register(engine); \
        p6##_::Register(engine); \
        p7##_::Register(engine); \
        p8##_::Register(engine); \
        p9##_::Register(engine); \
        p10##_::Register(engine); \
    GAME_PROPERTY_SCRIPTNAME() \
    PROPERTY(cc, ac, p1, t1, i, e1) \
    PROPERTY(cc, ac, p2, t2, i, e2) \
    PROPERTY(cc, ac, p3, t3, i, e3) \
    PROPERTY(cc, ac, p4, t4, i, e4) \
    PROPERTY(cc, ac, p5, t5, i, e5) \
    PROPERTY(cc, ac, p6, t6, i, e6) \
    PROPERTY(cc, ac, p7, t7, i, e7) \
    PROPERTY(cc, ac, p8, t8, i, e8) \
    PROPERTY(cc, ac, p9, t9, i, e9) \
    PROPERTY(cc, ac, p10, t10, i, e10) \
    a \
};

namespace awe {
	GAME_PROPERTY_4(movement_type, "MovementType", 4,
		longName, std::string,,
		shortName, std::string,,
		icon, std::string,,
		description, std::string,,
	, )

	GAME_PROPERTY_5(country, "Country", 3,
		longName, std::string,,
		shortName, std::string,,
		icon, std::string,,
		description, std::string,,
		colour, sf::Color,,
		_turnOrder = _turnOrderCounter++;,
		private: awe::ArmyID _turnOrder; static awe::ArmyID _turnOrderCounter;
	)
	awe::ArmyID country::_turnOrderCounter = 0;

	GAME_PROPERTY_6(weather, "Weather", 1,
		longName, std::string,,
		shortName, std::string,,
		icon, std::string,,
		description, std::string,,
		sound, std::string,,
		particles, std::vector<awe::particle_data>,,
	, )

	GAME_PROPERTY_7(environment, "Environment", 2,
		longName, std::string,,
		shortName, std::string,,
		icon, std::string,,
		description, std::string,,
		spritesheet, std::string,,
		pictureSpritesheet, std::string,,
		structureIconSpritesheet, std::string,,
	, )
}

namespace awe {
	template<typename T>
	class bank : public engine::script_registrant, public engine::json_script {
	public:
	private:
		/**
		 * Each game property is stored in here, accessible via their script names.
		 */
		nlohmann::ordered_map<std::string, std::shared_ptr<T>> _bank;
	};
}
