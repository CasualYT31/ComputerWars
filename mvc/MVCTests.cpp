#include "Controller.hpp"

#include "gtest/gtest.h"
#include <iostream>

struct TestCommand : public cw::Command {
    TestCommand(const std::string& s = "") : statement(s) {}

    std::string statement;
};

struct TestCommand2 : public cw::Command {
    TestCommand2(const std::string& s = "") : statement(s) {}

    std::string statement;
};

struct TestQuery : public cw::Query {
    using ReturnType = int;

    TestQuery() = default;

    TestQuery(const int _x, const int _y) : x(_x), y(_y) {}

    int x = 0;
    int y = 0;
};

struct TestQuery2 : public cw::Query {
    using ReturnType = int;

    TestQuery2() = default;

    TestQuery2(const int _x, const int _y) : x(_x), y(_y) {}

    int x = 0;
    int y = 0;
};

struct TestEvent : public cw::Event {
    TestEvent(const std::string& d = "") : data(d) {}

    std::string data;
};

struct TestEvent2 : public cw::Event {
    TestEvent2(const std::string& d = "") : data(d) {}

    std::string data;
};

class MVCTests : public testing::Test {
public:
    void testCommand(const cw::Command& c) {
        const auto& command = dynamic_cast<const TestCommand&>(c);
        _output += command.statement;
    }

    void testCommand2(const cw::Command& c) {
        const auto& command = dynamic_cast<const TestCommand2&>(c);
        _output += command.statement;
        _output += '2';
    }

    void testEventHandler(const cw::Event& e) {
        const auto& event = dynamic_cast<const TestEvent&>(e);
        _output += event.data;
    }

    void testEventHandler1(const cw::Event& e) {
        const auto& event = dynamic_cast<const TestEvent&>(e);
        _output += event.data;
        _output += '1';
    }

    void testEventHandler2(const cw::Event& e) {
        const auto& event = dynamic_cast<const TestEvent2&>(e);
        _output += event.data;
        _output += '2';
    }

protected:
    MVCTests() {
        root = std::make_shared<cw::Controller>();
    }

    std::string getCapturedOutput() {
        const auto cpy(_output);
        _output.clear();
        return cpy;
    }

    std::shared_ptr<cw::ControllerNode> root;

private:
    std::string _output;
};

std::any testQuery(const cw::Query& q) {
    const auto& query = dynamic_cast<const TestQuery&>(q);
    return query.x + query.y;
}

std::any testQuery2(const cw::Query& q) {
    const auto& query = dynamic_cast<const TestQuery2&>(q);
    return query.x - query.y;
}

class TestModel : public cw::Model {
public:
    void registerModel(const std::shared_ptr<cw::ReadWriteController>& controller) final {
        _controller = controller;
        controller->registerCommand(TestCommand{}, [controller = this->_controller](const cw::Command& c) {
            const auto& command = dynamic_cast<const TestCommand&>(c);
            EVENT(controller, TestEvent, (command.statement));
        });
        controller->registerQuery(TestQuery{}, [](const cw::Query& q) {
            const auto& query = dynamic_cast<const TestQuery&>(q);
            return query.x + query.y;
        });
        controller->registerEventHandler(TestEvent{}, [](const cw::Event& e) {
            const auto& event = dynamic_cast<const TestEvent&>(e);
            std::cout << event.data;
        });
    }

    void fromJSON(const cw::json& j) final {}

    void toJSON(cw::json& j) const final {}

private:
    std::shared_ptr<cw::ReadWriteController> _controller;
};

class TestModel2 : public cw::Model {
public:
    void registerModel(const std::shared_ptr<cw::ReadWriteController>& controller) final {
        controller->registerEventHandler(TestEvent{}, [](const cw::Event& e) {
            const auto& event = dynamic_cast<const TestEvent&>(e);
            std::cout << event.data;
        });
    }

    void fromJSON(const cw::json& j) final {}

    void toJSON(cw::json& j) const final {}
};

class TestView : public cw::View {
public:
    void registerView(const std::shared_ptr<cw::ReadOnlyController>& controller) final {
        controller->registerEventHandler(TestEvent{}, [&state = this->_stateCache](const cw::Event& e) {
            const auto& event = dynamic_cast<const TestEvent&>(e);
            state = event.data;
        });
    }

    void render() final {
        std::cout << _stateCache;
    }

private:
    std::string _stateCache;
};

class TestView2 : public cw::View {
public:
    void registerView(const std::shared_ptr<cw::ReadOnlyController>& controller) final {}

    void render() final {
        std::cout << "TestView2";
    }
};

TEST_F(MVCTests, RegisterCommandDeathTest) {
    EXPECT_DEATH(root->registerCommand(TestCommand{}, {}), "");
    REGISTER(root, Command, TestCommand, MVCTests::testCommand, this);
    EXPECT_DEATH(REGISTER(root, Command, TestCommand, MVCTests::testCommand, this), "");
    REGISTER(root, Command, TestCommand2, MVCTests::testCommand2, this);
}

TEST_F(MVCTests, RegisterQueryDeathTest) {
    EXPECT_DEATH(root->registerQuery(TestQuery{}, {}), "");
    root->registerQuery(TestQuery{}, testQuery);
    EXPECT_DEATH(root->registerQuery(TestQuery{}, testQuery), "");
    root->registerQuery(TestQuery2{}, testQuery2);
}

TEST_F(MVCTests, RegisterEventHandlerDeathTest) {
    EXPECT_DEATH(root->registerEventHandler(TestEvent{}, {}), "");
    REGISTER(root, EventHandler, TestEvent, MVCTests::testEventHandler, this);
    REGISTER(root, EventHandler, TestEvent, MVCTests::testEventHandler, this);
    REGISTER(root, EventHandler, TestEvent2, MVCTests::testEventHandler2, this);
}

TEST_F(MVCTests, SuccessfulCommand) {
    REGISTER(root, Command, TestCommand, MVCTests::testCommand, this);
    COMMAND(root, TestCommand, ("Message"));
    EXPECT_EQ(getCapturedOutput(), "Message");
}

TEST_F(MVCTests, CommandDeathTest) {
    EXPECT_DEATH(COMMAND(root, TestCommand, ("Message")), "");
}

TEST_F(MVCTests, MultipleSuccessfulCommands) {
    REGISTER(root, Command, TestCommand, MVCTests::testCommand, this);
    REGISTER(root, Command, TestCommand2, MVCTests::testCommand2, this);
    COMMAND(root, TestCommand, ("Message"));
    EXPECT_EQ(getCapturedOutput(), "Message");
    COMMAND(root, TestCommand2, ("Message"));
    EXPECT_EQ(getCapturedOutput(), "Message2");
}

TEST_F(MVCTests, SuccessfulQuery) {
    root->registerQuery(TestQuery{}, testQuery);
    const auto res = QUERY(root, TestQuery, (4, 5));
    EXPECT_EQ(res, 9);
}

TEST_F(MVCTests, QueryDeathTest) {
    EXPECT_DEATH(root->query(TestQuery{ 4, 5 }), "");
}

TEST_F(MVCTests, MultipleSuccessfulQueries) {
    root->registerQuery(TestQuery{}, testQuery);
    root->registerQuery(TestQuery2{}, testQuery2);
    const auto res = QUERY(root, TestQuery, (4, 5));
    EXPECT_EQ(res, 9);
    const auto res2 = QUERY(root, TestQuery2, (4, 5));
    EXPECT_EQ(res2, -1);
}

TEST_F(MVCTests, NoEventHandlers) {
    EVENT(root, TestEvent, ());
    EXPECT_EQ(getCapturedOutput(), "");
}

TEST_F(MVCTests, SingleEventSingleHandler) {
    REGISTER(root, EventHandler, TestEvent, MVCTests::testEventHandler, this);
    EVENT(root, TestEvent, ("Event"));
    EXPECT_EQ(getCapturedOutput(), "");
    root->tick();
    EXPECT_EQ(getCapturedOutput(), "Event");
    root->tick();
    EXPECT_EQ(getCapturedOutput(), "");
}

TEST_F(MVCTests, SingleEventMultipleHandlers1Then2) {
    REGISTER(root, EventHandler, TestEvent, MVCTests::testEventHandler, this);
    REGISTER(root, EventHandler, TestEvent, MVCTests::testEventHandler1, this);
    EVENT(root, TestEvent, ("Event"));
    EXPECT_EQ(getCapturedOutput(), "");
    root->tick();
    EXPECT_EQ(getCapturedOutput(), "EventEvent1");
    root->tick();
    EXPECT_EQ(getCapturedOutput(), "");
}

TEST_F(MVCTests, SingleEventMultipleHandlers2Then1) {
    REGISTER(root, EventHandler, TestEvent, MVCTests::testEventHandler1, this);
    REGISTER(root, EventHandler, TestEvent, MVCTests::testEventHandler, this);
    EVENT(root, TestEvent, ("Event"));
    EXPECT_EQ(getCapturedOutput(), "");
    root->tick();
    EXPECT_EQ(getCapturedOutput(), "Event1Event");
    root->tick();
    EXPECT_EQ(getCapturedOutput(), "");
}

TEST_F(MVCTests, MultipleEventsSingleHandlers) {
    REGISTER(root, EventHandler, TestEvent, MVCTests::testEventHandler, this);
    REGISTER(root, EventHandler, TestEvent2, MVCTests::testEventHandler2, this);
    EVENT(root, TestEvent, ("Event"));
    EXPECT_EQ(getCapturedOutput(), "");
    root->tick();
    EXPECT_EQ(getCapturedOutput(), "Event");
    root->tick();
    EXPECT_EQ(getCapturedOutput(), "");
    EVENT(root, TestEvent2, ("Event"));
    EXPECT_EQ(getCapturedOutput(), "");
    root->tick();
    EXPECT_EQ(getCapturedOutput(), "Event2");
    EVENT(root, TestEvent, ("Event"));
    EVENT(root, TestEvent2, ("Event"));
    root->tick();
    EXPECT_EQ(getCapturedOutput(), "EventEvent2");
    EVENT(root, TestEvent2, ("Event"));
    EVENT(root, TestEvent, ("Event"));
    EVENT(root, TestEvent2, ("Event"));
    root->tick();
    EXPECT_EQ(getCapturedOutput(), "Event2EventEvent2");
}

TEST_F(MVCTests, AttachChildControllerDeathTest) {
    EXPECT_DEATH(root->attachChildController("child", nullptr), "");
    const std::shared_ptr<cw::ControllerNode> child = std::make_shared<cw::Controller>();
    root->attachChildController("child", child);
    EXPECT_DEATH(root->attachChildController("child", child), "");
}

TEST_F(MVCTests, AttachModelDeathTest) {
    EXPECT_DEATH(root->attachModel("model", nullptr), "");
    const std::shared_ptr<cw::Model> model = std::make_shared<TestModel>();
    root->attachModel("model", model);
    EXPECT_DEATH(root->attachModel("model", model), "");
}

TEST_F(MVCTests, AttachChildControllerThenModelDeathTest) {
    const std::shared_ptr<cw::ControllerNode> child = std::make_shared<cw::Controller>();
    const std::shared_ptr<cw::Model> model = std::make_shared<TestModel>();
    root->attachChildController("name", child);
    EXPECT_DEATH(root->attachModel("name", model), "");
}

TEST_F(MVCTests, AttachModelThenChildControllerDeathTest) {
    const std::shared_ptr<cw::ControllerNode> child = std::make_shared<cw::Controller>();
    const std::shared_ptr<cw::Model> model = std::make_shared<TestModel>();
    root->attachModel("name", model);
    EXPECT_DEATH(root->attachChildController("name", child), "");
}

TEST_F(MVCTests, AttachViewDeathTest) {
    EXPECT_DEATH(root->attachView(nullptr), "");
    const std::shared_ptr<cw::View> view = std::make_shared<TestView>();
    root->attachView(view);
    EXPECT_DEATH(root->attachView(view), "");
}

TEST_F(MVCTests, ModelControllerIntegration) {
    const std::shared_ptr<cw::Model> model = std::make_shared<TestModel>();
    root->attachModel("model", model);

    COMMAND(root, TestCommand, ("Message"));
    testing::internal::CaptureStdout();
    root->tick();
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "Message");
    EXPECT_EQ(QUERY(root, TestQuery, (1, 2)), 3);

    const std::shared_ptr<cw::Model> model2 = std::make_shared<TestModel2>();
    root->attachModel("model2", model2);
    COMMAND(root, TestCommand, ("Message"));
    testing::internal::CaptureStdout();
    root->tick();
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "MessageMessage");
}

TEST_F(MVCTests, ModelViewControllerIntegration) {
    const std::shared_ptr<cw::Model> model = std::make_shared<TestModel>();
    const std::shared_ptr<cw::View> view = std::make_shared<TestView>();
    root->attachModel("model", model);
    root->attachView(view);

    testing::internal::CaptureStdout();
    root->tick();
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "");

    testing::internal::CaptureStdout();
    COMMAND(root, TestCommand, ("Message"));
    root->tick();
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "MessageMessage");

    testing::internal::CaptureStdout();
    root->tick();
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "Message");
}

TEST_F(MVCTests, ChildControllerTestsAndDeathTests) {
    const std::shared_ptr<cw::Model> model = std::make_shared<TestModel>();
    const std::shared_ptr<cw::View> view = std::make_shared<TestView>();
    const std::shared_ptr<cw::ControllerNode> child = std::make_shared<cw::Controller>();
    root->attachChildController("child", child);
    child->attachView(view);
    child->attachModel("model", model);

    testing::internal::CaptureStdout();
    root->tick();
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "");

    testing::internal::CaptureStdout();
    COMMAND(root, TestCommand, ("Message"));
    root->tick();
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "MessageMessage");

    testing::internal::CaptureStdout();
    root->tick();
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "Message");
}

TEST_F(MVCTests, GrandchildControllerTests) {
    const std::shared_ptr<cw::Model> model = std::make_shared<TestModel>();
    const std::shared_ptr<cw::View> view = std::make_shared<TestView>(), view2 = std::make_shared<TestView2>();
    const std::shared_ptr<cw::ControllerNode> child = std::make_shared<cw::Controller>(),
                                              grandchild = std::make_shared<cw::Controller>();
    child->attachChildController("grandchild", grandchild);
    root->attachChildController("child", child);
    grandchild->attachView(view);
    grandchild->attachModel("model", model);
    child->attachView(view2);

    testing::internal::CaptureStdout();
    root->tick();
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "TestView2");

    testing::internal::CaptureStdout();
    COMMAND(root, TestCommand, ("Message"));
    root->tick();
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "MessageTestView2Message");

    testing::internal::CaptureStdout();
    root->tick();
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "TestView2Message");
}

TEST_F(MVCTests, ShutdownTest) {
    EXPECT_EQ(root->tick(), cw::Continue);
    root->shutdown(1);
    EXPECT_EQ(root->tick(), 1);
    EXPECT_EQ(root->tick(), cw::Continue);
}
