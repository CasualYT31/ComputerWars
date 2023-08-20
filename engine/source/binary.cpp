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

#include "binary.hpp"
#include <fstream>

const bool engine::convert::BIG_ENDIAN = (*(char*)&engine::convert::ONE) == 0;

engine::binary_istream::binary_istream(const engine::logger::data& data) :
	_logger(data) {
	_stream.exceptions(std::stringstream::failbit |
		std::stringstream::badbit | std::stringstream::eofbit);
}

engine::binary_istream::binary_istream(const std::string& file,
	const engine::logger::data& data) : _logger(data) {
	_stream.exceptions(std::stringstream::failbit |
		std::stringstream::badbit | std::stringstream::eofbit);
	std::ifstream bin(file, std::ios::binary);
	bin >> *this;
}

void engine::binary_istream::readBool(bool& boolean) {
	unsigned char inp = 0;
	try {
		_checkRead(sizeof(inp));
		_stream.read(reinterpret_cast<char*>(&inp), sizeof(inp));
		_bytes += sizeof(inp);
		boolean = inp;
	} catch (const std::exception& e) {
		_logger.error("Failed to read bool of size {} at position {}: {}",
			sizeof(inp), _bytes, e);
		throw e;
	}
}

void engine::binary_istream::readString(std::string& str) {
	sf::Uint32 len = 0;
	readNumber(len);
	sf::Uint32 i = 0;
	try {
		std::string ret;
		for (; i < len; ++i) {
			char inp;
			_checkRead(sizeof(inp));
			_stream.read(&inp, sizeof(inp));
			_bytes += sizeof(inp);
			ret += inp;
		}
		str = ret;
	} catch (const std::exception& e) {
		_logger.error("Failed to read character {} at position {} for string of "
			"length {}: {}", i, _bytes, len, e);
		throw e;
	}
}

std::istream& engine::operator>>(std::istream& is, engine::binary_istream& data) {
	const auto oldpos = is.tellg();
	is.seekg(0, std::ios::end);
	const auto size = is.tellg();
	is.seekg(oldpos);
	char* const block = reinterpret_cast<char* const>(::malloc(size));
	if (block) {
		try {
			is.seekg(std::ios::beg);
			is.read(block, size);
			is.seekg(oldpos);
			data._stream.clear();
			data._stream.write(block, size);
			data._bytes = 0;
			data._totalBytes = size;
			::free(block);
		} catch (...) {
			::free(block);
			throw;
		}
	}
	return is;
}

engine::binary_ostream::binary_ostream(const engine::logger::data& data) :
	_logger(data) {
	_stream.exceptions(std::stringstream::failbit |
		std::stringstream::badbit | std::stringstream::eofbit);
}

void engine::binary_ostream::writeBool(const bool val) {
	try {
		const char t = val ? 0xFF : 0x00;
		_stream.write(&t, 1);
		++_bytes;
	} catch (const std::exception& e) {
		_logger.error("Failed to write bool {} of size {} to position {}: {}", val,
			1, _bytes, e);
		throw e;
	}
}

void engine::binary_ostream::writeString(const std::string& str) {
	sf::Uint32 len = static_cast<sf::Uint32>(str.length());
	writeNumber(len);
	try {
		_stream.write(str.c_str(), len);
		_bytes += len;
	} catch (const std::exception& e) {
		_logger.error("Failed to write string \"{}\" (size {}) to position {}: {}",
			str, len, _bytes, e);
		throw e;
	}
}

std::ostream& engine::operator<<(std::ostream& os, engine::binary_ostream& data) {
	char* const block = reinterpret_cast<char* const>(::malloc(data._bytes));
	if (block) {
		try {
			data._stream.read(block, data._bytes);
			data._stream.seekg(0);
			os.write(block, data._bytes);
			::free(block);
		} catch (...) {
			::free(block);
			throw;
		}
	}
	return os;
}

void engine::operator>>(engine::binary_ostream& from, engine::binary_istream& to) {
	from._stream >> to;
}
