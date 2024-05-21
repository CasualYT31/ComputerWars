#include "file/file.hpp"
#include "mvc/Controller.hpp"

#include <chrono>
#include <iostream>
#include <thread>

/**
 * \namespace cw
 * \brief Contains all of the Computer Wars code.
 */

int main(int argc, char* argv[]) {
    // 1. Find core configuration file.
    std::string corePath("assets/core.json");
    if (argc > 1) {
        // LOG: Using explicitly given core file.
        corePath = argv[1];
    }

    // 2. Load core configuration file.
    // LOG: Loading core file.
    cw::json config;
    try {
        const auto core = cw::makeExceptionFStream<std::ifstream>(corePath);
        config = cw::json::parse(*core);
    } catch (const std::exception& e) {
        // LOG: Couldn't load core config file.
        return 2;
    }

    // 3. Construct the controller hierarchy.
    std::shared_ptr<cw::ControllerNode> root = std::make_shared<cw::Controller>();

    // 4. Load controller hierarchy using core configuration file.
    root->fromJSON(config);

    // 5. Run the game loop.
    int ret = cw::Continue;
    std::cout << "Looping...\n";
    for (; ret <= cw::Continue; ret = root->tick()) { std::this_thread::sleep_for(std::chrono::milliseconds(100)); }
    return ret;
}
