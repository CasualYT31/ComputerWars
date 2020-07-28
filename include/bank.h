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
		typedef unsigned int index;

		/**
		 * Destroys all dynamically allocated members of the bank.
		 */
		~bank() noexcept;

		/**
		 * Allows the client to access the game properties of a bank member.
		 * @param  id The ID of the member to access.
		 * @return A pointer to the properties.
		 */
		const T* operator[](index id) const noexcept;

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
		 * @param  j The \c nlohmann::json object representing the JSON script which this method writes to.
		 * @return Always returns \c FALSE.
		 */
		virtual bool _save(nlohmann::json& j) noexcept;

		/**
		 * The internal vector of game properties.
		 */
		std::vector<const T*> _bank;
	};

	/**
	 * Base class containing properties common to a majority of game property classes.
	 * Inheritance is to be used to implement these properties in your game property classes.
	 */
	class common_properties {
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
		 * @param j The object value containing the name, icon, and description properties.
		 */
		common_properties(safe::json& j) noexcept;
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
		 * @param j The object value containing the country's properties.
		 */
		country(safe::json& j) noexcept;

		/**
		 * Retrieves the colour property.
		 * @return The colour of the country.
		 */
		const sf::Color& getColour() const noexcept;
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
		 * @param j The object value containing the weather's properties.
		 */
		weather(safe::json& j) noexcept;
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
		 * @param j The object value containing the environment's properties.
		 */
		environment(safe::json& j) noexcept;
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
		 * @param j The object value containing the movement type's properties.
		 */
		movement_type(safe::json& j) noexcept;
	};

	class terrain : public common_properties {
	public:
		terrain(safe::json& j) noexcept;
		unsigned int getMaxHP() const noexcept;
		unsigned int getDefence() const noexcept;
		int getMoveCost(const bank<movement_type>::index movecostID) const noexcept;
		unsigned int getPicture(const bank<country>::index countryID) const noexcept;
		bool isCapturable() const noexcept;
	private:
		unsigned int _maxHP = 0;
		unsigned int _defence = 0;
		std::vector<int> _movecosts;
		std::vector<unsigned int> _pictures;
	};

	class tile_type {
	public:
		tile_type(safe::json& j) noexcept;
		bank<terrain>::index getTypeIndex() const noexcept;
		unsigned int getTile(bank<country>::index countryID) const noexcept;
		const terrain* getType() const noexcept;
		void updateTerrain(const bank<terrain>& terrainBank) const noexcept;
	private:
		bank<terrain>::index _terrainType = 0;
		mutable const terrain* _terrain = nullptr;
		std::vector<unsigned int> _tiles;
	};

	class unit_type : public common_properties {
	public:
		unit_type(safe::json& j) noexcept;
		bank<movement_type>::index getMovementType() const noexcept;
		unsigned int getPicture(bank<country>::index countryID) const noexcept;
		unsigned int getUnit(bank<country>::index countryID) const noexcept;
		unsigned int getCost() const noexcept;
		int getMaxFuel() const noexcept;
		int getMaxAmmo() const noexcept;
		unsigned int getMaxHP() const noexcept;
		unsigned int getMovementPoints() const noexcept;
		unsigned int getVision() const noexcept;
		unsigned int getLowerRange() const noexcept;
		unsigned int getHigherRange() const noexcept;
		bool isInfiniteFuel() const noexcept;
		bool isInfiniteAmmo() const noexcept;
	private:
		bank<movement_type>::index _movementTypeID = 0;
		std::vector<unsigned int> _pictures;
		std::vector<unsigned int> _units;
		unsigned int _cost = 0;
		int _maxFuel = 0;
		int _maxAmmo = 0;
		unsigned int _maxHP = 100;
		unsigned int _movementPoints = 0;
		unsigned int _vision = 1;
		unsigned int _lowerRange = 1;
		unsigned int _higherRange = 1;
	};
	
	class commander : public common_properties {
	public:
		commander(safe::json& j) noexcept;
		unsigned int getPortrait() const noexcept;
	private:
		unsigned int _portrait = 0;
	};

	void updateAllTerrains(bank<tile_type>& bank, const awe::bank<awe::terrain>& terrainBank) noexcept;
}

template<typename T>
awe::bank<T>::~bank() noexcept {
	for (auto& i : _bank) {
		delete i;
	}
}

template<typename T>
const T* awe::bank<T>::operator[](index id) const noexcept {
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
	nlohmann::json jj = j.nlohmannJSON();
	for (auto& i : jj.items()) {
		//loop through each object, allowing the template type T to construct its values based on each object
		safe::json input(i.value());
		const T* pItem = new T(input);
		_bank.push_back(pItem);
	}
	return true;
}

template<typename T>
bool awe::bank<T>::_save(nlohmann::json& j) noexcept {
	return false;
}