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

#include <any>
#include <new>
#include <sstream>
#include <unordered_set>
#include "boost/call_traits.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/System/Time.hpp"
#include "script.hpp"
#include "typedef.hpp"
#include "tpp/pod.tpp"

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
	class overrides : public engine::script_reference_type<awe::overrides> {
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
		overrides(const awe::overrides& cpy) : _overrides(cpy._overrides) {}
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
	//struct particle_data {
	//	std::string sheet;
	//	std::string spriteID;
	//	float density = 0.f;
	//	sf::Vector2f vector;
	//	sf::Time respawnDelay;
	//};
}

namespace awe {
	// To easily support arrays in bank-v2:
	// 1. Have them be stored as CScriptArrays initially.
	// 2. Overrides will then work with those arrays directly.
	// 3. Then, after all overrides have been calculated, create an std::vector for
	//    every override (and the default) that is then accessed by the engine by
	//    ref. This keeps both C++ and AngelScript speedy during gameplay without
	//    compromising on ease of use.
	// Only compromise is that any code *during* and *before* the override code
	// must *only* work with the CScriptArray. This includes fromJSON
	// specialisations.
	// DO NOT USE THIS TYPE OUTSIDE OF BANKS, due to bank code limitations,
	// the resource management of this class is partially manual.
	template<typename T>
	struct bank_array {
		/// @warning THIS MUST BE SET ASAP, BEFORE ANY ARRAYS ARE CREATED BY JSON LOADING CODE.
		static std::shared_ptr<engine::scripts> scripts;
		bank_array() {
			array = std::make_unique<engine::CScriptWrapper<CScriptArray>>(scripts->createArray(engine::script_type<T>()));
		};
		bank_array(const bank_array<T>& o) { *this = o; }
		static void Register(asIScriptEngine* engine, const std::shared_ptr<DocumentationGenerator>& document) {
			if (engine->GetTypeInfoByName(engine::script_type<bank_array<T>>().c_str())) return;
			// Could register a template type but I don't see the benefit for our use case.
			auto r = engine->RegisterObjectType(engine::script_type<bank_array<T>>().c_str(), 0,
				asOBJ_REF | asOBJ_NOCOUNT);
			// No factory functions, no ref counting: banks will manage the lifetime of these objects.
			// Just provide const access to the underlying array (and non-const for override code).
			// TODO: somehow find nicer way to achieve this, guess we could implement the entire
			// array interface directly into this type, but I want to avoid that if possible.
			r = engine->RegisterObjectMethod(
				engine::script_type<bank_array<T>>().c_str(),
				std::string("array<").append(engine::script_type<T>().c_str()).append(">@ get_array() property").c_str(),
				asMETHOD(bank_array<T>, _getArray), asCALL_THISCALL
			);
			r = engine->RegisterObjectMethod(
				engine::script_type<bank_array<T>>().c_str(),
				std::string("const array<").append(engine::script_type<T>().c_str()).append(">@ get_array() const property").c_str(),
				asMETHOD(bank_array<T>, _getArray), asCALL_THISCALL
			);
		}
		bank_array<T>& operator=(const bank_array<T>& o) {
			CScriptArray* newArray = scripts->createArray(engine::script_type<T>());
			*newArray = *o.array->operator->();
			array = std::make_unique<engine::CScriptWrapper<CScriptArray>>(newArray);
			// The = operator is used on a bank_array when applying overrides.
			// Calling initVector() will ensure that the vector is a copy of the
			// script array as and when overrides are applied.
			initVector();
			return *this;
		}
		std::unique_ptr<engine::CScriptWrapper<CScriptArray>> array;
		std::vector<T> vector;
		inline void initVector() {
			// Increase the ref counter of our array so we don't lose it after
			// conversion.
			array->operator->()->AddRef();
			vector = engine::ConvertCScriptArray<std::vector<T>, T>(array->operator->());
		}
	private:
		inline CScriptArray* _getArray() {
			return array->operator->();
		}
	};
	template<typename T>
	std::shared_ptr<engine::scripts> bank_array<T>::scripts = nullptr;
}

namespace awe {
	template<typename T> struct Serialisable {
		static void fromJSON(T& value, engine::json& j,
			const engine::json::KeySequence& keys, engine::logger&,
			const std::shared_ptr<engine::scripts>&) {
			j.apply(value, keys, true);
		}
	};
	template<> struct Serialisable<sf::Color> {
		static void fromJSON(sf::Color& value, engine::json& j,
			const engine::json::KeySequence& keys, engine::logger&,
			const std::shared_ptr<engine::scripts>&) {
			j.applyColour(value, keys, true);
		}
	};
	template<typename T> struct Serialisable<sf::Vector2<T>> {
		static void fromJSON(sf::Vector2<T>& value, engine::json& j,
			const engine::json::KeySequence& keys, engine::logger&,
			const std::shared_ptr<engine::scripts>&) {
			std::array<T, 2> vec;
			j.applyArray(vec, keys);
			if (!j.inGoodState()) {
				j.resetState();
				return;
			}
			value.x = vec[0];
			value.y = vec[1];
		}
	};
	template<> struct Serialisable<sf::Time> {
		static void fromJSON(sf::Time& value, engine::json& j,
			const engine::json::KeySequence& keys, engine::logger& logger,
			const std::shared_ptr<engine::scripts>&) {
			sf::Uint32 ms = 0;
			j.apply(ms, keys);
			if (!j.inGoodState()) {
				j.resetState();
				return;
			}
			if (ms > static_cast<sf::Uint32>(std::numeric_limits<sf::Int32>::max())) {
				logger.error("Cannot store a millisecond value {} at {} that is "
					"greater than {}.", ms, j.synthesiseKeySequence(keys),
					std::numeric_limits<sf::Int32>::max());
				return;
			}
			value = sf::milliseconds(static_cast<sf::Int32>(ms));
		}
	};
	// Replace direct calls to j with calls to awe::Serialisable,
	// and I think we can include this in the pod macro.
	//template<> struct Serialisable<particle_data> {
	//	static void fromJSON(particle_data& value, engine::json& j,
	//		const engine::json::KeySequence& keys, engine::logger& logger) {
	//		nlohmann::ordered_json p;
	//		if (!j.keysExist(keys, &p)) {
	//			logger.error("Attempting to read {}: these keys do not exist.",
	//				j.synthesiseKeySequence(keys));
	//			return;
	//		}
	//		if (!p.is_object()) {
	//			logger.error("Attempting to read {} as an object, but the value "
	//				"at these keys is of type \"{}\".",
	//				j.synthesiseKeySequence(keys), j.getTypeName(p));
	//			return;
	//		}
	//		j.apply(value.sheet, j.concatKeys(keys, { "sheet" }), true, true);
	//		j.apply(value.spriteID, j.concatKeys(keys, { "spriteID" }), true,
	//			true);
	//		j.apply(value.density, j.concatKeys(keys, { "density" }), true, true);
	//		if (p.contains("vector")) {
	//			awe::Serialisable<decltype(value.vector)>::fromJSON(
	//				value.vector, j, j.concatKeys(keys, { "vector" }), logger);
	//		}
	//		if (p.contains("respawnDelay")) {
	//			awe::Serialisable<decltype(value.respawnDelay)>::fromJSON(
	//				value.respawnDelay, j, j.concatKeys(keys, { "respawnDelay" }),
	//				logger);
	//		}
	//	}
	//};
	template<typename E> struct Serialisable<awe::bank_array<E>> {
		static void fromJSON(awe::bank_array<E>& value, engine::json& j,
			const engine::json::KeySequence& keys, engine::logger& logger,
			const std::shared_ptr<engine::scripts>& scripts) {
			nlohmann::ordered_json p;
			if (!j.keysExist(keys, &p)) {
				logger.error("Attempting to read {}: these keys do not exist.",
					j.synthesiseKeySequence(keys));
				return;
			}
			if (!p.is_array()) {
				logger.error("Attempting to read {} as an array, but the value at "
					"these keys is of type \"{}\".", j.synthesiseKeySequence(keys),
					j.getTypeName(p));
				return;
			}
			if (p.empty()) return;
			value.array->operator->()->Resize(static_cast<asUINT>(p.size()));
			for (asUINT i = 0, end = value.array->operator->()->GetSize(); i < end; ++i) {
				// Create an engine::json object for the array value so a fromJSON
				// implementation can get at the value.
				const auto valueKey = "arrayValue" + std::to_string(i);
				nlohmann::json valueObject;
				valueObject[valueKey] = p[i];
				engine::json valueObjectEngine(valueObject, logger.getData());
				E* element = static_cast<E*>(value.array->operator->()->At(i));
				// TODO: error logs produced by this call will be close to
				// meaningless to the user without the full key sequence. This
				// method should return FALSE so that callers know something went
				// wrong and can report more details.
				awe::Serialisable<E>::fromJSON(*element, valueObjectEngine,
					engine::json::KeySequence{ valueKey }, logger, scripts);
			}
			value.initVector();
		}
	};
	//template<> struct Serialisable<std::vector<particle_data>> {
	//	static void fromJSON(std::vector<particle_data>& value, engine::json& j,
	//		const engine::json::KeySequence& keys, engine::logger&) {
	//		// TODO: Write logs.
	//		nlohmann::ordered_json p;
	//		if (j.keysExist({ "particles" }, &p) &&
	//			p.is_array() && !p.empty() && p.at(0).is_object()) {
	//			value.reserve(p.size());
	//			for (const auto& particle : p) {
	//				value.emplace_back();
	//				if (particle.contains("sheet") &&
	//					particle["sheet"].is_string())
	//					value.back().sheet = particle["sheet"];
	//				if (particle.contains("sprite") &&
	//					particle["sprite"].is_string())
	//					value.back().spriteID = particle["sprite"];
	//				const nlohmann::ordered_json t = static_cast<std::size_t>(0);
	//				const nlohmann::ordered_json testFloat = 0.0f;
	//				if (particle.contains("density") &&
	//					engine::json::equalType(testFloat, particle["density"]))
	//					value.back().density = particle["density"];
	//				if (particle.contains("vectorx") &&
	//					engine::json::equalType(testFloat, particle["vectorx"]))
	//					value.back().vector.x = particle["vectorx"];
	//				if (particle.contains("vectory") &&
	//					engine::json::equalType(testFloat, particle["vectory"]))
	//					value.back().vector.y = particle["vectory"];
	//				if (particle.contains("respawndelay") &&
	//					engine::json::equalType(t, particle["respawndelay"]))
	//					value.back().respawnDelay =
	//						sf::milliseconds(particle["respawndelay"]);
	//			}
	//		}
	//	}
	//};

	// T = type of field.
	// N = depth of the hierarchy desired. 1 = just CO, 2 = weather, then CO, etc.
	//                                     0 = no overrides.
	template<typename T, std::size_t N>
	class property_field {
		static_assert(N < GAME_PROPERTY_COUNT,
			"N must be within the game property count!");
	public:
		inline property_field(engine::json& j,
			const engine::json::KeySequence& keys, engine::logger& logger,
			const std::shared_ptr<engine::scripts>& scripts) {
			for (auto& i : _scriptNamesWithOverrides) i.insert("");
			// We can always rely on the default value existing in a blank state so
			// long as T is default constructible.
			awe::Serialisable<T>::fromJSON(_values[{}], j, keys, logger, scripts);
		}
		inline typename boost::call_traits<T>::reference operator[](
			const overrides& overrides) {
			for (std::size_t i = 0; i < N; ++i)
				_scriptNamesWithOverrides[i].insert(overrides[i]);
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

	/**
	 * When a field in a game property is accessed, the engine should always return const refs.
	 * @tparam T The type of the parameter.
	 * @return The full AngelScript type for the return value of a game property field.
	 */
	template<typename T>
	constexpr std::string bank_return_type() {
		return std::string("const ").append(engine::script_type<T>()).append("&");
		// TODO: In the future, it would be nice to be able to differentiate between
		// primitive types and all others.
		// It makes little sense to return "const int&" when a copy ("int") would suffice.
		// To achieve this, we can wrap the call_traits<T>::const_reference instantiation
		// in the macros, in some template function that returns different values between
		// primitive values (T) and others (call_traits<T>::const_reference).
		// Let's just get something working for now.
		//if constexpr (std::is_integral<T>::value || std::is_floating_point<T>::value) {
		//	return engine::script_type<T>();
		//} else {
		//	return std::string("const ").append(engine::script_type<T>()).append("&");
		//}
	}

	template<typename T> struct OverrideVariable {
		static std::any read(const std::shared_ptr<engine::scripts>& scripts,
			const asUINT variable) {
			void* var = scripts->getGlobalVariableAddress(
				engine::scripts::modules[engine::scripts::BANK_OVERRIDE], variable);
			if (!var) return {};
			return *static_cast<T*>(var);
		}
	};
	// If a type needs a special read method for variables, specialise here.

	template<typename T> struct OverrideFunction {
		static std::any read(const std::shared_ptr<engine::scripts>& scripts,
			asIScriptFunction* const function, const std::any& parent) {
			T p = std::any_cast<T>(parent);
			scripts->callFunction(function, &p);
			return p;
		}
	};
	class particle_data;
	template<> struct OverrideFunction<awe::bank_array<awe::particle_data>> {
		static std::any read(const std::shared_ptr<engine::scripts>& scripts,
			asIScriptFunction* const function, const std::any& parent) {
			awe::bank_array<awe::particle_data> p = std::any_cast<awe::bank_array<awe::particle_data>>(parent);
			// TODO: Find a different way to address this refcount issue...
			// I don't want to have to specialise for every bank_array type,
			// I already have to do something similar with bank_array::scripts...
			p.array->operator->()->AddRef();
			scripts->callFunction(function, &p);
			return p;
		}
	};
	// If a type needs a special read method for functions, specialise here.
}

/**
 * Data used to setup \c awe::random_particles::data objects.
 * @sa \c awe::random_particles::data.
 */
DECLARE_POD_5(awe, particle_data, "ParticleData",
	std::string, sheet,
	std::string, spriteID,
	float, density,
	sf::Vector2f, vector,
	sf::Time, respawnDelay
);
	
DEFINE_POD_5(awe, particle_data, "ParticleData",
	std::string, sheet,
	std::string, spriteID,
	float, density,
	sf::Vector2f, vector,
	sf::Time, respawnDelay
);

template<>
inline constexpr std::string engine::script_type<awe::bank_array<awe::particle_data>>() {
	return std::string(engine::script_type<awe::particle_data>()).append("Array");
}

#include "tpp/bank-v2-macros.tpp"

namespace awe {
	GAME_PROPERTY_4(movement_type, "MovementType", "movementtype", 4,
		longName, std::string,,
		shortName, std::string,,
		icon, std::string,,
		description, std::string,,
	,, )

	GAME_PROPERTY_5(country, "Country", "country", 3,
		longName, std::string,,
		shortName, std::string,,
		icon, std::string,,
		description, std::string,,
		colour, sf::Color,,
		,
		_turnOrder = _turnOrderCounter++;,
		private: awe::ArmyID _turnOrder; static awe::ArmyID _turnOrderCounter;
		public: awe::ArmyID	turnOrder() { return _turnOrder; }
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
	,, )

	GAME_PROPERTY_6(weather, "Weather", "weather", 1,
		longName, std::string,,
		shortName, std::string,,
		icon, std::string,,
		description, std::string,,
		sound, std::string,,
		particles, awe::bank_array<awe::particle_data>,,
		awe::particle_data::Register(engine, document);
		awe::bank_array<awe::particle_data>::Register(engine, document);
	,, )
	
	GAME_PROPERTY_6(commander, "Commander", "commander", 0,
		longName, std::string,,
		shortName, std::string,,
		icon, std::string,,
		description, std::string,,
		portrait, std::string,,
		theme, std::string,,
	,, )
}

#define COMMA ,

namespace awe {
	template<typename T>
	class bank : public engine::script_registrant, public engine::json_script {
		std::shared_ptr<engine::scripts> _scripts;
	public:
		/**
		 * The type of the container used to store game property values.
		 */
		using container = nlohmann::ordered_map<std::string, std::shared_ptr<T>>;

		/**
		 * The type of game property stored in this bank.
		 */
		using type = T;

		/**
		 * Provides script interface details to this @c bank instance.
		 * @param scripts Pointer to the @c scripts object to register this bank
		 *                with. If @c nullptr, the bank won't be registered with
		 *                any script interface.
		 * @param data    The data to initialise the logger object with.
		 */
		bank(const std::shared_ptr<engine::scripts>& scripts,
			const engine::logger::data& data) :
			engine::json_script({ data.sink, "json_script" }), _scripts(scripts), _logger(data) {
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
		class base_iterator : public engine::script_reference_type<awe::bank<T>::base_iterator<itr_type, J>> {
			itr_type _itr;
			inline base_iterator<itr_type, J>* opAssign(const base_iterator<itr_type, J>* const o) {
				return &(*this = *o);
			}
			inline bool opEquals(const base_iterator<itr_type, J>* const o) const {
				return *this == *o;
			}
			inline base_iterator<itr_type, J>* opPreInc() {
				return &(++*this);
			}
			inline base_iterator<itr_type, J>* opPostInc() {
				base_iterator<itr_type, J> copyItr(*this);
				++*this;
				copyItr.AddRef();
				return &copyItr;
			}
			inline base_iterator<itr_type, J>* opPreDec() {
				return &(--*this);
			}
			inline base_iterator<itr_type, J>* opPostDec() {
				base_iterator<itr_type, J> copyItr(*this);
				--*this;
				copyItr.AddRef();
				return &copyItr;
			}
			inline base_iterator<itr_type, J>* opAdd(const sf::Int64 ad) const {
				base_iterator<itr_type, J> res(*this + ad);
				res.AddRef();
				return &res;
			}
			inline base_iterator<itr_type, J>* opSub(const sf::Int64 rm) const {
				base_iterator<itr_type, J> res(*this - rm);
				res.AddRef();
				return &res;
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
				const char* itrPostfix = nullptr;
				if constexpr (std::is_const<J>::value) {
					itrPostfix = "ConstItr";
				} else {
					itrPostfix = "Itr";
				}
				const char* const tc = t.append(itrPostfix).c_str();
				auto r = engine::script_reference_type<awe::bank<T>::base_iterator<itr_type, J>>::RegisterType(engine, tc,
					[](asIScriptEngine* engine, const std::string& type) {});
				r = engine->RegisterObjectMethod(tc, std::string(tc)
					.append("@ opAssign(const ").append(tc).append("@ const)").c_str(),
					asMETHOD(base_iterator<itr_type COMMA J>, opAssign),
					asCALL_THISCALL);
				r = engine->RegisterObjectMethod(tc, std::string("bool opEquals(const ").append(tc)
					.append("@ const) const").c_str(),
					asMETHOD(base_iterator<itr_type COMMA J>, opEquals),
					asCALL_THISCALL);
				r = engine->RegisterObjectMethod(tc, std::string(tc)
					.append("@ opPreInc()").c_str(),
					asMETHOD(base_iterator<itr_type COMMA J>, opPreInc),
					asCALL_THISCALL);
				r = engine->RegisterObjectMethod(tc, std::string(tc)
					.append("@ opPostInc()").c_str(),
					asMETHOD(base_iterator<itr_type COMMA J>, opPostInc),
					asCALL_THISCALL);
				r = engine->RegisterObjectMethod(tc, std::string(tc)
					.append("@ opPreDec()").c_str(),
					asMETHOD(base_iterator<itr_type COMMA J>, opPreDec),
					asCALL_THISCALL);
				r = engine->RegisterObjectMethod(tc, std::string(tc)
					.append("@ opPostDec()").c_str(),
					asMETHOD(base_iterator<itr_type COMMA J>, opPostDec),
					asCALL_THISCALL);
				r = engine->RegisterObjectMethod(tc, std::string(tc)
					.append("@ opAdd(const int64) const").c_str(),
					asMETHOD(base_iterator<itr_type COMMA J>, opAdd),
					asCALL_THISCALL);
				r = engine->RegisterObjectMethod(tc, std::string(tc)
					.append("@ opSub(const int64) const").c_str(),
					asMETHOD(base_iterator<itr_type COMMA J>, opSub),
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
		using iterator = base_iterator<typename container::iterator, T>;
		using const_iterator = base_iterator<typename container::const_iterator, const T>;

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
	private:
		inline bank<T>::const_iterator* cbeginScript() const {
			bank<T>::const_iterator b(cbegin());
			b.AddRef();
			return &b;
		}
		inline bank<T>::const_iterator* cendScript() const {
			bank<T>::const_iterator b(cend());
			b.AddRef();
			return &b;
		}
	public:
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
			engine::RegisterColourType(engine, document);
			engine::RegisterVectorTypes(engine, document);
			engine::RegisterTimeTypes(engine, document);
			engine::RegisterRectTypes(engine, document);
			awe::overrides::Register(engine, document);

			// 2. Register the game property type that this bank stores (i.e. T).
			auto r = engine->RegisterObjectType(T::type.c_str(), 0,
				asOBJ_REF | asOBJ_NOCOUNT);
			T::Register(engine, document);

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
				.append("@ begin() const").c_str(),
				asMETHOD(awe::bank<T>, cbeginScript), asCALL_THISCALL);
			r = engine->RegisterObjectMethod(bankType, std::string(itrType)
				.append("@ end() const").c_str(),
				asMETHOD(awe::bank<T>, cendScript), asCALL_THISCALL);

			// 5. Register the global point of access to the bank object.
			const auto globalProperty = std::string(bankTypeName).append(" ")
				.append(T::global_property);
			engine->RegisterGlobalProperty(globalProperty.c_str(), this);
		}

		// We should cache scripts.
		template<typename... Os>
		void processOverrides(const std::shared_ptr<engine::scripts>& scripts,
			const std::unordered_map<std::string,
			std::vector<std::pair<asUINT, std::vector<std::string>>>>&vars,
			const std::unordered_map<std::string,
			std::vector<std::pair<asIScriptFunction*, std::vector<std::string>>>>&funcs,
			Os... banks) {
			std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string,
				std::variant<asUINT, asIScriptFunction*>>>>> overrideValues;
			// 1. Go through each override bank and extract valid override vars and funcs.
			processOverridesForBanks(overrideValues, scripts, vars, funcs, banks...);
			// 2. Calculate overrides and store them in each field.
			for (auto& gameProperty : *this) {
				for (const auto& field : T::fields) {
					if (!T::isFieldOverrideable(field)) continue;
					std::any defaultValue = gameProperty.getFieldDefaultValue(field);
					awe::overrides overrides;
					calculateOverride(
						scripts,
						gameProperty,
						field,
						overrides,
						defaultValue,
						overrideValues,
						banks...
					);
				}
			}
		}

		// Terminate as no more banks left.
		void calculateOverride(
			const std::shared_ptr<engine::scripts>& scripts,
			T& gameProperty,
			const std::string& field,
			awe::overrides& overrides,
			std::any& parent,
			const std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string,
				std::variant<asUINT, asIScriptFunction*>>>>>& overrideValues
		) {
			// 4. Store the override.
			gameProperty.setFieldValue(field, parent, overrides);
		}

		template<typename O, typename... Os>
		void calculateOverride(
			const std::shared_ptr<engine::scripts>& scripts,
			T& gameProperty,
			const std::string& field,
			awe::overrides& overrides,
			std::any& parent,
			const std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string,
				std::variant<asUINT, asIScriptFunction*>>>>>& overrideValues,
			const O& bank,
			Os... banks
		) {
			// If there's no overrides for this game property, return early.
			if (!overrideValues.count(gameProperty.scriptName())) {
				return;
			}
			// If there's no overrides for this field, return early.
			if (!overrideValues.at(gameProperty.scriptName()).count(field)) {
				return;
			}
			// If there's no overrides for this layer in the hierarchy, traverse to the next one.
			if (!overrideValues.at(gameProperty.scriptName()).at(field).count(O::type::type)) {
				calculateOverride(
					scripts,
					gameProperty,
					field,
					overrides,
					parent,
					overrideValues,
					banks...
				);
				return;
			}
			for (const auto& overrider : overrideValues.at(gameProperty.scriptName()).at(field).at(O::type::type)) {
				// 1. If this isn't the else branch, apply the override.
				std::any depthCopy(parent);
				if (!overrider.first.empty()) {
					if (const asUINT* const var = std::get_if<asUINT>(&overrider.second)) {
						// Global variable, replace value.
						depthCopy = T::readFieldOverrideVariable(field, scripts, *var);
					} else if (asIScriptFunction* const* const func = std::get_if<asIScriptFunction*>(&overrider.second)) {
						// Global function, perform code on value.
						depthCopy = T::readFieldOverrideFunction(field, scripts, *func, depthCopy);
					} else {
						assert(false);
					}
				}
				// 2. Create copy of overrides, and apply this overrider's script name.
				awe::overrides depthCopyOverrides(overrides);
				depthCopyOverrides[O::type::overrideID] = overrider.first;
				// 3. Make recursive call.
				calculateOverride(
					scripts,
					gameProperty,
					field,
					depthCopyOverrides,
					depthCopy,
					overrideValues,
					banks...
				);
			}
		}

		// Terminate as no more banks left.
		void processOverridesForBanks(std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string,
				std::variant<asUINT, asIScriptFunction*>>>>>&overrideValues,
			const std::shared_ptr<engine::scripts>& scripts,
			const std::unordered_map<std::string,
			std::vector<std::pair<asUINT, std::vector<std::string>>>>&vars,
			const std::unordered_map<std::string,
			std::vector<std::pair<asIScriptFunction*, std::vector<std::string>>>>&funcs) {}

		/**
		 * Goes through all of the overrides configured for this bank and stores
		 * them.
		 * @param scripts The scripts engine containing the overrides.
		 */
		template<typename O, typename... Os>
		void processOverridesForBanks(std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string,
				std::variant<asUINT, asIScriptFunction*>>>>>& overrideValues,
			const std::shared_ptr<engine::scripts>& scripts,
			const std::unordered_map<std::string,
			std::vector<std::pair<asUINT, std::vector<std::string>>>>& vars,
			const std::unordered_map<std::string,
			std::vector<std::pair<asIScriptFunction*, std::vector<std::string>>>>& funcs,
			const O& bank,
			Os... banks) {
			// vars and funcs will be within four namespaces.
			if (vars.count(T::type)) {
				for (const auto& var : vars.at(T::type)) {
					// 1. Is the base type script name valid?
					std::string baseTypeScriptName = var.second[1];
					// Ideally, I would never have had script names that begin with
					// digits. But with so many of my original tile type script names
					// starting with hex digits, the easiest way to support them is
					// to prepend them with an underscore that's then removed, as
					// opposed to fixing them everywhere (scripts, JSON, map files...).
					if (baseTypeScriptName[0] == '_')
						baseTypeScriptName = baseTypeScriptName.substr(1);
					if (!contains(baseTypeScriptName)) {
						_logger.error("Attempting to override fields within game "
							"property \"{}\" of type \"{}\", the former of which "
							"does not exist.", baseTypeScriptName, T::type);
						continue;
					}

					// 2. Is the overrider type valid? Does it exist and is it
					//    within the acceptable range of the hierarchy?
					const auto& overriderType = var.second[2];
					if (overriderType != O::type::type) {
						// Tiny UX problem: we can't tell here if it's actually an
						// invalid overrider type, or if it's just a type we haven't
						// visited yet (i.e. it's within Os... banks). To warn of
						// these, we'll have to maintain a separate list of unused
						// entries and report them all later.
						continue;
					}

					// 3. Is the overrider type script name valid?
					std::string overriderTypeScriptName = var.second[3];
					if (overriderTypeScriptName[0] == '_')
						overriderTypeScriptName = overriderTypeScriptName.substr(1);
					if (!bank.contains(overriderTypeScriptName)) {
						_logger.error("Attempting to override fields within game "
							"property \"{}\" of type \"{}\", with game property "
							"\"{}\" of overrider type \"{}\". The overrider game "
							"property \"{}\" does not exist.", baseTypeScriptName,
							T::type, overriderTypeScriptName, O::type::type,
							overriderTypeScriptName);
						continue;
					}

					// 4. Does the variable name match a field name?
					std::string varName; int varTypeID = 0;
					if (!scripts->getGlobalVariable(
						engine::scripts::modules[engine::scripts::BANK_OVERRIDE],
						var.first, varName, varTypeID)) {
						_logger.error("Will not apply override from "
							"{}::{}::{}::{}.", T::type, baseTypeScriptName,
							overriderType, overriderTypeScriptName);
						continue;
					}
					if (!T::hasField(varName)) {
						_logger.error("Attempting to override non-existent field "
							"\"{}\" within game property \"{}\" of type \"{}\", "
							"with game property \"{}\" of overrider type \"{}\".",
							varName, baseTypeScriptName, T::type,
							overriderTypeScriptName, O::type::type);
						continue;
					}
					if (!T::isFieldOverrideable(varName)) {
						_logger.error("Attempting to override non-overrideable "
							"field \"{}\" within game property \"{}\" of type "
							"\"{}\", with game property \"{}\" of overrider type "
							"\"{}\".", varName, baseTypeScriptName, T::type,
							overriderTypeScriptName, O::type::type);
						continue;
					}

					// For each game property type, we could define a map, e.g.:
					// weather["longName"] = <references> weather.longName
					// Then for type...
					// weather["longName"].ANGEL_SCRIPT_TYPE = "string"
					// To assign "string" within the macro, let's use a new set of
					// template specialisations.

					// Actually, because they're all different types, we can't
					// easily map each field like that (actually we could use
					// inheritance if we really wanted to).
					// constexpr weather.getFieldAngelScriptType() == "string"
					// weather.readFieldOverrideVariable()
					// weather.readFieldOverrideFunction()

					// Next big problem though is how we're going to convert the
					// AS object values into C++ values to assign to the overrides.
					// Could make use of std::any?
					// Vars:: module->GetAddressOfGlobalVar(asUINT).
					//        Cast pointer, dereference.
					// Funcs: _scripts->callFunction(inter.Var).
					//        intermediate variable is of the correct type.
					// Could do template specialisations again for each, it's not
					// a lot of lines.
					// To support that though, we're going to have to assign the
					// functions to the property_field in the same way as ANGEL_SCRIPT_TYPE.
					// We have to do it this way because I don't think it's possible
					// to access the actual typename from the field, you can only do
					// so from the field type itself. E.g. you can't do this:
					// weather.longName::type
					// Or
					// weather["longName"]::type
					// It might be possible to compile each property_field subtype
					// in a type list somehow, and then use a parameter pack to go
					// through each property and access the actual C++ types that
					// way, but I'm not clever enough to figure it out, and this
					// approach should work anyway.

					// 5. Does the variable type match the field's type?
					const auto actualType = T::getFieldAngelScriptType(varName);
					const auto varType = scripts->getTypeName(varTypeID);
					if (varType != actualType) {
						_logger.error("Attempting to override field \"{}\" of "
							"type \"{}\" within game property \"{}\" of type "
							"\"{}\", with game property \"{}\" of overrider type "
							"\"{}\". The type you gave was \"{}\".",
							varName, actualType, baseTypeScriptName, T::type,
							overriderTypeScriptName, O::type::type, varType);
						continue;
					}

					// 6. All checks pass, store the override.
					overrideValues[baseTypeScriptName][varName][overriderType][overriderTypeScriptName] =
						var.first;
					// And make sure the else branch is stored.
					overrideValues[baseTypeScriptName][varName][overriderType][""] = {};
				}
			}
			if (funcs.count(T::type)) {
				for (const auto& func : funcs.at(T::type)) {
					// 1. Is the base type script name valid?
					std::string baseTypeScriptName = func.second[1];
					if (baseTypeScriptName[0] == '_')
						baseTypeScriptName = baseTypeScriptName.substr(1);
					if (!contains(baseTypeScriptName)) {
						_logger.error("Attempting to override fields within game "
							"property \"{}\" of type \"{}\", the former of which "
							"does not exist.", baseTypeScriptName, T::type);
						continue;
					}

					// 2. Is the overrider type valid? Does it exist and is it
					//    within the acceptable range of the hierarchy?
					const auto& overriderType = func.second[2];
					if (overriderType != O::type::type) {
						// Tiny UX problem: we can't tell here if it's actually an
						// invalid overrider type, or if it's just a type we haven't
						// visited yet (i.e. it's within Os... banks). To warn of
						// these, we'll have to maintain a separate list of unused
						// entries and report them all later.
						continue;
					}

					// 3. Is the overrider type script name valid?
					std::string overriderTypeScriptName = func.second[3];
					if (overriderTypeScriptName[0] == '_')
						overriderTypeScriptName = overriderTypeScriptName.substr(1);
					if (!bank.contains(overriderTypeScriptName)) {
						_logger.error("Attempting to override fields within game "
							"property \"{}\" of type \"{}\", with game property "
							"\"{}\" of overrider type \"{}\". The overrider game "
							"property \"{}\" does not exist.", baseTypeScriptName,
							T::type, overriderTypeScriptName, O::type::type,
							overriderTypeScriptName);
						continue;
					}

					// 4. Does the function name match a field name?
					std::string funcName(func.first->GetName());
					if (!T::hasField(funcName)) {
						_logger.error("Attempting to override non-existent field "
							"\"{}\" within game property \"{}\" of type \"{}\", "
							"with game property \"{}\" of overrider type \"{}\".",
							funcName, baseTypeScriptName, T::type,
							overriderTypeScriptName, O::type::type);
						continue;
					}
					if (!T::isFieldOverrideable(funcName)) {
						_logger.error("Attempting to override non-overrideable "
							"field \"{}\" within game property \"{}\" of type "
							"\"{}\", with game property \"{}\" of overrider type "
							"\"{}\".", funcName, baseTypeScriptName, T::type,
							overriderTypeScriptName, O::type::type);
						continue;
					}
					
					// 5. Does the function signature match the field's type?
					const auto actualType = T::getFieldAngelScriptType(funcName);
					std::string funcSignature(func.first->GetDeclaration(true, true, true));
					if (func.first->GetParamCount() != 1) {
						_logger.error("Attempting to override field \"{}\" of "
							"type \"{}\" within game property \"{}\" of type "
							"\"{}\", with game property \"{}\" of overrider type "
							"\"{}\", with function \"{}\". The function must have "
							"only one parameter of type \"{}&[out]\".",
							funcName, actualType, baseTypeScriptName, T::type,
							overriderTypeScriptName, O::type::type,
							funcSignature, actualType);
						continue;
					}
					int typeID = 0; asDWORD mods = 0;
					func.first->GetParam(0, &typeID, &mods);
					const auto funcType = scripts->getTypeName(typeID);
					if (funcType != actualType || !(mods & asETypeModifiers::asTM_OUTREF) || !(mods & asETypeModifiers::asTM_INOUTREF)) {
						_logger.error("Attempting to override field \"{}\" of "
							"type \"{}\" within game property \"{}\" of type "
							"\"{}\", with game property \"{}\" of overrider type "
							"\"{}\", with function \"{}\". The function must have "
							"only one parameter of type \"{}&[out]\".",
							funcName, actualType, baseTypeScriptName, T::type,
							overriderTypeScriptName, O::type::type,
							funcSignature, actualType);
						continue;
					}

					// 6. All checks pass, store the override.
					overrideValues[baseTypeScriptName][funcName][overriderType][overriderTypeScriptName] =
						func.first;
					// And make sure the else branch is stored.
					overrideValues[baseTypeScriptName][funcName][overriderType][""] = {};
				}
			}

			processOverridesForBanks(overrideValues, scripts, vars, funcs, banks...);

			//// Function overrides will always replace any variable overrides, if
			//// they're for the same game property.
			//const auto varMetadata =
			//	scripts->getGlobalVariableMetadata(
			//		engine::scripts::modules[engine::scripts::BANK_OVERRIDE]);
			//for (const auto& var : varMetadata) {
			//	// Find out if the base metadata exists, and matches this game
			//	// property type. If not, skip this entry.
			//	// Let's use T::type instead of T::global_property (implied in my
			//	// notes), but make it case insensitive. Make use of
			//	// tgui::String::equalIgnoreCase().
			//
			//	// To map overrider type to awe::overrides index, let's add
			//	// T::override_index.
			//
			//	// May need to rethink how I'm storing overrides. I'll need to do
			//	// them in order of hierarchy, and this will be especially
			//	// important for functions, which will accept the value that
			//	// immediately precedes the level of the hierarchy given in the
			//	// metadata.
			//
			//	// Just realised this is super inefficient. We'd only want to go
			//	// through these lists once. We we'll need to take this method out
			//	// and instead write a class method that processes just one
			//	// override, that this method will invoke.
			//}
			//
			//const auto funcMetadata =
			//	scripts->getGlobalFunctionMetadata(
			//		engine::scripts::modules[engine::scripts::BANK_OVERRIDE]);
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
			container bank;
			nlohmann::ordered_json jj = j.nlohmannJSON();
			for (auto& i : jj.items()) {
				engine::json input(i.value(), { _logger.getData().sink, "json" });
				bank[i.key()] = std::make_shared<T>(i.key(), input, _logger, _scripts);
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
		container _bank;
	};
}

/* Overrides are applied in scripts like so:
namespace BaseType {
	namespace BaseScriptName {
		namespace OverriderType {
			namespace OverriderScriptName {
				const string tile = "oscity"; // Prefer const...
				string tile = "oscity";       // ...but technically it doesn't matter.
				void vision(uint& parent) {
					parent + 2;
				}
			}
		}
	}
}
*/

namespace awe {
	template<typename T>
	void filterOnBaseType(const std::unordered_map<T, std::vector<std::string>>& in,
		std::unordered_map<std::string, std::vector<std::pair<T, std::vector<std::string>>>>& out) {
		for (const auto& i : in) {
			if (i.second.size() == 4) {
				out[i.second[0]].push_back(i);
				// Idea: could find a way to warn user if func or var within four namespaces
				//       isn't used.
			} else {
				// Log warning.
				// TODO: could use metadata here, [helper], to tell the engine that
				// it's intended to be excluded from the override calculations (at least directly).
			}
		}
	}

	/**
	 * Scans the \c BANK_OVERRIDE script module and applies overrides accordingly.
	 */
	template<typename O, typename... Os>
	void processOverrides(const std::shared_ptr<engine::scripts>& scripts,
		O& bank, Os... banks) {
		// 1. Filter each global variable and global function on base type.
		auto vars = scripts->getGlobalVariablesAndTheirNamespaces(
			engine::scripts::modules[engine::scripts::BANK_OVERRIDE]);
		std::unordered_map<std::string, std::vector<std::pair<asUINT, std::vector<std::string>>>> filteredVars;
		filterOnBaseType(vars, filteredVars);
		auto funcs = scripts->getGlobalFunctionsAndTheirNamespaces(
			engine::scripts::modules[engine::scripts::BANK_OVERRIDE]);
		std::unordered_map<std::string, std::vector<std::pair<asIScriptFunction*, std::vector<std::string>>>> filteredFuncs;
		filterOnBaseType(funcs, filteredFuncs);
		// 2. Go through each bank, and process every valid override.
		bank.processOverrides(scripts, filteredVars, filteredFuncs, banks...);
		//commanders->processOverrides(scripts, filteredVars, filteredFuncs);
		//weathers->processOverrides(scripts, filteredVars, filteredFuncs, commanders);
		//environments->processOverrides(scripts, filteredVars, filteredFuncs, weathers, commanders);
		// Etc.
	}
}
