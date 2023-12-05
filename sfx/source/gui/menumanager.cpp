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

#include "gui/menumanager.hpp"

sfx::gui::menu_manager::menu_manager(
	const std::shared_ptr<engine::scripts>& scripts,
	const engine::logger::data& data) : json_script({ data.sink, "json_script" }),
	_logger(data), _scripts(scripts) {
	if (_scripts) {
		_scripts->addRegistrant(this);
	} else {
		_logger.critical("No scripts object has been given to this menu manager: "
			"no menus will be loaded.");
	}
}

sfx::gui::menu_manager::~menu_manager() noexcept {
	_releaseAllMenus();
}

bool sfx::gui::menu_manager::_load(engine::json& j) {
	// Locate all subclasses of Menu and instantiate them.
	_releaseAllMenus();
	const auto menuTypes = _scripts->getConcreteClassNames("Menu");
	for (const auto& menuType : menuTypes) {
		_logger.warning("DA: {}", menuType);
		const auto menu = _scripts->createObject(menuType);
		if (menu) {
			_menus.push_back(menu);
		} else {
			_logger.error("Could not create menu \"{}\"!", menuType);
		}
	}
	return true;
}

bool sfx::gui::menu_manager::_save(nlohmann::ordered_json& j) {
	return false;
}

void sfx::gui::menu_manager::_releaseAllMenus() noexcept {
	_logger.write("{}", _menus.size());
	for (const auto& menu : _menus) menu->Release();
	_menus.clear();
}
