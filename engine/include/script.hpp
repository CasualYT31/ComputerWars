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

/**@file script.hpp
 * Defines code realted to executing scripts on disc.
 * This implementation uses AngelScript.
 */

#pragma once

#include "angelscript.h"
#include "scriptbuilder.h"
#include "scriptstdstring.h"
#include "scriptdictionary.h"
#include "scriptarray.h"
#include "scriptfilesystem.h"
#include "scripthelper.h"
#include "scriptany.h"
#include "logger.hpp"
#include <type_traits>
#include "docgen.h"

namespace engine {
	/**
	 * Registers and documents the \c Colour script type, if it hasn't already been
	 * registered.
	 */
	void RegisterColourType(asIScriptEngine* engine,
		const std::shared_ptr<DocumentationGenerator>& document) noexcept;

	/**
	 * Interface which allows a subclass to register functions, object types, etc.
	 * with a \c scripts object.
	 */
	class script_registrant {
	public:
		/**
		 * Polymorphic base classes should have virtual destructors.
		 */
		virtual ~script_registrant() noexcept = default;

		/**
		 * Adds to the interface between a \c scripts object and the game engine.
		 * See <a
		 * href="https://www.angelcode.com/angelscript/sdk/docs/manual/classas_i_script_engine.html"
		 * target="_blank">the documentation on asIScriptEngine</a> for help on how
		 * to register the interface.
		 * @param engine   Pointer to the AngelScript script engine to register
		 *                 with.
		 * @param document Pointer to the AngelScript documentation generator to
		 *                 register script interface documentation with.
		 */
		virtual void registerInterface(asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>& document) noexcept = 0;
	};

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
		 * Adds a script interface registrant to the list.
		 * Upon the first call to @c loadScripts(), the interface between the game
		 * engine and the scripts is registered. In order to achieve this,
		 * @c scripts needs access to the different classes that will contain the
		 * functionality that needs to be registered with the script engine so that
		 * scripts can access that functionality. By giving a list of registrants
		 * before calling @c loadScripts(), you can tell @c scripts exactly which
		 * objects to invoke the methods of.\n
		 * After the call to @c loadScripts(), the internal list of registrants
		 * will be cleared. This is because once the interface has been registered,
		 * it does not need to be registered again.
		 * @warning You must ensure that the registrants you give remain alive for
		 *          the lifetime of the @c scripts instance, if you have registered
		 *          a class' methods with the interface.
		 * @param   r Pointer to the script registrant to add. If @c nullptr is
		 *            provided, it won't be added and an error will be logged.
		 */
		void addRegistrant(engine::script_registrant* const r)
			noexcept;

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
		 * Used to translate application exceptions into AngelScript exceptions.
		 * @param context The pointer to the function context which has encountered
		 *                an application exception during runtime.
		 */
		void translateExceptionCallback(asIScriptContext* context, void*) noexcept;

		/**
		 * Loads a folder of scripts recursively.
		 * Note that this function expects every file within the folder
		 * (recursively) to be a valid AngelScript file. If one file couldn't be
		 * loaded, this function will return @c FALSE.\n
		 * A new module is built, and each file is attached to it. This means all
		 * scripts aren't separate, so all functions across all scripts should have
		 * a unique name.\n
		 * Every time this method is called, the previous module is discarded. This
		 * means any functions registered in the old module can no longer be
		 * called. Note that the previous module is discarded regardless of the
		 * outcome of this method.
		 * @warning It is imperative that the interface is registered \em before
		 *          loading scripts by adding registrants!
		 * @param   folder The path containing all the script files to load. If
		 *                 blank, the last folder used with this method will be
		 *                 substituted.
		 * @return  \c TRUE if successful, \c FALSE if not. Note that this method
		 *          returns \c TRUE even if the given folder did not exist or could
		 *          not be read.
		 * @sa      engine::scripts::addRegistrant()
		 */
		bool loadScripts(std::string folder = "") noexcept;

		/**
		 * Generate the documentation for this @c scripts instance.
		 * Can only be called after at least one call to @c loadScripts(), since
		 * the script interface is only registered in that method.
		 * @return The result of the generation operation. @c INT_MIN if the
		 *         document generator object couldn't be initialised in the
		 *         constructor (this is also logged).
		 */
		int generateDocumentation() noexcept;

		/**
		 * Retrieves the last path used with \c loadScripts().
		 * Even if the path did not exist, it will be internally assigned and
		 * returned here.
		 * @return The path containing all the loaded scripts.
		 */
		const std::string& getScriptsFolder() const noexcept;

		/**
		 * Tests to see if a function with the given name exists in any of the
		 * loaded scripts.
		 * @param  name The name of the function.
		 * @return \c TRUE if the function exists, \c FALSE if not, or if there
		 *         were multiple matches.
		 */
		bool functionExists(const std::string& name) const noexcept;

		/**
		 * Tests to see if a function with the given declaration exists in any of
		 * the loaded scripts.
		 * @param  name The declaration of the function.
		 * @return \c TRUE if the function exists, \c FALSE otherwise.
		 */
		bool functionDeclExists(const std::string& decl) const noexcept;

		/**
		 * Will write a message to the log, using the current context to retrieve
		 * extra information.
		 * @param message The message to log.
		 */
		void writeToLog(const std::string& message) const noexcept;

		/**
		 * Will write a warning to the log, using the current context to retrieve
		 * extra information.
		 * @param message The message to log.
		 */
		void warningToLog(const std::string& message) const noexcept;
		
		/**
		 * Will write an error to the log, using the current context to retrieve
		 * extra information.
		 * @param message The message to log.
		 */
		void errorToLog(const std::string& message) const noexcept;

		/**
		 * Parameter pack method called when a script function requires parameters.
		 * It is called recursively, so that each parameter is added, with the
		 * first parameter being the first parameter passed to the function, etc.
		 * Upon the final call (no parameters left to add), the non-template
		 * version of this method is called.\n
		 * All primitive data types are supported (including \c bool, all integral
		 * types, \c float, and \c double). Objects are also supported, \b however,
		 * they \b must be passed in as a raw pointer, and the object's class
		 * \b must have its own \c fmt::formatter code (this is because this method
		 * expects your object type to be able to output to the log via spdlog,
		 * which uses fmt). Your object type must also be copyable. Expect
		 * seemingly unrelated [template] build errors if these conditions aren't
		 * met. Strings are also supported, \b however, string literals are not.
		 * This may change in the future.
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

		/**
		 * Creates a @c CScriptDictionary object.
		 * @return Pointer to a new AngelScript @c dictionary object that's been
		 *         registered with this engine.
		 */
		CScriptDictionary* createDictionary() noexcept;

		/**
		 * Creates a @c CScriptArray object.
		 * @param  type The name of the type to give as the array's type.
		 * @return Pointer to a new AngelScript @c array object, or \c nullptr if
		 *         it could not be created.
		 */
		CScriptArray* createArray(const std::string& type) const noexcept;
	private:
		/**
		 * Allocates a new function context.
		 * @return The error code.
		 */
		int _allocateContext() noexcept;

		/**
		 * Prepares the function context when making a call to \c callFunction().
		 * @param  name The name of the function to call.
		 * @return \c TRUE if the context was setup successfully, \c FALSE
		 *         otherwise.
		 */
		bool _setupContext(const std::string& name) noexcept;

		/**
		 * Resets \c callFunction() variables.
		 * Call this method just before a call to \c callFunction() is over.
		 */
		void _resetCallFunctionVariables() noexcept;

		/**
		 * Constructs a log message from the most current context.
		 * @param msg The message to log.
		 */
		std::string _constructMessage(const std::string& msg) const noexcept;

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
		 * Pointers to the function contexts, used to carry out script function
		 * calls.
		 */
		std::vector<asIScriptContext*> _context;

		/**
		 * Keeps track of which context object to use when making a function call.
		 */
		std::size_t _contextId = 0;

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

		/**
		 * The list of registrants.
		 */
		std::vector<engine::script_registrant*> _registrants;

		/**
		 * Used to generate documentation on the script interface.
		 */
		std::shared_ptr<DocumentationGenerator> _document;
	};
}

#include "tpp/script.tpp"