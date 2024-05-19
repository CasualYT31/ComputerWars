/**
 * \file Command.hpp
 * \brief Defines the base type of all commands.
 */

#pragma once

namespace cw {
/**
 * \brief Represents a single command in the game engine.
 */
struct Command {
    virtual ~Command() noexcept = default;

protected:
    Command() = default;
};
} // namespace cw
