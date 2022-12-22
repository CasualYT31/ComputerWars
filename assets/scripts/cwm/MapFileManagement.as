/**
 * @file MapFileManagement.as
 * The entry point via which the engine can serialise and deserialise map data.
 */

/**
 * Loads a binary file and updates a map object based on the file's contents.
 * The engine will clear the state of the map object before invoking this
 * function. You will not be able to perform write operations.\n
 * You are permitted to throw exceptions to signify unrecoverable errors.
 * @param file   Handle to the binary file to read from.
 * @param map    Handle to the map object to update.
 * @param format A number indicating which format the binary file's data is
 *               expected to take.
 */
void LoadMap(BinaryFile@ file, Map@ map, uint8 format) {
	/* `format` can mean a variety of things, not just CWM format versions.
	For example, a range of values could be assigned to CWM, whilst some other
	values could be assigned to original Advance Wars map file formats, allowing
	one to easily add side-by-side support for both. This function would be used
	to define these ranges, and invoke different code accordingly. */
	cwm::LoadMap(file, map, format);
}

/**
 * Serialises a map object to a binary file.
 * The given file will be empty. You will not be able to perform read
 * operations.\n
 * You are permitted to throw exceptions to signify unrecoverable errors.
 * @param file   Handle to the binary file to write to.
 * @param map    Handle to the map object to update.
 * @param format A number indicating which format the binary file's data must
 *               take.
 */
void SaveMap(BinaryFile@ file, Map@ map, uint8 format) {
	cwm::SaveMap(file, map, format);
}
