/*Copyright 2019-2024 CasualYouTuber31 <naysar@protonmail.com>

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

/**@file resourcepool.hpp
 * Defines the base class used by basic resource containers.
 */

#pragma once

#include "safejson.hpp"

namespace engine {
	/**
	 * Defines a pool of \c T resources loaded using a JSON script.
	 * The intention of this class is to populate \c _pool yourself in your sub
	 * classes using \c _load().
	 * @tparam T The type of object to store in this container.
	 */
	template<typename T>
	class resource_pool : public engine::json_script {
	public:
		/**
		 * Used with the objects stored in this pool.
		 */
		typedef std::shared_ptr<T> ObjectType;

		/**
		 * Used with the pool itself.
		 */
		typedef std::unordered_map<std::string, ObjectType> PoolType;

		/**
		 * Iterator typedef.
		 */
		typedef typename PoolType::iterator iterator;

		/**
		 * Const iterator typedef.
		 */
		typedef typename PoolType::const_iterator const_iterator;

		/**
		 * Accesses a previously loaded \c T object.
		 * If an object with the given does not exist, an error will be logged.
		 * @param  key The string name of the object which was given in the JSON
		 *             script.
		 * @return The pointer to the \c sfx::animated_spritesheet object, or
		 *         \c nullptr if the spritesheet didn't exist.
		 */
		ObjectType operator[](const std::string& key) const;

		/**
		 * Finds out if there is an object stored under the given key.
		 * @param  key The key to test.
		 * @return \c TRUE if there is an object with the given name, \c FALSE if
		 *         not.
		 */
		inline bool exists(const std::string& key) const {
			return _pool.find(key) != _pool.end();
		}

		/**
		 * Points to the beginning of the pool.
		 * @return Internal pool \c iterator.
		 */
		inline iterator begin() {
			return _pool.begin();
		}

		/**
		 * Points to the end of the pool.
		 * @return Internal pool \c iterator.
		 */
		inline iterator end() {
			return _pool.end();
		}

		/**
		 * Points to the beginning of the pool.
		 * @return Internal pool \c const_iterator.
		 */
		inline const_iterator cbegin() const {
			return _pool.cbegin();
		}

		/**
		 * Points to the end of the pool.
		 * @return Internal pool \c const_iterator.
		 */
		inline const_iterator cend() const {
			return _pool.cend();
		}
	protected:
		/**
		 * Initialises the internal logger object.
		 * @param data The data to initialise the logger object with.
		 * @param name A string that describes the types of objects stored in this
		 *             pool. Used for logging purposes.
		 * @sa    \c engine::logger
		 */
		resource_pool(const engine::logger::data& data, const std::string& name);

		/**
		 * The internal logger object.
		 * Subclasses are encouraged to use this logger, too.
		 */
		mutable engine::logger _logger;

		/**
		 * The resource pool.
		 */
		PoolType _pool;

		/**
		 * String describing the types of objects stored in this pool.
		 */
		const std::string _objectType;
	};
}

#include "tpp/resourcepool.tpp"
