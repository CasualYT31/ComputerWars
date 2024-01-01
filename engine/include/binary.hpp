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

/**
 * @file binary.hpp
 * Declares classes that can be used to read and write binary streams.
 */

#pragma once

#include <sstream>
#include "SFML/System/NonCopyable.hpp"
#include "logger.hpp"

namespace engine {
	/**
	 * Used to convert numbers into the little endian format.
	 */
	class convert {
	public:
		/**
		 * Converts a number into little/big endian format.
		 * @tparam T      The type of number to convert.
		 * @param  number The number to convert.
		 * @return The converted number.
		 */
		template<typename T>
		static T number(T number) noexcept;

		/**
		 * \c TRUE if this system is running on big endian, \c FALSE otherwise.
		 */
		static const bool BIG_ENDIAN;
	private:
		/**
		 * Used to detect whether this system is running on big endian.
		 */
		static constexpr int ONE = 1;
	};

	/**
	 * Class used to read a block of binary data.
	 */
	class binary_istream : sf::NonCopyable {
	public:
		/**
		 * Initialises the internal data stream.
		 * In the constructor, the internal data stream is configured to throw an
		 * exception when any of its fail, bad, and eof bits are set.
		 * @param data The data to initialise the logger object with.
		 * @sa    \c engine::logger
		 */
		binary_istream(const engine::logger::data& data);

		/**
		 * Initialises the internal data stream.
		 * In the constructor, the internal data stream is configured to throw an
		 * exception when any of its fail, bad, and eof bits are set.\n
		 * @param file Path to a binary file to load into this input stream.
		 * @param data The data to initialise the logger object with.
		 * @sa    \c engine::logger
		 */
		binary_istream(const std::string& file, const engine::logger::data& data);

		/**
		 * Moves a binary input stream.
		 * @param stream The stream to move.
		 */
		binary_istream(engine::binary_istream&& stream) noexcept;

		/**
		 * Moves a binary input stream.
		 * @param  stream The stream to move into this one.
		 * @return Reference to \c this.
		 */
		engine::binary_istream& operator=(
			engine::binary_istream&& stream) noexcept;

		/**
		 * Reads a number and sets it to the variable given.
		 * @tparam T      The type of number of read.
		 * @param  number If the number was retrieved from the binary file
		 *                successfully, then it will replace the value stored at
		 *                \c number.
		 * @throws If the number could not be read.
		 * @safety If an exception was thrown, this method guarantees that the
		 *         given variable isn't updated.
		 */
		template<typename T>
		void readNumber(T& number);

		/**
		 * Reads a \c bool and sets it to the variable given.
		 * @param  boolean If the bool was retrieved from the binary file
		 *                 successfully, then it will replace the value stored at
		 *                 \c boolean.
		 * @throws If the \c bool could not be read.
		 * @safety If an exception was thrown, this method guarantees that the
		 *         given variable isn't updated.
		 */
		void readBool(bool& boolean);

		/**
		 * Reads a string and sets it to the variable given.
		 * @param  str If the string was retrieved from the binary file
		 *             successfully, then it will replace the value stored at
		 *             \c str.
		 * @throws If the string could not be read.
		 * @safety If an exception was thrown, this method guarantees that the
		 *         given variable isn't updated.
		 */
		void readString(std::string& str);

		/**
		 * Replaces the binary data stored in this object and seeks the get pointer
		 * to the beginning of the new data block.
		 * @param  is   The input stream to read the data from.
		 * @param  data The binary input stream to replace the data of.
		 * @return \c is.
		 */
		friend std::istream& operator>>(std::istream& is,
			engine::binary_istream& data);
	private:
		/**
		 * Checks if a read operation can be performed, and if not, throws an
		 * exception.
		 * @param  size The number of bytes to read.
		 * @throws \c std::istream::failure() if there are not enough bytes left in
		 *         the stream to read.
		 */
		inline void _checkRead(const std::size_t size) const {
			if (_bytes + size > _totalBytes) {
				throw std::istream::failure("buffer size is " +
					std::to_string(_totalBytes));
			}
		}

		/**
		 * The internal logger object.
		 */
		mutable engine::logger _logger;

		/**
		 * The stream used to store the binary data.
		 */
		std::stringstream _stream;

		/**
		 * The number of bytes read from the stream.
		 */
		std::size_t _bytes = 0;

		/**
		 * The total number of bytes available in the stream.
		 */
		std::size_t _totalBytes = 0;
	};

	/**
	 * Class used to write binary data and then read it in one block.
	 */
	class binary_ostream : sf::NonCopyable {
	public:
		/**
		 * Initialises the internal data stream.
		 * In the constructor, the internal data stream is configured to throw an
		 * exception when any of its fail, bad, and eof bits are set.
		 * @param data The data to initialise the logger object with.
		 * @sa    \c engine::logger
		 */
		binary_ostream(const engine::logger::data& data);

		/**
		 * Moves a binary output stream.
		 * @param stream The stream to move.
		 */
		binary_ostream(engine::binary_ostream&& stream) noexcept;

		/**
		 * Moves a binary output stream.
		 * @param  stream The stream to move into this one.
		 * @return Reference to \c this.
		 */
		engine::binary_ostream& operator=(
			engine::binary_ostream&& stream) noexcept;

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

		/**
		 * Reads the written binary data in one block.
		 * @param  os   The output stream to write the data to.
		 * @param  data The binary output stream to read from.
		 * @return \c os.
		 */
		friend std::ostream& operator<<(std::ostream& os,
			engine::binary_ostream& data);

		/**
		 * Replaces the binary data stored in \c to and seeks the get pointer to
		 * the beginning of the new data block.
		 * @param  from The binary output stream to read the data from.
		 * @param  to   The binary input stream to replace the data of.
		 */
		friend void operator>>(engine::binary_ostream& from,
			engine::binary_istream& to);
	private:
		/**
		 * The internal logger object.
		 */
		mutable engine::logger _logger;

		/**
		 * The stream used to store the binary data.
		 */
		std::stringstream _stream;

		/**
		 * The number of bytes written to the stream.
		 */
		std::size_t _bytes = 0;
	};
}

#include "tpp/binary.tpp"
