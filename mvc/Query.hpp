/**
 * \file Query.hpp
 * \brief Defines the base type of all queries.
 */

#pragma once

namespace cw {
/**
 * \brief Represents a single query in the game engine.
 * \warning Concrete types of Query are \b required to have a public using or typedef statement with the identifier
 * ReturnType. This informs the QUERY() macro how to cast the std::any result into the type desired.
 */
struct Query {
    virtual ~Query() noexcept = default;

protected:
    Query() = default;
};
} // namespace cw
