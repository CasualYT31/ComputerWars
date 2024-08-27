#include "ScriptEngine.hpp"

namespace cw {
void ScriptEngine::registerModel(const std::shared_ptr<ReadWriteController>& controller) {}

void ScriptEngine::fromJSON(const json& j) {
    _lastKnownJSON = j;
}

void ScriptEngine::toJSON(json& j) const {
    j = _lastKnownJSON;
}
} // namespace cw
