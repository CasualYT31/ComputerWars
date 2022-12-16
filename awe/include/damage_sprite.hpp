/*Copyright 2019-2022 CasualYouTuber31 <naysar@protonmail.com>

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

/**@file damage_sprite.hpp
 * Defines the class which represents a damage sprite.
 * @warning This code is a temporary measure whilst <a
 *          href="https://trello.com/c/7QUrpwyG/259">the TGUI animated sprites
 *          issue</a> sticks around. This is why there are many flaws with the way
 *          I've written this code. Once I get round to resolving that, this code
 *          can be completely replaced with a set of GUI widgets, and all this code
 *          can be moved to the script side, where it will be properly documented
 *          and maintained. I will emphasise: this code will not end up in a beta
 *          release!
 */

#pragma once

#include "texture.hpp"

namespace awe {
	/**
	 * Represents a damage tooltip.
	 */
	class damage_tooltip : public sfx::animated_drawable {
	public:
		damage_tooltip() noexcept;
		inline void setSpritesheet(
			const std::shared_ptr<sfx::animated_spritesheet>& spritesheet) {
			_sprite.setSpritesheet(spritesheet);
			_sprite.setSprite("damagetooltip");
		}
		void setDamage(const unsigned int dmg) noexcept;
		/// Should be the position of the attack cursor with offset applied!
		void setPosition(const sf::Vector2f& pos) noexcept;
		void visible(const bool v) noexcept {
			_visible = v;
		}
		virtual bool animate(const sf::RenderTarget& target,
			const double scaling = 1.0) noexcept;
	private:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
		sfx::animated_sprite _sprite;
		sf::Text _damage;
		bool _visible = false;
	};
}
