# Computer Wars Map File Format

## Introduction

The different specifications for each version of the Computer Wars map
format will make reference to the typedefs defined in the C++ header file
[typedef.h](./include/typedef.h), so knowledge on most of these typedefs is
required to understand them.

All map files in Computer Wars are written in binary, and are read from and
written to using the `engine::binary_file` class. As such, familiarity with
how different data types are handled by that class is required in order to
understand this specification. This class also writes files in Little
Endian format, but in case you're running on a Big Endian machine, the
class will convert the numbers for you so you shouldn't have to be
concerned with this small detail.

This map file format is in no way based on the original Advance Wars map
file format. They may be similar by coincidence but chances are they will
be extremely different. I also do not intend to program support for any AW
map format, so conversion from/to the CW map format will have to be carried
out separately, or could be coded into the `map` class if you so wished to.

Also note that the version I've coded so far (0CWM) will drastically change
in the future as I create new iterations of the format.

All Computer Wars Map files will have the extension `.cwm`. There is a
possibility that I could have `.#cwm` extensions to refer to specific
versions, but the header carries out this job anyway so... eh. I'll live it
here as an idea I might come back to later if I don't forget about it.

## Header

All CW map file formats will begin with some variation of the four byte
sequence `#CWM`, with `#` being a byte starting with the value `0` for the
first version, then `1` for the second version, etc. This header was only
added in case the file format has to undergo serious changes after initial
release in a way that breaks early versions of the format, and so support
for older versions can be retained alongside the newer versions which
would directly support more features.

## 0CWM

This section contains the specification for the initial alpha release
version of the Computer Wars map format. It can't do an awful lot, but that
is to be expected :wink:: all I wanted to support at this stage was basic
visual representation of maps and their units, without much care for the
HUD or unit details.

No separation bytes are defined for this format: what's listed below is
stored one after the other in the binary file. The order of the fields in
the tables below define the order in which they are to be stored. To save
time looking up the typedefs: right now, all fields except for the map name
are 32-bits in size. This will likely be reduced in the future (something
easily carried out by simply changing the definition of the typedefs -
hopefully without breaking code that shouldn't be relying on variables of
these types to be a certain size...).

| Control Flow | Field Name | Field Data Type | Notes |
| ---: | :---: | :---: | :--- |
| | Map Name | `string` | |
| | Map Width | `uint32` | |
| | Map Height | `uint32` | |
| | Army Count | `uint32` | |
| FOR EACH Army | Army Country ID | `awe::ArmyID` | |
| END LOOP | Army Funds | `awe::Funds` | |
| FOR EACH TileRow | | | TileRow = Y co-ordinate, starting at `0`. |
| FOR EACH TileColumn | Tile Type ID | `awe::BankID` | TileColumn = X co-ordinate, starting at `0`. |
| | Tile HP | `awe::HP` | This would be used for capturing and cannon HP and stuff like this. |
| | Tile Owner | `awe::ArmyID` | `0xFFFFFFFF` means no army owns the tile. |
| | Unit Owner | `awe::ArmyID` | The owner of the unit on the tile. `0xFFFFFFFF` means that the tile is vacant (or that there wasn't any loaded units, or there are no more loaded units: see below). |
| IF `UnitOwner != 0xFFFFFFFF` | Unit Type ID | `awe::BankID` | |
| | Unit HP | `awe::HP` | |
| | Unit Fuel | `awe::Fuel` | |
| | Unit Ammo | `awe::Ammo` | |
| END LOOP | First Loaded Unit's Owner | `awe::ArmyID` | At this point, the map loading code calls unit reading instructions recursively for each loaded unit, firstly calling the conditional statement four rows above. Execution continues as normal after the recursive call has been carried out. |
| END LOOP | | | |