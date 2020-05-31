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
	 * @todo Object composition should be considered instead, especially for \c json_script: this prevents classes beyond the \c safe namespace to fiddle with the bit sequence in any way.
	 *
	 * @todo An alternative to error bits should be considered: exceptions could be appropriate here.
	 */
	class json_state {
	public:
		/**
		 * Polymorphic base classes should have virtual destructors.
		 */
		virtual ~json_state() noexcept;

		/**
		 * Typedef representing a set of error bits stored within a \c json_state object.
		 */
		typedef unsigned short FailBits;

		static const FailBits                  SUCCESS = 0b0000000000000000; /**< Error code representing success. That is, no bits are filled. */
		static const FailBits      JSON_WAS_NOT_OBJECT = 0b0000000000000001; /**< Error bit signifying that the root JSON value was not an object. */
		static const FailBits       KEYS_DID_NOT_EXIST = 0b0000000000000010; /**< Error bit signifying that a set of keys did not exist within the JSON object. */
		static const FailBits         MISMATCHING_TYPE = 0b0000000000000100; /**< Error bit signifying that the type of a value was not as expected. */
		static const FailBits            NO_KEYS_GIVEN = 0b0000000000001000; /**< Error bit signifying that no key sequence was provided when it was expected. */
		static const FailBits       FAILED_LOAD_METHOD = 0b0000000000010000; /**< Error bit signifying that a subclass' \c _load() method failed. */
		static const FailBits       FAILED_SAVE_METHOD = 0b0000000000100000; /**< Error bit signifying that a subclass' \c _save() method failed. */
		static const FailBits       FAILED_SCRIPT_LOAD = 0b0000000001000000; /**< Error bit signifying that loading the JSON script failed. */
		static const FailBits       FAILED_SCRIPT_SAVE = 0b0000000010000000; /**< Error bit signifying that saving the JSON script failed. */
		static const FailBits               UNPARSABLE = 0b0000000100000000; /**< Error bit signifying that parsing the JSON script (while loading it) failed. */
		static const FailBits         MISMATCHING_SIZE = 0b0000001000000000; /**< Error bit signifying that an array value in the JSON object was not of the expected size. */
		static const FailBits MISMATCHING_ELEMENT_TYPE = 0b0000010000000000; /**< Error bit signifying that an array value's elements in the JSON object were not homogenous. */
		
		/**
		 * Tests the state of the \c json_state object.
		 * If the internal bit sequence is equal to \c SUCCESS, then the \c json_state object is said to be in a "good" state.
		 * If any bit within the bit sequence is on, then the object is said to be in a "bad" state: this should be addressed by the client and reset using \c resetState().
		 * @return \c TRUE if the object is in a good state, \c FALSE if not.
		 */
		bool inGoodState() const noexcept;

		/**
		 * Returns the internal bit sequence which can be tested against.
		 * The return value of this method can be tested against using the \c FailBits constants in this class.
		 * For example one could perform the check <tt>whatFailed() & UNPARSABLE</tt> to determine if the error was due to a wrongly formatted JSON script.
		 * @return The bit sequence to test against using the bitwise-AND operator.
		 */
		FailBits whatFailed() const noexcept;

		/**
		 * Resets the state of the object.
		 * The internal bit sequence representing the state of the object is set to \c SUCCESS.
		 */
		void resetState() noexcept;
	protected:
		/**
		 * This class cannot be instantiated by the client.
		 */
		json_state() noexcept;

		/**
		 * Sets the error state of the object.
		 * This method is used to set an error bit in the internal bit sequence. A single fail bit from the ones stored in this class should be passed.
		 * The fail bit is assigned via a bitwise-OR operation.
		 * @param state The fail bit to set to the internal bit sequence.
		 */
		void _toggleState(FailBits state) noexcept;
	private:
		/**
		 * The internal bit sequence.
		 * Initialised to \c SUCCESS.
		 */
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
		/**
		 * Typedef representing a key sequence.
		 * A key sequence is a list of strings, each containing a key which should exist in the JSON object.
		 * The first key in the list should exist in the root object. The second key should exist within the object referenced by the first key, and so.
		 * This means the following: the last key can point to any type of value, but all the keys before it must point to object values.
		 * Since this typedef uses the \c vector STL container, initialiser lists can be used for parameters of type \c KeySequence.
		 * @todo Perhaps this should be made into its own class if I'm confident enough to implement support for initialiser lists.
		 */
		typedef std::vector<std::string> KeySequence;

		/**
		 * Constructs an empty JSON object.
		 * @param name The string name of the internal logger object which helps identify this object if it causes an error.
		 */
		json(const std::string& name = "json") noexcept;

		/**
		 * Constructs a JSON object from a \c nlohmann one.
		 * The assignment operator is called in order to achieve this.
		 * @param jobj The \c nlohmann::json object to store within this object - this is referred to as the JSON object.
		 * @param name The string name of the internal logger object which helps identify this object if it causes an error.
		 * @sa    json.operator=()
		 */
		json(const nlohmann::json& jobj, const std::string& name = "json") noexcept;

		/**
		 * Assignment operator which accepts a \c nlohmann::json object.
		 * This operator replaces the JSON object stored within this object if it is called. All JSON objects given must have a root object:
		 * they cannot just contain a number, or a string, or an array, etc.
		 * @warning If the given JSON object did not have a root object, the \c JSON_WAS_NOT_OBJECT bit will be set.
		 * @param   jobj The \c nlohmann::json object to store within this object - this is referred to as the JSON object.
		 * @return  A reference to this object.
		 */
		safe::json& operator=(const nlohmann::json& jobj) noexcept;

		/**
		 * Determines if a specified value within the JSON object exists.
		 * A key sequence must be given which pinpoints the value within the JSON object's object hierarchy.
		 * @param  keys The key sequence which is to be tested.
		 * @param  ret  A pointer to a \c nlohmann::json object which stores the JSON, but only if it exists. This method will only change the pointer value if it succeeds.
		 * @return \c TRUE if the key sequence existed (succeeded), \c FALSE if at least one expected key did not exist.
		 * @sa     json.KeySequence
		 */
		bool keysExist(KeySequence keys, nlohmann::json* ret = nullptr) const noexcept;

		/**
		 * Determines if two \c nlohmann::json objects contain a value with the same or compatible data types.
		 * The test is performed on a source-destination basis: if the source could be safely assigned to the destination, then they are of compatible types.
		 * It is to be emphasised that \b no actual value transfer occurs between source and destination objects.
		 * 
		 * Here are the cases which cause this method to return \c TRUE:
		 * <ol>
		 * <li>If \c type() returns the same for both \c nlohmann::json objects.</li>
		 * <li>If \c source is unsigned and within the limits of the <tt>signed int</tt> data type, and the \c destination is signed.</li>
		 * <li>If \c source is signed or unsigned, and \c destination is floating point.</li>
		 * <li>If \c source is a floating point with a fraction of \c 0, and \c destination is signed or unsigned.</li>
		 * </ol>
		 * @warning The case of \c source signed, \c destination unsigned isn't necessary as the underlying JSON library only parses integers as signed if they are negative.
		 *          This case mustn't be forgotten if another JSON library is used and this behaviour doesn't occur!
		 * @todo    This should be made into a static method.
		 * @param   dest The "destination" JSON value.
		 * @param   src  The "source" JSON value.
		 * @return  \c TRUE if the two JSON values are of compatible data types, \c FALSE if not.
		 */
		bool equalType(nlohmann::json& dest, nlohmann::json& src) const noexcept;

		/**
		 * Converts a key sequence into a single string.
		 * This method loops through each key in a key sequence and generates one long string which lists all of them in this format:
		 * <tt>{"key1", "key2", "keyEtc"}</tt>. This is helpful for debugging/logging purposes.
		 * @todo   This should be made into a static method.
		 * @param  keys The key sequence to convert.
		 * @return The string containing all the keys in the sequence.
		 */
		std::string synthesiseKeySequence(KeySequence& keys) const noexcept;

		/**
		 * Returns the \c nlohmann::json object stored in this object.
		 * This is helpful when all the keys in an object value need to be iterated through.
		 * Otherwise, this should be avoided as no error checking occurs with the use of this method.
		 * The \c apply() template methods should be used whenever possible.
		 * @return The root JSON object stored in this object.
		 * @sa     json.apply()
		 */
		nlohmann::json nlohmannJSON() const noexcept;

		/**
		 * Applies a value found within the JSON object to a given C++ object.
		 * This method automatically checks for the existance of keys, that data types match etc. and reports errors via the internal logging object if these checks fail.
		 * The \c suppressErrors flag does not disable these checks: rather it automatically resets the error state of this object by the end of the call.
		 * This is helpful when a non-critical JSON value is faulty. If this JSON value is indeed non-critical, then a fallback or default value must be specified
		 * to ensure that the program executes as smoothly as possible after this method is called. This is done by providing a pointer to the value to set to the destination
		 * object in case of an error. The destination object is usually initialised the default value explicitly by the client, and a pointer to the object is provided,
		 * though this is not always the case. If no default value is given, and an error occurred, the destination object will remain unamended.
		 * 
		 * It should be noted that only simple types are supported with this method. Array and object values are not supported. However, if the destination object can be
		 * reliably assigned signed and unsigned values, or strings, or floating point values, or boolean values, then this method can be used. Primative data types have been
		 * tested extensively and they work, as well as \c std::string objects. If you wish to utilise your own classes with this method, be sure to test that they work as expected.
		 * They must be assignable to \c nlohmann::json objects.
		 * @warning The \c NO_KEYS_GIVEN bit will be set if an empty key sequence was given.
		 * @warning The \c KEYS_DID_NOT_EXIST bit will be set if the key sequence given did not exist in the JSON object.
		 * @warning The \c MISMATCHING_TYPE bit will be set if the value pointed to by the key sequence contained a value of an incompatible type to the destination object.
		 * @todo    Is the default value system really required? Find a case where this parameter \e isn't just a pointer to destination...
		 * @tparam  T The type of the destination object. This parameter is inferred so it does not usually need to be explicitly provided.
		 * @param   dest          The destination object.
		 * @param   keys          The key sequence uniquely identifying the JSON value to apply to the C++ object.
		 * @param   defval        Points to a C++ object containing the value to assign to \c dest in case an error occurs. If \c NULL, no default value is considered.
		 * @param   supressErrors If \c TRUE, the error state of this object will be reset at the end of the call.
		 * @sa      json.applyArray()
		 * @sa      json.applyColour()
		 * @sa      json.applyVector()
		 */
		template<typename T>
		void apply(T& dest, KeySequence keys, const T* defval = nullptr, const bool suppressErrors = false) noexcept;

		/**
		 * Applies a JSON array of homogenous values to a given \c std::array object.
		 * All the checks peformed in \c apply() are performed in this method too, along with a few array-specific checks, such as a size check and homogenous value checks.
		 * 
		 * Array elements must be of a type that can easily interact with \c nlohmann::json. Please see the second paragraph of \c apply() for more details.
		 * 
		 * The C++ array will only be changed if the method will be successful, i.e. all checks are satisfied.
		 * @warning This method assumes that \c N is more than 0!
		 * @warning The \c NO_KEYS_GIVEN bit will be set if an empty key sequence was given.
		 * @warning The \c KEYS_DID_NOT_EXIST bit will be set if the key sequence given did not exist in the JSON object.
		 * @warning The \c MISMATCHING_TYPE bit will be set if the value pointed to by the key sequence did not contain an array.
		 * @warning The \c MISMATCHING_SIZE bit will be set if the JSON array did not have the same number of elements as the C++ array.
		 * @warning The \c MISMATCHING_ELEMENT_TYPE bit will be set if a value within the JSON array was not of the correct data type (dictated by \c T).
		 * @tparam  T The type of elements within the \c std::array object. Inferred from \c dest.
		 * @tparam  N The size of the \c std::array object. Inferred from \c dest.
		 * @param   dest The destination array object.
		 * @param   keys The key sequence uniquely identifying the JSON array value to apply to the C++ array object.
		 * @sa      json.apply()
		 * @sa      json.applyColour()
		 * @sa      json.applyVector()
		 */
		template<typename T, std::size_t N>
		void applyArray(std::array<T, N>& dest, KeySequence keys) noexcept;
		
		/**
		 * Applies a JSON array of a specific format to an \c sf::Color object.
		 * This method can be used to easily read RGBA colour values from JSON scripts and store them in SFML \c Color objects. The required format is as follows:
		 * <tt>"key": [RED, GREEN, BLUE, ALPHA]</tt>, where each variable is an integer between \c 0 and \c 255.
		 * 
		 * Please see json.apply() for a more in-depth explaination of the parameters of this method.
		 * @warning The json.applyArray() method is used internally: please see the documentation for this method to see the error bits that can be set and their meanings.
		 * @param   dest           The destination \c Color object.
		 * @param   keys           The key sequence uniquely identifying the JSON array to apply to the \c Color object.
		 * @param   defval         A pointer to the default colour value to fall back on in case of an error.
		 * @param   suppressErrors Resets the error state of this object when the call is finished.
		 * @sa      json.apply()
		 * @sa      json.applyArray()
		 * @sa      json.applyVector()
		 */
		void applyColour(sf::Color& dest, KeySequence keys, const sf::Color* defval = nullptr, const bool suppressErrors = false) noexcept;
		
		/**
		 * Applies a JSON array of a variable size to a given \c std::vector object.
		 * This method is similar to applyArray(), except no size checking occurs.
		 * If the call will be successful, the vector is cleared and then filled with the entire contents of the JSON array.
		 * Just like with the applyArray() call, the JSON array must be homogenous.
		 * @warning Please see the json.applyArray() method for information on the error bits that might be set for this method: all except the MISMATCHING_SIZE bit can be set by this method.
		 * @tparam  T The type of elements that are stored within the vector object. Inferred from \c dest.
		 * @param   dest The destination vector object.
		 * @param   keys The key sequence uniquely identifying the JSON array to apply.
		 * @sa      json.apply()
		 * @sa      json.applyArray()
		 * @sa      json.applyColour()
		 */
		template<typename T>
		void applyVector(std::vector<T>& dest, KeySequence keys);
	private:
		/**
		 * Returns the data type of the value stored in a given \c nlohmann::json object as a string.
		 * Internally, \c nlohmann::json's \c type_name() method is used. This private method only exists to return "float" in case the JSON number is indeed a float:
		 * this method does not distinguish between different types of numbers.
		 * @todo   This method should be made static.
		 * @param  j The \c nlohmann::json object containing the JSON value to evaluate.
		 * @return The string name of the JSON data type as defined by the nlohmann::json class (unless where specified in the detailed section).
		 */
		std::string _getTypeName(nlohmann::json& j) const noexcept;

		/**
		 * The \c nlohmann::json object stored internally.
		 */
		nlohmann::json _j;

		/**
		 * The internal logger object.
		 */
		global::logger _logger;
	};

	/**
	 * This abstract class is used to read from and write to JSON script files.
	 * This class allows derived classes to be "configured" using a JSON script,
	 * by reading values from the script via the \c safe::json class and applying
	 * them to member fields. In addition to this, derived classes can also save these
	 * values to a JSON script.
	 * @todo Ideally, there should only be one "method" which outlines the
	 * expected format of the JSON script. Then this class can, in some way, either
	 * read or write using this method, instead of the programmer manually typing
	 * out both the read and write methods themselves. This will help to reduce errors
	 * but will end up with the programmer having less power. More analysis should be
	 * conducted to see if this approach is viable.
	 */
	class json_script : public json_state {
	public:
		/**
		 * Polymorphic base classes should have virtual destructors.
		 */
		virtual ~json_script() noexcept;

		/**
		 * Retrieves a copy of the path of the last opened script file.
		 * The path returned from this method is set indirectly via calls to the load() and save() methods.
		 * @return The path last set via load() or save(). It will store it exactly as it was given.
		 */
		std::string getScriptPath() const noexcept;
		
		/**
		 * Retrieves the last error text caught upon a failure to read or write a file.
		 * This method only returns a non-blank string if the input or output stream operators failed:
		 * no error string will be stored in case the file could not be opened.
		 * @return A copy of the error text. Blank if no error has occurred yet.
		 */
		std::string jsonwhat() const noexcept;

		/**
		 * Loads a JSON script.
		 * This method performs the initialisation steps necessary to setup the safe::json object before it is passed on to the derived _load() method.
		 * It loads the script file, parses it, and initialises the safe::json object. It outputs to the logger when loading commenses and ends.
		 * @warning The \c FAILED_LOAD_METHOD bit will be set if the derived _load() method returned \c FALSE.
		 * @warning Please see _loadFromScript() for more error bits.
		 * @param   script The path of the script file to load. If a blank string, the last opened script will be loaded.
		 */
		void load(const std::string script = "") noexcept;
		
		/**
		 * Saves a JSON script.
		 * This method performs the initialisation steps necessary to setup the nlohmann::json object before it is passed on to the derived _save() method.
		 * It allows _save() to initialise the nlohmann::json object before attempting to write the JSON script to disc. It outputs to the logger when saving commenses and ends.
		 * 
		 * Files will be \b overwritten if they exist.
		 * @warning The \c FAILED_SAVE_METHOD bit will be set if the derived _save() method returned \c FALSE.
		 * @warning Please see _saveToScript() for more error bits.
		 * @param   script The path of the script file to load. If a blank string, the last opened script will be written to.
		 */
		void save(const std::string script = "") noexcept;
	private:
		/**
		 * The method which derived classes use to configure themselves.
		 * This method must be implemented by derived classes. It accepts a safe::json object, which is used to set the derived class' data fields as necessary.
		 * The structure of the JSON script (besides the requirement for a root object) is entirely up to the derived class.
		 * It should, however, be consistent with the one implemented in the corresponding _save() method.
		 * The apply methods within the safe::json object are recommended for use over accessing the underlying JSON object directly.
		 * 
		 * The return value should usually be \c TRUE: only under fatal circumstances should \c FALSE be returned.
		 * Derived classes should try their best to provide default values for all of their fields where possible:
		 * it should not trust that the required values are stored in the JSON script.
		 * @return \c TRUE if the method succeeded with minimal errors, or \c FALSE if a fatal error occurred.
		 */
		virtual bool _load(safe::json&) noexcept = 0;
		
		/**
		 * The method which derived classes use to store their configurations.
		 * This method must be implemented by derived classes. It accepts a nlohmann::json object, which can be populated with the derived class' data fields as necessary.
		 * The structure of the resulting JSON object (besides the requirement for a root object) is entirely up to the derived class.
		 * It should, however, be consistent with the one implmented in the corresponding _load() method.
		 * 
		 * The return value should usually be \c TRUE: only under very rare circumstances should \c FALSE ever be returned.
		 * @todo   Ensure that both loading and saving enforce the root object requirement!
		 * @return \c TRUE if the method succeeded, or \c FALSE if a serious/unrecoverable error occurred.
		 */
		virtual bool _save(nlohmann::json&) noexcept = 0;

		/**
		 * This method loads a JSON script file and stores it in the given JSON object.
		 * \c _script stores the path to use.
		 * @warning The \c FAILED_SCRIPT_LOAD bit will be set if the JSON script file couldn't be opened.
		 * @warning The \c UNPARSABLE bit will be set if the JSON contained in the script file was invalid. jsonwhat() retrieves more information on the error.
		 * @param   jobj The nlohmann::json object to load the script into.
		 * @return  \c TRUE if loading was successful, \c FALSE if not.
		 */
		bool _loadFromScript(nlohmann::json& jobj) noexcept;
		
		/**
		 * This method saves a given JSON object to a JSON script file.
		 * \c _script stores the path to use.
		 * @warning The \c FAILED_SCRIPT_SAVE bit will be set if the JSON script file couldn't be opened or written to.
		 *          jsonwhat() retrieves more information if opening succeeded but writing did not.
		 * @param   jobj The nlohmann::json object to save to the script.
		 * @return  \c TRUE if saving was successful, \c FALSE if not.
		 */
		bool _saveToScript(nlohmann::json& jobj) noexcept;

		/**
		 * A string storing the path of the last written to or read from script file.
		 */
		std::string _script = "";
		
		/**
		 * A string storing the exception what() string caught upon a failure in the _loadFromScript() or _saveToScript() methods.
		 */
		std::string _what = "";
		
		/**
		 * The internal logger object.
		 */
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