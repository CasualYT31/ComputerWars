/**
 * \file Request.hpp
 * \brief Defines the base type of all requests.
 */

#pragma once

namespace cw {
/**
 * \brief Represents a single request in the game engine.
 * \warning Concrete types of Request are \b required to have a public using or typedef statement with the identifier
 * ReturnType. This informs the REQUEST() macro how to cast the std::any result into the type desired.
 */
struct Request {
    virtual ~Request() noexcept = default;

protected:
    Request() = default;
};
} // namespace cw
