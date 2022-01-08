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

/**@file bank.h
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

#include "texture.h"
#include "typedef.h"

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
	 *         design your class to be mutable.</li></ol>
	 * @tparam T The type of static game property to store within this bank.
	 */
	template<typename T>
	class bank : public engine::json_script {
	public:
		/**
		 * Allows the client to access the game properties of a bank member.
		 * @param  id The ID of the member to access.
		 * @return A pointer to the properties.
		 */
		std::shared_ptr<const T> operator[](const awe::BankID id) const noexcept;

		/**
		 * Calculates the size of the bank.
		 * @return The number of members or elements of the internal vector \c
		 *         _bank.
		 */
		std::size_t size() const noexcept;
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
		 * @sa     awe::bank<T>
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
		 * The internal vector of game properties.
		 */
		std::vector<std::shared_ptr<const T>> _bank;
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
		virtual ~bank_id() noexcept;

		/**
		 * Retrieves the ID of this bank entry as defined during allocation of the
		 * entry.
		 * @return The 0-based ID.
		 */
		awe::BankID getID() const noexcept;
	protected:
		/**
		 * Constructor which assigns the ID to the bank entry.
		 * For classes that inherit from this one, this protected constructor is to
		 * be called, either in the subclass' constructor definition's initialiser
		 * list, or in the code block of the subclass constructor.
		 * @param id The ID of this bank entry.
		 */
		bank_id(const awe::BankID id) noexcept;

		/**
		 * Used by subclasses to return an empty string when attempting to retrieve
		 * a string property that doesn't exist.
		 * Should be initialised to an empty string. Note that this is not
		 * thread-safe (https://stackoverflow.com/questions/30239268/how-can-i-return-a-const-reference-to-an-empty-string-without-a-compiler-warning),
		 * but honestly none of my code is up to this point so I don't care.
		 */
		static const std::string emptyString;
	private:
		/**
		 * The ID of this bank entry.
		 */
		awe::BankID _id = 0;
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
		virtual ~common_properties() noexcept;

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
		 * @param id The ID of this bank entry.
		 * @param j  The object value containing the name, icon, and description
		 *           properties, as well as any other properties subclasses need to
		 *           store (which this class ignores).
		 */
		common_properties(const awe::BankID id, engine::json& j) noexcept;
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
		 * @param id The ID of the bank entry.
		 * @param j  The object value containing the country's properties.
		 */
		country(const awe::BankID id, engine::json& j) noexcept;

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
		 * @param id The ID of the bank entry.
		 * @param j  The object value containing the weather's properties.
		 */
		weather(const awe::BankID id, engine::json& j) noexcept;

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
		 * @param id The ID of the bank entry.
		 * @param j  The object value containing the environment's properties.
		 */
		environment(const awe::BankID id, engine::json& j) noexcept;

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
		 * @param id The ID of the bank entry.
		 * @param j  The object value containing the movement type's properties.
		 */
		movement_type(const awe::BankID id, engine::json& j) noexcept;

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
		 *     <li>\c "capturable" = \c _isCapturable, <tt>(bool)</tt></li>
		 *     <li>\c "movecosts" = \c _movecosts, <tt>([signed 32-bit int{,
		 *         signed 32-bit int, etc.}])</tt></li>
		 *     <li>\c "pictures" = \c _pictures, <tt>([string{, string, etc.}])
		 *         </tt></li></ul>
		 * 
		 * The \c movecosts array stores a list of movement points associated with
		 * each movement type. For example, the first value will store the number
		 * of movement points it takes for the first type of movement to traverse
		 * over it (in the default implementation, Infantry).\n
		 * 
		 * The \c pictures array stores a list of animated sprite names associated
		 * with each country. For example, the first value will store the name of
		 * the sprite shown when the first country owns it (in the default
		 * implementation, Neutral). Not all countries have to be accounted for if
		 * the tile cannot be "owned," i.e. captured.
		 * @param id The ID of the bank entry.
		 * @param j  The object value containing the terrain type's properties.
		 */
		terrain(const awe::BankID id, engine::json& j) noexcept;

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
		 * Determines if this property is capturable.
		 * @return \c TRUE if capturable, \c FALSE if not.
		 */
		bool isCapturable() const noexcept;

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

		/**
		 * Capturable property.
		 */
		bool _isCapturable = false;
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
		 * @param id The ID of the bank entry.
		 * @param j  The object value containing the tile type's properties.
		 */
		tile_type(const awe::BankID id, engine::json& j) noexcept;

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
		 * @sa     updateTerrain()
		 */
		std::shared_ptr<const awe::terrain> getType() const noexcept;

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
		 *         signed 32-bit int's maximum value)</tt></li>
		 *     <li>\c "mp" = \c _movementPoints, <tt>(unsigned 32-bit int)</tt>
		 *     </li>
		 *     <li>\c "vision" = \c _vision, <tt>(unsigned 32-bit int)</tt></li>
		 *     <li>\c "lowrange" = \c _lowerRange, <tt>(unsigned 32-bit int)</tt>
		 *     </li>
		 *     <li>\c "highrange" = \c _higherRange, <tt>(unsigned 32-bit int)</tt>
		 *     </li>
		 *     <li>\c "pictures" = \c _pictures, <tt>([string{, string, etc.}])
		 *         </tt></li>
		 *     <li>\c "sprites" = \c _units, <tt>([string{, string, etc.}])
		 *         </tt></li>
		 *     <li>\c "canload" = \c _canLoadThese, <tt>([unsigned 32-bit int{,
		 *         unsigned 32-bit int, etc.}])</tt></li>
		 *     <li>\c "loadlimit" = \c _loadLimit, <tt>(unsigned 32-bit int)</tt>
		 *     </li>
		 *     <li>\c "fuelperturn" = \c _fuelPerTurn, <tt>(signed 32-bit int)</tt>
		 *     </li></ul>
		 * 
		 * Range values work by counting the number of tiles away from the unit's
		 * current tile. If the tile is within both the lower and higher ranges
		 * inclusive, then the attack is valid. If not, the attack is invalid.\n
		 * 
		 * Pictures is an array of sprite names corresponding to each country's
		 * portrait of the type of unit.\n
		 * 
		 * Sprites is an array of sprite names corresponding to each country's map
		 * representation of the type of unit.
		 * @param id The ID of the bank entry.
		 * @param j  The object value containing the terrain type's properties.
		 * @sa    isInfiniteFuel()
		 * @sa    isInfiniteAmmo()
		 */
		unit_type(const awe::BankID id, engine::json& j) noexcept;

		/**
		 * Retrieves the movement type of this unit.
		 * @return The index of the movement type of this unit.
		 */
		awe::BankID getMovementTypeIndex() const noexcept;

		/**
		 * Retrieves a pointer to the details of the type of movement this unit
		 * has.
		 * @return The pointer to the movement type's properties.
		 * @sa     updateMovementType()
		 */
		std::shared_ptr<const awe::movement_type> getMovementType() const noexcept;

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
		 * Retrieves the amount of fuel consumed at the beginning of each day.
		 * @return The amount of fuel this unit type consumes at the beginning of
		 *         each day.
		 */
		signed int fuelPerTurn() const noexcept;

		/**
		 * Updates the stored unit type properties pointers for units that can be
		 * loaded onto this one.
		 * @param unitBank A reference to the unit type bank to pull the pointers
		 *                 from.
		 */
		void updateUnitTypes(const bank<unit_type>& unitBank) const noexcept;

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
		 * The amount of fuel this unit type consumes at the beginning of every
		 * day.
		 */
		signed int _fuelPerTurn = 0;
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
		 * @param id The ID of the bank entry.
		 * @param j  The object value containing the commander's properties.
		 */
		commander(const awe::BankID id, engine::json& j) noexcept;

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
	void updateAllTerrains(bank<tile_type>& tileBank,
		const awe::bank<awe::terrain>& terrainBank) noexcept;

	/**
	 * Calls \c unit_type::updateMovementType() and \c unit_type::updateUnitTypes
	 * on an entire bank of \c unit_type objects.
	 * @param unitBank     The \c unit_type bank to update. Also the \c unit_type
	 *                     bank that is used to update itself.
	 * @param movementBank The \c movement_type bank to pull the pointers from.
	 */
	void updateAllMovementsAndLoadedUnits(bank<unit_type>& unitBank,
		const awe::bank<awe::movement_type>& movementBank) noexcept;
}

template<typename T>
std::shared_ptr<const T> awe::bank<T>::operator[](const awe::BankID id) const
	noexcept {
	if (id >= size()) return nullptr;
	return _bank[id];
}

template<typename T>
std::size_t awe::bank<T>::size() const noexcept {
	return _bank.size();
}

template<typename T>
bool awe::bank<T>::_load(engine::json& j) noexcept {
	_bank.clear();
	nlohmann::ordered_json jj = j.nlohmannJSON();
	awe::BankID id = 0;
	for (auto& i : jj.items()) {
		// loop through each object, allowing the template type T to construct its
		// values based on each object
		engine::json input(i.value());
		_bank.push_back(std::make_shared<const T>(id++, input));
	}
	return true;
}

template<typename T>
bool awe::bank<T>::_save(nlohmann::ordered_json& j) noexcept {
	return false;
}