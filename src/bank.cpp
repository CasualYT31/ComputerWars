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

#include "bank.h"

//*******************
//*COMMON PROPERTIES*
//*******************
awe::common_properties::common_properties(safe::json& j) noexcept {
	j.apply(_name, { "longname" }, &_name, true);
	j.apply(_shortName, { "shortname" }, &_shortName, true);
	j.apply(_iconKey, { "icon" }, &_iconKey, true);
}
awe::common_properties::~common_properties() noexcept {}
const std::string& awe::common_properties::getName() const noexcept {
	return _name;
}
const std::string& awe::common_properties::getShortName() const noexcept {
	return _shortName;
}
sfx::SpriteKey awe::common_properties::getIconKey() const noexcept {
	return _iconKey;
}

//*********
//*COUNTRY*
//*********
awe::country::country(safe::json& j) noexcept : common_properties(j) {
	j.applyColour(_colour, { "colour" }, &_colour, true);
}
const sf::Color& awe::country::getColour() const noexcept {
	return _colour;
}

//*********
//*WEATHER*
//*********
awe::weather::weather(safe::json& j) noexcept : common_properties(j) {}

//*************
//*ENVIRONMENT*
//*************
awe::environment::environment(safe::json& j) noexcept : common_properties(j) {}

//***************
//*MOVEMENT TYPE*
//***************
awe::movement_type::movement_type(safe::json& j) noexcept : common_properties(j) {}