/*Copyright 2019-2022 CasualYouTuber31 <naysar@protonmail.com>

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

#include "typedef.h"

void awe::RegisterGameTypedefs(asIScriptEngine* engine) noexcept {
	if (!engine->GetTypeInfoByName("TeamID")) {
		engine->RegisterTypedef("TeamID", "uint8");
	}
	if (!engine->GetTypeInfoByName("BankID")) {
		engine->RegisterTypedef("BankID", "uint32");
	}
	if (!engine->GetTypeInfoByName("Funds")) {
		engine->RegisterTypedef("Funds", "int32");
	}
	if (!engine->GetTypeInfoByName("HP")) {
		engine->RegisterTypedef("HP", "int32");
	}
	if (!engine->GetTypeInfoByName("Fuel")) {
		engine->RegisterTypedef("Fuel", "int32");
	}
	if (!engine->GetTypeInfoByName("Ammo")) {
		engine->RegisterTypedef("Ammo", "int32");
	}
	if (!engine->GetTypeInfoByName("UnitID")) {
		engine->RegisterTypedef("UnitID", "uint32");
	}
	if (!engine->GetTypeInfoByName("ArmyID")) {
		engine->RegisterTypedef("ArmyID", "uint32");
	}
}