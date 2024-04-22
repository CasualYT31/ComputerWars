/*Copyright 2019-2024 CasualYouTuber31 <naysar@protonmail.com>

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

#include "safejson.hpp"
#include <fstream>
#include "fmtengine.hpp"

engine::json::json(const engine::logger::data& data) {
	_logger.setData(data);
}

engine::json::json(const nlohmann::ordered_json& jobj,
	const engine::logger::data& data) {
	try {
		_logger.setData(data);
		*this = jobj;
	} catch (const std::exception& e) {
		*this = jobj;
		throw e;
	}
}

engine::json::json(nlohmann::ordered_json&& jobj, const engine::logger::data& data)
	{
	try {
		_logger.setData(data);
		*this = std::move(jobj);
	} catch (const std::exception& e) {
		*this = std::move(jobj);
		throw e;
	}
}

engine::json::json(const engine::json& obj, const engine::logger::data& data) {
	try {
		_logger.setData(data);
		*this = obj;
	} catch (const std::exception& e) {
		*this = obj;
		throw e;
	}
}

engine::json::json(engine::json&& obj, const engine::logger::data& data) {
	try {
		_logger.setData(data);
		*this = std::move(obj);
	} catch (const std::exception& e) {
		*this = std::move(obj);
		throw e;
	}
}

engine::json::json(const engine::json& obj) : _j(obj._j), _logger(obj._logger) {}

engine::json::json(engine::json&& obj) noexcept : _j(std::move(obj._j)),
	_logger(std::move(obj._logger)) {}

engine::json& engine::json::operator=(const engine::json& obj) noexcept {
	return *this = obj._j;
}

engine::json& engine::json::operator=(engine::json&& obj) noexcept {
	return *this = std::move(obj._j);
}

engine::json& engine::json::operator=(const nlohmann::ordered_json& jobj)
	noexcept {
	if (jobj.is_object()) {
		_j = jobj;
	} else {
		_toggleState(engine::json_state::JSON_WAS_NOT_OBJECT);
		_logger.error("Attempted to assign a nlohmann::ordered_json object which "
			"had no root object.");
	}
	return *this;
}

engine::json& engine::json::operator=(nlohmann::ordered_json&& jobj) noexcept {
	if (jobj.is_object()) {
		_j = std::move(jobj);
	} else {
		_toggleState(engine::json_state::JSON_WAS_NOT_OBJECT);
		_logger.error("Attempted to assign a nlohmann::ordered_json object which "
			"had no root object.");
	}
	return *this;
}

static bool keyExists(const nlohmann::ordered_json& obj,
	const engine::json::KeySequence::const_iterator& key,
	const engine::json::KeySequence::const_iterator& end,
	nlohmann::ordered_json* const ret) {
	if (key == end) {
		if (ret) *ret = obj;
		return true;
	}
	if (obj.contains(*key)) return keyExists(obj[*key], key + 1, end, ret);
	return false;
}

bool engine::json::keysExist(const engine::json::KeySequence& keys,
	nlohmann::ordered_json* const ret) const noexcept {
	if (!keys.empty()) return keyExists(_j, keys.cbegin(), keys.cend(), ret);
	return false;
}

bool engine::json::equalType(const nlohmann::ordered_json& dest,
	const nlohmann::ordered_json& src) noexcept {
	if (dest.type() == src.type()) return true;
	// Special case 1: unsigned onto signed.
	// When this JSON library parses a positive integer, it interprets that as an
	// unsigned integer. However, so long as that positive integer is within the
	// data limtis of a signed int, it could technically be used with signed
	// integers as well.
	if (dest.is_number_integer() && src.is_number_unsigned() && src <= INT_MAX)
		return true;
	// Special case 2: integer or unsigned onto float.
	if (dest.is_number_float() &&
		(src.is_number_integer() || src.is_number_unsigned())) return true;
	// Special case 3: float with a fraction of 0 onto integer or unsigned.
	double temp;
	if ((dest.is_number_integer() || dest.is_number_unsigned()) &&
		src.is_number_float() && modf(src, &temp) == 0.0) return true;
	return false;
}

std::string engine::json::synthesiseKeySequence(
	const engine::json::KeySequence& keys) {
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

engine::json::KeySequence engine::json::concatKeys(KeySequence parentKeys,
	const KeySequence& childKeys) {
	std::copy(childKeys.begin(), childKeys.end(), std::back_inserter(parentKeys));
	return parentKeys;
}

void engine::json::applyColour(sf::Color& dest,
	const engine::json::KeySequence& keys, const bool suppressErrors) {
	std::array<unsigned int, 4> colour = { 0, 0, 0, 255 };
	applyArray(colour, keys);
	if (!inGoodState()) {
		if (suppressErrors) resetState();
		_logger.write("{} colour property faulty: left to the default of {}.",
			synthesiseKeySequence(keys), dest);
	} else {
		dest = sf::Color(colour[0], colour[1], colour[2], colour[3]);
	}
}

bool engine::json::_performInitialChecks(const engine::json::KeySequence& keys,
	nlohmann::ordered_json& test, nlohmann::ordered_json dest, std::string type,
	const bool optional) {
	if (type == "") type = getTypeName(dest);
	if (keys.empty()) {
		_logger.error("Attempted to assign a value to a destination of type "
			"\"{}\" without specifying a key sequence.", type);
		_toggleState(NO_KEYS_GIVEN);
	} else {
		if (keysExist(keys, &test)) {
			if (equalType(dest, test)) {
				return true;
			} else {
				_logger.error("Attempted to assign a value of data type \"{}\" to "
					"a destination of type \"{}\", in the key sequence {}.",
					getTypeName(test), type, synthesiseKeySequence(keys));
				_toggleState(MISMATCHING_TYPE);
			}
		} else if (!optional) {
			_logger.error("The key sequence {} does not exist in the JSON object.",
				synthesiseKeySequence(keys));
			_toggleState(KEYS_DID_NOT_EXIST);
		}
	}
	return false;
}

engine::json_script::json_script(const engine::logger::data& data) : _logger(data)
	{}

std::string engine::json_script::jsonwhat() const {
	return _what;
}

void engine::json_script::load(const std::string script) {
	if (script != "") _script = script;
	_logger.write("Loading JSON script {}...", getScriptPath());
	nlohmann::ordered_json nlohmannJSON;
	if (_loadFromScript(nlohmannJSON)) {
		engine::json safeJSON(nlohmannJSON, {_logger.getData().sink, "json"});
		if (safeJSON.whatFailed() & engine::json_state::JSON_WAS_NOT_OBJECT) {
			_logger.error("Failed to load JSON script {}: the JSON saved in the "
				"script didn't contain a root object.", getScriptPath());
			_toggleState(engine::json_state::JSON_WAS_NOT_OBJECT);
		} else {
			bool ret = false;
			try {
				ret = _load(safeJSON);
			} catch (...) {
				_logger.write("Failed to load JSON script {}.", getScriptPath());
				_toggleState(engine::json_state::FAILED_LOAD_METHOD);
				throw;
			}
			if (!ret) {
				_logger.write("Failed to load JSON script {}.", getScriptPath());
				_toggleState(engine::json_state::FAILED_LOAD_METHOD);
			} else {
				_logger.write("Finished loading JSON script {}.", getScriptPath());
			}
		}
	}
}

void engine::json_script::save(const std::string script) {
	std::string scriptPath = _script; if (script != "") scriptPath = script;
	_logger.write("Saving JSON script {}...", scriptPath);
	nlohmann::ordered_json nlohmannJSON;
	bool ret = false;
	try {
		ret = _save(nlohmannJSON);
	} catch (...) {
		_logger.write("Failed to save JSON script {}.", scriptPath);
		_toggleState(engine::json_state::FAILED_SAVE_METHOD);
		throw;
	}
	if (!ret) {
		_logger.write("Failed to save JSON script {}.", scriptPath);
		_toggleState(engine::json_state::FAILED_SAVE_METHOD);
	} else {
		if (!nlohmannJSON.is_object()) {
			_logger.error("Failed to save JSON script {}: given JSON object did "
				"not contain a root object.", scriptPath);
			_toggleState(engine::json_state::JSON_WAS_NOT_OBJECT);
		} else {
			_script = scriptPath;
			if (_saveToScript(nlohmannJSON)) {
				_logger.write("Finished saving JSON script {}.", scriptPath);
			} else {
				_logger.write("Failed to save JSON script {}.", scriptPath);
			}
		}
	}
}

bool engine::json_script::_loadFromScript(nlohmann::ordered_json& jobj) {
	std::ifstream jscript(_script);
	if (jscript.good()) {
		try {
			jscript >> jobj;
			jscript.close();
		} catch (const std::exception& e) {
			jscript.close();
			_what = e.what();
			_logger.error("Provided JSON script \"{}\" has incorrect syntax: {}",
				getScriptPath(), jsonwhat());
			_toggleState(engine::json_state::UNPARSABLE);
		}
	} else {
		_logger.error("Failed to open JSON script \"{}\" for reading.",
			getScriptPath());
		_toggleState(engine::json_state::FAILED_SCRIPT_LOAD);
	}
	return inGoodState();
}

bool engine::json_script::_saveToScript(nlohmann::ordered_json& jobj) {
	std::ofstream jscript(_script);
	if (jscript.good()) {
		try {
			jscript << jobj;
			jscript.close();
		} catch (const std::exception& e) {
			jscript.close();
			_what = e.what();
			_logger.error("Could not write JSON object to JSON script \"{}\": {}",
				getScriptPath(), jsonwhat());
			_toggleState(engine::json_state::FAILED_SCRIPT_SAVE);
		}
	} else {
		_logger.error("Failed to open JSON script \"{}\" for writing.",
			getScriptPath());
		_toggleState(engine::json_state::FAILED_SCRIPT_SAVE);
	}
	return inGoodState();
}
