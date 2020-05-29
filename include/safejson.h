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

/**@file safejson.h
 * Classes which are used to interact with JSON in a "safer" way.
 * Classes in this file allow clients to interact with JSON in a safer way by
 * reporting errors in assumptions that are made, such as the existance of a key
 * sequence or the type of a value. Nlohmann's JSON library is used as the backend
 * for these classes.
 */

#pragma once

#include "logger.h"
#include "nlohmann/json.hpp"
#include "sfml/Graphics/Color.hpp"

/**
 * The \c safe namespace contains classes used to interact with JSON in a "safer" way.
 */
namespace safe {
	/**
	 * This class is used to track the error state of all JSON-based classes.
	 * This is a superclass for the other classes in this namespace. It provides
	 * common error-tracking functionality using the error bit model found in the
	 * STL.
	 */
	class json_state {
	public:
		/**
		 * Polymorphic base classes must have virtual destructors.
		 */
		virtual ~json_state() noexcept;
		/**
		 * 
		 */
		typedef unsigned short FailBits;
		static const FailBits                  SUCCESS = 0b0000000000000000;
		static const FailBits      JSON_WAS_NOT_OBJECT = 0b0000000000000001;
		static const FailBits       KEYS_DID_NOT_EXIST = 0b0000000000000010;
		static const FailBits         MISMATCHING_TYPE = 0b0000000000000100;
		static const FailBits            NO_KEYS_GIVEN = 0b0000000000001000;
		static const FailBits       FAILED_LOAD_METHOD = 0b0000000000010000;
		static const FailBits       FAILED_SAVE_METHOD = 0b0000000000100000;
		static const FailBits       FAILED_SCRIPT_LOAD = 0b0000000001000000;
		static const FailBits       FAILED_SCRIPT_SAVE = 0b0000000010000000;
		static const FailBits               UNPARSABLE = 0b0000000100000000;
		static const FailBits         MISMATCHING_SIZE = 0b0000001000000000;
		static const FailBits MISMATCHING_ELEMENT_TYPE = 0b0000010000000000;
		bool inGoodState() const noexcept;
		FailBits whatFailed() const noexcept;
		void resetState() noexcept;
	protected:
		json_state() noexcept;
		void _toggleState(FailBits state) noexcept;
	private:
		FailBits _bits = SUCCESS;
	};

	/**
	 * This class is used to interact with a JSON object in a "safer" way.
	 * A \c nlohmann::json object is given, and then accessed through an instantiation
	 * of this class. Key sequences can be tested, as well as values and their data
	 * types. A variety of methods are provided to allow clients to apply different
	 * types of values to C++ variables and objects.
	 */
	class json : public json_state {
	public:
		typedef std::vector<std::string> KeySequence;

		json(const std::string& name = "json") noexcept;
		json(const nlohmann::json& jobj, const std::string& name = "json") noexcept;

		safe::json& operator=(const nlohmann::json& jobj) noexcept;

		bool keysExist(KeySequence keys, nlohmann::json* ret = nullptr) const noexcept;
		bool equalType(nlohmann::json& dest, nlohmann::json& src) const noexcept;

		std::string synthesiseKeySequence(KeySequence& keys) const noexcept;
		nlohmann::json nlohmannJSON() const noexcept;

		template<typename T>
		void apply(T& dest, KeySequence keys, const T* defval = nullptr, const bool suppressErrors = false) noexcept;
		template<typename T, std::size_t N>
		void applyArray(std::array<T, N>& dest, KeySequence keys) noexcept;
		void applyColour(sf::Color& dest, KeySequence keys, const sf::Color* defval = nullptr, const bool suppressErrors = false) noexcept;
		template<typename T>
		void applyVector(std::vector<T>& dest, KeySequence keys);
	private:
		std::string _getTypeName(nlohmann::json& j) const noexcept;

		nlohmann::json _j;
		global::logger _logger;
	};

	/**
	 * This abstract class is used to read from and write to JSON script files.
	 * This class allows derived classes to be "configured" using a JSON script,
	 * by reading values from the script via the \c safe::json class and applying
	 * them to member fields. In addition to this, derived classes can also save these
	 * values to a JSON script.
	 *
	 * PROBLEM: ideally, there should only be "one" method: one which outlines the
	 * expected format of the JSON script. Then this class can, in some way, either
	 * read or write using this method, instead of the programmer manually typing
	 * out both the read and write methods themselves. This will help to reduce errors
	 * but will end up with the programmer having less power. More analysis should be
	 * conducted to see if this approach is viable.
	 */
	class json_script : public json_state {
	public:
		virtual ~json_script() noexcept;

		std::string getScriptPath() const noexcept;
		std::string jsonwhat() const noexcept;

		void load(const std::string script = "") noexcept;
		void save(const std::string script = "") noexcept;
	private:
		virtual bool _load(safe::json&) noexcept = 0;
		virtual bool _save(nlohmann::json&) noexcept = 0;

		bool _loadFromScript(nlohmann::json& jobj) noexcept;
		bool _saveToScript(nlohmann::json& jobj) noexcept;

		std::string _script = "";
		std::string _what = "";
		global::logger _logger = global::logger("json_script");
	};
}

template<typename T>
void safe::json::apply(T& dest, safe::json::KeySequence keys, const T* defval, const bool suppressErrors) noexcept {
	nlohmann::json test;
	if (keys.empty()) {
		_toggleState(NO_KEYS_GIVEN);
		_logger.error("Attempted to assign a value to a variable without specifying a key sequence.");
	} else {
		if (keysExist(keys, &test)) {
			nlohmann::json testDataType = dest;
			if (equalType(testDataType, test)) {
				dest = test.get<T>();
				return;
			} else {
				_toggleState(MISMATCHING_TYPE);
				_logger.error("Attempted to assign a value of data type \"{}\" to a destination of type \"{}\", in the key sequence {}.", _getTypeName(test), _getTypeName(testDataType), synthesiseKeySequence(keys));
			}
		} else {
			_toggleState(KEYS_DID_NOT_EXIST);
			_logger.error("The key sequence {} does not exist in the JSON object.", synthesiseKeySequence(keys));
		}
	}
	//something went wrong with the assignment, so fall back on the given default value if provided
	if (defval) {
		dest = *defval;
		_logger.write("{} property faulty: reset to the default of {}.", synthesiseKeySequence(keys), *defval);
	}
	if (suppressErrors) resetState();
}

template<typename T, std::size_t N>
void safe::json::applyArray(std::array<T, N>& dest, safe::json::KeySequence keys) noexcept {
	if (!N) return;
	nlohmann::json test;
	if (keys.empty()) {
		_toggleState(NO_KEYS_GIVEN);
		_logger.error("Attempted to assign a value to an array without specifying a key sequence.");
	} else {
		if (keysExist(keys, &test)) {
			if (test.is_array()) {
				if (test.size() == N) {
					nlohmann::json testDataType = dest[0];
					//loop through each element in the JSON array to see if it matches completely in data type
					for (std::size_t i = 0; i < N; i++) {
						//if the JSON array is homogeneous, assign it to the destination array
						if (i == N - 1 && equalType(testDataType, test[i])) {
							for (std::size_t j = 0; j < N; j++) {
								dest[j] = test[j].get<T>();
							}
						} else if (!equalType(testDataType, test[i])) {
							_toggleState(MISMATCHING_ELEMENT_TYPE);
							_logger.error("The specified JSON array was not homogeneous, found an element of data type \"{}\" when attempting to assign to an array of data type \"{}\", in the key sequence {}.", _getTypeName(test[i]), _getTypeName(testDataType), synthesiseKeySequence(keys));
							break;
						}
					}
				} else {
					_toggleState(MISMATCHING_SIZE);
					_logger.error("The size of the JSON array specified ({}) does not match with the size of the provided array ({}), in the key sequence {}.", test.size(), N, synthesiseKeySequence(keys));
				}
			} else {
				_toggleState(MISMATCHING_TYPE);
				_logger.error("Attempted to assign a value of data type \"{}\" to an array, in the key sequence {}.", _getTypeName(test), synthesiseKeySequence(keys));
			}
		} else {
			_toggleState(KEYS_DID_NOT_EXIST);
			_logger.error("The key sequence {} does not exist in the JSON object.", synthesiseKeySequence(keys));
		}
	}
}

template<typename T>
void safe::json::applyVector(std::vector<T>& dest, safe::json::KeySequence keys) {
	if (keys.empty()) {
		_toggleState(NO_KEYS_GIVEN);
		_logger.error("Attempted to assign a value to a vector without specifying a key sequence.");
	} else {
		nlohmann::json test;
		if (keysExist(keys, &test)) {
			if (test.is_array()) {
				nlohmann::json testDataType = T();
				for (std::size_t i = 0; i < test.size(); i++) {
					if (i == test.size() - 1 && equalType(testDataType, test[i])) {
						dest.clear();
						for (std::size_t j = 0; j < test.size(); j++) {
							dest.push_back(test[j].get<T>());
						}
					} else if (!equalType(testDataType, test[i])) {
						_toggleState(MISMATCHING_ELEMENT_TYPE);
						_logger.error("The specified JSON array was not homogeneous, found an element of data type \"{}\" when attempting to assign to a vector of data type \"{}\", in the key sequence {}.", _getTypeName(test[i]), _getTypeName(testDataType), synthesiseKeySequence(keys));
						break;
					}
				}
			} else {
				_toggleState(MISMATCHING_TYPE);
				_logger.error("Attempted to assign a value of data type \"{}\" to a vector, in the key sequence {}.", _getTypeName(test), synthesiseKeySequence(keys));
			}
		} else {
			_toggleState(KEYS_DID_NOT_EXIST);
			_logger.error("The key sequence {} does not exist in the JSON object.", synthesiseKeySequence(keys));
		}
	}
}