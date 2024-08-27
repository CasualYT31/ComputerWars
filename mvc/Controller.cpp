#include "Controller.hpp"

#include "file/File.hpp"
#include "log/Log.hpp"

#include <set>

namespace cw {
void Controller::registerCommand(const Command& c, const CommandCallback& cb) {
    ASSERT(cb, "An empty command callback was given!");
    // If this is a child controller, redirect request up the hierarchy.
    const auto parent = _parentController.lock();
    if (parent) {
        LOG(debug, "Registering command in parent controller");
        return parent->registerCommand(c, cb);
    }
    // If this is the root controller, respond to request.
    std::type_index cIndex(typeid(c));
    ASSERT(!_commands.contains(cIndex), "This command was already registered!");
    _commands.emplace(cIndex, cb);
}

void Controller::registerQuery(const Query& q, const QueryCallback& cb) {
    ASSERT(cb, "An empty query callback was given!");
    // If this is a child controller, redirect request up the hierarchy.
    const auto parent = _parentController.lock();
    if (parent) {
        LOG(debug, "Registering query in parent controller");
        return parent->registerQuery(q, cb);
    }
    // If this is the root controller, respond to request.
    std::type_index qIndex(typeid(q));
    ASSERT(!_queries.contains(qIndex), "This query was already registered!");
    _queries.emplace(qIndex, cb);
}

void Controller::registerEventHandler(const Event& e, const EventCallback& cb) {
    ASSERT(cb, "An empty event callback was given!");
    // If this is a child controller, redirect request up the hierarchy.
    const auto parent = _parentController.lock();
    if (parent) {
        LOG(debug, "Registering event handler in parent controller");
        return parent->registerEventHandler(e, cb);
    }
    // If this is the root controller, respond to request.
    _events[typeid(e)].push_back(cb);
}

CommandResponse Controller::command(const Command& c) {
    // If this is a child controller, redirect request up the hierarchy.
    const auto parent = _parentController.lock();
    if (parent) { return parent->command(c); }
    // If this is the root controller, respond to request.
    std::type_index cIndex(typeid(c));
    ASSERT(_commands.contains(cIndex), "This command was not registered!");
    return _commands.at(cIndex)(c);
}

QueryResponse Controller::query(const Query& q) const {
    // If this is a child controller, redirect request up the hierarchy.
    const auto parent = _parentController.lock();
    if (parent) { return parent->query(q); }
    // If this is the root controller, respond to request.
    std::type_index qIndex(typeid(q));
    ASSERT(_queries.contains(qIndex), "This query was not registered!");
    return _queries.at(qIndex)(q);
}

EventResponse Controller::event(const std::shared_ptr<Event>& e) {
    ASSERT(e, "You cannot emit nullptr!");
    // If this is a child controller, redirect request up the hierarchy.
    const auto parent = _parentController.lock();
    if (parent) { return parent->event(e); }
    // If this is the root controller, respond to request.
    _incomingEvents.push(e);
    return EventResponse();
}

void Controller::attachChildController(const std::string& name, const std::shared_ptr<ControllerNode>& cc) {
    LOG(debug, "Attaching child controller \"{}\"", name);
    ASSERT(cc, "Tried to attach a nullptr controller!");
    ASSERT(
        !_childControllers.contains(name) && !_models.contains(name), "A child controller or model already has that name!"
    );
    ControllerNode::setParentController(cc, shared_from_this());
    _childControllers[name] = cc;
}

void Controller::attachModel(const std::string& name, const std::shared_ptr<Model>& m) {
    LOG(debug, "Attaching model \"{}\"", name);
    ASSERT(m, "Tried to attach a nullptr model!");
    ASSERT(
        !_childControllers.contains(name) && !_models.contains(name), "A child controller or model already has that name!"
    );
    m->registerModel(shared_from_this());
    _models[name] = m;
}

void Controller::attachView(const std::shared_ptr<View>& v) {
    ASSERT(v, "Tried to attach a nullptr view!");
    ASSERT(std::find(_views.begin(), _views.end(), v) == _views.end(), "This view's already been attached!");
    v->registerView(shared_from_this());
    LOG(debug, "Attaching view with ID {}", _views.size());
    _views.push_back(v);
}

TickResponse Controller::tick() {
    if (_parentController.expired()) {
        // The root controller.
        // 1. Dispatch incoming events.
        while (!_incomingEvents.empty()) {
            auto& e = _incomingEvents.front();
            const auto eIndex = std::type_index(typeid(*e));
            if (_events.contains(eIndex)) {
                for (const auto& cb : _events.at(eIndex)) { cb(*e); }
            }
            _incomingEvents.pop();
        }
        // 2. Render each view in order.
        for (const auto& view : _views) { view->render(); }
        // 3. Render every child view.
        for (const auto& child : _childControllers) { child.second->tick(); }
        // 4. Reset the tick code before returning it.
        const auto tickCode(_tickCode);
        _tickCode = Continue;
        return tickCode;
    } else {
        // The child controller.
        for (const auto& view : _views) { view->render(); }
        for (const auto& child : _childControllers) { child.second->tick(); }
        return Continue;
    }
}

void Controller::shutdown(const TickResponse exitCode) {
    // If this is a child controller, redirect request up the hierarchy.
    const auto parent = _parentController.lock();
    if (parent) {
        LOG(debug, "Redirecting shutdown request with code {} to parent controller", exitCode);
        return parent->shutdown(exitCode);
    }
    // If this is the root controller, respond to request.
    LOG(info, "Requesting shutdown with code {}", exitCode);
    _tickCode = exitCode;
}

void Controller::fromJSON(const cw::json& j) {
    LOG(debug, "Deserialising JSON into {} controller", (_parentController.expired() ? "root" : "child"));
    _scriptFiles.clear();
    std::set<std::string> successfullyLoadedModels, successfullyLoadedControllers;
    for (const auto& keyValues : j.items()) {
        const std::string key = keyValues.key();
        const auto& value = keyValues.value();
        cw::json obj;
        if (value.is_object()) {
            LOG(debug, "Key \"{}\" contains an object", key);
            obj = value;
        } else if (value.is_string()) {
            LOG(debug, "Key \"{}\" contains a script filepath", key);
            // Attempt to load the script whose path is stored in the string.
            const auto path = value.get<std::string>();
            try {
                const auto script = makeExceptionFStream<std::ifstream>(path);
                obj = json::parse(*script);
            } catch (const std::exception& e) {
                LOG(err,
                    "Couldn't load the configuration script \"{}\", the path of which was stored in the \"{}\" key: {}",
                    path,
                    key,
                    e);
                continue;
            }
            LOG(info, "Loaded configuration script \"{}\" for key \"{}\" successfully", path, key);
            _scriptFiles[key] = path;
        } else {
            LOG(warn, "Key \"{}\" pointed to a value of invalid type, \"{}\"", key, value.type_name());
            continue;
        }
        const auto model = _models.find(key);
        if (model != _models.end()) {
            try {
                LOG(info, "Loading model \"{}\"", key);
                model->second->fromJSON(obj);
                LOG(info, "Loaded model \"{}\" successfully", key);
                successfullyLoadedModels.insert(key);
            } catch (const std::exception& e) { LOG(err, "Could not load model \"{}\": {}", key, e); }
            continue;
        }
        const auto controller = _childControllers.find(key);
        if (controller != _childControllers.end()) {
            try {
                LOG(info, "Loading controller \"{}\"", key);
                controller->second->fromJSON(obj);
                LOG(info, "Loaded controller \"{}\" successfully", key);
                successfullyLoadedControllers.insert(key);
            } catch (const std::exception& e) { LOG(err, "Could not load controller \"{}\": {}", key, e); }
            continue;
        }
        LOG(warn, "No controller or model has the name \"{}\"", key);
    }
    std::set<std::string> allModels, allControllers, unloadedModels, unloadedControllers;
    std::transform(_models.begin(), _models.end(), std::inserter(allModels, allModels.end()), [](const auto& pair) {
        return pair.first;
    });
    std::transform(
        _childControllers.begin(),
        _childControllers.end(),
        std::inserter(allControllers, allControllers.end()),
        [](const auto& pair) { return pair.first; }
    );
    std::set_difference(
        allModels.begin(),
        allModels.end(),
        successfullyLoadedModels.begin(),
        successfullyLoadedModels.end(),
        std::inserter(unloadedModels, unloadedModels.end())
    );
    std::set_difference(
        allControllers.begin(),
        allControllers.end(),
        successfullyLoadedControllers.begin(),
        successfullyLoadedControllers.end(),
        std::inserter(unloadedControllers, unloadedControllers.end())
    );
    if (!unloadedModels.empty()) { LOG(err, "Some models from this controller were not loaded: {}", unloadedModels); }
    if (!unloadedControllers.empty()) {
        LOG(err, "Some child controllers from this controller were not loaded: {}", unloadedControllers);
    }
}

void Controller::toJSON(json& j) const {
    LOG(debug, "Serialising {} controller into JSON", (_parentController.expired() ? "root" : "child"));
    for (const auto& child : _childControllers) { _toInlineOrScript(j, child.first, *child.second); }
    for (const auto& model : _models) { _toInlineOrScript(j, model.first, *model.second); }
}

void Controller::setParentController(const std::shared_ptr<ControllerNode>& pc) {
    // This assertion makes sure that a root controller isn't made a child.
    ASSERT(
        _commands.empty() && _queries.empty() && _events.empty() && _incomingEvents.empty() && _models.empty()
            && _views.empty(),
        "You must register the entire controller hierarchy before registering any models or views!"
    );
    _parentController = pc;
}

void Controller::_toInlineOrScript(json& j, const std::string& key, const JSONSerialised& f) const {
    cw::json object;
    f.toJSON(object);
    if (_scriptFiles.contains(key)) {
        // JSON is saved to a separate script file. If it can't be written there, save it inline as a fail-safe.
        // It will try to write to the stored file path again next time, unless the state is updated via fromJSON()
        // and it's stored inline in the given JSON.
        const auto& filepath = _scriptFiles.at(key);
        LOG(info, "Attempting to write component \"{}\"'s state to \"{}\"", key, filepath);
        try {
            {
                const auto script = makeExceptionFStream<std::ofstream>(filepath);
                *script << object;
            }
            j[key] = filepath;
            LOG(info, "Successfully wrote component \"{}\"'s state to \"{}\"", key, filepath);
            return;
        } catch (const std::exception& e) {
            LOG(warn,
                "Could not write component \"{}\" to script file \"{}\": {}. Writing inline instead. Will attempt to write "
                "to the script file next time",
                key,
                filepath,
                e);
        }
    } else {
        LOG(debug, "Writing component \"{}\"'s state inline", key);
    }
    // JSON is saved inline.
    j[key] = object;
}
} // namespace cw
