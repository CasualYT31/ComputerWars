/**
 * \file ScriptEngine.hpp
 * \brief Declares the model that stores and manages external scripts.
 */

#pragma once

#include "mvc/Model.hpp"

namespace cw {
/**
 * \brief Manages external scripts.
 */
class ScriptEngine : public Model {
public:
    /**
     * \warning This model must be registered before any model that adds to the script interface.
     * \copydoc Model::registerModel
     */
    void registerModel(const std::shared_ptr<ReadWriteController>& controller) final;

    /**
     * \brief Deletes every module currently loaded and loads new ones based on the contents of the given JSON.
     * \copydetails JSONSerialised::fromJSON
     */
    void fromJSON(const json& j) final;

    /**
     * \brief Regurgitates the JSON that was last given to the model.
     * \copydetails JSONSerialised::toJSON
     */
    void toJSON(json& j) const final;

private:
    /**
     * \brief Caches the JSON that was last given to fromJSON().
     */
    cw::json _lastKnownJSON;
};
} // namespace cw
