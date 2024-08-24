#include "Controller.hpp"

#include "file/file.hpp"

#include <cassert>

namespace cw {
void Controller::registerCommand(const Command& c, const CommandCallback& cb) {
    // If this assertion fails, an empty callback was given.
    assert(cb);
    // If this is a child controller, redirect request up the hierarchy.
    const auto parent = _parentController.lock();
    if (parent) { return parent->registerCommand(c, cb); }
    // If this is the root controller, respond to request.
    std::type_index cIndex(typeid(c));
    // If this assertion fails, a command was registered twice, which is not allowed.
    assert(!_commands.contains(cIndex));
    _commands.emplace(cIndex, cb);
}

void Controller::registerQuery(const Query& q, const QueryCallback& cb) {
    // If this assertion fails, an empty callback was given.
    assert(cb);
    // If this is a child controller, redirect request up the hierarchy.
    const auto parent = _parentController.lock();
    if (parent) { return parent->registerQuery(q, cb); }
    // If this is the root controller, respond to request.
    std::type_index qIndex(typeid(q));
    // If this assertion fails, a query was registered twice, which is not allowed.
    assert(!_queries.contains(qIndex));
    _queries.emplace(qIndex, cb);
}

void Controller::registerEventHandler(const Event& e, const EventCallback& cb) {
    // If this assertion fails, an empty callback was given.
    assert(cb);
    // If this is a child controller, redirect request up the hierarchy.
    const auto parent = _parentController.lock();
    if (parent) { return parent->registerEventHandler(e, cb); }
    // If this is the root controller, respond to request.
    _events[typeid(e)].push_back(cb);
}

CommandResponse Controller::command(const Command& c) {
    // If this is a child controller, redirect request up the hierarchy.
    const auto parent = _parentController.lock();
    if (parent) { return parent->command(c); }
    // If this is the root controller, respond to request.
    std::type_index cIndex(typeid(c));
    // If this assertion fails, this command wasn't registered.
    assert(_commands.contains(cIndex));
    return _commands.at(cIndex)(c);
}

QueryResponse Controller::query(const Query& q) const {
    // If this is a child controller, redirect request up the hierarchy.
    const auto parent = _parentController.lock();
    if (parent) { return parent->query(q); }
    // If this is the root controller, respond to request.
    std::type_index qIndex(typeid(q));
    // If this assertion fails, this query wasn't registered.
    assert(_queries.contains(qIndex));
    return _queries.at(qIndex)(q);
}

EventResponse Controller::event(const std::shared_ptr<Event>& e) {
    // If this assertion fails, a nullptr event was emitted.
    assert(e);
    // If this is a child controller, redirect request up the hierarchy.
    const auto parent = _parentController.lock();
    if (parent) { return parent->event(e); }
    // If this is the root controller, respond to request.
    _incomingEvents.push(e);
    return EventResponse();
}

void Controller::attachChildController(const std::string& name, const std::shared_ptr<ControllerNode>& cc) {
    // If this assertion fails, a nullptr was attached.
    assert(cc);
    // If this assertion fails, the given name was not unique.
    assert(!_childControllers.contains(name) && !_models.contains(name));
    cc->setParentController(shared_from_this());
    _childControllers[name] = cc;
}

void Controller::attachModel(const std::string& name, const std::shared_ptr<Model>& m) {
    // If this assertion fails, a nullptr was attached.
    assert(m);
    // If this assertion fails, the given name was not unique.
    assert(!_childControllers.contains(name) && !_models.contains(name));
    m->registerModel(shared_from_this());
    _models[name] = m;
}

void Controller::attachView(const std::shared_ptr<View>& v) {
    // If this assertion fails, a nullptr was attached.
    assert(v);
    // If this assertion fails, the view has already been added to the controller.
    assert(std::find(_views.begin(), _views.end(), v) == _views.end());
    v->registerView(shared_from_this());
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
    if (parent) { return parent->shutdown(exitCode); }
    // If this is the root controller, respond to request.
    _tickCode = exitCode;
}

void Controller::fromJSON(const cw::json& j) {
    _scriptFiles.clear();
    for (const auto& keyValues : j.items()) {
        cw::json obj;
        if (keyValues.value().is_object()) {
            obj = keyValues.value();
        } else if (keyValues.value().is_string()) {
            // Attempt to load the script whose path is stored in the string.
            try {
                const auto script = makeExceptionFStream<std::ifstream>(keyValues.value().get<std::string>());
                obj = json::parse(*script);
            } catch (const std::exception& e) {
                // LOG: Couldn't load script.
                continue;
            }
            // LOG: Loaded script successfully.
            _scriptFiles[keyValues.key()] = keyValues.value();
        } else {
            // LOG: Invalid value type.
            continue;
        }
        const auto model = _models.find(keyValues.key());
        if (model != _models.end()) {
            try {
                model->second->fromJSON(obj);
                // LOG: Loaded model.
            } catch (const std::exception& e) {
                // LOG: Couldn't load model.
            }
            continue;
        }
        const auto controller = _childControllers.find(keyValues.key());
        if (controller != _childControllers.end()) {
            try {
                controller->second->fromJSON(obj);
                // LOG: Loaded controller.
            } catch (const std::exception& e) {
                // LOG: Couldn't load controller.
            }
            continue;
        }
        // LOG: No controller or model with that name.
    }
}

void Controller::toJSON(json& j) const {
    for (const auto& child : _childControllers) { _toInlineOrScript(j, child.first, *child.second); }
    for (const auto& model : _models) { _toInlineOrScript(j, model.first, *model.second); }
}

void Controller::_toInlineOrScript(json& j, const std::string& key, const JSONSerialised& f) const {
    cw::json object;
    f.toJSON(object);
    if (_scriptFiles.contains(key)) {
        // JSON is saved to a separate script file. If it can't be written there, save it inline as a fail-safe.
        // It will try to write to the stored file path again next time, unless the state is updated via fromJSON()
        // and it's stored inline in the given JSON.
        const auto& filepath = _scriptFiles.at(key);
        try {
            {
                const auto script = makeExceptionFStream<std::ofstream>(filepath);
                *script << object;
            }
            j[key] = filepath;
            return;
        } catch (const std::exception& e) {
            // LOG: Can't write to script file, saving inline instead.
        }
    }
    // JSON is saved inline.
    j[key] = object;
}
} // namespace cw
