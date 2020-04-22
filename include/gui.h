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

#include "tgui/tgui.hpp"
#include "texture.h"
#include "script.h"

namespace awe {
	class gui_background {
	public:
		enum type {
			Sprite,
			Colour
		};
		gui_background() noexcept;
		gui_background(unsigned int key) noexcept;
		gui_background(sf::Color colour) noexcept;
		void set(unsigned int key) noexcept;
		void set(sf::Color colour) noexcept;
		type getType() const noexcept;
		unsigned int getSprite() const noexcept;
		sf::Color getColour() const noexcept;
	private:
		type _flag = type::Colour;
		unsigned int _key = 0;
		sf::Color _colour;
	};

	class gui : public safe::json_script {
	public:
		gui(awe::scripts* scripts, const std::string& name = "gui") noexcept;

		std::string setGUI(const std::string& newPanel) noexcept;
		std::string getGUI() noexcept;

		void setTarget(sf::RenderTarget& newTarget) noexcept;
		bool handleEvent(sf::Event e) noexcept;
		void drawBackground(sfx::spritesheet* sprites = nullptr) noexcept;
		void drawForeground(sfx::spritesheet* sprites = nullptr) noexcept;

		void signalHandler(tgui::Widget::Ptr widget, const std::string& signalName) noexcept;
	private:
		virtual bool _load(safe::json& j) noexcept;
		virtual bool _save(nlohmann::json& j) noexcept;
		bool _loadGUI(const std::string& name, const std::string& filepath) noexcept;
		void _connectSignals(tgui::Widget::Ptr widget) noexcept;

		global::logger _logger;
		tgui::Gui _gui;
		std::string _currentGUI;
		std::unordered_map<std::string, gui_background> _guiBackground;
		awe::scripts* _scripts = nullptr;
		//background drawing stuff
		sf::Sprite _bgsprite;
		//data
		std::unordered_map<std::string, std::unordered_map<std::string, unsigned int>> _guiSpriteKeys;
	};
}