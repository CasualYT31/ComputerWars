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

#include "animation.hpp"

void awe::RegisterAnimationTypes(asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	if (!engine->GetTypeInfoByName("Animation")) {
		auto r = engine->RegisterEnum("Animation");
		document->DocumentObjectEnum(r, "Stores the animations supported in the "
			"engine.");
		engine->RegisterEnumValue("Animation", "DayBegin",
			static_cast<int>(awe::Animation::DayBegin));

		r = engine->RegisterFuncdef("void AnimationCallback()");
		document->DocumentObjectFuncDef(r, "Signature of the callback that is "
			"invoked when an animation either couldn't be played or has "
			"completed.");
	}
}
