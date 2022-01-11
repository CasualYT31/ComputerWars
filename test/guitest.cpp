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

/**@file guitest.h
 * Tests the \c sfx::gui class.
 */

#include "sharedfunctions.h"
#include "gui.h"

/**
 * Registers the interface.
 * @param engine Pointer to the script engine to register the interface with.
 */
void registerInterfaceGUI(asIScriptEngine* engine) {

}

/**
 * This test fixture is used to initialise a \c gui object for testing.
 */
class GUITest : public ::testing::Test {
protected:
	/**
	 * Registers the GUI object.
	 */
	void SetUp() override {
		_scripts->registerInterface(registerInterfaceGUI);
		EXPECT_TRUE(_scripts->loadScripts(getTestAssetPath("gui/")));
		_gui = std::make_shared<sfx::gui>(_scripts);
	}

	/**
	 * The \c gui object to test on.
	 */
	std::shared_ptr<sfx::gui> _gui;

	/**
	 * The \c scripts object containing the signal handlers.
	 */
	std::shared_ptr<engine::scripts> _scripts =
		std::make_shared<engine::scripts>();
};

/**
 * Test behaviour of an \c sfx::gui object when it is empty.
 * Also tests that the object is writing to the log file as expected.
 */
TEST_F(GUITest, EmptyGUI) {
	EXPECT_NOT_IN_LOG("[gui_");
	_gui->setGUI("testing");
	EXPECT_IN_LOG("[gui_");
}

/**
 * Test behaviour when loading an invalid JSON script.
 */
TEST_F(GUITest, InvalidJSONScript) {

}

/**
 * Test behaviour when loading a valid JSON script.
 */
TEST_F(GUITest, ValidJSONScript) {

}

#ifdef COMPUTER_WARS_FULL_GUI_TESTING

/**
 * Launch the GUI testing environment.
 * @todo Add button which toggles language dictionary.
 */
TEST_F(GUITest, Environment) {

}

#endif