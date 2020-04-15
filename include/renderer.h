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
#include "sfml/Graphics.hpp"

namespace sfx {
	class delta_timer {
	public:
		void calculate() noexcept;
		float get() const noexcept;
	private:
		float _delta = 0.0f;
		sf::Clock _deltaTimer; //UNIT (usually pixels) per second
	};

	struct renderer_properties {
		unsigned int width = 900;
		unsigned int height = 600;
		unsigned int size = 0;
		std::string caption = "Window";
		bool fullscreen = false;
		float scale = 1.0;
	};

	class renderer : public sf::RenderWindow, public safe::json_script {
	public:
		static const renderer_properties default_properties;

		renderer(const std::string& name = "renderer") noexcept;

		void setProperties(const renderer_properties& newval) noexcept;
		renderer_properties getProperties() const noexcept;

		void openWindow() noexcept;

		//draw method which abides by the scale property
		void drawToScale(const sf::Texture& tex, float x = 0.0, float y = 0.0, const bool scale = true);
		void drawToScale(const sf::Drawable& drawing, float x = 0.0, float y = 0.0, const bool scale = true);

		//camera methods
		sf::Vector2f getCamera() const noexcept;
		sf::Vector2f setCamera(const sf::Vector2f& newCamera = sf::Vector2f(0,0)) noexcept;
		sf::Vector2f setCamera(const float x = 0.0f, const float y = 0.0f) noexcept;
		sf::Vector2f moveCamera(const sf::Vector2f& offset) noexcept;
		sf::Vector2f moveCamera(const float x, const float y) noexcept;
	private:
		virtual bool _load(safe::json& j) noexcept;
		virtual bool _save(nlohmann::json& j) noexcept;

		global::logger _logger;
		renderer_properties _window;
		sf::Vector2f _camera;
	};

	namespace trans {
		class iTrans {
		public:
			virtual bool transition() = 0;
		protected:
			delta_timer _delta;
		};

		class rectangle : public iTrans {
		public:
			rectangle(sf::RenderWindow* window, const bool fadingIn, const sf::Time& duration = sf::seconds(1), const sf::Color& colour = sf::Color()) noexcept;
			bool transition();
		private:
			//properties
			bool _fadingIn;
			sf::Time _duration;
			sf::Color _colour;
			//live data
			sf::RenderWindow* _window;
			sf::Clock _clock;
			sf::Vector2f _size;
			sf::RectangleShape _toprect, _bottomrect;
		};
	}
}