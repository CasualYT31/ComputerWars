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

/**@file script.h
 * Defines code realted to executing scripts on disc.
 * This implementation uses AngelScript.
 */

#pragma once

#include "angelscript.h"
#include "scriptbuilder.h"
#include "scriptstdstring.h"
#include "logger.h"
#include <type_traits>

namespace engine {
	/**
	 * Class representing a layer of abstraction between a script engine and the
	 * client.
	 * A folder of scripts is loaded from disc. This class can then be used to call
	 * functions from these scripts, given the name of the function and a variety
	 * of parameters.
	 */
	class scripts {
	public:
		/**
		 * Sets up the script engine and initialises the internal logger object.
		 * The following steps are carried out:
		 * <ol><li>The script engine is loaded.</li>
		 *     <li>Error callbacks and extra modules are added to the engine.</li>
		 *     <li>The scripts are loaded if \c folder is not blank.</li>
		 *     <li>The function context is initialised.</li></ol>
		 * The only steps the constructor does not carry out are registering the
		 * interface, and loading the script files. This is because the interface
		 * will likely require access to code outside of the \c engine
		 * namespace/library, so it is carried out separately using the
		 * \c registerInterface() method. In addition, loading and building scripts
		 * relies on the interface being established, so it is carried out
		 * afterwards using the \c loadScripts() method.
		 * @param name The name to give this particular instantiation within the
		 *             log file. Defaults to "scripts."
		 * @sa    \c engine::logger
		 * @sa    \c engine::scripts::registerInterface()
		 */
		scripts(const std::string& name = "scripts") noexcept;

		/**
		 * Releases the sole function context and shuts down the engine.
		 */
		~scripts() noexcept;

		/**
		 * Registers the interface between the client and the scripts.
		 * The callback must accept a pointer to the script engine. Using this
		 * pointer, the callback is to register the interface directly to the
		 * script engine. See <a
		 * href="https://www.angelcode.com/angelscript/sdk/docs/manual/classas_i_script_engine.html"
		 * target="_blank">the documentation on asIScriptEngine</a> for help on how
		 * to register the interface.
		 * @warning This method must be called before loading scripts, or else
		 *          building the scripts will likely fail.
		 * @param   callback The function which registers the interface.
		 */
		void registerInterface(
			const std::function<void(asIScriptEngine*)>& callback) noexcept;

		/**
		 * The message callback assigned to the script engine.
		 * This callback mirrors messages to the log file.
		 * @param msg   The object containing the message information to output.
		 * @param param Ignored.
		 */
		void scriptMessageCallback(const asSMessageInfo* msg, void* param)
			noexcept;

		/**
		 * The runtime error callback assigned to the function context.
		 * @param context The pointer to the function context which has encountered
		 *                an error during runtime.
		 */
		void contextExceptionCallback(asIScriptContext* context) noexcept;

		/**
		 * Loads a folder of scripts.
		 * A new module is built, and each file is attached to it. This means all
		 * scripts aren't separate, so all functions across all scripts should have
		 * a unique name.\n
		 * Every time this method is called, the previous module is discarded. This
		 * means any functions registered in the old module can no longer be
		 * called. Note that the previous module is discarded regardless of the
		 * outcome of this method.
		 * @warning It is imperative that the interface is registered \em before
		 *          loading scripts.
		 * @param   folder The path containing all the script files to load. If
		 *                 blank, the last folder used with this method will be
		 *                 substituted.
		 * @return  \c TRUE if successful, \c FALSE if not.
		 * @sa      engine::scripts::registerInterface()
		 */
		bool loadScripts(std::string folder = "") noexcept;

		/**
		 * Retrieves the last path used with \c loadScripts().
		 * @return The path containing all the loaded scripts.
		 */
		const std::string& getScriptsFolder() const noexcept;

		/**
		 * Tests to see if a function with the given name exists in any of the
		 * loaded scripts.
		 * @return \c TRUE if the function exists, \c FALSE if not.
		 */
		bool functionExists(const std::string& name) const noexcept;

		/**
		 * Parameter pack method called when a script function requires parameters.
		 * It is called recursively, so that each parameter is added, with the
		 * first parameter being the first parameter passed to the function, etc.
		 * Upon the final call (no parameters left to add), the non-template
		 * version of this method is called.\n
		 * All primitive data types are supported (including \c bool, all integral
		 * types, \c float, and \c double). Objects are also supported, \b however,
		 * they \b must be passed in as a pointer, and the object's class \b must
		 * have its own \c fmt::formatter code (this is because this method expects
		 * your object type to be able to output to the log via spdlog, which uses
		 * fmt). Your object type must also be copyable. Expect seemingly unrelated
		 * [template] build errors if these conditions aren't met. Strings are also
		 * supported, \b however, string literals are not. This may change in the
		 * future.
		 * @tparam T      The type of the first parameter to add.
		 * @tparam Ts     The types of the next parameters to add, if any.
		 * @param  name   The name of the script function to call.
		 * @param  value  The first parameter to add.
		 * @param  values The next parameters to add, if any.
		 * @return \c FALSE if a parameter couldn't be added to the script function
		 *         call, or if the function context couldn't be setup, or if the
		 *         function couldn't be called. \c TRUE if the call to the function
		 *         was successful.
		 */
		template<typename T, typename... Ts>
		bool callFunction(const std::string& name, T value, Ts... values) noexcept;

		/**
		 * Calls a script function when no more parameters have to be added to the
		 * function call.
		 * @param  name The name of the script function to call.
		 * @return See the template version of \c callFunction().
		 */
		bool callFunction(const std::string& name) noexcept;
	private:
		/**
		 * Prepares the function context when making a call to \c callFunction().
		 * @param  name The name of the function to call.
		 * @return \c TRUE if the context was setup successfully, \c FALSE
		 *         otherwise.
		 */
		bool _setupContext(const std::string& name) noexcept;

		/**
		 * The internal logger object.
		 */
		mutable engine::logger _logger;

		/**
		 * The folder containing all the scripts loaded.
		 */
		std::string _scriptsFolder;

		/**
		 * Pointer to the script engine.
		 */
		asIScriptEngine* _engine = nullptr;

		/**
		 * Pointer to the function context, used to carry out script function
		 * calls.
		 */
		asIScriptContext* _context = nullptr;

		/**
		 * Flag used to keep track of whether the template version of
		 * \c callFunction() was called or not.
		 */
		bool _callFunction_TemplateCall = false;

		/**
		 * Keeps track of the number of arguments added when calling a script
		 * function.
		 */
		asUINT _argumentID = 0;
	};
}

template<typename T, typename... Ts>
bool engine::scripts::callFunction(const std::string& name, T value, Ts... values)
	noexcept {
	if (!_callFunction_TemplateCall) {
		// first call to the template version, setup the context
		if (!_setupContext(name)) return false;
		_callFunction_TemplateCall = true;
	}
	// add the parameter then call either the template version (if more parameters
	// remaining)
	// or the non-template version if there are no more paramters remaining to be
	// added (this is decided for us implicitly)
	int r = 0;
	if constexpr (std::is_integral<T>::value) {
		// this also conveniently covers bool for us
		switch (sizeof value) {
		case 1:
			r = _context->SetArgByte(_argumentID, (asBYTE)value);
			break;
		case 2:
			r = _context->SetArgWord(_argumentID, (asWORD)value);
			break;
		case 4:
			r = _context->SetArgDWord(_argumentID, (asDWORD)value);
			break;
		case 8:
			r = _context->SetArgQWord(_argumentID, (asQWORD)value);
			break;
		default:
			_logger.error("Unexpected length {} of integer variable {}, it will "
				"not be set to argument {} - behaviour is undefined after this "
				"point.", sizeof value, value, _argumentID);
			break;
		}
	} else if constexpr (std::is_floating_point<T>::value) {
		if (sizeof value == 4) {
			r = _context->SetArgFloat(_argumentID, (float)value);
		} else {
			r = _context->SetArgDouble(_argumentID, (double)value);
		}
	} else if constexpr (std::is_pointer<T>::value) {
		r = _context->SetArgObject(_argumentID, value);
	}
	if (r < 0) {
		if constexpr (std::is_pointer<T>::value) {
			_logger.error("Failed to set argument {} to the value \"{}\": code "
				"{}.", _argumentID, *value, r);
		} else {
			_logger.error("Failed to set argument {} to the value \"{}\": code "
				"{}.", _argumentID, value, r);
		}
		_argumentID = 0;
		_callFunction_TemplateCall = false;
		return false;
	}
	_argumentID++;
	return callFunction(name, values...);
}