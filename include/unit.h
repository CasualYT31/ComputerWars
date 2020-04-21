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

namespace awe {
	struct unit_data : public awe::property {
		std::string description = "";
		unsigned int movementTypeID = 0;
		int cost = 0;
		int max_fuel = 0;
		int max_ammo = 0;
		int max_hp = 100;
	};

	class unit_bank : public safe::json_script {
	public:
		unit_bank(awe::movement* movetypes, const std::string& name = "unit_bank") noexcept;
		const unit_data* operator[](const std::size_t& id) const noexcept;
		bool find(const std::size_t& id) const noexcept;
	private:
		virtual bool _load(safe::json& j) noexcept;
		virtual bool _save(nlohmann::json& j) noexcept;

		global::logger _logger;
		std::vector<unit_data> _types;
		awe::movement* _movementTypes = nullptr;
	};

	class unit {
	public:
		unit(const unit_data* type = nullptr, const unsigned int owner = 0, const int hp = 0, const int fuel = 0, const int ammo = 0) noexcept;
		const unit_data* setType(const unit_data* newType) noexcept;
		const unit_data* getType() const noexcept;
		unsigned int setOwner(const unsigned int newOwner) noexcept;
		unsigned int getOwner() const noexcept;
		int setHP(const int newHP) noexcept;
		int getHP() const noexcept;
		int setFuel(const int newFuel) noexcept;
		int getFuel() const noexcept;
		int setAmmo(const int newAmmo) noexcept;
		int getAmmo() const noexcept;
	private:
		const unit_data* _unitType = nullptr;
		unsigned int _owner = 0;
		int _hp = 0;
		int _fuel = 0;
		int _ammo = 0;
	};
}