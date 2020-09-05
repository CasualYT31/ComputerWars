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

/**
 * @file game.h
 * Declares the class used to bring together the \c awe::map and \c awe::army classes.
 */

#pragma once

#include "map.h"
#include "army.h"

// for documentation on the awe namespace, please see bank.h
namespace awe {
	/**
	 * Class that represents a single game of Advance Wars, with a map and its armies.
	 * This class is only responsible for the storage and direct manipulation of the game,
	 * which means game rules are defined in that other class.
	 */
	class game {
	public:
		/**
		 * Current version number representing the file format in use.
		 * The standard implementation uses the four bytes "#CWM",
		 * where # is a 0-255 value representing the version of the standard map file format in use.
		 * \c 1297564416 is the 32-bit integer value representing "[NUL]", "C", "W", and "M". Adding
		 * one to this value will increase the first byte (little endian is used).\n
		 * It is a \b custom format that is in no way based on the map formats of the original game.
		 */
		static const sf::Uint32 VERSION_NUMBER = 1297564416;

		/**
		 * Initialises the internal logger object.
		 * @param name The name to give this particular instantiation within the log file. Defaults to "game."
		 * @sa    \c global::logger
		 */
		game(const std::string& name = "game") noexcept;

		/**
		 * Opens a binary file and reads the contents to overwrite what's currently stored in this object.
		 * This method has strong exception safety.
		 * @param   filename The name of the file to open. If a blank string, \c _filename will be substitued.
		 * @return  \c TRUE if reading was successful, \c FALSE otherwise.
		 */
		bool read(std::string filename = "") noexcept;

		/**
		 * Opens a binary file and replaces its contents with what's stored in this object.
		 * @param  filename The name of the file to open. If a blank string, \c _filename will be substitued.
		 * @return \c TRUE if writing was successful, \c FALSE otherwise.
		 */
		bool write(std::string filename = "") noexcept;

		/**
		 * Allows access to the map object.
		 * @return Pointer to the map object, or an empty pointer if a map hasn't been allocated yet.
		 */
		std::shared_ptr<awe::map> getMap() const noexcept;

		/**
		 * Allows access to a given army object.
		 * @param  i The 0-based index of the army to retrieve.
		 * @return Pointer to the army object, or an empty pointer if \c i is out of range.
		 */
		std::shared_ptr<awe::army> getArmy(std::size_t i) const noexcept;

		/**
		 * Retrieves the number of armies allocated.
		 * @return The number of armies allocated.
		 */
		std::size_t getNumberOfArmies() const noexcept;
	private:
		/**
		 * Initialises a file stream for either input or output.
		 * @param  filename The path of the file to open.
		 * @param  forInput \c TRUE if opening the file for input, \c FALSE for output.
		 * @return Pointer to the opened file.
		 */
		std::shared_ptr<std::fstream> _openFile(std::string& filename, bool forInput);

		/**
		 * Automatically converts from little endian to big endian if required.
		 * @param  file The file to read a number from.
		 * @return The number read from the given file.
		 */
		template<typename T>
		T _readNumber(const std::shared_ptr<std::fstream>& file) const;

		/**
		 * Automatically converts to little endian from big endian if required.
		 * @param file   The file to write the number to.
		 * @param number The number to write.
		 */
		template<typename T>
		void _writeNumber(const std::shared_ptr<std::fstream>& file, T number) const;

		/**
		 * Determines if a conversion should be carried out.
		 * @return \c TRUE if the system is in big endian, \c FALSE if not.
		 */
		bool _isBigEndian() const noexcept;

		/**
		 * Internal logger object.
		 */
		mutable global::logger _logger;

		/**
		 * The name of the map file last opened \em successfully using \c read() or \c write().
		 */
		std::string _filename = "";

		/**
		 * The map.
		 */
		std::shared_ptr<awe::map> _map;

		/**
		 * The armies involved in the game.
		 */
		std::shared_ptr<std::vector<std::shared_ptr<awe::army>>> _armies;
	};
}

template<typename T>
T awe::game::_readNumber(const std::shared_ptr<std::fstream>& file) const {
	T ret = 1;
	if (sizeof(T) == 1 || ret >> 8 * sizeof(ret) - 1 == 1) {
		// if the system running is little endian or number is a single byte, read as normal
		*file >> ret;
	} else {
		// if the system is running on big endian encoding, convert from little endian
		T copy;
		*file >> copy;
		for (std::size_t i = 0; i < sizeof(T); i++) {
			ret << 8;
			ret |= copy & 0xFF;
			copy >> 8;
		}
	}
	return ret;
}

template<typename T>
void awe::game::_writeNumber(const std::shared_ptr<std::fstream>& file, T number) const {
	*file << number;
}