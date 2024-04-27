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

/**@file script.hpp
 * Defines code related to executing scripts on disc.
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
#include "scriptmath.h"
#include "scripthandle.h"
#include "weakref.h"
#include "logger.hpp"
#include "safejson.hpp"
#include "maths.hpp"
#include "binary.hpp"
#include <type_traits>
#include "SFML/Graphics/Color.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/System/Clock.hpp"
#include "SFML/Graphics/Rect.hpp"
#include <unordered_set>
#include "docgen.h"

namespace engine {
	/**
	 * Registers and documents the \c Colour script type, if it hasn't already been
	 * registered.
	 * @safety No guarantee.
	 */
	void RegisterColourType(asIScriptEngine* engine,
		const std::shared_ptr<DocumentationGenerator>& document);

	/**
	 * Registers and documents the \c Vector2 and \c MousePosition types, if they
	 * haven't already been registered.
	 * @safety No guarantee.
	 */
	void RegisterVectorTypes(asIScriptEngine* engine,
		const std::shared_ptr<DocumentationGenerator>& document);

	/**
	 * Registers and documents the \c IntRect type, if it hasn't already been
	 * registered.
	 * @safety No guarantee.
	 */
	void RegisterRectTypes(asIScriptEngine* engine,
		const std::shared_ptr<DocumentationGenerator>& document);

	/**
	 * Registers and documents the \c Time and \c Clock types, if they haven't
	 * already been registered.
	 * @safety No guarantee.
	 */
	void RegisterTimeTypes(asIScriptEngine* engine,
		const std::shared_ptr<DocumentationGenerator>& document);

	/**
	 * Registers and documents the \c BinaryIStream and \c BinaryOStream types, if
	 * they haven't already been registered.
	 * @safety No guarantee.
	 */
	void RegisterStreamTypes(asIScriptEngine* engine,
		const std::shared_ptr<DocumentationGenerator>& document);

	/**
	 * This constant expression is used to alert the programmer that the type's
	 * corresponding AngelScript type hasn't been configured.
	 * All types that are registered with the script interface should ideally have
	 * their corresponding AngelScript typename configured for use throughout the
	 * engine. This involves specialising this function, within the \c engine
	 * namespace, to return the name of the C++ type in AngelScript, without
	 * qualifiers. If the engine code compilation results in this error, you'll
	 * need to specialise for the type you're trying to work with.\n
	 * If you wish to specialise an STL type, or a type otherwise outside of the
	 * Computer Wars codebase (e.g. in a dependency), please do so within
	 * \c script.tpp. If a non-engine level dependency needs to define their AS
	 * type, introduce a new *.tpp file and include that in the header that makes
	 * the most sense.
	 * @tparam T The type to retrieve the AngelScript type of.
	 * @return The AngelScript type of the C++ type \c T.
	 */
	template<typename T>
	inline constexpr std::string script_type() {
		static_assert(false, "You must specialise engine::script_type<T>()");
	}

	/**
	 * Automatically determines the best AngelScript type qualifiers for the given
	 * type if used as a constant input parameter.
	 * @tparam T The type of the parameter.
	 * @return The full AngelScript type for the parameter.
	 */
	template<typename T>
	constexpr std::string script_param_type();

	/**
	 * No more parameters to generate.
	 * @tparam N     The number of custom AngelScript types to insert.
	 * @tparam C     Which custom AngelScript type to insert next.
	 * @tparam COMMA Should a comma prepend the string generated in this call?
	 * @param  customParams The custom AngelScript types to insert into the
	 *                      generated string.
	 * @return An empty string.
	 * @sa     \c params_builder().
	 */
	template<std::size_t N, std::size_t C, bool COMMA>
	constexpr std::string params_builder(
		const std::array<const char*, N>& customParams);

	/**
	 * Generate the next AngelScript parameter in a parameter list for a function
	 * signature.
	 * @tparam N     The number of custom AngelScript types to insert.
	 * @tparam C     Which custom AngelScript type to insert next.
	 * @tparam COMMA Should a comma prepend the string generated in this call?
	 * @tparam T     The next C++ type to be converted into an AngelScript constant
	 *               input parameter type. If \c void, the next custom type will be
	 *               inserted instead.
	 * @tparam Ts    The next C++ types in the list, if any.
	 * @param  customParams The custom AngelScript types to insert into the
	 *                      generated string.
	 * @return \c T as an AngelScript type, with the rest of the parameters
	 *         appended.
	 */
	template<std::size_t N, std::size_t C, bool COMMA, typename T, typename... Ts>
	constexpr std::string params_builder(
		const std::array<const char*, N>& customParams);

	/**
	 * Build a signature for an AngelScript function that includes custom types
	 * without \c script_param_type specialisations.
	 * @tparam N  The number of custom AngelScript types to insert.
	 * @tparam Ts The C++ types to insert into the signature, in order. Use \c void
	 *            to represent each custom type.
	 * @param  funcName     The name of the function.
	 * @param  customParams The custom AngelScript types to insert into the
	 *                      generated string.
	 * @return The function's full AngelScript signature.
	 */
	template<std::size_t N, typename... Ts>
	constexpr std::string sig_builder(const std::string& funcName,
		const std::array<const char*, N>& customParams,
		const std::string& retType = "void");

	/**
	 * Build a signature for an AngelScript function that accepts C++ types.
	 * Each C++ type given must have \c script_param_type specialisation. Do not
	 * provide const qualifiers, as these will be automatically included in the
	 * AngelScript signature.
	 * @tparam Ts The C++ types to insert into the signature, in order.
	 * @param  funcName The name of the function.
	 * @return The function's full AngelScript signature.
	 */
	template<typename... Ts>
	constexpr std::string sig_builder(const std::string& funcName,
		const std::string& retType = "void");

	/**
	 * Base class for AngelScript reference types.
	 * @tparam T A subclass should pass their class name in to this parameter.
	 */
	template<typename T>
	class script_reference_type {
	public:
		/**
		 * Polymorphic base classes must have virtual destructors.
		 */
		virtual ~script_reference_type() noexcept = default;

		/**
		 * Copies a reference to the object.
		 */
		void AddRef() const noexcept;

		/**
		 * Releases a reference to the object.
		 * When the reference counter hits \c 0, the object will be deleted.
		 */
		void Release() const;
	protected:
		/**
		 * One cannot instantiate this class by itself.
		 */
		script_reference_type() = default;

		/**
		 * Registers the reference type with a given script engine and registers
		 * the factory and reference counting behaviours.
		 * @param  engine          Pointer to the script engine to register with.
		 * @param  type            The name of the type to register.
		 * @param  registerFactory Callback used to register the factory function
		 *                         behaviour/s of this type. Your reference type
		 *                         must define its own factory function/s.
		 * @return The result of the \c RegisterObjectType() method.
	     * @safety No guarantee.
		 */
		static int RegisterType(asIScriptEngine* engine, const std::string& type,
			const std::function<void(asIScriptEngine*, const std::string&)>&
				registerFactory);
	private:
		/**
		 * The reference counter.
		 */
		mutable asUINT _refCount = 1;
	};

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
			const std::shared_ptr<DocumentationGenerator>& document) = 0;
	};

	/**
	 * Class used to automatically handle reference counting of AngelScript
	 * objects.
	 * @tparam The AngelScript object type to handle.
	 */
	template<typename T>
	class CScriptWrapper {
	public:
		/**
		 * Initialises the wrapper object with no object.
		 */
		CScriptWrapper() = default;

		/**
		 * Initialises the wrapper object with an existing AngelScript object.
		 */
		CScriptWrapper(T* const obj);

		/**
		 * Copies the pointer and increases its reference count.
		 */
		CScriptWrapper(const CScriptWrapper<T>& obj);

		/**
		 * Moves the pointer over and increases the reference count.
		 * Even though it's a move, we are technically creating another reference
		 * to the object, so increase the reference count anyway. If you remove
		 * this, \c emplace_back() and other "move" calls will cause the reference
		 * counter to fall down by one, which will cause a nasty crash later on at
		 * whatever point.
		 */
		CScriptWrapper(CScriptWrapper<T>&& obj) noexcept;

		/**
		 * Releases the reference to the stored AngelScript object.
		 */
		~CScriptWrapper() noexcept;

		/**
		 * Allows direct access to the stored AngelScript object.
		 * @return Pointer to the AngelScript object.
		 */
		T* operator->() const noexcept;
	private:
		/**
		 * The AngelScript object.
		 */
		T* _ptr = nullptr;
	};

	/**
	 * Okay tbh idk wtf this does but it should work.
	 * @sa <a href="https://stackoverflow.com/a/19392596" target="_blank">
	 *     Credit.</a>
	 */
	template<typename C, typename = void>
	struct has_reserve : std::false_type {};

	/**
	 * Sees if a type has the correct \c reserve() method available by attempting
	 * to call it.
	 * @sa <a href="https://stackoverflow.com/a/19392596" target="_blank">
	 *     Credit.</a>
	 */
	template<typename C>
	struct has_reserve<C, std::enable_if_t<std::is_same<decltype(
		std::declval<C>().reserve(std::declval<typename C::size_type>())),
		void>::value>> : std::true_type {};

	/**
	 * Attempts to reserve space in a container.
	 * Since this container cannot reserve space, don't do anything.
	 * @sa <a href="https://stackoverflow.com/a/19392596" target="_blank">
	 *     Credit.</a>
	 */
	template<typename C> std::enable_if_t<!has_reserve<C>::value>
		AttemptToReserve(C& c, const std::size_t n) {}

	/**
	 * Attempts to reserve space in a container.
	 * Since this container can reserve space, invoke \c reserve() on it.
	 * @sa <a href="https://stackoverflow.com/a/19392596" target="_blank">
	 *     Credit.</a>
	 */
	template<typename C> std::enable_if_t<has_reserve<C>::value>
		AttemptToReserve(C& c, const std::size_t n) { c.reserve(c.size() + n); }

	/**
	 * Converts a \c CScriptArray into an STL container and releases the array.
	 * @tparam T The type of container to convert the array into.
	 * @tparam U The type of elements stored in the container.
	 * @param  a The \c CScriptArray to convert.
	 * @return The converted array. An empty container if \c a is \c nullptr.
	 */
	template<typename T, typename U>
	T ConvertCScriptArray(const CScriptArray* const a);

	/**
	 * Class representing a layer of abstraction between a script engine and the
	 * client.
	 * A folder of scripts is loaded from disc. This class can then be used to call
	 * functions from these scripts, given the name of the function and a variety
	 * of parameters.
	 */
	class scripts : public engine::json_script {
	public:
		/**
		 * Represents a collection of script files.
		 * The key stores the name of the file, and the value stores the contents
		 * of the file.
		 */
		typedef std::unordered_map<std::string, std::string> files;

		/**
		 * The core modules that the scripts engine expects.
		 */
		static const std::array<const std::string, 2> modules;

		/**
		 * The index pointing to the \c modules entry that stores the name of the
		 * main module.
		 */
		static constexpr std::size_t MAIN = 0;

		/**
		 * The index pointing to the \c modules entry that stores the name of the
		 * module containing all of the bank override code.
		 */
		static constexpr std::size_t BANK_OVERRIDE = 1;

		/**
		 * Is the given string the name of a core module?
		 * @param  name The string to test.
		 * @return \c TRUE if the given string identifies a core module, \c FALSE
		 *         otherwise.
		 */
		inline static constexpr bool IsCoreModule(const std::string& name) {
			return
				std::find(modules.begin(), modules.end(), name) != modules.end();
		}

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
		 * @param data The data to initialise the logger object with.
		 * @sa    \c engine::logger
		 * @sa    \c engine::scripts::registerInterface()
		 */
		scripts(const engine::logger::data& data);

		/**
		 * Releases all the function contexts and shuts down the engine.
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
		 * it does not need to be registered again.\n
		 * For classes that need to register themselves with the script interface,
		 * but that aren't instantiated at the time of calling this method, define
		 * a function with the same signature as
		 * \c script_registrant::registerInterface() that registers the class with
		 * the interface, e.g. as an object type, and call this function when
		 * necessary in \c registerInterface() methods. Your functions should check
		 * if the type has already been added before attempting to add the type!
		 * @warning You must ensure that the registrants you give remain alive for
		 *          the lifetime of the @c scripts instance, if you have registered
		 *          a class' methods with the interface.
		 * @param   r Pointer to the script registrant to add. If @c nullptr is
		 *            provided, it won't be added and an error will be logged.
		 * @safety  Strong guarantee.
		 */
		void addRegistrant(engine::script_registrant* const r);

		/**
		 * The message callback assigned to the script engine.
		 * This callback mirrors messages to the log file.
		 * @param  msg   The object containing the message information to output.
		 * @param  param Ignored.
		 * @safety Basic guarantee.
		 */
		void scriptMessageCallback(const asSMessageInfo* msg, void* param);

		/**
		 * The runtime error callback assigned to the function context.
		 * @param  context The pointer to the function context which has
		 *                 encountered an error during runtime.
		 * @safety No guarantee. Unsure if \c asIScriptContext methods can throw or
		 *         not, and if they can, what those methods guarantee. Cba finding
		 *         out.
		 */
		void contextExceptionCallback(asIScriptContext* context);

		/**
		 * Used to translate application exceptions into AngelScript exceptions.
		 * @param context The pointer to the function context which has encountered
		 *                an application exception during runtime.
		 */
		void translateExceptionCallback(asIScriptContext* context, void*) noexcept;

		/**
		 * Generate the documentation for this @c scripts instance.
		 * Can only be called after at least one call to @c loadScripts(), since
		 * the script interface is only registered in that method.
		 * @return The result of the generation operation. @c INT_MIN if the
		 *         document generator object couldn't be initialised in the
		 *         constructor (this is also logged).
		 * @safety No guarantee.
		 */
		int generateDocumentation();

		/**
		 * Tests to see if a function with the given name exists in the specified
		 * module.
		 * @param  module The name of the module that is to contain the function.
		 * @param  name   The name of the function.
		 * @return \c TRUE if the module and function exists, \c FALSE if not, or
		 *         if there were multiple function matches.
		 * @safety No guarantee.
		 */
		bool functionExists(const std::string& module,
			const std::string& name) const;

		/**
		 * Tests to see if a function with the given declaration exists in the
		 * specified module.
		 * @param  module The module to search in.
		 * @param  decl   The declaration of the function.
		 * @return \c TRUE if the module and function exists, \c FALSE otherwise.
		 * @safety No guarantee.
		 */
		bool functionDeclExists(const std::string& module,
			const std::string& decl) const;

		/**
		 * Will write a message to the log, using the current context to retrieve
		 * extra information.
		 * @param  message The message to log.
		 * @safety No guarantee.
		 */
		void writeToLog(const std::string& message) const;

		/**
		 * Will write a warning to the log, using the current context to retrieve
		 * extra information.
		 * @param  message The message to log.
		 * @safety No guarantee.
		 */
		void warningToLog(const std::string& message) const;
		
		/**
		 * Will write an error to the log, using the current context to retrieve
		 * extra information.
		 * @param  message The message to log.
		 * @safety No guarantee.
		 */
		void errorToLog(const std::string& message) const;

		/**
		 * Will write a critical error to the log, using the current context to
		 * retrieve extra information.
		 * @param  message The message to log.
		 * @safety No guarantee.
		 */
		void criticalToLog(const std::string& message) const;

		/**
		 * Will write a complete C++ and AngelScript stacktrace to the log.
		 * @safety No guarantee.
		 */
		void stacktraceToLog() const;

		/**
		 * Version of \c callFunction() which accepts a name to an existing script
		 * function.
		 * If the function didn't exist, or there was more than one function with
		 * the given name, or the given module didn't exist, then this method will
		 * fail.
		 * @tparam Ts     The types of the parameters to pass to the function, if
		 *                any.
		 * @param  module The name of the module which contains the named function.
		 * @param  name   The name of the function written in the scripts to call.
		 * @param  values The parameters to provide to the function call, if any.
		 * @return See the other template version of \c callFunction().
		 * @safety No guarantee.
		 */
		template<typename... Ts>
		bool callFunction(const std::string& module, const std::string& name,
			Ts... values);

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
		 * @tparam T      The type of the first parameter to add.
		 * @tparam Ts     The types of the next parameters to add, if any.
		 * @param  func   Pointer to the script function to call.
		 * @param  value  The first parameter to add.
		 * @param  values The next parameters to add, if any.
		 * @return \c FALSE if a parameter couldn't be added to the script function
		 *         call, or if the function context couldn't be setup, or if the
		 *         function couldn't be called. \c TRUE if the call to the function
		 *         was successful.
		 * @safety No guarantee.
		 */
		template<typename T, typename... Ts>
		bool callFunction(asIScriptFunction* const func, T value, Ts... values);

		/**
		 * Calls a script function when no more parameters have to be added to the
		 * function call.
		 * @param  func Pointer to the script function to call.
		 * @return See the template version of \c callFunction().
		 * @safety No guarantee.
		 */
		bool callFunction(asIScriptFunction* const func);

		/**
		 * Invokes a method on an object.
		 * @tparam Ts         The types of the parameters to add to the call.
		 * @param  obj        Pointer to the object whose method is to be called.
		 * @param  methodDecl The declaration of the method to invoke.
		 * @param  values     The parameters to add to the call, if any.
		 * @return See the template version of \c callFunction(). It will also fail
		 *         if the given object pointer is \c nullptr, or if the given
		 *         method declaration couldn't identify a method in the object.
		 */
		template<typename... Ts>
		bool callMethod(asIScriptObject* const obj, const std::string& methodDecl,
			Ts... values);

		/**
		 * Compiles and executes the given code, which can call any code that is in
		 * the given module.
		 * Wish I had known about \c ExecuteString() before I wrote this function.
		 * @param  code       The code to execute. It is automatically put into a
		 *                    function so you don't have to do this maually in the
		 *                    code.
		 * @param  moduleName The module to execute the code within. Defaults to
		 *                    \c MAIN.
		 * @return If execution was successful, an empty string is returned. If
		 *         not, an error strng will be returned. The error string will
		 *         also be logged. These include build errors and exception errors.
		 * @safety No guarantee.
		 */
		std::string executeCode(std::string code, std::string moduleName = "");

		/**
		 * Creates a @c CScriptDictionary object.
		 * @return Pointer to a new AngelScript @c dictionary object that's been
		 *         registered with this engine.
		 * @safety No guarantee.
		 */
		CScriptDictionary* createDictionary();

		/**
		 * Creates a @c CScriptArray object.
		 * @param  type The name of the type to give as the array's type.
		 * @return Pointer to a new AngelScript @c array object, or \c nullptr if
		 *         it could not be created.
		 * @safety No guarantee.
		 */
		CScriptArray* createArray(const std::string& type) const;

		/**
		 * Creates a @c CScriptArray object and fills it with the contents of a
		 * given STL container.
		 * @tparam T    The type of container to copy the elements of.
		 * @param  type The name of the type to give as the array's type.
		 * @param  stl  The STL container whose items should be added to the array.
		 * @return Pointer to a new AngelScript @c array object, or @c nullptr if
		 *         it could not be created.
		 */
		template<typename T>
		CScriptArray* createArrayFromContainer(const std::string& type,
			T& stl) const;

		/**
		 * Creates a @c CScriptAny object.
		 * @warning You must remember to \c delete or \c Release() the returned
		 *          pointer!
		 * @return  Pointer to a new AngelScript @c any object that's been
		 *          registered with this engine.
		 * @safety  No guarantee.
		 */
		CScriptAny* createAny() const;

		/**
		 * Creates a script object.
		 * Currently this method can only be used with classes defined in the main
		 * module that have a factory function that accepts no parameters.
		 * @param  type The name of the type of script object to instantiate. This
		 *              type is defined by the scripts themselves.
		 * @return Pointer to the new AngelScript object, with its refcount already
		 *         initialised appropriately. \c nullptr if the object could not be
		 *         created (the reason why will be logged).
		 */
		asIScriptObject* createObject(const std::string& type);

		/**
		 * Returns the type ID of a given type.
		 * @param  type The name of the type.
		 * @return The type ID, or \c -1 if there was an error.
		 * @safety No guarantee.
		 */
		int getTypeID(const std::string& type) const;

		/**
		 * Returns the name of a given type.
		 * @param  id The type ID of the type to query.
		 * @return The name of the type, or a blank string if there was an error.
		 * @safety No guarantee.
		 */
		std::string getTypeName(const int id) const;

		/**
		 * Returns a list of names of types in the given module that implement a
		 * given interface.
		 * @param  moduleName    The name of the module to search in.
		 * @param  interfaceName The name of the interface that is used when
		 *                       searching through all of the registered and
		 *                       declared types.
		 * @return A list of names of types that implement a given interface. If
		 *         there are none, or if the interface does not exist, then the
		 *         returned list will be empty.
		 */
		std::vector<std::string> getConcreteClassNames(
			const std::string& moduleName, const std::string& interfaceName) const;

		/**
		 * Creates a new module, or replaces an existing one.
		 * This method will fail in the following circumstances:
		 * <ol><li>If \c name is within \c modules. The core modules are managed
		 *     separately and so cannot be amended using this method.</li>
		 *     <li>If the code given couldn't be built or added.</li>
		 *     <li>If the given module name contains \c ~.</li></ol>
		 * If a failure occurs, information will be logged as to why.
		 * @param  name        The name of the module.
		 * @param  code        The code to build and store in the new module.
		 * @param  errorString If an error occurred, a string describing the error
		 *                     will be stored here.
		 * @return \c TRUE if the module could be built and added/updated, \c FALSE
		 *         otherwise.
		 */
		bool createModule(const std::string& name, const files& code,
			std::string& errorString);

		/**
		 * Deletes a non-core module.
		 * @param  name The name of the module to discard.
		 * @return \c TRUE if the module could be discarded, \c FALSE if not (the
		 *         reason why will be logged).
		 */
		bool deleteModule(const std::string& name);

		/**
		 * Does a module with the given name exist?
		 * @param  name The name to test.
		 * @return \c TRUE if a module exists with the given name, \c FALSE if not.
		 */
		bool doesModuleExist(const std::string& name) const;

		/**
		 * Stores a global function or variable's declaration and metadata.
		 */
		struct global_object {
			/**
			 * Declaration of the function or variable.
			 */
			std::string declaration;

			/**
			 * The metadata.
			 */
			std::vector<std::string> metadata;
		};

		/**
		 * A collection of global functions that have metadata.
		 */
		using global_function_metadata =
			std::unordered_map<asIScriptFunction*, global_object>;

		/**
		 * A collection of global variables that have metadata.
		 */
		using global_variable_metadata = std::unordered_map<asUINT, global_object>;

		/**
		 * Retrieves a copy of each global function's metadata within a given
		 * module.
		 * @param  moduleName The name of the module to search in.
		 * @return A container of global functions within the given module that
		 *         have metadata. Empty if the given module does not exist.
		 */
		global_function_metadata getGlobalFunctionMetadata(
			const std::string& moduleName) const;

		/**
		 * Retrieves a copy of each global variable's metadata within a given
		 * module.
		 * @param  moduleName The name of the module to search in.
		 * @return A container of global variables within the given module that
		 *         have metadata. Empty if the given module does not exist.
		 */
		global_variable_metadata getGlobalVariableMetadata(
			const std::string& moduleName) const;

		/**
		 * A collection of global functions that key to a list of namespaces.
		 */
		using global_functions_and_their_namespaces =
			std::unordered_map<asIScriptFunction*, std::vector<std::string>>;

		/**
		 * A collection of global variables that key to a list of namespaces.
		 */
		using global_variables_and_their_namespaces =
			std::unordered_map<asUINT, std::vector<std::string>>;

		/**
		 * Retrieves a map of global functions that key to a list of strings, each
		 * string containing the name of a namespace that the function is within.
		 * The list begins with the outermost namespace and finishes with the
		 * innermost namespace. If a function is not within a namespace, the list
		 * will be empty but the function will still be included in the map.
		 * @param  moduleName The name of the module to search in.
		 * @return A list of global functions and their namespaces. Empty if the
		 *         given module doesn't exist.
		 */
		global_functions_and_their_namespaces getGlobalFunctionsAndTheirNamespaces(
			const std::string& moduleName) const;

		/**
		 * Retrieves a map of global variables that key to a list of strings, each
		 * string containing the name of a namespace that the function is within.
		 * The list begins with the outermost namespace and finishes with the
		 * innermost namespace. If a variable is not within a namespace, the list
		 * will be empty but the variable will still be included in the map.
		 * @param  moduleName The name of the module to search in.
		 * @return A list of global variables and their namespaces. Empty if the
		 *         given module doesn't exist.
		 */
		global_variables_and_their_namespaces getGlobalVariablesAndTheirNamespaces(
			const std::string& moduleName) const;

		/**
		 * Retrieves a module's global variable's name and type ID.
		 * @param  moduleName The name of the module containing the variable.
		 * @param  variable   The ID of the global variable to query.
		 * @param  name       If the variable's name could be retrieved, it will be
		 *                    assigned to this parameter.
		 * @param  typeID     If the variable's type ID could be retrieved, it will
		 *                    be assigned to this parameter.
		 * @return \c TRUE if the call succeeded, \c FALSE otherwise.
		 */
		bool getGlobalVariable(const std::string& moduleName,
			const asUINT variable, std::string& name, int& typeID) const;

		/**
		 * Retrieves the address to a module's global variable.
		 * @param  moduleName The name of the module containing the variable.
		 * @param  variable   The ID of the global variable to query.
		 * @return Address to the variable, or \c nullptr if it couldn't be
		 *         accessed.
		 */
		void* getGlobalVariableAddress(const std::string& moduleName,
			const asUINT variable) const;

		/**
		 * Evaluates each module's registered assertions.
		 * The evaluation of assertions was separated from \c load() because some
		 * assertions may rely on a script interface that may not be fully
		 * initialised at the time \c load() is called.
		 * @return \c TRUE if every assertion in every module passed, \c FALSE if
		 *         at least one failed (no further assertions will be evaluated in
		 *         this case).
		 */
		bool evaluateAssertions();
	private:
		/**
		 * The JSON load method for this class.
		 * Within the root object, there are simply a list of key-string pairs,
		 * with the keys defining the names of the modules the engine creates, and
		 * the string values containing the paths to the folders to pass to the
		 * \c _loadScripts() method.
		 * @warning It is imperative that the interface is registered \em before
		 *          loading scripts by adding registrants!
		 * @param   j The \c engine::json object representing the contents of the
		 *            loaded script which this method reads.
		 * @return  \c TRUE if every module could be loaded successfully, \c FALSE
		 *          if even one of the \c _loadScripts() calls returned \c FALSE.
		 * @safety  No guarantee.
		 * @sa      \c engine::scripts::modules
		 */
		bool _load(engine::json& j);

		/**
		 * Loads a folder of scripts recursively into a given module.
		 * Note that this function expects every file within the folder
		 * (recursively) to be a valid AngelScript file. If one file couldn't be
		 * loaded, this function will return @c FALSE.\n
		 * A new module with the given name is built, and each file is attached to
		 * it. This means all scripts within a module aren't separate, so all
		 * functions across all scripts within the folder should have a unique
		 * name.\n
		 * Every time this method is called, the previous module with the given
		 * name is discarded. This means any functions registered in the old module
		 * can no longer be called. Note that the previous module is discarded
		 * regardless of the outcome of this method.
		 * @param   folder The path containing all the script files to load.
		 * @return  \c TRUE if successful, \c FALSE if not. Note that this method
		 *          returns \c TRUE even if the given folder did not exist or could
		 *          not be read.
		 * @safety  No guarantee.
		 * @sa      engine::scripts::addRegistrant()
		 */
		bool _loadScripts(const char* const moduleName, const std::string& folder);

		/**
		 * Iterates through the templates queue, instantiates each of them, then
		 * attempts to add each instantiated template to the builder.
		 * It will empty the queue as it processes each request.
		 * @return \c FALSE if an instantiated template couldn't be added to the
		 *         module. \c TRUE if all of the instantiated templates were
		 *         well-formed.
		 */
		bool _instantiateTemplatesInQueue();

		/**
		 * Performs basic string substitution and generation on a given script
		 * template based on a list of parameters, and attempts to add the
		 * resulting script to the builder after performing preprocessing.
		 * @param  templateName   The name of the template.
		 * @param  templateScript The template script to expand/instantiate.
		 * @param  parameters     The parameters to instantiate with.
		 * @return \c FALSE if the instantiated template couldn't be added to the
		 *         module, \c TRUE if it could.
		 */
		bool _instantiateTemplate(const std::string& templateName,
			const std::string& templateScript,
			const std::vector<std::string>& parameters);

		/**
		 * Performs normal substitution on the given section.
		 * Every occurrence of \c $ will be replaced with a value from
		 * \c parameters, based on the number that immediately follows the
		 * symbol.\n
		 * If it is \c 0, the number of parameters will be substituted. If it's
		 * \c > 0, the value of the corresonding parameter will be substituted. If
		 * no number was given, or the number was invalid, a warn or error will be
		 * logged, and the $ with its number will be inserted into the final
		 * result.
		 * @param  section    The code to perform substitution on.
		 * @param  parameters The parameters to subtitute into the section.
		 * @return The result of inserting \c parameters into \c section.
		 */
		std::string _normalSubstitution(const std::string& section,
			const std::vector<std::string>& parameters);

		/**
		 * 
		 */
		void _normalSubstitution_parseNormalChar(const char chr,
			bool& readingParam, std::string& number, std::string& result);

		/**
		 * 
		 */
		void _normalSubstitution_substituteParameter(const char* chr,
			bool& readingParam, std::string& number, std::string& result,
			const std::vector<std::string>& parameters);

		/**
		 * Reads an integer from a given string.
		 * All whitespace will be removed from \c from before conversion.\n
		 * If the string is equal to '$', then <tt>parameters.size() + 1</tt> will
		 * be returned.\n
		 * If an integer couldn't be extracted, then an error is logged and \c 0 is
		 * returned.
		 * @param  from       Extract an integer from this string.
		 * @param  parameters Used to retrieve the number of parameters.
		 * @param  which      Used for logging: describes how the integer will be
		 *                    used.
		 * @return The extracted integer.
		 */
		sf::Int64 _readInt(std::string from,
			const std::vector<std::string>& parameters, const char* const which);

		/**
		 * Performs normal substitution on \c section as many times as \c start,
		 * \c stop, and \c step allow, with the results of each concatenated
		 * together.
		 * On each iteration, instances of '$i' or '$I' are replaced with whatever
		 * the loop index is. Then, normal substituion occurs.
		 * @param  section    The section of code to continuously insert.
		 * @param  endSection The second part of the code to continuously insert
		 *                    after the main \c section. Except on the last
		 *                    iteration, \c endSection will not be inserted.
		 * @param  parameters The parameters to subtitute with.
		 * @param  start      The loop index starts here.
		 * @param  stop       Looping will stop when \c start is <tt>&gt;=
		 *                    stop</tt> if \c step is <tt>&gt; 0</tt>, or <tt>&lt;=
		 &                    stop</tt> if \c step is <tt>&lt; 0</tt>.
		 * @param  step       The amount to increment \c start by on each loop.
		 * @return The result of the substitution.
		 */
		std::string _loopSubtitution(const std::string& section,
			const std::string& endSection,
			const std::vector<std::string>& parameters, sf::Int64 start,
			sf::Int64 stop, sf::Int64 step);

		/**
		 * Before adding a file to the builder, it must be scanned for custom
		 * directives.
		 * @param  filePath The path to the script file.
		 * @param  file     An already opened input stream to read from. The read
		 *                  pointer must be at the beginning of the stream! The
		 *                  stream will be configured to throw exceptions on the
		 *                  bad and fail bits being set.
		 * @return If the file is to be added to the builder as a section, it will
		 *         be returned with custom directives removed. If the file is a
		 *         template, an empty string will be returned.
		 */
		std::string _parseDirectives(const std::string& filePath,
			std::istream& file);

		/**
		 * Parses a #template directive.
		 * @param  directiveText The text that follows the #template directive.
		 * @param  lineNumber    The line number the directive was found on.
		 *                       Template directives only take effect on line 1.
		 * @return The name of the template, if one was given. An empty string
		 *         indicates an invalid #template directive.
		 */
		std::string _parseTemplateDirective(const std::string& directiveText,
			const std::size_t lineNumber);

		/**
		 * Parses an #expand directive.
		 * @param  directiveText The text that follows the #expand directive.
		 * @param  lineNumber    The line number the directive was found on.
		 * @return The name of the template to instantiate, along with a list of
		 *         parameters to instantiate it with. If \c first is empty, the
		 *         directive given was invalid.
		 */
		std::pair<std::string, std::vector<std::string>>
			_parseInstantiateDirective(const std::string& directiveText,
				const std::size_t lineNumber);

		/**
		 * Represents an assertion.
		 */
		struct assertion {
			/**
			 * The code to evaluate after a successful build.
			 * If this code evaluates to \c FALSE, the build will "fail."\n
			 * No terminating <tt>;</tt> will be stored.
			 */
			std::string code;

			/**
			 * If the code evaluates to \c FALSE, this help text will be logged.
			 */
			std::string helpText = "\"\"";
		};

		/**
		 * Parses an #assert directive.
		 * @param  directiveText The text that follows the #assert directive.
		 * @param  lineNumber    The line number the directive was found on.
		 * @return The code to evaluate after a successful build, and the error
		 *         text to log when that code evaluates to \c FALSE. If \c code is
		 *         empty, the directive given was invalid.
		 */
		assertion _parseAssertDirective(const std::string& directiveText,
			const std::size_t lineNumber);

		/**
		 * Allocates a new function context.
		 * @return The error code.
		 * @safety No guarantee.
		 */
		int _allocateContext();

		/**
		 * Prepares the function context when making a call to \c callFunction().
		 * @param  func Pointer to the script function to call.
		 * @return \c TRUE if the context was setup successfully, \c FALSE
		 *         otherwise.
		 * @safety No guarantee.
		 */
		bool _setupContext(asIScriptFunction* const func);

		/**
		 * Resets \c callFunction() variables.
		 * Call this method just before a call to \c callFunction() is over.
		 */
		void _resetCallFunctionVariables() noexcept;

		/**
		 * Constructs a log message from the most current context.
		 * @param  msg The message to log.
		 * @safety No guarantee.
		 */
		std::string _constructMessage(const std::string& msg) const;

		/**
		 * Constructs an error log message from a failed build or compile.
		 * @param  code The error code.
		 * @return The error string.
		 */
		std::string _constructBuildErrorMessage(const int code) const;

		/**
		 * The internal logger object.
		 */
		mutable engine::logger _logger;

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
		 * Points to the object to assign to a context when invoking a method.
		 */
		asIScriptObject* _functionObject = nullptr;

		/**
		 * The list of registrants.
		 */
		std::vector<engine::script_registrant*> _registrants;

		/**
		 * Used to generate documentation on the script interface.
		 */
		std::shared_ptr<DocumentationGenerator> _document;

		/**
		 * Flag used to tell the message callback to fill \c _cachedMsg.
		 */
		bool _fillCachedMsg = false;

		/**
		 * Caches messages given to the \c scriptMessageCallback().
		 */
		std::string _cachedMsg;

		/**
		 * The context used with \c executeCode().
		 */
		asIScriptContext* _executeCodeContext = nullptr;

		/**
		 * The module builder.
		 */
		CScriptBuilder _builder;

		//////////////////////
		// ADDITIONAL STATE //
		//////////////////////

		/**
		 * Clears additional state used for building and metadata etc. storage.
		 * Additionally clears templates and assertions.
		 */
		void _clearState() noexcept;

		/**
		 * Clears template and assertion data after building.
		 */
		void _clearTemplatesAndAssertions() noexcept;

		/**
		 * Stores the metadata associated with each global function in each module.
		 */
		std::unordered_map<std::string, global_function_metadata>
			_functionMetadata;

		/**
		 * Stores the metadata associated with each global variable in each module.
		 */
		std::unordered_map<std::string, global_variable_metadata>
			_variableMetadata;

		/**
		 * Stores the namespace/s of each global function in each module.
		 */
		std::unordered_map<std::string, global_functions_and_their_namespaces>
			_functionNamespaces;

		/**
		 * Stores the namespace/s of each global variable in each module.
		 */
		std::unordered_map<std::string, global_variables_and_their_namespaces>
			_variableNamespaces;

		/**
		 * Stores the templates available to the scripts.
		 */
		std::unordered_map<std::string, std::string> _templates;

		/**
		 * When a script attempts to instantiate a template, its request will be
		 * added here.
		 * The map is keyed on template name (which may or may not end up being
		 * valid). Each parameter of the instantiation is then stored within a
		 * vector, and since a template may be instantiated multiple times, a set
		 * of vectors is stored.\n
		 * We only attempt to service the request once all script files have been
		 * loaded.
		 */
		std::unordered_map<std::string,
			std::unordered_set<std::vector<std::string>>>
			_templateInstantiationQueue;

		/**
		 * Assertions read from scripts will be stored in this queue, then
		 * processed once the build has succeeded.
		 */
		std::vector<assertion> _assertionQueue;
	};
}

#include "tpp/script.tpp"
