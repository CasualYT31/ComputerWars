/*Copyright 2019-2021 CasualYouTuber31 <naysar@protonmail.com>

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

/**@file jsontest.h
 * Tests the \c engine::json and \c engine::json_state classes.
 */

#include "sharedfunctions.h"
#include "safejson.h"

/**
 * This test fixture is used to initialise \c json objects with values before
 * testing.
 */
class JsonTest : public ::testing::Test {
protected:
	/**
	 * Assigns the \c json object with example data from Nlohmann's README.
	 */
	void SetUp() override {
		j = R"({
			"pi": 3.141,
			"happy": true,
			"name": "John",
			"nothing": null,
			"answer": {
				"everything": 42
			},
			"list": [1, 0, 2],
			"object": {
				"currency": "USD",
				"value": -42
			},
			"colour": [255, 180, 255, 255]
		})"_json;
	}

	/**
	 * The \c json object to test on.
	 */
	engine::json j = engine::json(std::string("name:test_json"));
};

/**
 * This function tests an empty \c engine::json object.
 */
TEST_F(JsonTest, EmptyJson) {
	engine::json empty_j(std::string("name:test_empty_json"));
	EXPECT_FALSE(empty_j.keysExist({ "test", "test" }));
	EXPECT_FALSE(empty_j.keysExist({}));
}

/**
 * This function tests \c engine::json::apply() and \c engine::json_state.
 */
TEST_F(JsonTest, ApplyJSONState) {
	engine::json apply_j(std::string("name:test_apply_json"));
	// test apply() NO_KEYS_GIVEN
	int holder = 0;
	apply_j.apply(holder, {});
	EXPECT_TRUE(apply_j.whatFailed() & engine::json_state::NO_KEYS_GIVEN);
	// test resetState()
	apply_j.resetState();
	EXPECT_TRUE(apply_j.inGoodState());
	// test apply() KEYS_DID_NOT_EXIST
	apply_j.apply(holder, { "test" });
	EXPECT_TRUE(apply_j.whatFailed() & engine::json_state::KEYS_DID_NOT_EXIST);
}

/**
 * This function tests \c engine::json::operator=().
 */
TEST_F(JsonTest, AssignmentOperator) {
	EXPECT_FALSE(j.keysExist({ "test" }));
	EXPECT_TRUE(j.keysExist({ "object", "value" }));
}

/**
 * This function tests \c engine::json::apply() and the \c MISMATCHING_TYPE state.
 */
TEST_F(JsonTest, ApplyMismatchingType) {
	int holder = 0;
	j.apply(holder, { "happy" });
	EXPECT_TRUE(j.whatFailed() & engine::json_state::MISMATCHING_TYPE);
	j.resetState();
	j.apply(holder, { "pi" });
	EXPECT_TRUE(j.whatFailed() & engine::json_state::MISMATCHING_TYPE);
}

/**
 * This function tests if \c engine::json::apply() works as intended.
 * This includes if it outputs to the log file as necessary.
 */
TEST_F(JsonTest, Apply) {
	int holder = 0;
	j.apply(holder, { "answer", "everything" });
	EXPECT_TRUE(j.inGoodState());
	EXPECT_EQ(holder, 42);
	EXPECT_IN_LOG("name:test_json");
}

/**
 * This function tests \c engine::json::applyArray() and the \c MISMATCHING_SIZE
 * state.
 */
TEST_F(JsonTest, ApplyArrayMismatchingSize) {
	std::array<int, 2> holderArrayError;
	j.applyArray(holderArrayError, { "list" });
	EXPECT_TRUE(j.whatFailed() & engine::json_state::MISMATCHING_SIZE);
	j.resetState();
	std::array<int, 4> holderArrayTooBig;
	j.applyArray(holderArrayTooBig, { "list" });
	EXPECT_TRUE(j.whatFailed() & engine::json_state::MISMATCHING_SIZE);
}

/**
 * This function tests \c engine::json::applyArray() and the
 * \c MISMATCHING_ELEMENT_TYPE state.
 */
TEST_F(JsonTest, ApplyArrayMismatchingElementType) {
	std::array<std::string, 3> holderArrayBadType;
	j.applyArray(holderArrayBadType, { "list" });
	EXPECT_TRUE(j.whatFailed() & engine::json_state::MISMATCHING_ELEMENT_TYPE);
}

/**
 * This function tests if \c engine::json::applyArray() works as intended.
 */
TEST_F(JsonTest, ApplyArray) {
	std::array<int, 3> holderArray;
	j.applyArray(holderArray, { "list" });
	EXPECT_TRUE(j.inGoodState());
	EXPECT_EQ(holderArray[2], 2);
}

/**
 * This function tests if \c engine::json::applyColour() works as intended.
 */
TEST_F(JsonTest, ApplyColour) {
	sf::Color recipient;
	j.applyColour(recipient, { "colour" });
	EXPECT_TRUE(j.inGoodState());
	EXPECT_EQ(recipient.r, 255);
	EXPECT_EQ(recipient.g, 180);
}