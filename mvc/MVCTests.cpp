#include "Controller.hpp"
#include "log/Log.hpp"

#include <gmock/gmock.h>
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

struct TestRequest : public cw::Request {
    using ReturnType = double;

    TestRequest() = default;

    TestRequest(const double _x, const double _y) : x(_x), y(_y) {}

    double x = 0;
    double y = 0;
};

struct TestRequest2 : public cw::Request {
    using ReturnType = double;

    TestRequest2() = default;

    TestRequest2(const double _x, const double _y) : x(_x), y(_y) {}

    double x = 0;
    double y = 0;
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
    DECLARE_COMMAND(testCommand) {
        RECEIVE_COMMAND(TestCommand);
        _output += command.statement;
    }

    DECLARE_COMMAND(testCommand2) {
        RECEIVE_COMMAND(TestCommand2);
        _output += command.statement;
        _output += '2';
    }

    DECLARE_EVENT(testEventHandler) {
        RECEIVE_EVENT(TestEvent);
        _output += event.data;
    }

    DECLARE_EVENT(testEventHandler1) {
        RECEIVE_EVENT(TestEvent);
        _output += event.data;
        _output += '1';
    }

    DECLARE_EVENT(testEventHandler2) {
        RECEIVE_EVENT(TestEvent2);
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

DECLARE_QUERY(testQuery) {
    RECEIVE_QUERY(TestQuery);
    return query.x + query.y;
}

DECLARE_QUERY(testQuery2) {
    RECEIVE_QUERY(TestQuery2);
    return query.x - query.y;
}

DECLARE_REQUEST(testRequest) {
    RECEIVE_REQUEST(TestRequest);
    return request.x * request.y;
}

DECLARE_REQUEST(testRequest2) {
    RECEIVE_REQUEST(TestRequest2);
    return request.x / request.y;
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

struct TestModel2SetX : public cw::Command {
    TestModel2SetX(const int s = 0) : x(s) {}

    int x;
};

struct TestModel2SetMessage : public cw::Command {
    TestModel2SetMessage(const std::string& s = "") : message(s) {}

    std::string message;
};

struct TestModel2QueryX : public cw::Query {
    using ReturnType = int;

    TestModel2QueryX() = default;
};

struct TestModel2QueryMessage : public cw::Query {
    using ReturnType = std::string;

    TestModel2QueryMessage() = default;
};

class TestModel2 : public cw::Model {
public:
    void registerModel(const std::shared_ptr<cw::ReadWriteController>& controller) final {
        REGISTER(controller, Command, TestModel2SetX, TestModel2::SetX, this);
        REGISTER(controller, Command, TestModel2SetMessage, TestModel2::SetMessage, this);
        REGISTER(controller, Query, TestModel2QueryX, TestModel2::GetX, this);
        REGISTER(controller, Query, TestModel2QueryMessage, TestModel2::GetMessage, this);
        controller->registerEventHandler(TestEvent{}, [](const cw::Event& e) {
            const auto& event = dynamic_cast<const TestEvent&>(e);
            std::cout << event.data;
        });
    }

    void fromJSON(const cw::json& j) final {
        _x = j["x"];
        _message = j["message"];
    }

    void toJSON(cw::json& j) const final {
        j["x"] = _x;
        j["message"] = _message;
    }

    DECLARE_COMMAND(SetX) {
        RECEIVE_COMMAND(TestModel2SetX);
        _x = command.x;
    }

    DECLARE_COMMAND(SetMessage) {
        RECEIVE_COMMAND(TestModel2SetMessage);
        _message = command.message;
    }

    DECLARE_QUERY(GetX) {
        return _x;
    }

    DECLARE_QUERY(GetMessage) {
        return _message;
    }

private:
    int _x = 0;
    std::string _message;
};

struct TestModel3SetX : public cw::Command {
    TestModel3SetX(const int s = 0) : x(s) {}

    int x;
};

struct TestModel3SetMessage : public cw::Command {
    TestModel3SetMessage(const std::string& s = "") : message(s) {}

    std::string message;
};

struct TestModel3QueryX : public cw::Query {
    using ReturnType = int;

    TestModel3QueryX() = default;
};

struct TestModel3QueryMessage : public cw::Query {
    using ReturnType = std::string;

    TestModel3QueryMessage() = default;
};

class TestModel3 : public cw::Model {
public:
    void registerModel(const std::shared_ptr<cw::ReadWriteController>& controller) final {
        REGISTER(controller, Command, TestModel3SetX, TestModel3::SetX, this);
        REGISTER(controller, Command, TestModel3SetMessage, TestModel3::SetMessage, this);
        REGISTER(controller, Query, TestModel3QueryX, TestModel3::GetX, this);
        REGISTER(controller, Query, TestModel3QueryMessage, TestModel3::GetMessage, this);
        controller->registerEventHandler(TestEvent{}, [](const cw::Event& e) {
            const auto& event = dynamic_cast<const TestEvent&>(e);
            std::cout << event.data;
        });
    }

    void fromJSON(const cw::json& j) final {
        _x = j["x"];
        _message = j["message"];
    }

    void toJSON(cw::json& j) const final {
        j["x"] = _x;
        j["message"] = _message;
    }

    DECLARE_COMMAND(SetX) {
        RECEIVE_COMMAND(TestModel3SetX);
        _x = command.x;
    }

    DECLARE_COMMAND(SetMessage) {
        RECEIVE_COMMAND(TestModel3SetMessage);
        _message = command.message;
    }

    DECLARE_QUERY(GetX) {
        return _x;
    }

    DECLARE_QUERY(GetMessage) {
        return _message;
    }

private:
    int _x = 0;
    std::string _message;
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
    EXPECT_THROW(root->registerCommand(TestCommand{}, {}), cw::AssertionError);
    REGISTER(root, Command, TestCommand, MVCTests::testCommand, this);
    EXPECT_THROW(REGISTER(root, Command, TestCommand, MVCTests::testCommand, this), cw::AssertionError);
    REGISTER(root, Command, TestCommand2, MVCTests::testCommand2, this);
}

TEST_F(MVCTests, RegisterQueryDeathTest) {
    EXPECT_THROW(root->registerQuery(TestQuery{}, {}), cw::AssertionError);
    root->registerQuery(TestQuery{}, testQuery);
    EXPECT_THROW(root->registerQuery(TestQuery{}, testQuery), cw::AssertionError);
    root->registerQuery(TestQuery2{}, testQuery2);
}

TEST_F(MVCTests, RegisterRequestDeathTest) {
    EXPECT_THROW(root->registerRequest(TestRequest{}, {}), cw::AssertionError);
    root->registerRequest(TestRequest{}, testRequest);
    EXPECT_THROW(root->registerRequest(TestRequest{}, testRequest), cw::AssertionError);
    root->registerRequest(TestRequest2{}, testRequest2);
}

TEST_F(MVCTests, RegisterEventHandlerDeathTest) {
    EXPECT_THROW(root->registerEventHandler(TestEvent{}, {}), cw::AssertionError);
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
    EXPECT_THROW(COMMAND(root, TestCommand, ("Message")), cw::AssertionError);
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
    EXPECT_THROW(root->query(TestQuery{ 4, 5 }), cw::AssertionError);
}

TEST_F(MVCTests, MultipleSuccessfulQueries) {
    root->registerQuery(TestQuery{}, testQuery);
    root->registerQuery(TestQuery2{}, testQuery2);
    const auto res = QUERY(root, TestQuery, (4, 5));
    EXPECT_EQ(res, 9);
    const auto res2 = QUERY(root, TestQuery2, (4, 5));
    EXPECT_EQ(res2, -1);
}

TEST_F(MVCTests, SuccessfulRequest) {
    root->registerRequest(TestRequest{}, testRequest);
    const auto res = REQUEST(root, TestRequest, (4.0, 5.0));
    EXPECT_DOUBLE_EQ(res, 20.0);
}

TEST_F(MVCTests, RequestDeathTest) {
    EXPECT_THROW(root->request(TestRequest{ 4.0, 5.0 }), cw::AssertionError);
}

TEST_F(MVCTests, MultipleSuccessfulRequests) {
    root->registerRequest(TestRequest{}, testRequest);
    root->registerRequest(TestRequest2{}, testRequest2);
    const auto res = REQUEST(root, TestRequest, (4.0, 5.0));
    EXPECT_DOUBLE_EQ(res, 20.0);
    const auto res2 = REQUEST(root, TestRequest2, (4.0, 5.0));
    EXPECT_DOUBLE_EQ(res2, 0.8);
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
    EXPECT_THROW(root->attachChildController("child", nullptr), cw::AssertionError);
    const std::shared_ptr<cw::ControllerNode> child = std::make_shared<cw::Controller>();
    root->attachChildController("child", child);
    EXPECT_THROW(root->attachChildController("child", child), cw::AssertionError);
}

TEST_F(MVCTests, AttachRootAsChildDeathTest) {
    const std::shared_ptr<cw::ControllerNode> child = std::make_shared<cw::Controller>();
    const std::shared_ptr<cw::Model> model = std::make_shared<TestModel>();
    child->attachModel("name", model);
    EXPECT_THROW(root->attachChildController("child", child), cw::AssertionError);
}

TEST_F(MVCTests, AttachModelDeathTest) {
    EXPECT_THROW(root->attachModel("model", nullptr), cw::AssertionError);
    const std::shared_ptr<cw::Model> model = std::make_shared<TestModel>();
    root->attachModel("model", model);
    EXPECT_THROW(root->attachModel("model", model), cw::AssertionError);
}

TEST_F(MVCTests, AttachChildControllerThenModelDeathTest) {
    const std::shared_ptr<cw::ControllerNode> child = std::make_shared<cw::Controller>();
    const std::shared_ptr<cw::Model> model = std::make_shared<TestModel>();
    root->attachChildController("name", child);
    EXPECT_THROW(root->attachModel("name", model), cw::AssertionError);
}

TEST_F(MVCTests, AttachModelThenChildControllerDeathTest) {
    const std::shared_ptr<cw::ControllerNode> child = std::make_shared<cw::Controller>();
    const std::shared_ptr<cw::Model> model = std::make_shared<TestModel>();
    root->attachModel("name", model);
    EXPECT_THROW(root->attachChildController("name", child), cw::AssertionError);
}

TEST_F(MVCTests, AttachViewDeathTest) {
    EXPECT_THROW(root->attachView(nullptr), cw::AssertionError);
    const std::shared_ptr<cw::View> view = std::make_shared<TestView>();
    root->attachView(view);
    EXPECT_THROW(root->attachView(view), cw::AssertionError);
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

TEST_F(MVCTests, DeserialiseDirectTest) {
    const std::shared_ptr<cw::Model> model = std::make_shared<TestModel2>();
    root->attachModel("model", model);
    root->fromJSON(R"({ "model": { "x": 5, "message": "Hello, World!" }})"_json);
    const auto x = QUERY(root, TestModel2QueryX, ());
    EXPECT_EQ(x, 5);
    const auto message = QUERY(root, TestModel2QueryMessage, ());
    EXPECT_EQ(message, "Hello, World!");
}

TEST_F(MVCTests, DeserialiseIndirectTest) {
    const std::shared_ptr<cw::Model> model = std::make_shared<TestModel2>();
    root->attachModel("model", model);
    root->fromJSON(R"({ "model": "MVCTests_DeserialiseIndirectTest.json"})"_json);
    const auto x = QUERY(root, TestModel2QueryX, ());
    EXPECT_EQ(x, -10);
    const auto message = QUERY(root, TestModel2QueryMessage, ());
    EXPECT_EQ(message, "Goodbye");
}

TEST_F(MVCTests, DeserialiseHierarchyTest) {
    const std::shared_ptr<cw::ControllerNode> child = std::make_shared<cw::Controller>();
    const std::shared_ptr<cw::Model> model = std::make_shared<TestModel2>();
    root->attachChildController("child", child);
    child->attachModel("model", model);
    root->fromJSON(R"({ "child": { "model": { "x": 5, "message": "Hello, World!" }} })"_json);
    const auto x = QUERY(root, TestModel2QueryX, ());
    EXPECT_EQ(x, 5);
    const auto message = QUERY(root, TestModel2QueryMessage, ());
    EXPECT_EQ(message, "Hello, World!");
}

TEST_F(MVCTests, DeserialiseIndirectHierarchyTest) {
    const std::shared_ptr<cw::ControllerNode> child = std::make_shared<cw::Controller>();
    const std::shared_ptr<cw::Model> model = std::make_shared<TestModel2>();
    root->attachChildController("child", child);
    child->attachModel("model", model);
    root->fromJSON(R"({ "child": "MVCTests_DeserialiseIndirectHierarchyTest.json" })"_json);
    const auto x = QUERY(root, TestModel2QueryX, ());
    EXPECT_EQ(x, -10);
    const auto message = QUERY(root, TestModel2QueryMessage, ());
    EXPECT_EQ(message, "Goodbye");
}

TEST_F(MVCTests, SerialiseDirectTest) {
    const std::shared_ptr<cw::Model> model = std::make_shared<TestModel2>();
    root->attachModel("model", model);
    COMMAND(root, TestModel2SetX, (110));
    COMMAND(root, TestModel2SetMessage, ("Hello"));
    cw::json result;
    root->toJSON(result);
    EXPECT_EQ(result["model"]["x"], 110);
    EXPECT_EQ(result["model"]["message"], "Hello");
}

TEST_F(MVCTests, SerialiseIndirectTest) {
    {
        std::ofstream json("tmp/MVCTests_SerialiseIndirectTest.json");
        json << R"({ "x": 50, "message": "msg" })";
    }
    const std::shared_ptr<cw::Model> model = std::make_shared<TestModel2>();
    root->attachModel("model", model);
    const auto originalJSON = R"({ "model": "tmp/MVCTests_SerialiseIndirectTest.json" })"_json;
    root->fromJSON(originalJSON);
    COMMAND(root, TestModel2SetX, (90));
    COMMAND(root, TestModel2SetMessage, ("Red"));
    cw::json newJSON;
    root->toJSON(newJSON);
    EXPECT_EQ(originalJSON, newJSON);
    cw::json storedJSON;
    {
        std::ifstream json("tmp/MVCTests_SerialiseIndirectTest.json");
        storedJSON = cw::json::parse(json);
    }
    EXPECT_EQ(storedJSON["x"], 90);
    EXPECT_EQ(storedJSON["message"], "Red");
}

TEST_F(MVCTests, SerialiseIndirectTwoModelTest) {
    {
        std::ofstream json("tmp/MVCTests_SerialiseIndirectTwoModelTest.json");
        json << R"({ "x": 500, "message": "msg2" })";
    }
    const std::shared_ptr<cw::Model> model1 = std::make_shared<TestModel2>(), model2 = std::make_shared<TestModel3>();
    root->attachModel("model1", model1);
    root->attachModel("model2", model2);
    const auto originalJSON =
        R"({ "model1": "tmp/MVCTests_SerialiseIndirectTwoModelTest.json", "model2": { "x": 300, "message": "Second" } })"_json;
    root->fromJSON(originalJSON);
    COMMAND(root, TestModel2SetX, (900));
    COMMAND(root, TestModel2SetMessage, ("Red0"));
    COMMAND(root, TestModel3SetX, (9000));
    COMMAND(root, TestModel3SetMessage, ("Red00"));
    cw::json newJSON;
    root->toJSON(newJSON);
    EXPECT_EQ(originalJSON["model1"], newJSON["model1"]);
    EXPECT_EQ(newJSON["model2"]["x"], 9000);
    EXPECT_EQ(newJSON["model2"]["message"], "Red00");
    cw::json storedJSON;
    {
        std::ifstream json("tmp/MVCTests_SerialiseIndirectTwoModelTest.json");
        storedJSON = cw::json::parse(json);
    }
    EXPECT_EQ(storedJSON["x"], 900);
    EXPECT_EQ(storedJSON["message"], "Red0");
}

TEST_F(MVCTests, SerialiseIndirectHierarchyTest) {
    {
        std::ofstream json("tmp/MVCTests_SerialiseIndirectHierarchyTest_Values.json");
        json << R"({ "x": 1234, "message": "5678" })";
    }
    {
        std::ofstream json("tmp/MVCTests_SerialiseIndirectHierarchyTest.json");
        json << R"({ "model1": "tmp/MVCTests_SerialiseIndirectHierarchyTest_Values.json" })";
    }
    const std::shared_ptr<cw::ControllerNode> child = std::make_shared<cw::Controller>();
    const std::shared_ptr<cw::Model> model1 = std::make_shared<TestModel2>();
    root->attachChildController("child", child);
    child->attachModel("model1", model1);
    const auto originalJSON = R"({ "child": "tmp/MVCTests_SerialiseIndirectHierarchyTest.json" })"_json;
    root->fromJSON(originalJSON);
    COMMAND(root, TestModel2SetX, (5678));
    COMMAND(root, TestModel2SetMessage, ("1234"));
    cw::json newJSON;
    root->toJSON(newJSON);
    EXPECT_EQ(originalJSON, newJSON);
    {
        cw::json storedJSON;
        {
            std::ifstream json("tmp/MVCTests_SerialiseIndirectHierarchyTest.json");
            storedJSON = cw::json::parse(json);
        }
        EXPECT_EQ(storedJSON["model1"], "tmp/MVCTests_SerialiseIndirectHierarchyTest_Values.json");
    }
    {
        cw::json storedJSON;
        {
            std::ifstream json("tmp/MVCTests_SerialiseIndirectHierarchyTest_Values.json");
            storedJSON = cw::json::parse(json);
        }
        EXPECT_EQ(storedJSON["x"], 5678);
        EXPECT_EQ(storedJSON["message"], "1234");
    }
}
