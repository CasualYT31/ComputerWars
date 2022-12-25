/*Copyright 2019-2023 CasualYouTuber31 <naysar@protonmail.com>

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

/**@file drawabletest.hpp
 * Tests the \c sfx::animated_drawable class.
 */

#include "sharedfunctions.hpp"
#include "renderer.hpp"

/**
 * Defines an implementation of \c animated_drawable for testing purposes.
 */
class test_drawable : public sfx::animated_drawable {
public:
	/**
	 * This gives the test fixture access to the protected methods to test.
	 */
	friend class DrawableTest;

	/**
	 * Empty \c animate() method.
	 * This test suite tests the core internals of \c animated_drawables. Subclass
	 * implementations of this method must be tested in the test suite dedicated to
	 * their subclass, and not here.
	 */
	bool animate(const sf::RenderTarget& target, const double scaling = 1.0)
		noexcept { return true; }
private:
	/**
	 * Empty \c draw() method.
	 * @sa test_drawable::animate()
	 */
	void draw(sf::RenderTarget& target, sf::RenderStates states) const {}
};

/**
 * Test fixture for this suite.
 */
class DrawableTest : public ::testing::Test {
protected:
	/**
	 * Resets the delta timer at the beginning of most tests.
	 */
	void SetUp() override {
		if (!isTest({ "HandlesFirstTime" })) calculateDelta();
	}

	/**
	 * Calls \c drawable.calculateDelta(timeout).
	 */
	float calculateDelta(const sf::Time& timeout = sf::seconds(1.0f))
		noexcept {
		return drawable.calculateDelta(timeout);
	}

	/**
	 * Calls \c drawable.accumulatedDelta(timeout).
	 */
	float accumulatedDelta(const sf::Time& timeout = sf::seconds(1.0f))
		noexcept {
		return drawable.accumulatedDelta(timeout);
	}

	/**
	 * Calls \c drawable.resetDeltaAccumulation().
	 */
	void resetDeltaAccumulation() noexcept {
		drawable.resetDeltaAccumulation();
	}

	/**
	 * Calls \c drawable.finish().
	 */
	void finish() noexcept {
		drawable.finish();
	}

	/**
	 * Calls \c drawable.isFinished().
	 */
	bool isFinished() noexcept {
		return drawable.isFinished();
	}

	/**
	 * Calls \c drawable.firstTimeAnimated().
	 */
	bool firstTimeAnimated() noexcept {
		return drawable.firstTimeAnimated();
	}

	/**
	 * Calls \c drawable.resetAnimation().
	 */
	void resetAnimation() noexcept {
		drawable.resetAnimation();
	}

	/**
	 * The \c animated_drawable object to test on.
	 */
	test_drawable drawable;
};

#ifdef COMPUTER_WARS_FULL_DRAWABLE_TESTING

/**
 * Tests \c sfx::animated_drawable::calculateDelta().
 */
TEST_F(DrawableTest, CalculateDelta) {
	longWait();
	EXPECT_LT(calculateDelta(), 0.0001f); // should be basically 0
	longWait();
	EXPECT_GE(calculateDelta(sf::seconds(5.0f)), 3.0f);
}

/**
 * Tests \c sfx::animated_drawable::accumulatedDelta() and
 * \c sfx::animated_drawable::resetDeltaAccumulation().
 */
TEST_F(DrawableTest, AccumulateDelta) {
	longWait();
	EXPECT_EQ(3, static_cast<int>(accumulatedDelta(sf::seconds(5.0f))));
	longWait();
	EXPECT_EQ(6, static_cast<int>(accumulatedDelta(sf::seconds(5.0f))));
	longWait();
	EXPECT_EQ(6, static_cast<int>(accumulatedDelta()));
	resetDeltaAccumulation();
	longWait();
	EXPECT_EQ(3, static_cast<int>(accumulatedDelta(sf::seconds(5.0f))));
}

#endif

/**
 * Tests \c sfx::animated_drawable::finish(),
 * \c sfx::animated_drawable::isFinished(), and
 * \c sfx::animated_drawable::resetAnimation().
 */
TEST_F(DrawableTest, HandlesFinish) {
	EXPECT_FALSE(isFinished());
	finish();
	EXPECT_TRUE(isFinished());
	resetAnimation();
	EXPECT_FALSE(isFinished());
}

/**
 * Tests \c sfx::animated_drawable::firstTimeAnimated() and
 * \c sfx::animated_drawable::resetAnimation().
 */
TEST_F(DrawableTest, HandlesFirstTime) {
	EXPECT_TRUE(firstTimeAnimated());
	calculateDelta();
	EXPECT_FALSE(firstTimeAnimated());
	resetAnimation();
	EXPECT_TRUE(firstTimeAnimated());
}
