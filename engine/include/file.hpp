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

/**
 * @file file.hpp
 * Declares a class that can be used to read and write binary files.
 */

#pragma once

#include <string>
#include <fstream>
#include <type_traits>
#include "sfml/System/NonCopyable.hpp"
#include "logger.hpp"

namespace engine {
	/**
	 * Represents a binary file written in little endian encoding.
	 * This class can be used to ensure that binary files are written in a
	 * consistent encoding (which is little endian), regardless of the byte
	 * ordering the executing system uses internally.
	 */
	class binary_file : sf::NonCopyable {
	public:
		/**
		 * Initialises the internal file stream.
		 * In the constructor, the internal file stream is configured to throw an
		 * exception when any of its fail, bad, and eof bits are set.
		 * @param data The data to initialise the logger object with.
		 * @sa    \c engine::logger
		 */
		binary_file(const engine::logger::data& data);

		/**
		 * Determines if the system is running on big endian byte ordering.
		 * @return \c TRUE if the system is in big endian, \c FALSE if not.
		 */
		static bool isBigEndian() noexcept;

		/**
		 * Converts a number between little and big endian encoding.
		 * @warning The given value must be an arithmetic value!
		 * @tparam  T      The type of integer or floating point value to convert.
		 * @param   number The number to convert.
		 * @return  The converted number.
		 */
		template<typename T>
		static T convertNumber(T number) noexcept;

		/**
		 * Opens a given file for either input or output.
		 * This method also automatically closes the previously opened file, if
		 * any. \c _bytes is only reset to \c 0 if opening the file was successful.
		 * @param  filepath The path of the file to open.
		 * @param  forInput \c TRUE if the file is to be open for input, \c FALSE
		 *                  if for output.
		 * @throws If the file couldn't be opened, or if the old file couldn't be
		 *         closed. See \c fstream::close() and \c fstream::open().
		 * @safety The internal file stream is left in a valid state if an
		 *         exception is thrown.
		 */
		void open(const std::string& filepath, const bool forInput);

		/**
		 * Closes the currently open file, if any is open.
		 * @throws If the file couldn't be closed. See \c fstream::close().
		 * @safety The internal file stream is left in a valid state if an
		 *         exception is thrown.
		 */
		void close();

		/**
		 * Returns the current byte position of the file.
		 * Read and write methods in this class count the number of bytes they read
		 * or write in a file.
		 * @return The number of bytes read/written since the beginning of the
		 *         file.
		 * @sa     engine::binary_file::open()
		 */
		sf::Uint64 position() const noexcept;

		/**
		 * Reads a number from the binary file.
		 * @tparam The type of arithmetic value to read.
		 * @return The number retrieved from the binary file, in the correct
		 *         format.
		 * @throws If the number could not be read.
		 * @safety If the read operation failed, \c _bytes is not incremented, and
		 *         the stream is left in a valid state.
		 */
		template<typename T>
		T readNumber();

		/**
		 * Version of \c readNumber() that updates a given variable with the read
		 * value.
		 * These versions of the read methods were added to prevent explicit
		 * references to types. This is particularly useful for typedefs.
		 * @param  number If the number was retrieved from the binary file
		 *                successfully, then it will replace the value stored at
		 *                \c number.
		 * @safety If an exception was thrown, this method guarantees that the
		 *         given variable isn't updated.
		 * @sa     \c readNumber()
		 */
		template<typename T>
		void readNumber(T& number);

		/**
		 * Reads a bool value from the binary file.
		 * This class reads and writes bool values as single bytes. \c FALSE is
		 * represented by a value of \c 0, whereas \c TRUE is a value of \c !0,
		 * with \c 0xFF being the value that is written by the \c writeBool()
		 * method.
		 * @return The boolean value retrieved from the binary file.
		 * @throws If the bool could not be read.
		 * @safety If the read operation failed, \c _bytes is not incremented, and
		 *         the stream is left in a valid state.
		 */
		bool readBool();

		/**
		 * Version of \c readBool() that updates a given variable with the read
		 * value.
		 * @param  boolean If the bool was retrieved from the binary file
		 *                 successfully, then it will replace the value stored at
		 *                 \c boolean.
		 * @safety If an exception was thrown, this method guarantees that the
		 *         given variable isn't updated.
		 * @sa     \c readBool()
		 */
		void readBool(bool& boolean);

		/**
		 * Reads a string from the binary file.
		 * This class reads and writes strings as a list of bytes prepended by the
		 * length of the string, which is stored as an unsigned 32-bit integer.
		 * @return The string retrieved from the binary file.
		 * @throws If the string could not be read.
		 * @safety If the read operation failed, the stream is left in a valid
		 *         state. \c _bytes will have been incremented by the number of
		 *         bytes that were successfully read (which will not have been all
		 *         of them).
		 */
		std::string readString();

		/**
		 * Version of \c readString() that updates a given variable with the read
		 * value.
		 * If an exception was thrown, this method guarantees that the given
		 * variable isn't updated.
		 * @param  str If the string was retrieved from the binary file
		 *             successfully, then it will replace the value stored at
		 *             \c str.
		 * @throws If the string could not be read.
		 * @safety If an exception was thrown, this method guarantees that the
		 *         given variable isn't updated.
		 * @sa     \c readString()
		 */
		void readString(std::string& str);

		/**
		 * Writes a number value to the binary file.
		 * @tparam The type of arithmetic value to write.
		 * @param  number The arithmetic value to write, converted to little endian
		 *                if required.
		 * @throws If the number could not be written.
		 * @safety If the write operation failed, \c _bytes is not incremented, and
		 *         the stream is left in a valid state.
		 */
		template<typename T>
		void writeNumber(T number);

		/**
		 * Writes a bool value to the binary file.
		 * @param  val The bool value to write.
		 * @throws If the bool could not be written.
		 * @safety If the write operation failed, \c _bytes is not incremented, and
		 *         the stream is left in a valid state.
		 * @sa     \c readBool()
		 */
		void writeBool(const bool val);

		/**
		 * Writes a string to the binary file.
		 * @param  str The string to write.
		 * @throws If the string could not be written.
		 * @safety If the write operation failed, the stream is left in a valid
		 *         state. \c _bytes will have been incremented by the number of
		 *         bytes that were successfully written (which will not have been
		 *         all of them).
		 * @sa     \c readString()
		 */
		void writeString(const std::string& str);
	private:
		/**
		 * The internal logger object.
		 */
		mutable engine::logger _logger;

		/**
		 * The internal file stream.
		 */
		std::fstream _file;

		/**
		 * Counts the number of bytes read and/or written since the last call to
		 * \c open().
		 */
		sf::Uint64 _bytes = 0;
	};
}

#include "tpp/file.tpp"
