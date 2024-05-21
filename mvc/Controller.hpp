/**
 * \file Controller.hpp
 * \brief Declares the basic implementation of the ControllerNode interface.
 */

#pragma once

#include "ControllerNode.hpp"
#include "Model.hpp"
#include "View.hpp"

#include <algorithm>
#include <nlohmann/json.hpp>
#include <queue>
#include <typeindex>
#include <unordered_map>

namespace cw {
/**
 * \brief A controller that owns a set of models and views, and redirects commands, queries, and events as needed.
 * \details A controller can either be the root or a child.
 *
 * The root controller is in charge of storing and performing commands and queries, and is also responsible for
 * receiving and dispatching events. It can receive commands, queries, and events from within itself, as well as all of its
 * children.
 *
 * Child controllers redirect all of their commands, queries, and events up to the root controller.
 */
class Controller : public ControllerNode {
public:
    void registerCommand(const Command&, const CommandCallback&) final;
    void registerQuery(const Query&, const QueryCallback&) final;
    void registerEventHandler(const Event&, const EventCallback&) final;
    CommandResponse command(const Command&) final;
    QueryResponse query(const Query&) const final;
    EventResponse event(const std::shared_ptr<Event>&) final;
    void attachChildController(const std::string&, const std::shared_ptr<ControllerNode>&) final;

    inline void setParentController(const std::shared_ptr<ControllerNode>& pc) final {
        _parentController = pc;
    }

    void attachModel(const std::string&, const std::shared_ptr<Model>&) final;
    void attachView(const std::shared_ptr<View>& v) final;
    TickResponse tick() final;
    void shutdown(const TickResponse exitCode) final;
    /**
     * \brief Loads every attached model (including child models) by deserialising them all from JSON scripts.
     * \details Models are loaded in the order they were attached in.
     */
    void fromJSON(const cw::json& j) final;
    /**
     * \brief Serialises every attached model (including child models) into JSON scripts.
     */
    void toJSON(json& j) const final;

private:
    /**
     * \brief Serialises a child controller or model into the given JSON object, either inline, or as a separate JSON script.
     * \param j The JSON object to serialise to.
     * \param key The name of the child controller or model.
     * \param f Child controller or model.
     */
    void _toInlineOrScript(json& j, const std::string& key, const JSONSerialised& f) const;

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
     * \brief The immediate parent of this child controller.
     */
    std::weak_ptr<ControllerNode> _parentController;
    /**
     * \brief The immediate children of the root controller.
     */
    std::unordered_map<std::string, std::shared_ptr<ControllerNode>> _childControllers;
    /**
     * \brief The list of models stored within this node of the controller hierarchy.
     */
    std::unordered_map<std::string, std::shared_ptr<Model>> _models;
    /**
     * \brief The list of views stored within this node of the controller hierarchy.
     */
    std::vector<std::shared_ptr<View>> _views;
    /**
     * \brief The next response for tick().
     */
    TickResponse _tickCode = cw::Continue;
    /**
     * \brief A map of script file paths, keyed on the names of the child controllers and models that last had their state
     * loaded from their respective script file.
     */
    std::unordered_map<std::string, std::string> _scriptFiles;
};
} // namespace cw
