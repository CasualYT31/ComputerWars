/**
 * \file JSONSerialised.hpp
 * \brief Classes that can be serialised to and deserialised from JSON must implement this interface.
 */

#pragma once

#include <nlohmann/json.hpp>

namespace cw {
/**
 * \brief Allows you to customise the type of JSON object to use throughout the engine.
 */
using json = nlohmann::ordered_json;

/**
 * \brief Classes that can be serialised to and deserialised from JSON must implement this interface.
 */
class JSONSerialised {
public:
    /**
     * \brief Deserialises JSON into this object, changing this object's state as needed.
     * \param j The JSON object to deserialise from.
     */
    virtual void fromJSON(const json& j) = 0;
    /**
     * \brief Serialises this object's state into JSON.
     * \param j The JSON object to serialise to.
     */
    virtual void toJSON(json& j) const = 0;
};
} // namespace cw
