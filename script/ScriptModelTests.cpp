#include "angelscript/AngelScriptEngine.hpp"
#include "ScriptEngineTypes.hpp"
#include "ScriptModel.hpp"

#include <gmock/gmock.h>

template <typename Engine> class ScriptModelTests : public testing::Test {
protected:
    cw::ScriptModel<Engine> model;
};

TYPED_TEST_SUITE(ScriptModelTests, cw::ScriptEngineTypesToTest);

TYPED_TEST(ScriptModelTests, FromJSONSuccess) {
    this->model.fromJSON(R"({ "modules": { "main": { "folder": "AngelScriptSpecificTests" } } })"_json);
}

// TODO: CW-27: FromJSON tests will be a lot more useful once the outside can query the state of the internal script engine.

TYPED_TEST(ScriptModelTests, ToJSONSuccess) {
    const auto input = R"({ "modules": { "main": { "folder": "AngelScriptSpecificTests" } } })"_json;
    this->model.fromJSON(input);
    cw::json output;
    this->model.toJSON(output);
    EXPECT_EQ(input, output);
}
