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

#include "options.hpp"

std::shared_ptr<engine::scripts> awe::game_options::_scripts = nullptr;

void awe::game_options::registerGameOptionsType(asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document,
	const std::shared_ptr<engine::scripts>& scripts) noexcept {
	awe::game_options::_scripts = scripts;
	auto r = engine->RegisterObjectType("GameOptions", 0, asOBJ_REF);
	document->DocumentObjectType(r, "Holds options that the game engine will "
		"require when it creates a new game on a map.");
	r = engine->RegisterObjectBehaviour("GameOptions", asBEHAVE_FACTORY,
		"GameOptions@ f()", asFUNCTION(awe::game_options::factory), asCALL_CDECL);
	r = engine->RegisterObjectBehaviour("GameOptions", asBEHAVE_ADDREF,
		"void f()", asMETHOD(awe::game_options, addRef), asCALL_THISCALL);
	r = engine->RegisterObjectBehaviour("GameOptions", asBEHAVE_RELEASE,
		"void f()", asMETHOD(awe::game_options, releaseRef), asCALL_THISCALL);
	r = engine->RegisterObjectProperty("GameOptions", "array<int>@ currentCOs",
		asOFFSET(awe::game_options, currentCOs));
	r = engine->RegisterObjectProperty("GameOptions", "array<int>@ tagCOs",
		asOFFSET(awe::game_options, tagCOs));
}

awe::game_options* awe::game_options::factory() noexcept {
	// The reference counter is set to 1 for all new game_options objects.
	auto obj = new awe::game_options();
	if (_scripts) {
		obj->currentCOs = _scripts->createArray("int");
		obj->tagCOs = _scripts->createArray("int");
	}
	return obj;
}

void awe::game_options::addRef() noexcept {
	++_refCount;
}

void awe::game_options::releaseRef() noexcept {
	if (--_refCount == 0) {
		if (currentCOs) {
			currentCOs->Release();
			currentCOs = nullptr;
		}
		if (tagCOs) {
			tagCOs->Release();
			tagCOs = nullptr;
		}
		delete this;
	}
}