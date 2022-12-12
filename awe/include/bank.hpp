/*Copyright 2019-2022 CasualYouTuber31 <naysar@protonmail.com>

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
	 * <ol><li>There should be a constructor which accepts a <tt>const awe::BankID
	 *         </tt> and a <tt>engine::json&</tt>. These parameters are intended to
	 *         store the ID of the bank entry, and allow the constructor to read
	 *         the JSON script object pertaining to the bank entry in order to
	 *         initialise the object, respectively. You can automatically handle ID
	 *         storage by inheriting from \c awe::bank_id.</li>
	 *     <li>All instantiations of your class stored in the bank will be const.
	 *         Therefore use the \c const modifier liberally with your class to
	 *         allow it to be accessed appropriately via the bank. You should not
	 *         design your class to be mutable.</li>
	 *     <li>Your class needs to inherit from @c awe::bank_id. This is so that
	 *         the string version of \c operator[] can be supported.</li>
	 *     <li>In addition, your class needs to define a static method:
	 *         <tt>template<typename T> void Register(const std::string& type,
	 *         asIScriptEngine* engine, const
	 *         std::shared_ptr<DocumentationGenerator>& document) noexcept</tt>.
	 *         This method should register your T reference type with the given
	 *         script engine so that the scripts can call the get methods.</ol>
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
		 * @param logName Name given for to this bank object in the log file.
		 *                Defaults to "bank."
		 */
		bank(const std::shared_ptr<engine::scripts>& scripts,
			const std::string& name, const std::string& logName = "bank") noexcept;

		/**
		 * Allows the client to access the game properties of a bank member.
		 * @param  id The ID of the member to access.
		 * @return A pointer to the properties.
		 */
		std::shared_ptr<const T> operator[](const awe::BankID id) const noexcept;

		/**
		 * Allows the client to access the game properties of a bank member.
		 * @param  sn The script name of the member to access.
		 * @return A pointer to the properties.
		 */
		std::shared_ptr<const T> operator[](const std::string& sn) const noexcept;

		/**
		 * Calculates the size of the bank.
		 * @return The number of members or elements of the internal vector \c
		 *         _bank.
		 */
		std::size_t size() const noexcept;

		/**
		 * Callback given to \c engine::scripts::registerInterface() to register
		 * game engine functions with a \c scripts object.
		 * @sa \c engine::scripts::registerInterface()
		 */
		void registerInterface(asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>& document) noexcept;
	private:
		/**
		 * The JSON load method for this class.
		 * All classes substituted for \c T should have a common JSON script
		 * format. In the root object, key-value pairs list each member/entry of
		 * the bank and their properties.\n
		 * The key is essentially unused: it can be used to store an unofficial
		 * name of the entry, or it could store a version of the entry's ID, but
		 * since this class now uses \c nlohmann::orderer_json, the order of
		 * key-value pairs as written in the JSON script will be retained, and as
		 * such, each key-value pair will be added in the order they are written in
		 * the script. E.g. the first object will have ID \c 0, the second object
		 * will have ID \c 1, and so on.\n
		 * It is then up to the classes used with this template class to parse the
		 * object value of these keys in its constructor.
		 * @param  j The \c engine::json object representing the contents of the
		 *           loaded script which this method reads.
		 * @return Always returns \c TRUE.
		 * @sa     @c awe::bank<T>
		 */
		bool _load(engine::json& j) noexcept;

		/**
		 * The JSON save method for this class.
		 * This class does not have the ability to be saved.
		 * @param  j The \c nlohmann::ordered_json object representing the JSON
		 *           script which this method writes to.
		 * @return Always returns \c FALSE.
		 */
		bool _save(nlohmann::ordered_json& j) noexcept;

		/**
		 * Script's integer index operator.
		 * @throws std::runtime_error if no game property with the given ID exists.
		 * @sa     @c awe::bank<T>::operator[](const awe::BankID)
		 */
		const T _opIndexInt(const awe::BankID id) const;

		/**
		 * Script's string index operator.
		 * @throws std::runtime_error if no game property with the given name
		 *                            exists.
		 * @sa     @c awe::bank<T>::operator[](const std::string&)
		 */
		const T _opIndexStr(const std::string name) const;

		/**
		 * Internal logger object.
		 */
		mutable engine::logger _logger;

		/**
		 * The internal vector of game properties.
		 */
		std::vector<std::shared_ptr<const T>> _bank;

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
		 */
		template<typename T>
		static void Register(const std::string& type,
			asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>& document) noexcept;

		/**
		 * Retrieves the ID of this bank entry as defined during allocation of the
		 * entry.
		 * @return The 0-based ID.
		 */
		awe::BankID getID() const noexcept;

		/**
		 * Retrieves the script name of this bank entry as defined by the JSON
		 * script it is loaded with.
		 * @return The string identifier used in @c engine::script scripts.
		 */
		std::string getScriptName() const noexcept;
	protected:
		/**
		 * Constructor which assigns the ID to the bank entry.
		 * For classes that inherit from this one, this protected constructor is to
		 * be called, either in the subclass' constructor definition's initialiser
		 * list, or in the code block of the subclass constructor.
		 * @param id         The ID of this bank entry.
		 * @param scriptName The identifier of this bank entry that is to be used
		 *                   within game scripts.
		 */
		bank_id(const awe::BankID id, const std::string& scriptName) noexcept;

		/**
		 * Used by subclasses to return an empty string when attempting to retrieve
		 * a string property that doesn't exist.
		 * Should be initialised to an empty string. Note that this is not
		 * thread-safe (https://stackoverflow.com/questions/30239268/how-can-i-return-a-const-reference-to-an-empty-string-without-a-compiler-warning),
		 * but honestly none of my code has been up to this point so I don't care.
		 */
		static const std::string emptyString;
	private:
		/**
		 * The ID of this bank entry.
		 */
		awe::BankID _id = 0;

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
		 * @tparam T        The type that is being registered, that inherits from
		 *                  \c common_properties in some way.
		 * @param  type     Name of the type to add the properties to.
		 * @param  engine   Pointer to the AngelScript script engine to register
		 *                  with.
		 * @param  document Pointer to the AngelScript documentation generator to
		 *                  register script interface documentation with.
		 */
		template<typename T>
		static void Register(const std::string & type,
			asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>&document,
			const std::string& extraIconDoc = "") noexcept;

		/**
		 * Retrieves the long name property.
		 * @return The long name property, which should be a language dictionary
		 *         key.
		 */
		const std::string& getName() const noexcept;

		/**
		 * Retrieves the short name property.
		 * @return The short name property, which should be a language dictionary
		 *         key.
		 */
		const std::string& getShortName() const noexcept;

		/**
		 * Retrieves the sprite name of the icon associated with this property.
		 * @return The sprite name.
		 */
		const std::string& getIconName() const noexcept;

		/**
		 * Retrieves the description property.
		 * @return The description property, which should be a language dictionary
		 *         key.
		 */
		const std::string& getDescription() const noexcept;
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
		 * @param id         The ID of this bank entry.
		 * @param scriptName The identifier of this bank entry that is to be used
		 *                   within game scripts.
		 * @param j          The object value containing the name, icon, and
		 *                   description properties, as well as any other
		 *                   properties subclasses need to store (which this class
		 *                   ignores).
		 */
		common_properties(const awe::BankID id, const std::string& scriptName,
			engine::json& j) noexcept;
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
		 *     </li></ul>
		 * @param id         The ID of the bank entry.
		 * @param scriptName The identifier of this bank entry that is to be used
		 *                   within game scripts.
		 * @param j          The object value containing the country's properties.
		 */
		country(const awe::BankID id, const std::string& scriptName,
			engine::json& j) noexcept;

		/**
		 * Registers \c country with a given type.
		 * @tparam T        The type that is being registered, that inherits from
		 *                  \c country in some way.
		 * @param  type     Name of the type to add the properties to.
		 * @param  engine   Pointer to the AngelScript script engine to register
		 *                  with.
		 * @param  document Pointer to the AngelScript documentation generator to
		 *                  register script interface documentation with.
		 */
		template<typename T>
		static void Register(const std::string& type,
			asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>& document) noexcept;

		/**
		 * Retrieves the colour property.
		 * @return The colour of the country.
		 */
		const sf::Color& getColour() const noexcept;

		/**
		 * Comparison operator.
		 * @param  rhs The \c country object to test against.
		 * @return \c TRUE if both object's UUIDs are the same, \c FALSE if not.
		 */
		bool operator==(const awe::country& rhs) const noexcept;

		/**
		 * Inverse comparison operator.
		 * @param  rhs The \c country object to test against.
		 * @return \c TRUE if both object's UUIDs are not the same, \c FALSE if
		 *         they are.
		 */
		bool operator!=(const awe::country& rhs) const noexcept;
	private:
		/**
		 * The colour property.
		 */
		sf::Color _colour;
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
		 * @param id         The ID of the bank entry.
		 * @param scriptName The identifier of this bank entry that is to be used
		 *                   within game scripts.
		 * @param j          The object value containing the weather's properties.
		 */
		weather(const awe::BankID id, const std::string& scriptName,
			engine::json& j) noexcept;

		/**
		 * Registers \c weather with a given type.
		 * @tparam T        The type that is being registered, that inherits from
		 *                  \c weather in some way.
		 * @param  type     Name of the type to add the properties to.
		 * @param  engine   Pointer to the AngelScript script engine to register
		 *                  with.
		 * @param  document Pointer to the AngelScript documentation generator to
		 *                  register script interface documentation with.
		 */
		template<typename T>
		static void Register(const std::string& type,
			asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>& document) noexcept;

		/**
		 * Comparison operator.
		 * @param  rhs The \c weather object to test against.
		 * @return \c TRUE if both object's UUIDs are the same, \c FALSE if not.
		 */
		bool operator==(const awe::weather& rhs) const noexcept;

		/**
		 * Inverse comparison operator.
		 * @param  rhs The \c weather object to test against.
		 * @return \c TRUE if both object's UUIDs are not the same, \c FALSE if
		 *         they are.
		 */
		bool operator!=(const awe::weather& rhs) const noexcept;
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
		 * @param id         The ID of the bank entry.
		 * @param scriptName The identifier of this bank entry that is to be used
		 *                   within game scripts.
		 * @param j          The object value containing the environment's
		 *                   properties.
		 */
		environment(const awe::BankID id, const std::string& scriptName,
			engine::json& j) noexcept;

		/**
		 * Registers \c environment with a given type.
		 * @tparam T        The type that is being registered, that inherits from
		 *                  \c environment in some way.
		 * @param  type     Name of the type to add the properties to.
		 * @param  engine   Pointer to the AngelScript script engine to register
		 *                  with.
		 * @param  document Pointer to the AngelScript documentation generator to
		 *                  register script interface documentation with.
		 */
		template<typename T>
		static void Register(const std::string& type,
			asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>& document) noexcept;

		/**
		 * Comparison operator.
		 * @param  rhs The \c environment object to test against.
		 * @return \c TRUE if both object's UUIDs are the same, \c FALSE if not.
		 */
		bool operator==(const awe::environment& rhs) const noexcept;

		/**
		 * Inverse comparison operator.
		 * @param  rhs The \c environment object to test against.
		 * @return \c TRUE if both object's UUIDs are not the same, \c FALSE if
		 *         they are.
		 */
		bool operator!=(const awe::environment& rhs) const noexcept;
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
		 * @param id         The ID of the bank entry.
		 * @param scriptName The identifier of this bank entry that is to be used
		 *                   within game scripts.
		 * @param j          The object value containing the movement type's
		 *                   properties.
		 */
		movement_type(const awe::BankID id, const std::string& scriptName,
			engine::json& j) noexcept;

		/**
		 * Registers \c movement_type with a given type.
		 * @tparam T        The type that is being registered, that inherits from
		 *                  \c movement_type in some way.
		 * @param  type     Name of the type to add the properties to.
		 * @param  engine   Pointer to the AngelScript script engine to register
		 *                  with.
		 * @param  document Pointer to the AngelScript documentation generator to
		 *                  register script interface documentation with.
		 */
		template<typename T>
		static void Register(const std::string& type,
			asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>& document) noexcept;

		/**
		 * Comparison operator.
		 * @param  rhs The \c movement_type object to test against.
		 * @return \c TRUE if both object's UUIDs are the same, \c FALSE if not.
		 */
		bool operator==(const awe::movement_type& rhs) const noexcept;

		/**
		 * Inverse comparison operator.
		 * @param  rhs The \c environment object to test against.
		 * @return \c TRUE if both object's UUIDs are not the same, \c FALSE if
		 *         they are.
		 */
		bool operator!=(const awe::movement_type& rhs) const noexcept;
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
		 *     <li>\c "movecosts" = \c _movecosts, <tt>([signed 32-bit int{,
		 *         signed 32-bit int, etc.}])</tt></li>
		 *     <li>\c "pictures" = \c _pictures, <tt>([string{, string, etc.}])
		 *         </tt></li></ul>
		 * 
		 * The \c movecosts array stores a list of movement points associated with
		 * each movement type. For example, the first value will store the number
		 * of movement points it takes for the first type of movement to traverse
		 * over it (in the default implementation, Infantry). A negative value
		 * indicates that a unit of the specified movement type cannot traverse the
		 * terrain.\n
		 * 
		 * The \c pictures array stores a list of animated sprite names associated
		 * with each country. For example, the first value will store the name of
		 * the sprite shown when the first country owns it (in the default
		 * implementation, Neutral). Not all countries have to be accounted for if
		 * the tile cannot be "owned," i.e. captured.
		 * @param id         The ID of the bank entry.
		 * @param scriptName The identifier of this bank entry that is to be used
		 *                   within game scripts.
		 * @param j          The object value containing the terrain type's
		 *                   properties.
		 */
		terrain(const awe::BankID id, const std::string& scriptName,
			engine::json& j) noexcept;

		/**
		 * Registers \c terrain with a given type.
		 * @tparam T        The type that is being registered, that inherits from
		 *                  \c terrain in some way.
		 * @param  type     Name of the type to add the properties to.
		 * @param  engine   Pointer to the AngelScript script engine to register
		 *                  with.
		 * @param  document Pointer to the AngelScript documentation generator to
		 *                  register script interface documentation with.
		 */
		template<typename T>
		static void Register(const std::string& type,
			asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>& document) noexcept;

		/**
		 * Retrieves the maximum health property.
		 * This can be the health points of a cannon, or the capture points of a
		 * property.
		 * @return The health points this terrain can have.
		 */
		unsigned int getMaxHP() const noexcept;

		/**
		 * Retrieves the defence property.
		 * @return The defence this terrain provides.
		 */
		unsigned int getDefence() const noexcept;

		/**
		 * Retrieves the movement point cost property associated with a given
		 * movement type.
		 * @param  movecostID The ID of the type of movement.
		 * @return The movement point cost.
		 */
		int getMoveCost(const awe::BankID movecostID) const
			noexcept;

		/**
		 * Retrieves the sprite name associated with a given country.
		 * @param  countryID The ID of the country.
		 * @return The sprite name of the terrain picture, or a blank string if the
		 *         given country ID didn't identify a sprite name.
		 */
		const std::string& getPicture(const awe::BankID countryID) const noexcept;

		/**
		 * Copies the internal list of movement costs and returns it.
		 * @return All the move costs assigned to this terrain.
		 */
		std::vector<int> copyMoveCosts() const noexcept;

		/**
		 * Copies the internal list of picture sprite names and returns it.
		 * @return All the pictures assigned to this terrain.
		 */
		std::vector<std::string> copyPictures() const noexcept;

		/**
		 * Comparison operator.
		 * @param  rhs The \c terrain object to test against.
		 * @return \c TRUE if both object's UUIDs are the same, \c FALSE if not.
		 */
		bool operator==(const awe::terrain& rhs) const noexcept;

		/**
		 * Inverse comparison operator.
		 * @param  rhs The \c terrain object to test against.
		 * @return \c TRUE if both object's UUIDs are not the same, \c FALSE if
		 *         they are.
		 */
		bool operator!=(const awe::terrain& rhs) const noexcept;
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
		std::vector<int> _movecosts;

		/**
		 * Picture properties.
		 */
		std::vector<std::string> _pictures;
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
		 * <ul><li>\c "type" = \c _terrainType, <tt>(unsigned 32-bit int)</tt></li>
		 *     <li>\c "neutral" = \c _neutralTile, <tt>(string)</tt></li>
		 *     <li>\c "tiles" = \c _tiles, <tt>([string{, string, etc.}])
		 *         </tt></li></ul>
		 * The \c neutral key stores a sprite name shown when the tile is not owned
		 * by any country. This should be given for all tile types.\n
		 * The \c tiles vector stores a list of animated sprite names associated
		 * with each country's version of the tile. For example, the first value
		 * will store the name of the sprite shown on the map when the first
		 * country owns it (in the default implementation: Orange Star). This
		 * vector does not have to be accounted for if the tile cannot be
		 * owned/captured. In which case, an empty vector should be given in the
		 * script.
		 * @param id         The ID of the bank entry.
		 * @param scriptName The identifier of this bank entry that is to be used
		 *                   within game scripts.
		 * @param j          The object value containing the tile type's
		 *                   properties.
		 */
		tile_type(const awe::BankID id, const std::string& scriptName,
			engine::json& j) noexcept;

		/**
		 * Registers \c tile_type with a given type.
		 * @tparam T        The type that is being registered, that inherits from
		 *                  \c tile_type in some way.
		 * @param  type     Name of the type to add the properties to.
		 * @param  engine   Pointer to the AngelScript script engine to register
		 *                  with.
		 * @param  document Pointer to the AngelScript documentation generator to
		 *                  register script interface documentation with.
		 */
		template<typename T>
		static void Register(const std::string& type,
			asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>& document) noexcept;

		/**
		 * Retrieves the ID of the type of terrain this tile represents (i.e.
		 * "Plains" or "Road").
		 * @return The ID of the type of terrain.
		 */
		awe::BankID getTypeIndex() const noexcept;

		/**
		 * Retrieves the name of the sprite that is shown for a given country.
		 * @param  countryID The ID of the country.
		 * @return The name of the tile's sprite, or \c _neutralTile if the given
		 *         country ID didn't identify a sprite name.
		 */
		const std::string& getOwnedTile(const awe::BankID countryID) const
			noexcept;

		/**
		 * Retrieves the name of the sprite that is shown when no country owns the
		 * tile.
		 * @return The name of the tile's neutral sprite.
		 */
		const std::string& getNeutralTile() const noexcept;

		/**
		 * Retrieves a pointer to the details of the type of terrain this tile
		 * represents.
		 * @return The pointer to the terrain type's properties.
		 * @sa     @c updateTerrain()
		 */
		std::shared_ptr<const awe::terrain> getType() const noexcept;

		/**
		 * Copies the details of the type of terrain this tile represents.
		 * Primarily used for the script interface; for C++ code, use \c getType()
		 * instead.
		 * @return The terrain type's properties.
		 * @sa     @c getType()
		 */
		const awe::terrain getTypeObj() const noexcept;

		/**
		 * Updates the stored terrain type properties pointer.
		 * @param terrainBank A reference to the terrain bank to pull the pointer
		 *        from.
		 */
		void updateTerrain(const awe::bank<awe::terrain>& terrainBank) const
			noexcept;

		/**
		 * Comparison operator.
		 * @param  rhs The \c tile_type object to test against.
		 * @return \c TRUE if both object's UUIDs are the same, \c FALSE if not.
		 */
		bool operator==(const awe::tile_type& rhs) const noexcept;

		/**
		 * Inverse comparison operator.
		 * @param  rhs The \c tile_type object to test against.
		 * @return \c TRUE if both object's UUIDs are not the same, \c FALSE if
		 *         they are.
		 */
		bool operator!=(const awe::tile_type& rhs) const noexcept;
	private:
		/**
		 * The ID of the type of terrain this tile represents.
		 */
		awe::BankID _terrainType = 0;

		/**
		 * Pointer to the properties of this tile's type of terrain.
		 * It was made mutable so that it can be updated after construction in the
		 * \c bank constructor, via \c updateTerrain().
		 */
		mutable std::shared_ptr<const awe::terrain> _terrain;

		/**
		 * The sprite names of the tile corresponding to each country.
		 */
		std::vector<std::string> _tiles;

		/**
		 * The sprite name of the tile with no owner.
		 */
		std::string _neutralTile = "";
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
		 * <ul><li>\c "movetype" = \c _movementTypeID, <tt>(unsigned 32-bit int)
		 *     </tt></li>
		 *     <li>\c "price" = \c _cost, <tt>(unsigned 32-bit int)</tt></li>
		 *     <li>\c "fuel" = \c _maxFuel, <tt>(signed 32-bit int)</tt></li>
		 *     <li>\c "ammo" = \c _maxAmmo, <tt>(signed 32-bit int)</tt></li>
		 *     <li>\c "hp" = \c _maxHP, <tt>(unsigned 32-bit int - capped off at
		 *         signed 32-bit int's maximum value divided by the HP granularity
		 *         value)</tt></li>
		 *     <li>\c "mp" = \c _movementPoints, <tt>(unsigned 32-bit int)</tt>
		 *         </li>
		 *     <li>\c "vision" = \c _vision, <tt>(unsigned 32-bit int)</tt></li>
		 *     <li>\c "lowrange" = \c _lowerRange, <tt>(unsigned 32-bit int)</tt>
		 *         </li>
		 *     <li>\c "highrange" = \c _higherRange, <tt>(unsigned 32-bit int)</tt>
		 *         </li>
		 *     <li>\c "pictures" = \c _pictures, <tt>([string{, string, etc.}])
		 *         </tt></li>
		 *     <li>\c "sprites" = \c _units, <tt>([string{, string, etc.}])
		 *         </tt></li>
		 *     <li>\c "canload" = \c _canLoadThese, <tt>([unsigned 32-bit int{,
		 *         unsigned 32-bit int, etc.}])</tt></li>
		 *     <li>\c "loadlimit" = \c _loadLimit, <tt>(unsigned 32-bit int)</tt>
		 *         </li>
		 *     <li>\c "turnstartpriority" = \c _turnStartPriority, <tt>(unsigned
		 *         32-bit int)</tt></li>
		 *     <li>\c "cancapture" = \c _canCaptureThese,
		 *         <tt>([unsigned 32-bit int{, unsigned 32-bit int, etc.}])</tt>
		 *         </li>
		 *     <li>\c "canhide" = \c _canHide, <tt>(bool)</tt></li>
		 *     <li>\c "canonlyunloadfrom" = \c _canUnloadFromThese,
		 *         <tt>([unsigned 32-bit int{, unsigned 32-bit int, etc.}])</tt>
		 *         </li></ul>
		 * 
		 * Range values work by counting the number of tiles away from the unit's
		 * current tile. If the tile is within both the lower and higher ranges
		 * inclusive, then the attack is valid. If not, the attack is invalid.\n
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
		 * Cancapture and canonlyunloadfrom are arrays of terrain type bank IDs. If
		 * no terrain type IDs are given for canonlyunloadfrom, then it is assumed
		 * that the unit can unload from any tile. If cancapture is empty, however,
		 * the unit won't be able to capture anything.
		 * @param id         The ID of the bank entry.
		 * @param scriptName The identifier of this bank entry that is to be used
		 *                   within game scripts.
		 * @param j          The object value containing the unit type's
		 *                   properties.
		 * @sa    isInfiniteFuel()
		 * @sa    isInfiniteAmmo()
		 */
		unit_type(const awe::BankID id, const std::string& scriptName,
			engine::json& j) noexcept;

		/**
		 * Registers \c unit_type with a given type.
		 * @tparam T        The type that is being registered, that inherits from
		 *                  \c unit_type in some way.
		 * @param  type     Name of the type to add the properties to.
		 * @param  engine   Pointer to the AngelScript script engine to register
		 *                  with.
		 * @param  document Pointer to the AngelScript documentation generator to
		 *                  register script interface documentation with.
		 */
		template<typename T>
		static void Register(const std::string& type,
			asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>& document) noexcept;

		/**
		 * Retrieves the movement type of this unit.
		 * @return The index of the movement type of this unit.
		 */
		awe::BankID getMovementTypeIndex() const noexcept;

		/**
		 * Retrieves a pointer to the details of the type of movement this unit
		 * has.
		 * @return The pointer to the movement type's properties.
		 * @sa     @c updateMovementType()
		 */
		std::shared_ptr<const awe::movement_type> getMovementType() const noexcept;

		/**
		 * Copies the details of the type of movement this unit has.
		 * Primarily used for the script interface; for C++ code, use
		 * \c getMovementType() instead.
		 * @return The movement type's properties.
		 * @sa     @c getMovementType()
		 */
		const awe::movement_type getMovementTypeObj() const noexcept;

		/**
		 * Updates the stored movement type properties pointer.
		 * @param movementBank A reference to the movement type bank to pull the
		 * pointer from.
		 */
		void updateMovementType(const awe::bank<awe::movement_type>& movementBank)
			const noexcept;

		/**
		 * Retrieves the sprite name of a given country's portrait of this unit.
		 * @param  countryID The ID of the country.
		 * @return The sprite name, or a blank string if the given country ID
		 *         didn't map to a sprite name in the internal list.
		 */
		const std::string& getPicture(const awe::BankID countryID) const noexcept;

		/**
		 * Retrieves the sprite name of a given country's map sprite of this unit.
		 * @param  countryID The ID of the country.
		 * @return The sprite name, or a blank string if the given country ID
		 *         didn't map to a sprite name in the internal list.
		 */
		const std::string& getUnit(const awe::BankID countryID) const noexcept;

		/**
		 * Retrieves the price property.
		 * @return The price property.
		 */
		unsigned int getCost() const noexcept;

		/**
		 * Retrieves the max fuel property.
		 * @return The max fuel property.
		 */
		int getMaxFuel() const noexcept;

		/**
		 * Retrieves the max primary ammo property.
		 * @return The max ammo property.
		 */
		int getMaxAmmo() const noexcept;

		/**
		 * Retrieves the max HP property.
		 * Note that the value returned will be an \em internal HP value, and not
		 * one that should be displayed to the user.
		 * @return The max HP property.
		 */
		unsigned int getMaxHP() const noexcept;

		/**
		 * Retrieves the MP property.
		 * @return The MP property.
		 */
		unsigned int getMovementPoints() const noexcept;

		/**
		 * Retrieves the vision property.
		 * @return The vision property.
		 */
		unsigned int getVision() const noexcept;

		/**
		 * Retrieves the lower range property.
		 * @return The lower bound of the range property.
		 */
		unsigned int getLowerRange() const noexcept;

		/**
		 * Retrieves the higher range property.
		 * @return The higher bound of the range property.
		 */
		unsigned int getHigherRange() const noexcept;

		/**
		 * Finds out if this type of unit has infinite fuel.
		 * @return \c TRUE if \c _maxFuel is less than \c 0, \c FALSE otherwise.
		 */
		bool hasInfiniteFuel() const noexcept;

		/**
		 * Finds out if this type of unit has infinite ammo.
		 * @return \c TRUE if \c _maxAmmo is less than \c 0, \c FALSE otherwise.
		 */
		bool hasInfiniteAmmo() const noexcept;

		/**
		 * Finds out if this type of unit can load onto it a given unit type.
		 * @param  typeID The ID of the type of unit to test.
		 * @return \c TRUE if yes, \c FALSE otherwise.
		 */
		bool canLoad(const awe::BankID typeID) const noexcept;

		/**
		 * Overloaded version of \c canLoad() that checks using a given unit type.
		 * @param  type The unit type to check for. Returns \c FALSE if \c nullptr.
		 * @return \c TRUE if the given unit type can be loaded onto units of this
		 *         type, \c FALSE if not.
		 */
		bool canLoad(const std::shared_ptr<const awe::unit_type>& type) const
			noexcept;

		/**
		 * Retrieves the load limit of this unit type.
		 * @return The maximum number of units this unit can hold at any given
		 *         time.
		 */
		unsigned int loadLimit() const noexcept;

		/**
		 * Retrieves the turn start priority level for this unit type.
		 * @return The priority level.
		 */
		unsigned int getTurnStartPriority() const noexcept;

		/**
		 * Updates the stored unit type properties pointers for units that can be
		 * loaded onto this one.
		 * @param unitBank A reference to the unit type bank to pull the pointers
		 *                 from.
		 */
		void updateUnitTypes(const bank<unit_type>& unitBank) const noexcept;

		/**
		 * Finds out if this type of unit can capture a given terrain type.
		 * @param  typeID The ID of the type of terrain to test.
		 * @return \c TRUE if yes, \c FALSE otherwise.
		 */
		bool canCapture(const awe::BankID typeID) const noexcept;

		/**
		 * Overloaded version of \c canCapture() that checks using a given terrain
		 * type.
		 * @param  type The terrain type to check for. Returns \c FALSE if
		 *              \c nullptr.
		 * @return \c TRUE if the given terrain type can be captured by units of
		 *         this type, \c FALSE if not.
		 */
		bool canCapture(const std::shared_ptr<const awe::terrain>& type) const
			noexcept;

		/**
		 * Finds out if this type of unit can unload from a given terrain type.
		 * @param  typeID The ID of the type of terrain to test.
		 * @return \c TRUE if yes, \c FALSE otherwise.
		 */
		bool canUnloadFrom(const awe::BankID typeID) const noexcept;

		/**
		 * Overloaded version of \c canUnloadFrom() that checks using a given
		 * terrain type.
		 * @param  type The terrain type to check for. Returns \c FALSE if
		 *              \c nullptr.
		 * @return \c TRUE if the given terrain type allows units of this type to
		 *         unload units from it, \c FALSE if not.
		 */
		bool canUnloadFrom(const std::shared_ptr<const awe::terrain>& type) const
			noexcept;

		/**
		 * Finds out if this type of unit can hide.
		 * @return \c TRUE if yes, \c FALSE otherwise.
		 */
		bool canHide() const noexcept;

		/**
		 * Updates the stored terrain type properties pointers stored in this unit
		 * type.
		 * @param terrainBank A reference to the terrain type bank to pull the
		 *                    pointers from.
		 */
		void updateTerrainTypes(const bank<terrain>& terrainBank) const noexcept;

		/**
		 * Copies the internal list of picture sprite names and returns it.
		 * @return All the pictures assigned to this unit.
		 */
		std::vector<std::string> copyPictures() const noexcept;

		/**
		 * Copies the internal list of unit sprite names and returns it.
		 * @return All the map sprites assigned to this unit.
		 */
		std::vector<std::string> copyUnits() const noexcept;

		/**
		 * Copies the internal list of IDs of unit types this unit can hold and
		 * returns it.
		 * @return All the IDs of the types of units that can be loaded onto this
		 *         unit.
		 */
		std::vector<awe::BankID> copyLoadableUnitIDs() const noexcept;

		/**
		 * Copies the internal list of units that can be loaded onto this one and
		 * returns it.
		 * @return All the types of units that can be loaded onto this unit.
		 */
		std::vector<std::shared_ptr<const awe::unit_type>> copyLoadableUnits()
			const noexcept;

		/**
		 * Copies the internal list of IDs of terrain types this unit can capture
		 * and returns it.
		 * @return All the IDs of the types of terrain that can be captured by this
		 *         unit.
		 */
		std::vector<awe::BankID> copyCapturableTerrainIDs() const noexcept;

		/**
		 * Copies the internal list of terrains that can be captured by this unit
		 * type and returns it.
		 * @return All the types of terrain that can be captured by this unit.
		 */
		std::vector<std::shared_ptr<const awe::terrain>> copyCapturableTerrains()
			const noexcept;

		/**
		 * Copies the internal list of IDs of terrain types this unit can unload
		 * from and returns it.
		 * @return All the IDs of the types of terrain.
		 */
		std::vector<awe::BankID> copyUnloadableTerrainIDs() const noexcept;

		/**
		 * Copies the internal list of terrains that allow this unit type to unload
		 * units from it, and returns it.
		 * @return All the types of terrain.
		 */
		std::vector<std::shared_ptr<const awe::terrain>> copyUnloadableTerrains()
			const noexcept;

		/**
		 * Comparison operator.
		 * @param  rhs The \c unit_type object to test against.
		 * @return \c TRUE if both object's UUIDs are the same, \c FALSE if not.
		 */
		bool operator==(const awe::unit_type& rhs) const noexcept;

		/**
		 * Inverse comparison operator.
		 * @param  rhs The \c unit_type object to test against.
		 * @return \c TRUE if both object's UUIDs are not the same, \c FALSE if
		 *         they are.
		 */
		bool operator!=(const awe::unit_type& rhs) const noexcept;
	private:
		/**
		 * The movement type ID property.
		 */
		awe::BankID _movementTypeID = 0;

		/**
		 * Pointer to this unit's movement typre details.
		 * It was made mutable so that it can be updated after construction in the
		 * \c bank constructor, via \c updateMovementType().
		 */
		mutable std::shared_ptr<const awe::movement_type> _movementType;

		/**
		 * The portrait IDs.
		 */
		std::vector<std::string> _pictures;

		/**
		 * The sprite IDs.
		 */
		std::vector<std::string> _units;

		/**
		 * The price property.
		 */
		unsigned int _cost = 0;

		/**
		 * The max fuel property.
		 */
		int _maxFuel = 0;

		/**
		 * The max ammo property.
		 */
		int _maxAmmo = 0;

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
		 * The lower bound of the unit's range.
		 */
		unsigned int _lowerRange = 1;

		/**
		 * The higher bound of the unit's range.
		 */
		unsigned int _higherRange = 1;

		/**
		 * List of unit type IDs that can be loaded onto this type of unit.
		 */
		std::vector<awe::BankID> _canLoadThese;

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
		 * List of terrain type IDs that this unit type can capture.
		 */
		std::vector<awe::BankID> _canCaptureThese;

		/**
		 * List of terrain types that this unit type can capture.
		 * It was made mutable so that it can be updated after construction in the
		 * \c bank constructor, via \c updateTerrainTypes().
		 */
		mutable std::vector<std::shared_ptr<const awe::terrain>>
			_canCaptureTheseTerrainTypes;

		/**
		 * List of terrain type IDs that this unit type can unload from.
		 */
		std::vector<awe::BankID> _canUnloadFromThese;

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
		 * @param id         The ID of the bank entry.
		 * @param scriptName The identifier of this bank entry that is to be used
		 *                   within game scripts.
		 * @param j          The object value containing the commander's
		 *                   properties.
		 */
		commander(const awe::BankID id, const std::string& scriptName,
			engine::json& j) noexcept;

		/**
		 * Registers \c commander with a given type.
		 * @tparam T        The type that is being registered, that inherits from
		 *                  \c commander in some way.
		 * @param  type     Name of the type to add the properties to.
		 * @param  engine   Pointer to the AngelScript script engine to register
		 *                  with.
		 * @param  document Pointer to the AngelScript documentation generator to
		 *                  register script interface documentation with.
		 */
		template<typename T>
		static void Register(const std::string& type,
			asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>& document) noexcept;

		/**
		 * Retrieves the animated sprite name of this commander's portrait.
		 * @return The animated sprite ID.
		 */
		const std::string& getPortrait() const noexcept;

		/**
		 * Comparison operator.
		 * @param  rhs The \c commander object to test against.
		 * @return \c TRUE if both object's UUIDs are the same, \c FALSE if not.
		 */
		bool operator==(const awe::commander& rhs) const noexcept;

		/**
		 * Inverse comparison operator.
		 * @param  rhs The \c commander object to test against.
		 * @return \c TRUE if both object's UUIDs are not the same, \c FALSE if
		 *         they are.
		 */
		bool operator!=(const awe::commander& rhs) const noexcept;
	private:
		/**
		 * The portrait property.
		 */
		std::string _portrait = "";
	};

	/**
	 * Calls \c tile_type::updateTerrain() on an entire bank of \c tile_type
	 * objects.
	 * @param tileBank    The \c tile_type bank to update.
	 * @param terrainBank The \c terrian bank to pull the pointers from.
	 */
	void updateTileTypeBank(bank<tile_type>& tileBank,
		const awe::bank<awe::terrain>& terrainBank) noexcept;

	/**
	 * Calls \c unit_type::updateMovementType(), \c unit_type::updateUnitTypes(),
	 * and \c unit_type::updateTerrainTypes() on an entire bank of \c unit_type
	 * objects.
	 * @param unitBank     The \c unit_type bank to update. Also the \c unit_type
	 *                     bank that is used to update itself.
	 * @param movementBank The \c movement_type bank to pull the pointers from.
	 * @param terrainBank  The \c terrain bank to pull the pointers from.
	 */
	void updateUnitTypeBank(bank<unit_type>& unitBank,
		const awe::bank<awe::movement_type>& movementBank,
		const awe::bank<awe::terrain>& terrainBank) noexcept;
}

#include "tpp/bank.tpp"