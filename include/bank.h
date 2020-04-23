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

#include "texture.h"

namespace awe {
	class common_properties {
	public:
		virtual ~common_properties() noexcept;
		const std::string& getName() const noexcept;
		const std::string& getShortName() const noexcept;
		sfx::SpriteKey getIconKey() const noexcept;
	protected:
		common_properties(safe::json& j) noexcept;
	private:
		std::string _name = "";
		std::string _shortName = "";
		sfx::SpriteKey _iconKey = 0;
	};

	class country : public common_properties {
	public:
		country(safe::json& j) noexcept;
		const sf::Color& getColour() const noexcept;
	private:
		sf::Color _colour;
	};

	class weather : public common_properties {
	public:
		weather(safe::json& j) noexcept;
	};

	class environment : public common_properties {
	public:
		environment(safe::json& j) noexcept;
	};

	class movement_type : public common_properties {
	public:
		movement_type(safe::json& j) noexcept;
	};

	typedef unsigned int BankIndex;
	template<typename T>
	class bank : public safe::json_script {
	public:
		~bank() noexcept;
		const T* operator[](BankIndex id) const noexcept;
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
const T* awe::bank<T>::operator[](awe::BankIndex id) const noexcept {
	if (id >= _bank.size()) return nullptr;
	return _bank[id];
}

template<typename T>
bool awe::bank<T>::_load(safe::json& j) noexcept {
	nlohmann::json jj = j.nlohmannJSON();
	for (auto& i : jj.items()) {
		//loop through each object, allowing the template type T to construct its values based on each object
		safe::json input(i.value());
		const T* pItem = new T(input);
		_bank.push_back(pItem);
	}
	return true;
}

template<typename T>
bool awe::bank<T>::_save(nlohmann::json& j) noexcept {
	return false;
}