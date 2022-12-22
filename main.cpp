/*Copyright 2019-2022 CasualYouTuber31 <naysar@protonmail.com>

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/**@file main.cpp
 * The entry point into the program.
 */

/**@namespace engine
 * @brief     The \c engine namespace defines game code that isn't specific to
 *            Computer Wars.
 * @details   Code within this namespace utilises dependencies defined within
 *            non-<tt>awe</tt> namespaces to define game-based classes that aren't
 *            specific to Computer Wars. These could include script, GUI, and
 *            dialogue management classes.
 */

/**@namespace sfx
 * @brief     The \c sfx namespace contains components which build upon existing
 *            SFML classes.
 */

/**@namespace awe
 * @brief     The \c awe namespace contains Computer Wars-specific code.
 */

/**@namespace transition
 * @brief     The \c transition namespace contains all the transition drawables.
 */

#include "engine.hpp"
#include <filesystem>

/**
 * Loads the game engine, then runs it.
 * The first command-line argument should be a path to an assets folder with which
 * to load all of the game's resources. If it's not given, <tt>"./assets"</tt> will
 * be assumed.
 * @param argc The number of command-line arguments given.
 * @param argv The command-line arguments.
 */
int main(int argc, char* argv[]) {
    // Debugging measure. Since the application runs from within "out", I have to
    // make the current directory match with where "main.cpp" is so that my default
    // assets folder can be accessed by the application. It's also handy to keep
    // the log file in the same folder as the root of my repository.
    std::filesystem::current_path(
        std::filesystem::current_path().parent_path().parent_path().parent_path().
        parent_path()
    );
    // Initialise the sink all loggers output to.
#ifdef COMPUTER_WARS_DEBUG
    engine::sink::Get("Computer Wars", "CasualYouTuber31", ".", false, false);
#else
    engine::sink::Get("Computer Wars", "CasualYouTuber31", ".");
#endif
    engine::logger rootLogger("main");
    // Allocate the game engine.
    awe::game_engine engine;
    // Load the game engine within a scope in order to get rid of temporary objects
    // before running the engine.
    {
        // Find assets folder path from command-line arguments.
        std::string assetsFolder = "./assets";
        if (argc >= 2) {
            assetsFolder = std::string(argv[1]);
            rootLogger.write("Assets folder provided: \"{}\".", assetsFolder);
        } else {
            rootLogger.write("Assets folder not provided in command-line "
                "arguments, assuming \"{}\".", assetsFolder);
        }

        // Find config.json within the assets folder, then load the game engine
        // with it.
        std::string configPath = assetsFolder + "/config.json";
        if (!std::filesystem::exists(configPath)) {
            rootLogger.critical("config.json script not found in assets folder "
                "\"{}\", aborting.", assetsFolder);
            return 2;
        } else {
            engine.load(configPath);
        }
    }
    // Run the game, but only if the engine is in a good state.
    if (engine.inGoodState()) {
        return engine.run();
    } else {
        rootLogger.error("Game engine in bad state after loading, aborting...");
        return 3;
    }
}
