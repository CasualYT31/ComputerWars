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

#include "game.h"
#include <fstream>

awe::game::game(const std::string& name) noexcept : _logger(name) {}

bool awe::game::read(std::string filename) noexcept {
	std::shared_ptr<awe::map> map;
	std::shared_ptr<std::vector<std::shared_ptr<awe::army>>> armies;
	try {
		std::shared_ptr<std::fstream> file = _openFile(filename, true);

		sf::Uint32 version = _readNumber<sf::Uint32>(file);

		if (version == 1297564416) { // first version of the standard format

		} else {
			_logger.error("The version of the format used in the map file \"{}\" is unsupported: {}.", filename, version);
			return false;
		}
	} catch (std::exception& e) {
		_logger.error("An error occurred when attempting to open map file \"{}\": {}", filename, e.what());
		return false;
	}
	_map = map;
	_armies = armies;
	_filename = filename;
	return true;
}

bool awe::game::write(std::string filename) noexcept {
	try {
		std::shared_ptr<std::fstream> file = _openFile(filename, false);

		*file << VERSION_NUMBER;
	} catch (std::exception& e) {
		_logger.error("An error occurred when attempting to write to map file \"{}\": {}", filename, e.what());
		return false;
	}
	_filename = filename;
	return true;
}

bool awe::game::_isBigEndian() const noexcept {
	/* Explanation
	1. Define an integer holding the value 1.
	2. Retrieve its address.
	3. Convert that address to an address pointing to a single byte instead.
	4. Access the value of the integer's first byte (the one with the smallest address).
	5. If it is still 1, then we can be sure the system is running on little endian,
	   as this signifies that the least significant byte is stored FIRST.
	https://developer.ibm.com/articles/au-endianc/ */
	static const int i = 1;
	return (*(char*)&i) == 0;
}

std::shared_ptr<std::fstream> awe::game::_openFile(std::string& filename, bool forInput) {
	if (filename == "") filename = _filename;
	std::shared_ptr<std::fstream> ret = std::make_shared<std::fstream>();
	ret->exceptions(std::fstream::failbit | std::fstream::badbit | std::fstream::eofbit);
	if (forInput) {
		ret->open(filename, std::ios::binary | std::ios::in);
	} else {
		ret->open(filename, std::ios::binary | std::ios::out | std::ios::trunc);
	}
	return ret;
}

std::shared_ptr<awe::map> awe::game::getMap() const noexcept {
	return _map;
}

std::shared_ptr<awe::army> awe::game::getArmy(std::size_t i) const noexcept {
	if (i < getNumberOfArmies()) return (*_armies)[i];
	return std::shared_ptr<awe::army>();
}

std::size_t awe::game::getNumberOfArmies() const noexcept {
	if (_armies) return _armies->size();
	return 0;
}