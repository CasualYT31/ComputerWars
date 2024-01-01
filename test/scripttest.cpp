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

/**@file scripttest.hpp
 * Tests the \c engine::scripts class.
 */

#include "sharedfunctions.hpp"
#include "script.hpp"

/**
 * Registers the interface.
 * @param engine Pointer to the script engine to register the interface with.
 */
void registerInterface(asIScriptEngine* engine) {
	// simple function used to write to the log file
	static engine::logger log = engine::logger("from_script");
	engine->RegisterGlobalFunction("void info(const string& in)",
		asMETHODPR(engine::logger, write, (const std::string&), void),
		asCALL_THISCALL_ASGLOBAL, &log);
}

/**
 * This test fixture initialises the \c scripts object before each test.
 */
class ScriptTest : public ::testing::Test {
protected:
	/**
	 * Registers the interface and loads the test script/s.
	 */
	void SetUp() override {
		_scripts.registerInterface(registerInterface);
		// each time scripts are loaded from a folder, nothing at the script
		// engine level should fail. If it does, there's no point testing anything
		// else
		ASSERT_TRUE(_scripts.loadScripts(getTestAssetPath("script/")));
	}

	/**
	 * The \c scripts object to test on.
	 */
	engine::scripts _scripts;
};

/**
 * Tests behaviour of \c engine::scripts::loadScripts() and
 * \c engine::scripts::getScriptsFolder().
 */
TEST_F(ScriptTest, GetScriptsFolder) {
	EXPECT_EQ(getTestAssetPath("script/"), _scripts.getScriptsFolder());
	ASSERT_TRUE(_scripts.loadScripts("badpath"));
	EXPECT_EQ("badpath", _scripts.getScriptsFolder());
	ASSERT_TRUE(_scripts.loadScripts(getTestAssetPath("script/")));
	EXPECT_EQ(getTestAssetPath("script/"), _scripts.getScriptsFolder());
}

/**
 * Tests behaviour of \c engine::scripts::functionExists().
 */
TEST_F(ScriptTest, FunctionExists) {
	EXPECT_FALSE(_scripts.functionExists("overloadedFunction"));
	EXPECT_TRUE(_scripts.functionExists("add"));
	EXPECT_FALSE(_scripts.functionExists("null"));
	EXPECT_TRUE(_scripts.functionExists("print"));
	EXPECT_FALSE(_scripts.functionExists("subtract"));
	ASSERT_TRUE(_scripts.loadScripts(getTestAssetPath("script2/")));
	EXPECT_FALSE(_scripts.functionExists("overloadedFunction"));
	EXPECT_FALSE(_scripts.functionExists("add"));
	EXPECT_FALSE(_scripts.functionExists("null"));
	EXPECT_TRUE(_scripts.functionExists("print"));
	EXPECT_TRUE(_scripts.functionExists("subtract"));
}

/**
 * Tests correct calls to \c engine::scripts::callFunction().
 */
TEST_F(ScriptTest, CorrectFunctionCalls) {
	EXPECT_FALSE(_scripts.callFunction("overloadedFunction", 9));
	EXPECT_IN_LOG("Failed to access function \"overloadedFunction\": ");
	EXPECT_TRUE(_scripts.callFunction("printInt", 9));
	EXPECT_IN_LOG("scriptanswer: 9");
	EXPECT_TRUE(_scripts.callFunction("printFloat", -56.9f));
	EXPECT_IN_LOG("scriptanswer: -56.9");
	EXPECT_TRUE(_scripts.callFunction("add", (short)7, (short)-8));
	EXPECT_IN_LOG("scriptanswer: -1");
	std::string str = "ScriptTestCorrectFunctionCalls is working!";
	EXPECT_TRUE(_scripts.callFunction("print", &str));
	EXPECT_IN_LOG(str);
	ASSERT_TRUE(_scripts.loadScripts(getTestAssetPath("script2/")));
	EXPECT_FALSE(_scripts.callFunction("add", (short)89, (short)-34));
	EXPECT_IN_LOG("Failed to access function \"add\": ");
	EXPECT_TRUE(_scripts.callFunction("subtract", (short)4, (short)-4));
	EXPECT_IN_LOG("scriptanswer: 8");
	EXPECT_TRUE(_scripts.callFunction("printHello"));
	EXPECT_IN_LOG("Hello, World! from script");
}

/**
 * Tests incorrect calls to \c engine::scripts::callFunction().
 */
TEST_F(ScriptTest, IncorrectFunctionCalls) {
	engine::logger temp("b");
	temp.write("Starting...");
	// too few arguments
	EXPECT_FALSE(_scripts.callFunction("printInt"));
	EXPECT_FALSE(_scripts.callFunction("add", (short)8));
	EXPECT_FALSE(_scripts.callFunction("print"));
	// too many arguments
	EXPECT_FALSE(_scripts.callFunction("printInt", -72, 9));
	ASSERT_TRUE(_scripts.loadScripts(getTestAssetPath("script2/")));
	EXPECT_FALSE(_scripts.callFunction("printHello", 4.5));
	// wrong type of arguments
	std::string str = "ScriptTestIncorrectFunctionCalls in working!";
	EXPECT_FALSE(_scripts.callFunction("subtract", 4, 5));
	EXPECT_FALSE(_scripts.callFunction("print", 45.12));
	EXPECT_FALSE(_scripts.callFunction("printFloat", &str));
	// attempt to pass in object and reference
	EXPECT_FALSE(_scripts.callFunction("print", str));
	std::string& ref = str;
	EXPECT_FALSE(_scripts.callFunction("print", ref));
	// attempt to pass in NULL pointer
	std::string* ptr = nullptr;
	EXPECT_FALSE(_scripts.callFunction("print", ptr));
}