/**
 * \file ControllerNode.hpp
 * \brief Defines the interface all controllers implement.
 */

#pragma once

#include "Command.hpp"
#include "Event.hpp"
#include "JSONSerialised.hpp"
#include "Query.hpp"

#include <any>
#include <functional>
#include <memory>

namespace cw {
/**
 * \brief The type of response sent back after an event's callback has finished processing.
 */
using EventResponse = void;
/**
 * \brief The signature of an event callback.
 */
using EventCallback = std::function<EventResponse(const Event&)>;
/**
 * \brief The type of response sent back after a command's processing has finished.
 */
using CommandResponse = void;
/**
 * \brief The signature of code that's invoked when a command is performed.
 */
using CommandCallback = std::function<CommandResponse(const Command&)>;
/**
 * \brief The type of response that every query sends out.
 */
using QueryResponse = std::any;
/**
 * \brief The signature of code that's invoked when a query is made.
 */
using QueryCallback = std::function<QueryResponse(const Query&)>;
/**
 * \brief Once a tick has completed, this will be the response sent back to the caller.
 */
using TickResponse = int;

/**
 * \brief The tick() response that means "continue running the game loop."
 */
static constexpr TickResponse Continue = -1;

/**
 * \brief Interface that allows components to perform queries and respond to events.
 */
class ReadOnlyController {
public:
    virtual ~ReadOnlyController() noexcept = default;
    /**
     * \brief Tell the controller to invoke the given callback whenever it receives the given event.
     * \param e The type of event to respond to.
     * \param cb The callback to invoke.
     */
    virtual void registerEventHandler(const Event& e, const EventCallback& cb) = 0;
    /**
     * \brief Allows a component to perform a query into the controller to retrieve information.
     * \details Note that queries need to be performed immediately. They cannot be queued.
     * \param q The type of query to make.
     * \returns The response of the query.
     */
    virtual QueryResponse query(const Query& q) const = 0;
};

/**
 * \brief Interface that allows components to perform commands and queries, as well as dispatch and respond to events.
 */
class ReadWriteController : public ReadOnlyController {
public:
    virtual ~ReadWriteController() noexcept = default;
    /**
     * \brief Tells the controller what code to invoke when a given command is sent to the controller.
     * \param c The type of command to register code for.
     * \param cb The code to invoke when a command is received.
     */
    virtual void registerCommand(const Command& c, const CommandCallback& cb) = 0;
    /**
     * \brief Tells the controller what code to invoke when a given query is sent to the controller.
     * \param q The type of query to register code for.
     * \param cb The code to invoke when a query is received.
     */
    virtual void registerQuery(const Query& q, const QueryCallback& cb) = 0;
    /**
     * \brief Dispatches an event.
     * \details Note that the implementation need not immediately respond to the event; it may queue the event for
     * processing later.
     * \param e The event to dispatch.
     * \returns The response of dispatching the event.
     */
    virtual EventResponse event(const std::shared_ptr<Event>& e) = 0;
    /**
     * \brief Performs a command.
     * \details Note that commands need to be performed immediately. They cannot be queued.
     * \param c The command to send.
     * \returns The response of the command.
     */
    virtual CommandResponse command(const Command& c) = 0;
};

class Model;
class View;

/**
 * \brief Represents a single controller in a controller hierarchy.
 */
class ControllerNode : public ReadWriteController,
                       public JSONSerialised,
                       public std::enable_shared_from_this<ControllerNode> {
public:
    virtual ~ControllerNode() noexcept = default;
    /**
     * \brief Registers a child controller with this controller.
     * \param name The unique name of the child controller. The name must be unique across all other child controllers in
     * this controller, as well as models.
     * \param cc The child controller.
     */
    virtual void attachChildController(const std::string& name, const std::shared_ptr<ControllerNode>& cc) = 0;
    /**
     * \brief Adds a model to this controller node.
     * \param name The unique name of the model. The name must be unique across all other child controllers in this
     * controller, as well as models.
     * \param m The model to add.
     */
    virtual void attachModel(const std::string& name, const std::shared_ptr<Model>& m) = 0;
    /**
     * \brief Adds a view to this controller node.
     * \param v The view to add.
     */
    virtual void attachView(const std::shared_ptr<View>& v) = 0;
    /**
     * \brief Executes one iteration of the controller's game loop.
     * \details The implementation of this method shall receive and dispatch events, redirect commands and queries to their
     * intended destinations, and render views.
     * \returns A return code. >= 0 if the game should shutdown. 0 shall indicate success, any other value shall indicate an
     * error.
     */
    virtual TickResponse tick() = 0;
    /**
     * \brief Sets the response to return when tick() is next called.
     * \param exitCode Must be >= 0 to cause the game to shutdown.
     */
    virtual void shutdown(const TickResponse exitCode) = 0;

protected:
    /**
     * \brief Creates a link between a parent controller and this child controller.
     * \param pc The controller who's the parent of this controller.
     */
    virtual void setParentController(const std::shared_ptr<ControllerNode>& pc) = 0;

    /**
     * \brief Makes setParentController() invokable from derived classes.
     * \param cc Pointer to the child controller whose parent pointer is being updated.
     * \param pc Pointer to the controller who is the parent of self.
     */
    inline static void setParentController(
        const std::shared_ptr<ControllerNode>& cc, const std::shared_ptr<ControllerNode>& pc
    ) {
        cc->setParentController(pc);
    }
};
} // namespace cw

/**
 * \brief Used to register a class method as a command, query, or event handler callback.
 * \param controller The pointer to the controller to register with.
 * \param type Command, Query, or EventListener.
 * \param obj The concrete Command, Query, or Event typename.
 * \param method A method name in the format Class::method.
 * \param ptr A pointer to the instance of the method's class to invoke the method of. Usually \c this.
 */
#define REGISTER(controller, type, obj, method, ptr)                                                                        \
    LOG(debug, #type " " #obj " is being registered to invoke " #ptr "->" #method);                                         \
    controller->register##type(obj{}, std::bind(&method, ptr, std::placeholders::_1))

/**
 * \brief Used to perform commands.
 * \param controller The pointer to the controller that has access to the command.
 * \param obj The concrete Command typename.
 * \param params Parentheses-surrounded list of parameters to give to the command's constructor. Provide an empty pair of
 * parentheses if there are no parameters required.
 */
#define COMMAND(controller, obj, params)                                                                                    \
    LOG(trace, "Invoking command " #obj #params);                                                                           \
    controller->command(obj params);

/**
 * \brief Used to perform queries.
 * \param controller The pointer to the controller that has access to the query.
 * \param obj The concrete Query typename.
 * \param params Parentheses-surrounded list of parameters to give to the query's constructor. Provide an empty pair of
 * parentheses if there are no parameters required.
 */
#define QUERY(controller, obj, params)                                                                                      \
    [&controller = controller]() {                                                                                          \
        LOG(trace, "Invoking query " #obj #params);                                                                         \
        return std::any_cast<obj::ReturnType>(controller->query(obj params));                                               \
    }()

/**
 * \brief Used to emit events.
 * \param controller The pointer to the controller to emit events to.
 * \param obj The concrete Event typename.
 * \param params Parentheses-surrounded list of parameters to give to the event's constructor. Provide an empty pair of
 * parentheses if there are no parameters required.
 */
#define EVENT(controller, obj, params)                                                                                      \
    LOG(trace, "Emitting event " #obj #params);                                                                             \
    controller->event(std::make_shared<obj> params)

/**
 * \brief Declares a command handler.
 * \param name The name of the command handler.
 */
#define DECLARE_COMMAND(name) void name(const cw::Command& c)

/**
 * \brief Declares a query handler.
 * \param name The name of the query handler.
 */
#define DECLARE_QUERY(name) std::any name(const cw::Query& q)

/**
 * \brief Declares an event handler.
 * \param name The name of the event handler.
 */
#define DECLARE_EVENT(name) void name(const cw::Event& e)

/**
 * \brief Used to define a reference called command that points to the given concrete command object.
 * \param obj The concrete Command typename.
 */
#define RECEIVE_COMMAND(obj)                                                                                                \
    LOG(trace, "Received command " #obj);                                                                                   \
    const auto& command = dynamic_cast<const obj&>(c)

/**
 * \brief Used to define a reference called query that points to the given concrete query object.
 * \param obj The concrete Query typename.
 */
#define RECEIVE_QUERY(obj)                                                                                                  \
    LOG(trace, "Received query " #obj);                                                                                     \
    const auto& query = dynamic_cast<const obj&>(q)

/**
 * \brief Used to define a reference called event that points to the given concrete event object.
 * \param obj The concrete Event typename.
 */
#define RECEIVE_EVENT(obj)                                                                                                  \
    LOG(trace, "Received event " #obj);                                                                                     \
    const auto& event = dynamic_cast<const obj&>(e)
