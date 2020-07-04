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

#include "renderer.h"

namespace transition {
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