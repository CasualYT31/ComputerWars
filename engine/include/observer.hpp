/*Copyright 2019-2023 CasualYouTuber31 <naysar@protonmail.com>

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
 * @file observer.hpp
 * Declares the C++ and AngelScript interface for an observer object.
 */

#pragma once

#include <any>

namespace engine {
	/**
	 * A C++ object that listens to a subject for notifications.
	 */
	class observer {
	public:
		/**
		 * Polymorphic base classes should have virtual destructors.
		 */
		virtual ~observer() noexcept = default;

		/**
		 * Subjects should call this method when they need to notify listeners of
		 * changes.
		 * @param type Defines the type of notification emitted. This will be a
		 *             value that's completely subject-defined. It may not be used
		 *             or it may be set, depending on the needs of the subject.
		 * @param data Additional data concerning the notification will be stored
		 *             in here. You can use the \c type parameter to figure out
		 *             what type of data is stored in this object.
		 */
		virtual void update(const int type, const std::any& data) = 0;
	};
}
