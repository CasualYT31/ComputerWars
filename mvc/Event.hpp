/**
 * \file Event.hpp
 * \brief Defines the base type of all events.
 */

#pragma once

namespace cw {
/**
 * \brief Represents a single event in the game engine.
 */
struct Event {
    virtual ~Event() noexcept = default;

protected:
    Event() = default;
};
} // namespace cw
