/*Copyright 2019-2024 CasualYouTuber31 <naysar@protonmail.com>

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

#include "../include/zoom.hpp"

awe::zoom::zoom(float& scalingToUpdate, const float newScaleValue,
	const float duration) :
	_scaling(scalingToUpdate), _scaleFrom(scalingToUpdate),
	_scaleTo(newScaleValue), _duration(duration) {}

bool awe::zoom::animate(const sf::RenderTarget& target) {
	const auto delta = accumulatedDelta();
	if (delta >= _duration) {
		_scaling = _scaleTo;
		return true;
	} else {
		if (_scaleFrom < _scaleTo) {
			_scaling = _scaleFrom + (_scaleTo - _scaleFrom) * (delta / _duration);
		} else {
			_scaling = _scaleFrom - (_scaleFrom - _scaleTo) * (delta / _duration);
		}
		return false;
	}
}

void awe::zoom::draw(sf::RenderTarget& target, sf::RenderStates states) const {}
