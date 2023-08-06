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
 * Parses a \c Day from a string.
 * @param  str       The string to parse.
 * @param  base      The expected base of the number.
 * @param  byteCount Stores the number of bytes processed.
 * @return The \c Day value.
 */
awe::Day parseDay(const std::string& str, const unsigned int base = 10,
	std::size_t* byteCount = nullptr) {
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
	}
	if (!engine->GetTypeInfoByName("Fuel")) {
		engine->RegisterTypedef("Fuel", "int32");
		document->DocumentExpectedFunction("typedef int32 Fuel",
			"Represents a fuel amount.");
	}
	if (!engine->GetTypeInfoByName("Ammo")) {
		engine->RegisterTypedef("Ammo", "int32");
		document->DocumentExpectedFunction("typedef int32 Ammo",
			"Represents an ammo amount.");
	}
	if (!engine->GetTypeInfoByName("UnitID")) {
		engine->RegisterTypedef("UnitID", "uint32");
		document->DocumentExpectedFunction("typedef uint32 UnitID",
			"Index used to identify a unit.");
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
