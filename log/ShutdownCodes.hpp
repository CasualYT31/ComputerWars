/**
 * \file ShutdownCodes.hpp
 * \brief Defines unique codes that this game can shutdown with.
 */

#pragma once

namespace cw {
/**
 * \brief The collection of return codes that this game can exit with.
 * \details Success will always be 0, and any other code will always be greater than 0.
 */
namespace ShutdownCode {
/**
 * \brief The user shut the game down successfully.
 */
static constexpr int Success = 0;
/**
 * \brief The user requested help on the game's command-line options.
 */
static constexpr int DisplayedHelp = 1;
/**
 * \brief The user provided invalid command-line options.
 */
static constexpr int InvalidOptions = 2;
/**
 * \brief The user provided an invalid core configuration file, or it couldn't be loaded for some other reason.
 */
static constexpr int InvalidCoreConfigurationFile = 3;
/**
 * \brief The logging component could not be setup due to a critical error.
 */
static constexpr int CriticalLoggingError = 4;
/**
 * \brief An assertion made within the game failed.
 */
static constexpr int AssertionFailed = 5;
/**
 * \brief The user generated the script interface documentation.
 */
static constexpr int GeneratedScriptInterfaceDocumentation = 6;
/**
 * \brief The user tried to generate the script interface documentation, but was unsuccessful in doing so.
 */
static constexpr int FailedToGenerateScriptInterfaceDocumentation = 7;
}; // namespace ShutdownCode
} // namespace cw
