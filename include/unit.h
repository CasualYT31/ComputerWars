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

#include "bank.h"

namespace awe {
	class unit {
	public:
		unit(const unit_type* type = nullptr, const int hp = 0, const int fuel = 0, const int ammo = 0) noexcept;
		const unit_type* setType(const unit_type* newType) noexcept;
		const unit_type* getType() const noexcept;
		int setHP(const int newHP) noexcept;
		int getHP() const noexcept;
		int setFuel(const int newFuel) noexcept;
		int getFuel() const noexcept;
		int setAmmo(const int newAmmo) noexcept;
		int getAmmo() const noexcept;
	private:
		const unit_type* _unitType = nullptr;
		int _hp = 0;
		int _fuel = 0;
		int _ammo = 0;
	};
}