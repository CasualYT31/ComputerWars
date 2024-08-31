#include "angelscript/AngelScriptEngine.hpp"
#include "file/File.hpp"
#include "ScriptEngine.hpp"
#include "ScriptEngineTypes.hpp"

#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>

template <typename Engine> class ScriptEngineTests : public testing::Test {
protected:
    Engine engine;
};

TYPED_TEST_SUITE(ScriptEngineTests, cw::ScriptEngineTypesToTest);

TYPED_TEST(ScriptEngineTests, CreateModuleSuccess) {
    EXPECT_TRUE(this->engine.createModule("module", false));
    EXPECT_TRUE(this->engine.moduleExists("module"));
    EXPECT_EQ(this->engine.getModuleCount(), 1);
    EXPECT_TRUE(this->engine.createModule("module", true));
    EXPECT_TRUE(this->engine.moduleExists("module"));
    EXPECT_EQ(this->engine.getModuleCount(), 1);
}

TYPED_TEST(ScriptEngineTests, CreateModuleFailure) {
    EXPECT_TRUE(this->engine.createModule("module", false));
    EXPECT_TRUE(this->engine.moduleExists("module"));
    EXPECT_EQ(this->engine.getModuleCount(), 1);
    EXPECT_FALSE(this->engine.createModule("module", false));
    EXPECT_TRUE(this->engine.moduleExists("module"));
    EXPECT_EQ(this->engine.getModuleCount(), 1);
}

TYPED_TEST(ScriptEngineTests, DiscardModuleSuccess) {
    EXPECT_TRUE(this->engine.createModule("moduleToDiscard", false));
    EXPECT_TRUE(this->engine.moduleExists("moduleToDiscard"));
    EXPECT_EQ(this->engine.getModuleCount(), 1);
    EXPECT_TRUE(this->engine.discardModule("moduleToDiscard"));
    EXPECT_FALSE(this->engine.moduleExists("moduleToDiscard"));
    EXPECT_EQ(this->engine.getModuleCount(), 0);
}

TYPED_TEST(ScriptEngineTests, DiscardModuleFailure) {
    EXPECT_EQ(this->engine.getModuleCount(), 0);
    EXPECT_FALSE(this->engine.discardModule("moduleToDiscard"));
    EXPECT_EQ(this->engine.getModuleCount(), 0);
}

TYPED_TEST(ScriptEngineTests, DiscardAllModulesSuccess) {
    EXPECT_TRUE(this->engine.createModule("module1", false));
    EXPECT_TRUE(this->engine.createModule("module2", false));
    EXPECT_TRUE(this->engine.createModule("module3", false));
    EXPECT_EQ(this->engine.getModuleCount(), 3);
    EXPECT_TRUE(this->engine.discardAllModules());
    EXPECT_EQ(this->engine.getModuleCount(), 0);
}

TYPED_TEST(ScriptEngineTests, GetModuleNamesSuccess) {
    EXPECT_TRUE(this->engine.createModule("module4", false));
    EXPECT_TRUE(this->engine.createModule("module5", false));
    EXPECT_TRUE(this->engine.createModule("module6", false));
    EXPECT_THAT(this->engine.getModuleNames(), testing::UnorderedElementsAre("module4", "module5", "module6"));
}

TYPED_TEST(ScriptEngineTests, SetUpDocumentationGeneratorSuccess) {
    EXPECT_FALSE(this->engine.documentationGeneratorIsSetUp());
    EXPECT_FALSE(this->engine.generateDocumentation());
    EXPECT_TRUE(this->engine.setUpDocumentationGenerator("tmp/ScriptInterface.html"));
    EXPECT_FALSE(this->engine.setUpDocumentationGenerator("tmp/ScriptInterface.html"));
    EXPECT_TRUE(this->engine.documentationGeneratorIsSetUp());
}

TEST(AngelScriptSpecificTests, LoadModuleSuccess) {
    cw::AngelScriptEngine engine;
    EXPECT_TRUE(engine.loadModule("ASTest", true, "AngelScriptSpecificTests", "\\.as$"));
    EXPECT_TRUE(engine.moduleExists("ASTest"));
    EXPECT_EQ(engine.getModuleCount(), 1);
    EXPECT_FALSE(engine.loadModule("ASTest", false, "AngelScriptSpecificTests", "\\.as$"));
}

TEST(AngelScriptSpecificTests, GenerateDocumentationSuccess) {
    cw::AngelScriptEngine engine;
    EXPECT_TRUE(engine.setUpDocumentationGenerator("tmp/ScriptInterface.html"));
    EXPECT_TRUE(engine.generateDocumentation());
    const auto output = cw::readEntireTextFile("tmp/ScriptInterface.html");
    EXPECT_THAT(output, testing::HasSubstr("<h2 name=\"string\">string</h2>"));
    // Could be a very large file, so try to delete once we're finished, but don't panic if it fails.
    try {
        std::filesystem::remove_all("tmp/ScriptInterface.html");
    } catch (...) {}
}

// TODO: should test loadModule() and generateDocumentation() more (don't forget to test file filtering)
