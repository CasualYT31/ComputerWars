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
		 * Releases AngelScript objects.
		 */
		~bank() noexcept;

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
		 * Returns a list of script names pointing to members of this bank.
		 * @return All the script names found within this bank, in the order they
		 *         were given.
		 */
		inline const std::vector<std::string>& getScriptNames() const {
			return _scriptNames;
		}

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
		inline CScriptArray* _getScriptNamesArray() const {
			if (_scriptNamesAsArray) _scriptNamesAsArray->AddRef();
			return _scriptNamesAsArray;
		}

		/**
		 * Internal logger object.
		 */
		mutable engine::logger _logger;

		/**
		 * The internal map of game properties.
		 */
		bank_type _bank;

		/**
		 * The list of script names given to this bank, in the order they were
		 * given.
		 */
		std::vector<std::string> _scriptNames;

		/**
		 * Copy of \c _scriptNames, for use with the scripts.
		 */
		CScriptArray* _scriptNamesAsArray = nullptr;

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

		/**
		 * Used by subclasses to return an empty Vector2i when attempting to
		 * retrieve a Vector2i property that doesn't exist.
		 * Should be initialised to <tt>(0, 0)</tt>.
		 * @sa \c EMPTY_STRING.
		 */
		static const sf::Vector2i EMPTY_VECTOR_I;
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
		 * There is an additional key-string pair required, \c "spritesheet". This
		 * key will store the name of the tile spritesheet to use with this
		 * environment.
		 * @param scriptName The identifier of this bank entry that is to be used
		 *                   within game scripts.
		 * @param j          The object value containing the environment's
		 *                   properties.
		 */
		inline environment(const std::string& scriptName, engine::json& j) :
			common_properties(scriptName, j) {
			j.apply(_spritesheet, { "spritesheet" }, true);
		}

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

		/**
		 * Retrieves the name of this environment's tile spritesheet.
		 * @return The name of the tile spritesheet.
		 */
		inline const std::string& getSpritesheet() const {
			return _spritesheet;
		}
	private:
		/**
		 * This environment's tile spritesheet.
		 */
		std::string _spritesheet;
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

	class tile_type;

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
		 *         string[, etc.]})</tt></li>
		 *     <li>\c "primarytiletype" = \c _primaryTileTypeScriptName, <tt>
		 *         (string, TILE_TYPE_SCRIPT_NAME)</tt></li></ul>
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
		 * returned, i.e. whatever was assigned to \c "icon".\n
		 *
		 * The \c primarytiletype object is used to define the tile type that
		 * represents this terrain. It can be left undefined. If the given tile
		 * type:
		 * <ol><li>Has a terrain that matches this one.</li>
		 *     <li>Is itself paintable.</li>
		 *     <li>Exists.<li></ol>
		 * Then a pointer to the tile type will be stored internally, and the
		 * terrain is considered \b paintable.
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
		 * The script name of the primary tile type for this terrain.
		 * @return The script name, if one was given.
		 */
		inline const std::string& getPrimaryTileTypeScriptName() const {
			return _primaryTileTypeScriptName;
		}

		/**
		 * Returns the primary tile type for this terrain, if this terrain is
		 * paintable.
		 * @return Pointer to the primary tile type. Will return \c nullptr if this
		 *         terrain is not paintable.
		 */
		inline std::shared_ptr<const awe::tile_type> getPrimaryTileType() const {
			return _primaryTileType;
		}

		/**
		 * Is this terrain paintable in the map maker?
		 * @return \c TRUE if this terrain has a primary tile type that is
		 *         paintable, and is thus itself paintable. \c FALSE otherwise.
		 */
		inline bool isPaintable() const {
			return _primaryTileType.operator bool();
		}

		/**
		 * Updates \c _picturesTurnOrder by copying over the contents of
		 * \c _pictures and supplying the respective turn order IDs as keys.
		 * @param  countries Pointer to the country bank to pull the turn order IDs
		 *                   from.
		 * @safety Basic guarantee.
		 */
		void updatePictureMap(const awe::bank<awe::country>& countries) const;

		/**
		 * Retrieves a pointer to the primary tile type from a given bank.
		 * If a primary tile type wasn't given, it didn't exist in the bank, it
		 * doesn't have a matching terrain, or it isn't paintable, then the pointer
		 * will not be assigned.
		 * @param tileBank A reference to the tile types bank to pull information
		 *                 from.
		 */
		void updateTileType(const awe::bank<awe::tile_type>& tileBank) const;
	private:
		/**
		 * Script interface version of \c getPrimaryTileType().
		 * @return The primary tile type's properties.
		 * @sa     @c getPrimaryTileType()
		 */
		inline const awe::tile_type* _getPrimaryTileTypeObj() const {
			return getPrimaryTileType().get();
		}

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

		/**
		 * The primary tile type's script name.
		 */
		std::string _primaryTileTypeScriptName;

		/**
		 * Pointer to this terrain's primary tile type, if it has one.
		 * It was made mutable so that it can be updated after construction in the
		 * \c bank constructor, via \c updateTileType().
		 */
		mutable std::shared_ptr<const awe::tile_type> _primaryTileType;
	};

	class structure;

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
		 *     <li>\c "tiles" = \c _ownedTiles, <tt>({COUNTRY_SCRIPT_NAME:
		 *         SPRITE_NAME (string)[, etc.]}  [string{, string, etc.}])</tt>
		 *         </li>
		 *     <li>\c "neutralproperty" = \c _neutralProperty,
		 *         <tt>(string, optional, default = "")</tt></li>
		 *     <li>\c "properties" = \c _ownedProperties,
		 *         <tt>({COUNTRY_SCRIPT_NAME: SPRITE_NAME (string)[, etc.]}
		 *         [string{, string, etc.}], optional, default = {})</tt></li></ul>
		 * The \c neutral key stores a sprite name shown when the tile is not owned
		 * by any country. This should be given for all tile types.\n
		 * The \c tiles object stores a list of animated sprite names associated
		 * with each country's version of the tile (i.e. when they own the tile).
		 * This object does not have to be accounted for if the tile cannot be
		 * owned/captured. In which case, the key and object can be omitted.\n
		 * The \c neutralproperty and \c properties keys store the sprites shown in
		 * the capturing animation when capturing this tile type. They must be
		 * given if the tile type's terrain is capturable.
		 * @param scriptName The identifier of this bank entry that is to be used
		 *                   within game scripts.
		 * @param j          The object value containing the tile type's
		 *                   properties.
		 */
		tile_type(const std::string& scriptName, engine::json& j);

		/**
		 * Releases the structure script name array held by \c tile_type.
		 */
		~tile_type() noexcept;

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
		 * Finds out if this tile type has owned tile sprites.
		 * @return \c TRUE if this tile type has at least one owned tile sprite.
		 *         \c FALSE if it has none.
		 */
		inline bool hasOwnedTiles() const {
			return !_ownedTiles.empty();
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
		 * Finds out if this tile type has owned property sprites.
		 * @return \c TRUE if this tile type has at least one owned property
		 *         sprite. \c FALSE if it has none.
		 */
		inline bool hasOwnedProperties() const {
			return !_ownedProperties.empty();
		}

		/**
		 * Retrieves the name of the property sprite that is shown for a given
		 * country.
		 * @param  countryName The script name of the country.
		 * @return The name of the property's sprite, or \c _neutralProperty if the
		 *         given country name didn't identify a sprite name.
		 */
		inline const std::string& getOwnedProperty(
			const std::string& countryName) const {
			return ((_ownedProperties.find(countryName) == _ownedProperties.end())
				? (getNeutralProperty()) : (_ownedProperties.at(countryName)));
		}

		/**
		 * Retrieves the name of the property sprite that is shown for a given
		 * country.
		 * @param  countryID The turn order ID of the country.
		 * @return The name of the property's sprite, or \c _neutralProperty if the
		 *         given country name didn't identify a sprite name.
		 */
		inline const std::string& getOwnedProperty(
			const awe::ArmyID countryID) const {
			return ((_ownedPropertiesTurnOrder.find(countryID) ==
				_ownedPropertiesTurnOrder.end()) ?
				(getNeutralProperty()) :
				(_ownedPropertiesTurnOrder.at(countryID)));
		}

		/**
		 * Retrieves the name of the property sprite that is shown when no country
		 * owns the tile.
		 * @return The name of the property's neutral sprite.
		 */
		inline const std::string& getNeutralProperty() const noexcept {
			return _neutralProperty;
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
		 * Is this tile type always paintable via the Tiles tab in the map maker,
		 * even if it is a tile used in a structure?
		 * @return \c TRUE if the tile type is always paintable individually,
		 *         \c FALSE if not (default).
		 */
		inline bool alwaysPaintable() const noexcept {
			return _alwaysPaintable;
		}

		/**
		 * Is this tile type paintable via the Tiles tab in the map maker?
		 * @return \c TRUE if the tile type is either always paintable, or is part
		 *         of no structure (as a normal, destroyed, or deleted tile),
		 *         \c FALSE otherwise.
		 */
		inline bool isPaintable() const noexcept {
			return _structures.empty() || _alwaysPaintable;
		}

		/**
		 * Which structures is this tile type a member of?
		 * @return List of structures this tile type is a member of, either as the
		 *         root or as a dependent.
		 */
		inline const std::vector<std::shared_ptr<const awe::structure>>&
			structures() const noexcept {
			return _structures;
		}

		/**
		 * Which structures is this tile type a member of?
		 * @return Array of script names of structures this tile type is a member
		 *         of.
		 */
		inline const CScriptArray* const structureScriptNames() const noexcept {
			if (_structureScriptNames) _structureScriptNames->AddRef();
			return _structureScriptNames;
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

		/**
		 * Updates \c _structures with a list of structures that this type type is
		 * included in.
		 * @param structureBank Pointer to the structure bank to pull information
		 *                      from.
		 * @param scripts       The scripts engine used to allocate the script name
		 *                      array.
		 */
		void updateStructures(
			const awe::bank<awe::structure>& structureBank,
			const std::shared_ptr<engine::scripts>& scripts) const;
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

		/**
		 * The names of the large property sprites corresponding to each country.
		 * Should be filled in if the terrain type is capturable.
		 */
		std::unordered_map<std::string, std::string> _ownedProperties;

		/**
		 * \c _ownedProperties keyed by turn order ID.
		 */
		mutable std::unordered_map<awe::ArmyID, std::string>
			_ownedPropertiesTurnOrder;

		/**
		 * The name of the large property sprite with no owner.
		 * Should be set if the terrain type is capturable.
		 */
		std::string _neutralProperty;

		/**
		 * If \c TRUE, the tile is always paintable individually, even if it forms
		 * part of at least one structure.
		 */
		bool _alwaysPaintable = false;

		/**
		 * List of structures this tile is a member of.
		 */
		mutable std::vector<std::shared_ptr<const awe::structure>> _structures;

		/**
		 * List of script names of structures this tile is a member of.
		 */
		mutable CScriptArray* _structureScriptNames = nullptr;
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
		 *     <li>\c "spritesheets" is an object with the following keys:
		 *         <ul><li>\c "idle" = \c _idleSpritesheet, <tt>(string)</tt></li>
		 *             <li>\c "up" = \c _upSpritesheet, <tt>(string)</tt></li>
		 *             <li>\c "down" = \c _downSpritesheet, <tt>(string)</tt></li>
		 *             <li>\c "left" = \c _leftSpritesheet, <tt>(string)</tt></li>
		 *             <li>\c "right" = \c _rightSpritesheet, <tt>(string)</tt>
		 *                 </li>
		 *             <li>\c "selected" = \c _selectedSpritesheets,
		 *                 <tt>{COUNTRY_SCRIPT_NAME: string SHEET_NAME[, etc.]}
		 *                 </tt></li></ul>
		 *     </li>
		 *     <li>\c "sounds" is an object with the following keys:
		 *         <ul><li>\c "hide" = \c _sound_hide, <tt>(string)</tt></li>
		 *             <li>\c "unhide" = \c _sound_unhide, <tt>(string)</tt></li>
		 *             <li>\c "destroy" = \c _sound_destroy, <tt>(string)</tt></li>
		 *             <li>\c "move" = \c _sound_move[false], <tt>(string)</tt>
		 *                 -OR-
		 *                 \c _sound_move_on_terrain[TERRAIN_SCRIPT_NAME][false]
		 *                 <tt>({TERRAIN_SCRIPT_NAME: string[, etc.]})</tt></li>
		 *             <li>\c "movehidden" = same as \c "move" except \c bool key
		 *                 is \c TRUE not \c FALSE.</li></ul>
		 *     </li>
		 *     <li>\c "sprites" = \c _units, <tt>{COUNTRY_SCRIPT_NAME: string
		 *         SPRITE_NAME[, etc.]}</tt></li>
		 *     <li>\c "destroyedsprites" = \c _destroyedUnits,
		 *         <tt>{COUNTRY_SCRIPT_NAME: string SPRITE_NAME[, etc.]}</tt> OR
		 *         <tt>string SPRITE_NAME</tt></li>
		 *     <li>\c "capturingsprites" = \c _capturingUnits,
		 *         <tt>({COUNTRY_SCRIPT_NAME: string SPRITE_NAME[, etc.]},
		 *         optional, default = {})</tt></li>
		 *     <li>\c "capturedsprites" = \c _capturedUnits,
		 *         <tt>({COUNTRY_SCRIPT_NAME: string SPRITE_NAME[, etc.]},
		 *         optional, default = {})</tt></li>
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
		 * Pictures is a dictionary of sprite names corresponding to each country's
		 * portrait of the type of unit.\n
		 *
		 * Sprites is a dictionary of sprite names corresponding to each country's
		 * map representation of the type of unit.\n
		 *
		 * DestroyedSprites is a dictionary of sprite IDs corresponding to the
		 * particle that is momentarily displayed on the map when the unit is
		 * destroyed, keyed on country script name. If a single string is given
		 * instead of a dictionary, then that sprite will be used for all
		 * countries.\n
		 *
		 * CapturingSprites and CapturedSprites are the larger graphics that are
		 * shown during this unit's capturing animation. The former is shown when
		 * the unit begins a capture, and the latter is shown when the unit
		 * completes a capture. They should be given if this unit type can
		 * capture.\n
		 *
		 * Moving sounds can either be a single string, or an object of strings
		 * keyed on terrain type script name. If a single string is given, that
		 * will be the sound played when a unit moves across all terrain types. If
		 * an object is given, then different sounds will play when a unit moves
		 * across different terrains, whose script names are used as keys. The
		 * first non-empty value in this object will also be defined as the default
		 * move sound, so if a terrain does not have a sound, or if the default
		 * move sound is queried, this sound will be played. Different move sounds
		 * can also be defined for if the unit is hidden. If a move sound's
		 * non-hidden version is defined, but not its hidden version, then the
		 * hidden version will be set to the non-hidden version, and vice versa.
		 * This means the only way a unit can have no move sound associated with it
		 * is by giving no values to either \c "move" or \c "movehidden". A unit
		 * cannot have a sound for moving and no sound for hidden moving.\n
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
		 * Retrieves the name of the idle spritesheet.
		 * @return The name of the spritesheet.
		 */
		inline const std::string& getIdleSpritesheet() const {
			return _idleSpritesheet;
		}

		/**
		 * Retrieves the name of the moving up spritesheet.
		 * @return The name of the spritesheet.
		 */
		inline const std::string& getUpSpritesheet() const {
			return _upSpritesheet;
		}

		/**
		 * Retrieves the name of the moving down spritesheet.
		 * @return The name of the spritesheet.
		 */
		inline const std::string& getDownSpritesheet() const {
			return _downSpritesheet;
		}

		/**
		 * Retrieves the name of the moving left spritesheet.
		 * @return The name of the spritesheet.
		 */
		inline const std::string& getLeftSpritesheet() const {
			return _leftSpritesheet;
		}

		/**
		 * Retrieves the name of the moving right spritesheet.
		 * @return The name of the spritesheet.
		 */
		inline const std::string& getRightSpritesheet() const {
			return _rightSpritesheet;
		}

		/**
		 * Retrieves the name of a given country's selected unit spritesheet.
		 * @param  countryName The script name of the country.
		 * @return The sheet name, or a blank string if the given country ID didn't
		 *         map to a sheet name in the internal list.
		 */
		inline const std::string& getSelectedSpritesheet(
			const std::string& countryName) const {
			return ((_selectedSpritesheets.find(countryName) ==
				_selectedSpritesheets.end()) ? (EMPTY_STRING) :
				(_selectedSpritesheets.at(countryName)));
		}

		/**
		 * Retrieves the name of a given country's selected unit spritesheet.
		 * @param  countryID The turn order ID of the country.
		 * @return The sheet name, or a blank string if the given country ID didn't
		 *         map to a sheet name in the internal list.
		 */
		inline const std::string& getSelectedSpritesheet(
			const awe::ArmyID countryID) const {
			return ((_selectedSpritesheetsTurnOrder.find(countryID) ==
				_selectedSpritesheetsTurnOrder.end()) ? (EMPTY_STRING) :
				(_selectedSpritesheetsTurnOrder.at(countryID)));
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
		 * Retrieves the sprite name of a given country's map sprite of this unit
		 * when it is destroyed.
		 * @param  countryName The script name of the country.
		 * @return The sprite name, or a blank string if the given country ID
		 *         didn't map to a sprite name in the internal list. If the
		 *         internal list is empty, \c _destroyedUnitForAll.
		 */
		inline const std::string& getDestroyedUnit(
			const std::string& countryName) const {
			if (_destroyedUnits.empty()) return _destroyedUnitForAll;
			return ((_destroyedUnits.find(countryName) == _destroyedUnits.end()) ?
				(EMPTY_STRING) : (_destroyedUnits.at(countryName)));
		}

		/**
		 * Retrieves the sprite name of a given country's map sprite of this unit
		 * when it is destroyed.
		 * @param  countryID The turn order ID of the country.
		 * @return The sprite name, or a blank string if the given country ID
		 *         didn't map to a sprite name in the internal list. If the
		 *         internal list is empty, \c _destroyedUnitForAll.
		 */
		inline const std::string& getDestroyedUnit(
			const awe::ArmyID countryID) const {
			if (_destroyedUnitsTurnOrder.empty()) return _destroyedUnitForAll;
			return ((_destroyedUnitsTurnOrder.find(countryID) ==
				_destroyedUnitsTurnOrder.end()) ? (EMPTY_STRING) :
				(_destroyedUnitsTurnOrder.at(countryID)));
		}

		/**
		 * Retrieves the sprite name of a given country's capturing sprite of this
		 * unit.
		 * @param  countryName The script name of the country.
		 * @return The sprite name, or a blank string if the given country ID
		 *         didn't map to a sprite name in the internal list.
		 */
		inline const std::string& getCapturingUnit(
			const std::string& countryName) const {
			return ((_capturingUnits.find(countryName) == _capturingUnits.end()) ?
				(EMPTY_STRING) : (_capturingUnits.at(countryName)));
		}

		/**
		 * Retrieves the sprite name of a given country's capturing sprite of this
		 * unit.
		 * @param  countryID The turn order ID of the country.
		 * @return The sprite name, or a blank string if the given country ID
		 *         didn't map to a sprite name in the internal list.
		 */
		inline const std::string& getCapturingUnit(
			const awe::ArmyID countryID) const {
			return ((_capturingUnitsTurnOrder.find(countryID) ==
				_capturingUnitsTurnOrder.end()) ? (EMPTY_STRING) :
				(_capturingUnitsTurnOrder.at(countryID)));
		}

		/**
		 * Retrieves the sprite name of a given country's captured sprite of this
		 * unit.
		 * @param  countryName The script name of the country.
		 * @return The sprite name, or a blank string if the given country ID
		 *         didn't map to a sprite name in the internal list.
		 */
		inline const std::string& getCapturedUnit(
			const std::string& countryName) const {
			return ((_capturedUnits.find(countryName) == _capturedUnits.end()) ?
				(EMPTY_STRING) : (_capturedUnits.at(countryName)));
		}

		/**
		 * Retrieves the sprite name of a given country's captured sprite of this
		 * unit.
		 * @param  countryID The turn order ID of the country.
		 * @return The sprite name, or a blank string if the given country ID
		 *         didn't map to a sprite name in the internal list.
		 */
		inline const std::string& getCapturedUnit(
			const awe::ArmyID countryID) const {
			return ((_capturedUnitsTurnOrder.find(countryID) ==
				_capturedUnitsTurnOrder.end()) ? (EMPTY_STRING) :
				(_capturedUnitsTurnOrder.at(countryID)));
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

		/**
		 * The name of the sound played when this type of unit hides.
		 * @return The name of the sound, or an empty string if there is no sound.
		 */
		inline std::string getHideSound() const {
			return _sound_hide;
		}

		/**
		 * The name of the sound played when this type of unit unhides.
		 * @return The name of the sound, or an empty string if there is no sound.
		 */
		inline std::string getUnhideSound() const {
			return _sound_unhide;
		}

		/**
		 * The name of the sound played when this type of unit is destroyed.
		 * @return The name of the sound, or an empty string if there is no sound.
		 */
		inline std::string getDestroySound() const {
			return _sound_destroy;
		}

		/**
		 * The name of the sound played when this type of unit is moving.
		 * @param  terrain The script name of the terrain the unit is moving on. If
		 *                 this unit has a special sound for this terrain, it will
		 *                 be returned. If not, or if this parameter is an empty
		 *                 string, the unit's "default" moving sound will be
		 *                 returned.
		 * @param  hidden  If \c TRUE, the returned sound is the unit's moving
		 *                 sound when it is hidden. If \c FALSE, it will be the
		 *                 sound when it is not hidden.
		 * @return The name of the sound, or an empty string if there is no sound.
		 */
		inline std::string getMoveSound(const std::string& terrain = "",
			const bool hidden = false) const {
			if (!terrain.empty() && _sound_move_on_terrain.find(terrain) !=
				_sound_move_on_terrain.end())
				return _sound_move_on_terrain.at(terrain).at(hidden);
			return _sound_move.at(hidden);
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
		 * Pointer to this unit's movement type details.
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
		 * The name of the spritesheet containing the idle unit map sprites.
		 */
		std::string _idleSpritesheet;

		/**
		 * The name of the spritesheet containing unit map sprites that are moving
		 * up.
		 */
		std::string _upSpritesheet;

		/**
		 * The name of the spritesheet containing unit map sprites that are moving
		 * down.
		 */
		std::string _downSpritesheet;

		/**
		 * The name of the spritesheet containing unit map sprites that are moving
		 * left.
		 */
		std::string _leftSpritesheet;

		/**
		 * The name of the spritesheet containing unit map sprites that are moving
		 * right.
		 */
		std::string _rightSpritesheet;

		/**
		 * The names of the spritesheets to use when a unit has been selected.
		 */
		std::unordered_map<std::string, std::string> _selectedSpritesheets;

		/**
		 * \c _selectedSpritesheets keyed by turn order ID.
		 */
		mutable std::unordered_map<awe::ArmyID, std::string>
			_selectedSpritesheetsTurnOrder;

		/**
		 * The sprite IDs.
		 */
		std::unordered_map<std::string, std::string> _units;

		/**
		 * \c _units keyed by turn order ID.
		 */
		mutable std::unordered_map<awe::ArmyID, std::string> _unitsTurnOrder;

		/**
		 * The ID to use if \c _destroyedUnits is not populated.
		 */
		std::string _destroyedUnitForAll;

		/**
		 * The IDs for sprites that are shown when this unit is destroyed.
		 */
		std::unordered_map<std::string, std::string> _destroyedUnits;

		/**
		 * \c _destroyedUnits keyed by turn order ID.
		 */
		mutable std::unordered_map<awe::ArmyID, std::string>
			_destroyedUnitsTurnOrder;

		/**
		 * The IDs of sprites that are shown when this unit is capturing.
		 */
		std::unordered_map<std::string, std::string> _capturingUnits;

		/**
		 * \c _capturingUnits keyed by turn order ID.
		 */
		mutable std::unordered_map<awe::ArmyID, std::string>
			_capturingUnitsTurnOrder;

		/**
		 * The IDs of sprites that are shown when this unit has completed a
		 * capture.
		 */
		std::unordered_map<std::string, std::string> _capturedUnits;

		/**
		 * \c _capturedUnits keyed by turn order ID.
		 */
		mutable std::unordered_map<awe::ArmyID, std::string>
			_capturedUnitsTurnOrder;

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
		 */
		mutable std::unordered_map<std::string, std::shared_ptr<const awe::weapon>>
			_weapons;

		/**
		 * If \c TRUE, this type of unit should ignore terrain defence in damage
		 * calculations.
		 */
		bool _ignoreDefence = false;

		/**
		 * This unit's hide sound.
		 */
		std::string _sound_hide;

		/**
		 * This unit's unhide sound.
		 */
		std::string _sound_unhide;

		/**
		 * The unit's destroy sound.
		 */
		std::string _sound_destroy;

		/**
		 * The unit's default move sound.
		 * Key differentiates movement sound whilst hidden (\c TRUE) from not
		 * hidden (\c FALSE).
		 * @warning Both \c TRUE and \c FALSE keys need to be stored, even if the
		 *          stored value is an empty string.
		 */
		std::unordered_map<bool, std::string> _sound_move;

		/**
		 * The unit's move sounds for each terrain type.
		 * Key differentiates movement sound whilst hidden (\c TRUE) from not
		 * hidden (\c FALSE).
		 * @warning Both \c TRUE and \c FALSE keys need to be stored for each
		 *          terrain given.
		 */
		std::unordered_map<std::string, std::unordered_map<bool, std::string>>
			_sound_move_on_terrain;
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
		 * <ul><li>\c "portrait" = \c _portrait, <tt>(string)</tt></li>
		 *     <li>\c "theme" = \c _theme, <tt>(string)</tt></li></ul>
		 * @param scriptName The identifier of this bank entry that is to be used
		 *                   within game scripts.
		 * @param j          The object value containing the commander's
		 *                   properties.
		 */
		commander(const std::string& scriptName, engine::json& j);

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

		/**
		 * Retrieves the music name of this commander's theme.
		 * @return The music name.
		 */
		inline const std::string& getTheme() const noexcept {
			return _theme;
		}
	private:
		/**
		 * The portrait property.
		 */
		std::string _portrait;

		/**
		 * The name of the commander's theme music.
		 */
		std::string _theme;
	};

	/**
	 * A game property class which stores the information associated with a single
	 * structure.
	 * A structure is made up of a single root tile and several optional dependent
	 * tiles. The root tile represents the structure as far as the game engine is
	 * concerned. The root tile is assigned an owner, and a structure can perform
	 * different tasks at the beginning of its owner's turn, like properties (but
	 * before properties). For example, a Black Cannon's root tile would be
	 * assigned to the Black Hole army on the map, so that at the beginning of
	 * Black Hole's turn, the cannon can fire. A volcano's root tile could be
	 * assigned to Orange Star, so that at the beginning of every turn (except the
	 * first one), it will erupt. A HQ, which has no dependent tiles, can be
	 * assigned to each army, and will heal units and award funds like a city. For
	 * some structures, assigning the root to the correct tile is important. For
	 * example, a Big Cannon structure might fire at units within a certain range
	 * of its root tile, which is assigned to be the weak point of the cannon. For
	 * other structures, the root tile can be any tile of the structure and it
	 * would make no functional difference. For example, the volcano's root tile
	 * could be any of its tiles, but it will still need a root tile in order to
	 * know when to erupt.\n
	 * Destroying a structure will mean transforming all of its tiles into
	 * different types, and removing ownership of the root tile, to prevent
	 * carrying out the structure's tasks every turn. Deleting a structure only
	 * occurs in the map maker, when one of the structure's tiles is painted on,
	 * the whole structure should transform to the deleted tile types (which can be
	 * plains, for example) so as not to leave a structure partially constructed.
	 * There is no technical reason why a structure should have all of its
	 * dependent tiles in the correct state: dependent tiles are only useful in the
	 * map maker, so that it knows what tiles to paint where when placing a
	 * structure. Dependent tiles are only useful gameplay-wise when destroying a
	 * structure, and even then they do not provide any game logic, they are only
	 * there for the graphics.
	 */
	class structure : public awe::common_properties {
	public:
		/**
		 * Constructor which scans a JSON object for the structure's properties.
		 * It also passes on the JSON object to the \c common_properties
		 * constructor. In addition to the keys defined in the superclass, the
		 * following keys are are to be/can be defined:
		 * <ul><li>\c "root", <b>mandatory</b>, <tt>(object)</tt></li>
		 *     <li>\c "dependent", <em>optional</em>, <tt>([object, ...])</tt></li>
		 *     <li>\c "ownedicons", <em>optional</em>, <tt>{COUNTRY_SCRIPT_NAME:
		 *         string SPRITE_NAME[, etc.]}</tt></li>
		 *     <li>\c "paintable" = \c _paintable, <em>optional</em>,
		 *         <tt>(bool)</tt></li>
		 *     <li>\c "keepunits" = \c _keepUnits, <em>optional</em>,
		 *         <tt>(bool)</tt></li>
		 *     <li>\c "destroyedlongname" = \c _destroyedLongName,
		 *         <em>optional</em>, <tt>(string)</tt></li>
		 *     <li>\c "destroyediconname" = \c _destroyedIconName,
		 *         <em>optional</em>, <tt>(string)</tt></li></ul>
		 *
		 * The \c "root" object is made up of the following keys:
		 * <ul><li>\c "tile" = \c _rootTile, <em>mandatory</em>, <tt>
		 *         (TILE_TYPE_SCRIPT_NAME: string)</tt></li>
		 *     <li>\c "destroyed" = \c _rootDestroyedTile, <em>optional</em>, <tt>
		 *         (TILE_TYPE_SCRIPT_NAME: string)</tt></li>
		 *         \c "deleted" = \c _rootDeletedTile, <em>optional</em>, <tt>
		 *         (TILE_TYPE_SCRIPT_NAME: string)</tt></li></ul>
		 *
		 * Each \c "dependent" object is made up of the following keys:
		 * <ul><li>\c "tile" = \c dependent_tile::tile, <em>mandatory</em>, <tt>
		 *         (TILE_TYPE_SCRIPT_NAME: string)</tt></li>
		 *     <li>\c "offset" = \c dependent_tile::offset, <em>mandatory</em>,
		 *         <tt>([signed 32-bit int, signed 32-bit int])</tt>)
		 *     <li>\c "destroyed" = \c dependent_tile::destroyedTile, <em>optional
		 *         </em>, <tt>(TILE_TYPE_SCRIPT_NAME: string)</tt></li>
		 *         \c "deleted" = \c dependent_tile::deletedTile, <em>optional
		 *         </em>, <tt>(TILE_TYPE_SCRIPT_NAME: string)</tt></li></ul>
		 *
		 * Each dependent tile must have a unique offset! If a second dependent
		 * tile is given with the same offset as one previous, the second tile will
		 * be dropped! That offset must also not be <tt>[0, 0]</tt>, since that
		 * represents the root tile.\n\n
		 *
		 * @warning If a structure is not paintable, all of its dependent tiles
		 *          will be removed, and its root tile type \b must be unique
		 *          across non-paintable structures! Code that searches through
		 *          structures to find a non-paintable structure that has a given
		 *          root tile type should always select the structure found first
		 *          to at least try and maintain consistency if this constraint is
		 *          not followed.
		 * @param   scriptName The identifier of this bank entry that is to be used
		 *                     within game scripts.
		 * @param   j          The object value containing the structure's
		 *                     properties.
		 */
		structure(const std::string& scriptName, engine::json& j);

		/**
		 * Registers \c structure with a given type.
		 * @tparam T        The type that is being registered, that inherits from
		 *                  \c structure in some way.
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
		 * Finds out if this structure has owned icon sprites.
		 * @return \c TRUE if this structure has at least one owned icon sprite.
		 *         \c FALSE if it has none.
		 */
		inline bool hasOwnedIcons() const {
			return !_ownedIcons.empty();
		}

		/**
		 * Retrieves the name of the sprite that is shown for a given country.
		 * @param  countryName The script name of the country.
		 * @return The name of the structure's icon sprite, or \c _iconName if the
		 *         given country name didn't identify a sprite name.
		 */
		inline const std::string& getOwnedIconName(
			const std::string& countryName) const {
			return ((_ownedIcons.find(countryName) == _ownedIcons.end()) ?
				(getIconName()) : (_ownedIcons.at(countryName)));
		}

		/**
		 * Retrieves the name of the sprite that is shown for a given country.
		 * @param  countryID The turn order ID of the country.
		 * @return The name of the structure's icon sprite, or \c _iconName if the
		 *         given country name didn't identify a sprite name.
		 */
		inline const std::string& getOwnedIconName(
			const awe::ArmyID countryID) const {
			return ((_ownedIconsTurnOrder.find(countryID) ==
				_ownedIconsTurnOrder.end()) ?
				(getIconName()) : (_ownedIconsTurnOrder.at(countryID)));
		}

		/**
		 * The tile type to be assigned to the root tile of the structure.
		 * @return The script name of the root tile's type.
		 */
		inline const std::string& getRootTileTypeScriptName() const noexcept {
			return _rootTile;
		}

		/**
		 * The tile type to be assigned to the root tile of the structure.
		 * @return Pointer to the root tile's type.
		 * @sa     \c updateTileTypes().
		 */
		inline std::shared_ptr<const awe::tile_type>
			getRootTileType() const noexcept {
			return _rootTileType;
		}

		/**
		 * Does the root tile have a destroyed tile type?
		 * @return \c TRUE if yes, \c FALSE if not.
		 */
		inline bool hasRootDestroyedTileType() const noexcept {
			return !_rootDestroyedTile.empty();
		}

		/**
		 * The tile type to be assigned to the root tile of the structure when the
		 * structure is destroyed.
		 * If one was not given for this structure, the script name of the normal
		 * root tile type is returned.
		 * @return The script name of the destroyed root tile's type.
		 */
		inline const std::string&
			getRootDestroyedTileTypeScriptName() const noexcept {
			return hasRootDestroyedTileType() ? _rootDestroyedTile :
				getRootTileTypeScriptName();
		}

		/**
		 * The tile type to be assigned to the root tile of the structure when the
		 * structure is destroyed.
		 * If one was not given for this structure, the pointer to the normal root
		 * tile type is returned.
		 * @return Pointer to the destroyed root tile's type.
		 * @sa     \c updateTileTypes().
		 */
		inline std::shared_ptr<const awe::tile_type>
			getRootDestroyedTileType() const {
			return hasRootDestroyedTileType() ? _rootDestroyedTileType :
				getRootTileType();
		}

		/**
		 * Does the root tile have a deleted tile type?
		 * @return \c TRUE if yes, \c FALSE if not.
		 */
		inline bool hasRootDeletedTileType() const noexcept {
			return !_rootDeletedTile.empty();
		}

		/**
		 * The tile type to be assigned to the root tile of the structure when the
		 * structure is deleted.
		 * If one was not given for this structure, the script name of the normal
		 * root tile type is returned.
		 * @return The script name of the root tile's type when the root tile is
		 *         deleted.
		 */
		inline const std::string&
			getRootDeletedTileTypeScriptName() const noexcept {
			return hasRootDeletedTileType() ? _rootDeletedTile :
				getRootTileTypeScriptName();
		}

		/**
		 * The tile type to be assigned to the root tile of the structure when the
		 * structure is deleted.
		 * If one was not given for this structure, the pointer to the normal root
		 * tile type is returned.
		 * @return Pointer to the root tile's type when the root tile is deleted.
		 * @sa     \c updateTileTypes().
		 */
		inline std::shared_ptr<const awe::tile_type>
			getRootDeletedTileType() const {
			return hasRootDeletedTileType() ? _rootDeletedTileType :
				getRootTileType();
		}

		/**
		 * Retrieves the number of dependent tiles this structure is to have.
		 * @return The number of dependent tiles.
		 */
		inline std::size_t getDependentTileCount() const noexcept {
			return _dependents.size();
		}

		/**
		 * Retrieves the offset to be applied to the specified dependent tile.
		 * @param  index The 0-based index of the dependent tile.
		 * @return The offset in tiles. <tt>(0, 0)</tt> if the given index was out
		 *         of bounds.
		 */
		inline const sf::Vector2i& getDependentTileOffset(
			const std::size_t index) const noexcept {
			return index >= getDependentTileCount() ? EMPTY_VECTOR_I :
				_dependents[index].offset;
		}

		/**
		 * The tile type to be assigned to the specified dependent tile of the
		 * structure.
		 * @warning Returns the equivalent root tile type if an out-of-bounds
		 *          index is given!
		 * @param   index The 0-based index of the dependent tile.
		 * @return  The script name of the root tile's type.
		 */
		inline const std::string& getDependentTileTypeScriptName(
			const std::size_t index) const noexcept {
			return index >= getDependentTileCount() ? getRootTileTypeScriptName() :
				_dependents[index].tile;
		}

		/**
		 * The tile type to be assigned to the specified dependent tile of the
		 * structure.
		 * @warning Returns the equivalent root tile type if an out-of-bounds
		 *          index is given!
		 * @param   index The 0-based index of the dependent tile.
		 * @return  Pointer to the root tile's type.
		 * @sa      \c updateTileTypes().
		 */
		inline std::shared_ptr<const awe::tile_type> getDependentTileType(
			const std::size_t index) const {
			return index >= getDependentTileCount() ? getRootTileType() :
				_dependents[index].tileType;
		}

		/**
		 * Does the specified dependent tile have a destroyed tile type?
		 * @param  index The 0-based index of the dependent tile.
		 * @return \c TRUE if yes, \c FALSE if not.
		 */
		inline bool hasDependentDestroyedTileType(
			const std::size_t index) const noexcept {
			return index >= getDependentTileCount() ? false :
				_dependents[index].hasDestroyedTileType();
		}

		/**
		 * The tile type to be assigned to the specified dependent tile of the
		 * structure when the structure is destroyed.
		 * If one was not given for this structure, the script name of the normal
		 * specified dependent tile type is returned.
		 * @warning Returns the equivalent root tile type if an out-of-bounds
		 *          index is given!
		 * @param   index The 0-based index of the dependent tile.
		 * @return  The script name of the destroyed specified dependent tile's
		 *          type.
		 */
		inline const std::string& getDependentDestroyedTileTypeScriptName(
			const std::size_t index) const noexcept {
			return index >= getDependentTileCount() ?
				getRootDestroyedTileTypeScriptName() : (
					hasDependentDestroyedTileType(index) ?
					_dependents[index].destroyedTile :
					getDependentTileTypeScriptName(index)
					);
		}

		/**
		 * The tile type to be assigned to the specified dependent tile of the
		 * structure when the structure is destroyed.
		 * If one was not given for this structure, the pointer to the normal
		 * specified dependent tile type is returned.
		 * @warning Returns the equivalent root tile type if an out-of-bounds
		 *          index is given!
		 * @param   index The 0-based index of the dependent tile.
		 * @return  Pointer to the destroyed specified dependent tile's type.
		 * @sa      \c updateTileTypes().
		 */
		inline std::shared_ptr<const awe::tile_type> getDependentDestroyedTileType(
			const std::size_t index) const {
			return index >= getDependentTileCount() ?
				getRootDestroyedTileType() : (
					hasDependentDestroyedTileType(index) ?
					_dependents[index].destroyedTileType :
					getDependentTileType(index)
					);
		}

		/**
		 * Does the specified dependent tile have a deleted tile type?
		 * @param  index The 0-based index of the dependent tile.
		 * @return \c TRUE if yes, \c FALSE if not.
		 */
		inline bool hasDependentDeletedTileType(
			const std::size_t index) const noexcept {
			return index >= getDependentTileCount() ? false :
				_dependents[index].hasDeletedTileType();
		}

		/**
		 * The tile type to be assigned to the specified dependent tile of the
		 * structure when the structure is deleted.
		 * If one was not given for this structure, the script name of the normal
		 * specified dependent tile type is returned.
		 * @warning Returns the equivalent root tile type if an out-of-bounds
		 *          index is given!
		 * @param   index The 0-based index of the dependent tile.
		 * @return  The script name of the dependent tile's type when the specified
		 *          dependent tile is deleted.
		 */
		inline const std::string& getDependentDeletedTileTypeScriptName(
			const std::size_t index) const noexcept {
			return index >= getDependentTileCount() ?
				getRootDeletedTileTypeScriptName() : (
					hasDependentDeletedTileType(index) ?
					_dependents[index].deletedTile :
					getDependentTileTypeScriptName(index)
					);
		}

		/**
		 * The tile type to be assigned to the specified dependent tile of the
		 * structure when the structure is deleted.
		 * If one was not given for this structure, the pointer to the normal
		 * specified dependent tile type is returned.
		 * @warning Returns the equivalent root tile type if an out-of-bounds
		 *          index is given!
		 * @param   index The 0-based index of the dependent tile.
		 * @return  Pointer to the dependent tile's type when the specified
		 *          dependent tile is deleted.
		 * @sa      \c updateTileTypes().
		 */
		inline std::shared_ptr<const awe::tile_type> getDependentDeletedTileType(
			const std::size_t index) const {
			return index >= getDependentTileCount() ?
				getRootDeletedTileType() : (
					hasDependentDeletedTileType(index) ?
					_dependents[index].deletedTileType :
					getDependentTileType(index)
					);
		}

		/**
		 * Is this structure paintable as an entire structure?
		 * @return \c TRUE if all of a structure's tiles can be painted at once in
		 *         the map maker, \c FALSE if not.
		 */
		inline bool isPaintable() const noexcept {
			return _paintable;
		}

		/**
		 * If this structure is painted, will units on its tiles be deleted or
		 * kept on the map?
		 * @return \c TRUE if all the tiles of the structure should keep any units
		 *         that are on them when the structure is painted, \c FALSE if not
		 *         (which is the default).
		 */
		inline bool keepUnitsWhenPainted() const noexcept {
			return _keepUnits;
		}

		/**
		 * The translation key of the long name of this structure when it is
		 * destroyed.
		 * If it was not assigned, returns the normal long name instead.
		 * @return The long name.
		 */
		inline const std::string& getDestroyedName() const noexcept {
			return _destroyedLongName.empty() ? getName() : _destroyedLongName;
		}

		/**
		 * The sprite key of the icon representing a destroyed version of this
		 * structure.
		 * @return The key of the sprite used to represented this structure when
		 *         it's destroyed.
		 */
		inline const std::string& getDestroyedIconName() const noexcept {
			return _destroyedIconName.empty() ? getIconName() : _destroyedIconName;
		}

		/**
		 * Founds out if a given tile type is within this structure.
		 * @param  tileType Script name of the tile type to search for.
		 * @return \c TRUE if the tile type was found at least once, as the root
		 *         tile, and/or as a dependent tile.
		 */
		bool containsTileType(const std::string& tileType) const;

		/**
		 * Retrieves pointers to all the tile types from a given bank, referenced
		 * in this object.
		 * If any destroyed or deleted tile type script names aren't set (since
		 * they are optional), then the pointers will not be assigned.
		 * @param tileBank A reference to the tile types bank to pull information
		 *                 from.
		 */
		void updateTileTypes(const awe::bank<awe::tile_type>& tileBank) const;

		/**
		 * Updates \c _ownedIconsTurnOrder by copying over the contents of
		 * \c _ownedIcons and supplying the respective turn order IDs as keys.
		 * @param  countries Pointer to the country bank to pull the turn order IDs
		 *                   from.
		 * @safety Basic guarantee.
		 */
		void updateOwnedIconsMap(const awe::bank<awe::country>& countries) const;
	private:
		/**
		 * The sprite names of the icons corresponding to each country.
		 */
		std::unordered_map<std::string, std::string> _ownedIcons;

		/**
		 * \c _ownedIcons keyed by turn order ID.
		 */
		mutable std::unordered_map<awe::ArmyID, std::string> _ownedIconsTurnOrder;

		/**
		 * The type of tile that is used to represent the root of the structure.
		 */
		std::string _rootTile;

		/**
		 * \c _rootTile but it holds a pointer to the \c tile_type object.
		 * It was made mutable so that it can be updated after construction in the
		 * \c bank constructor, via \c updateTileTypes().
		 */
		mutable std::shared_ptr<const awe::tile_type> _rootTileType;

		/**
		 * The type of tile that is used to represent the root of the structure
		 * when the structure is destroyed.
		 */
		std::string _rootDestroyedTile;

		/**
		 * \c _rootDestroyedTile but it holds a pointer to the \c tile_type object.
		 * It was made mutable so that it can be updated after construction in the
		 * \c bank constructor, via \c updateTileTypes().
		 */
		mutable std::shared_ptr<const awe::tile_type> _rootDestroyedTileType;

		/**
		 * The type of tile that replaces the root tile's type when the root tile
		 * is deleted in the map maker.
		 */
		std::string _rootDeletedTile;

		/**
		 * \c _rootDeletedTile but it holds a pointer to the \c tile_type object.
		 * It was made mutable so that it can be updated after construction in the
		 * \c bank constructor, via \c updateTileTypes().
		 */
		mutable std::shared_ptr<const awe::tile_type> _rootDeletedTileType;

		/**
		 * Holds information on a dependent tile.
		 */
		struct dependent_tile {
			/**
			 * Initialises this \c dependent_tile with mandatory information.
			 * @param o The offset.
			 * @param t The script name of the normal tile type.
			 */
			dependent_tile(const sf::Vector2i& o, const std::string& t);

			/**
			 * Does this dependent tile have a destroyed tile type?
			 * @return \c TRUE if yes, \c FALSE otherwise.
			 */
			inline bool hasDestroyedTileType() const noexcept {
				return !destroyedTile.empty();
			}

			/**
			 * Does this dependent tile have a deleted tile type?
			 * @return \c TRUE if yes, \c FALSE otherwise.
			 */
			inline bool hasDeletedTileType() const noexcept {
				return !deletedTile.empty();
			}

			/**
			 * Defines where the dependent tile should be on the map in relation to
			 * the root tile.
			 * Must be unique on a per structure basis!
			 */
			sf::Vector2i offset;

			/**
			 * The type of tile that is used to represent this dependent tile of
			 * the structure.
			 */
			std::string tile;

			/**
			 * \c tile but it holds a pointer to the \c tile_type object.
			 */
			std::shared_ptr<const awe::tile_type> tileType;

			/**
			 * The type of tile that is used to represent this dependent tile of a
			 * destroyed structure.
			 */
			std::string destroyedTile;

			/**
			 * \c destroyedTile but it holds a pointer to the \c tile_type object.
			 */
			std::shared_ptr<const awe::tile_type> destroyedTileType;

			/**
			 * When the structure is deleted in the map maker, replace this
			 * dependent tile's type with this tile type.
			 */
			std::string deletedTile;

			/**
			 * \c deletedTile but it holds a pointer to the \c tile_type object.
			 */
			std::shared_ptr<const awe::tile_type> deletedTileType;
		};

		/**
		 * Stores information on each dependent tile, keyed on its offset.
		 * It was made mutable so that it can be updated after construction in the
		 * \c bank constructor, via \c updateTileTypes().
		 */
		mutable std::vector<dependent_tile> _dependents;

		/**
		 * Can this structure be painted as a whole in the map maker?
		 * If this is set to \c FALSE, the structure can still technically be
		 * painted, but it will have to be painted a tile at a time, and it won't
		 * show up in the Structures tab of the palette.
		 */
		bool _paintable = true;

		/**
		 * If a structure is painted in the map maker, usually all of the units
		 * previously occupying the structure's tiles will be deleted.
		 * If this is set to \c TRUE, the units will be preserved.
		 */
		bool _keepUnits = false;

		/**
		 * The long name of the structure when it is destroyed.
		 */
		std::string _destroyedLongName;

		/**
		 * The sprite key of the GUI icon that is to be displayed when representing
		 * a destroyed version of this structure.
		 */
		std::string _destroyedIconName;
	};

	/**
	 * Calls \c terrain::updatePictureMap() on an entire bank of \c terrain
	 * objects.
	 * @param  terrainBank The \c terrain bank to update.
	 * @param  countryBank The \c country bank to pull turn order IDs from.
	 * @param  tileBank    The \c tile_type bank to pull the pointers from.
	 * @safety Basic guarantee.
	 */
	void updateTerrainBank(awe::bank<awe::terrain>& terrainBank,
		const awe::bank<awe::country>& countryBank,
		const awe::bank<awe::tile_type>& tileBank);

	/**
	 * Calls \c tile_type::updateTerrain() and \c tile_type::updateOwnedTilesMap()
	 * on an entire bank of \c tile_type objects.
	 * @param tileBank      The \c tile_type bank to update.
	 * @param terrainBank   The \c terrian bank to pull the pointers from.
	 * @param countryBank   The \c country bank to pull the turn order IDs from.
	 * @param structureBank The \c structure bank to search through.
	 * @param scripts       The \c scripts engine used to create script arrays.
	 */
	void updateTileTypeBank(awe::bank<awe::tile_type>& tileBank,
		const awe::bank<awe::terrain>& terrainBank,
		const awe::bank<awe::country>& countryBank,
		const awe::bank<awe::structure>& structureBank,
		const std::shared_ptr<engine::scripts>& scripts);

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
	 * Calls \c structure::updateTileTypes() and
	 * \c structure::updateOwnedIconsMap() on an entire bank of \c structure
	 * objects.
	 * @param structureBank The \c structure bank to update.
	 * @param tileBank      The \c tile_type bank to pull the pointers from.
	 * @param countryBank   The \c country bank to pull turn order IDs from.
	 */
	void updateStructureBank(awe::bank<awe::structure>& structureBank,
		const awe::bank<awe::tile_type>& tileBank,
		const awe::bank<awe::country>& countryBank);

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
