#include "angelscript/AngelScriptEngine.hpp"
#include "log/TestHelpers.hpp"
#include "mvc/Controller.hpp"
#include "ScriptModel.hpp"
#include "TestHelpers.hpp"

#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>

class ScriptModelTests : public testing::Test {
public:
    ScriptModelTests(const std::string& documentationOutputFile = "") {
        controller = std::make_shared<cw::Controller>();
        model = std::make_shared<cw::model::Script>(documentationOutputFile);
        controller->attachModel("model", model);
    }

protected:
    std::shared_ptr<cw::ControllerNode> controller;
    std::shared_ptr<cw::model::Script> model;
};

TEST_F(ScriptModelTests, FromJSONSuccess) {
    model->fromJSON(R"({ "modules": { "main": { "folder": "AngelScriptSpecificTests" } } })"_json);
}

// TODO: CW-27: FromJSON tests will be a lot more useful once the outside can query the state of the internal script engine.

TEST_F(ScriptModelTests, ToJSONSuccess) {
    const auto input = R"({ "modules": { "main": { "folder": "AngelScriptSpecificTests" } } })"_json;
    model->fromJSON(input);
    cw::json output;
    model->toJSON(output);
    EXPECT_EQ(input, output);
}

class ParentDependency : public cw::Model {
public:
    ParentDependency(const cw::command::RegisterInterface::Callback& cb) : _cb(cb) {}

    void registerModel(const std::shared_ptr<cw::ReadWriteController>& controller) final {
        const cw::command::RegisterInterface cmd = cw::command::RegisterInterface().IAm<ParentDependency>().AndIWant(_cb);
        SEND_COMMAND(controller, cmd);
    }

    void fromJSON(const cw::json& j) final {}

    void toJSON(cw::json& j) const final {}

private:
    cw::command::RegisterInterface::Callback _cb;
};

class Parent : public cw::Model {
public:
    Parent(const cw::command::RegisterInterface::Callback& cb) : _cb(cb) {}

    void registerModel(const std::shared_ptr<cw::ReadWriteController>& controller) final {
        const cw::command::RegisterInterface cmd =
            cw::command::RegisterInterface().IAm<Parent>().IDependOn<ParentDependency>().AndIWant(_cb);
        SEND_COMMAND(controller, cmd);
    }

    void fromJSON(const cw::json& j) final {}

    void toJSON(cw::json& j) const final {}

private:
    cw::command::RegisterInterface::Callback _cb;
};

class TestModelForScriptModelTests : public cw::Model {
public:
    TestModelForScriptModelTests(const cw::command::RegisterInterface::Callback& cb) : _cb(cb) {}

    void registerModel(const std::shared_ptr<cw::ReadWriteController>& controller) final {
        const cw::command::RegisterInterface cmd =
            cw::command::RegisterInterface().IAm<TestModelForScriptModelTests>().IDependOn<Parent>().AndIWant(_cb);
        SEND_COMMAND(controller, cmd);
    }

    void fromJSON(const cw::json& j) final {}

    void toJSON(cw::json& j) const final {}

private:
    cw::command::RegisterInterface::Callback _cb;
};

class TestModelForScriptModelTests2 : public cw::Model {
public:
    TestModelForScriptModelTests2(const cw::command::RegisterInterface::Callback& cb) : _cb(cb) {}

    void registerModel(const std::shared_ptr<cw::ReadWriteController>& controller) final {
        const cw::command::RegisterInterface cmd =
            cw::command::RegisterInterface().IAm<TestModelForScriptModelTests2>().IDependOn<Parent>().AndIWant(_cb);
        SEND_COMMAND(controller, cmd);
    }

    void fromJSON(const cw::json& j) final {}

    void toJSON(cw::json& j) const final {}

private:
    cw::command::RegisterInterface::Callback _cb;
};

class TestModelForScriptModelTestsCircular2;

class TestModelForScriptModelTestsCircular1 : public cw::Model {
public:
    TestModelForScriptModelTestsCircular1(const cw::command::RegisterInterface::Callback& cb) : _cb(cb) {}

    void registerModel(const std::shared_ptr<cw::ReadWriteController>& controller) final {
        const cw::command::RegisterInterface cmd = cw::command::RegisterInterface()
                                                       .IAm<TestModelForScriptModelTestsCircular1>()
                                                       .IDependOn<TestModelForScriptModelTestsCircular2>()
                                                       .AndIWant(_cb);
        SEND_COMMAND(controller, cmd);
    }

    void fromJSON(const cw::json& j) final {}

    void toJSON(cw::json& j) const final {}

private:
    cw::command::RegisterInterface::Callback _cb;
};

class TestModelForScriptModelTestsCircular2 : public cw::Model {
public:
    TestModelForScriptModelTestsCircular2(const cw::command::RegisterInterface::Callback& cb) : _cb(cb) {}

    void registerModel(const std::shared_ptr<cw::ReadWriteController>& controller) final {
        const cw::command::RegisterInterface cmd = cw::command::RegisterInterface()
                                                       .IAm<TestModelForScriptModelTestsCircular2>()
                                                       .IDependOn<TestModelForScriptModelTestsCircular1>()
                                                       .AndIWant(_cb);
        SEND_COMMAND(controller, cmd);
    }

    void fromJSON(const cw::json& j) final {}

    void toJSON(cw::json& j) const final {}

private:
    cw::command::RegisterInterface::Callback _cb;
};

TEST(RegisterInterfaceLowLevelTests, CopyConstructorSuccess) {
    std::size_t counter = 0;
    cw::command::RegisterInterface test =
        cw::command::RegisterInterface()
            .IAm<Parent>()
            .IDependOn<ParentDependency, TestModelForScriptModelTests, TestModelForScriptModelTests2>()
            .AndIWant([&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; });
    auto testCopy(test);
    EXPECT_EQ(test.whoAreThey(), testCopy.whoAreThey());
    EXPECT_THAT(testCopy.whoDoTheyDependOn(), testing::ContainerEq(test.whoDoTheyDependOn()));
    EXPECT_EQ(counter, 0);
    test.whatDoTheyWant()(nullptr);
    EXPECT_EQ(counter, 1);
    testCopy.whatDoTheyWant()(nullptr);
    EXPECT_EQ(counter, 2);
    EXPECT_NE(&test.IAm<Parent>().AndIWant({}), &testCopy.IAm<Parent>().AndIWant({}));
}

TEST(RegisterInterfaceLowLevelTests, MoveConstructorSuccess) {
    std::size_t counter = 0;
    cw::command::RegisterInterface test =
        cw::command::RegisterInterface()
            .IAm<Parent>()
            .IDependOn<ParentDependency, TestModelForScriptModelTests, TestModelForScriptModelTests2>()
            .AndIWant([&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; });
    const auto oldThis =
        &test.IAm<Parent>().AndIWant([&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; });
    auto testCopy(std::move(test));
    EXPECT_EQ(typeid(Parent), testCopy.whoAreThey());
    EXPECT_THAT(
        testCopy.whoDoTheyDependOn(),
        testing::ContainerEq(std::set{ std::type_index(typeid(ParentDependency)),
                                       std::type_index(typeid(TestModelForScriptModelTests)),
                                       std::type_index(typeid(TestModelForScriptModelTests2)) })
    );
    EXPECT_EQ(counter, 0);
    EXPECT_THROW(test.whatDoTheyWant()(nullptr), cw::AssertionError);
    EXPECT_EQ(counter, 0);
    testCopy.whatDoTheyWant()(nullptr);
    EXPECT_EQ(counter, 1);
    EXPECT_NE(oldThis, &testCopy.IAm<Parent>().AndIWant({}));
}

TEST(RegisterInterfaceLowLevelTests, CopyAssignmentSuccess) {
    std::size_t counter = 0;
    cw::command::RegisterInterface
        test = cw::command::RegisterInterface()
                   .IAm<Parent>()
                   .IDependOn<ParentDependency, TestModelForScriptModelTests, TestModelForScriptModelTests2>()
                   .AndIWant([&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }),
        testCopy;
    testCopy = test;
    EXPECT_EQ(test.whoAreThey(), testCopy.whoAreThey());
    EXPECT_THAT(testCopy.whoDoTheyDependOn(), testing::ContainerEq(test.whoDoTheyDependOn()));
    EXPECT_EQ(counter, 0);
    test.whatDoTheyWant()(nullptr);
    EXPECT_EQ(counter, 1);
    testCopy.whatDoTheyWant()(nullptr);
    EXPECT_EQ(counter, 2);
    EXPECT_NE(&test.IAm<Parent>().AndIWant({}), &testCopy.IAm<Parent>().AndIWant({}));
}

TEST(RegisterInterfaceLowLevelTests, MoveAssignmentSuccess) {
    std::size_t counter = 0;
    cw::command::RegisterInterface
        test = cw::command::RegisterInterface()
                   .IAm<Parent>()
                   .IDependOn<ParentDependency, TestModelForScriptModelTests, TestModelForScriptModelTests2>()
                   .AndIWant([&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }),
        testCopy;
    const auto oldThis =
        &test.IAm<Parent>().AndIWant([&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; });
    testCopy = std::move(test);
    EXPECT_EQ(typeid(Parent), testCopy.whoAreThey());
    EXPECT_THAT(
        testCopy.whoDoTheyDependOn(),
        testing::ContainerEq(std::set{ std::type_index(typeid(ParentDependency)),
                                       std::type_index(typeid(TestModelForScriptModelTests)),
                                       std::type_index(typeid(TestModelForScriptModelTests2)) })
    );
    EXPECT_EQ(counter, 0);
    EXPECT_THROW(test.whatDoTheyWant()(nullptr), cw::AssertionError);
    EXPECT_EQ(counter, 0);
    testCopy.whatDoTheyWant()(nullptr);
    EXPECT_EQ(counter, 1);
    EXPECT_NE(oldThis, &testCopy.IAm<Parent>().AndIWant({}));
}

TEST_F(ScriptModelTests, RegisterInterfaceAttachModelsSuccess) {
    std::size_t counter = 0;
    EXPECT_NO_THROW(controller->attachModel(
        "PD", std::make_shared<ParentDependency>([&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) {
            ++counter;
        })
    ));
    EXPECT_EQ(counter, 1);
    EXPECT_NO_THROW(controller->attachModel(
        "Test",
        std::make_shared<TestModelForScriptModelTests>(
            [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
        )
    ));
    EXPECT_EQ(counter, 1);
    EXPECT_NO_THROW(controller->attachModel(
        "Parent",
        std::make_shared<Parent>([&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; })
    ));
    EXPECT_EQ(counter, 3);
}

TEST_F(ScriptModelTests, RegisterInterfaceAttachModelsCircularDependencyFailure) {
    std::size_t counter = 0;
    EXPECT_NO_THROW(controller->attachModel(
        "PD", std::make_shared<ParentDependency>([&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) {
            ++counter;
        })
    ));
    EXPECT_EQ(counter, 1);
    EXPECT_NO_THROW(controller->attachModel(
        "Test2",
        std::make_shared<TestModelForScriptModelTestsCircular2>(
            [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
        )
    ));
    EXPECT_EQ(counter, 1);
    EXPECT_NO_THROW(controller->attachModel(
        "Test1",
        std::make_shared<TestModelForScriptModelTestsCircular1>(
            [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
        )
    ));
    EXPECT_EQ(counter, 1);
    EXPECT_LOG(
        controller->fromJSON(R"({ "PD": {}, "Test2": {}, "Test1": {}, "model": {} })"_json),
        testing::HasSubstr("2 models could not register their script interfaces!")
    );
}

TEST_F(ScriptModelTests, RegisterInterfaceAttachModelsFailureTestModelThenSuccessTestModel) {
    std::size_t counter = 0;
    EXPECT_NO_THROW(controller->attachModel(
        "PD", std::make_shared<ParentDependency>([&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) {
            ++counter;
        })
    ));
    EXPECT_EQ(counter, 1);
    EXPECT_NO_THROW(controller->attachModel(
        "Test",
        std::make_shared<TestModelForScriptModelTests>(
            [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
        )
    ));
    EXPECT_EQ(counter, 1);
    EXPECT_NO_THROW(controller->attachModel(
        "Test2",
        std::make_shared<TestModelForScriptModelTests2>(
            [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { throw std::runtime_error(""); }
        )
    ));
    EXPECT_EQ(counter, 1);
    EXPECT_THROW(
        controller->attachModel(
            "Parent",
            std::make_shared<Parent>([&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; })
        ),
        std::runtime_error
    );
    EXPECT_EQ(counter, 2);
}

TEST_F(ScriptModelTests, RegisterInterfaceAttachModelsSuccessTestModelThenFailureTestModel) {
    std::size_t counter = 0;
    EXPECT_NO_THROW(controller->attachModel(
        "PD", std::make_shared<ParentDependency>([&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) {
            ++counter;
        })
    ));
    EXPECT_EQ(counter, 1);
    EXPECT_NO_THROW(controller->attachModel(
        "Test",
        std::make_shared<TestModelForScriptModelTests>(
            [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { throw std::runtime_error(""); }
        )
    ));
    EXPECT_EQ(counter, 1);
    EXPECT_NO_THROW(controller->attachModel(
        "Test2",
        std::make_shared<TestModelForScriptModelTests2>(
            [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
        )
    ));
    EXPECT_EQ(counter, 1);
    EXPECT_THROW(
        controller->attachModel(
            "Parent",
            std::make_shared<Parent>([&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; })
        ),
        std::runtime_error
    );
    EXPECT_EQ(counter, 3);
}

TEST_F(ScriptModelTests, RegisterInterfaceEmptyDeathTest) {
    EXPECT_THROW(COMMAND(controller, cw::command::RegisterInterface, ()), cw::AssertionError);
}

TEST_F(ScriptModelTests, RegisterInterfaceIAmDeathTest) {
    cw::command::RegisterInterface cmd;
#ifdef __GNUC__
    #pragma GCC diagnostic ignored "-Wunused-result"
#endif
    cmd.IAm<Test>();
#ifdef __GNUC__
    #pragma GCC diagnostic pop
#endif
    EXPECT_THROW(SEND_COMMAND(controller, cmd), cw::AssertionError);
}

TEST_F(ScriptModelTests, RegisterInterfaceIAmIDependOnDeathTest) {
    cw::command::RegisterInterface cmd;
#ifdef __GNUC__
    #pragma GCC diagnostic ignored "-Wunused-result"
#endif
    cmd.IAm<Test>().IDependOn<Parent>();
#ifdef __GNUC__
    #pragma GCC diagnostic pop
#endif
    EXPECT_THROW(SEND_COMMAND(controller, cmd), cw::AssertionError);
}

TEST_F(ScriptModelTests, RegisterInterfaceIAmAndIWantEmptySuccess) {
    std::size_t counter = 0;
    EXPECT_NO_THROW(SEND_COMMAND(
        controller,
        (cw::command::RegisterInterface().IAm<Parent>().AndIWant(
            [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
        ))
    ));
    EXPECT_EQ(counter, 1);
}

TEST_F(ScriptModelTests, RegisterInterfaceIAmIDependOnEmptyAndIWantEmptySuccess) {
    std::size_t counter = 0;
    EXPECT_NO_THROW(SEND_COMMAND(
        controller,
        (cw::command::RegisterInterface().IAm<Parent>().IDependOn().AndIWant(
            [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
        ))
    ));
    EXPECT_EQ(counter, 1);
}

TEST_F(ScriptModelTests, RegisterInterfaceIAmIDependOnAndIWantEmptySuccess) {
    std::size_t counter = 0;
    EXPECT_NO_THROW(SEND_COMMAND(
        controller,
        (cw::command::RegisterInterface().IAm<Test>().IDependOn<Parent>().AndIWant(
            [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
        ))
    ));
    EXPECT_EQ(counter, 0);
}

TEST_F(ScriptModelTests, RegisterInterfaceNoWaitForDependencySuccess) {
    std::size_t counter = 0;
    EXPECT_NO_THROW(SEND_COMMAND(
        controller,
        (cw::command::RegisterInterface().IAm<Parent>().AndIWant(
            [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
        ))
    ));
    EXPECT_EQ(counter, 1);
    EXPECT_NO_THROW(SEND_COMMAND(
        controller,
        (cw::command::RegisterInterface().IAm<Test>().IDependOn<Parent>().AndIWant(
            [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
        ))
    ));
    EXPECT_EQ(counter, 2);
}

TEST_F(ScriptModelTests, RegisterInterfaceWaitForDependencySuccess) {
    std::size_t counter = 0;
    EXPECT_NO_THROW(SEND_COMMAND(
        controller,
        (cw::command::RegisterInterface().IAm<Test>().IDependOn<Parent>().AndIWant(
            [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
        ))
    ));
    EXPECT_EQ(counter, 0);
    EXPECT_NO_THROW(SEND_COMMAND(
        controller,
        (cw::command::RegisterInterface().IAm<Parent>().AndIWant(
            [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
        ))
    ));
    EXPECT_EQ(counter, 2);
}

TEST_F(ScriptModelTests, RegisterInterfaceWaitLongerForDependencySuccess) {
    std::size_t counter = 0;
    EXPECT_NO_THROW(SEND_COMMAND(
        controller,
        (cw::command::RegisterInterface().IAm<Test>().IDependOn<Parent>().AndIWant(
            [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
        ))
    ));
    EXPECT_EQ(counter, 0);
    EXPECT_NO_THROW(SEND_COMMAND(
        controller,
        (cw::command::RegisterInterface().IAm<ParentDependency>().AndIWant(
            [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
        ))
    ));
    EXPECT_EQ(counter, 1);
    EXPECT_NO_THROW(SEND_COMMAND(
        controller,
        (cw::command::RegisterInterface().IAm<Parent>().AndIWant(
            [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
        ))
    ));
    EXPECT_EQ(counter, 3);
}

TEST_F(ScriptModelTests, RegisterInterfaceWaitForIndirectDependencySuccess) {
    std::size_t counter = 0;
    EXPECT_NO_THROW(SEND_COMMAND(
        controller,
        (cw::command::RegisterInterface().IAm<Test>().IDependOn<Parent>().AndIWant(
            [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
        ))
    ));
    EXPECT_EQ(counter, 0);
    EXPECT_NO_THROW(SEND_COMMAND(
        controller,
        (cw::command::RegisterInterface().IAm<Parent>().IDependOn<ParentDependency>().AndIWant(
            [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
        ))
    ));
    EXPECT_EQ(counter, 0);
    EXPECT_NO_THROW(SEND_COMMAND(
        controller,
        (cw::command::RegisterInterface().IAm<ParentDependency>().AndIWant(
            [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
        ))
    ));
    EXPECT_EQ(counter, 3);
}

TEST_F(ScriptModelTests, RegisterInterfaceWaitForIndirectDependencyOutOfOrderSuccess) {
    std::size_t counter = 0;
    EXPECT_NO_THROW(SEND_COMMAND(
        controller,
        (cw::command::RegisterInterface().IAm<Parent>().IDependOn<ParentDependency>().AndIWant(
            [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
        ))
    ));
    EXPECT_EQ(counter, 0);
    EXPECT_NO_THROW(SEND_COMMAND(
        controller,
        (cw::command::RegisterInterface().IAm<Test>().IDependOn<Parent>().AndIWant(
            [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
        ))
    ));
    EXPECT_EQ(counter, 0);
    EXPECT_NO_THROW(SEND_COMMAND(
        controller,
        (cw::command::RegisterInterface().IAm<ParentDependency>().AndIWant(
            [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
        ))
    ));
    EXPECT_EQ(counter, 3);
}

TEST_F(ScriptModelTests, RegisterInterfaceWaitForMultipleDependenciesSuccess) {
    std::size_t counter = 0;
    EXPECT_NO_THROW(SEND_COMMAND(
        controller,
        (cw::command::RegisterInterface().IAm<Parent>().IDependOn<Test, ParentDependency>().AndIWant(
            [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
        ))
    ));
    EXPECT_EQ(counter, 0);
    EXPECT_NO_THROW(SEND_COMMAND(
        controller,
        (cw::command::RegisterInterface().IAm<Test>().AndIWant(
            [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
        ))
    ));
    EXPECT_EQ(counter, 1);
    EXPECT_NO_THROW(SEND_COMMAND(
        controller,
        (cw::command::RegisterInterface().IAm<ParentDependency>().AndIWant(
            [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
        ))
    ));
    EXPECT_EQ(counter, 3);
}

TEST_F(ScriptModelTests, RegisterInterfaceWaitForMultipleDependenciesWithOneAlreadyRegisteredSuccess) {
    std::size_t counter = 0;
    EXPECT_NO_THROW(SEND_COMMAND(
        controller,
        (cw::command::RegisterInterface().IAm<Test>().AndIWant(
            [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
        ))
    ));
    EXPECT_EQ(counter, 1);
    EXPECT_NO_THROW(SEND_COMMAND(
        controller,
        (cw::command::RegisterInterface().IAm<Parent>().IDependOn<Test, ParentDependency>().AndIWant(
            [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
        ))
    ));
    EXPECT_EQ(counter, 1);
    EXPECT_NO_THROW(SEND_COMMAND(
        controller,
        (cw::command::RegisterInterface().IAm<ParentDependency>().AndIWant(
            [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
        ))
    ));
    EXPECT_EQ(counter, 3);
}

TEST_F(ScriptModelTests, RegisterInterfaceRemoveFailingRegistrant) {
    std::size_t counter = 0;
    EXPECT_THROW(
        SEND_COMMAND(
            controller,
            (cw::command::RegisterInterface().IAm<Parent>().AndIWant(
                [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) {
                    if (counter++ == 0) { throw std::runtime_error(""); }
                }
            ))
        ),
        std::runtime_error
    );
    EXPECT_EQ(counter, 1);
    EXPECT_NO_THROW(SEND_COMMAND(
        controller,
        (cw::command::RegisterInterface().IAm<ParentDependency>().AndIWant(
            [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
        ))
    ));
    EXPECT_EQ(counter, 2);
}

TEST_F(ScriptModelTests, RegisterInterfaceCircularDependencies) {
    std::size_t counter = 0;
    EXPECT_NO_THROW(SEND_COMMAND(
        controller,
        (cw::command::RegisterInterface().IAm<Test>().IDependOn<Parent>().AndIWant(
            [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
        ))
    ));
    EXPECT_EQ(counter, 0);
    EXPECT_NO_THROW(SEND_COMMAND(
        controller,
        (cw::command::RegisterInterface().IAm<ParentDependency>().AndIWant(
            [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
        ))
    ));
    EXPECT_EQ(counter, 1);
    EXPECT_NO_THROW(SEND_COMMAND(
        controller,
        (cw::command::RegisterInterface().IAm<Parent>().IDependOn<Test>().AndIWant(
            [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
        ))
    ));
    EXPECT_EQ(counter, 1);
}

TEST_F(ScriptModelTests, RegisterInterfaceSelfDependencyDeathTest) {
    std::size_t counter = 0;
    EXPECT_THROW(
        SEND_COMMAND(
            controller,
            (cw::command::RegisterInterface().IAm<Test>().IDependOn<Parent, Test>().AndIWant(
                [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
            ))
        ),
        cw::AssertionError
    );
    EXPECT_EQ(counter, 0);
    EXPECT_NO_THROW(SEND_COMMAND(
        controller,
        (cw::command::RegisterInterface().IAm<ParentDependency>().AndIWant(
            [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
        ))
    ));
    EXPECT_EQ(counter, 1);
}

TEST_F(ScriptModelTests, RegisterInterfaceDuplicateRegistrantAlreadyRegisteredDeathTest) {
    std::size_t counter = 0;
    EXPECT_NO_THROW(SEND_COMMAND(
        controller,
        (cw::command::RegisterInterface().IAm<ParentDependency>().AndIWant(
            [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
        ))
    ));
    EXPECT_EQ(counter, 1);
    EXPECT_THROW(
        SEND_COMMAND(
            controller,
            (cw::command::RegisterInterface().IAm<ParentDependency>().AndIWant(
                [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
            ))
        ),
        cw::AssertionError
    );
    EXPECT_EQ(counter, 1);
}

TEST_F(ScriptModelTests, RegisterInterfaceDuplicateRegistrantToBeRegisteredDeathTest) {
    std::size_t counter = 0;
    EXPECT_NO_THROW(SEND_COMMAND(
        controller,
        (cw::command::RegisterInterface().IAm<Test>().IDependOn<Parent>().AndIWant(
            [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
        ))
    ));
    EXPECT_EQ(counter, 0);
    EXPECT_THROW(
        SEND_COMMAND(
            controller,
            (cw::command::RegisterInterface().IAm<Test>().AndIWant(
                [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
            ))
        ),
        cw::AssertionError
    );
    EXPECT_EQ(counter, 0);
    EXPECT_NO_THROW(SEND_COMMAND(
        controller,
        (cw::command::RegisterInterface().IAm<Parent>().AndIWant(
            [&counter](const std::shared_ptr<cw::model::ScriptInterfaceDelegate>&) { ++counter; }
        ))
    ));
    EXPECT_EQ(counter, 2);
}
