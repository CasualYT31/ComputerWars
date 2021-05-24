/* Map loading and saving code
This is just a way for me to experiment and find out how I'd like this
code to be written like, before I try implementing this all in
AngelScript in C++. I can't imagine it will be easy....
*/

// ideally, a reference to the entire map object should be provided
// there will be certain methods that shouldn't be ported over... such as save()
void load_cwm_1(awe::Map map) {
	// functions for reading from binary files
	// if these functions throw, then the call will be aborted
	val = readInt8();
	val = readUint8();
	val = readInt16();
	val = readUint16();
	val = readInt32();
	val = readUint32();
	val = readInt64();
	val = readUint64();
	val = readBool();
	val = readString();
	// all map methods e.g...
	map.setMapName(readString());
	// operator[] from bank classes
	
	// let's write some pseudocode based on the CWM format in game_OLD
	// map
	map.setMapName(readString());
	uint32 w = readUint32();
	uint32 h = readUint32();
	map.setMapSize(w, h); // program a non-vector version to avoid extra work
	// armies
	uint8 armyCount = readUint8();
	for (uint16 aCounter = 0; aCount < armyCounter; aCounter++) {
		uint32 current = readUint32(); // country ID
		map.createArmy(current); // private version which accepts country ID,
								 // avoiding having to include bank stuff in the script
		map.setArmyTeam(current, readUint8());
		map.setArmyFunds(current, readInt32();
		// forgot to implement Team and COs in C++!
	}
	// tiles (and each of their units)
	for (uint32 y = 0; y < h; y++) {
		for (uint32 x = 0; x < w; x++) {
			map.setTileType(x, y, readUint32()); // see setMapSize() and createArmy above
			map.setTileHP(x, y, readInt32());
			map.setTileOwner(x, y, readUint32());
			
		}
	}
	
	// tbh, it would be better if we could somehow remove sizes of integers from script...
}

// ideally, a const reference to the entire map object should be provided
void save_cwm_1() {
	// functions for writing to binary files
	writeInt8(val);
	writeUint8(val);
	writeInt16(val);
	writeUint16(val);
	writeInt32(val);
	writeUint32(val);
	writeInt64(val);
	writeUint64(val);
	writeBool(val);
	writeString(val);
}