/**
 * \file Model.hpp
 * \brief The interface that all concrete model types implement.
 */

#pragma once

#include "Controller.hpp"

namespace cw {
/**
 * \brief A model contains data and logic, and emits events, notifying interested parties of changes to its state.
 */
class Model {
public:
    virtual ~Model() = default;
    /**
     * \brief Allows a controller to setup the link between it and this model.
     * \details When a model is added to a controller, the controller will pass itself to the model so that the model can
     * register the commands and queries it supports. The model shall also register event handlers, in case it needs to
     * handle other model changes. Lastly, the model shall store the controller pointer, so that it can emit events and
     * perform commands and queries later.
     * \param controller The controller that this model is being added to.
     */
    virtual void registerModel(const std::shared_ptr<Controller>& controller) = 0;
};
} // namespace cw
