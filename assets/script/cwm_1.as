/* Map loading and saving code
This is just a way for me to experiment and find out how I'd like this
code to be written like, before I try implementing this all in
AngelScript in C++. I can't imagine it will be easy....
*/

// ideally, a reference to the entire map object should be provided
// there will be certain methods that shouldn't be ported over...
// such as save()
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
	
	// let's write some pseudocode based on the CWM format in game_OLD
	// map
	map.setMapName(readString());
	uint32 w = readUint32();
	uint32 h = readUint32();
	// program a non-vector version to avoid extra work
	map.setMapSize(w, h);
	// armies
	uint8 armyCount = readUint8();
	for (uint16 aCounter = 0; aCount < armyCounter; aCounter++) {
		uint32 current = readUint32(); // country ID
		// private version which accepts country ID,
		// avoiding having to include bank stuff in the script:
		map.createArmy(current);
		map.setArmyTeam(current, readUint8());
		map.setArmyFunds(current, readInt32();
		// forgot to implement Team and COs in C++!
	}
	// tiles (and each of their units)
	for (uint32 y = 0; y < h; y++) {
		for (uint32 x = 0; x < w; x++) {
			// see setMapSize() and createArmy above
			map.setTileType(x, y, readUint32());
			map.setTileHP(x, y, readInt32());
			map.setTileOwner(x, y, readUint32());
			
		}
	}
	
	// tbh, it would be better if we could somehow remove sizes of
	// integers from script... like this:
	readMapName();
	// only issue are the strings... strings will have a different
	// format for sure in legit AW binaries...
	// I suppose modders will have to rewrite the file class in C++ if
	// that's the case...
	readMapSize();
	// other issue: data will be different sizes in legit AW binaries
	// again C++ would have to change anyway...
	// so maybe we can't do it like this after all
	
	// why am I even fretting over legit AW map files?
	// I was considering using scripts to read files to allow for
	// modders to program support for legit AW map binaries,
	// but they will be very different in almost every way
	// for example, AW map files won't even refer to unit, tile, or
	// army types in the same way,
	// requiring a lot of work in-script to convert shit
	// it would probably be less work all-round to write a completely
	// separate program that converts AW maps into CW maps...
	
	// okay, I've made up my mind: write load and save code in C++
}

// ideally, a const reference to the entire map object should be
// provided
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