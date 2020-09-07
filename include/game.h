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
#include "file.h"

// for documentation on the awe namespace, please see bank.h
namespace awe {
	/**
	 * Class that represents a single game of Advance Wars, with a map and its armies.
	 * This class is only responsible for the storage and direct manipulation of the game,
	 * which means game rules are defined in that other class.
	 */
	class game : sf::NonCopyable {
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
		std::shared_ptr<engine::binary_file> _openFile(std::string& filename, bool forInput);

		/**
		 * Reads a map file in the first version of the CWM format.
		 * @param file The binary file to read from.
		 */
		void _read_CWM_1(const std::shared_ptr<engine::binary_file>& file);

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