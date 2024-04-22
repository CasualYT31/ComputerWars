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

/////////////////////////////////
// SCRIPT_TYPE SPECIALISATIONS //
/////////////////////////////////

template<>
inline constexpr std::string engine::script_type<bool>() { return "bool"; }
template<>
inline constexpr std::string engine::script_type<sf::Int8>() { return "int8"; }
template<>
inline constexpr std::string engine::script_type<sf::Uint8>() { return "uint8"; }
template<>
inline constexpr std::string engine::script_type<sf::Int16>() { return "int16"; }
template<>
inline constexpr std::string engine::script_type<sf::Uint16>() { return "uint16"; }
template<>
inline constexpr std::string engine::script_type<sf::Int32>() { return "int"; }
template<>
inline constexpr std::string engine::script_type<sf::Uint32>() { return "uint"; }
template<>
inline constexpr std::string engine::script_type<sf::Int64>() { return "int64"; }
template<>
inline constexpr std::string engine::script_type<sf::Uint64>() { return "uint64"; }
template<>
inline constexpr std::string engine::script_type<float>() { return "float"; }
template<>
inline constexpr std::string engine::script_type<double>() { return "double"; }
template<>
inline constexpr std::string engine::script_type<std::string>() {
	return "string";
}

template<>
inline constexpr std::string engine::script_type<sf::Color>() { return "Colour"; }
template<>
inline constexpr std::string engine::script_type<sf::Vector2u>() {
	return "Vector2";
}
template<>
inline constexpr std::string engine::script_type<sf::Vector2i>() {
	return "MousePosition";
}
template<>
inline constexpr std::string engine::script_type<sf::Vector2f>() {
	return "Vector2f";
}
template<>
inline constexpr std::string engine::script_type<sf::IntRect>() {
	return "IntRect";
}
template<>
inline constexpr std::string engine::script_type<sf::Time>() { return "Time"; }
template<>
inline constexpr std::string engine::script_type<sf::Clock>() { return "Clock"; }

template<>
inline constexpr std::string engine::script_type<engine::binary_istream>() {
	return "BinaryIStream";
}
template<>
inline constexpr std::string engine::script_type<engine::binary_ostream>() {
	return "BinaryOStream";
}

template<typename T>
constexpr std::string engine::script_param_type() {
	constexpr const bool NUMBER =
		std::is_integral<T>::value || std::is_floating_point<T>::value;
	constexpr const bool PTR = std::is_pointer<T>::value &&
		!std::is_integral<std::remove_pointer<T>::type>::value &&
		!std::is_floating_point<std::remove_pointer<T>::type>::value;
	constexpr const bool OBJ = std::is_object<T>::value;
	static_assert(NUMBER || PTR || OBJ, "engine::script_param_type<T>(): type T "
		"isn't supported as a constant input parameter, you will have to write "
		"out your intended qualifier/s manually");
	if constexpr (NUMBER) {
		return std::string("const ").append(engine::script_type<T>());
	} else if constexpr (PTR) {
		return std::string("const ").append(
			engine::script_type<std::remove_pointer<T>::type>()).append("&in");
	} else if constexpr (OBJ) {
		return std::string("const ").append(engine::script_type<T>())
			.append("&in");
	}
	return "";
}

template<std::size_t N, std::size_t C, bool COMMA>
constexpr std::string engine::params_builder(
	const std::array<const char*, N>& customParams) {
	static_assert(C == N, "Extra custom parameters were given to the "
		"sig_builder() call, please remove these if they are not needed");
	return "";
}

template<std::size_t N, std::size_t C, bool COMMA, typename T, typename... Ts>
constexpr std::string engine::params_builder(
	const std::array<const char*, N>& customParams) {
	if constexpr (COMMA) {
		if constexpr (std::is_void<T>::value) {
			static_assert(C < N, "Not enough custom parameters were given to "
				"the sig_builder() call");
			return std::string(", ").append(customParams[C])
				.append(engine::params_builder<N, C + 1, true, Ts...>(
					customParams));
		} else {
			return std::string(", ").append(engine::script_param_type<T>())
				.append(engine::params_builder<N, C, true, Ts...>(customParams));
		}
	} else {
		if constexpr (std::is_void<T>::value) {
			static_assert(C < N, "Not enough custom parameters were given to "
				"the sig_builder() call");
			return std::string(customParams[C])
				.append(engine::params_builder<N, C + 1, true, Ts...>(
					customParams));
		} else {
			return std::string(engine::script_param_type<T>())
				.append(engine::params_builder<N, C, true, Ts...>(customParams));
		}
	}
}

template<std::size_t N, typename... Ts>
constexpr std::string engine::sig_builder(const std::string& funcName,
	const std::array<const char*, N>& customParams, const std::string& retType) {
	return std::string(retType).append(" ").append(funcName).append("(")
		.append(engine::params_builder<N, 0, false, Ts...>(customParams))
		.append(")");
}

template<typename... Ts>
constexpr std::string sig_builder(const std::string& funcName,
	const std::string& retType) {
	return engine::sig_builder<0, Ts...>(funcName, {}, retType);
}

///////////////////////////
// SCRIPT_REFERENCE_TYPE //
///////////////////////////

template<typename T>
void engine::script_reference_type<T>::AddRef() const noexcept {
	++_refCount;
}

template<typename T>
void engine::script_reference_type<T>::Release() const {
	if (--_refCount == 0) delete static_cast<const T*>(this);
}

template<typename T>
int engine::script_reference_type<T>::RegisterType(asIScriptEngine* engine,
	const std::string& type, const std::function<void(asIScriptEngine*,
		const std::string&)>& registerFactory) {
	assert(("At least one factory function must be registered!", registerFactory));
	int r = engine->RegisterObjectType(type.c_str(), 0, asOBJ_REF);
	registerFactory(engine, type);
	engine->RegisterObjectBehaviour(type.c_str(), asBEHAVE_ADDREF,
		"void f()", asMETHOD(engine::script_reference_type<T>, AddRef),
		asCALL_THISCALL);
	engine->RegisterObjectBehaviour(type.c_str(), asBEHAVE_RELEASE,
		"void f()", asMETHOD(engine::script_reference_type<T>, Release),
		asCALL_THISCALL);
	return r;
}

////////////////////
// CSCRIPTWRAPPER //
////////////////////

template<typename T>
engine::CScriptWrapper<T>::CScriptWrapper(T* const obj) : _ptr(obj) {
	if (_ptr) _ptr->AddRef();
}

template<typename T>
engine::CScriptWrapper<T>::CScriptWrapper(
	const engine::CScriptWrapper<T>& obj) : _ptr(obj.operator->()) {
	if (_ptr) _ptr->AddRef();
}

template<typename T>
engine::CScriptWrapper<T>::CScriptWrapper(
	engine::CScriptWrapper<T>&& obj) noexcept : _ptr(std::move(obj.operator->())) {
	if (_ptr) _ptr->AddRef();
}

template<typename T>
engine::CScriptWrapper<T>::~CScriptWrapper() noexcept {
	if (_ptr) _ptr->Release();
}

template<typename T>
T* engine::CScriptWrapper<T>::operator->() const noexcept {
	return _ptr;
}

///////////////
// FUNCTIONS //
///////////////

template<typename T, typename U>
T engine::ConvertCScriptArray(const CScriptArray* const a) {
	T ret = {};
	if (a) {
		engine::AttemptToReserve(ret, a->GetSize());
		for (asUINT i = 0; i < a->GetSize(); ++i)
			ret.insert(ret.end(), *static_cast<const U*>(a->At(i)));
		a->Release();
	}
	return ret;
}

/////////////
// SCRIPTS //
/////////////

template<typename... Ts>
bool engine::scripts::callFunction(const std::string& module,
	const std::string& name, Ts... values) {
	auto const m = _engine->GetModule(module.c_str());
	if (!m) {
		_logger.error("Failed to call function \"{}\" in module \"{}\": that "
			"module does not exist.", name, module);
		return false;
	}
	auto const func = m->GetFunctionByName(name.c_str());
	if (!func) {
		_logger.error("Failed to access function \"{}\" in module \"{}\": either "
			"it was not defined in any of the scripts or it was defined more than "
			"once.", name, module);
		return false;
	}
	return callFunction(func, values...);
}

template<typename T, typename... Ts>
bool engine::scripts::callFunction(asIScriptFunction* const func, T value,
	Ts... values) {
	if (!_callFunction_TemplateCall) {
		// First call to the template version, so setup the context.
		if (!_setupContext(func)) return false;
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
				sizeof value, _argumentID, func->GetName());
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
		// If value points to a primitive type or an address, pass it using
		// Address(). Otherwise, assume it's an object.
		if constexpr (std::is_integral<std::remove_pointer<T>::type>::value ||
			std::is_floating_point<std::remove_pointer<T>::type>::value) {
			if (!value) {
				_logger.error("Attempted to assign a null pointer to argument {} "
					"of function \"{}\", which should point to a primitive type: "
					"function call aborted.", _argumentID, func->GetName());
				_resetCallFunctionVariables();
				return false;
			}
			r = _context[_contextId]->SetArgAddress(_argumentID, value);
		} else {
			r = _context[_contextId]->SetArgObject(_argumentID, value);
		}
	} else if constexpr (std::is_object<T>::value || std::is_reference<T>::value) {
		_logger.error("Attempted to add an object to argument {} of function "
			"\"{}\", which is not supported: function call aborted.", _argumentID,
			func->GetName());
		_resetCallFunctionVariables();
		return false;
	}
	if (r < 0) {
		if constexpr (std::is_pointer<T>::value) {
			if (!value) {
				_logger.error("Failed to set argument {} of function \"{}\" to "
					"the value nullptr: code {}.", _argumentID, func->GetName(),
					r);
				_resetCallFunctionVariables();
				return false;
			}
		}
		_logger.error("Failed to set argument {} of function \"{}\": code {}.",
			_argumentID, func->GetName(), r);
		_resetCallFunctionVariables();
		return false;
	}
	++_argumentID;
	return callFunction(func, values...);
}

template<typename... Ts>
bool engine::scripts::callMethod(asIScriptObject* const obj,
	const std::string& methodDecl, Ts... values) {
	if (!obj) {
		_logger.error("Attempted to invoke a method \"{}\" on a null object!",
			methodDecl);
		return false;
	}
	const auto objType = obj->GetObjectType();
	const auto funcPtr = objType->GetMethodByDecl(methodDecl.c_str());
	if (!funcPtr) {
		_logger.error("Attempted to invoke a method \"{}\" on an object of type "
			"\"{}\". The method could not be found.", methodDecl,
			objType->GetName());
		return false;
	}
	_functionObject = obj;
	return callFunction(funcPtr, values...);
}

template<typename T>
CScriptArray* engine::scripts::createArrayFromContainer(const std::string& type,
	T& stl) const {
	CScriptArray* const ret = createArray(type);
	ret->Resize(static_cast<asUINT>(stl.size()));
	asUINT x = 0;
	if (ret) for (typename T::value_type i : stl) ret->SetValue(x++, &i);
	return ret;
}
