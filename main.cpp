#include "file/File.hpp"
#include "log/Log.hpp"
#include "mvc/Controller.hpp"
#include "script/angelscript/AngelScriptEngine.hpp"
#include "script/ScriptModel.hpp"

#include <boxer/boxer.h>
#include <chrono>
#include <cxxopts.hpp>
#include <filesystem>
#include <iostream>
#include <thread>

/**
 * \namespace cw
 * \brief Contains all of the Computer Wars code.
 */

/**
 * \namespace cw::model
 * \brief Defines all of the "components" of the game engine.
 */

/**
 * \namespace cw::request
 * \brief Defines all of the requests that can be made to the game engine.
 */

/**
 * \brief Used to parse a log level option.
 */
struct LogLevelOption {
    /**
     * \brief Default initialise the level field.
     * \details Debug builds default this to trace, whilst release builds default this to info.
     */
    LogLevelOption()
        :
#ifdef COMPUTER_WARS_DEBUG
          level(cw::Log::Level::trace)
#else
          level(cw::Log::Level::info)
#endif
    {
    }

    /**
     * \brief Initialise the level field.
     * \param l The log level to initialise it with.
     */
    LogLevelOption(const cw::Log::Level l) : level(l) {}

    /**
     * \brief The log level chosen by the user.
     */
    cw::Log::Level level;
};

/**
 * \brief Streams a log level string from a given input stream.
 */
static std::istream& operator>>(std::istream& is, LogLevelOption& v) {
    std::string str;
    std::getline(is, str);
    v.level = cw::Log::LevelFromString(str);
    return is;
}

/**
 * \brief Streams a log level option to a given output stream.
 */
static inline std::ostream& operator<<(std::ostream& os, const LogLevelOption& v) {
    return os << cw::Log::LevelToString(v.level);
}

/**
 * \brief Extract options from the command line.
 * \details If the log and log level arguments are not give by the user, they'll be set to assets/log/Log.log and trace in
 * debug builds, and assets/log/Log %DATE%.log and info in release builds.
 * \param argc The number of command-line arguments to parse.
 * \param argv The command-line arguments to parse.
 * \param vm The variables map to fill in with the options provided by the user.
 * \returns cw::ShutdownCode::Success if the program should continue, another value if it should shutdown.
 * \throws std::exception if the command-line arguments could not be parsed.
 */
int parseCommandLine(const int argc, char* const argv[], cxxopts::ParseResult& vm) {
#ifdef _WIN32
    const auto applicationName = "ComputerWars.exe";
#else
    const auto applicationName = "ComputerWars";
#endif
#ifdef COMPUTER_WARS_DEBUG
    const auto defaultLog = "assets/log/Log.log";
    const auto defaultLogLevel = cw::Log::LevelToString(cw::Log::Level::trace);
#else
    const auto defaultLog = "assets/log/Log %DATE%.log";
    const auto defaultLogLevel = cw::Log::LevelToString(cw::Log::Level::info);
#endif
    cxxopts::Options opts(applicationName, "Logging and core configuration options");
    // clang-format off
    opts.add_options()
        ("h,help", "produce help message")
        ("f,log", "tell the game where to write the log file", cxxopts::value<std::string>()->default_value(defaultLog))
        ("l,log-level", "set the log level", cxxopts::value<::LogLevelOption>()->default_value(defaultLogLevel))
        ("log-no-hardware-details", "prevents the game from logging hardware details")
        ("c,core-config", "specify the location of the core configuration file", cxxopts::value<std::string>()->default_value("assets/core.json"))
        ("d,write-documentation-to", "if this parameter is given, the engine's script interface documentation will be written to the given file, then the game will close. "
                                     "The generated file will be in the HTML format", cxxopts::value<std::string>());
    // clang-format on
    vm = opts.parse(argc, argv);
    if (vm.count("help")) {
        std::cout << opts.help() << "\n";
        return cw::ShutdownCode::DisplayedHelp;
    }
    return cw::ShutdownCode::Success;
}

int main(int argc, char* argv[]) {
    // Parse the command-line.
    cxxopts::ParseResult vm;
    try {
        const auto code = parseCommandLine(argc, argv, vm);
        if (code != cw::ShutdownCode::Success) { return code; }
    } catch (const std::exception& e) {
        std::cout << "Could not parse command-line arguments: " << e.what() << "\n";
        return cw::ShutdownCode::InvalidOptions;
    }

    // Setup logging.
    {
        const auto logFilePattern = vm["log"].as<std::string>();
        try {
            cw::Log::Setup(logFilePattern, !vm.count("log-no-hardware-details"));
            cw::Log::SetLevel(vm["log-level"].as<LogLevelOption>().level);
        } catch (const std::exception& e) {
            try {
                boxer::show(fmt::format("Could not setup the logs: {}", e).c_str(), "Critical Error!", boxer::Style::Error);
            } catch (...) {}
            return cw::ShutdownCode::CriticalLoggingError;
        }
    }

    std::string scriptInterfaceDocumentationOutputFile;
    if (vm.count("write-documentation-to")) {
        scriptInterfaceDocumentationOutputFile = vm["write-documentation-to"].as<std::string>();
        LOG(info, "Will write script interface documentation to \"{}\"", scriptInterfaceDocumentationOutputFile);
    } else {
        LOG(debug, "Script interface documentation will not be written");
    }

    try {
        LOG(debug, "Constructing controller hierarchy");
        std::shared_ptr<cw::ControllerNode> root = std::make_shared<cw::Controller>();
        root->attachModel(
            "scripts", std::make_shared<cw::model::Script<cw::AngelScriptEngine>>(scriptInterfaceDocumentationOutputFile)
        );

        // If the user wants the script interface documentation, generate it, then exit early.
        if (!scriptInterfaceDocumentationOutputFile.empty()) {
            LOG(info, "Requesting the generation of the script interface documentation");
            const bool success = REQUEST(root, cw::request::GenerateDocumentation, ());
            const std::string fmtString = success ? "Script interface documentation has been written to \"{}\", exiting..."
                                                  : "Failed to write script interface documentation to \"{}\", exiting...";
            const auto message = fmt::format(fmt::runtime(fmtString), scriptInterfaceDocumentationOutputFile);
            if (success) {
                LOG(info, message);
            } else {
                LOG(err, message);
            }
            std::cout << message << "\n";
            return success ? cw::ShutdownCode::GeneratedScriptInterfaceDocumentation
                           : cw::ShutdownCode::FailedToGenerateScriptInterfaceDocumentation;
        }

        const auto corePath = vm["core-config"].as<std::string>();
        LOG(info, "Parsing core configuration file {}", corePath);
        cw::json config;
        try {
            const auto core = cw::makeExceptionFStream<std::ifstream>(corePath);
            config = cw::json::parse(*core);
        } catch (const std::exception& e) {
            LOG(critical, "Could not parse core configuration file: {}", e);
            return cw::ShutdownCode::InvalidCoreConfigurationFile;
        }

        const auto newCWD = std::filesystem::absolute(corePath).parent_path();
        LOG(debug, "Setting the current working directory to {}", newCWD);
        std::filesystem::current_path(newCWD);

        LOG(debug, "Configuring controller hierarchy");
        root->fromJSON(config);

        int ret = cw::Continue;
        LOG(debug, "Entering game loop");
        for (; ret <= cw::Continue; ret = root->tick()) { std::this_thread::sleep_for(std::chrono::milliseconds(100)); }
        return ret;
    } catch (const cw::AssertionError&) { return cw::ShutdownCode::AssertionFailed; }
}
