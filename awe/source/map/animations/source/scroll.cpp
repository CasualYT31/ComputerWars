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

#include "../include/scroll.hpp"
#include "maths.hpp"

awe::scroll::scroll(std::optional<float>& viewOffsetX,
	std::optional<float>& viewOffsetY,
	const std::shared_ptr<const sf::RenderTarget>& target,
	const sf::Vector2f& end, const float speed, const sf::Vector2f& mapPixelSize,
	const float scaling, const bool drawCursors, sf::Vector2f padding) :
	_viewOffsetX(viewOffsetX), _viewOffsetY(viewOffsetY), _speed(speed),
	_enableCursorGraphics(drawCursors) {
	// Step 1. Retrieve the target's size and adjust padding with it. If there is
	//         no view offset for a given coordinate, override its corresponding
	//         padding to 0.
	const auto size = sf::Vector2f(static_cast<float>(target->getSize().x),
		static_cast<float>(target->getSize().y));
	const sf::Vector2f viewSize = size / scaling;
	// If it ain't broke, don't fix it... Should really find a way to remove these
	// constants though (1920, 1080, 2.f). And I might be able to refactor all of
	// this code at some point.
	if (viewOffsetX) padding.x *= size.x / 1920 * 2.f * scaling;
	else padding.x = 0.0f;
	if (viewOffsetY) padding.y *= size.y / 1080 * 2.f * scaling;
	else padding.y = 0.0f;

	// Step 2. Define our start point, which is the centre of the target.
	const auto start = size * 0.5f;

	// Step 3. Find the gradient and Y intercept of the line segment defined by the
	//         two points.
	float m = 0.0f, c = 0.0f;
	const auto noXMovement = end.x - start.x == 0;
	if (!noXMovement) {
		m = (end.y - start.y) / (end.x - start.x);
		c = end.y - m * end.x;
	}

	// The distance between this point (fully initialised by step 5's completion)
	// and the end point dictates the distance the view needs to travel.
	sf::Vector2f constrainedEnd = end;

	// Step 4. If the end point is outside of the bounds along the X axis, then we
	//         will need to adjust the X coordinate and solve for Y.
	bool xConstrained = false;
	const auto upperPadding =
		((viewOffsetX && engine::closeTo(*viewOffsetX, 0.0f)) ? 0.0f : padding.x),
		lowerPadding = ((viewOffsetX && engine::closeTo(*viewOffsetX,
			mapPixelSize.x - viewSize.x)) ? 0.0f : padding.x);
	if (constrainedEnd.x < upperPadding) {
		constrainedEnd.x = upperPadding;
		xConstrained = true;
	} else if (constrainedEnd.x > size.x - lowerPadding) {
		constrainedEnd.x = size.x - lowerPadding;
		xConstrained = true;
	}
	if (!noXMovement) constrainedEnd.y = m * constrainedEnd.x + c;

	// Step 5. If the end point is outside of the bounds along the Y axis, then we
	//         will need to adjust the Y coordinate and solve for X.
	bool yConstrained = false;
	const auto leftPadding =
		((viewOffsetY && engine::closeTo(*viewOffsetY, 0.0f)) ? 0.0f : padding.y),
		rightPadding = ((viewOffsetY && engine::closeTo(*viewOffsetY,
			mapPixelSize.y - viewSize.y)) ? 0.0f : padding.y);
	if (constrainedEnd.y < leftPadding) {
		constrainedEnd.y = leftPadding;
		yConstrained = true;
	} else if (constrainedEnd.y > size.y - rightPadding) {
		constrainedEnd.y = size.y - rightPadding;
		yConstrained = true;
	}
	if (m && !noXMovement) constrainedEnd.x = (constrainedEnd.y - c) / m;

	// Step 6. Calculate the distance between the given end point and the
	//         constrained end point. If it's > 0, we need to move the view.
	_distance = engine::closeTo(end, constrainedEnd) ? sf::Vector2f(0.0f, 0.0f) :
		end - constrainedEnd;

	// Step 7. However, we should not move the view if it's at the edge of the map.
	static const auto offsetChecks = [](float& viewOffset, const float offsetMin,
		const float offsetMax, float& distance, const bool constrained) {
		// 7a. If view offset is at min, and there is negative distance in the
		//     axis to check for, and it is constrained, then cancel the
		//     distance in X.
		if (engine::closeTo(viewOffset, offsetMin) && constrained &&
			distance < 0.0f) distance = 0.0f;
		// 7b. If view offset is at max, and there is positive distance in the
		//     axis to check for, and it is constrained, then cancel the
		//     distance in X.
		if (engine::closeTo(viewOffset, offsetMax) && constrained &&
			distance > 0.0f) distance = 0.0f;
	};

	// Step 8. Solving for X or Y may have caused a distance to be calculated, even
	//         if there is no offset for their axes. So explicitly clear them.
	if (viewOffsetX) offsetChecks(*viewOffsetX, 0.0f, mapPixelSize.x - viewSize.x,
		_distance.x, xConstrained);
	else _distance.x = 0.0f;
	if (viewOffsetY) offsetChecks(*viewOffsetY, 0.0f, mapPixelSize.y - viewSize.y,
		_distance.y, yConstrained);
	else _distance.y = 0.0f;

	_firstDistance = _distance /= scaling;
}

// For now, assume render window will not resize.
// If it does, we might have to make adjustments to the distance.
bool awe::scroll::animate(const sf::RenderTarget& target) {
	// If the tile was already in view, cancel animation.
	if (firstTimeAnimated() && engine::closeTo(_firstDistance, { 0.0f, 0.0f }))
		return true;
	const auto delta = calculateDelta();
	auto distanceToTravel = _speed * delta * engine::normalise(_distance);
	_distance -= distanceToTravel;
	if (_viewOffsetX) *_viewOffsetX += distanceToTravel.x;
	if (_viewOffsetY) *_viewOffsetY += distanceToTravel.y;
	if ((_firstDistance.x < 0.0f && _distance.x > 0.0f) ||
		(_firstDistance.x > 0.0f && _distance.x < 0.0f)) _distance.x = 0.0f;
	if ((_firstDistance.y < 0.0f && _distance.y > 0.0f) ||
		(_firstDistance.y > 0.0f && _distance.y < 0.0f)) _distance.y = 0.0f;
	return engine::closeTo(_distance, { 0.0f, 0.0f });
}

void awe::scroll::draw(sf::RenderTarget& target, sf::RenderStates states) const {}
