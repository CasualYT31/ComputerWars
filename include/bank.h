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

#pragma once
/*
#include "texture.h"

namespace awe {
	template<typename T>
	class bank : public safe::json_script {
	public:
		typedef unsigned int index;
		~bank() noexcept;
		const T* operator[](index id) const noexcept;
		std::size_t size() const noexcept;
	private:
		virtual bool _load(safe::json& j) noexcept;
		virtual bool _save(nlohmann::json& j) noexcept;
		std::vector<const T*> _bank;
	};

	class common_properties {
	public:
		virtual ~common_properties() noexcept;
		const std::string& getName() const noexcept;
		const std::string& getShortName() const noexcept;
		sfx::SpriteKey getIconKey() const noexcept;
		const std::string& getDescription() const noexcept;
	protected:
		common_properties(safe::json& j) noexcept;
	private:
		std::string _name = "";
		std::string _shortName = "";
		sfx::SpriteKey _iconKey = 0;
		std::string _description = "";
	};

	class country : public common_properties {
	public:
		country(safe::json& j) noexcept;
		const sf::Color& getColour() const noexcept;
	private:
		sf::Color _colour;
	};

	class weather : public common_properties {
	public:
		weather(safe::json& j) noexcept;
	};

	class environment : public common_properties {
	public:
		environment(safe::json& j) noexcept;
	};

	class movement_type : public common_properties {
	public:
		movement_type(safe::json& j) noexcept;
	};

	class terrain : public common_properties {
	public:
		terrain(safe::json& j) noexcept;
		unsigned int getMaxHP() const noexcept;
		unsigned int getDefence() const noexcept;
		int getMoveCost(const bank<movement_type>::index movecostID) const noexcept;
		sfx::SpriteKey getPicture(const bank<country>::index countryID) const noexcept;
		bool isCapturable() const noexcept;
	private:
		unsigned int _maxHP = 0;
		unsigned int _defence = 0;
		std::vector<int> _movecosts;
		std::vector<sfx::SpriteKey> _pictures;
	};

	class tile_type {
	public:
		tile_type(safe::json& j) noexcept;
		bank<terrain>::index getTypeIndex() const noexcept;
		sfx::SpriteKey getTile(bank<country>::index countryID) const noexcept;
		const terrain* getType() const noexcept;
		void updateTerrain(const bank<terrain>& terrainBank) const noexcept;
	private:
		bank<terrain>::index _terrainType = 0;
		mutable const terrain* _terrain = nullptr;
		std::vector<sfx::SpriteKey> _tiles;
	};

	class unit_type : public common_properties {
	public:
		unit_type(safe::json& j) noexcept;
		bank<movement_type>::index getMovementType() const noexcept;
		sfx::SpriteKey getPicture(bank<country>::index countryID) const noexcept;
		sfx::SpriteKey getUnit(bank<country>::index countryID) const noexcept;
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
		std::vector<sfx::SpriteKey> _pictures;
		std::vector<sfx::SpriteKey> _units;
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
		sfx::SpriteKey getPortrait() const noexcept;
	private:
		sfx::SpriteKey _portrait = 0;
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
}*/