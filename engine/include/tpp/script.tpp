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

#pragma once

template<typename T>
T* engine::script_reference_type<T>::Create() noexcept {
	// The reference counter is automatically set to 1 for all new objects.
	return new T();
}

template<typename T>
void engine::script_reference_type<T>::AddRef() const noexcept {
	++_refCount;
}

template<typename T>
void engine::script_reference_type<T>::Release() const noexcept {
	if (--_refCount == 0) {
		delete static_cast<const T*>(this);
	}
}

template<typename T>
int engine::script_reference_type<T>::RegisterType(asIScriptEngine* engine,
	const std::string& type) noexcept {
	int r = engine->RegisterObjectType(type.c_str(), 0, asOBJ_REF);
	engine->RegisterObjectBehaviour(type.c_str(), asBEHAVE_FACTORY,
		std::string(type + "@ f()").c_str(),
		asFUNCTION(engine::script_reference_type<T>::Create), asCALL_CDECL);
	engine->RegisterObjectBehaviour(type.c_str(), asBEHAVE_ADDREF,
		"void f()", asMETHOD(engine::script_reference_type<T>, AddRef),
		asCALL_THISCALL);
	engine->RegisterObjectBehaviour(type.c_str(), asBEHAVE_RELEASE,
		"void f()", asMETHOD(engine::script_reference_type<T>, Release),
		asCALL_THISCALL);
	return r;
}

template<typename T, typename... Ts>
bool engine::scripts::callFunction(const std::string& name, T value, Ts... values)
noexcept {
	if (!_callFunction_TemplateCall) {
		// First call to the template version, so setup the context.
		if (!_setupContext(name)) return false;
		_callFunction_TemplateCall = true;
	}
	// Add the parameter then call either the template version (if more parameters
	// remaining),
	// or the non-template version if there are no more paramters remaining to be
	// added (this is decided for us implicitly).
	int r = 0;
	if constexpr (std::is_integral<T>::value) {
		// This also conveniently covers bool for us.
		switch (sizeof value) {
		case 1:
			r = _context[_contextId]->SetArgByte(_argumentID, (asBYTE)value);
			break;
		case 2:
			r = _context[_contextId]->SetArgWord(_argumentID, (asWORD)value);
			break;
		case 4:
			r = _context[_contextId]->SetArgDWord(_argumentID, (asDWORD)value);
			break;
		case 8:
			r = _context[_contextId]->SetArgQWord(_argumentID, (asQWORD)value);
			break;
		default:
			_logger.error("Unexpected length {} of integer variable, it will not "
				"be set to argument {} of function \"{}\": function call aborted.",
				sizeof value, _argumentID, name);
			_resetCallFunctionVariables();
			return false;
		}
	} else if constexpr (std::is_floating_point<T>::value) {
		if (sizeof value == 4) {
			r = _context[_contextId]->SetArgFloat(_argumentID, (float)value);
		} else {
			r = _context[_contextId]->SetArgDouble(_argumentID, (double)value);
		}
	} else if constexpr (std::is_pointer<T>::value) {
		if (!value) {
			_logger.error("Attempted to assign a null pointer to argument {} of "
				"function \"{}\": function call aborted.", _argumentID, name);
			_resetCallFunctionVariables();
			return false;
		}
		r = _context[_contextId]->SetArgObject(_argumentID, value);
	} else if constexpr (std::is_object<T>::value || std::is_reference<T>::value) {
		_logger.error("Attempted to add an object to argument {} of function "
			"\"{}\", which is not supported: function call aborted.", _argumentID,
			name);
		_resetCallFunctionVariables();
		return false;
	}
	if (r < 0) {
		if constexpr (std::is_pointer<T>::value) {
			if (!value) {
				_logger.error("Failed to set argument {} of function \"{}\" to "
					"the value nullptr: code {}.", _argumentID, name, r);
				_resetCallFunctionVariables();
				return false;
			}
		}
		_logger.error("Failed to set argument {} of function \"{}\": code {}.",
			_argumentID, name, r);
		_resetCallFunctionVariables();
		return false;
	}
	_argumentID++;
	return callFunction(name, values...);
}