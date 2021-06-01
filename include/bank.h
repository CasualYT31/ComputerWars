/*Copyright 2020 CasualYouTuber31 <naysar@protonmail.com>

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
 * This header file declares classes used to store static game information specific to Computer Wars.
 * Static game information describes information that doesn't usually change during execution.
 * These include, but are not limited to:
 * <ul><li>Types of weather.</li>
 * <li>The countries available.</li>
 * <li>Movement types.</li>
 * <li>Types of terrain.</li>
 * <li>Types of units.</li>
 * <li>Commanders available.</li></ul>
 */

#pragma once

#include "texture.h"
#include "uuid.h"

/**
 * The \c awe namespace contains Computer Wars-specific code.
 */
namespace awe {
	/**
	 * Template class used to store lists of game properties.
	 * Static game informaion is stored in objects called \b banks, which are essentially glorified vectors.
	 * Banks utilise \c safe::json_script to load member information during runtime.
	 * Since they are intended to be "static," they cannot be saved (doing so wouldn't be very useful since they cannot be changed).
	 * Despite this, banks can load any number of sets of static data during runtime, each load overwritting what was previously stored.\n
	 * The classes declared in this header are intended to be used with this template class.
	 * For example, \c awe::bank<awe::country> defines a bank of country properties.
	 * @todo   Turn raw pointers into shared pointers.
	 * @tparam T The type of static game property to store within this bank.
	 */
	template<typename T>
	class bank : public safe::json_script {
	public:
		/**
		 * Typedef used to identify members of the bank.
		 * This acts as a way to ensure that IDs intended for different banks cannot be used with this one.
		 */
		typedef awe::BankID index;

		/**
		 * Allows the client to access the game properties of a bank member.
		 * @param  id The ID of the member to access.
		 * @return A pointer to the properties.
		 */
		std::shared_ptr<const T> operator[](index id) const noexcept;

		/**
		 * Calculates the size of the bank.
		 * @return The number of members or elements of the internal vector \c _bank.
		 */
		std::size_t size() const noexcept;
	private:
		/**
		 * The JSON load method for this class.
		 * All classes used with this template class should have a common JSON script format.\n
		 * In the root object, key-value pairs list each member of the bank and their properties.
		 * The key essentially represents the ID of the member, since the JSON backend stores key-value pairs in ASCII/UTF-8 order and not in the order defined within the script file.
		 * This means keys in the root object should be a four or three digit code representing the ID. For example, the first member should have a key of \c "000" or \c "0000".
		 * The next member should have a key of \c "001" or \c "0001", etc. The eleventh member would have a key of \c "010" or \c "0010". Keeping IDs in keys the same length gives a lot
		 * of room for the programmer/modder to add new properties without worrying about changing hundreds of keys later to preserve alphabetical order. Keep in mind that the key doesn't
		 * \b actually define the ID: it simply defines the order which values are read.\n
		 * It is then up to the classes used with this template class to parse the object value of these keys. The constructor of the class must accept a \c safe::json& parameter only,
		 * which it then uses to populate its members.
		 * @param  j The \c safe::json object representing the contents of the loaded script which this method reads.
		 * @return \c TRUE if all GUIs could be loaded, \c FALSE if at least one couldn't (\c _loadGUI() returned \c FALSE at least once).
		 */
		virtual bool _load(safe::json& j) noexcept;

		/**
		 * The JSON save method for this class.
		 * This class does not have the ability to be saved.
		 * @param  j The \c nlohmann::ordered_json object representing the JSON script which this method writes to.
		 * @return Always returns \c FALSE.
		 */
		virtual bool _save(nlohmann::ordered_json& j) noexcept;

		/**
		 * The internal vector of game properties.
		 */
		std::vector<std::shared_ptr<const T>> _bank;
	};

	/**
	 * Base class for all game property classes.
	 * Inheritance is to be used to implement these properties in your game property classes.
	 */
	class bank_id {
	public:
		/**
		 * Polymorphic base classes should have virtual destructors.
		 */
		virtual ~bank_id() noexcept;

		/**
		 * Retrieves the ID of this bank entry as defined during allocation of the entry.
		 * @return The 0-based ID.
		 */
		awe::BankID getID() const noexcept;
	protected:
		/**
		 * Constructor which assigns the ID to the bank entry.
		 * For classes that inherit from this one, this protected constructor is to be called,
		 * either in the subclass' constructor definition's initialiser list,
		 * or in the code block of the subclass constructor.
		 * @param id The ID of this bank entry.
		 */
		bank_id(const awe::BankID id) noexcept;
	private:
		/**
		 * The ID of this bank entry.
		 */
		awe::BankID _id = 0;
	};

	/**
	 * Base class containing properties common to a majority of game property classes.
	 * Inheritance is to be used to implement these properties in your game property classes.
	 */
	class common_properties : public awe::bank_id {
	public:
		/**
		 * Polymorphic base classes should have virtual destructors.
		 */
		virtual ~common_properties() noexcept;

		/**
		 * Retrieves the long name property.
		 * @return The long name property, which should be a language dictionary key.
		 */
		const std::string& getName() const noexcept;

		/**
		 * Retrieves the short name property.
		 * @return The short name property, which should be a language dictionary key.
		 */
		const std::string& getShortName() const noexcept;

		/**
		 * Retrieves the sprite ID of the icon associated with this property.
		 * @return The sprite ID.
		 */
		unsigned int getIconKey() const noexcept;

		/**
		 * Retrieves the description property.
		 * @return The description property, which should be a language dictionary key.
		 */
		const std::string& getDescription() const noexcept;
	protected:
		/**
		 * Constructor which reads a given JSON script object and fills in the properties accordingly.
		 * For classes that inherit from this one, this protected constructor is to be called,
		 * either in the subclass' constructor definition's initialiser list, or in the code block of the subclass constructor.
		 * The reference received in the subclass' constructor should be passed on directly without changes.\n
		 * The following keys correspond to the following properties:
		 * <ul><li>\c "longname" = \c _name</li>
		 * <li>\c "shortname" = \c _shortName</li>
		 * <li>\c "icon" = \c _iconKey</li>
		 * <li>\c "description" = \c _description</li></ul>
		 * @param id The ID of this bank entry.
		 * @param j  The object value containing the name, icon, and description properties.
		 */
		common_properties(const awe::BankID id, safe::json& j) noexcept;

		/**
		 * See \c awe::terrain::terrain(const awe::terrain*).
		 */
		common_properties(const awe::common_properties* old) noexcept;
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
		unsigned int _iconKey = 0;

		/**
		 * The description property.
		 */
		std::string _description = "";
	};

	/**
	 * A game property class which stores the information associated with a single country.
	 * @sa \c awe::common_properties
	 */
	class country : public common_properties {
	public:
		/**
		 * Constructor which scans a JSON object for the colour property.
		 * It also passes on the JSON object to the \c common_properties constructor.
		 * In addition to the keys defined in the superclass, the following keys are required:
		 * <ul><li>\c "colour" = \c _colour, <tt>([r,g,b,a])</tt></li></ul>
		 * @param id The ID of the bank entry.
		 * @param j  The object value containing the country's properties.
		 */
		country(const awe::BankID id, safe::json& j) noexcept;

		/**
		 * Retrieves the colour property.
		 * @return The colour of the country.
		 */
		const sf::Color& getColour() const noexcept;

		/**
		 * The object's UUID.
		 */
		engine::uuid<awe::country> UUID;

		/**
		 * Comparison operator.
		 * @param  rhs The \c country object to test against.
		 * @return \c TRUE if both object's UUIDs are the same, \c FALSE if not.
		 */
		bool operator==(const awe::country& rhs) const noexcept;

		/**
		 * Inverse comparison operator.
		 * @param  rhs The \c country object to test against.
		 * @return \c TRUE if both object's UUIDs are not the same, \c FALSE if they are.
		 */
		bool operator!=(const awe::country& rhs) const noexcept;
	private:
		/**
		 * The colour property.
		 */
		sf::Color _colour;
	};

	/**
	 * A game property class which stores the information associated with a single weather.
	 * @sa \c awe::common_properties
	 */
	class weather : public common_properties {
	public:
		/**
		 * Constructor which passes on the JSON object to the \c common_properties constructor.
		 * No additional keys are required.
		 * @param id The ID of the bank entry.
		 * @param j  The object value containing the weather's properties.
		 */
		weather(const awe::BankID id, safe::json& j) noexcept;

		/**
		 * The object's UUID.
		 */
		engine::uuid<awe::weather> UUID;

		/**
		 * Comparison operator.
		 * @param  rhs The \c weather object to test against.
		 * @return \c TRUE if both object's UUIDs are the same, \c FALSE if not.
		 */
		bool operator==(const awe::weather& rhs) const noexcept;

		/**
		 * Inverse comparison operator.
		 * @param  rhs The \c weather object to test against.
		 * @return \c TRUE if both object's UUIDs are not the same, \c FALSE if they are.
		 */
		bool operator!=(const awe::weather& rhs) const noexcept;
	};

	/**
	 * A game property class which stores the information associated with a single environment.
	 * An environment defines the tileset to use for a map (for example, normal, desert, snowy, etc.).
	 * @sa \c awe::common_properties
	 */
	class environment : public common_properties {
	public:
		/**
		 * Constructor which passes on the JSON object to the \c common_properties constructor.
		 * No additional keys are required.
		 * @param id The ID of the bank entry.
		 * @param j  The object value containing the environment's properties.
		 */
		environment(const awe::BankID id, safe::json& j) noexcept;

		/**
		 * The object's UUID.
		 */
		engine::uuid<awe::environment> UUID;

		/**
		 * Comparison operator.
		 * @param  rhs The \c environment object to test against.
		 * @return \c TRUE if both object's UUIDs are the same, \c FALSE if not.
		 */
		bool operator==(const awe::environment& rhs) const noexcept;

		/**
		 * Inverse comparison operator.
		 * @param  rhs The \c environment object to test against.
		 * @return \c TRUE if both object's UUIDs are not the same, \c FALSE if they are.
		 */
		bool operator!=(const awe::environment& rhs) const noexcept;
	};

	/**
	 * A game property class which stores the information associated with a single movement type.
	 * @sa \c awe::common_properties
	 */
	class movement_type : public common_properties {
	public:
		/**
		 * Constructor which passes on the JSON object to the \c common_properties constructor.
		 * No additional keys are required.
		 * @param id The ID of the bank entry.
		 * @param j  The object value containing the movement type's properties.
		 */
		movement_type(const awe::BankID id, safe::json& j) noexcept;

		/**
		 * The object's UUID.
		 */
		engine::uuid<awe::movement_type> UUID;

		/**
		 * Comparison operator.
		 * @param  rhs The \c movement_type object to test against.
		 * @return \c TRUE if both object's UUIDs are the same, \c FALSE if not.
		 */
		bool operator==(const awe::movement_type& rhs) const noexcept;

		/**
		 * Inverse comparison operator.
		 * @param  rhs The \c environment object to test against.
		 * @return \c TRUE if both object's UUIDs are not the same, \c FALSE if they are.
		 */
		bool operator!=(const awe::movement_type& rhs) const noexcept;
	};

	/**
	 * A game property class which stores the information associated with a single terrain type.
	 * @sa \c awe::common_properties
	 */
	class terrain : public common_properties {
	public:
		/**
		 * Constructor which scans a JSON object for the terrain type's properties.
		 * It also passes on the JSON object to the \c common_properties constructor.
		 * In addition to the keys defined in the superclass, the following keys are required:
		 * <ul><li>\c "hp" = \c _maxHP, <tt>(unsigned 32-bit int - capped off at signed 32-bit int's maximum value)</tt></li>
		 * <li>\c "defence" = \c _defenece, <tt>(unsigned 32-bit int)</tt></li>
		 * <li>\c "capturable" = \c _isCapturable, <tt>(bool)</tt></li>
		 * <li>\c "movecosts" = \c _movecosts, <tt>([signed 32-bit int{, signed 32-bit int, etc.}])</tt></li>
		 * <li>\c "pictures" = \c _pictures, <tt>([unsigned 32-bit int{, unsigned 32-bit int, etc.}])</tt></li></ul>
		 * The \c movecosts array stores a list of movement points associated with each movement type.
		 * For example, the first value will store the number of movement points it takes for the first type of movement to traverse over it (in the default implementation, Infantry).\n
		 * The \c pictures array stores a list of animated sprite IDs associated with each country.
		 * For example, the first value will store the ID of the sprite shown when the first country owns it (in the default implementation, Neutral).
		 * Not all countries have to be accounted for if the tile cannot be "owned," i.e. captured.
		 * @param id The ID of the bank entry.
		 * @param j  The object value containing the terrain type's properties.
		 */
		terrain(const awe::BankID id, safe::json& j) noexcept;

		/**
		 * Retrieves the maximum health property.
		 * This can be the health points of a cannon, or the capture points of a property.
		 * @return The health points this terrain can have.
		 */
		unsigned int getMaxHP() const noexcept;

		/**
		 * Retrieves the defence property.
		 * @return The defence this terrain provides.
		 */
		unsigned int getDefence() const noexcept;

		/**
		 * Retrieves the movement point cost property associated with a given movement type.
		 * @param  movecostID The ID of the type of movement.
		 * @return The movement point cost.
		 */
		int getMoveCost(const bank<movement_type>::index movecostID) const noexcept;

		/**
		 * Retrieves the sprite ID associated with a given country.
		 * @param  countryID The ID of the country.
		 * @return The sprite ID of the terrain picture, or \c UINT_MAX if the given country ID didn't identify a sprite ID.
		 */
		unsigned int getPicture(const bank<country>::index countryID) const noexcept;

		/**
		 * Retrieves the capturable property.
		 * @return The capturable property.
		 */
		bool isCapturable() const noexcept;

		/**
		 * Copies the internal list of movement costs and returns it.
		 * @return All the move costs assigned to this terrain.
		 */
		std::vector<int> copyMoveCosts() const noexcept;

		/**
		 * Copies the internal list of picture keys and returns it.
		 * @return All the pictures assigned to this terrain.
		 */
		std::vector<unsigned int> copyPictures() const noexcept;

		/**
		 * The object's UUID.
		 */
		engine::uuid<awe::terrain> UUID;

		/**
		 * Comparison operator.
		 * @param  rhs The \c terrain object to test against.
		 * @return \c TRUE if both object's UUIDs are the same, \c FALSE if not.
		 */
		bool operator==(const awe::terrain& rhs) const noexcept;

		/**
		 * Inverse comparison operator.
		 * @param  rhs The \c terrain object to test against.
		 * @return \c TRUE if both object's UUIDs are not the same, \c FALSE if they are.
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
		std::vector<unsigned int> _pictures;

		/**
		 * Capturable property.
		 */
		bool _isCapturable = false;
	};

	/**
	 * A game property class which stores the information associated with a single type of tile.
	 * Tiles and terrain types were separated in this way so that different visual representations
	 * of the same terrain can be maintained.
	 * For example, a road may be straight, a bend, a T-junction, or a crossroads.
	 */
	class tile_type : public awe::bank_id {
	public:
		/**
		 * Constructor which reads the given JSON object for tile properties.
		 * The following keys are required:
		 * <ul><li>\c "type" = \c _terrainType, <tt>(unsigned 32-bit int)</tt></li>
		 * <li>\c "neutral" = \c _neutralTile, <tt>(unsigned 32-bit int)</tt></li>
		 * <li>\c "tiles" = \c _tiles, <tt>([unsigned 32-bit int{, unsigned 32-bit int, etc.}])</tt></li></ul>
		 * The \c neutral key stores a sprite ID shown when the tile is not owned by any country. This should be given for
		 * all tile types.\n
		 * The \c tiles vector stores a list of animated sprite IDs associated with each country's version of the tile.
		 * For example, the first value will store the ID of the sprite shown on the map when the first country owns it.
		 * This vector does not have to be accounted for if the tile cannot be owned/captured. In which case, an empty
		 * vector should be given in the script.
		 * @param id The ID of the bank entry.
		 * @param j  The object value containing the tile type's properties.
		 */
		tile_type(const awe::BankID id, safe::json& j) noexcept;

		/**
		 * Retrieves the ID of the type of terrain this tile represents (i.e. "Plains" or "Road").
		 * @return The ID of the type of terrain.
		 */
		bank<terrain>::index getTypeIndex() const noexcept;

		/**
		 * Retrieves the ID of the sprite that is shown for a given country.
		 * @param  countryID The ID of the country.
		 * @return The ID of the tile's sprite, or \c _neutralTile if the given country ID didn't identify a sprite ID.
		 */
		unsigned int getOwnedTile(bank<country>::index countryID) const noexcept;

		/**
		 * Retrieves the ID of the sprite that is shown when no country owns the tile.
		 * @return The ID of the tile's neutral sprite.
		 */
		unsigned int getNeutralTile() const noexcept;

		/**
		 * Retrieves a pointer to the details of the type of terrain this tile represents.
		 * @return The pointer to the terrain type's properties.
		 * @sa     updateTerrain()
		 */
		std::shared_ptr<const terrain> getType() const noexcept;

		/**
		 * Updates the stored terrain type properties pointer.
		 * @param terrainBank A reference to the terrain bank to pull the pointer from.
		 */
		void updateTerrain(const bank<terrain>& terrainBank) const noexcept;

		/**
		 * The object's UUID.
		 */
		engine::uuid<awe::tile_type> UUID;

		/**
		 * Comparison operator.
		 * @param  rhs The \c tile_type object to test against.
		 * @return \c TRUE if both object's UUIDs are the same, \c FALSE if not.
		 */
		bool operator==(const awe::tile_type& rhs) const noexcept;

		/**
		 * Inverse comparison operator.
		 * @param  rhs The \c tile_type object to test against.
		 * @return \c TRUE if both object's UUIDs are not the same, \c FALSE if they are.
		 */
		bool operator!=(const awe::tile_type& rhs) const noexcept;
	private:
		/**
		 * The ID of the type of terrain this tile represents.
		 */
		bank<terrain>::index _terrainType = 0;

		/**
		 * Pointer to the properties of this tile's type of terrain.
		 * It was made mutable so that it can be updated after construction if an instance of \c tile_type is constant.
		 */
		mutable std::shared_ptr<const awe::terrain> _terrain;

		/**
		 * The sprite IDs of the tile corresponding to each country.
		 */
		std::vector<unsigned int> _tiles;

		/**
		 * The sprite ID of the tile with no owner.
		 */
		unsigned int _neutralTile = 0;
	};

	/**
	 * A game property class which stores the information associated with type of unit.
	 * @sa \c awe::common_properties
	 */
	class unit_type : public common_properties {
	public:
		/**
		 * Constructor which scans a JSON object for the unit type's properties.
		 * It also passes on the JSON object to the \c common_properties constructor.
		 * In addition to the keys defined in the superclass, the following keys are required:
		 * <ul><li>\c "movetype" = \c _movementTypeID, <tt>(unsigned 32-bit int)</tt></li>
		 * <li>\c "price" = \c _cost, <tt>(unsigned 32-bit int)</tt></li>
		 * <li>\c "fuel" = \c _maxFuel, <tt>(signed 32-bit int)</tt></li>
		 * <li>\c "ammo" = \c _maxAmmo, <tt>(signed 32-bit int)</tt></li>
		 * <li>\c "hp" = \c _maxHP, <tt>(unsigned 32-bit int - capped off at signed 32-bit int's maximum value)</tt></li>
		 * <li>\c "mp" = \c _movementPoints, <tt>(unsigned 32-bit int)</tt></li>
		 * <li>\c "vision" = \c _vision, <tt>(unsigned 32-bit int)</tt></li>
		 * <li>\c "lowrange" = \c _lowerRange, <tt>(unsigned 32-bit int)</tt></li>
		 * <li>\c "highrange" = \c _higherRange, <tt>(unsigned 32-bit int)</tt></li>
		 * <li>\c "pictures" = \c _pictures, <tt>([unsigned 32-bit int{, unsigned 32-bit int, etc.}])</tt></li>
		 * <li>\c "sprites" = \c _units, <tt>([unsigned 32-bit int{, unsigned 32-bit int, etc.}])</tt></li>
		 * <li>\c "canload" = \c _canLoadThese, <tt>([unsigned 32-bit int{, unsigned 32-bit int, etc.}])</tt></li>
		 * <li>\c "loadlimit" = \c _loadLimit, <tt>(unsigned 32-bit int)</tt></li>
		 * <li>\c "fuelperturn" = \c _fuelPerTurn, <tt>(signed 32-bit int)</tt></li></ul>
		 * The \c movecosts array stores a list of movement points associated with each movement type.
		 * For example, the first value will store the number of movement points it takes for the first type of movement to traverse over it (in the default implementation, Infantry).\n
		 * The \c pictures array stores a list of animated sprite IDs associated with each country.
		 * For example, the first value will store the ID of the sprite shown when the first country owns it (in the default implementation, Neutral).
		 * Not all countries have to be accounted for if the tile cannot be "owned," i.e. captured.
		 * Ranged values work by counting the number of tiles away from the unit's current tile.
		 * If the tile is within both the lower and higher ranges inclusive, then the attack is valid. If not, the attack is awe::army::NO_ARMY.\n
		 * Pictures is an array of sprite IDs corresponding to each country's portrait of the type of unit.\n
		 * Sprites is an array of sprite IDs corresponding to each country's map representation of the type of unit.
		 * @param id The ID of the bank entry.
		 * @param j  The object value containing the terrain type's properties.
		 * @sa    isInfiniteFuel()
		 * @sa    isInfiniteAmmo()*/
		unit_type(const awe::BankID id, safe::json& j) noexcept;

		/**
		 * Retrieves the movement type of this unit.
		 * @return The index of the movement type of this unit.
		 */
		bank<movement_type>::index getMovementTypeIndex() const noexcept;

		/**
		 * Retrieves a pointer to the details of the type of movement this unit has.
		 * @return The pointer to the movement type's properties.
		 * @sa     updateMovementType()
		 */
		std::shared_ptr<const movement_type> getMovementType() const noexcept;

		/**
		 * Updates the stored movement type properties pointer.
		 * @param movementBank A reference to the movement type bank to pull the pointer from.
		 */
		void updateMovementType(const bank<movement_type>& movementBank) const noexcept;

		/**
		 * Retrieves the sprite ID of a given country's portrait of this unit.
		 * @param  countryID The ID of the country.
		 * @return The sprite ID, or \c UINT_MAX if the given country ID didn't identify a sprite ID.
		 */
		unsigned int getPicture(bank<country>::index countryID) const noexcept;

		/**
		 * Retrieves the sprite ID of a given country's map sprite of this unit.
		 * @param  countryID The ID of the country.
		 * @return The sprite ID, or \c UINT_MAX if the given country ID didn't identify a sprite ID.
		 */
		unsigned int getUnit(bank<country>::index countryID) const noexcept;

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
		bool canLoad(const awe::bank<unit_type>::index typeID) const noexcept;

		/**
		 * Overloaded version of \c canLoad() that checks using a given unit type.
		 * @param  type The unit type to check for. Returns \c FALSE if empty.
		 * @return \c TRUE if the given unit type can be loaded onto units of this type, \c FALSE if not.
		 */
		bool canLoad(const std::shared_ptr<const awe::unit_type>& type) const noexcept;

		/**
		 * Retrieves the load limit of this unit type.
		 * @return The maximum number of units this unit can hold at any given time.
		 */
		unsigned int loadLimit() const noexcept;

		/**
		 * Retrieves the amount of fuel consumed at the beginning of each day.
		 * @return The amount of fuel this unit type consumes at the beginning of each day.
		 */
		signed int fuelPerTurn() const noexcept;

		/**
		 * Updates the stored unit type properties pointers for units that can be loaded onto this one.
		 * @param unitBank A reference to the unit type bank to pull the pointers from.
		 */
		void updateUnitTypes(const bank<unit_type>& unitBank) const noexcept;

		std::vector<unsigned int> copyPictures() const noexcept;

		std::vector<unsigned int> copyUnits() const noexcept;

		std::vector<awe::bank<unit_type>::index> copyLoadableUnitIDs() const noexcept;

		std::vector<std::shared_ptr<const awe::unit_type>> copyLoadableUnits() const noexcept;

		/**
		 * The object's UUID.
		 */
		engine::uuid<awe::unit_type> UUID;

		/**
		 * Comparison operator.
		 * @param  rhs The \c unit_type object to test against.
		 * @return \c TRUE if both object's UUIDs are the same, \c FALSE if not.
		 */
		bool operator==(const awe::unit_type& rhs) const noexcept;

		/**
		 * Inverse comparison operator.
		 * @param  rhs The \c unit_type object to test against.
		 * @return \c TRUE if both object's UUIDs are not the same, \c FALSE if they are.
		 */
		bool operator!=(const awe::unit_type& rhs) const noexcept;
	private:
		/**
		 * The movement type ID property.
		 */
		bank<movement_type>::index _movementTypeID = 0;

		/**
		 * Pointer to this unit's movement typre details.
		 * It was made mutable so that it can be updated after construction if an instance of \c unit_type is constant.
		 */
		mutable std::shared_ptr<const awe::movement_type> _movementType;

		/**
		 * The portrait IDs.
		 */
		std::vector<unsigned int> _pictures;

		/**
		 * The sprite IDs.
		 */
		std::vector<unsigned int> _units;

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
		std::vector<awe::bank<unit_type>::index> _canLoadThese;

		/**
		 * List of unit types that can be loaded onto this type of unit.
		 * It was made mutable so that it can be updated after construction if an instance of \c unit_type is constant.
		 */
		mutable std::vector<std::shared_ptr<const awe::unit_type>> _canLoadTheseUnitTypes;

		/**
		 * The maximum number of units this unit type can load.
		 */
		unsigned int _loadLimit = 0;

		/**
		 * The amount of fuel this unit type consumes at the beginning of every day.
		 */
		signed int _fuelPerTurn = 0;
	};

	/**
	 * A game property class which stores the information associated with a single commander.
	 * @sa \c awe::common_properties
	 */
	class commander : public common_properties {
	public:
		/**
		 * Constructor which scans a JSON object for the portrait property.
		 * It also passes on the JSON object to the \c common_properties constructor.
		 * In addition to the keys defined in the superclass, the following keys are required:
		 * <ul><li>\c "portrait" = \c _portrait, <tt>(unsigned 32-bit int)</tt></li></ul>
		 * @param id The ID of the bank entry.
		 * @param j  The object value containing the commander's properties.
		 */
		commander(const awe::BankID id, safe::json& j) noexcept;

		/**
		 * Retrieves the animated sprite ID of this commander's portrait.
		 * @return The animated sprite ID.
		 */
		unsigned int getPortrait() const noexcept;

		/**
		 * The object's UUID.
		 */
		engine::uuid<awe::commander> UUID;

		/**
		 * Comparison operator.
		 * @param  rhs The \c commander object to test against.
		 * @return \c TRUE if both object's UUIDs are the same, \c FALSE if not.
		 */
		bool operator==(const awe::commander& rhs) const noexcept;

		/**
		 * Inverse comparison operator.
		 * @param  rhs The \c commander object to test against.
		 * @return \c TRUE if both object's UUIDs are not the same, \c FALSE if they are.
		 */
		bool operator!=(const awe::commander& rhs) const noexcept;
	private:
		/**
		 * The portrait property.
		 */
		unsigned int _portrait = 0;
	};

	/**
	 * Calls \c tile_type::updateTerrain() on an entire bank of \c tile_type objects.
	 * @param tileBank    The \c tile_type bank to update.
	 * @param terrainBank The \c terrian bank to pull the pointers from.
	 */
	void updateAllTerrains(bank<tile_type>& tileBank, const awe::bank<awe::terrain>& terrainBank) noexcept;

	/**
	 * Calls \c unit_type::updateMovementType() and \c unit_type::updateUnitTypes on an entire bank of \c unit_type objects.
	 * @param unitBank     The \c unit_type bank to update. Conversely the \c unit_type bank that is used to update itself.
	 * @param movementBank The \c movement_type bank to pull the pointers from.
	 */
	void updateAllMovementsAndLoadedUnits(bank<unit_type>& unitBank, const awe::bank<awe::movement_type>& movementBank) noexcept;
}

template<typename T>
std::shared_ptr<const T> awe::bank<T>::operator[](index id) const noexcept {
	if (id >= size()) return nullptr;
	return _bank[id];
}

template<typename T>
std::size_t awe::bank<T>::size() const noexcept {
	return _bank.size();
}

template<typename T>
bool awe::bank<T>::_load(safe::json& j) noexcept {
	_bank.clear();
	nlohmann::ordered_json jj = j.nlohmannJSON();
	awe::BankID id = 0;
	for (auto& i : jj.items()) {
		//loop through each object, allowing the template type T to construct its values based on each object
		safe::json input(i.value());
		_bank.push_back(std::make_shared<const T>(id++, input));
	}
	return true;
}

template<typename T>
bool awe::bank<T>::_save(nlohmann::ordered_json& j) noexcept {
	return false;
}