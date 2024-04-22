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

// An example of a POD type in AngelScript.
// Let's make the declaration and definition of these, macros.
// Use param_type from call_traits for the constructors.
    // For AS, primitive types will be pass by value: bools, all ints, all floats.
    // Everything else should be const&in.
// Macros should let you define the public fields.
// This will be most useful for banks, but I think this will be useful for
// awe::map-based types, too.

// Small correction: they aren't *technically* POD types in the C++ sense, but they
// are just fields in a struct pretty much.

// The ultimate aim of this work is to make it super super easy to let objects be
// overrideable via bank-v2. Both read<>() methods can be supported for POD types
// out-of-the-box with no need for specialisation. At least, in theory...

DECLARE_POD_1(, pod, "pod", std::string, message);

DEFINE_POD_1(, pod, "pod", std::string, message);

DECLARE_POD_3(, coords, "Coords", double, x, double, y, double, z);

DEFINE_POD_3(, coords, "Coords", double, x, double, y, double, z);

DECLARE_POD_2(, coord_pair, "CoordPair", coords, a, coords, b);

DEFINE_POD_2(, coord_pair, "CoordPair", coords, a, coords, b);

class reg : public engine::script_registrant {
public:
    void registerInterface(asIScriptEngine* engine,
        const std::shared_ptr<DocumentationGenerator>& document) final;
    engine::logger logger;
};

void reg::registerInterface(asIScriptEngine* engine,
    const std::shared_ptr<DocumentationGenerator>& document) {
    pod::Register(engine, document);
    coords::Register(engine, document);
    coord_pair::Register(engine, document);
    engine->RegisterGlobalFunction("void info(const string&in)", asMETHODPR(engine::logger, write, (const std::string&), void), asCALL_THISCALL_ASGLOBAL, &this->logger);
}

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

        std::shared_ptr<engine::scripts> scripts = std::make_shared<engine::scripts>(rootLogger.getData());

        // TODO: how tf do we provide the scripts pointer to each bank_array specialisation?
        awe::bank_array<awe::particle_data>::scripts = scripts;

        //reg regInterface;
        //regInterface.logger.setData(rootLogger);
        //scripts->addRegistrant(&regInterface);

        //coords simCoords(-0.4789, 51.6704, 0);

        //scripts->callFunction(engine::scripts::modules[engine::scripts::BANK_OVERRIDE], "main", &simCoords);

        awe::bank<awe::commander> commanders(scripts, rootLogger.getData());

        awe::bank<awe::weather> weathers(scripts, rootLogger.getData());

        awe::bank<awe::environment> environments(scripts, rootLogger.getData());

        awe::bank<awe::country> countries(scripts, rootLogger.getData());

        awe::bank<awe::movement_type> movementTypes(scripts, rootLogger.getData());

        scripts->load("assets/test-scripts.json");

        commanders.load("assets/property/co.json");
        weathers.load("assets/property/weather.json");
        environments.load("assets/property/environment.json");
        countries.load("assets/property/country.json");
        movementTypes.load("assets/property/movement.json");

        awe::processOverrides(scripts, commanders);
        awe::processOverrides(scripts, weathers, commanders);
        awe::processOverrides(scripts, environments, weathers, commanders);
        awe::processOverrides(scripts, countries, environments, weathers, commanders);
        awe::processOverrides(scripts, movementTypes, countries, environments, weathers, commanders);

        rootLogger.write("{} --- {}", weathers["CLEAR"]->longName(), weathers["CLEAR"]->longName(awe::overrides().commander("JAKE")));
        rootLogger.write("{} --- {}", weathers["CLEAR"]->shortName(), static_cast<const awe::bank<awe::weather>>(weathers)["CLEAR"]->shortName(awe::overrides().commander("JAKER")));

        rootLogger.write("{} --- {}",
            weathers["CLEAR"]->particles().vector[3].respawnDelay.asSeconds(),
            weathers["CLEAR"]->particles(awe::overrides().commander("JAKE")).vector[4].vector.x
        );

        const auto& e = environments;
        rootLogger.write("{}", e["NORMAL"]->icon(awe::overrides().weather("CLEAR").commander("JAKE")));
        rootLogger.write("{}", e["NORMAL"]->icon(awe::overrides().weather("CLEARR").commander("JAKE")));
        rootLogger.write("{}", e["NORMAL"]->icon(awe::overrides().weather("CLEAR").commander("JAKER")));
        rootLogger.write("{}", e["NORMAL"]->icon(awe::overrides().weather("").commander("")));
        rootLogger.write("Counter");

        const auto& c = countries;
        rootLogger.write("{}", c["ORANGE"]->longName(awe::overrides().environment("NORMAL").weather("CLEAR").commander("JAKE")));
        rootLogger.write("{}", c["ORANGE"]->longName(awe::overrides().environment("NORMAL").weather("CLEARR").commander("JAKE")));
        rootLogger.write("{}", c["ORANGE"]->longName(awe::overrides().environment("NORMAL").weather("CLEAR").commander("JAKER")));
        rootLogger.write("{}", c["ORANGE"]->longName(awe::overrides().environment("NORMAL").weather("CLEARR").commander("JAKER")));
        rootLogger.write("{}", c["ORANGE"]->longName(awe::overrides().environment("NORMALL").weather("CLEARR").commander("JAKE")));
        rootLogger.write("{}", c["ORANGE"]->longName(awe::overrides().environment("NORMALL").weather("CLEAR").commander("JAKER")));
        rootLogger.write("{}", c["ORANGE"]->longName(awe::overrides().environment("NORMALL").weather("CLEAR").commander("JAKE")));
        rootLogger.write("{}", c["ORANGE"]->longName(awe::overrides().environment("").weather("").commander("")));
        rootLogger.write("Counter");

        // TODO:
        //      Keep testing.
        //      

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
