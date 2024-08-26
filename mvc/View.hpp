/**
 * \file View.hpp
 * \brief The interface all views implement.
 */

#pragma once

#include "ControllerNode.hpp"

namespace cw {
/**
 * \brief A view renders information that's indirectly based on the state of at least one model.
 */
class View {
public:
    virtual ~View() = default;
    /**
     * \brief Allows a controller to setup the link between it and this view.
     * \details When a view is added to a controller, the controller will pass itself to the view so that the view can
     * register the event handlers the view requires. The view should also store the controller pointer if it wishes to
     * perform any queries.
     * \param controller The controller that this view is being added to.
     */
    virtual void registerView(const std::shared_ptr<ReadOnlyController>& controller) = 0;
    /**
     * \brief The controller invokes this method when the view needs to be rendered.
     * \details This method is invoked every tick.
     */
    virtual void render() = 0;
};
} // namespace cw
