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

#include "gui/menu.hpp"
#include "TGUI/TGUI.hpp"

void sfx::gui::menu::Register(asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	if ((!engine->GetTypeInfoByName("Menu"))) {
		auto r = RegisterType(engine, "Menu",
			[](asIScriptEngine* engine, const std::string& type) {
				engine->RegisterObjectBehaviour(type.c_str(), asBEHAVE_FACTORY,
					std::string(type + "@ f()").c_str(),
					asFUNCTION(sfx::gui::menu::Create), asCALL_CDECL);
			});
		document->DocumentObjectType(r, "Represents a menu.");
	}
}

sfx::gui::menu* sfx::gui::menu::Create() {
	return new sfx::gui::menu();
}

bool sfx::gui::menu::animate(const sf::RenderTarget& target) {
	return false;
}

void sfx::gui::menu::draw(sf::RenderTarget& target,
	sf::RenderStates states) const {

}
