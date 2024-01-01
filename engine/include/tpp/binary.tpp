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

#pragma once

template<typename T>
T engine::convert::number(T number) noexcept {
	static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type!");
	if (sizeof(T) < 2) return number;
	T copy = number;
	for (std::size_t i = 0; i < sizeof(T); ++i) {
		*((unsigned char*)&number + i) =
			*((unsigned char*)&copy + sizeof(T) - i - 1);
	}
	return number;
}

template<typename T>
void engine::binary_istream::readNumber(T& number) {
	static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type!");
	try {
		T ret;
		_checkRead(sizeof(T));
		_stream.read(reinterpret_cast<char*>(&ret), sizeof(T));
		_bytes += sizeof(T);
		if (engine::convert::BIG_ENDIAN) ret = engine::convert::number(ret);
		number = ret;
	} catch (const std::exception& e) {
		_logger.error("Failed to read number of size {} at position {}: {}",
			sizeof(T), _bytes, e);
		throw e;
	}
}

template<typename T>
void engine::binary_ostream::writeNumber(T number) {
	static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type!");
	try {
		if (engine::convert::BIG_ENDIAN) number = engine::convert::number(number);
		_stream.write(reinterpret_cast<const char*>(&number), sizeof(T));
		_bytes += sizeof(T);
	} catch (const std::exception& e) {
		_logger.error("Failed to write number {} of size {} to position {}: {}",
			number, sizeof(T), _bytes, e);
		throw e;
	}
}
