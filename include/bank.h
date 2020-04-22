/*Copyright 2020 CasualYouTuber31 <naysar@protonmail.com>

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

#include "safejson.h"

namespace awe {
	template<typename T>
	class bank : public safe::json_script {
	public:
		typedef unsigned int index;
		~bank() noexcept;
		const T* operator[](bank::index id) const noexcept;
	private:
		virtual bool _load(safe::json& j) noexcept;
		virtual bool _save(nlohmann::json& j) noexcept;
		std::vector<const T*> _bank;
	};
}

template<typename T>
awe::bank<T>::~bank() noexcept {
	for (auto& i : _bank) {
		delete i;
	}
}

template<typename T>
const T* awe::bank<T>::operator[](awe::bank<T>::index id) const noexcept {
	if (id >= _bank.size()) return nullptr;
	return &_bank[id];
}

template<typename T>
bool awe::bank<T>::_load(safe::json& j) noexcept {
	nlohmann::json jj = j.nlohmannJSON();
	for (auto& i : jj.items()) {
		//loop through each object, allowing the template type T to construct its values based on each object
		const T* pItem = new T(i.value());
		_bank.push_back(pItem);
	}
}

template<typename T>
bool awe::bank<T>::_save(nlohmann::json& j) noexcept {
	return false;
}