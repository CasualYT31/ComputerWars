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

#pragma once

template<typename T>
T engine::binary_file::convertNumber(T number) noexcept {
	static_assert(std::is_arithmetic<T>::value,
		"Value must be an arithmetic type!");
	T copy = number;
	for (std::size_t i = 0; i < sizeof(T); i++) {
		*((unsigned char*)&number + i) =
			*((unsigned char*)&copy + sizeof(T) - i - 1);
	}
	return number;
}

template<typename T>
T engine::binary_file::readNumber() {
	static_assert(std::is_arithmetic<T>::value,
		"The return must be an arithmetic type!");
	try {
		T ret;
		_file.read(reinterpret_cast<char*>(&ret), sizeof(T));
		_bytes += sizeof(T);
		if (sizeof(T) > 1 && isBigEndian()) ret = convertNumber(ret);
		return ret;
	} catch (const std::exception& e) {
		_logger.error("Failed to read number of size {} at position {}: {}",
			sizeof(T), _bytes, e.what());
		throw e;
	}
}

template<typename T>
void engine::binary_file::readNumber(T& number) {
	const auto numberRead = readNumber<T>();
	number = numberRead;
}

template<typename T>
void engine::binary_file::writeNumber(T number) {
	static_assert(std::is_arithmetic<T>::value,
		"Value must be an arithmetic type!");
	try {
		if (sizeof(T) > 1 && isBigEndian()) number = convertNumber(number);
		_file.write(reinterpret_cast<char*>(&number), sizeof(T));
		_bytes += sizeof(T);
	} catch (const std::exception& e) {
		_logger.error("Failed to write number {} of size {} to position {}: {}",
			number, sizeof(T), _bytes, e.what());
		throw e;
	}
}
