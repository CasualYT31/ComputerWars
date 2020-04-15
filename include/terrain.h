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

#include "property.h"
#include "texture.h"

namespace awe {
	struct terrain : public awe::property {
		std::string description = "";
		int max_hp = 0;
		int defence = 0;
		std::unordered_map<unsigned int, int> movecost;
	};

	class terrain_bank : public safe::json_script {
	public:
		terrain_bank(awe::movement* movetypes, const std::string& name = "terrain_bank") noexcept;
		const terrain* operator[](const std::size_t& id) const noexcept;
		bool find(const std::size_t& id) const noexcept;
	private:
		virtual bool _load(safe::json& j) noexcept;
		virtual bool _save(nlohmann::json& j) noexcept;

		global::logger _logger;
		std::vector<terrain> _types;
		awe::movement* _movementTypes = nullptr;
	};

	struct terrain_tile {
		const terrain* type = nullptr;
		std::unordered_map<unsigned int, int> tile; //animted tiles for each country
	};

	class terrain_tile_bank : public safe::json_script {
	public:
		terrain_tile_bank(terrain_bank* tBank, awe::country* countries, const std::string& name = "tile_bank") noexcept;
		const terrain_tile* operator[](const std::size_t& id) const noexcept;
		bool find(const std::size_t& id) const noexcept;
	private:
		virtual bool _load(safe::json& j) noexcept;
		virtual bool _save(nlohmann::json& j) noexcept;

		global::logger _logger;
		std::vector<terrain_tile> _tiles;
		terrain_bank* _bank = nullptr;
		awe::country* _countries = nullptr;
	};

	class tile {
	public:
		tile(const sfx::spritesheet* spritesheet, const terrain_tile* tile = nullptr, const int hp = 0, const unsigned int owner = 0) noexcept;
		const terrain_tile* setTile(const terrain_tile* newTile) noexcept;
		const terrain_tile* getTile() const noexcept;
		int setHP(const int newHP) noexcept;
		int getHP() const noexcept;
		unsigned int setOwner(const unsigned int newOwner) noexcept;
		unsigned int getOwner() const noexcept;
	private:
		const terrain_tile* _tileType = nullptr; //not dynamically allocated within the class, so no destructor is required
		int _hp = 0;
		unsigned int _owner = 0;
	};
}

/*

#include "property.h"

namespace awe {
	class terrain /*: public safe::json_script {
	public:
		virtual ~terrain() noexcept; //always declare destructors virtual for polymorphic base classes
		virtual void startOfTurn() noexcept;
	protected:
		terrain() noexcept;
		/*virtual bool _load(safe::json& j) noexcept;
		virtual bool _save(nlohmann::json& j) noexcept;

		//global::logger _logger;
		awe::property _property;
		std::string _description;
		int _defence = 0;
		std::vector<int> _movementCost;

		//dynamic data

	};
}

*/