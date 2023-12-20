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

/**@file animation.hpp
 * Defines all of the animation code, either directly, or by including other
 * animation code files.
 */

#pragma once

#include "renderer.hpp"
#include "script.hpp"

#include "animation/daybegin.hpp"

namespace awe {
	/**
	 * The animations that are available.
	 */
	enum class Animation {
		// Don't forget to update the FMT formatter for this enum in fmtawe.hpp!
		DayBegin,
		Count
	};

	/**
	 * Registers and documents the types required for animations, if they haven't
	 * already been registered.
	 */
	void RegisterAnimationTypes(asIScriptEngine* engine,
		const std::shared_ptr<DocumentationGenerator>& document);

	/**
	 * Builds an \c animated_drawable object whose subclass is defined by the given
	 * type parameter.
	 * @param  type   The type of animation to allocate.
	 * @param  params The parameters to give to the animation's constructor.
	 * @return Pointer to the animation object.
	 * @throws \c std::invalid_argument If the given \c params were rejected by the
	 *                                  subclass's constructor, or if \c type was
	 *                                  invalid. The attached message will explain
	 *                                  the error in more detail.
	 */
	std::unique_ptr<sfx::animated_drawable> animationFactory(
		const Animation type, CScriptArray* const params,
		const std::shared_ptr<engine::scripts>& scripts);
}
