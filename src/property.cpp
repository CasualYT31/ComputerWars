/*Copyright 2020 CasualYouTuber31 <naysar@protonmail.com>

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

#include "property.h"

awe::property::~property() noexcept {}

awe::game_property::~game_property() noexcept {}

awe::property& awe::game_property::operator[](const std::string& key) noexcept {
	return _data[key];
}

awe::property& awe::game_property::operator[](const unsigned int id) noexcept {
	if (_findByID.find(id) == _findByID.end()) {
		_data[""].id = id;
		_findByID[id] = &_data[""];
	}
	return *_findByID[id];
}

bool awe::game_property::_load(safe::json& j) noexcept {
	bool ret = true;
	nlohmann::json jj = j.nlohmannJSON();
	for (auto& i : jj.items()) {
		j.apply(_data[i.key()].nativeName, { i.key(), "longname" }, &_data[i.key()].nativeName, true);
		j.apply(_data[i.key()].nativeShortName, { i.key(), "shortname" }, &_data[i.key()].nativeShortName, true);
		j.apply(_data[i.key()].spriteKey, { i.key(), "icon" }, &_data[i.key()].spriteKey, true);
		j.applyColour(_data[i.key()].colour, { i.key(), "colour" }, &_data[i.key()].colour, true);
		j.apply(_data[i.key()].id, { i.key(), "id" });
		if (!j.inGoodState()) {
			_data.erase(i.key());
			ret = false;
		} else {
			_findByID[_data[i.key()].id] = &_data[i.key()];
		}
	}
	return ret;
}

bool awe::game_property::_save(nlohmann::json& j) noexcept {
	for (auto itr = _data.begin(), enditr = _data.end(); itr != enditr; itr++) {
		j[itr->first]["id"] = itr->second.id;
		j[itr->first]["longname"] = itr->second.nativeName;
		j[itr->first]["shortname"] = itr->second.nativeShortName;
		j[itr->first]["icon"] = itr->second.spriteKey;
		j[itr->first]["colour"][0] = itr->second.colour.r;
		j[itr->first]["colour"][1] = itr->second.colour.g;
		j[itr->first]["colour"][2] = itr->second.colour.b;
		j[itr->first]["colour"][3] = itr->second.colour.a;
	}
	return true;
}