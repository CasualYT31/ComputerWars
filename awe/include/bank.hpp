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

/**@file bank.hpp
 * This header file declares classes used to store static game information specific
 * to Computer Wars.
 * Static game information describes information that doesn't usually change during
 * execution. These include, but are not limited to:
 * <ul><li>Types of weather.</li>
 * <li>The countries available.</li>
 * <li>Movement types.</li>
 * <li>Types of terrain.</li>
 * <li>Types of units.</li>
 * <li>Commanders available.</li></ul>
 */

#pragma once

#include "texture.hpp"
#include "typedef.hpp"
#include "script.hpp"
#include "TGUI/String.hpp"
#include <unordered_set>
#include <variant>

namespace awe {
	/**
	 * Template class used to store lists of game properties.
	 * Static game informaion is stored in objects called \b banks, which are
	 * essentially glorified vectors. Banks utilise \c engine::json_script to load
	 * member information during runtime. Since they are intended to be "static,"
	 * they cannot be saved (doing so wouldn't be very useful since they cannot be
	 * changed). However, banks can load any number of sets of static data during
	 * runtime, each load overwritting what was previously stored.\n
	 * The classes declared in this header are intended to be used with this
	 * template class. For example, \c awe::bank<awe::country> defines a bank of
	 * country properties. The only requirements that a class \c T needs to fulfil
	 * are as follows:
	 * <ol><li>There should be a constructor which accepts a <tt>const
	 *         std::string&#38;</tt> and a <tt>engine::json&#38;</tt>. These
	 *         parameters are intended to store the script name of the bank entry,
	 *         and allow the constructor to read the JSON script object pertaining
	 *         to the bank entry in order to initialise the object, respectively.
	 *         You will need to publicly inherit from \c awe::bank_id.</li>
	 *     <li>All instantiations of your class stored in the bank will be const.
	 *         Therefore use the \c const modifier liberally with your class to
	 *         allow it to be accessed appropriately via the bank. You should not
	 *         design your class to be mutable.</li>
	 *     <li>In addition, your class needs to define a static method:
	 *         <tt>template<typename T> void Register(const std::string&#38; type,
	 *         asIScriptEngine* engine, const
	 *         std::shared_ptr<DocumentationGenerator>&#38; document) noexcept
	 *         </tt>. This method should register your T reference type with the
	 *         given script engine so that the scripts can call the get methods.
	 *         </li></ol>
	 * @tparam T The type of static game property to store within this bank.
	 */
	template<typename T>
	class bank : public engine::script_registrant, public engine::json_script {
	public:
		/**
		 * Provides script interface details to this @c bank instance.
		 * @param scripts Pointer to the @c scripts object to register this bank
		 *                with. If @c nullptr, the bank won't be registered with
		 *                any script interface.
		 * @param name    The base name of the script interface's new type that
		 *                will be registered. The bank type's name will be the one
		 *                given but with \c "Bank" appended, and the single global
		 *                property of this bank type will be called the given name,
		 *                but in lowercase.
		 * @param data    The data to initialise the logger object with.
		 */
		bank(const std::shared_ptr<engine::scripts>& scripts,
			const std::string& name, const engine::logger::data& data);

		/**
		 * The type of container used to store bank values internally.
		 */
		typedef typename std::unordered_map<std::string, std::shared_ptr<const T>>
			bank_type;

		/**
		 * Const iterator typedef.
		 */
		typedef typename bank_type::const_iterator const_iterator;

		/**
		 * Points to the beginning of the bank.
		 * @return Internal bank \c const_iterator.
		 */
		inline const_iterator begin() const {
			return _bank.cbegin();
		}

		/**
		 * Points to the end of the bank.
		 * @return Internal bank \c const_iterator.
		 */
		inline const_iterator end() const {
			return _bank.cend();
		}

		/**
		 * Allows the client to access the game properties of a bank member.
		 * @param  sn The script name of the member to access.
		 * @return A pointer to the properties.
		 */
		std::shared_ptr<const T> operator[](const std::string& sn) const {
			if (_bank.find(sn) == _bank.end()) {
				_logger.error("Game property \"{}\" does not exist in this bank!",
					sn);
			}
			return _bank.at(sn);
		}

		/**
		 * Calculates the size of the bank.
		 * @return The number of members or elements of the internal vector \c
		 *         _bank.
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

		/**
		 * Returns a set of script names pointing to members of this bank.
		 * @return All the script names found within this bank.
		 */
		std::unordered_set<std::string> getScriptNames() const;

		/**
		 * Callback given to \c engine::scripts::registerInterface() to register
		 * game engine functions with a \c scripts object.
		 * @safety No guarantee.
		 * @sa     \c engine::scripts::registerInterface()
		 */
		void registerInterface(asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>& document);
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
		bool _load(engine::json& j);

		/**
		 * The JSON save method for this class.
		 * This class does not have the ability to be saved.
		 * @param  j The \c nlohmann::ordered_json object representing the JSON
		 *           script which this method writes to.
		 * @return Always returns \c FALSE.
		 */
		inline bool _save(nlohmann::ordered_json& j) noexcept {
			return false;
		}

		/**
		 * Script's string index operator.
		 * @throws std::runtime_error if no game property with the given name
		 *                            exists.
		 * @sa     @c awe::bank<T>::operator[](const std::string&)
		 */
		const T* _opIndexStr(const std::string& name) const {
			auto ret = operator[](name);
			if (ret) return ret.get();
			throw std::runtime_error("Could not access game property");
		}

		/**
		 * Calls \c getScriptNames() and converts the result into a \c CScriptArray
		 * of strings.
		 * @return The array that can be used by scripts.
		 */
		CScriptArray* _getScriptNamesArray() const;

		/**
		 * Internal logger object.
		 */
		mutable engine::logger _logger;

		/**
		 * The internal map of game properties.
		 */
		bank_type _bank;

		/**
		 * The pointer to the @c scripts instance.
		 */
		std::shared_ptr<engine::scripts> _scripts;

		/**
		 * The name of the single global property.
		 */
		std::string _propertyName;
	};

	/**
	 * Base class for all game property classes.
	 * Inheritance is to be used to implement these properties in your game
	 * property classes.
	 */
	class bank_id {
	public:
		/**
		 * Polymorphic base classes should have virtual destructors.
		 */
		virtual ~bank_id() noexcept = default;

		/**
		 * Registers \c bank_id with a given type.
		 * @tparam T        The type that is being registered, that inherits from
		 *                  \c bank_id in some way.
		 * @param  type     Name of the type to add the properties to.
		 * @param  engine   Pointer to the AngelScript script engine to register
		 *                  with.
		 * @param  document Pointer to the AngelScript documentation generator to
		 *                  register script interface documentation with.
		 * @safety No guarantee.
		 */
		template<typename T>
		static void Register(const std::string& type,
			asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		/**
		 * Retrieves the script name of this bank entry as defined by the JSON
		 * script it is loaded with.
		 * @return The string identifier.
		 */
		inline const std::string& getScriptName() const noexcept {
			return _scriptName;
		}

		/**
		 * Comparison operator.
		 * @param  rhs The \c bank_id object to test against.
		 * @return \c TRUE if both object's script names are the same, \c FALSE if
		 *         not.
		 */
		virtual inline bool operator==(const awe::bank_id& rhs) const noexcept {
			return _scriptName == rhs._scriptName;
		}

		/**
		 * Inverse comparison operator.
		 * @param  rhs The \c bank_id object to test against.
		 * @return \c TRUE if both object's script names are not the same,
		 *         \c FALSE if they are.
		 */
		virtual inline bool operator!=(const awe::bank_id& rhs) const noexcept {
			return !(*this == rhs);
		}
	protected:
		/**
		 * Constructor which assigns the script name to the bank entry.
		 * For classes that inherit from this one, this protected constructor is to
		 * be called, either in the subclass' constructor definition's initialiser
		 * list, or in the code block of the subclass constructor.
		 * @param scriptName The identifier of this bank entry.
		 */
		inline bank_id(const std::string& scriptName) : _scriptName(scriptName) {}

		/**
		 * Used by subclasses to return an empty string when attempting to retrieve
		 * a string property that doesn't exist.
		 * Should be initialised to an empty string. Note that this is not
		 * thread-safe (https://stackoverflow.com/questions/30239268/how-can-i-return-a-const-reference-to-an-empty-string-without-a-compiler-warning),
		 * but honestly none of my code has been up to this point so I don't care.
		 */
		static const std::string EMPTY_STRING;
	private:
		/**
		 * The script name/identifier of this bank entry.
		 */
		std::string _scriptName = "";
	};

	/**
	 * Base class containing properties common to a majority of game property
	 * classes.
	 * Inheritance is to be used to implement these properties in your game
	 * property classes.
	 */
	class common_properties : public awe::bank_id {
	public:
		/**
		 * Polymorphic base classes should have virtual destructors.
		 */
		virtual ~common_properties() noexcept = default;

		/**
		 * Registers \c common_properties with a given type.
		 * @tparam T            The type that is being registered, that inherits
		 *                      from \c common_properties in some way.
		 * @param  type         Name of the type to add the properties to.
		 * @param  engine       Pointer to the AngelScript script engine to
		 *                      register with.
		 * @param  document     Pointer to the AngelScript documentation generator
		 *                      to register script interface documentation with.
		 * @param  extraIconDoc Used by subclasses to provide extra documentation
		 *                      on how their \c icon property should be used.
		 * @safety No guarantee.
		 */
		template<typename T>
		static void Register(const std::string & type,
			asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>& document,
			const std::string& extraIconDoc = "");

		/**
		 * Retrieves the long name property.
		 * @return The long name property, which should be a language dictionary
		 *         key.
		 */
		inline const std::string& getName() const noexcept {
			return _name;
		}

		/**
		 * Retrieves the short name property.
		 * @return The short name property, which should be a language dictionary
		 *         key.
		 */
		inline const std::string& getShortName() const noexcept {
			return _shortName;
		}

		/**
		 * Retrieves the sprite name of the icon associated with this property.
		 * @return The sprite name.
		 */
		inline const std::string& getIconName() const noexcept {
			return _iconKey;
		}

		/**
		 * Retrieves the description property.
		 * @return The description property, which should be a language dictionary
		 *         key.
		 */
		inline const std::string& getDescription() const noexcept {
			return _description;
		}
	protected:
		/**
		 * Constructor which reads a given JSON script object and fills in the
		 * properties accordingly.
		 * For classes that inherit from this one, this protected constructor is to
		 * be called, either in the subclass' constructor definition's initialiser
		 * list, or in the code block of the subclass constructor. The reference to
		 * the \c engine::json object received in the subclass' constructor should
		 * be passed on directly without changes.\n
		 * 
		 * The following keys correspond to the following properties:
		 * <ul><li>\c "longname" = \c _name</li>
		 *     <li>\c "shortname" = \c _shortName</li>
		 *     <li>\c "icon" = \c _iconKey</li>
		 *     <li>\c "description" = \c _description</li>
		 * </ul>
		 * @param scriptName The identifier of this bank entry that is to be used
		 *                   within game scripts.
		 * @param j          The object value containing the name, icon, and
		 *                   description properties, as well as any other
		 *                   properties subclasses need to store (which this class
		 *                   ignores).
		 */
		common_properties(const std::string& scriptName, engine::json& j);
	private:
		/**
		 * The long name property.
		 */
		std::string _name = "";

		/**
		 * The short name property.
		 */
		std::string _shortName = "";

		/**
		 * The icon property.
		 */
		std::string _iconKey = "";

		/**
		 * The description property.
		 */
		std::string _description = "";
	};

	/**
	 * A game property class which stores the information associated with a single
	 * country.
	 * @sa \c awe::common_properties
	 */
	class country : public awe::common_properties {
	public:
		/**
		 * Constructor which scans a JSON object for the colour property.
		 * It also passes on the JSON object to the \c common_properties
		 * constructor.\n
		 * In addition to the keys defined in the superclass, the following keys
		 * are required:
		 * <ul><li>\c "colour" = \c _colour, in the format <tt>[r,g,b,a]</tt>
		 *         </li></ul>
		 * And the following keys are optional:
		 * <ul><li>\c "turnorder" = \c _turnorder. Usually, the turn order of a
		 *         country is determined by its location in the overall script. The
		 *         first country defined will have a turn order ID of 0, then the
		 *         second country will have ID 1, etc. <b>It's important to note
		 *         that even if a turn order ID is given explicitly, the internal
		 *         counter will still increment!</b> E.g. if there are three
		 *         countries, and the second country is given an explicitly ID of
		 *         5, then the turn order IDs will go 0, 5, 2. Then, in-game, the
		 *         second country with ID 5 would come last in the turn order. Each
		 *         turn order ID must be unique and must not equal \c NO_ARMY.</li>
		 *         </ul>
		 * @param scriptName The identifier of this bank entry that is to be used
		 *                   within game scripts.
		 * @param j          The object value containing the country's properties.
		 */
		country(const std::string& scriptName, engine::json& j);

		/**
		 * Registers \c country with a given type.
		 * @tparam T        The type that is being registered, that inherits from
		 *                  \c country in some way.
		 * @param  type     Name of the type to add the properties to.
		 * @param  engine   Pointer to the AngelScript script engine to register
		 *                  with.
		 * @param  document Pointer to the AngelScript documentation generator to
		 *                  register script interface documentation with.
		 * @safety No guarantee.
		 */
		template<typename T>
		static void Register(const std::string& type,
			asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		/**
		 * Retrieves the colour property.
		 * @return The colour of the country.
		 */
		inline const sf::Color& getColour() const noexcept {
			return _colour;
		}

		/**
		 * Retrieves the turn order property.
		 * @return The turn order property of this country.
		 */
		inline awe::ArmyID getTurnOrder() const noexcept {
			return _turnOrder;
		}
	private:
		/**
		 * The colour property.
		 */
		sf::Color _colour;

		/**
		 * The turn order property.
		 */
		awe::ArmyID _turnOrder;

		/**
		 * The turn order counter.
		 */
		static awe::ArmyID _turnOrderCounter;
	};

	/**
	 * A game property class which stores the information associated with a single
	 * weather.
	 * @sa \c awe::common_properties
	 */
	class weather : public awe::common_properties {
	public:
		/**
		 * Constructor which passes on the JSON object to the \c common_properties
		 * constructor.
		 * No additional keys are required.
		 * @param scriptName The identifier of this bank entry that is to be used
		 *                   within game scripts.
		 * @param j          The object value containing the weather's properties.
		 */
		inline weather(const std::string& scriptName, engine::json& j) :
			common_properties(scriptName, j) {}

		/**
		 * Registers \c weather with a given type.
		 * @tparam T        The type that is being registered, that inherits from
		 *                  \c weather in some way.
		 * @param  type     Name of the type to add the properties to.
		 * @param  engine   Pointer to the AngelScript script engine to register
		 *                  with.
		 * @param  document Pointer to the AngelScript documentation generator to
		 *                  register script interface documentation with.
		 * @safety No guarantee.
		 */
		template<typename T>
		static void Register(const std::string& type,
			asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>& document);
	};

	/**
	 * A game property class which stores the information associated with a single
	 * environment.
	 * An environment defines the tileset to use for a map (for example, normal,
	 * desert, snowy, etc.).
	 * @sa \c awe::common_properties
	 */
	class environment : public awe::common_properties {
	public:
		/**
		 * Constructor which passes on the JSON object to the \c common_properties
		 * constructor.
		 * No additional keys are required.
		 * @param scriptName The identifier of this bank entry that is to be used
		 *                   within game scripts.
		 * @param j          The object value containing the environment's
		 *                   properties.
		 */
		environment(const std::string& scriptName, engine::json& j) :
			common_properties(scriptName, j) {}

		/**
		 * Registers \c environment with a given type.
		 * @tparam T        The type that is being registered, that inherits from
		 *                  \c environment in some way.
		 * @param  type     Name of the type to add the properties to.
		 * @param  engine   Pointer to the AngelScript script engine to register
		 *                  with.
		 * @param  document Pointer to the AngelScript documentation generator to
		 *                  register script interface documentation with.
		 * @safety No guarantee.
		 */
		template<typename T>
		static void Register(const std::string& type,
			asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>& document);
	};

	/**
	 * A game property class which stores the information associated with a single
	 * movement type.
	 * @sa \c awe::common_properties
	 */
	class movement_type : public awe::common_properties {
	public:
		/**
		 * Constructor which passes on the JSON object to the \c common_properties
		 * constructor.
		 * No additional keys are required.
		 * @param scriptName The identifier of this bank entry that is to be used
		 *                   within game scripts.
		 * @param j          The object value containing the movement type's
		 *                   properties.
		 */
		movement_type(const std::string& scriptName, engine::json& j) :
			common_properties(scriptName, j) {}

		/**
		 * Registers \c movement_type with a given type.
		 * @tparam T        The type that is being registered, that inherits from
		 *                  \c movement_type in some way.
		 * @param  type     Name of the type to add the properties to.
		 * @param  engine   Pointer to the AngelScript script engine to register
		 *                  with.
		 * @param  document Pointer to the AngelScript documentation generator to
		 *                  register script interface documentation with.
		 * @safety No guarantee.
		 */
		template<typename T>
		static void Register(const std::string& type,
			asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>& document);
	};

	/**
	 * A game property class which stores the information associated with a single
	 * terrain type.
	 * @sa \c awe::common_properties
	 */
	class terrain : public awe::common_properties {
	public:
		/**
		 * Constructor which scans a JSON object for the terrain type's properties.
		 * It also passes on the JSON object to the \c common_properties
		 * constructor. In addition to the keys defined in the superclass, the
		 * following keys are required:
		 * 
		 * <ul><li>\c "hp" = \c _maxHP, <tt>(unsigned 32-bit int - capped off at
		 *         signed 32-bit int's maximum value)</tt></li>
		 *     <li>\c "defence" = \c _defenece, <tt>(unsigned 32-bit int)</tt></li>
		 *     <li>\c "movecosts" = \c _movecosts,
		 *         <tt>({"MOVEMENT_TYPE_SCRIPT_NAME": signed 32-bit int[, etc.]})
		 *         </tt></li>
		 *     <li>\c "pictures" = \c _pictures, <tt>({"COUNTRY_SCRIPT_NAME":
		 *         string[, etc.]})</tt></li></ul>
		 * 
		 * The \c movecosts object stores a list of movement points associated with
		 * each movement type. A negative value indicates that a unit of the
		 * specified movement type cannot traverse the terrain. If a movement type
		 * was not given an explicit movement point cost, then it will be given
		 * -1. If \c movecosts is not given, then no movement types will be able to
		 * traverse this terrain type.\n
		 * 
		 * The \c pictures object stores a list of animated sprite names associated
		 * with each country. Not all countries have to be accounted for if the
		 * tile cannot be "owned," i.e. captured. If a country doesn't have a
		 * assigned picture sprite, then the neutral picture sprite will be
		 * returned, i.e. whatever was assigned to \c "icon".
		 * @param scriptName The identifier of this bank entry that is to be used
		 *                   within game scripts.
		 * @param j          The object value containing the terrain type's
		 *                   properties.
		 */
		terrain(const std::string& scriptName, engine::json& j);

		/**
		 * Registers \c terrain with a given type.
		 * @tparam T        The type that is being registered, that inherits from
		 *                  \c terrain in some way.
		 * @param  type     Name of the type to add the properties to.
		 * @param  engine   Pointer to the AngelScript script engine to register
		 *                  with.
		 * @param  document Pointer to the AngelScript documentation generator to
		 *                  register script interface documentation with.
		 * @safety No guarantee.
		 */
		template<typename T>
		static void Register(const std::string& type,
			asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		/**
		 * Retrieves the maximum health property.
		 * This can be the health points of a cannon, or the capture points of a
		 * property.
		 * @return The health points this terrain can have.
		 */
		inline unsigned int getMaxHP() const noexcept {
			return _maxHP;
		}

		/**
		 * Retrieves the defence property.
		 * @return The defence this terrain provides.
		 */
		inline unsigned int getDefence() const noexcept {
			return _defence;
		}

		/**
		 * Retrieves the movement point cost property associated with a given
		 * movement type.
		 * @param  movecostName The script name of the type of movement.
		 * @return The movement point cost.
		 */
		inline int getMoveCost(const std::string& movecostName) const {
			return (( _movecosts.find(movecostName) == _movecosts.end() ) ?
				( -1 ) : ( _movecosts.at(movecostName) ));
		}

		/**
		 * Retrieves the sprite name associated with a given country.
		 * @param  countryName The script name of the country.
		 * @return The sprite name of the terrain picture, or a blank string if the
		 *         given country ID didn't identify a sprite name.
		 */
		inline const std::string& getPicture(
			const std::string& countryName) const {
			return ((_pictures.find(countryName) == _pictures.end()) ?
				(getIconName()) : (_pictures.at(countryName)));
		}

		/**
		 * Retrieves the sprite name associated with a given country.
		 * @param  countryID The turn order ID of the country.
		 * @return The sprite name of the terrain picture, or a blank string if the
		 *         given country ID didn't identify a sprite name.
		 */
		inline const std::string& getPicture(const awe::ArmyID countryID) const {
			return
				((_picturesTurnOrder.find(countryID) == _picturesTurnOrder.end()) ?
				(getIconName()) : (_picturesTurnOrder.at(countryID)));
		}

		/**
		 * Updates \c _picturesTurnOrder by copying over the contents of
		 * \c _pictures and supplying the respective turn order IDs as keys.
		 * @param  countries Pointer to the country bank to pull the turn order IDs
		 *                   from.
		 * @safety Basic guarantee.
		 */
		void updatePictureMap(const awe::bank<awe::country>& countries) const;
	private:
		/**
		 * Maximum health points property.
		 */
		unsigned int _maxHP = 0;

		/**
		 * Defence property.
		 */
		unsigned int _defence = 0;

		/**
		 * Movement point cost properties.
		 */
		std::unordered_map<std::string, int> _movecosts;

		/**
		 * Picture properties.
		 */
		std::unordered_map<std::string, std::string> _pictures;

		/**
		 * Picture properties keyed by turn order ID.
		 */
		mutable std::unordered_map<awe::ArmyID, std::string> _picturesTurnOrder;
	};

	/**
	 * A game property class which stores the information associated with a single
	 * type of tile.
	 * Tiles and terrain types were separated in this way so that different visual
	 * representations of the same terrain can be supported. For example, a road
	 * may be straight, a bend, a T-junction, or a crossroads.
	 * @sa awe::bank_id
	 */
	class tile_type : public awe::bank_id {
	public:
		/**
		 * Constructor which reads the given JSON object for tile properties.
		 * The following keys are required:
		 * <ul><li>\c "type" = \c _terrainType, <tt>(string)</tt></li>
		 *     <li>\c "neutral" = \c _neutralTile, <tt>(string)</tt></li>
		 *     <li>\c "tiles" = \c _tiles, <tt>({COUNTRY_SCRIPT_NAME: SPRITE_NAME
		 *         (string)[, etc.]}  [string{, string, etc.}])</tt></li></ul>
		 * The \c neutral key stores a sprite name shown when the tile is not owned
		 * by any country. This should be given for all tile types.\n
		 * The \c tiles object stores a list of animated sprite names associated
		 * with each country's version of the tile (i.e. when they own the tile).
		 * This object does not have to be accounted for if the tile cannot be
		 * owned/captured. In which case, the key and object can be omitted.
		 * @param scriptName The identifier of this bank entry that is to be used
		 *                   within game scripts.
		 * @param j          The object value containing the tile type's
		 *                   properties.
		 */
		tile_type(const std::string& scriptName, engine::json& j);

		/**
		 * Registers \c tile_type with a given type.
		 * @tparam T        The type that is being registered, that inherits from
		 *                  \c tile_type in some way.
		 * @param  type     Name of the type to add the properties to.
		 * @param  engine   Pointer to the AngelScript script engine to register
		 *                  with.
		 * @param  document Pointer to the AngelScript documentation generator to
		 *                  register script interface documentation with.
		 * @safety No guarantee.
		 */
		template<typename T>
		static void Register(const std::string& type,
			asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		/**
		 * Retrieves the script name of the type of terrain this tile represents
		 * (i.e. "Plains" or "Road").
		 * @return The script name of the type of terrain.
		 */
		inline const std::string& getTypeScriptName() const noexcept {
			return _terrainTypeScriptName;
		}

		/**
		 * Retrieves the name of the sprite that is shown for a given country.
		 * @param  countryName The script name of the country.
		 * @return The name of the tile's sprite, or \c _neutralTile if the given
		 *         country name didn't identify a sprite name.
		 */
		inline const std::string& getOwnedTile(
			const std::string& countryName) const {
			return ((_ownedTiles.find(countryName) == _ownedTiles.end()) ?
				(getNeutralTile()) : (_ownedTiles.at(countryName)));
		}

		/**
		 * Retrieves the name of the sprite that is shown for a given country.
		 * @param  countryID The turn order ID of the country.
		 * @return The name of the tile's sprite, or \c _neutralTile if the given
		 *         country name didn't identify a sprite name.
		 */
		inline const std::string& getOwnedTile(const awe::ArmyID countryID) const {
			return ((_ownedTilesTurnOrder.find(countryID) ==
				_ownedTilesTurnOrder.end()) ?
				(getNeutralTile()) : (_ownedTilesTurnOrder.at(countryID)));
		}

		/**
		 * Retrieves the name of the sprite that is shown when no country owns the
		 * tile.
		 * @return The name of the tile's neutral sprite.
		 */
		inline const std::string& getNeutralTile() const noexcept {
			return _neutralTile;
		}

		/**
		 * Retrieves a pointer to the details of the type of terrain this tile
		 * represents.
		 * @return The pointer to the terrain type's properties.
		 * @sa     @c updateTerrain()
		 */
		inline std::shared_ptr<const awe::terrain> getType() const {
			return _terrain;
		}

		/**
		 * Updates the stored terrain type properties pointer.
		 * @param  terrainBank A reference to the terrain bank to pull the pointer
		 *         from.
		 * @safety Strong guarantee.
		 */
		inline void updateTerrain(
			const awe::bank<awe::terrain>& terrainBank) const {
			_terrain = terrainBank[_terrainTypeScriptName];
		}

		/**
		 * Updates \c _ownedTilesTurnOrder by copying over the contents of
		 * \c _ownedTiles and supplying the respective turn order IDs as keys.
		 * @param  countries Pointer to the country bank to pull the turn order IDs
		 *                   from.
		 * @safety Basic guarantee.
		 */
		void updateOwnedTilesMap(const awe::bank<awe::country>& countries) const;
	private:
		/**
		 * Script interface version of \c getType().
		 * @return The terrain type's properties.
		 * @sa     @c getType()
		 */
		inline const awe::terrain* _getTypeObj() const {
			return getType().get();
		}

		/**
		 * The script name of the type of terrain this tile represents.
		 */
		std::string _terrainTypeScriptName;

		/**
		 * Pointer to the properties of this tile's type of terrain.
		 * It was made mutable so that it can be updated after construction in the
		 * \c bank constructor, via \c updateTerrain().
		 */
		mutable std::shared_ptr<const awe::terrain> _terrain;

		/**
		 * The sprite names of the tile corresponding to each country.
		 */
		std::unordered_map<std::string, std::string> _ownedTiles;

		/**
		 * \c _ownedTiles keyed by turn order ID.
		 */
		mutable std::unordered_map<awe::ArmyID, std::string> _ownedTilesTurnOrder;

		/**
		 * The sprite name of the tile with no owner.
		 */
		std::string _neutralTile;
	};

	class unit_type;

	/**
	 * A game property class which stores the information associated with a weapon
	 * belonging to a type of unit.
	 */
	class weapon : public common_properties {
	public:
		/**
		 * Constructor which reads the given JSON object for weapon properties.
		 * The following keys can be read:
		 * <ul><li>\c "ammo" = \c _maxAmmo, <tt>(signed int32)</tt></li>
		 *     <li>\c "canattackaftermoving" = \c _canAttackAfterMoving,
		 *         <tt>(bool, optional, default = true)</tt></li>
		 *     <li>\c "cancounterattackdirectly" = \c _canCounterattackDirectly,
		 *         <tt>(bool, optional, default = true)</tt></li>
		 *     <li>\c "cancounterattackindirectly" =
		 *         \c _canCounterattackIndirectly, <tt>(bool, optional, default =
		 *         false)</tt></li>
		 *     <li>\c "lowrange" = _range.x, <tt>(unsigned int32)</tt></li>
		 *     <li>\c "highrange" = _range.y, <tt>(unsigned int32)</tt></li>
		 *     <li>\c "units" = \c _canAttackTheseUnits,
		 *         <tt>({UNIT_TYPE_SCRIPT_NAME: base damage (signed int32) -OR-
		 *         bool[, etc.]}
		 *         </tt></li>
		 *     <li>\c "hiddenunits" = \c _canAttackTheseHiddenUnits,
		 *         <tt>({UNIT_TYPE_SCRIPT_NAME: base damage (signed int32) -OR-
		 *         bool[, etc.]}</tt></li>
		 *     <li>\c "terrains" = \c _canAttackTheseTerrains,
		 *         <tt>({TERRAIN_SCRIPT_NAME: base damage (signed int32)[, etc.]}
		 *         </tt></li></ul>
		 * Range values work by counting the number of tiles away from the unit's
		 * current tile. If the tile is within both the lower and higher ranges
		 * inclusive, then the attack is valid. If not, the attack is invalid.\n
		 * If \c ammo is less than 0, the weapon has infinite ammo.\n
		 * \c units, \c hiddenunits, \c terrains all have the same format. If the
		 * weapon can attack a given target, then its script name should be put in
		 * the appropriate object as a key, and the base damage should be given as
		 * its value. The base damage maps to \em internal HP in the case of units.
		 * If a weapon cannot attack a given target, then its key must be left out
		 * of the appropriate object entirely.\n
		 * \c units is for units that are visible. \c hiddenunits is for units that
		 * are hidden. If a \c hiddenunits value is a boolean, and the value is
		 * \c TRUE, and the script name is also found in \c units, then the base
		 * damage against that unit will be the same regardless of the hiding state
		 * of the unit. If \c FALSE is given, then this will have the same effect
		 * as leaving the unit script name out of the \c hiddenunits object.
		 * @param scriptName The identifier of this bank entry that is to be used
		 *                   within game scripts.
		 * @param j          The object value containing the tile type's
		 *                   properties.
		 */
		weapon(const std::string& scriptName, engine::json& j);

		/**
		 * Registers \c weapon with a given type.
		 * @tparam T        The type that is being registered, that inherits from
		 *                  \c weapon in some way.
		 * @param  type     Name of the type to add the properties to.
		 * @param  engine   Pointer to the AngelScript script engine to register
		 *                  with.
		 * @param  document Pointer to the AngelScript documentation generator to
		 *                  register script interface documentation with.
		 * @safety No guarantee.
		 */
		template<typename T>
		static void Register(const std::string& type, asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		/**
		 * Retrieves a reference to the JSON used to initialise this weapon.
		 * @return The JSON object.
		 */
		inline const engine::json& getJSON() const noexcept {
			return _jsonCache;
		}

		/**
		 * Retrieves the maximum amount of ammo that can be stored in this weapon.
		 * @return The max ammo property of this weapon.
		 */
		inline int getMaxAmmo() const noexcept {
			return _maxAmmo;
		}

		/**
		 * Determines if this weapon has infinite ammo.
		 * @return \c TRUE if the max ammo is below 0, \c FALSE otherwise.
		 */
		inline bool hasInfiniteAmmo() const noexcept {
			return _maxAmmo < 0;
		}

		/**
		 * Retrieves the range of this weapon.
		 * @return The lower range (\c x), followed by the higher range (\c y).
		 */
		inline const sf::Vector2u& getRange() const noexcept {
			return _range;
		}

		/**
		 * Finds out if this weapon can attack after the weapon's owner has moved.
		 * @return \c TRUE if a unit with this weapon can attack on any tile,
		 *         \c FALSE if the unit can only attack from the tile it began its
		 *         turn on.
		 */
		inline bool canAttackAfterMoving() const noexcept {
			return _canAttackAfterMoving;
		}

		/**
		 * Finds out if this weapon can counterattack directly.
		 * I.e. within a one tile range.
		 * @return \c TRUE if the unit can counterattack with a direct attack,
		 *         \c FALSE otherwise.
		 */
		inline bool canCounterattackDirectly() const noexcept {
			return _canCounterattackDirectly;
		}

		/**
		 * Finds out if this weapon can counterattack indirectly.
		 * I.e. outside of a one tile range.
		 * @return \c TRUE if the unit can counterattack with an indirect attack,
		 *         \c FALSE otherwise.
		 */
		inline bool canCounterattackIndirectly() const noexcept {
			return _canCounterattackIndirectly;
		}

		/**
		 * Finds out if this weapon can attack the given type of unit.
		 * @param  unit   The script name of the type of unit to search for.
		 * @param  hidden If \c TRUE, this will query the hidden units damage
		 *                table instead of the visible units damage table.
		 * @return \c TRUE if the given unit has a base damage stored, \c FALSE
		 *         if not.
		 */
		inline bool canAttackUnit(const std::string& unit,
			const bool hidden = false) const {
			return ((hidden) ? ( _canAttackTheseHiddenUnits.find(unit) !=
				_canAttackTheseHiddenUnits.end()) :
				(_canAttackTheseUnits.find(unit) != _canAttackTheseUnits.end()));
		}

		/**
		 * Finds out the base damage that this weapon deals to a unit.
		 * @param  unit   The script name of the type of unit to query.
		 * @param  hidden If \c TRUE, this will query the hidden units damage
		 *                table instead of the visible units damage table.
		 * @return The base damage, if this unit can be attacked. \c 0 otherwise.
		 */
		inline int getBaseDamageUnit(const std::string& unit,
			const bool hidden = false) const {
			return ((canAttackUnit(unit, hidden)) ?
				((hidden) ? (_canAttackTheseHiddenUnits.at(unit)) :
				(_canAttackTheseUnits.at(unit))) : (0));
		}

		/**
		 * Finds out if this weapon can attack the given type of terrain.
		 * @param  terrain The script name of the type of terrain to search for.
		 * @return \c TRUE if the given terrain has a base damage stored, \c FALSE
		 *         if not.
		 */
		inline bool canAttackTerrain(const std::string& terrain) const {
			return _canAttackTheseTerrains.find(terrain) !=
				_canAttackTheseTerrains.end();
		}

		/**
		 * Finds out the base damage that this weapon deals to a terrain.
		 * @param  terrain The script name of the type of terrain to query.
		 * @return The base damage, if this terrain can be attacked. \c 0
		 *         otherwise.
		 */
		inline int getBaseDamageTerrain(const std::string& terrain) const {
			return ((canAttackTerrain(terrain)) ?
				(_canAttackTheseTerrains.at(terrain)) : (0));
		}
	private:
		/**
		 * Cache of the JSON object used to initialise this weapon.
		 * Used for weapon overrides in \c unit_type.
		 */
		engine::json _jsonCache;

		/**
		 * The max ammo that can be stored in this weapon.
		 */
		int _maxAmmo = 0;

		/**
		 * The range of this weapon.
		 */
		sf::Vector2u _range = sf::Vector2u(1, 1);

		/**
		 * Can a unit with this weapon attack after moving?
		 */
		bool _canAttackAfterMoving = true;

		/**
		 * Can a unit with this weapon counterattack if their assailant is on a
		 * tile directly next to theirs?
		 */
		bool _canCounterattackDirectly = true;

		/**
		 * Can a unit with this weapon counterattack if their assailant is on a
		 * tile other than one directly next to theirs?
		 */
		bool _canCounterattackIndirectly = false;

		/**
		 * List of unit types that can be attacked by this weapon, along with their
		 * base damages.
		 */
		std::unordered_map<std::string, int> _canAttackTheseUnits;

		/**
		 * List of unit types that can be attacked by this weapon when the unit is
		 * hidden, along with their base damages.
		 */
		std::unordered_map<std::string, int> _canAttackTheseHiddenUnits;

		/**
		 * List of terrain types that can be attacked by this weapon, along with
		 * their base damages.
		 */
		std::unordered_map<std::string, int> _canAttackTheseTerrains;
	};

	/**
	 * A game property class which stores the information associated with types of
	 * units.
	 * @sa \c awe::common_properties
	 */
	class unit_type : public common_properties {
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
		 * precise as possible, at least internally.
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

		/**
		 * Constructor which scans a JSON object for the unit type's properties.
		 * It also passes on the JSON object to the \c common_properties
		 * constructor. In addition to the keys defined in the superclass, the
		 * following keys are required:
		 * 
		 * <ul><li>\c "movetype" = \c _movementTypeScriptName, <tt>(string)</tt>
		 *     </li>
		 *     <li>\c "price" = \c _cost, <tt>(unsigned 32-bit int)</tt></li>
		 *     <li>\c "fuel" = \c _maxFuel, <tt>(signed 32-bit int)</tt></li>
		 *     <li>\c "hp" = \c _maxHP, <tt>(unsigned 32-bit int - capped off at
		 *         signed 32-bit int's maximum value divided by the HP granularity
		 *         value)</tt></li>
		 *     <li>\c "mp" = \c _movementPoints, <tt>(unsigned 32-bit int)</tt>
		 *         </li>
		 *     <li>\c "vision" = \c _vision, <tt>(unsigned 32-bit int)</tt></li>
		 *     <li>\c "pictures" = \c _pictures, <tt>{COUNTRY_SCRIPT_NAME: string
		 *         SPRITE_NAME[, etc.]}</tt></li>
		 *     <li>\c "sprites" = \c _units, <tt>{COUNTRY_SCRIPT_NAME: string
		 *         SPRITE_NAME[, etc.]}</tt></li>
		 *     <li>\c "canload" = \c _canLoadThese, <tt>([UNIT_TYPE_SCRIPT_NAME{,
		 *         etc.}])</tt></li>
		 *     <li>\c "loadlimit" = \c _loadLimit, <tt>(unsigned 32-bit int)</tt>
		 *         </li>
		 *     <li>\c "turnstartpriority" = \c _turnStartPriority, <tt>(unsigned
		 *         32-bit int)</tt></li>
		 *     <li>\c "cancapture" = \c _canCaptureThese,
		 *         <tt>([TERRAIN_TYPE_SCRIPT_NAME{, etc.}])</tt></li>
		 *     <li>\c "canhide" = \c _canHide, <tt>(bool)</tt></li>
		 *     <li>\c "canunloadfrom" = \c _canUnloadFromThese,
		 *         <tt>([TERRAIN_TYPE_SCRIPT_NAME{, etc.}])</tt></li>
		 *     <li>\c "weapons" = \c _weapons, <tt>({WEAPON_SCRIPT_NAME: {insert
		 *         optional overrides here}[, etc.]})</tt></li>
		 *     <li>\c "ignoredefence" = \c _ignoreDefence, <tt>(bool, default =
		 *         false)</tt></li></ul>
		 *
		 * Pictures is an array of sprite names corresponding to each country's
		 * portrait of the type of unit.\n
		 *
		 * Sprites is an array of sprite names corresponding to each country's map
		 * representation of the type of unit.\n
		 *
		 * Upon the start of an army's turn, all of their units go through a script
		 * function which will affect the unit depending on its type.
		 * TurnStartPriority allows you to group units together into priority
		 * levels, where units with a higher number are passed to this script
		 * function first, before units with a lower number. This allows you to,
		 * for example, ensure that APCs always resupply adjacent units at the
		 * start of every turn before any planes crash or ships sink. There is no
		 * guaranteed order defined for units that fall in the same priority level.
		 * By default, units have the lowest priority level.\n
		 *
		 * Cancapture and canunloadfrom are arrays of terrain type script names. If
		 * no terrain types are given for canunloadfrom (or it is not provided),
		 * then it is assumed that the unit can unload from any tile. If cancapture
		 * is empty or nonexistent, however, the unit won't be able to capture
		 * anything.\n
		 *
		 * Weapons defines the weapons available to a unit. The order in which they
		 * are listed defines their order of precedence. I.e. if both the first
		 * and second weapons can be used, then the first weapon is picked over the
		 * second (though technically this restriction isn't enforced by this
		 * class). Weapons can be given to units verbatim, in which case an empty
		 * object must be assigned to the weapon's script name. Or, units may have
		 * overrides for certain properties of a weapon, in which case they can be
		 * provided in this object. Any keys that would apply to a normal weapon
		 * can be used here.\n
		 * It is important to note extra behaviour regarding how overrides work
		 * with the \c units, \c terrains, and \c hiddenunits objects. If a
		 * \c hiddenunits object is given in the override object, then it shall
		 * \em completely override the base weapon's \c hiddenunits object for that
		 * unit type. \c units and \c terrains objects will combine together, with
		 * the keys in the override objects replacing those in the base objects, if
		 * they exist in both. Values in the \c units and \c terrains override
		 * objects can also be boolean. If the value is \c true, then if the key
		 * has a value in the base object, it shall be retained. This has the same
		 * effect as simply leaving the key out of the override object so that the
		 * base object's value is kept. However, if \c false is stored, then the
		 * base object's value is removed and is not replaced with a new value.
		 * @param scriptName The identifier of this bank entry that is to be used
		 *                   within game scripts.
		 * @param j          The object value containing the unit type's
		 *                   properties.
		 * @sa    isInfiniteFuel()
		 */
		unit_type(const std::string& scriptName, engine::json& j);

		/**
		 * Registers \c unit_type with a given type.
		 * @tparam T        The type that is being registered, that inherits from
		 *                  \c unit_type in some way.
		 * @param  type     Name of the type to add the properties to.
		 * @param  engine   Pointer to the AngelScript script engine to register
		 *                  with.
		 * @param  document Pointer to the AngelScript documentation generator to
		 *                  register script interface documentation with.
		 * @safety No guarantee.
		 */
		template<typename T>
		static void Register(const std::string& type,
			asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		/**
		 * Retrieves the movement type of this unit.
		 * @return The index of the movement type of this unit.
		 */
		inline const std::string& getMovementTypeScriptName() const noexcept {
			return _movementTypeScriptName;
		}

		/**
		 * Retrieves a pointer to the details of the type of movement this unit
		 * has.
		 * @return The pointer to the movement type's properties.
		 * @sa     @c updateMovementType()
		 */
		inline std::shared_ptr<const awe::movement_type> getMovementType() const {
			return _movementType;
		}

		/**
		 * Updates the stored movement type properties pointer.
		 * @param movementBank A reference to the movement type bank to pull the
		 * pointer from.
		 */
		inline void updateMovementType(
			const awe::bank<awe::movement_type>& movementBank) const {
			_movementType = movementBank[_movementTypeScriptName];
		}

		/**
		 * Retrieves the sprite name of a given country's portrait of this unit.
		 * @param  countryName The script name of the country.
		 * @return The sprite name, or a blank string if the given country ID
		 *         didn't map to a sprite name in the internal list.
		 */
		inline const std::string& getPicture(
			const std::string& countryName) const {
			return ((_pictures.find(countryName) == _pictures.end()) ?
				(EMPTY_STRING) : (_pictures.at(countryName)));
		}

		/**
		 * Retrieves the sprite name of a given country's portrait of this unit.
		 * @param  countryID The turn order ID of the country.
		 * @return The sprite name, or a blank string if the given country ID
		 *         didn't map to a sprite name in the internal list.
		 */
		inline const std::string& getPicture(const awe::ArmyID countryID) const {
			return
				((_picturesTurnOrder.find(countryID) == _picturesTurnOrder.end()) ?
				(EMPTY_STRING) : (_picturesTurnOrder.at(countryID)));
		}

		/**
		 * Retrieves the sprite name of a given country's map sprite of this unit.
		 * @param  countryName The script name of the country.
		 * @return The sprite name, or a blank string if the given country ID
		 *         didn't map to a sprite name in the internal list.
		 */
		inline const std::string& getUnit(const std::string& countryName) const {
			return ((_units.find(countryName) == _units.end()) ? (EMPTY_STRING) :
				(_units.at(countryName)));
		}

		/**
		 * Retrieves the sprite name of a given country's map sprite of this unit.
		 * @param  countryID The turn order ID of the country.
		 * @return The sprite name, or a blank string if the given country ID
		 *         didn't map to a sprite name in the internal list.
		 */
		inline const std::string& getUnit(const awe::ArmyID countryID) const {
			return ((_unitsTurnOrder.find(countryID) == _unitsTurnOrder.end()) ?
				(EMPTY_STRING) : (_unitsTurnOrder.at(countryID)));
		}

		/**
		 * Retrieves the price property.
		 * @return The price property.
		 */
		inline unsigned int getCost() const noexcept {
			return _cost;
		}

		/**
		 * Retrieves the max fuel property.
		 * @return The max fuel property.
		 */
		inline int getMaxFuel() const noexcept {
			return _maxFuel;
		}

		/**
		 * Retrieves the max HP property.
		 * Note that the value returned will be an \em internal HP value, and not
		 * one that should be displayed to the user.
		 * @return The max HP property.
		 */
		inline unsigned int getMaxHP() const noexcept {
			return _maxHP;
		}

		/**
		 * Retrieves the MP property.
		 * @return The MP property.
		 */
		inline unsigned int getMovementPoints() const noexcept {
			return _movementPoints;
		}

		/**
		 * Retrieves the vision property.
		 * @return The vision property.
		 */
		inline unsigned int getVision() const noexcept {
			return _vision;
		}

		/**
		 * Finds out if this type of unit has infinite fuel.
		 * @return \c TRUE if \c _maxFuel is less than \c 0, \c FALSE otherwise.
		 */
		inline bool hasInfiniteFuel() const noexcept {
			return _maxFuel < 0;
		}

		/**
		 * Finds out if this type of unit can load onto it a given unit type.
		 * @param  typeScriptName The script name of the type of unit to test.
		 * @return \c TRUE if yes, \c FALSE otherwise.
		 */
		inline bool canLoad(const std::string& typeScriptName) const {
			return std::find(_canLoadThese.begin(), _canLoadThese.end(),
				typeScriptName) != _canLoadThese.end();
		}

		/**
		 * Retrieves the load limit of this unit type.
		 * @return The maximum number of units this unit can hold at any given
		 *         time.
		 */
		inline unsigned int loadLimit() const noexcept {
			return _loadLimit;
		}

		/**
		 * Retrieves the turn start priority level for this unit type.
		 * @return The priority level.
		 */
		inline unsigned int getTurnStartPriority() const noexcept {
			return _turnStartPriority;
		}

		/**
		 * Updates the stored unit type properties pointers for units that can be
		 * loaded onto this one.
		 * @param  unitBank A reference to the unit type bank to pull the pointers
		 *                  from.
		 * @safety Basic guarantee.
		 */
		void updateUnitTypes(const bank<unit_type>& unitBank) const;

		/**
		 * Finds out if this type of unit can capture a given terrain type.
		 * @param  typeScriptName The script name of the type of terrain to test.
		 * @return \c TRUE if yes, \c FALSE otherwise.
		 */
		inline bool canCapture(const std::string& typeScriptName) const {
			return std::find(_canCaptureThese.begin(), _canCaptureThese.end(),
				typeScriptName) != _canCaptureThese.end();
		}

		/**
		 * Finds out if this type of unit can unload from a given terrain type.
		 * @param  typeScriptName The script name of the type of terrain to test.
		 * @return \c TRUE if yes, \c FALSE otherwise.
		 */
		inline bool canUnloadFrom(const std::string& typeScriptName) const {
			return _canUnloadFromThese.empty() || std::find(
				_canUnloadFromThese.begin(), _canUnloadFromThese.end(),
				typeScriptName) != _canUnloadFromThese.end();
		}

		/**
		 * Finds out if this type of unit can hide.
		 * @return \c TRUE if yes, \c FALSE otherwise.
		 */
		inline bool canHide() const noexcept {
			return _canHide;
		}

		/**
		 * Updates the stored terrain type properties pointers stored in this unit
		 * type.
		 * @param  terrainBank A reference to the terrain type bank to pull the
		 *                     pointers from.
		 * @safety Basic guarantee.
		 */
		void updateTerrainTypes(const awe::bank<awe::terrain>& terrainBank) const;

		/**
		 * Retrieves a unit's weapon.
		 * @param  name The script name of the weapon.
		 * @return Pointer to the unit's weapon, \c nullptr if the given weapon
		 *         wasn't found.
		 */
		inline std::shared_ptr<const awe::weapon> getWeapon(
			const std::string& name) const {
			return ((_weapons.find(name) == _weapons.end()) ? (nullptr) :
				(_weapons.at(name)));
		}

		/**
		 * Retrieves the number of weapons that this unit has.
		 * @return The number of weapons.
		 */
		inline std::size_t getWeaponCount() const noexcept {
			return _weapons.size();
		}

		/**
		 * Retrieves a unit's weapon based on its position in the weapons list.
		 * @param  index 0-based index of the weapon.
		 * @return Pointer to the unit's weapon. If the index was invalid, then
		 *         \c nullptr will be returned.
		 */
		inline std::shared_ptr<const awe::weapon> getWeaponByIndex(
			const std::size_t index) const {
			return ((index >= getWeaponCount()) ? (nullptr) :
				(getWeapon(_baseWeapons.at(index).first)));
		}

		/**
		 * Finds the first weapon in this unit's list of weapons that possesses
		 * finite ammo.
		 * @return Pointer to the information on the weapon with finite ammo. If
		 *         no weapon with finite ammo could be found, \c nullptr.
		 */
		std::shared_ptr<const awe::weapon> getFirstWeaponWithFiniteAmmo() const;

		/**
		 * Sets up the weapons for a unit.
		 * "Copies" weapons from a given bank, and applies overrides, if any.
		 * Clears \c _weapons before copying.
		 * @warning \b Must be called before accessing any properties associated
		 *          with a unit's weapons.
		 * @param   weaponBank A reference to the weapons bank to pull information
		 *                     from.
		 * @param   sink       Pointer to the sink which JSON objects created in
		 *                     this method will output to.
		 * @safety  Basic guarantee.
		 */
		void updateWeapons(const awe::bank<awe::weapon>& weaponBank,
			const std::shared_ptr<engine::sink>& sink) const;

		/**
		 * Updates \c _picturesTurnOrder and \c _unitsTurnOrder by copying over the
		 * contents of their respective maps and supplying the respective turn
		 * order IDs as keys.
		 * @param  countries Pointer to the country bank to pull the turn order IDs
		 *                   from.
		 * @safety Basic guarantee.
		 */
		void updateSpriteMaps(const awe::bank<awe::country>& countries) const;

		/**
		 * Does this type of unit ignore terrain defence?
		 * @return \c TRUE if this unit always has 0 defence, regardless of what
		 *         terrain it is on.
		 */
		inline bool ignoresDefence() const noexcept {
			return _ignoreDefence;
		}
	private:
		/**
		 * Script interface version of \c getMovementType().
		 * @return The movement type's properties.
		 * @sa     @c getMovementType()
		 */
		inline const awe::movement_type* _getMovementTypeObj() const {
			return getMovementType().get();
		}

		/**
		 * Script version of \c getWeapon().
		 */
		inline const awe::weapon* _getWeapon(const std::string& name) const {
			// Will cause segfault if name doesn't exist...
			return getWeapon(name).get();
		}

		/**
		 * Script version of \c getWeaponByIndex().
		 */
		inline const awe::weapon* _getWeaponByIndex(
			const std::size_t index) const {
			// Will cause segfault if index doesn't exist...
			return getWeaponByIndex(index).get();
		}

		/**
		 * The movement type property.
		 */
		std::string _movementTypeScriptName;

		/**
		 * Pointer to this unit's movement typre details.
		 * It was made mutable so that it can be updated after construction in the
		 * \c bank constructor, via \c updateMovementType().
		 */
		mutable std::shared_ptr<const awe::movement_type> _movementType;

		/**
		 * The portrait IDs.
		 */
		std::unordered_map<std::string, std::string> _pictures;

		/**
		 * \c _pictures keyed by turn order ID.
		 */
		mutable std::unordered_map<awe::ArmyID, std::string> _picturesTurnOrder;

		/**
		 * The sprite IDs.
		 */
		std::unordered_map<std::string, std::string> _units;

		/**
		 * \c _units keyed by turn order ID.
		 */
		mutable std::unordered_map<awe::ArmyID, std::string> _unitsTurnOrder;

		/**
		 * The price property.
		 */
		unsigned int _cost = 0;

		/**
		 * The max fuel property.
		 */
		int _maxFuel = 0;

		/**
		 * The max HP property.
		 */
		unsigned int _maxHP = 100;

		/**
		 * The MP property.
		 */
		unsigned int _movementPoints = 0;

		/**
		 * The vision property.
		 */
		unsigned int _vision = 1;

		/**
		 * List of unit type script names that can be loaded onto this type of
		 * unit.
		 */
		std::vector<std::string> _canLoadThese;

		/**
		 * List of unit types that can be loaded onto this type of unit.
		 * It was made mutable so that it can be updated after construction in the
		 * \c bank constructor, via \c updateUnitTypes().
		 */
		mutable std::vector<std::shared_ptr<const awe::unit_type>>
			_canLoadTheseUnitTypes;

		/**
		 * The maximum number of units this unit type can load.
		 */
		unsigned int _loadLimit = 0;

		/**
		 * The turn start priority.
		 */
		unsigned int _turnStartPriority = 0;

		/**
		 * List of terrain type script names that this unit type can capture.
		 */
		std::vector<std::string> _canCaptureThese;

		/**
		 * List of terrain types that this unit type can capture.
		 * It was made mutable so that it can be updated after construction in the
		 * \c bank constructor, via \c updateTerrainTypes().
		 */
		mutable std::vector<std::shared_ptr<const awe::terrain>>
			_canCaptureTheseTerrainTypes;

		/**
		 * List of terrain type script names that this unit type can unload from.
		 */
		std::vector<std::string> _canUnloadFromThese;

		/**
		 * List of terrain types that this unit type can unload from.
		 * It was made mutable so that it can be updated after construction in the
		 * \c bank constructor, via \c updateTerrainTypes().
		 */
		mutable std::vector<std::shared_ptr<const awe::terrain>>
			_canUnloadFromTheseTerrainTypes;

		/**
		 * Can this unit hide?
		 */
		bool _canHide = false;

		/**
		 * The list of weapons assigned to this unit, and their overrides.
		 */
		std::vector<std::pair<std::string, nlohmann::ordered_json>> _baseWeapons;

		/**
		 * Each of the unit's weapons with overrides applied.
		 * 
		 */
		mutable std::unordered_map<std::string, std::shared_ptr<const awe::weapon>>
			_weapons;

		/**
		 * If \c TRUE, this type of unit should ignore terrain defence in damage
		 * calculations.
		 */
		bool _ignoreDefence = false;
	};

	/**
	 * A game property class which stores the information associated with a single
	 * commander.
	 * @sa \c awe::common_properties
	 */
	class commander : public awe::common_properties {
	public:
		/**
		 * Constructor which scans a JSON object for the portrait property.
		 * It also passes on the JSON object to the \c common_properties
		 * constructor. In addition to the keys defined in the superclass, the
		 * following keys are required:
		 * <ul><li>\c "portrait" = \c _portrait, <tt>(string)</tt></li></ul>
		 * @param scriptName The identifier of this bank entry that is to be used
		 *                   within game scripts.
		 * @param j          The object value containing the commander's
		 *                   properties.
		 */
		inline commander(const std::string& scriptName, engine::json& j) :
			common_properties(scriptName, j) {
			j.apply(_portrait, { "portrait" }, true);
		}

		/**
		 * Registers \c commander with a given type.
		 * @tparam T        The type that is being registered, that inherits from
		 *                  \c commander in some way.
		 * @param  type     Name of the type to add the properties to.
		 * @param  engine   Pointer to the AngelScript script engine to register
		 *                  with.
		 * @param  document Pointer to the AngelScript documentation generator to
		 *                  register script interface documentation with.
		 * @safety No guarantee.
		 */
		template<typename T>
		static void Register(const std::string& type,
			asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		/**
		 * Retrieves the animated sprite name of this commander's portrait.
		 * @return The animated sprite ID.
		 */
		inline const std::string& getPortrait() const noexcept {
			return _portrait;
		}
	private:
		/**
		 * The portrait property.
		 */
		std::string _portrait;
	};

	/**
	 * Calls \c terrain::updatePictureMap() on an entire bank of \c terrain
	 * objects.
	 * @param  terrainBank The \c terrain bank to update.
	 * @param  countryBank The \c country bank to pull turn order IDs from.
	 * @safety Basic guarantee.
	 */
	void updateTerrainBank(awe::bank<awe::terrain>& terrainBank,
		const awe::bank<awe::country>& countryBank);

	/**
	 * Calls \c tile_type::updateTerrain() and \c tile_type::updateOwnedTilesMap()
	 * on an entire bank of \c tile_type objects.
	 * @param  tileBank    The \c tile_type bank to update.
	 * @param  terrainBank The \c terrian bank to pull the pointers from.
	 * @param  countryBank The \c country bank to pull the turn order IDs from.
	 * @safety Basic guarantee.
	 */
	void updateTileTypeBank(awe::bank<awe::tile_type>& tileBank,
		const awe::bank<awe::terrain>& terrainBank,
		const awe::bank<awe::country>& countryBank);

	/**
	 * Calls \c unit_type::updateMovementType(), \c unit_type::updateUnitTypes(),
	 * \c unit_type::updateTerrainTypes(), \c unit_type::updateWeapons(), and
	 * \c unit_type::updateSpriteMaps() on an entire bank of \c unit_type objects.
	 * @param  unitBank     The \c unit_type bank to update. Also the \c unit_type
	 *                      bank that is used to update itself.
	 * @param  movementBank The \c movement_type bank to pull the pointers from.
	 * @param  terrainBank  The \c terrain bank to pull the pointers from.
	 * @param  weaponBank   The \c weapon bank to pull the pointers from.
	 * @param  countryBank  The \c country bank to pull turn order IDs from.
	 * @param  sink         Pointer to the sink which JSON objects created in this
	 *                      function will output to.
	 * @safety Basic guarantee.
	 */
	void updateUnitTypeBank(awe::bank<awe::unit_type>& unitBank,
		const awe::bank<awe::movement_type>& movementBank,
		const awe::bank<awe::terrain>& terrainBank,
		const awe::bank<awe::weapon>& weaponBank,
		const awe::bank<awe::country>& countryBank,
		const std::shared_ptr<engine::sink>& sink);

	/**
	 * Checks an entire bank of countries to ensure each country's turn order ID is
	 * unique and valid.
	 * @param  countries The country bank to validate.
	 * @return \c TRUE if each turn order ID is valid (i.e. they are unique and
	 *         are not equal to \c NO_ARMY.
	 */
	bool checkCountryTurnOrderIDs(const awe::bank<awe::country>& countries);
}

#include "tpp/bank.tpp"
