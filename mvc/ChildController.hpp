/**
 * \file ChildController.hpp
 * \brief Declares the basic implementation of the child controller interface.
 */

#pragma once

#include "Controller.hpp"
#include "Model.hpp"
#include "View.hpp"

#include <unordered_set>

namespace cw {
/**
 * \brief Represents a child controller that redirects all of its requests up the controller hierarchy to the root
 * controller.
 */
class ChildController : public ControllerNode {
public:
    inline void registerCommand(const Command& c, const CommandCallback& cb) final {
        const auto parent = _parentController.lock();
        // If this assertion fails, no parent was assigned to the child controller.
        assert(parent);
        return parent->registerCommand(c, cb);
    }

    inline void registerQuery(const Query& q, const QueryCallback& cb) final {
        const auto parent = _parentController.lock();
        // If this assertion fails, no parent was assigned to the child controller.
        assert(parent);
        return parent->registerQuery(q, cb);
    }

    inline void registerEventHandler(const Event& e, const EventCallback& cb) final {
        const auto parent = _parentController.lock();
        // If this assertion fails, no parent was assigned to the child controller.
        assert(parent);
        return parent->registerEventHandler(e, cb);
    }

    inline CommandResponse command(const Command& c) final {
        const auto parent = _parentController.lock();
        // If this assertion fails, no parent was assigned to the child controller.
        assert(parent);
        return parent->command(c);
    }

    inline QueryResponse query(const Query& q) const final {
        const auto parent = _parentController.lock();
        // If this assertion fails, no parent was assigned to the child controller.
        assert(parent);
        return parent->query(q);
    }

    inline EventResponse event(const std::shared_ptr<Event>& e) final {
        const auto parent = _parentController.lock();
        // If this assertion fails, no parent was assigned to the child controller.
        assert(parent);
        return parent->event(e);
    }

    inline void attachChildController(const std::shared_ptr<ControllerNode>& cc) final {
        // If this assertion fails, a nullptr was attached.
        assert(cc);
        // If this assertion fails, the child controller has already been added to this one.
        assert(!_childControllers.contains(cc));
        _childControllers.insert(cc);
        cc->setParentController(shared_from_this());
    }

    inline void setParentController(const std::shared_ptr<ControllerNode>& pc) final {
        _parentController = pc;
    }

    inline void attachModel(const std::shared_ptr<Model>& m) final {
        // If this assertion fails, a model was attached to an orphan child controller.
        // An indirectly orphaned child controller will fail assertions when attempting to register commands, etc.
        assert(!_parentController.expired());
        // If this assertion fails, a nullptr was attached.
        assert(m);
        // If this assertion fails, the model has already been added to the controller.
        assert(!_models.contains(m));
        m->registerModel(shared_from_this());
        _models.insert(m);
    }

    inline void attachView(const std::shared_ptr<View>& v) final {
        // If this assertion fails, a model was attached to an orphan child controller.
        // An indirectly orphaned child controller will fail assertions when attempting to register commands, etc.
        assert(!_parentController.expired());
        // If this assertion fails, a nullptr was attached.
        assert(v);
        // If this assertion fails, the view has already been added to the controller.
        assert(std::find(_views.begin(), _views.end(), v) == _views.end());
        v->registerView(shared_from_this());
        _views.push_back(v);
    }

    /**
     * \brief Renders all of the views attached to this child controller.
     * \returns cw::Continue.
     */
    TickResponse tick() final;

    inline void shutdown(const TickResponse exitCode) final {
        const auto parent = _parentController.lock();
        // If this assertion fails, no parent was assigned to the child controller.
        assert(parent);
        return parent->shutdown(exitCode);
    }

private:
    /**
     * \brief The immediate parent of this child controller.
     */
    std::weak_ptr<ControllerNode> _parentController;
    /**
     * \brief The immediate children of this child controller.
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
};
} // namespace cw
