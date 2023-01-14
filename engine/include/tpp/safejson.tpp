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
void engine::json::apply(T& dest, engine::json::KeySequence keys,
	const bool suppressErrors) {
	nlohmann::ordered_json test;
	if (_performInitialChecks(keys, test, dest)) {
		dest = test.get<T>();
	} else {
		if (suppressErrors) resetState();
		_logger.write("{} property faulty: left to the default of {}.",
			synthesiseKeySequence(keys), dest);
	}
}

template<typename T, std::size_t N>
void engine::json::applyArray(std::array<T, N>& dest,
	engine::json::KeySequence keys) {
	if (!N) return;
	nlohmann::ordered_json test;
	if (_performInitialChecks(keys, test, dest, "array")) {
		if (test.size() == N) {
			nlohmann::ordered_json testDataType = dest[0];
			// Loop through each element in the JSON array to see if it matches
			// completely in data type.
			for (std::size_t i = 0; i < N; ++i) {
				// If the JSON array is homogeneous, assign it to the destination
				// array.
				if (i == N - 1 && equalType(testDataType, test[i])) {
					for (std::size_t j = 0; j < N; ++j) dest[j] = test[j].get<T>();
				} else if (!equalType(testDataType, test[i])) {
					_logger.error("The specified JSON array was not homogeneous, "
						"found an element of data type \"{}\" when attempting to "
						"assign to an array of data type \"{}\", in the key "
						"sequence {}.", _getTypeName(test[i]),
						_getTypeName(testDataType), synthesiseKeySequence(keys));
					_toggleState(MISMATCHING_ELEMENT_TYPE);
					break;
				}
			}
		} else {
			_logger.error("The size of the JSON array specified ({}) does not "
				"match with the size of the provided array ({}), in the key "
				"sequence {}.", test.size(), N, synthesiseKeySequence(keys));
			_toggleState(MISMATCHING_SIZE);
		}
	}
}

template<typename T>
void engine::json::applyVector(std::vector<T>& dest,
	engine::json::KeySequence keys) {
	nlohmann::ordered_json test;
	if (_performInitialChecks(keys, test, dest, "vector")) {
		nlohmann::ordered_json testDataType = T();
		for (std::size_t i = 0; i < test.size(); ++i) {
			if (i == test.size() - 1 && equalType(testDataType, test[i])) {
				dest.clear();
				for (std::size_t j = 0; j < test.size(); j++) {
					dest.push_back(test[j].get<T>());
				}
			} else if (!equalType(testDataType, test[i])) {
				_logger.error("The specified JSON array was not homogeneous, "
					"found an element of data type \"{}\" when attempting to "
					"assign to a vector of data type \"{}\", in the key sequence "
					"{}.", _getTypeName(test[i]), _getTypeName(testDataType),
					synthesiseKeySequence(keys));
				_toggleState(MISMATCHING_ELEMENT_TYPE);
				break;
			}
		}
	}
}

template<typename T>
void engine::json::applyMap(std::unordered_map<std::string, T>& dest,
	KeySequence keys, const bool continueReadingOnTypeError) {
	nlohmann::ordered_json test;
	if (_performInitialChecks(keys, test, dest, "map")) {
		nlohmann::ordered_json testDataType = T();
		for (const auto& item : test.items()) {
			if (equalType(testDataType, item.value())) {
				dest[item.key()] = item.value();
			} else {
				_logger.error("The specified JSON object was not homogeneous, "
					"found a value of data type \"{}\" with key \"{}\" when "
					"attempting to assign to a map with values of data type "
					"\"{}\", in the key sequence {}.{}", _getTypeName(
					item.value()), item.key(), _getTypeName(testDataType),
					synthesiseKeySequence(keys), ((continueReadingOnTypeError) ?
						("") : (" Will now stop reading key-value pairs from this "
							"object.")));
				_toggleState(MISMATCHING_ELEMENT_TYPE);
				if (!continueReadingOnTypeError) break;
			}
		}
	}
}
