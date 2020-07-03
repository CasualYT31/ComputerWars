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
	class renderer : public sf::RenderWindow, public safe::json_script {
	public:
		void openWindow(const sf::ContextSettings& settings = sf::ContextSettings()) noexcept;
	private:
		virtual bool _load(safe::json& j) noexcept;
		virtual bool _save(nlohmann::json& j) noexcept;

		unsigned int width = 1280;
		unsigned int height = 720;
		std::string caption = "Application";
#pragma pack(push, 1)
		struct style_flags {
			bool close = false;
			bool def = true;
			bool fullscreen = false;
			bool none = false;
			bool resize = false;
			bool titlebar = false;
		} style;
#pragma pack(pop)
	};

	class animated_drawable : public sf::Drawable {
	public:
		virtual bool animate(sf::RenderTarget& target) noexcept = 0;
	protected:
		float calculateDelta() noexcept;
	private:
		sf::Clock _deltaTimer;
	};

	namespace trans {
		class rectangle : public sfx::animated_drawable {
		public:
			rectangle(const bool isFadingIn, const sf::Time& duration = sf::seconds(1), const sf::Color& colour = sf::Color()) noexcept;
			virtual bool animate(sf::RenderTarget& target) noexcept;
		private:
			virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
			// properties
			bool _isFadingIn;
			sf::Time _duration;
			// live data
			bool _isFirstCallToAnimate = true;
			sf::Vector2f _size;
			sf::RectangleShape _toprect, _bottomrect;
		};
	}
}