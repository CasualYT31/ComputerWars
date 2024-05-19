#include "ChildController.hpp"

namespace cw {
TickResponse ChildController::tick() {
    for (const auto& view : _views) { view->render(); }
    for (const auto& child : _childControllers) { child->tick(); }
    return Continue;
}
} // namespace cw
