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

/**
 * @file uuid.h
 * Declares a simple UUID class that can be used to compare objects of
 * a particular type to see if they are the same, instead of comparing
 * each field of those objects, resulting in faster comparison checks.
 */

#pragma once

#include <cstdint>
#include "sfml/Config.hpp"

// for documentation on the engine namespace, please see dialogue.h
namespace engine {
	/**
	 * Class used to generate unique identifiers for objects of a given type.
	 * I decided to create my own implementation of UUIDs because it's simple enough
	 * to warrant not installing a dedicated backend for it.\n
	 * It was designed to work via object composition: simply declare a public field of this
	 * class within that class, like so:
	 * @code{.cpp}
	 * class Abcd {
	 * public:
	 *     engine::uuid<Abcd> ID;
	 * };
	 * @endcode
	 * @tparam T The type of object to create an ID for.
	 */
	template<typename T>
	class uuid {
	public:
		/**
		 * Constructs a UUID.
		 * @warning Note that once \c _id_counter reaches its maximum value, it will wrap
		 *          around to \c 1 again. This can cause problems if previous objects with
		 *          old IDs such as \c 1 and \c 2 have not been destroyed yet. However,
		 *          since unsigned 64-bit integers can store extremely large values, for
		 *          most cases the engine should not have to explicitly manage these cases:
		 *          simply ensure that no more than \c 18446744073709551615 units are in one
		 *          game at a time, for example 😂.
		 * @param   init Optionally initialise both \c _id_counter and \c _id to a given value.
		 *               This should only be given once. It is useful, for example, when you
		 *               have a collection of units in a binary map file with IDs that did not
		 *               start at \c 1 (this can easily happen when starting a second game in
		 *               the same execution). If left to the default value, \c _id will be
		 *               assigned \c _id_counter then the latter will increment.
		 */
		uuid(const sf::Uint64 init = 1) noexcept;

		/**
		 * Retrieves the UUID.
		 * @return The UUID of this object.
		 */
		sf::Uint64 getID() const noexcept;

		/**
		 * Comparison operator.
		 * @param  rhs The UUID to test against.
		 * @return \c TRUE if both UUIDs are identical, \c FALSE if not.
		 */
		bool operator==(const uuid<T>& rhs) const noexcept;

		/**
		 * Inverse comparison operator.
		 * @param  rhs The UUID to test against.
		 * @return \c TRUE if both UUIDs are not identical, \c FALSE if they are.
		 */
		bool operator!=(const uuid<T>& rhs) const noexcept;
	private:
		/**
		 * ID counter that works across all objects of type \c T.
		 */
		static sf::Uint64 _id_counter;

		/**
		 * The UUID of this instantiation.
		 */
		sf::Uint64 _id = 1;
	};
}

template<typename T>
sf::Uint64 engine::uuid<T>::_id_counter = 1;

template<typename T>
engine::uuid<T>::uuid(const sf::Uint64 init) noexcept {
	if (init != 1) _id_counter = init;
	_id = _id_counter++;
	if (_id_counter == UINT64_MAX) _id_counter = 1;
}

template<typename T>
sf::Uint64 engine::uuid<T>::getID() const noexcept {
	return _id;
}

template<typename T>
bool engine::uuid<T>::operator==(const engine::uuid<T>& rhs) const noexcept {
	return getID() == rhs.getID();
}

template<typename T>
bool engine::uuid<T>::operator!=(const engine::uuid<T>& rhs) const noexcept {
	return !(*this == rhs);
}