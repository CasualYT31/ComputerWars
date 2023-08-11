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

#include "typedef.hpp"

/**
 * Formats an \c HP into a string.
 * @param  hp The \c HP to format.
 * @return The \c HP in string form.
 */
std::string formatHP(const awe::HP hp) {
	return std::to_string(hp);
}

/**
 * Formats a \c Fuel into a string.
 * @param  fuel The \c Fuel to format.
 * @return The \c Fuel in string form.
 */
std::string formatFuel(const awe::Fuel fuel) {
	return std::to_string(fuel);
}

/**
 * Formats an \c Ammo into a string.
 * @param  ammo The \c Ammo to format.
 * @return The \c Ammo in string form.
 */
std::string formatAmmo(const awe::Ammo ammo) {
	return std::to_string(ammo);
}

/**
 * Formats a \c UnitID into a string.
 * @param  unitID The \c UnitID to format.
 * @return The \c UnitID in string form.
 */
std::string formatUnitID(const awe::UnitID unitID) {
	return std::to_string(unitID);
}

/**
 * Formats a \c Day into a string.
 * @param  day The day to format.
 * @return The day in string form.
 */
std::string formatDay(const awe::Day day) {
	return std::to_string(day);
}

/**
 * Formats an \c ArmyID into a string.
 * @param  armyID The \c ArmyID to format.
 * @return The \c ArmyID in string form.
 */
std::string formatArmyID(const awe::ArmyID armyID) {
	return std::to_string(armyID);
}

/**
 * Parses an \c HP from a string.
 * @param  str       The string to parse.
 * @param  base      The expected base of the number.
 * @param  byteCount Stores the number of bytes processed.
 * @return The \c HP value.
 */
awe::HP parseHP(const std::string& str, const unsigned int base = 10,
	std::size_t* byteCount = nullptr) {
	if (str.empty()) return 0;
	return std::stol(str, byteCount, base);
}

/**
 * Parses a \c Fuel from a string.
 * @param  str       The string to parse.
 * @param  base      The expected base of the number.
 * @param  byteCount Stores the number of bytes processed.
 * @return The \c Fuel value.
 */
awe::Fuel parseFuel(const std::string& str, const unsigned int base = 10,
	std::size_t* byteCount = nullptr) {
	if (str.empty()) return 0;
	return std::stol(str, byteCount, base);
}

/**
 * Parses an \c Ammo from a string.
 * @param  str       The string to parse.
 * @param  base      The expected base of the number.
 * @param  byteCount Stores the number of bytes processed.
 * @return The \c Ammo value.
 */
awe::Ammo parseAmmo(const std::string& str, const unsigned int base = 10,
	std::size_t* byteCount = nullptr) {
	if (str.empty()) return 0;
	return std::stol(str, byteCount, base);
}

/**
 * Parses a \c UnitID from a string.
 * @param  str       The string to parse.
 * @param  base      The expected base of the number.
 * @param  byteCount Stores the number of bytes processed.
 * @return The \c UnitID value.
 */
awe::UnitID parseUnitID(const std::string& str, const unsigned int base = 10,
	std::size_t* byteCount = nullptr) {
	if (str.empty()) return 0;
	return std::stoul(str, byteCount, base);
}

/**
 * Parses a \c Day from a string.
 * @param  str       The string to parse.
 * @param  base      The expected base of the number.
 * @param  byteCount Stores the number of bytes processed.
 * @return The \c Day value.
 */
awe::Day parseDay(const std::string& str, const unsigned int base = 10,
	std::size_t* byteCount = nullptr) {
	if (str.empty()) return 0;
	return std::stoul(str, byteCount, base);
}

void awe::RegisterGameTypedefs(asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	if (!engine->GetTypeInfoByName("TeamID")) {
		engine->RegisterTypedef("TeamID", "uint8");
		document->DocumentExpectedFunction("typedef uint8 TeamID",
			"Index used to identify a team.");
	}
	if (!engine->GetTypeInfoByName("Funds")) {
		engine->RegisterTypedef("Funds", "int32");
		document->DocumentExpectedFunction("typedef int32 Funds",
			"Represents a fund amount.");
	}
	if (!engine->GetTypeInfoByName("HP")) {
		engine->RegisterTypedef("HP", "int32");
		document->DocumentExpectedFunction("typedef int32 HP",
			"Represents an HP amount.");

		auto r = engine->RegisterGlobalFunction("string formatHP(const HP)",
			asFUNCTION(formatHP), asCALL_CDECL);
		document->DocumentGlobalFunction(r, "Formats an HP value as a string.");
		r = engine->RegisterGlobalFunction("HP parseHP(const string&in, "
			"const uint = 10, uint64&out = 0)",
			asFUNCTION(parseHP), asCALL_CDECL);
		document->DocumentGlobalFunction(r, "Parses an HP value from a string.");
	}
	if (!engine->GetTypeInfoByName("Fuel")) {
		engine->RegisterTypedef("Fuel", "int32");
		document->DocumentExpectedFunction("typedef int32 Fuel",
			"Represents a fuel amount.");

		auto r = engine->RegisterGlobalFunction("string formatFuel(const Fuel)",
			asFUNCTION(formatFuel), asCALL_CDECL);
		document->DocumentGlobalFunction(r, "Formats a Fuel value as a string.");
		r = engine->RegisterGlobalFunction("Fuel parseFuel(const string&in, "
			"const uint = 10, uint64&out = 0)",
			asFUNCTION(parseFuel), asCALL_CDECL);
		document->DocumentGlobalFunction(r, "Parses a Fuel value from a string.");
	}
	if (!engine->GetTypeInfoByName("Ammo")) {
		engine->RegisterTypedef("Ammo", "int32");
		document->DocumentExpectedFunction("typedef int32 Ammo",
			"Represents an ammo amount.");

		auto r = engine->RegisterGlobalFunction("string formatAmmo(const Ammo)",
			asFUNCTION(formatAmmo), asCALL_CDECL);
		document->DocumentGlobalFunction(r, "Formats an Ammo value as a string.");
		r = engine->RegisterGlobalFunction("Ammo parseAmmo(const string&in, "
			"const uint = 10, uint64&out = 0)",
			asFUNCTION(parseHP), asCALL_CDECL);
		document->DocumentGlobalFunction(r, "Parses an Ammo value from a string.");
	}
	if (!engine->GetTypeInfoByName("UnitID")) {
		engine->RegisterTypedef("UnitID", "uint32");
		document->DocumentExpectedFunction("typedef uint32 UnitID",
			"Index used to identify a unit.");

		auto r = engine->RegisterGlobalFunction(
			"string formatUnitID(const UnitID)",
			asFUNCTION(formatUnitID), asCALL_CDECL);
		document->DocumentGlobalFunction(r, "Formats a UnitID value as a string.");
		r = engine->RegisterGlobalFunction("UnitID parseUnitID(const string&in, "
			"const uint = 10, uint64&out = 0)",
			asFUNCTION(parseUnitID), asCALL_CDECL);
		document->DocumentGlobalFunction(r, "Parses a <tt>UnitID</tt> from a "
			"string.");
	}
	if (!engine->GetTypeInfoByName("ArmyID")) {
		engine->RegisterTypedef("ArmyID", "uint32");
		document->DocumentExpectedFunction("typedef uint32 ArmyID",
			"Index used to identify an army.");

		auto r = engine->RegisterGlobalFunction(
			"string formatArmyID(const ArmyID)",
			asFUNCTION(formatArmyID), asCALL_CDECL);
		document->DocumentGlobalFunction(r, "Formats an <tt>ArmyID</tt> as a "
			"string.");
	}
	if (!engine->GetTypeInfoByName("Day")) {
		engine->RegisterTypedef("Day", "uint32");
		document->DocumentExpectedFunction("typdef uint32 Day",
			"Represents a day number.");

		auto r = engine->RegisterGlobalFunction("string formatDay(const Day)",
			asFUNCTION(formatDay), asCALL_CDECL);
		document->DocumentGlobalFunction(r, "Formats a day number as a string.");
		r = engine->RegisterGlobalFunction("Day parseDay(const string&in, "
			"const uint = 10, uint64&out = 0)",
			asFUNCTION(parseDay), asCALL_CDECL);
		document->DocumentGlobalFunction(r, "Parses a day number from a string.");
	}
}
