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
 * @file game.h
 * Declares the class used to bring together the \c awe::map and \c awe::army classes.
 */

#pragma once

#include "map.h"
#include "army.h"

// for documentation on the awe namespace, please see bank.h
namespace awe {
	/**
	 * Class that represents a single game of Advance Wars, with a map and its armies.
	 */
	class game {
	public:
		bool read(const std::string& filename = "") noexcept;
		bool write(const std::string& filename = "") noexcept;
	private:
		std::string _filename = "";
		awe::map _map;
		std::vector<awe::army> _armies;
	};
}