/*Copyright 2019-2021 CasualYouTuber31 <naysar@protonmail.com>

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

#pragma once

#include "safejson.h"
#include <fstream>

engine::json_state::~json_state() noexcept {}
engine::json_state::json_state() noexcept {}
bool engine::json_state::inGoodState() const noexcept {
	return _bits == engine::json_state::SUCCESS;
}
engine::json_state::FailBits engine::json_state::whatFailed() const noexcept {
	return _bits;
}
void engine::json_state::resetState() noexcept {
	_bits = engine::json_state::SUCCESS;
}
void engine::json_state::_toggleState(engine::json_state::FailBits state) noexcept {
	_bits |= state;
}

engine::json::json(const std::string& name) noexcept : _logger(name) {}

engine::json::json(const nlohmann::ordered_json& jobj, const std::string& name)
	noexcept : _logger(name) {
	*this = jobj;
}

bool engine::json::equalType(nlohmann::ordered_json& dest,
	nlohmann::ordered_json& src) const noexcept {
	if (dest.type() == src.type()) return true;
	// special case 1: unsigned onto signed
	// when this JSON library parses a positive integer, it interrprets that as an
	// unsigned integer
	// however, so long as that positive integer is within the data limtis of int
	// it could technically be used with signed integers as well
	if (dest.is_number_integer() && src.is_number_unsigned() && src <= INT_MAX)
		return true;
	// special case 2: integer or unsigned onto float
	if (dest.is_number_float() &&
		(src.is_number_integer() || src.is_number_unsigned())) return true;
	// special case 3: float with a fraction of 0 onto integer or unsigned
	double temp;
	if ((dest.is_number_integer() || dest.is_number_unsigned()) &&
		src.is_number_float() && modf(src, &temp) == 0.0) return true;
	return false;
}

bool engine::json::keysExist(engine::json::KeySequence keys,
	nlohmann::ordered_json* ret) const noexcept {
	if (!keys.empty()) {
		nlohmann::ordered_json jCopy = _j;
		for (auto itr = keys.begin(), enditr = keys.end(); itr != enditr; itr++) {
			if (jCopy.contains(*itr)) {
				jCopy = jCopy[*itr];
			} else {
				return false;
			}
		}
		if (ret) *ret = jCopy;
		return true;
	}
	return false;
}

std::string engine::json::synthesiseKeySequence(engine::json::KeySequence& keys) const
	noexcept {
	if (keys.empty()) {
		return "";
	} else {
		std::string ret = "{";
		bool firstLoop = true;
		for (auto& itr : keys) {
			if (firstLoop)
				firstLoop = false;
			else
				ret += ", ";
			ret += '"' + itr + '"';
		}
		return ret + "}";
	}
}

engine::json& engine::json::operator=(const nlohmann::ordered_json& jobj) noexcept {
	if (jobj.is_object()) {
		_j = jobj;
	} else {
		_toggleState(engine::json_state::JSON_WAS_NOT_OBJECT);
		_logger.error("Attempted to assign a nlohmann::ordered_json object which "
			"had no root object.");
	}
	return *this;
}

nlohmann::ordered_json engine::json::nlohmannJSON() const noexcept {
	return _j;
}

void engine::json::applyColour(sf::Color& dest, engine::json::KeySequence keys,
	const sf::Color* defval, const bool suppressErrors) noexcept {
	std::array<unsigned int, 4> colour = { 0, 0, 0, 255 };
	applyArray(colour, keys);
	if (defval && !inGoodState()) {
		dest = *defval;
		_logger.write("{} colour property faulty: reset to the default of "
			"[{},{},{},{}].", synthesiseKeySequence(keys),
			defval->r, defval->g, defval->b, defval->a);
	} else {
		dest = sf::Color(colour[0], colour[1], colour[2], colour[3]);
	}
	if (suppressErrors) resetState();
}

std::string engine::json::_getTypeName(nlohmann::ordered_json& j) const noexcept {
	if (j.is_number_float()) return "float";
	return j.type_name();
}

engine::json_script::~json_script() noexcept {}

std::string engine::json_script::getScriptPath() const noexcept {
	return _script;
}

std::string engine::json_script::jsonwhat() const noexcept {
	return _what;
}

void engine::json_script::load(const std::string script) noexcept {
	if (script != "") _script = script;
	_logger.write("Loading JSON script {}...", getScriptPath());
	nlohmann::ordered_json nlohmannJSON;
	if (_loadFromScript(nlohmannJSON)) {
		engine::json safeJSON = nlohmannJSON;
		if (!_load(safeJSON)) {
			_toggleState(engine::json_state::FAILED_LOAD_METHOD);
			_logger.write("Failed to load JSON script {}.", getScriptPath());
		} else {
			_logger.write("Finished loading JSON script {}.", getScriptPath());
		}
	}
}

void engine::json_script::save(const std::string script) noexcept {
	std::string scriptPath = _script; if (script != "") scriptPath = script;
	_logger.write("Saving JSON script {}...", scriptPath);
	nlohmann::ordered_json nlohmannJSON;
	if (!_save(nlohmannJSON)) {
		_toggleState(engine::json_state::FAILED_SAVE_METHOD);
		_logger.write("Failed to save JSON script {}.", scriptPath);
	} else {
		_script = scriptPath;
		if (_saveToScript(nlohmannJSON)) {
			_logger.write("Finished saving JSON script {}.", scriptPath);
		} else {
			_logger.write("Failed to save JSON script {}.", scriptPath);
		}
	}
}

bool engine::json_script::_loadFromScript(nlohmann::ordered_json& jobj) noexcept {
	std::ifstream jscript(_script);
	if (jscript.good()) {
		try {
			jscript >> jobj;
		}
		catch (std::exception & e) {
			_what = e.what();
			_toggleState(engine::json_state::UNPARSABLE);
			_logger.error("Provided JSON script \"{}\" has incorrect syntax: {}.",
				getScriptPath(), jsonwhat());
		}
		jscript.close();
	} else {
		_toggleState(engine::json_state::FAILED_SCRIPT_LOAD);
		_logger.error("Failed to open JSON script \"{}\" for reading.",
			getScriptPath());
	}
	if (inGoodState()) return true;
	return false;
}

bool engine::json_script::_saveToScript(nlohmann::ordered_json& jobj) noexcept {
	std::ofstream jscript(_script);
	if (jscript.good()) {
		try {
			jscript << jobj;
		}
		catch (std::exception & e) {
			_what = e.what();
			_toggleState(engine::json_state::FAILED_SCRIPT_SAVE);
			_logger.error("Could not write JSON object to JSON script \"{}\": {}.",
				getScriptPath(), jsonwhat());
		}
		jscript.close();
	} else {
		_toggleState(engine::json_state::FAILED_SCRIPT_SAVE);
		_logger.error("Failed to open JSON script \"{}\" for writing.",
			getScriptPath());
	}
	if (inGoodState()) return true;
	return false;
}