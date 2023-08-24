/**
 * @file MapFileManagement.as
 * The entry point via which the engine can serialise and deserialise map data.
 */

/**
 * Loads a binary stream and updates a map object based on the stream's contents.
 * The engine will clear the state of the map object before invoking this
 * function.
 * @param  stream Handle to the binary stream to read from.
 * @param  map    Handle to the map object to update.
 * @param  format A number indicating which format the binary stream's data is
 *                expected to take.
 * @throws You are permitted to throw exceptions to signify unrecoverable errors.
 */
void LoadMap(BinaryIStream@ stream, Map@ map, uint8 format) {
	/* `format` can mean a variety of things, not just CWM format versions.
	For example, a range of values could be assigned to CWM, whilst some other
	values could be assigned to original Advance Wars map file formats, allowing
	one to easily add side-by-side support for both. This function would be used
	to define these ranges, and invoke different code accordingly. */
	cwm::LoadMap(stream, map, format);
}

/**
 * Serialises a map object to a binary stream.
 * The given stream will be empty.
 * @param  stream Handle to the binary stream to write to.
 * @param  map    Handle to the map object to update.
 * @param  format A number indicating which format the binary stream's data must
 *                take.
 * @throws You are permitted to throw exceptions to signify unrecoverable errors.
 */
void SaveMap(BinaryOStream@ stream, Map@ map, uint8 format) {
	cwm::SaveMap(stream, map, format);
}
