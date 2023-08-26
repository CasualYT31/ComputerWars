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

#include "mapstrings.hpp"

static_assert(static_cast<std::size_t>(awe::map_strings::operation::count)
	== awe::map_strings::OPERATION.size(),
	"operation enum must always match with OPERATION array!");

awe::map_strings::map_strings(const engine::logger::data& data) :
	json_script({ data.sink, "json_script" }), _logger(data) {}

std::string awe::map_strings::operator[](operation op) {
	const auto name = OPERATION[static_cast<std::size_t>(op)];
	if (_strings.find(name) == _strings.end()) return ERROR_STRING;
	return _strings[name];
}

bool awe::map_strings::_load(engine::json& j) {
	std::unordered_map<std::string, std::string> strings;
	for (const auto& name : OPERATION) {
		j.apply(strings[name], { name });
		if (!j.inGoodState()) {
			_logger.error("Could not locate translation key for operation \"{}\", "
				"cancelling load operation!", name);
			return false;
		}
	}
	_strings = std::move(strings);
	return true;
}

bool awe::map_strings::_save(nlohmann::ordered_json& j) {
	for (const auto& name : _strings) j[name.first] = name.second;
	return true;
}
