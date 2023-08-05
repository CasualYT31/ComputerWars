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

#pragma once

template<typename T>
typename T::Ptr sfx::gui::_findWidget(std::string name,
	std::vector<std::string>* namesList, std::string* fullname) const {
	// Split string.
	std::vector<std::string> names;
	std::size_t pos = 0;
	do {
		pos = name.find('.');
		names.push_back(name.substr(0, pos));
		if (pos != std::string::npos) name = name.substr(pos + 1);
	} while (pos != std::string::npos);
	// If group name was not given, insert it.
	tgui::Group::Ptr groupPtr = _gui.get<tgui::Group>(names[0]);
	if (!groupPtr) {
		names.insert(names.begin(), _currentGUI);
	} else if (groupPtr && names.size() == 1) {
		// If the group name is all that was given, then simply return the group.
		if (namesList) {
			namesList->clear();
			*namesList = names;
		}
		if (fullname) *fullname = name;
		return _gui.get<T>(names[0]);
	}
	if (namesList) {
		namesList->clear();
		*namesList = names;
	}
	if (fullname) {
		*fullname = "";
		for (auto& name : names) *fullname += name + ".";
		if (fullname->size() > 0) fullname->pop_back();
	}
	// Find it.
	tgui::Container::Ptr container = _gui.get<tgui::Container>(names[0]);
	if (names.size() > 2) {
		for (std::size_t w = 1; w < names.size() - 1; ++w) {
			if (!container) return nullptr;
			tgui::String containerName;
			for (std::size_t v = 0; v <= w; ++v) {
				containerName += names[v] + ".";
			}
			containerName.pop_back();
			container = container->get<tgui::Container>(containerName);
		}
	}
	if (container) {
		tgui::String widgetName;
		for (std::size_t v = 0; v < names.size(); ++v) {
			widgetName += names[v] + ".";
		}
		widgetName.pop_back();
		return container->get<T>(widgetName);
	} else {
		return nullptr;
	}
}

////////////////////
// CSCRIPTWRAPPER //
////////////////////

template<typename T>
sfx::gui::CScriptWrapper<T>::CScriptWrapper(T* const obj) : _ptr(obj) {
	if (_ptr) _ptr->AddRef();
}

template<typename T>
sfx::gui::CScriptWrapper<T>::CScriptWrapper(
	const sfx::gui::CScriptWrapper<T>& obj) : _ptr(obj.operator->()) {
	if (_ptr) _ptr->AddRef();
}

template<typename T>
sfx::gui::CScriptWrapper<T>::CScriptWrapper(sfx::gui::CScriptWrapper<T>&& obj)
noexcept : _ptr(std::move(obj.operator->())) {
	if (_ptr) _ptr->AddRef();
}

template<typename T>
sfx::gui::CScriptWrapper<T>::~CScriptWrapper() noexcept {
	if (_ptr) _ptr->Release();
}

template<typename T>
T* sfx::gui::CScriptWrapper<T>::operator->() const noexcept {
	return _ptr;
}
