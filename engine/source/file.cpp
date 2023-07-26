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

#include "file.hpp"

engine::binary_file::binary_file(const engine::logger::data& data) : _logger(data)
	{
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
		_logger.write("Successfully opened file \"{}\" for {}.", filepath,
			((forInput) ? ("input") : ("output")));
	} catch (const std::exception& e) {
		_logger.error("Could not open file \"{}\" for {}: {}", filepath,
			((forInput) ? ("input") : ("output")), e);
		throw e;
	}
}

void engine::binary_file::close() {
	try {
		if (_file.is_open()) {
			_file.close();
			_logger.write("Successfully closed file.");
		}
	} catch (const std::exception& e) {
		_logger.error("Could not close file.");
		throw e;
	}
}

sf::Uint64 engine::binary_file::position() const noexcept {
	return _bytes;
}

bool engine::binary_file::readBool() {
	unsigned char inp = 0;
	try {
		_file.read(reinterpret_cast<char*>(&inp), sizeof(inp));
		_bytes += sizeof(inp);
		return inp;
	} catch (const std::exception& e) {
		_logger.error("Failed to read bool of size {} at position {}: {}",
			sizeof(inp), _bytes, e);
		throw e;
	}
}

void engine::binary_file::readBool(bool& boolean) {
	const auto booleanRead = readBool();
	boolean = booleanRead;
}

void engine::binary_file::writeBool(const bool val) {
	unsigned char out = val ? 0xFF : 0x00;
	try {
		_file.write(reinterpret_cast<char*>(&out), sizeof(out));
		_bytes += sizeof(out);
	} catch (const std::exception& e) {
		_logger.error("Failed to write bool {} of size {} to position {}: {}", out,
			sizeof(out), _bytes, e);
		throw e;
	}
}

std::string engine::binary_file::readString() {
	sf::Uint32 len = 0;
	try {
		_file.read(reinterpret_cast<char*>(&len), sizeof(len));
		_bytes += sizeof(len);
	} catch (const std::exception& e) {
		_logger.error("Failed to read string length of size {} at position {}: {}",
			sizeof(len), _bytes, e);
		throw e;
	}
	sf::Uint32 i = 0;
	try {
		std::string ret;
		for (; i < len; ++i) {
			char inp;
			_file.read(&inp, sizeof(inp));
			_bytes += sizeof(inp);
			ret += inp;
		}
		return ret;
	} catch (const std::exception& e) {
		_logger.error("Failed to read character {} at position {} for string of "
			"length {}: {}", i, _bytes, len, e);
		throw e;
	}
}

void engine::binary_file::readString(std::string& str) {
	const auto stringRead = readString();
	str = stringRead;
}

void engine::binary_file::writeString(const std::string& str) {
	sf::Uint32 len = (sf::Uint32)str.length();
	try {
		_file.write(reinterpret_cast<char*>(&len), sizeof(len));
		_bytes += sizeof(len);
	} catch (const std::exception& e) {
		_logger.error("Failed to write string \"{}\"'s size {} to position {}: "
			"{}", str, len, _bytes, e);
		throw e;
	}
	sf::Uint32 i = 0;
	try {
		for (; i < len; ++i) {
			_file.write(&str.at(i), 1);
			_bytes += 1;
		}
	} catch (const std::exception& e) {
		_logger.error("Failed to write character {} ('{}') of string \"{}\" "
			"(size {}) to position {}: {}", i, str.at(i), str, len, _bytes, e);
		throw e;
	}
}
