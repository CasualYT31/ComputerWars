/*Copyright 2019-2024 CasualYouTuber31 <naysar@protonmail.com>

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

//#include <filesystem>
//#include "engine.hpp"

#include "bank-v2.hpp"
#include "fmtsfx.hpp"

/**
 * Loads the game engine, then runs it.
 * The first command-line argument should be a path to an assets folder with which
 * to load all of the game's resources. If it's not given, <tt>"./assets"</tt> will
 * be assumed.
 * @param argc The number of command-line arguments given.
 * @param argv The command-line arguments.
 */
int main(int argc, char* argv[]) {
    try {
        // Debugging measure. Since the application runs from within "out", I have
        // to make the current directory match with where "main.cpp" is so that my
        // default assets folder can be accessed by the application. It's also
        // handy to keep the log file in the same folder as the root of my
        // repository.
        std::filesystem::current_path(
            std::filesystem::current_path().parent_path().parent_path().
            parent_path().parent_path()
        );
        // Initialise the sink all loggers output to.
#ifdef COMPUTER_WARS_DEBUG
        std::shared_ptr<engine::sink> sink = std::make_shared<engine::sink>(
            "Computer Wars", "CasualYouTuber31", "", false, nullptr);
#else
        std::shared_ptr<engine::sink> sink = std::make_shared<engine::sink>(
            "Computer Wars", "CasualYouTuber31", "", true,
            std::make_shared<System::Properties>());
#endif
        engine::logger rootLogger({ sink, "main" });

        // DEBUG START.
        std::filesystem::current_path("assets");

        std::shared_ptr<engine::scripts> scripts = std::make_shared<engine::scripts>(rootLogger.getData());

        awe::banks banks(scripts, rootLogger.getData());

        // We need to load the scripts before loading the bank JSON data because
        // the latter relies on the script interface to be able to allocate
        // CScriptArrays.
        scripts->load("test-scripts.json");
        if (!scripts->inGoodState()) {
            throw std::exception("Failed to load scripts!");
        }

        banks.load("properties.json");

        if (!scripts->evaluateAssertions()) {
            throw std::exception("Script assertions failed!");
        }

        banks.processOverrides();

        const auto& weathers = banks.get<awe::weather>();

        rootLogger.write("{} --- {}", weathers["CLEAR"]->longName(), weathers["CLEAR"]->longName(awe::overrides().commander("JAKE")));
        rootLogger.write("{} --- {}", weathers["CLEAR"]->shortName(), static_cast<const awe::bank<awe::weather>>(weathers)["CLEAR"]->shortName(awe::overrides().commander("JAKER")));

        rootLogger.write("{} --- {}",
            weathers["CLEAR"]->particles().vector[3].respawnDelay.asSeconds(),
            weathers["CLEAR"]->particles(awe::overrides().commander("JAKE")).vector[4].vector.x
        );

        const auto& e = banks.get<awe::environment>();
        rootLogger.write("{}", e["NORMAL"]->icon(awe::overrides().weather("CLEAR").commander("JAKE")));
        rootLogger.write("{}", e["NORMAL"]->icon(awe::overrides().weather("CLEARR").commander("JAKE")));
        rootLogger.write("{}", e["NORMAL"]->icon(awe::overrides().weather("CLEAR").commander("JAKER")));
        rootLogger.write("{}", e["NORMAL"]->icon(awe::overrides().weather("").commander("")));
        rootLogger.write("Counter");

        const auto& c = banks.get<awe::country>();
        rootLogger.write("{}", c["ORANGE"]->longName(awe::overrides().environment("NORMAL").weather("CLEAR").commander("JAKE")));
        rootLogger.write("{}", c["ORANGE"]->longName(awe::overrides().environment("NORMAL").weather("CLEARR").commander("JAKE")));
        rootLogger.write("{}", c["ORANGE"]->longName(awe::overrides().environment("NORMAL").weather("CLEAR").commander("JAKER")));
        rootLogger.write("{}", c["ORANGE"]->longName(awe::overrides().environment("NORMAL").weather("CLEARR").commander("JAKER")));
        rootLogger.write("{}", c["ORANGE"]->longName(awe::overrides().environment("NORMALL").weather("CLEARR").commander("JAKE")));
        rootLogger.write("{}", c["ORANGE"]->longName(awe::overrides().environment("NORMALL").weather("CLEAR").commander("JAKER")));
        rootLogger.write("{}", c["ORANGE"]->longName(awe::overrides().environment("NORMALL").weather("CLEAR").commander("JAKE")));
        rootLogger.write("{}", c["ORANGE"]->longName(awe::overrides().environment("").weather("").commander("")));
        rootLogger.write("Counter");

        const auto& s = banks.get<awe::structure>();
        rootLogger.write("Default={}", s["HQ"]->ownedIcon());
        for (const auto& country : c) {
            rootLogger.write("{}={}", country.scriptName(), s["HQ"]->ownedIcon(awe::overrides().country(country.scriptName())));
        }
        return 0;

        //awe::game_engine engine({ sink, "engine" });
        //{
        //    // Find assets folder path from command-line arguments.
        //    std::string assetsFolder = "./assets";
        //    if (argc >= 2) {
        //        assetsFolder = std::string(argv[1]);
        //        rootLogger.write("Assets folder provided: \"{}\".", assetsFolder);
        //    } else {
        //        rootLogger.write("Assets folder not provided in command-line "
        //            "arguments, assuming \"{}\".", assetsFolder);
        //    }
        //    // Find config.json within the assets folder, then load the game engine
        //    // with it.
        //    std::string configPath = assetsFolder + "/config.json";
        //    if (!std::filesystem::exists(configPath)) {
        //        rootLogger.critical("config.json script not found in assets "
        //            "folder \"{}\", aborting.", assetsFolder);
        //        return 2;
        //    } else {
        //        try {
        //            engine.load(configPath);
        //        } catch (const awe::game_engine::load_cancelled&) {
        //            return 5;
        //        }
        //    }
        //}
        //if (engine.inGoodState()) {
        //    const auto r = engine.run();
        //    return r;
        //} else {
        //    rootLogger.critical("Game engine in bad state after loading, "
        //        "aborting...");
        //    return 3;
        //}
    } catch (const std::exception& e) {
        boxer::show(e.what(), "Critical Error!", boxer::Style::Error);
        return 4;
    }
}
