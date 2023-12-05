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

/**@file menumanager.hpp
 * Defines code that manages menus.
 */

#pragma once

#include "safejson.hpp"
#include "script.hpp"
#include "gui/menu.hpp"

namespace sfx { namespace gui {
	/**
	 * Loads menus, draws them, handles input, and switches between them.
	 * Also defines the API used by the scripts to work with this GUI engine.
	 */
	class menu_manager : public engine::script_registrant,
		public engine::json_script {
	public:
		/**
		 * Initialises the menu manager with a pointer to a \c scripts object and a
		 * name for the internal logger object.
		 * @param scripts Pointer to a scripts object containing the GUI code to
		 *                run. An error is logged if \c nullptr is given. <b>It is
		 *                important to note that the given scripts object should
		 *                <em>not</em> have any scripts loaded, as this constructor
		 *                adds to the scripts' interface.</b> Scripts must be
		 *                loaded \em after this class is instantiated but \em
		 *                before it is <tt>load()</tt>ed.
		 * @param data    The data to initialise the logger object with.
		 * @sa    \c engine::logger
		 */
		menu_manager(const std::shared_ptr<engine::scripts>& scripts,
			const engine::logger::data& data);

		/**
		 * Releases all menus managed by this object.
		 */
		~menu_manager() noexcept;

		/**
		 * Callback given to \c engine::scripts::registerInterface() to register
		 * the GUI API with a \c scripts object.
		 * @sa \c engine::scripts::registerInterface()
		 */
		void registerInterface(asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>& document);
	private:
		/**
		 * The JSON load method for this class.
		 */
		bool _load(engine::json& j) override;

		/**
		 * The JSON save method for this class.
		 * This class does not have the ability to be saved.
		 * @param  j The \c nlohmann::ordered_json object representing the JSON
		 *           script which this method writes to.
		 * @return Always returns \c FALSE.
		 */
		bool _save(nlohmann::ordered_json& j) override;

		/**
		 * Release all menus.
		 */
		void _releaseAllMenus() noexcept;

		/**
		 * The internal logger object.
		 */
		mutable engine::logger _logger;

		/**
		 * Pointer to the scripts object containing the GUI code to run.
		 */
		std::shared_ptr<engine::scripts> _scripts;

		/**
		 * The menus managed by this manager.
		 */
		std::vector<asIScriptObject*> _menus;
	};
}}
