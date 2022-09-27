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

void awe::game_options::Register(asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) noexcept {
	awe::RegisterGameTypedefs(engine, document);

	auto r = engine->RegisterObjectType("GameOptions", 0, asOBJ_REF);
	document->DocumentObjectType(r, "Holds options that the game engine will "
		"require when it creates a new game on a map.");

	r = engine->RegisterObjectBehaviour("GameOptions", asBEHAVE_FACTORY,
		"GameOptions@ f()", asFUNCTION(awe::game_options::Create), asCALL_CDECL);
	r = engine->RegisterObjectBehaviour("GameOptions", asBEHAVE_ADDREF,
		"void f()", asMETHOD(awe::game_options, AddRef), asCALL_THISCALL);
	r = engine->RegisterObjectBehaviour("GameOptions", asBEHAVE_RELEASE,
		"void f()", asMETHOD(awe::game_options, Release), asCALL_THISCALL);

	r = engine->RegisterObjectMethod("GameOptions",
		"void setCurrentCO(const ArmyID, const BankID)",
		asMETHOD(awe::game_options, setCurrentCO), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Sets an override for an army's current "
		"CO.");

	r = engine->RegisterObjectMethod("GameOptions",
		"void setTagCO(const ArmyID, const BankID)",
		asMETHOD(awe::game_options, setTagCO), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Sets an override for an army's tag CO.");

	r = engine->RegisterObjectMethod("GameOptions",
		"void setNoTagCO(const ArmyID, const bool)",
		asMETHOD(awe::game_options, setNoTagCO), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Sets whether or not an army's tag CO "
		"should be overridden with a lack of a CO.");

	r = engine->RegisterObjectMethod("GameOptions",
		"void setTeam(const ArmyID, const TeamID)",
		asMETHOD(awe::game_options, setTeam), asCALL_THISCALL);
	document->DocumentObjectMethod(r, "Sets an override for an army's team.");
}
awe::game_options* awe::game_options::Create() noexcept {
	// The reference counter is set to 1 for all new game_options objects.
	auto obj = new awe::game_options();
	return obj;
}
void awe::game_options::AddRef() const noexcept {
	++_refCount;
}
void awe::game_options::Release() const noexcept {
	if (--_refCount == 0) {
		delete this;
	}
}

void awe::game_options::setCurrentCO(const awe::ArmyID armyID,
	const awe::BankID bankID) noexcept {
	CommanderPair currentPair = _commanderOverrides[armyID];
	currentPair.first = bankID;
	_commanderOverrides[armyID] = currentPair;
}

void awe::game_options::setTagCO(const awe::ArmyID armyID,
	const awe::BankID bankID) noexcept {
	CommanderPair currentPair = _commanderOverrides[armyID];
	currentPair.second = bankID;
	_commanderOverrides[armyID] = currentPair;
}

void awe::game_options::setNoTagCO(const awe::ArmyID armyID, const bool tag)
	noexcept {
	_noTags[armyID] = tag;
}

void awe::game_options::setTeam(const awe::ArmyID armyID, const awe::TeamID teamID)
	noexcept {
	_teamOverrides[armyID] = teamID;
}

std::shared_ptr<const awe::commander> awe::game_options::getCurrentCO(
	const awe::ArmyID armyID,
	const std::shared_ptr<awe::bank<awe::commander>>& coBank) const {
	if (coBank) {
		if (_commanderOverrides.find(armyID) != _commanderOverrides.end()) {
			auto coID = _commanderOverrides.at(armyID).first;
			if (coID) {
				if (*coID < coBank->size()) {
					return coBank->operator[](*coID);
				} else {
					throw std::range_error("commander index " +
						std::to_string(*coID) + " is out of range!");
				}
			}
		}
		// No override configured, so not an actual error.
		throw std::range_error("");
	}
	throw std::range_error("no commander bank given!");
}

std::shared_ptr<const awe::commander> awe::game_options::getTagCO(
	const awe::ArmyID armyID,
	const std::shared_ptr<awe::bank<awe::commander>>& coBank) const {
	if (_noTags.find(armyID) != _noTags.end() && _noTags.at(armyID))
		return nullptr;
	if (coBank) {
		if (_commanderOverrides.find(armyID) != _commanderOverrides.end()) {
			auto coID = _commanderOverrides.at(armyID).second;
			if (coID) {
				if (*coID < coBank->size()) {
					return coBank->operator[](*coID);
				} else {
					throw std::range_error("commander index " +
						std::to_string(*coID) + " is out of range!");
				}
			}
		}
		// No override configured, so not an actual error.
		throw std::range_error("");
	}
	throw std::range_error("no commander bank given!");
}

awe::TeamID awe::game_options::getTeam(const awe::ArmyID armyID) const {
	if (_teamOverrides.find(armyID) != _teamOverrides.end()) {
		return _teamOverrides.at(armyID);
	}
	// No override configured, so not an actual error.
	throw std::range_error("");
}