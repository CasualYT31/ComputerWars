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

/**@file menu.hpp
 * Defines code that manages a menu.
 */

#pragma once

#include "renderer.hpp"
#include "script.hpp"

namespace sfx { namespace gui {
	/**
	 * Represents a menu.
	 */
	class menu : public sfx::animated_drawable,
		public engine::script_reference_type<sfx::gui::menu> {
	public:
		/**
		 * Registers and documents this class with the script interface, if it
		 * hasn't already been registered.
		 */
		static void Register(asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>& document);

		/**
		 * Creates a \c Menu.
		 * @return Pointer to the \c Menu.
		 */
		static sfx::gui::menu* Create();

		/**
		 * Animates the menu.
		 * Any sprites are animated, and the colour background (if there is one) is
		 * resized to match the size of the target. In addition, if a langauge
		 * dictionary has been given, all captions will be translated.
		 * @return Always returns \c FALSE.
		 * @sa     sfx::gui::setLanguageDictionary()
		 */
		bool animate(const sf::RenderTarget& target) final;
	private:
		/**
		 * Draws the current GUI menu.
		 * @todo    Below warning may not apply.
		 * @warning It is to be noted that this implementation of \c draw()
		 *          \b ignores any given render states. This was done to remain
		 *          consistent with TGUI (which does not seem to allow
		 *          \c sf::RenderStates). The internal \c tgui::Gui object also
		 *          ignores the given \c target when drawing: it must be previously
		 *          set with \c setTarget().
		 */
		void draw(sf::RenderTarget& target, sf::RenderStates states) const final;
	};
}}
