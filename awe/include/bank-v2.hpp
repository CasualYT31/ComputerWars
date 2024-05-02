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
#include "TGUI/String.hpp"
#include "script.hpp"
#include "maths.hpp"
#include "typedef.hpp"
#include "tpp/pod.tpp"

#define GAME_PROPERTY_COUNT 10

namespace awe {
	class weapon;
	class unit_type;
	class terrain;
	class tile_type;
	class structure;
	class movement_type;
	class country;
	class environment;
	class weather;
	class commander;
	template<typename T>
	inline constexpr std::size_t hierarchy_index() {
		static_assert(false,
			"Game property class must be given a hierarchy index");
	}
	template<>
	inline constexpr std::size_t hierarchy_index<weapon>() { return 9; }
	template<>
	inline constexpr std::size_t hierarchy_index<unit_type>() { return 8; }
	template<>
	inline constexpr std::size_t hierarchy_index<terrain>() { return 7; }
	template<>
	inline constexpr std::size_t hierarchy_index<tile_type>() { return 6; }
	template<>
	inline constexpr std::size_t hierarchy_index<structure>() { return 5; }
	template<>
	inline constexpr std::size_t hierarchy_index<movement_type>() { return 4; }
	template<>
	inline constexpr std::size_t hierarchy_index<country>() { return 3; }
	template<>
	inline constexpr std::size_t hierarchy_index<environment>() { return 2; }
	template<>
	inline constexpr std::size_t hierarchy_index<weather>() { return 1; }
	template<>
	inline constexpr std::size_t hierarchy_index<commander>() { return 0; }
}

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
	static_assert(awe::hierarchy_index<i>() >= 0 && \
		awe::hierarchy_index<i>() < GAME_PROPERTY_COUNT, \
		"awe::hierarchy_index<i>() must be within the game property count!"); \
	inline overrides& n(const std::string& newValue) { \
		_overrides[awe::hierarchy_index<i>()] = newValue; \
		return *this; \
	} \
	inline std::string& n() { \
		return _overrides[awe::hierarchy_index<i>()]; \
	} \
	inline const std::string& n() const { \
		return _overrides.at(awe::hierarchy_index<i>()); \
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
		inline decltype(_overrides)::iterator begin() noexcept {
			return _overrides.begin();
		}
		inline decltype(_overrides)::iterator end() noexcept {
			return _overrides.end();
		}
		inline decltype(_overrides)::const_iterator begin() const noexcept {
			return _overrides.begin();
		}
		inline decltype(_overrides)::const_iterator end() const noexcept {
			return _overrides.end();
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
		BANK_OVERRIDE_FIELD(weapon, awe::weapon)
		BANK_OVERRIDE_FIELD(unitType, awe::unit_type)
		BANK_OVERRIDE_FIELD(terrain, awe::terrain)
		BANK_OVERRIDE_FIELD(tileType, awe::tile_type)
		BANK_OVERRIDE_FIELD(structure, awe::structure)
		BANK_OVERRIDE_FIELD(movementType, awe::movement_type)
		BANK_OVERRIDE_FIELD(country, awe::country)
		BANK_OVERRIDE_FIELD(environment, awe::environment)
		BANK_OVERRIDE_FIELD(weather, awe::weather)
		BANK_OVERRIDE_FIELD(commander, awe::commander)
	};
	std::function<void(awe::overrides&)> awe::overrides::_factory = {};

	inline bool operator==(const awe::overrides& lhs, const awe::overrides& rhs) {
		return lhs._overrides == rhs._overrides;
	}
}

namespace std {
	template<> struct hash<awe::overrides> {
		std::size_t operator()(awe::overrides const& f) const noexcept {
			return engine::combinationHasher<std::string>(f);
		}
	};
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
		using type = T;
		// WARNING: to initialise a property field of type bank_array correctly,
		//          you must use the INIT_BANK_ARRAY() macro!
		bank_array() = default;
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
			scripts = o.scripts;
			CScriptArray* newArray = scripts->createArray(engine::script_type<T>());
			*newArray = *o.array->operator->();
			array = std::make_unique<engine::CScriptWrapper<CScriptArray>>(newArray);
			// The = operator is used on a bank_array when applying overrides.
			// Calling initVector() will ensure that the vector is a copy of the
			// script array as and when overrides are applied.
			initVector();
			return *this;
		}
		std::shared_ptr<engine::scripts> scripts;
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
			array->operator->()->AddRef();
			return array->operator->();
		}
	};
}

namespace awe {
	template<typename T> struct Serialisable {
		static bool fromJSON(T& value, engine::json& j,
			const engine::json::KeySequence& keys, engine::logger&,
			const bool optional) {
			j.apply(value, keys, false, optional);
			if (j.inGoodState()) return true;
			const bool doesntExist = j.whatFailed() &
				engine::json_state::KEYS_DID_NOT_EXIST;
			j.resetState();
			// If the keys didn't exist, but this is an optional field, do not
			// propagate any indication of an error (i.e. return true).
			return doesntExist && optional;
		}
	};
	template<> struct Serialisable<sf::Color> {
		static bool fromJSON(sf::Color& value, engine::json& j,
			const engine::json::KeySequence& keys, engine::logger&,
			const bool optional) {
			j.applyColour(value, keys, false, optional);
			if (j.inGoodState()) return true;
			const bool doesntExist = j.whatFailed() &
				engine::json_state::KEYS_DID_NOT_EXIST;
			j.resetState();
			// If the keys didn't exist, but this is an optional field, do not
			// propagate any indication of an error (i.e. return true).
			return doesntExist && optional;
		}
	};
	template<typename T> struct Serialisable<sf::Vector2<T>> {
		static bool fromJSON(sf::Vector2<T>& value, engine::json& j,
			const engine::json::KeySequence& keys, engine::logger&,
			const bool optional) {
			std::array<T, 2> vec;
			j.applyArray(vec, keys, optional);
			if (!j.inGoodState()) {
				const bool doesntExist = j.whatFailed() &
					engine::json_state::KEYS_DID_NOT_EXIST;
				j.resetState();
				// If the keys didn't exist, but this is an optional field, do not
				// propagate any indication of an error (i.e. return true).
				return doesntExist && optional;
			}
			value.x = vec[0];
			value.y = vec[1];
			return true;
		}
	};
	template<> struct Serialisable<sf::Time> {
		static bool fromJSON(sf::Time& value, engine::json& j,
			const engine::json::KeySequence& keys, engine::logger& logger,
			const bool optional) {
			sf::Uint32 ms = 0;
			j.apply(ms, keys, false, optional);
			if (!j.inGoodState()) {
				const bool doesntExist = j.whatFailed() &
					engine::json_state::KEYS_DID_NOT_EXIST;
				j.resetState();
				// If the keys didn't exist, but this is an optional field, do not
				// propagate any indication of an error (i.e. return true).
				return doesntExist && optional;
			}
			if (ms > static_cast<sf::Uint32>(std::numeric_limits<sf::Int32>::max())) {
				logger.error("Cannot store a millisecond value {} at {} that is "
					"greater than {}.", ms, j.synthesiseKeySequence(keys),
					std::numeric_limits<sf::Int32>::max());
				return false;
			}
			value = sf::milliseconds(static_cast<sf::Int32>(ms));
			return true;
		}
	};
	template<typename E> struct Serialisable<awe::bank_array<E>> {
		static bool fromJSON(awe::bank_array<E>& value, engine::json& j,
			const engine::json::KeySequence& keys, engine::logger& logger,
			const bool optional) {
			nlohmann::ordered_json p;
			if (!j.keysExist(keys, &p)) {
				if (!optional) {
					logger.error("Attempting to read {}: these keys do not exist.",
						j.synthesiseKeySequence(keys));
					return false;
				} else {
					return true;
				}
			}
			if (!p.is_array()) {
				logger.error("Attempting to read {} as an array, but the value at "
					"these keys is of type \"{}\".", j.synthesiseKeySequence(keys),
					j.getTypeName(p));
				return false;
			}
			if (p.empty()) return true;
			bool thereWasAnError = false;
			value.array->operator->()->Resize(static_cast<asUINT>(p.size()));
			for (asUINT i = 0, end = value.array->operator->()->GetSize(); i < end; ++i) {
				// Create an engine::json object for the array value so a fromJSON
				// implementation can get at the value.
				const auto valueKey = "arrayValue" + std::to_string(i);
				nlohmann::json valueObject;
				valueObject[valueKey] = p[i];
				engine::json valueObjectEngine(valueObject, logger.getData());
				E* element = static_cast<E*>(value.array->operator->()->At(i));
				if (!awe::Serialisable<E>::fromJSON(*element,
					valueObjectEngine, engine::json::KeySequence{ valueKey },
					logger, false)) {
					thereWasAnError = true;
					logger.error("The above error refers to object {}.",
						j.synthesiseKeySequence(keys));
				}
			}
			value.initVector();
			return !thereWasAnError;
		}
	};

	//  T = type of field.
	// GP = game property this field is a member of.
	template<typename T, typename GP>
	class property_field {
		static_assert(awe::hierarchy_index<GP>() < GAME_PROPERTY_COUNT,
			"awe::hierarchy_index<GP>() must be within the game property count!");
	public:
		inline property_field(engine::json& j, const std::string& scriptName,
			const engine::json::KeySequence& keys, engine::logger& logger,
			const std::shared_ptr<engine::scripts>& scripts, const bool optional,
			const std::function<void(T&, const std::shared_ptr<engine::scripts>&)>&
				defaultValue) {
			for (auto& i : _scriptNamesWithOverrides) i.insert("");
			// Allow the caller to initialise the field with a default value in
			// case the value couldn't be serialised from the JSON.
			defaultValue(_values[{}], scripts);
			if (!awe::Serialisable<T>::fromJSON(_values[{}], j, keys, logger,
				optional)) {
				logger.error("The above error refers to object \"{}\".",
					scriptName);
			}
		}
		inline typename boost::call_traits<T>::reference operator[](
			const overrides& overrides) {
			for (std::size_t i = 0; i < awe::hierarchy_index<GP>(); ++i)
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
			for (std::size_t i = 0; i < awe::hierarchy_index<GP>(); ++i)
				if (_scriptNamesWithOverrides[i].count(o[i]) > 0)
					result[i] = o[i];
			return result;
		}
		std::array<std::unordered_set<std::string>, awe::hierarchy_index<GP>()>
			_scriptNamesWithOverrides;
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

	template<typename T> struct is_bank_array : std::false_type {};
	template<typename T> struct is_bank_array<bank_array<T>> : std::true_type {};
	template<typename T> struct OverrideFunction {
		static std::any read(const std::shared_ptr<engine::scripts>& scripts,
			asIScriptFunction* const function, const std::any& parent) {
			if constexpr (is_bank_array<T>::value) {
				// Would've preferred to use a template specialisation here,
				// but I can't find a way to specialise a template struct with one
				// template parameter with another template struct that has one
				// template parameter (i.e. the T given to bank_array). There's
				// probably a way but I can't Google well enough.
				T p = std::any_cast<T>(parent);
				p.array->operator->()->AddRef();
				scripts->callFunction(function, &p);
				return p;
			} else {
				T p = std::any_cast<T>(parent);
				scripts->callFunction(function, &p);
				return p;
			}
		}
	};
	// If a type needs a special read method for functions, specialise here.
}

// TODO: AngelScript and C++ member-wise constructors need to have default values
// for optional parameters.
// this will mean optional members must come after all mandatory members.
// This is not straight-forward for non-basic types.

/**
 * Data used to setup \c awe::random_particles::data objects.
 * @sa \c awe::random_particles::data.
 */
DECLARE_POD_5(awe, particle_data, "ParticleData",
	std::string, sheet, true, {},
	std::string, spriteID, true, {},
	float, density, true, 0.0f,
	sf::Vector2f, vector, true, {},
	sf::Time, respawnDelay, true, {}
);
	
DEFINE_POD_5(awe, particle_data, "ParticleData",
	std::string, sheet,
	std::string, spriteID,
	float, density,
	sf::Vector2f, vector,
	sf::Time, respawnDelay
);

/**
 * Configuration of a structure's root tile.
 */
DECLARE_POD_3(awe, root_structure_tile, "RootStructureTile",
	std::string, tile, false, {},
	std::string, destroyed, true, {},
	std::string, deleted, true, {}
);

DEFINE_POD_3(awe, root_structure_tile, "RootStructureTile",
	std::string, tile,
	std::string, destroyed,
	std::string, deleted
);

/**
 * Configuration of a structure's dependent tile.
 */
DECLARE_POD_4(awe, dependent_structure_tile, "DependentStructureTile",
	std::string, tile, false, {},
	sf::Vector2i, offset, false, {},
	std::string, destroyed, true, {},
	std::string, deleted, true, {}
);

DEFINE_POD_4(awe, dependent_structure_tile, "DependentStructureTile",
	std::string, tile,
	sf::Vector2i, offset,
	std::string, destroyed,
	std::string, deleted
);

/**
 * Configures a unit's map sprite.
 */
DECLARE_POD_7(awe, unit_sprite_info, "UnitSpriteInfo",
	std::string, idleSheet, false, {},
	std::string, upSheet, false, {},
	std::string, downSheet, false, {},
	std::string, leftSheet, false, {},
	std::string, rightSheet, false, {},
	std::string, selectedSheet, true, {},
	std::string, sprite, true, {}
);

DEFINE_POD_7(awe, unit_sprite_info, "UnitSpriteInfo",
	std::string, idleSheet,
	std::string, upSheet,
	std::string, downSheet,
	std::string, leftSheet,
	std::string, rightSheet,
	std::string, selectedSheet,
	std::string, sprite
);

/**
 * Configures a unit's sounds.
 */
DECLARE_POD_5(awe, unit_sound_info, "UnitSoundInfo",
	std::string, move, false, {},
	std::string, destroy, false, {},
	std::string, moveHidden, true, {},
	std::string, hide, true, {},
	std::string, unhide, true, {}
);

DEFINE_POD_5(awe, unit_sound_info, "UnitSoundInfo",
	std::string, move,
	std::string, destroy,
	std::string, moveHidden,
	std::string, hide,
	std::string, unhide
);

/**
 * Describes the base damage a weapon deals against a single type of unit or terrain.
 * A negative integer means the weapon can't attack the target if it's visible or
 * hidden, depending on the field.
 */
DECLARE_POD_3(awe, weapon_damage, "WeaponDamage",
	std::string, target, false, {},
	sf::Int32, damage, false, 0,
	sf::Int32, damageWhenHidden, true, -1
)

DEFINE_POD_3(awe, weapon_damage, "WeaponDamage",
	std::string, target,
	sf::Int32, damage,
	sf::Int32, damageWhenHidden
)

// SPECIALISE BANK_ARRAY SCRIPT TYPENAMES HERE!
// TODO: Hopefully we can find a way round this, suffers from the same problem as:
// OverrideFunction::read()

template<>
inline constexpr std::string engine::script_type<awe::bank_array<awe::particle_data>>() {
	return std::string(engine::script_type<awe::particle_data>()).append("Array");
}

template<>
inline constexpr std::string engine::script_type<awe::bank_array<awe::dependent_structure_tile>>() {
	return std::string(engine::script_type<awe::dependent_structure_tile>()).append("Array");
}

template<>
inline constexpr std::string engine::script_type<awe::bank_array<std::string>>() {
	return std::string(engine::script_type<std::string>()).append("Array");
}

template<>
inline constexpr std::string engine::script_type<awe::bank_array<awe::weapon_damage>>() {
	return std::string(engine::script_type<awe::weapon_damage>()).append("Array");
}

#include "tpp/bank-v2-macros.tpp"

#define COMMA ,

namespace awe {
	/**
	 * Describes the different types of visibility properties that a terrain
	 * can have during Fog of War.
	 */
	enum class fow_visibility {
		/**
		 * Tiles of this terrain will be visible to a team if they occupy them,
		 * own them, or if it is within at least one of their units' vision
		 * ranges.
		 */
		Normal,

		/**
		 * Tiles of this terrain will be invisible to a team, unless they
		 * occupy them, own them, or have at least one of their units directly
		 * adjacent to them.
		 */
		Hidden,

		/**
		 * Tiles of this terrain will always be visible to every team.
		 */
		Visible
	};
}

template<>
inline constexpr std::string engine::script_type<awe::fow_visibility>() {
	return "FOWVisibility";
}

namespace awe {
	GAME_PROPERTY_11(weapon, "Weapon", "weapon",
		longName, std::string, false, DEFAULT_VALUE(""), ,
		shortName, std::string, false, DEFAULT_VALUE(""), ,
		icon, std::string, false, DEFAULT_VALUE(""), ,
		description, std::string, false, DEFAULT_VALUE(""), ,
		ammo, sf::Int32, false, DEFAULT_VALUE(9), ,
		unitTable, awe::bank_array<awe::weapon_damage>, false, INIT_BANK_ARRAY(), ,
		terrainTable, awe::bank_array<awe::weapon_damage>, true, INIT_BANK_ARRAY(), ,
		range, sf::Vector2u, true, DEFAULT_VALUE(sf::Vector2u(1, 1)), ,
		canAttackAfterMoving, bool, true, DEFAULT_VALUE(true), ,
		canCounterattackDirectly, bool, true, DEFAULT_VALUE(true), ,
		canCounterattackIndirectly, bool, true, DEFAULT_VALUE(false), ,
		awe::weapon_damage::Register(engine, document);
		awe::bank_array<awe::weapon_damage>::Register(engine, document);
	, ,)

	GAME_PROPERTY_23(unit_type, "UnitType", "unittype",
		longName, std::string, false, DEFAULT_VALUE(""), ,
		shortName, std::string, false, DEFAULT_VALUE(""), ,
		description, std::string, false, DEFAULT_VALUE(""), ,
		movementType, std::string, false, DEFAULT_VALUE(""), ,
		movementPoints, sf::Uint32, false, DEFAULT_VALUE(0), ,
		price, sf::Uint32, false, DEFAULT_VALUE(0), ,
		spriteInfo, awe::unit_sprite_info, false, DEFAULT_VALUE({}), ,
		destroyEffectSprite, std::string, false, DEFAULT_VALUE(""), ,
		soundInfo, awe::unit_sound_info, false, DEFAULT_VALUE({}), ,
		picture, std::string, true, DEFAULT_VALUE(""), ,
		capturingSprite, std::string, true, DEFAULT_VALUE(""), ,
		finishedCapturingSprite, std::string, true, DEFAULT_VALUE(""), ,
		maxFuel, sf::Int32, true, DEFAULT_VALUE(99), ,
		maxHP, sf::Uint32, true, DEFAULT_VALUE(10),
		if (operator()() > INT_MAX / HP_GRANULARITY) {
			logger.warning("Max HP of unit type \"{}\" overflowed ({}). "
				"Setting to {}...", scriptName, operator()(),
				static_cast<sf::Uint32>(INT_MAX / HP_GRANULARITY));
			operator()() = static_cast<sf::Uint32>(INT_MAX / HP_GRANULARITY);
		},
			vision, sf::Uint32, true, DEFAULT_VALUE(1), ,
			canLoad, awe::bank_array<std::string>, true, INIT_BANK_ARRAY(), ,
			loadLimit, sf::Uint32, true, DEFAULT_VALUE(0), ,
			canUnloadFrom, awe::bank_array<std::string>, true, INIT_BANK_ARRAY(), ,
			turnStartPriority, sf::Int32, true, DEFAULT_VALUE(1000), ,
			canCapture, awe::bank_array<std::string>, true, INIT_BANK_ARRAY(), ,
			canHide, bool, true, DEFAULT_VALUE(false), ,
			weapons, awe::bank_array<std::string>, true, INIT_BANK_ARRAY(), ,
			ignoresDefence, bool, true, DEFAULT_VALUE(false), ,
			awe::unit_sprite_info::Register(engine, document);
			awe::unit_sound_info::Register(engine, document);
		awe::bank_array<std::string>::Register(engine, document);
		, ,
public:
	/**
	 * The granularity of HP values that this engine works with internally.
	 * This engine calculates health to a finer granularity than 0-10 for units
	 * (I would be surprised if the original games did not do this). Instead,
	 * in the original version of this game, HP is calculated from 0-100, or 0
	 * to whatever the max HP of a unit type is multiplied by this granularity
	 * value. The higher the granularity, the higher the precision of HP
	 * calculations.\n
	 * I chose against using floating point values for HP values to remain as
	 * precise as possible.
	 * @sa awe::unit::getDisplayedHP()
	 */
	static const unsigned int HP_GRANULARITY;

	/**
	 * Converts an internal HP value into a user-friendly one.
	 * @param  hp The internal HP.
	 * @return The user-friendly HP.
	 */
	static inline awe::HP getDisplayedHP(const awe::HP hp) noexcept {
		return (awe::HP)ceil((double)hp /
			(double)awe::unit_type::HP_GRANULARITY);
	}

	/**
	 * Converts a user-friendly HP value into an internal one.
	 * @param  hp The user-friendly HP.
	 * @return The internal HP.
	 */
	static inline awe::HP getInternalHP(const awe::HP hp) noexcept {
		return hp * awe::unit_type::HP_GRANULARITY;
	}
	)
	const unsigned int awe::unit_type::HP_GRANULARITY = 10;

	static void RegisterFOWVisibility(asIScriptEngine* engine, const std::shared_ptr<DocumentationGenerator>& document) {
		const auto type = engine::script_type<awe::fow_visibility>();
		if (engine->GetTypeInfoByName(type.c_str())) return;
		auto r = engine->RegisterEnum(type.c_str());
		r = engine->RegisterEnumValue(type.c_str(), "Normal",
			static_cast<int>(fow_visibility::Normal));
		r = engine->RegisterEnumValue(type.c_str(), "Hidden",
			static_cast<int>(fow_visibility::Hidden));
		r = engine->RegisterEnumValue(type.c_str(), "Visible",
			static_cast<int>(fow_visibility::Visible));
	}

	template<> struct Serialisable<awe::fow_visibility> {
		static bool fromJSON(awe::fow_visibility& value, engine::json& j,
			const engine::json::KeySequence& keys, engine::logger& logger,
			const bool optional) {
			std::string name = "Normal";
			if (awe::Serialisable<std::string>::fromJSON(name, j, keys, logger,
				optional)) {
				const auto lowerCase = tgui::String(name).trim().toLower();
				if (lowerCase == "normal") {
					value = awe::fow_visibility::Normal;
				} else if (lowerCase == "hidden") {
					value = awe::fow_visibility::Hidden;
				} else if (lowerCase == "visible") {
					value = awe::fow_visibility::Visible;
				} else {
					logger.error("Unrecognised {} value \"{}\" at {}, defaulting "
						"to Normal...", engine::script_type<awe::fow_visibility>(),
						name, j.synthesiseKeySequence(keys));
					return false;
				}
				return true;
			}
			return false;
		}
	};

	GAME_PROPERTY_10(terrain, "Terrain", "terrain",
		longName, std::string, false, DEFAULT_VALUE(""), ,
		shortName, std::string, false, DEFAULT_VALUE(""), ,
		icon, std::string, false, DEFAULT_VALUE(""), ,
		description, std::string, false, DEFAULT_VALUE(""), ,
		defence, sf::Uint32, true, DEFAULT_VALUE(0), ,
		movementCost, sf::Int32, true, DEFAULT_VALUE(-1), ,
		maxHP, sf::Uint32, true, DEFAULT_VALUE(0),
		if (operator()() > INT_MAX) {
			logger.warning("Max HP of terrain \"{}\" overflowed ({}). "
				"Setting to {}...", scriptName, operator()(),
				static_cast<sf::Uint32>(INT_MAX));
			operator()() = static_cast<sf::Uint32>(INT_MAX);
		},
		primaryTileType, std::string, true, DEFAULT_VALUE(""), ,
		FOWVisibility, fow_visibility, true, DEFAULT_VALUE(awe::fow_visibility::Normal), ,
		showOwnerWhenHidden, bool, true, DEFAULT_VALUE(false), ,
		awe::RegisterFOWVisibility(engine, document);
	, , )

	GAME_PROPERTY_4(tile_type, "TileType", "tiletype",
		terrain, std::string, false, DEFAULT_VALUE(""), ,
		tile, std::string, false, DEFAULT_VALUE(""), ,
		capturingProperty, std::string, true, DEFAULT_VALUE(""), ,
		alwaysPaintable, bool, true, DEFAULT_VALUE(false), ,
	, , )

	/**
	 * @warning If a structure is not paintable, all of its dependent tiles
	 *          will be removed, and its root tile type \b must be unique
	 *          across non-paintable structures! Code that searches through
	 *          structures to find a non-paintable structure that has a given
	 *          root tile type should always select the structure found first
	 *          to at least try and maintain consistency if this constraint is
	 *          not followed.
	 */
	GAME_PROPERTY_11(structure, "Structure", "structure",
		longName, std::string, false, DEFAULT_VALUE(""), ,
		shortName, std::string, false, DEFAULT_VALUE(""), ,
		icon, std::string, false, DEFAULT_VALUE(""), ,
		description, std::string, false, DEFAULT_VALUE(""), ,
		ownedIcon, std::string, true, DEFAULT_VALUE(""), ,
		root, awe::root_structure_tile, false, DEFAULT_VALUE({}), ,
		paintable, bool, true, DEFAULT_VALUE(true), ,
		keepUnits, bool, true, DEFAULT_VALUE(false), ,
		destroyedLongName, std::string, true, DEFAULT_VALUE(""), ,
		destroyedIcon, std::string, true, DEFAULT_VALUE(""), ,
		dependent, awe::bank_array<awe::dependent_structure_tile>, true, INIT_BANK_ARRAY(), ,
		awe::root_structure_tile::Register(engine, document);
		awe::dependent_structure_tile::Register(engine, document);
		awe::bank_array<awe::dependent_structure_tile>::Register(engine, document);,
			// This checking works great for the dependent tiles without overrides,
			// but what if an override is applied that is invalid? Because of this,
			// it might be more beneficial to move this checking out of here and
			// into the code responsible for managing all the banks.
			// TODO: above, and unit_type sounds also have additional post-processing
			// that's applied after all overrides. awe::banks::_postOverrideProcessing().
			const auto depArr = dependent({}).array->operator->();
			if (depArr->GetSize() == 0) return;
			if (!paintable({})) {
				logger.warning("Structure \"{}\" was configured to be non-paintable. "
					"Removing {} dependent tile{}..." COMMA scriptName COMMA depArr->GetSize() COMMA
					(depArr->GetSize() == 1 ? "" : "s"));
				depArr->Resize(0);
			} else {
				// The root tile { 0, 0 } cannot be a dependent tile, since 0, 0
				// describes an offset in relation to the root tile.
				std::unordered_set<decltype(awe::dependent_structure_tile::offset)> roots = { { 0 COMMA 0 } };
				std::set<asUINT> elemsToDelete;
				for (asUINT i = 0 COMMA end = depArr->GetSize(); i < end; ++i) {
					const auto tile = static_cast<awe::dependent_structure_tile*>(depArr->At(i));
					if (roots.count(tile->offset)) {
						if (tile->offset.x == 0 && tile->offset.y == 0) {
							logger.warning("Structure \"{}\": dependent tile with "
								"offset {} is not allowed! Removing this tile..." COMMA
								scriptName COMMA tile->offset);
						} else {
							logger.warning("Structure \"{}\": dependent tile with "
								"offset {} already exists! Removing duplicate tile..." COMMA
								scriptName COMMA tile->offset);
						}
						elemsToDelete.insert(i);
					} else {
						roots.insert(tile->offset);
					}
				}
				for (auto itr = elemsToDelete.rbegin() COMMA end = elemsToDelete.rend(); itr != end; ++itr) {
					depArr->RemoveAt(*itr);
				}
			}
			dependent({}).initVector();
	, )

	GAME_PROPERTY_4(movement_type, "MovementType", "movementtype",
		longName, std::string, false, DEFAULT_VALUE(""), ,
		shortName, std::string, false, DEFAULT_VALUE(""), ,
		icon, std::string, false, DEFAULT_VALUE(""), ,
		description, std::string, false, DEFAULT_VALUE(""), ,
	,, )

	GAME_PROPERTY_5(country, "Country", "country",
		longName, std::string, false, DEFAULT_VALUE(""), ,
		shortName, std::string, false, DEFAULT_VALUE(""), ,
		icon, std::string, false, DEFAULT_VALUE(""), ,
		description, std::string, false, DEFAULT_VALUE(""), ,
		colour, sf::Color, false, DEFAULT_VALUE({}), ,
		,
		_turnOrder = _turnOrderCounter++;,
		private: awe::ArmyID _turnOrder; static awe::ArmyID _turnOrderCounter;
		public: awe::ArmyID	turnOrder() { return _turnOrder; }
	)
	awe::ArmyID country::_turnOrderCounter = 0;

	GAME_PROPERTY_7(environment, "Environment", "environment",
		longName, std::string, false, DEFAULT_VALUE(""), ,
		shortName, std::string, false, DEFAULT_VALUE(""), ,
		icon, std::string, false, DEFAULT_VALUE(""), ,
		description, std::string, false, DEFAULT_VALUE(""), ,
		spritesheet, std::string, false, DEFAULT_VALUE(""), ,
		pictureSpritesheet, std::string, false, DEFAULT_VALUE(""), ,
		structureIconSpritesheet, std::string, false, DEFAULT_VALUE(""), ,
	,, )

	GAME_PROPERTY_6(weather, "Weather", "weather",
		longName, std::string, false, DEFAULT_VALUE(""), ,
		shortName, std::string, false, DEFAULT_VALUE(""), ,
		icon, std::string, false, DEFAULT_VALUE(""), ,
		description, std::string, false, DEFAULT_VALUE(""), ,
		sound, std::string, false, DEFAULT_VALUE(""), ,
		particles, awe::bank_array<awe::particle_data>, false, INIT_BANK_ARRAY(), ,
		awe::particle_data::Register(engine, document);
		awe::bank_array<awe::particle_data>::Register(engine, document);
	,, )
	
	GAME_PROPERTY_6(commander, "Commander", "commander",
		longName, std::string, false, DEFAULT_VALUE(""), ,
		shortName, std::string, false, DEFAULT_VALUE(""), ,
		icon, std::string, false, DEFAULT_VALUE(""), ,
		description, std::string, false, DEFAULT_VALUE(""), ,
		portrait, std::string, false, DEFAULT_VALUE(""), ,
		theme, std::string, false, DEFAULT_VALUE(""), ,
	,, )
}

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

		inline bank<T>::const_iterator begin() const {
			return cbegin();
		}

		inline bank<T>::const_iterator end() const {
			return cend();
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
				depthCopyOverrides[awe::hierarchy_index<O::type>()] = overrider.first;
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

namespace awe {
	class banks : public engine::json_script {
	public:
		banks(const std::shared_ptr<engine::scripts>& scripts,
			const engine::logger::data& data) :
			engine::json_script({ data.sink, "json_script" }),
			_logger(data), _scripts(scripts),
			_weapons(std::make_shared<bank<weapon>>(scripts, engine::logger::data{ data.sink, "weapons" })),
			_unitTypes(std::make_shared<bank<unit_type>>(scripts, engine::logger::data{ data.sink, "unit_types" })),
			_terrains(std::make_shared<bank<terrain>>(scripts, engine::logger::data{ data.sink, "terrains" })),
			_tileTypes(std::make_shared<bank<tile_type>>(scripts, engine::logger::data{ data.sink, "tile_types" })),
			_structures(std::make_shared<bank<structure>>(scripts, engine::logger::data{ data.sink, "structures" })),
			_movementTypes(std::make_shared<bank<movement_type>>(scripts, engine::logger::data{ data.sink, "movement_types" })),
			_countries(std::make_shared<bank<country>>(scripts, engine::logger::data{ data.sink, "countries" })),
			_environments(std::make_shared<bank<environment>>(scripts, engine::logger::data{ data.sink, "environments" })),
			_weathers(std::make_shared<bank<weather>>(scripts, engine::logger::data{ data.sink, "weathers" })),
			_commanders(std::make_shared<bank<commander>>(scripts, engine::logger::data{ data.sink, "commanders" }))
		{}

		template<typename T>
		std::shared_ptr<const bank<T>> get() const {
			if constexpr (std::is_same<T, commander>::value) {
				return _commanders;
			} else if constexpr (std::is_same<T, weather>::value) {
				return _weathers;
			} else if constexpr (std::is_same<T, environment>::value) {
				return _environments;
			} else if constexpr (std::is_same<T, country>::value) {
				return _countries;
			} else if constexpr (std::is_same<T, movement_type>::value) {
				return _movementTypes;
			} else if constexpr (std::is_same<T, structure>::value) {
				return _structures;
			} else if constexpr (std::is_same<T, tile_type>::value) {
				return _tileTypes;
			} else if constexpr (std::is_same<T, terrain>::value) {
				return _terrains;
			} else if constexpr (std::is_same<T, unit_type>::value) {
				return _unitTypes;
			} else if constexpr (std::is_same<T, weapon>::value) {
				return _weapons;
			} else {
				static_assert(false, "There is no bank of type T, please "
					"implement support for it.");
			}
		}

		void processOverrides() {
			_processOverrides(*_commanders);
			_processOverrides(*_weathers, *_commanders);
			_processOverrides(*_environments, *_weathers, *_commanders);
			_processOverrides(*_countries, *_environments, *_weathers, *_commanders);
			_processOverrides(*_movementTypes, *_countries, *_environments, *_weathers, *_commanders);
			_processOverrides(*_structures, *_movementTypes, *_countries, *_environments, *_weathers, *_commanders);
			_processOverrides(*_tileTypes, *_structures, *_movementTypes, *_countries, *_environments, *_weathers, *_commanders);
			_processOverrides(*_terrains, *_tileTypes, *_structures, *_movementTypes, *_countries, *_environments, *_weathers, *_commanders);
			_processOverrides(*_unitTypes, *_terrains, *_tileTypes, *_structures, *_movementTypes, *_countries, *_environments, *_weathers, *_commanders);
			_processOverrides(*_weapons, *_unitTypes, *_terrains, *_tileTypes, *_structures, *_movementTypes, *_countries, *_environments, *_weathers, *_commanders);
			_postOverrideProcessing();
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
			std::string path;
			j.apply(path, { "commanders" }, true); _commanders->load(path); path.clear();
			j.apply(path, { "weathers" }, true); _weathers->load(path); path.clear();
			j.apply(path, { "environments" }, true); _environments->load(path); path.clear();
			j.apply(path, { "countries" }, true); _countries->load(path); path.clear();
			j.apply(path, { "movementTypes" }, true); _movementTypes->load(path); path.clear();
			j.apply(path, { "structures" }, true); _structures->load(path); path.clear();
			j.apply(path, { "tileTypes" }, true); _tileTypes->load(path); path.clear();
			j.apply(path, { "terrains" }, true); _terrains->load(path); path.clear();
			j.apply(path, { "unitTypes" }, true); _unitTypes->load(path); path.clear();
			j.apply(path, { "weapons" }, true); _weapons->load(path); path.clear();
			return _commanders->inGoodState() &&
				_weathers->inGoodState() &&
				_environments->inGoodState() &&
				_countries->inGoodState() &&
				_movementTypes->inGoodState() &&
				_structures->inGoodState() &&
				_tileTypes->inGoodState() &&
				_terrains->inGoodState() &&
				_unitTypes->inGoodState() &&
				_weapons->inGoodState();
		}

		void _postOverrideProcessing() {
			// TODO: Move post-override processing into here.
			// The GAME_PROPERTY_N macro might define the post-processing method,
			// that banks invokes via each bank.
		}

		template<typename T>
		void _filterOnBaseType(const std::unordered_map<T, std::vector<std::string>>& in,
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
		void _processOverrides(O& bank, Os... banks) {
			// 1. Filter each global variable and global function on base type.
			auto vars = _scripts->getGlobalVariablesAndTheirNamespaces(
				engine::scripts::modules[engine::scripts::BANK_OVERRIDE]);
			std::unordered_map<std::string, std::vector<std::pair<asUINT, std::vector<std::string>>>> filteredVars;
			_filterOnBaseType(vars, filteredVars);
			auto funcs = _scripts->getGlobalFunctionsAndTheirNamespaces(
				engine::scripts::modules[engine::scripts::BANK_OVERRIDE]);
			std::unordered_map<std::string, std::vector<std::pair<asIScriptFunction*, std::vector<std::string>>>> filteredFuncs;
			_filterOnBaseType(funcs, filteredFuncs);
			// 2. Go through each bank, and process every valid override.
			bank.processOverrides(_scripts, filteredVars, filteredFuncs, banks...);
			//commanders->processOverrides(scripts, filteredVars, filteredFuncs);
			//weathers->processOverrides(scripts, filteredVars, filteredFuncs, commanders);
			//environments->processOverrides(scripts, filteredVars, filteredFuncs, weathers, commanders);
			// Etc.
		}

		mutable engine::logger _logger;
		std::shared_ptr<engine::scripts> _scripts;
		// All the banks.
		std::shared_ptr<bank<weapon>> _weapons;
		std::shared_ptr<bank<unit_type>> _unitTypes;
		std::shared_ptr<bank<terrain>> _terrains;
		std::shared_ptr<bank<tile_type>> _tileTypes;
		std::shared_ptr<bank<structure>> _structures;
		std::shared_ptr<bank<movement_type>> _movementTypes;
		std::shared_ptr<bank<country>> _countries;
		std::shared_ptr<bank<environment>> _environments;
		std::shared_ptr<bank<weather>> _weathers;
		std::shared_ptr<bank<commander>> _commanders;
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
