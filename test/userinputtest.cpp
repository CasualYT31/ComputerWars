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

/**@file userinputtest.hpp
 * Tests the \c sfx::user_input class.
 */

#include "sharedfunctions.hpp"
#include "userinput.hpp"
#include "renderer.hpp"
#include "fonts.hpp"

/**
 * This test fixture is used to initialise the testing object.
 */
class UserInputTest : public ::testing::Test {
protected:
	/**
	 * Loads a configuration script for the test object before all tests.
	 */
	void SetUp() override {
		sf::Joystick::update();
		// only setup renderer and user_input config scripts once
		// also reset it before normal operation testing commences to remove any
		// changes made to it during testing up to that point
		if (isTest({ "LoadValidScript", "NormalOperationTiedToWindow" })) {
			setupRendererJSONScript();
			setupJSONScript([](nlohmann::json& j) {
				j["joystickaxis"] = 25.0f;
				j["joystickid"] = 0;
				j["up"]["type"] = 2;
				j["up"]["delays"] = { 800, 80 };
				j["up"]["keys"] = { 73 };
				j["up"]["axes"] = { {1, -1} };
				j["select"]["type"] = 1;
				j["select"]["keys"] = { 58 };
				j["select"]["buttons"] = { 0 };
				j["select"]["mouse"] = { 0 };
				j["hold"]["type"] = 0;
				j["hold"]["mouse"] = { 1 };
				j["hold"]["axes"] = { {0, 1}, {0, -1} };
			}, "ui/ui.json");
		}
		// always load test json script into ui object
		ui.load(getTestAssetPath("ui/ui.json"));
		// if the test relies on ui being tied to window, then set the tie
		if (testNameContains("TiedToWindow")) {
			window->load(getTestAssetPath("renderer/renderer.json"));
			window->openWindow();
			ui.tieWindow(window);
		}
	}

	/**
	 * The \c sfx::user_input object to test on.
	 */
	sfx::user_input ui;

	/**
	 * The \c sfx::renderer object to tie \c ui to during some tests.
	 */
	std::shared_ptr<sfx::renderer> window = std::make_shared<sfx::renderer>();
};

/**
 * Tests behaviour of a \c user_input object after loading a valid script.
 */
TEST_F(UserInputTest, LoadValidScript) {
	EXPECT_FLOAT_EQ(ui.getJoystickAxisThreshold(), 25.0);
	EXPECT_EQ(ui.getConfiguration("up").keyboard.size(), 1);
}

/**
 * Tests behaviour of \c user_input object after loading an invalid script,
 * \em after loading a valid script.
 * The state of the object should be reset.
 */
TEST_F(UserInputTest, LoadInvalidScript) {
	ui.load(getTestAssetPath("ui/faultyui.json"));
	EXPECT_EQ(ui.getConfiguration("up").keyboard.size(), 0);
}

/**
 * Tests the behaviour of providing invalid data to
 * \c sfx::user_input::setJoystickID().
 */
TEST_F(UserInputTest, SetJoystickHandlesInvalidData) {
	auto old = ui.getJoystickID();
	ui.setJoystickID(sf::Joystick::Count + 1);
	std::string find = "Attempted to set a joystick with ID " +
		std::to_string(sf::Joystick::Count + 1);
	EXPECT_IN_LOG(find);
	EXPECT_EQ(ui.getJoystickID(), old);
	EXPECT_NE(ui.getJoystickID(), sf::Joystick::Count + 1);
}

/**
 * Tests the behaviour of \c sfx::user_input::setJoystickAxisThreshold().
 */
TEST_F(UserInputTest, SetJoystickAxisThreshold) {
	ui.setJoystickAxisThreshold(50.5f);
	EXPECT_FLOAT_EQ(ui.getJoystickAxisThreshold(), 50.5f);
	ui.setJoystickAxisThreshold(5.0f);
	EXPECT_FLOAT_EQ(ui.getJoystickAxisThreshold(), 5.0f);
	ui.setJoystickAxisThreshold(1.0f);
	EXPECT_IN_LOG("Attempted to set a joystick axis threshold of 1");
	EXPECT_FLOAT_EQ(ui.getJoystickAxisThreshold(), 5.0f);
	ui.setJoystickAxisThreshold(95.0f);
	EXPECT_FLOAT_EQ(ui.getJoystickAxisThreshold(), 95.0f);
	ui.setJoystickAxisThreshold(500.0f);
	EXPECT_IN_LOG("Attempted to set a joystick axis threshold of 500");
	EXPECT_FLOAT_EQ(ui.getJoystickAxisThreshold(), 95.0f);
}

/**
 * Tests the behaviour of \c sfx::user_input::setConfiguration() and
 * \c sfx::user_input::save().
 */
TEST_F(UserInputTest, SetConfigurationAndSave) {
	sfx::user_configuration config = ui.getConfiguration("up");
	config.keyboard.push_back(sf::Keyboard::Key::W);
	ui.setConfiguration("up", config);
	EXPECT_EQ(ui.getConfiguration("up").keyboard.size(), 2);
	ui.save();
	config.mouse.push_back(sf::Mouse::Button::Right);
	config.keyboard.clear();
	ui.setConfiguration("up", config);
	EXPECT_EQ(ui.getConfiguration("up").mouse.size(), 1);
	EXPECT_EQ(ui.getConfiguration("up").keyboard.size(), 0);
	ui.load();
	EXPECT_EQ(ui.getConfiguration("up").mouse.size(), 0);
	EXPECT_EQ(ui.getConfiguration("up").keyboard.size(), 2);
}

#ifdef COMPUTER_WARS_FULL_USERINPUT_TESTING

/**
 * Tests the behaviour of \c sfx::user_input::mousePosition() when the test object
 * is not tied to a window.
 */
TEST_F(UserInputTest, MousePosition) {
	auto old = sf::Mouse::getPosition();

	sf::Mouse::setPosition(sf::Vector2i(5, 5));
	EXPECT_EQ(ui.mousePosition(), sf::Vector2i(5, 5));

	sf::Mouse::setPosition(old);
}

/**
 * Tests the behaviour of \c sfx::user_input::mousePosition() when the test object
 * is tied to a window.
 */
TEST_F(UserInputTest, MousePositionTiedToWindow) {
	auto old = sf::Mouse::getPosition();

	// firstly make it so that the test window does not have focus, then test
	sf::Window temp(sf::VideoMode(100, 100), "Temp");
	EXPECT_EQ(ui.mousePosition(), sfx::INVALID_MOUSE);
	// now set the focus onto the window, then test
	window->requestFocus();
	sf::Mouse::setPosition(sf::Vector2i(-15, 20), *window);
	EXPECT_EQ(ui.mousePosition(), sf::Vector2i(-15, 20));

	sf::Mouse::setPosition(old);
}

/**
 * Utility function which creates a string list from a list of numbers.
 * @tparam T    The specific type of number to extract.
 * @param  list The list to extract numbers from.
 * @return The string list.
 */
template<typename T>
std::string createList(const std::vector<T>& list) {
	std::string ret;
	for (auto& i : list) {
		ret += "   " + std::to_string(i);
	}
	return ret;
}

/**
 * Utility function which creates a string list from a list of \c sfx::joystick
 * objects.
 * @param  list The list to extract axes from.
 * @return The string list.
 */
std::string createListAxes(const sfx::JoystickAxisList& list) {
	std::string ret;
	for (auto& i : list) {
		ret += "   (" + std::to_string(i.axis) + ":" +
			std::to_string(i.direction) + ")";
	}
	return ret;
}

/**
 * Sets up a live environment in which the user can test the primary functions of
 * \c user_input.
 * The environment will allow the user to manually set the focus on and off the
 * window that will be tied to the \c user_input object.\n
 * This will allow the user to test the following methods:
 * <ul><li>\c update()</li>
 *     <li>\c operator[]()</li>
 *     <li>\c keyboardKeysBeingPressed()</li>
 *     <li>\c mouseButtonsBeingPressed()</li>
 *     <li>\c joystickButtonsBeingPressed()</li>
 *     <li>\c joystickAxesBeingPressed()</li></ul>
 */
TEST_F(UserInputTest, NormalOperationTiedToWindow) {
	sfx::fonts fonts;
	fonts.load(getTestAssetPath("fonts/fonts.json"));
	// operation won't work if the font didn't load
	ASSERT_TRUE(fonts["dialogue"]);
	sf::Text info("Close Window to Continue With Testing --- Press Esc to reset "
		"\"Up?\" and \"Select?\"", *fonts["dialogue"]);
	info.setFillColor(sf::Color::Red);
	sf::Text keyboard("Keyboard Keys", *fonts["dialogue"]);
	sf::Text mouse("Mouse Buttons", *fonts["dialogue"]);
	sf::Text joystickbtn("Joystick Buttons", *fonts["dialogue"]);
	sf::Text joystickaxes("Joystick Axes", *fonts["dialogue"]);
	sf::Text up("Up?", *fonts["dialogue"]);
	sf::Text select("Select?", *fonts["dialogue"]);
	sf::Text hold("Hold?", *fonts["dialogue"]);
	sf::Text windowfocus("Window Has Focus?", *fonts["dialogue"]);

	std::cout << ui.getConfiguration("up").keyboard[0] << std::endl;
	std::uint64_t upcounter = 0, selectcounter = 0;
	while (window->isOpen()) {
		sf::Event event;
		while (window->pollEvent(event)) {
			if (event.type == sf::Event::Closed) window->close();
			if (event.type == sf::Event::KeyReleased &&
				event.key.code == sf::Keyboard::Escape) {
				upcounter = 0;
				selectcounter = 0;
			}
		}
		ui.update();

		// Keyboard Keys
		keyboard.setString("Keyboard Keys:" +
			createList(ui.keyboardKeysBeingPressed()));
		// Mouse Buttons
		mouse.setString("Mouse Buttons:" +
			createList(ui.mouseButtonsBeingPressed()));
		// Joystick Buttons
		joystickbtn.setString("Joystick Buttons:" +
			createList(ui.joystickButtonsBeingPressed()));
		// Joystick Axes
		joystickaxes.setString("Joystick Axes:" +
			createListAxes(ui.joystickAxesBeingPressed()));
		// Up?
		if (ui["up"]) upcounter++;
		std::string controlCaption = "Up?";
		for (std::uint64_t x = 0; x < upcounter; x++)
			controlCaption += "   YES";
		up.setString(controlCaption);
		// Select?
		if (ui["select"]) selectcounter++;
		controlCaption = "Select?";
		for (std::uint64_t x = 0; x < selectcounter; x++)
			controlCaption += "   YES";
		select.setString(controlCaption);
		// Hold?
		if (ui["hold"])
			hold.setString("Hold? YES");
		else
			hold.setString("Hold?");
		// Window Has Focus?
		if (window->hasFocus())
			windowfocus.setString("Window Has Focus? TRUE");
		else
			windowfocus.setString("Window Has Focus? FALSE");

		window->clear();
		window->draw(info,
			sf::RenderStates(sf::Transform().translate(5.0f, 5.0f)));
		window->draw(keyboard,
			sf::RenderStates(sf::Transform().translate(5.0f, 45.0f)));
		window->draw(mouse,
			sf::RenderStates(sf::Transform().translate(5.0f, 85.0f)));
		window->draw(joystickbtn,
			sf::RenderStates(sf::Transform().translate(5.0f, 125.0f)));
		window->draw(joystickaxes,
			sf::RenderStates(sf::Transform().translate(5.0f, 165.0f)));
		window->draw(up,
			sf::RenderStates(sf::Transform().translate(5.0f, 205.0f)));
		window->draw(select,
			sf::RenderStates(sf::Transform().translate(5.0f, 245.0f)));
		window->draw(hold,
			sf::RenderStates(sf::Transform().translate(5.0f, 285.0f)));
		window->draw(windowfocus,
			sf::RenderStates(sf::Transform().translate(5.0f, 325.0f)));
		window->display();
	}
}

#endif