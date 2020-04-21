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

#pragma once

#include "angelscript.h"
#include "as-addons/scriptbuilder.h"
#include "as-addons/scriptstdstring.h"
#include "logger.h"
#include <type_traits>

namespace awe {
	class scripts {
	public:
		scripts(const std::string& folder = "", const std::string& name = "scripts") noexcept;
		~scripts() noexcept;

		void scriptMessageCallback(const asSMessageInfo* msg, void* param) noexcept;
		void contextExceptionCallback(asIScriptContext* context) noexcept;

		bool reloadScripts(std::string folder = "") noexcept;
		const std::string& getScriptsFolder() const noexcept;
		bool functionExists(const std::string& name) const noexcept;

		template<typename T, typename... Ts>
		bool callFunction(const std::string& name, T value, Ts... values) noexcept;
		bool callFunction(const std::string& name) noexcept;
	private:
		void _registerInterface() noexcept;
		bool _setupContext(const std::string& name) noexcept;

		global::logger _logger;
		std::string _scriptsFolder;
		asIScriptEngine* _engine = nullptr;
		asIScriptContext* _context = nullptr;
		bool _callFunction_TemplateCall = false;
		asUINT _argumentID = 0;
	};
}

template<typename T, typename... Ts>
bool awe::scripts::callFunction(const std::string& name, T value, Ts... values) noexcept {
	if (!_callFunction_TemplateCall) {
		//first call to the template version, setup the context
		if (!_setupContext(name)) return false;
		_callFunction_TemplateCall = true;
	}
	//add the parameter then call either the template version (if more parameters remaining)
	//or the non-template version if there are no more paramters remaining to be added
	//(this is decided for us implicitly)
	int r = 0;
	//types still to add:
	//address of reference or handle
	//object pointer
	if (std::is_integral<T>::value) {
		switch (sizeof value) {
		case 1:
			r = _context->SetArgByte(_argumentID, (asBYTE) value);
			break;
		case 2:
			r = _context->SetArgWord(_argumentID, (asWORD) value);
			break;
		case 4:
			r = _context->SetArgDWord(_argumentID, (asDWORD) value);
			break;
		case 8:
			r = _context->SetArgQWord(_argumentID, (asQWORD) value);
			break;
		default:
			_logger.error("Unexpected length {} of integer variable {}, it will not be set to argument {} - behaviour is undefined after this point.", sizeof value, value, _argumentID);
			break;
		}
	} else if (std::is_floating_point<T>::value) {
		if (sizeof value == 4) {
			r = _context->SetArgFloat(_argumentID, (float) value);
		} else {
			r = _context->SetArgDouble(_argumentID, (double) value);
		}
	}
	if (r < 0) {
		_logger.error("Failed to set argument {} to the value \"{}\": code {}.", _argumentID, value, r);
		_argumentID = 0;
		_callFunction_TemplateCall = false;
		return false;
	}
	_argumentID++;
	return callFunction(name, values...);
}