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

/**@file options.hpp
 * Declares the class which represents options that \c awe::game will accept when
 * creating a new game.
 */

#pragma once

#include "script.hpp"

namespace awe {
	/**
	 * Holds options that are to be passed to \c awe::game when loading a new map.
	 */
	struct game_options {
		/**
		 * Registers this struct with the script interface, if it hasn't been
		 * already.
		 * @warning The \c BankID typedef must already have been registered before
		 *          using this class!
		 */
		static void registerGameOptionsType(asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>& document,
			const std::shared_ptr<engine::scripts>& scripts) noexcept;

		/**
		 * The array of current COs to assign to each army.
		 * The \c ArmyID of an army will be used as an index. The values will be
		 * \c commander <tt>BankID</tt>s.
		 */
		CScriptArray* currentCOs = nullptr;

		/**
		 * The array of tag COs to assign to each army.
		 * The \c ArmyID of an army will be used as an index. The values will be
		 * \c commander <tt>BankID</tt>s.
		 */
		CScriptArray* tagCOs = nullptr;

		/**
		 * Pointer to the @c scripts object used to create the arrays.
		 * @warning <b>This pointer must be set before instantiating any objects of
		 *          this type!</b> You can use @c registerGameOptionsType() to
		 *          achieve this!
		 */
		static std::shared_ptr<engine::scripts> _scripts;

		/**
		 * Factory function for this reference type.
		 * @return Pointer to the new object.
		 */
		static awe::game_options* factory() noexcept;

		/**
		 * Copy reference to this object.
		 */
		void addRef() noexcept;

		/**
		 * Release reference to this object.
		 */
		void releaseRef() noexcept;

	private:
		/**
		 * Reference counter.
		 */
		asUINT _refCount = 1;
	};
}