/**
 * \file RootController.hpp
 * \brief Declares the basic implementation of the root controller interface.
 */

#pragma once

#include "Controller.hpp"
#include "Model.hpp"
#include "View.hpp"

#include <algorithm>
#include <queue>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>

namespace cw {
/**
 * \brief The root controller that owns every child controller in the engine.
 * \details The root controller is in charge of storing and performing commands and queries, and is also responsible for
 * receiving and dispatching events. It can receive commands, queries, and events from within itself, as well as all of its
 * children.
 */
class RootController : public RootControllerNode {
public:
    inline void registerCommand(const Command& c, const CommandCallback& cb) final {
        // If this assertion fails, an empty callback was given.
        assert(cb);
        std::type_index cIndex(typeid(c));
        // If this assertion fails, a command was registered twice, which is not allowed.
        assert(!_commands.contains(cIndex));
        _commands.emplace(cIndex, cb);
    }

    inline void registerQuery(const Query& q, const QueryCallback& cb) final {
        // If this assertion fails, an empty callback was given.
        assert(cb);
        std::type_index qIndex(typeid(q));
        // If this assertion fails, a query was registered twice, which is not allowed.
        assert(!_queries.contains(qIndex));
        _queries.emplace(qIndex, cb);
    }

    inline void registerEventHandler(const Event& e, const EventCallback& cb) final {
        // If this assertion fails, an empty callback was given.
        assert(cb);
        _events[typeid(e)].push_back(cb);
    }

    inline CommandResponse command(const Command& c) final {
        std::type_index cIndex(typeid(c));
        // If this assertion fails, this command wasn't registered.
        assert(_commands.contains(cIndex));
        return _commands.at(cIndex)(c);
    }

    inline QueryResponse query(const Query& q) const final {
        std::type_index qIndex(typeid(q));
        // If this assertion fails, this query wasn't registered.
        assert(_queries.contains(qIndex));
        return _queries.at(qIndex)(q);
    }

    inline EventResponse event(const std::shared_ptr<Event>& e) final {
        // If this assertion fails, a nullptr event was emitted.
        assert(e);
        _incomingEvents.push(e);
        return EventResponse();
    }

    inline void attachChildController(const std::shared_ptr<ControllerNode>& cc) final {
        // If this assertion fails, a nullptr was attached.
        assert(cc);
        // If this assertion fails, the child controller has already been added to this one.
        assert(!_childControllers.contains(cc));
        _childControllers.insert(cc);
        cc->setParentController(shared_from_this());
    }

    inline void attachModel(const std::shared_ptr<Model>& m) final {
        // If this assertion fails, a nullptr was attached.
        assert(m);
        // If this assertion fails, the model has already been added to the controller.
        assert(!_models.contains(m));
        m->registerModel(shared_from_this());
        _models.insert(m);
    }

    inline void attachView(const std::shared_ptr<View>& v) final {
        // If this assertion fails, a nullptr was attached.
        assert(v);
        // If this assertion fails, the view has already been added to the controller.
        assert(std::find(_views.begin(), _views.end(), v) == _views.end());
        v->registerView(shared_from_this());
        _views.push_back(v);
    }

    TickResponse tick() final;

    inline void shutdown(const TickResponse exitCode) final {
        _tickCode = exitCode;
    }

private:
    /**
     * \brief The commands supported by the controller.
     */
    std::unordered_map<std::type_index, CommandCallback> _commands;
    /**
     * \brief The queries supported by the controller.
     */
    std::unordered_map<std::type_index, QueryCallback> _queries;
    /**
     * \brief The events that have handlers in this controller.
     */
    std::unordered_map<std::type_index, std::vector<EventCallback>> _events;
    /**
     * \brief A queue of incoming events that have yet to be dispatched.
     */
    std::queue<std::shared_ptr<Event>> _incomingEvents;
    /**
     * \brief The immediate children of the root controller.
     */
    std::unordered_set<std::shared_ptr<ControllerNode>> _childControllers;
    /**
     * \brief The list of models stored within this node of the controller hierarchy.
     */
    std::unordered_set<std::shared_ptr<Model>> _models;
    /**
     * \brief The list of views stored within this node of the controller hierarchy.
     */
    std::vector<std::shared_ptr<View>> _views;
    /**
     * \brief The next response for tick().
     */
    TickResponse _tickCode = cw::Continue;
};
} // namespace cw
