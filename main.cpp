#include "mvc/RootController.hpp"

#include <chrono>
#include <thread>

/**
 * \namespace cw
 * \brief Contains all of the Computer Wars code.
 */

int main() {
    std::unique_ptr<cw::RootControllerNode> root = std::make_unique<cw::RootController>();
    int ret = cw::Continue;
    for (; ret <= cw::Continue; ret = root->tick()) { std::this_thread::sleep_for(std::chrono::milliseconds(100)); }
    return ret;
}
