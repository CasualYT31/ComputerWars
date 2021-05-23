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

/**@file army.h
 * Declares the class which represents an in-game army.
 */

#include "bank.h"

#pragma once

// for documentation on the awe namespace, please see bank.h
namespace awe {
	/**
	 * Class which represents a single army on a map.
	 */
	class army {
	public:
		/**
		 * Constructs a new army.
		 * @param country The country the army belongs to, which can't be changed.
		 */
		army(const std::shared_ptr<const awe::country>& country) noexcept;

		/**
		 * Gets the army's country information.
		 * @return The army's country information.
		 */
		std::shared_ptr<const awe::country> getCountry() const noexcept;

		/**
		 * Updates the army's fund count.
		 * If a value below \c 0 is given, \c 0 will be stored.
		 * @param funds The fund count to replace this army's old fund count with.
		 */
		void setFunds(const awe::Funds funds) noexcept;

		/**
		 * Retrieves the army's fund count.
		 * By default, it is \c 0.
		 * @return The funds this army obtains.
		 */
		awe::Funds getFunds() const noexcept;
	private:
		/**
		 * The country of the army.
		 */
		const std::shared_ptr<const awe::country> _country;

		/**
		 * The funds this army obtains.
		 */
		awe::Funds _funds = 0;
	};
}