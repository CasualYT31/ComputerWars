/*Copyright 2019-2022 CasualYouTuber31 <naysar@protonmail.com>

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

#include "file.hpp"

engine::binary_file::binary_file() noexcept {
	_file.exceptions(std::fstream::failbit |
		std::fstream::badbit | std::fstream::eofbit);
}

bool engine::binary_file::isBigEndian() noexcept {
	/* Explanation
	1. Define an integer holding the value 1.
	2. Retrieve its address.
	3. Convert that address to an address pointing to a single byte instead.
	4. Access the value of the integer's first byte (the one with the smallest
	   address).
	5. If it is still 1, then we can be sure the system is running on little
	   endian, as this signifies that the least significant byte is stored FIRST.
	https://developer.ibm.com/articles/au-endianc/ */
	static const int i = 1;
	return (*(char*)&i) == 0;
}

void engine::binary_file::open(const std::string& filepath, const bool forInput) {
	try {
		close();
		if (forInput) {
			_file.open(filepath, std::ios::binary | std::ios::in);
		} else {
			_file.open(filepath, std::ios::binary | std::ios::out |
				std::ios::trunc);
		}
		_bytes = 0;
	} catch (std::exception&) {
		throw std::exception("Could not open file.");
	}
}

void engine::binary_file::close() {
	try {
		if (_file.is_open()) _file.close();
	} catch (std::exception&) {
		throw std::exception("Could not close file.");
	}
}

sf::Uint64 engine::binary_file::position() const noexcept {
	return _bytes;
}

bool engine::binary_file::readBool() {
	try {
		unsigned char inp;
		_file.read(reinterpret_cast<char*>(&inp), sizeof(inp));
		_bytes += sizeof(inp);
		return inp;
	} catch (std::exception& e) {
		std::string w = "Failed to read bool at position " +
			std::to_string(_bytes) + ": " + e.what();
		throw std::exception(w.c_str());
	}
}

void engine::binary_file::writeBool(const bool val) {
	try {
		unsigned char out = val ? 0xFF : 0x00;
		_file.write(reinterpret_cast<char*>(&out), sizeof(out));
		_bytes += sizeof(out);
	} catch (std::exception& e) {
		std::string w = "Failed to write bool to position " +
			std::to_string(_bytes) + ": " + e.what();
		throw std::exception(w.c_str());
	}
}

std::string engine::binary_file::readString() {
	try {
		sf::Uint32 len = 0;
		_file.read(reinterpret_cast<char*>(&len), sizeof(len));
		_bytes += sizeof(len);
		std::string ret;
		for (sf::Uint32 i = 0; i < len; i++) {
			char inp;
			_file.read(&inp, sizeof(inp));
			_bytes += sizeof(inp);
			ret += inp;
		}
		return ret;
	} catch (std::exception& e) {
		std::string w = "Failed to read string at position " +
			std::to_string(_bytes) + ": " + e.what();
		throw std::exception(w.c_str());
	}
}

void engine::binary_file::writeString(const std::string& str) {
	try {
		sf::Uint32 len = (sf::Uint32)str.length();
		_file.write(reinterpret_cast<char*>(&len), sizeof(len));
		_bytes += sizeof(len);
		for (sf::Uint32 i = 0; i < len; i++) {
			_file.write(&str.at(i), 1);
			_bytes += 1;
		}
	} catch (std::exception& e) {
		std::string w = "Failed to write string to position " +
			std::to_string(_bytes) + ": " + e.what();
		throw std::exception(w.c_str());
	}
}
