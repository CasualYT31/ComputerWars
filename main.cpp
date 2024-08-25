#include "file/File.hpp"
#include "log/Log.hpp"
#include "mvc/Controller.hpp"

#include <boost/program_options.hpp>
#include <boxer/boxer.h>
#include <chrono>
#include <iostream>
#include <thread>

/**
 * \namespace cw
 * \brief Contains all of the Computer Wars code.
 */

/**
 * \brief Used to parse a log level option.
 */
struct LogLevelOption {
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
 * \brief Streams a log level option to a given output stream.
 */
static inline std::ostream& operator<<(std::ostream& os, const LogLevelOption& v) {
    return os << cw::Log::LevelToString(v.level);
}

/**
 * \brief Validates a log level option.
 */
void validate(boost::any& v, const std::vector<std::string>& values, LogLevelOption*, int) {
    std::string l = boost::program_options::validators::get_single_string(values);
    try {
        v = boost::any(LogLevelOption(cw::Log::LevelFromString(l)));
    } catch (const std::exception& e) {
        std::cout << e.what() << "\n";
        throw boost::program_options::validation_error(boost::program_options::validation_error::invalid_option_value);
    }
}

/**
 * \brief Extract options from the command line.
 * \param argc The number of command-line arguments to parse.
 * \param argv The command-line arguments to parse.
 * \param vm The variables map to fill in with the options provided by the user.
 * \returns cw::ShutdownCode::Success if the program should continue, another value if it should shutdown.
 * \throws std::exception if the command-line arguments could not be parsed.
 */
int parseCommandLine(const int argc, char* const argv[], boost::program_options::variables_map& vm) {
    boost::program_options::options_description opts("Available options");
#ifdef COMPUTER_WARS_DEBUG
    const auto defaultLog = "assets/log/Log.log";
    const auto defaultLogLevel = cw::Log::Level::trace;
#else
    const auto defaultLog = "assets/log/Log %DATE%.log";
    const auto defaultLogLevel = cw::Log::Level::info;
#endif
    // clang-format off
    opts.add_options()
        ("help", "produce help message")
        ("log", boost::program_options::value<std::string>()->default_value(defaultLog), "tell the game where to write the log file")
        ("log-level", boost::program_options::value<::LogLevelOption>()->default_value(LogLevelOption(defaultLogLevel)), "set the log level")
        ("log-no-hardware-details", "prevents the game from logging hardware details")
        ("core-config", boost::program_options::value<std::string>()->default_value("assets/core.json"), "specify the location of the core configuration file");
    // clang-format on
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, opts), vm);
    boost::program_options::notify(vm);
    if (vm.count("help")) {
        std::cout << opts << "\n";
        return cw::ShutdownCode::DisplayedHelp;
    }
    return cw::ShutdownCode::Success;
}

int main(int argc, char* argv[]) {
    // Parse the command-line.
    boost::program_options::variables_map vm;
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
            cw::Log::Setup(logFilePattern, !vm.contains("log-no-hardware-details"));
            cw::Log::SetLevel(vm["log-level"].as<LogLevelOption>().level);
        } catch (const std::exception& e) {
            try {
                boxer::show(fmt::format("Could not setup the logs: {}", e).c_str(), "Critical Error!", boxer::Style::Error);
            } catch (...) {}
            return cw::ShutdownCode::CriticalLoggingError;
        }
    }

    try {
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

        LOG(debug, "Constructing controller hierarchy");
        std::shared_ptr<cw::ControllerNode> root = std::make_shared<cw::Controller>();

        LOG(debug, "Configuring controller hierarchy");
        root->fromJSON(config);

        int ret = cw::Continue;
        LOG(debug, "Entering game loop");
        for (; ret <= cw::Continue; ret = root->tick()) { std::this_thread::sleep_for(std::chrono::milliseconds(100)); }
        return ret;
    } catch (const cw::AssertionError&) { return cw::ShutdownCode::AssertionFailed; }
}
