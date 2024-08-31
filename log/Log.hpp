/**
 * \file Log.hpp
 * \brief Contains logging code.
 */

#pragma once

#include "Formatters.hpp"
#include "helper/Types.hpp"
#include "ShutdownCodes.hpp"

#include <boxer/boxer.h>
#include <cassert>
#include <cpptrace/cpptrace.hpp>
#include <deque>
#include <memory>
#include <optional>
#include <spdlog/sinks/dup_filter_sink.h>
#include <spdlog/spdlog.h>
#include <sstream>
#include <string.h>

namespace cw {
/**
 * \brief Thrown when an assertion made via cw::Log fails.
 */
class AssertionError : public std::logic_error {
public:
    /**
     * \brief Construct a new AssertionError.
     * \param msg Should contain the expression that failed in string form.
     */
    AssertionError(const std::string& msg) : std::logic_error(msg) {}
};

/**
 * \brief Used to insert "artificial" frames into a stacktrace.
 * \details Particularly useful if you want to record external stacktraces within the root C++ stacktrace.
 */
struct ArtificialStackFrame {
    /**
     * \brief The stack frame pointer.
     */
    void* pointer = nullptr;

    /**
     * \brief The full declaration of the current function.
     */
    std::string symbol;

    /**
     * \brief The path to the source file containing the function's source code, if known.
     */
    std::optional<std::string> source;

    /**
     * \brief The line number of the current statement, if known.
     */
    std::optional<std::size_t> lineNumber;

    /**
     * \brief A code snippet including the current statement, if known.
     */
    std::optional<std::string> snippet;
};

/**
 * \brief Stores a list of artificial stack frames, ordered from higher up the stack to lower.
 */
using ArtificialStackFrames = std::vector<ArtificialStackFrame>;

/**
 * \brief An ordered list of artificial stack frames, coupled with a set of conditions in which they will be inserted.
 * \details Once a list of artificial stack frames are inserted into a stacktrace, they will not be inserted again in that
 * stacktrace.
 */
struct ArtificialStackFramesWithConditions {
    /**
     * \brief The artificial stack frames to insert if the conditions pass.
     */
    ArtificialStackFrames frames;

    /**
     * \brief If this field stores a string, the frames will be inserted before the first frame whose symbol includes this
     * string.
     */
    std::optional<std::string> symbolIncludes;

    /**
     * \brief Determines if the given real frame matches with the conditions stored in this object.
     * \param frame The frame to test.
     * \returns True if so, false otherwise (or if no conditions are defined).
     */
    bool isMatchingFrame(const cpptrace::stacktrace_frame& frame) const {
        if (symbolIncludes) {
            if (frame.symbol.find(*symbolIncludes) != std::string::npos) { return true; }
        }
        return false;
    }

    /**
     * \brief If true, the artificial frames will be inserted after the matching frame instead of before (which is the
     * default).
     */
    bool insertAfterMatchingFrame = false;

    /**
     * \brief If true, the matching frame will be removed from the resulting stacktrace, meaning the artificial frames will
     * replace the matching real one.
     */
    bool deleteMatchingFrame = false;
};

/**
 * \brief The signature of the callback function that returns a collection of artificial stack frames with their conditions.
 */
using ArtificialStackFramesGenerator = std::function<ArtificialStackFramesWithConditions(void)>;

/**
 * \brief An ordered list of artificial stack frame groups, each with their own separate conditions.
 */
using ListOfArtificialStackFramesWithConditions = std::vector<ArtificialStackFramesWithConditions>;

/**
 * \brief Global point of access to the log.
 */
class Log {
public:
    /**
     * \brief The log levels available to the game.
     */
    using Level = spdlog::level::level_enum;

    /**
     * \brief This class can't be constructed.
     */
    Log() = delete;

    /**
     * \brief Converts a string into a log level value.
     * \param lvl The string to convert.
     * \returns The corresponding log level.
     * \throws std::invalid_argument if the level string could not be mapped to a log level value.
     * \sa anonymous_namespace{Log.cpp}::logLevelStringMap
     */
    static Log::Level LevelFromString(const std::string& lvl);

    /**
     * \brief Converts a log level value into a string.
     * \param lvl The value to convert.
     * \returns The corresponding log level string.
     * \sa anonymous_namespace{Log.cpp}::stringLogLevelMap
     */
    static std::string LevelToString(const Log::Level lvl) noexcept;

    /**
     * \brief Scans a file pattern for variables and replaces them.
     * \param[in,out] filePattern The file pattern to expand.
     * \sa anonymous_namespace{Log.cpp}::variableMap
     */
    static void ExpandFilePattern(std::string& filePattern);

    /**
     * \brief Sets up the logging sinks.
     * \details This includes the internal string stream which will contain a copy of the file, and the log file, whose
     * pattern is given. The log level will also default to info.
     * \warning You must call this method before attempting to read or write any log entries, or set the log level!
     * \param filePattern The path to write the log file to, which may contain variables that will be expanded out. If
     * nullopt is given, no file log will be generated.
     * \param logHardwareDetails True if hardware details should be written to the log as soon as the sinks are setup, false
     * if not.
     * \param neverProduceDialogBoxes True if no dialog boxes should ever be displayed via this Log object.
     * \param neverWriteTraces True if no stacktraces should ever be written to the logs.
     * \throws std::exception if there was a critical error. If the log file could not be opened, an error dialog box will be
     * shown, if possible, and no exception will be thrown. If the hardware details could not be written, the reason why will
     * be logged and no exception will be thrown.
     */
    static void Setup(
        const std::optional<std::string>& filePattern,
        const bool logHardwareDetails,
        const bool neverProduceDialogBoxes = false,
        const bool neverWriteTraces = false
    );

    /**
     * \brief Sets the global log level.
     * \param level The level to apply to every sink.
     */
    static void SetLevel(const Log::Level level);

    /**
     * \brief Write a line to the log.
     * \details If the critical level is provided, a dialog box will be displayed containing the logged line (without
     * additional information such as source file, time, etc.).
     * \tparam Ts The types of the objects that are to be inserted into the message.
     * \param file String literal describing the file that is writing the log line.
     * \param line The line of the file the log line is being written from.
     * \param lvl The level to write the line at.
     * \param message The message to write. If the given message was an invalid format string, the log will still be written
     * at the desired level, but the line will not be formatted, and the reason why will also be logged.
     * \param produceCriticalDialogBox If true, and the given log level is critical, attempt to produce a dialog box after
     * writing the log. If one couldn't be produced, a critical log line will be written explaining why.
     * \param writeTraceIfErrorOrAbove If true, and the given log level is err or above, write the stacktrace to the logs at
     * the given level.
     * \param objects The objects to insert into the message using fmt.
     */
    template <typename... Ts>
    static void Write(
        const char* const file,
        const std::size_t line,
        const Log::Level lvl,
        const std::string& message,
        const bool produceCriticalDialogBox,
        const bool writeTraceIfErrorOrAbove,
        Ts... objects
    ) noexcept {
        assert(_logger);
        try {
            const auto finalMessage = fmt::format(fmt::runtime(message), objects...);
            _logger->log(lvl, fmt::format("{} [{}:{}]", finalMessage, file, line));
            if (writeTraceIfErrorOrAbove && lvl >= Log::Level::err) { WriteTrace(lvl); }
            if (!_neverProduceDialogBoxes && produceCriticalDialogBox && lvl == Log::Level::critical) {
                try {
                    boxer::show(finalMessage.c_str(), "Critical Error!", boxer::Style::Error);
                } catch (const std::exception& e) { _logger->log(lvl, "Could not produce dialog box for above log: {}", e); }
            }
        } catch (const std::exception& e) { _logger->log(lvl, "Could not format log \"{}\" due to {}", message, e); }
        // Logs don't seem to write in Ubuntu unless I flush every time...
        _logger->flush();
    }

    /**
     * \brief Writes the current stacktrace to the log.
     * \details Any stacktrace frames that pertain to cpptrace::stacktrace, the first Write and/or WriteTrace frames, and any
     * frames beyond main, will not be included.
     * \param lvl The level to write the stacktrace at.
     */
    static void WriteTrace(const Log::Level lvl = Log::Level::trace);

    /**
     * \brief Adds an artificial stack frame generator to the top of the generator stack.
     * \param generator The generator to add.
     */
    static void PushArtificialStackFrameGenerator(const ArtificialStackFramesGenerator& generator);

    /**
     * \brief Removes an artificial stack frame generator from the generator stack.
     */
    static void PopArtificialStackFrameGenerator();

    /**
     * \brief Retrieves a copy of the log written so far.
     * \returns The copy of the log.
     */
    static std::string Get();

private:
    /**
     * \brief Invokes the stored artificial stack frame generators.
     * \returns The result of each
     */
    static ListOfArtificialStackFramesWithConditions _GetArtificialStackFrames();

    /**
     * \brief Non-thread safe distribution sink that outputs to a file and to a string stream.
     */
    static std::shared_ptr<spdlog::sinks::dup_filter_sink_st> _sink;
    /**
     * \brief Contains a copy of the log that's been written so far.
     */
    static std::ostringstream _logCopy;
    /**
     * \brief The logger object.
     */
    static std::unique_ptr<spdlog::logger> _logger;
    /**
     * \brief If true, Log will never produce a dialog box.
     */
    static bool _neverProduceDialogBoxes;
    /**
     * \brief If true, Log will never write stacktraces.
     */
    static bool _neverWriteTraces;
    /**
     * \brief Stores the artificial stack frames to insert into stacktraces.
     */
    static std::deque<ArtificialStackFramesGenerator> _stackFrameGenerators;
};
} // namespace cw

/**
 * \brief Extracts the relative path from the FILE macro.
 * \details ROOT_SOURCE_PATH_SIZE is defined within the project's CMakeLists.txt.
 */
#define __RELATIVE_FILENAME__ (__FILE__ + ROOT_SOURCE_PATH_SIZE)

/**
 * \brief Will only add a preceding comma to __VA_ARGS__ if at least one argument was given.
 * \details Thanks go to: https://stackoverflow.com/a/5897216.
 * \param ... Parameters to expand out.
 */
#define VA_ARGS(...) , ##__VA_ARGS__

/**
 * \brief Write a line to the log.
 * \param lvl The level to write the log at. Should not be a fully qualified name (i.e. it should be e.g. info or error).
 * \param message The message to write.
 * \param ... The objects to insert into the message using fmt, if any.
 */
#define LOG(lvl, message, ...)                                                                                              \
    cw::Log::Write(__RELATIVE_FILENAME__, __LINE__, cw::Log::Level::lvl, message, true, true VA_ARGS(__VA_ARGS__))

/**
 * \brief Make an assertion.
 * \details If an assertion fails, it will be logged at critical level.
 * \warning Care should be taken not to perform any important logic within the expression! Doing this will make it a lot
 * easier to exclude assertions from the build if we want to.
 * \param expression Must be true, or else an exception will be raised.
 * \param message Additional message to log if expression is false.
 * \param ... The objects to insert into the message using fmt, if any.
 * \throws cw::AssertionError if the given expression evaluated to false.
 */
#define ASSERT(expression, message, ...)                                                                                    \
    if (!(expression)) {                                                                                                    \
        const auto additionalInfo = fmt::format(message VA_ARGS(__VA_ARGS__));                                              \
        LOG(critical, "Assertion failed: {}. {}", #expression, additionalInfo);                                             \
        throw cw::AssertionError(#expression);                                                                              \
    }

/**
 * \brief Writes an artificial stack frame to an output stream.
 * \details The line number will not be printed if the source field is not set.
 * \param os The output stream to write to.
 * \param frame The artificial frame to write.
 * \returns os.
 */
std::ostream& operator<<(std::ostream& os, const cw::ArtificialStackFrame& frame);
