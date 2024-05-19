#include "RootController.hpp"

namespace cw {
TickResponse RootController::tick() {
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
    for (const auto& child : _childControllers) { child->tick(); }
    // 4. Reset the tick code before returning it.
    const auto tickCode(_tickCode);
    _tickCode = Continue;
    return tickCode;
}
} // namespace cw
