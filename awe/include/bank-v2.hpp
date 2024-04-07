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

#include <new>
#include <sstream>
#include <unordered_set>
#include "boost/call_traits.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/System/Time.hpp"
#include "script.hpp"
#include "typedef.hpp"

#define GAME_PROPERTY_COUNT 10

#define REGISTER_BANK_OVERRIDE_FIELD(n) \
	r = engine->RegisterObjectMethod("Overrides", \
		"Overrides& " #n "(const string&in)", \
		asMETHODPR(awe::overrides, n, (const std::string&), \
			awe::overrides&), asCALL_THISCALL); \
	r = engine->RegisterObjectMethod("Overrides", "string& " #n "()", \
		asMETHODPR(awe::overrides, n, (), std::string&), asCALL_THISCALL); \
	r = engine->RegisterObjectMethod("Overrides", "string& " #n "() const", \
		asMETHODPR(awe::overrides, n, () const, const std::string&), \
		asCALL_THISCALL);

#define BANK_OVERRIDE_FIELD(n, i) \
	static_assert(i >= 0 && i < GAME_PROPERTY_COUNT, \
		"i must be within the game property count!"); \
	inline overrides& n(const std::string& newValue) { \
		_overrides[i] = newValue; \
		return *this; \
	} \
	inline std::string& n() { \
		return _overrides[i]; \
	} \
	inline const std::string& n() const { \
		return _overrides.at(i); \
	}

namespace awe {
	class overrides : engine::script_reference_type<awe::overrides> {
		std::array<std::string, GAME_PROPERTY_COUNT> _overrides;
		// By default, an empty overrides object is constructed.
		// However, the engine can register its own factory function for overrides
		// that's implicitly invoked when a new overrides object is constructed.
		// This is useful for awe::map, which can use this to automatically
		// provide override fields based on context (e.g. uses the current army and
		// their COs by default, etc.), without the engine or scripts having to
		// manually set them each time.
		static std::function<void(awe::overrides&)> _factory;
	public:
		static void setFactoryFunction(
			const std::function<void(awe::overrides&)>& func) { _factory = func; }
		overrides() { if (_factory) _factory(*this); }
		/**
		 * Creates the overrides.
		 * @return Pointer to the overrides.
		 */
		static awe::overrides* Create() {
			return new awe::overrides();
		}
		overrides& operator=(const awe::overrides& o) {
			_overrides = o._overrides;
			return *this;
		}
		inline std::string& operator[](const std::size_t i) {
			return _overrides[i];
		}
		inline const std::string& operator[](const std::size_t i) const {
			return _overrides.at(i);
		}
		friend bool operator==(const awe::overrides& lhs, const awe::overrides& rhs);
		static void Register(asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>& document) {
			if (engine->GetTypeInfoByName("Overrides")) return;
			auto r = RegisterType(engine, "Overrides",
				[](asIScriptEngine* engine, const std::string& type) {
					engine->RegisterObjectBehaviour(type.c_str(), asBEHAVE_FACTORY,
						std::string(type + "@ f()").c_str(),
						asFUNCTION(awe::overrides::Create), asCALL_CDECL);
				});
			REGISTER_BANK_OVERRIDE_FIELD(weapon)
			REGISTER_BANK_OVERRIDE_FIELD(unitType)
			REGISTER_BANK_OVERRIDE_FIELD(terrain)
			REGISTER_BANK_OVERRIDE_FIELD(tileType)
			REGISTER_BANK_OVERRIDE_FIELD(structure)
			REGISTER_BANK_OVERRIDE_FIELD(movementType)
			REGISTER_BANK_OVERRIDE_FIELD(country)
			REGISTER_BANK_OVERRIDE_FIELD(environment)
			REGISTER_BANK_OVERRIDE_FIELD(weather)
			REGISTER_BANK_OVERRIDE_FIELD(commander)
		}
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
	std::function<void(awe::overrides&)> awe::overrides::_factory = {};

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
		float density = 0.f;
		sf::Vector2f vector;
		sf::Time respawnDelay;
	};
}

namespace awe {
	template<typename T> struct Serialisable {
		static void fromJSON(std::string& value, engine::json& j,
			const engine::json::KeySequence& keys, engine::logger&) {
			j.apply(value, keys, true);
		}
	};
	template<> struct Serialisable<sf::Color> {
		static void fromJSON(sf::Color& value, engine::json& j,
			const engine::json::KeySequence& keys, engine::logger&) {
			j.applyColour(value, keys, true);
		}
	};
	template<> struct Serialisable<std::vector<particle_data>> {
		static void fromJSON(std::vector<particle_data>& value, engine::json& j,
			const engine::json::KeySequence& keys, engine::logger&) {
			// TODO: Write logs.
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
			const engine::json::KeySequence& keys, engine::logger& logger) {
			for (auto& i : _scriptNamesWithOverrides) i.insert("");
			// We can always rely on the default value existing in a blank state so
			// long as T is default constructible.
			awe::Serialisable<T>::fromJSON(_values[{}], j, keys, logger);
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
	n##_(engine::json& j, engine::logger& logger) : _##n(j, { #n }, logger) { e } \
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

#define GAME_PROPERTY_END(cc, ac, gp, a) a \
public: \
	static constexpr char* const type = ac; \
	static constexpr char* const global_property = gp; \
};

/* Generates a game property class with 1 field.
	Unfortunately, I can't come up with a cleaner solution to support variable
	numbers of fields in macros beyond manually defining each N-field macro. To
	simplify the process, I've written a Python script that can generate them.
	cc: C++ name of the game property type.
	ac: String literal containing the typename to give this game property in AS.
	gp: String literal containing the name of the global property of the bank type
	    that stores this game property type.
	 i: Depth of the hierarchy desired for every field (see awe::property_field).
	p1: The name of the first field.
	t1: The C++ type of the first field, without qualifiers.
	e1: Extra processing that's applied to the first field.
	 e: Extra processing that's applied to every field after every field has been
	    processed. Can be nothing.
	 a: Append extra code to the end of the class.
*/

#define GAME_PROPERTY_1(cc, ac, gp, i, p1, t1, e1, e, a) \
    GAME_PROPERTY_DECLARE(cc) \
        p1(j, logger), \
    GAME_PROPERTY_REGISTER(cc, ac, e) \
        p1##_::Register(engine); \
    GAME_PROPERTY_SCRIPTNAME() \
    PROPERTY(cc, ac, p1, t1, i, e1) \
    GAME_PROPERTY_END(cc, ac, gp, a)

#define GAME_PROPERTY_4(cc, ac, gp, i, p1, t1, e1, p2, t2, e2, p3, t3, e3, p4, t4, e4, e, a) \
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
    GAME_PROPERTY_END(cc, ac, gp, a)

#define GAME_PROPERTY_5(cc, ac, gp, i, p1, t1, e1, p2, t2, e2, p3, t3, e3, p4, t4, e4, p5, t5, e5, e, a) \
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
    GAME_PROPERTY_END(cc, ac, gp, a)

#define GAME_PROPERTY_6(cc, ac, gp, i, p1, t1, e1, p2, t2, e2, p3, t3, e3, p4, t4, e4, p5, t5, e5, p6, t6, e6, e, a) \
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
    GAME_PROPERTY_END(cc, ac, gp, a)

#define GAME_PROPERTY_7(cc, ac, gp, i, p1, t1, e1, p2, t2, e2, p3, t3, e3, p4, t4, e4, p5, t5, e5, p6, t6, e6, p7, t7, e7, e, a) \
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
    GAME_PROPERTY_END(cc, ac, gp, a)

#define GAME_PROPERTY_10(cc, ac, gp, i, p1, t1, e1, p2, t2, e2, p3, t3, e3, p4, t4, e4, p5, t5, e5, p6, t6, e6, p7, t7, e7, p8, t8, e8, p9, t9, e9, p10, t10, e10, e, a) \
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
    GAME_PROPERTY_END(cc, ac, gp, a)

namespace awe {
	GAME_PROPERTY_4(movement_type, "MovementType", "movementtype", 4,
		longName, std::string,,
		shortName, std::string,,
		icon, std::string,,
		description, std::string,,
	, )

	GAME_PROPERTY_5(country, "Country", "country", 3,
		longName, std::string,,
		shortName, std::string,,
		icon, std::string,,
		description, std::string,,
		colour, sf::Color,,
		_turnOrder = _turnOrderCounter++;,
		private: awe::ArmyID _turnOrder; static awe::ArmyID _turnOrderCounter;
	)
	awe::ArmyID country::_turnOrderCounter = 0;

	GAME_PROPERTY_7(environment, "Environment", "environment", 2,
		longName, std::string,,
		shortName, std::string,,
		icon, std::string,,
		description, std::string,,
		spritesheet, std::string,,
		pictureSpritesheet, std::string,,
		structureIconSpritesheet, std::string,,
	, )

	GAME_PROPERTY_6(weather, "Weather", "weather", 1,
		longName, std::string,,
		shortName, std::string,,
		icon, std::string,,
		description, std::string,,
		sound, std::string,,
		particles, std::vector<awe::particle_data>,,
	, )
}

#define COMMA ,

namespace awe {
	template<typename T>
	class bank : public engine::script_registrant, public engine::json_script {
	public:
		/**
		 * The type of the container used to store game property values.
		 */
		using type = nlohmann::ordered_map<std::string, std::shared_ptr<T>>;

		/**
		 * Provides script interface details to this @c bank instance.
		 * @param scripts Pointer to the @c scripts object to register this bank
		 *                with. If @c nullptr, the bank won't be registered with
		 *                any script interface.
		 * @param data    The data to initialise the logger object with.
		 */
		bank(const std::shared_ptr<engine::scripts>& scripts,
			const engine::logger::data& data) :
			engine::json_script({ data.sink, "json_script" }), _logger(data) {
			if (scripts) scripts->addRegistrant(this);
		}

		std::shared_ptr<T> operator[](const std::string& sn) {
			if (_bank.find(sn) == _bank.end()) {
				_logger.error("Game property \"{}\" does not exist in this bank!",
					sn);
			}
			return _bank[sn];
		}

		std::shared_ptr<const T> operator[](const std::string& sn) const {
			if (_bank.find(sn) == _bank.end()) {
				_logger.error("Game property \"{}\" does not exist in this bank!",
					sn);
			}
			return _bank.at(sn);
		}

		/**
		 * Calculates the size of the bank.
		 * @return The number of members or elements of the internal map \c _bank.
		 */
		inline std::size_t size() const noexcept {
			return _bank.size();
		}

		/**
		 * Finds out if an entry exists in this bank with the given script name.
		 * @param  scriptName The script name to search for.
		 * @return \c TRUE if there is an entry with the given name, \c FALSE
		 *         otherwise.
		 */
		inline bool contains(const std::string& scriptName) const {
			return _bank.find(scriptName) != _bank.end();
		}

		// I = type::iterator or type::const_iterator.
		// J = T or const T.
		template<typename itr_type, typename J>
		class base_iterator {
			itr_type _itr;
			void copyConstructor(void* memory,
				const base_iterator<itr_type, J>& o) {
				new(memory) base_iterator<itr_type, J>(o);
			}
			void destructor(void* memory) {
				static_cast<base_iterator<itr_type, J>*>(memory)
					->~base_iterator<itr_type, J>();
			}
		public:
			base_iterator(const itr_type& itr) : _itr(itr) {}
			base_iterator(const base_iterator<itr_type, J>& origin) :
				_itr(origin._itr) {}
			~base_iterator() noexcept = default;
			inline base_iterator<itr_type, J>& operator=(const base_iterator<itr_type, J>& origin) { _itr = origin._itr; return *this; }
			inline bool operator==(const base_iterator<itr_type, J>& r) const { return _itr == r._itr; }
			inline bool operator!=(const base_iterator<itr_type, J>& r) const { return _itr != r._itr; }
			inline base_iterator<itr_type, J>& operator++() { ++_itr; return *this; }
			inline base_iterator<itr_type, J> operator++(int) { const base_iterator<itr_type, J> copy(*this); ++_itr; return copy; }
			inline base_iterator<itr_type, J>& operator--() { --_itr; return *this; }
			inline base_iterator<itr_type, J> operator--(int) { const base_iterator<itr_type, J> copy(*this); --_itr; return copy; }
			inline base_iterator<itr_type, J> operator+(const sf::Int64 ad) const { base_iterator<itr_type, J> copy(*this); std::advance(copy._itr, ad); return copy; }
			inline base_iterator<itr_type, J> operator-(const sf::Int64 rm) const { base_iterator<itr_type, J> copy(*this); std::advance(copy._itr, -rm); return copy; }
			inline J& operator*() { return *(_itr->second); }
			inline J* operator->() { return _itr->second.get(); }
			static std::string Register(asIScriptEngine* engine, std::string t) {
				const char* const itrPostfix = ((constexpr
					(std::is_const<J>::value)) ? ("ConstItr") : ("Itr"));
				const char* const tc = t.append(itrPostfix).c_str();
				auto r = engine->RegisterObjectType(tc,
					sizeof(base_iterator<itr_type, J>),
					asOBJ_VALUE | asGetTypeTraits<base_iterator<itr_type, J>>());
				std::string copyConstructorSignature("void f(const ");
				copyConstructorSignature.append(tc).append("&in)");
				r = engine->RegisterObjectBehaviour(tc, asBEHAVE_CONSTRUCT,
					copyConstructorSignature.c_str(),
					asMETHOD(base_iterator<itr_type COMMA J>, copyConstructor),
					asCALL_THISCALL_OBJFIRST);
				r = engine->RegisterObjectBehaviour(tc, asBEHAVE_DESTRUCT,
					"void f()", asMETHOD(base_iterator<itr_type COMMA J>, destructor),
					asCALL_THISCALL_OBJFIRST);
				r = engine->RegisterObjectMethod(tc, std::string(tc)
					.append("& opAssign(const ").append(tc).append("&in)").c_str(),
					asMETHOD(base_iterator<itr_type COMMA J>, operator=),
					asCALL_THISCALL);
				r = engine->RegisterObjectMethod(tc, std::string(tc)
					.append("& opEquals(const ").append(tc)
					.append("&in) const").c_str(),
					asMETHOD(base_iterator<itr_type COMMA J>, operator==),
					asCALL_THISCALL);
				r = engine->RegisterObjectMethod(tc, std::string(tc)
					.append("& opPreInc()").c_str(),
					asMETHODPR(base_iterator<itr_type COMMA J>, operator++, (),
						base_iterator<itr_type COMMA J>&),
					asCALL_THISCALL);
				r = engine->RegisterObjectMethod(tc, std::string(tc)
					.append(" opPostInc()").c_str(),
					asMETHODPR(base_iterator<itr_type COMMA J>, operator++, (int),
						base_iterator<itr_type COMMA J>),
					asCALL_THISCALL);
				r = engine->RegisterObjectMethod(tc, std::string(tc)
					.append("& opPreDec()").c_str(),
					asMETHODPR(base_iterator<itr_type COMMA J>, operator--, (),
						base_iterator<itr_type COMMA J>&),
					asCALL_THISCALL);
				r = engine->RegisterObjectMethod(tc, std::string(tc)
					.append(" opPostDec()").c_str(),
					asMETHODPR(base_iterator<itr_type COMMA J>, operator--, (int),
						base_iterator<itr_type COMMA J>),
					asCALL_THISCALL);
				r = engine->RegisterObjectMethod(tc, std::string(tc)
					.append(" opAdd(const int64) const").c_str(),
					asMETHOD(base_iterator<itr_type COMMA J>, operator+),
					asCALL_THISCALL);
				r = engine->RegisterObjectMethod(tc, std::string(tc)
					.append(" opSub(const int64) const").c_str(),
					asMETHOD(base_iterator<itr_type COMMA J>, operator-),
					asCALL_THISCALL);
				if constexpr (std::is_const<J>::value) {
					r = engine->RegisterObjectMethod(tc, std::string("const ")
						.append(t).append("@ opCall()").c_str(),
						asMETHOD(base_iterator<itr_type COMMA J>, operator->),
						asCALL_THISCALL);
				} else {
					r = engine->RegisterObjectMethod(tc, std::string(t)
						.append("@ opCall()").c_str(),
						asMETHOD(base_iterator<itr_type COMMA J>, operator->),
						asCALL_THISCALL);
				}
				return tc;
			}
		};
		using iterator = base_iterator<typename type::iterator, T>;
		using const_iterator = base_iterator<typename type::const_iterator, const T>;

		inline bank<T>::iterator begin() {
			return bank<T>::iterator(_bank.begin());
		}

		inline bank<T>::iterator end() {
			return bank<T>::iterator(_bank.end());
		}

		inline bank<T>::const_iterator cbegin() const {
			return bank<T>::const_iterator(_bank.cbegin());
		}

		inline bank<T>::const_iterator cend() const {
			return bank<T>::const_iterator(_bank.cend());
		}

		/**
		 * Callback given to \c engine::scripts::registerInterface() to register
		 * the bank type, as well as the type the bank stores, with a \c scripts
		 * object.
		 * @sa \c engine::scripts::registerInterface().
		 */
		void registerInterface(asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>& document) {
			// 1. Register dependencies shared between all bank types, as well as
			//    all the types stored within all bank types.
			awe::RegisterGameTypedefs(engine, document);

			// 2. Register the game property type that this bank stores (i.e. T).
			auto r = engine->RegisterObjectType(T::type, 0,
				asOBJ_REF | asOBJ_NOCOUNT);
			T::Register(engine);

			// 3. Register the const iterator type for this bank object.
			const auto itrType = const_iterator::Register(engine, T::type);

			// 4. Register the bank type, called T::type + "Bank".
			const auto bankTypeName = std::string(T::type).append("Bank");
			const auto bankType = bankTypeName.c_str();
			r = engine->RegisterObjectType(bankType, 0,
				asOBJ_REF | asOBJ_NOHANDLE);
			// Scripts cannot amend game properties, only read them.
			r = engine->RegisterObjectMethod(bankType, std::string("const ")
				.append(T::type).append("@ opIndex(const string&in) const").c_str(),
				asMETHOD(awe::bank<T>, _opIndexStr), asCALL_THISCALL);
			r = engine->RegisterObjectMethod(bankType, "uint64 length() const",
				asMETHOD(awe::bank<T>, size), asCALL_THISCALL);
			r = engine->RegisterObjectMethod(bankType,
				"bool contains(const string&in) const",
				asMETHOD(awe::bank<T>, contains), asCALL_THISCALL);
			r = engine->RegisterObjectMethod(bankType, std::string(itrType)
				.append(" begin() const").c_str(),
				asMETHOD(awe::bank<T>, cbegin), asCALL_THISCALL);
			r = engine->RegisterObjectMethod(bankType, std::string(itrType)
				.append(" end() const").c_str(),
				asMETHOD(awe::bank<T>, cend), asCALL_THISCALL);

			// 5. Register the global point of access to the bank object.
			const auto globalProperty = std::string(bankTypeName).append(" ")
				.append(T::global_property);
			engine->RegisterGlobalProperty(globalProperty.c_str(), this);
		}
	private:
		/**
		 * The JSON load method for this class.
		 * All classes substituted for \c T should have a common JSON script
		 * format. In the root object, key-value pairs list each member/entry of
		 * the bank and their properties.\n
		 * The keys will store the script names of each bank entry. It is then up
		 * to the classes used with this template class to parse the object values
		 * of these keys in its constructor.
		 * @param  j The \c engine::json object representing the contents of the
		 *           loaded script which this method reads.
		 * @return Always returns \c TRUE.
		 * @safety Strong guarantee.
		 * @sa     @c awe::bank<T>
		 */
		bool _load(engine::json& j) {
			type bank;
			nlohmann::ordered_json jj = j.nlohmannJSON();
			for (auto& i : jj.items()) {
				engine::json input(i.value(), { _logger.getData().sink, "json" });
				bank[i.key()] = std::make_shared<T>(i.key(), input, _logger);
			}
			_bank = std::move(bank);
			return true;
		}

		/**
		 * Script's string index operator.
		 * @throws std::runtime_error if no game property with the given name
		 *                            exists.
		 * @sa     @c awe::bank<T>::operator[](const std::string&) const
		 */
		const T* _opIndexStr(const std::string& name) const {
			auto ret = operator[](name);
			if (ret) return ret.get();
			throw std::runtime_error("Could not access game property");
		}

		/**
		 * Internal logger object.
		 */
		mutable engine::logger _logger;

		/**
		 * Each game property is stored in here, accessible via their script names.
		 */
		type _bank;
	};
}
