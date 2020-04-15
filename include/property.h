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
	struct property {
		virtual ~property() noexcept;
		unsigned int id = 0;
		std::string nativeName = "";
		std::string nativeShortName = "";
		unsigned int spriteKey = 0; //icon
		sf::Color colour = sf::Color();
	};

	class game_property : public safe::json_script {
	public:
		virtual ~game_property() noexcept; //always declare destructors virtual for polymorphic base classes
		property& operator[](const std::string& key) noexcept;
		property& operator[](const unsigned int id) noexcept;
	private:
		virtual bool _load(safe::json& j) noexcept;
		virtual bool _save(nlohmann::json& j) noexcept;
		std::unordered_map<std::string, property> _data;
		std::unordered_map<unsigned int, property*> _findByID;
	};

	class country : public game_property {};
	class weather : public game_property {};
	class environment : public game_property {};
	class movement : public game_property {};
}