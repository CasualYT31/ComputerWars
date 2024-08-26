#include "Log.hpp"

#include "maths/Maths.hpp"

#include <cpptrace/cpptrace.hpp>
#include <ctime>
#include <fmt/format.h>
#include <functional>
#include <iostream>
#include <regex>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/ostream_sink.h>
#include <SystemProperties.hpp>
#include <unordered_map>
#include <unordered_set>

#ifndef GIT_BRANCH
    #define GIT_BRANCH "<unknown>"
#endif

#ifndef GIT_COMMIT
    #define GIT_COMMIT "<unknown>"
#endif

namespace {
/**
 * \brief Maps file pattern variables to their runtime computed values.
 */
const std::unordered_map<std::string, std::function<std::string()>> variableMap = { { "%DATE%", []() -> std::string {
                                                                                         time_t curtime = time(nullptr);
                                                                                         tm ltime;
#ifdef _WIN32
                                                                                         ::localtime_s(&ltime, &curtime);
#else
                                                                                         ::localtime_r(&curtime, &ltime);
#endif
                                                                                         return fmt::format(
                                                                                             "{}-{}-{} {}-{}-{}",
                                                                                             ltime.tm_mday,
                                                                                             ltime.tm_mon + 1,
                                                                                             ltime.tm_year + 1900,
                                                                                             ltime.tm_hour,
                                                                                             ltime.tm_min,
                                                                                             ltime.tm_sec
                                                                                         );
                                                                                     } } };

/**
 * \brief Maps strings to their corresponding log levels.
 */
const std::unordered_map<std::unordered_set<std::string>, cw::Log::Level> logLevelStringMap = {
    { { "trace", "0" }, cw::Log::Level::trace },      { { "debug", "dbg", "1" }, cw::Log::Level::debug },
    { { "info", "inf", "2" }, cw::Log::Level::info }, { { "warn", "wrn", "3" }, cw::Log::Level::warn },
    { { "error", "err", "4" }, cw::Log::Level::err }, { { "critical", "crit", "5" }, cw::Log::Level::critical }
};

/**
 * \brief Maps log levels to their string descriptors.
 */
const std::unordered_map<cw::Log::Level, std::string> stringLogLevelMap = {
    { cw::Log::Level::trace, "trace" }, { cw::Log::Level::debug, "debug" }, { cw::Log::Level::info, "info" },
    { cw::Log::Level::warn, "warn" },   { cw::Log::Level::err, "error" },   { cw::Log::Level::critical, "critical" }
};
} // namespace

namespace cw {
Log::Level Log::LevelFromString(const std::string& lvl) {
    std::string l = lvl;
    std::transform(l.begin(), l.end(), l.begin(), [](unsigned char c) { return std::tolower(c); });
    std::string availableLevels;
    for (const auto& logLevel : ::logLevelStringMap) {
        if (logLevel.first.contains(l)) { return logLevel.second; }
        for (const auto& str : logLevel.first) { availableLevels += fmt::format("\"{}\", ", str); }
    }
    availableLevels.pop_back();
    availableLevels.pop_back();
    throw std::invalid_argument(
        fmt::format("Invalid log level string \"{}\", use of one these instead: {}", lvl, availableLevels)
    );
}

std::string Log::LevelToString(const Log::Level lvl) noexcept {
    assert(::stringLogLevelMap.contains(lvl));
    return ::stringLogLevelMap.at(lvl);
}

void Log::ExpandFilePattern(std::string& filePattern) {
    for (const auto& var : ::variableMap) {
        filePattern = std::regex_replace(filePattern, std::regex(var.first), var.second());
    }
}

void Log::Setup(
    const std::optional<std::string>& filePattern,
    const bool logHardwareDetails,
    const bool neverProduceDialogBoxes,
    const bool neverWriteTraces
) {
    _neverProduceDialogBoxes = neverProduceDialogBoxes;
    _neverWriteTraces = neverWriteTraces;
    if (filePattern) {
        std::string path = *filePattern;
        ExpandFilePattern(path);
        try {
            _sink->add_sink(std::make_shared<spdlog::sinks::basic_file_sink_st>(path, true));
        } catch (const std::exception& e) {
            if (!_neverProduceDialogBoxes) {
                try {
                    boxer::show(
                        fmt::format("Could not open log file {}\nReason: {}", path, e).c_str(),
                        "Critical Error!",
                        boxer::Style::Error
                    );
                } catch (...) {}
            }
        }
    }
    _sink->add_sink(std::make_shared<spdlog::sinks::ostream_sink_st>(_logCopy));
    _sink->set_pattern("[%Y-%m-%d %T.%e] %v");
    _logger = std::make_unique<spdlog::logger>("logger", _sink);
    _logger->log(spdlog::level::off, "Computer Wars (c) CasualYouTuber31");
    _logger->log(spdlog::level::off, "Branch: {}", GIT_BRANCH);
    _logger->log(spdlog::level::off, "Commit: {}", GIT_COMMIT);
    if (logHardwareDetails) {
        try {
            _logger->log(spdlog::level::off, "Hardware specification:");
            const auto spec = std::make_unique<System::Properties>();
            _logger->log(spdlog::level::off, "     CPU\t{}", spec->CPUModel());
            _logger->log(spdlog::level::off, "  Memory\t{}", spec->RAMTotal());
            _logger->log(spdlog::level::off, "     GPU\t{}", spec->GPUName());
            _logger->log(
                spdlog::level::off,
                " Storage\t{} out of {} is free",
                spec->StorageFree(System::Unit::MB),
                spec->StorageTotal()
            );
            _logger->log(spdlog::level::off, "Platform\t{} ~ {}", spec->OSName(), spec->OSVersion());
        } catch (const std::exception& e) { _logger->log(spdlog::level::off, "Couldn't write the hardware spec: {}", e); }
    }
    _logger->flush();
    _sink->set_pattern("[%Y-%m-%d %T.%e] [%l] %v");
}

void Log::SetLevel(const Log::Level level) {
    assert(_logger);
    _sink->set_level(level);
    _logger->set_level(level);
    Write(__RELATIVE_FILENAME__, __LINE__, level, "Log level has been set to {}", false, false, LevelToString(level));
}

// TODO: CW-5: we will need a way to interweave the AngelScript stacktrace with this one.
void Log::WriteTrace(const Log::Level lvl) {
    assert(_logger);
    if (_neverWriteTraces) { return; }
    auto trace = cpptrace::generate_trace();
    std::stringstream traceOutput;
    bool foundAWriteTraceEntry = false;
    bool checkedForWriteFrame = false;
    for (const auto& entry : trace) {
        // Do not include anything up to and including the first WriteTrace frame.
        if (!foundAWriteTraceEntry) {
            foundAWriteTraceEntry = entry.symbol.find("Log::WriteTrace") != std::string::npos;
            continue;
        }
        // If a Write frame immediately follows from a WriteTrace frame, drop that, too.
        if (!checkedForWriteFrame) {
            checkedForWriteFrame = true;
            if (entry.symbol.find("Log::Write") != std::string::npos) { continue; }
        }
        traceOutput << entry << "\n";
        if (entry.line.has_value()) { traceOutput << cpptrace::get_snippet(entry.filename, entry.line.value(), 3, false); }
        // Once we hit main, leave. No point including low-level platform-dependent entries.
        if (entry.symbol == "main(int, char**)" || entry.symbol == "main") { break; }
        traceOutput << "\n";
    }
    _logger->log(lvl, "Stacktrace:\n\n{}", traceOutput.str());
    _logger->flush();
}

std::string Log::Get() {
    assert(_logger);
    _logger->flush();
    return _logCopy.str();
}

std::shared_ptr<spdlog::sinks::dup_filter_sink_st> Log::_sink =
    std::make_shared<spdlog::sinks::dup_filter_sink_st>(std::chrono::seconds(5));

std::ostringstream Log::_logCopy = std::ostringstream();

std::unique_ptr<spdlog::logger> Log::_logger = nullptr;

bool Log::_neverProduceDialogBoxes = false;

bool Log::_neverWriteTraces = false;
} // namespace cw
