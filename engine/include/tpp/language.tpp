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

#pragma once

template<typename T, typename... Ts>
std::string engine::expand_string::insert(const std::string& original, T value,
	Ts... values) noexcept {
	for (std::size_t c = 0; c < original.length(); c++) {
		if (original[c] == _varchar) {
			if (c < original.length() - 1 && original[c + 1] == _varchar) {
				// If two varchars appear in succession,
				// then one is printed, and the next one is ignored.
				_sstream << _varchar; // Print...
				c++; // Ignore...
			} else {
				_sstream << value;
				return insert(original.substr(c + 1), values...);
			}
		} else {
			_sstream << original[c];
		}
	}
	// Execution enters this point if more variables were given than varchars. In
	// which case, we've reached the end of the original string, so retrieve the
	// results and return them.
	return insert("");
}

template<typename... Ts>
std::string engine::language_dictionary::language::get(
	const std::string& nativeString, Ts... values) noexcept {
	if (_strings.find(nativeString) == _strings.end()) {
		if (nativeString.size() > 0 && nativeString[0] == TRANSLATION_OVERRIDE) {
			return nativeString.substr(1);
		} else {
			_logger.error("Native string \"{}\" does not exist in this string "
				"map.", nativeString);
			return "<error>";
		}
	} else {
		return engine::expand_string::insert(_strings.at(nativeString), values...);
	}
}

template<typename... Ts>
std::string engine::language_dictionary::operator()(
	const std::string& nativeString, Ts... values) noexcept {
	if (_currentLanguage == "") return engine::expand_string::insert(nativeString,
		values...);
	if (_languageMap) {
		return _languageMap->get(nativeString, values...);
	} else {
		_logger.error("Fatal - _languageMap was NULL.");
		return "<fatal>";
	}
}
