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

// To make this as seamless as possible, I should provide a standard method via which
// a programmer can define the core AngelScript datatype (without qualifiers) for each
// C++ type. I should use the template<> approach I've come up with in bank-v2.hpp,
// but the code for that should be in fmtXYZ.hpp-like headers (you may need to forward
// declare your custom types in those headers).

template<typename T>
constexpr const char* angel_script() {
    static_assert(false, "You must specialise engine::script_type<T>()");
}

template<>
constexpr const char* angel_script<std::string>() {
    return "string";
}

// And then with that approach, have an angel_script_param_type template function that
// either returns "const TYPE" for primitive types or "const TYPE&in" for other types.

struct pod : public engine::script_reference_type<pod> {
    static void Register(asIScriptEngine* engine,
        const std::shared_ptr<DocumentationGenerator>& document);
    inline static pod* Create() { return new pod(); }
    inline static pod* Create(const std::string& m) { return new pod(m); };
    inline static pod* Create(const pod* const o) { return new pod(*o); };
    pod() = default;
    pod(const std::string& i);
    pod(const pod& o);
    pod(pod&& o) noexcept;
    bool operator==(const pod& o);
    inline bool operator!=(const pod& o) { return !(*this == o); }
    pod& operator=(const pod& o);
    pod& operator=(pod&& o) noexcept;
    std::string message;
private:
    inline pod* opAssign(const pod* const o) { return &(*this = *o); }
    inline bool opEquals(const pod* const o) { return *this == *o; }
};

void pod::Register(asIScriptEngine* engine,
    const std::shared_ptr<DocumentationGenerator>& document) {
    if (engine->GetTypeInfoByName("pod")) return;
    auto r = RegisterType(engine, "pod",
        [](asIScriptEngine* engine, const std::string& type) {
            engine->RegisterObjectBehaviour(type.c_str(), asBEHAVE_FACTORY,
                std::string(type + "@ f()").c_str(),
                asFUNCTIONPR(pod::Create, (), pod*), asCALL_CDECL);
            engine->RegisterObjectBehaviour(type.c_str(), asBEHAVE_FACTORY,
                std::string(type + "@ f(const string&in)").c_str(),
                asFUNCTIONPR(pod::Create, (const std::string&), pod*), asCALL_CDECL);
            engine->RegisterObjectBehaviour(type.c_str(), asBEHAVE_FACTORY,
                std::string(type + "@ f(const pod&in)").c_str(),
                asFUNCTIONPR(pod::Create, (const pod* const), pod*), asCALL_CDECL);
        });
    document->DocumentObjectType(r, "POD data type.");
    r = engine->RegisterObjectProperty("pod", "string message",
        asOFFSET(pod, message));

    r = engine->RegisterObjectMethod("pod", "pod@ opAssign(const pod&in)",
        asMETHOD(pod, opAssign), asCALL_THISCALL);
    r = engine->RegisterObjectMethod("pod", "bool opEquals(const pod&in)",
        asMETHOD(pod, opEquals), asCALL_THISCALL);
}

pod::pod(const std::string& i) : message(i) {}

pod::pod(const pod& o) : message(o.message) {}

pod::pod(pod&& o) noexcept : message(std::move(o.message)) {}

bool pod::operator==(const pod& o) {
    return message == o.message;
}

pod& pod::operator=(const pod& o) {
    message = o.message;
    return *this;
}

pod& pod::operator=(pod&& o) noexcept {
    message = std::move(o.message);
    return *this;
}

class reg : public engine::script_registrant {
public:
    void registerInterface(asIScriptEngine* engine,
        const std::shared_ptr<DocumentationGenerator>& document) final;
    engine::logger logger;
};

void reg::registerInterface(asIScriptEngine* engine,
    const std::shared_ptr<DocumentationGenerator>& document) {
    pod::Register(engine, document);
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
    angel_script<std::string>();
    // Fails with static_assert:
    //angel_script<int>();
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
        reg regInterface;
        regInterface.logger.setData(rootLogger);
        scripts->addRegistrant(&regInterface);
        scripts->load("assets/test-scripts.json");

        scripts->callFunction(engine::scripts::modules[engine::scripts::BANK_OVERRIDE], "main");

        //awe::bank<awe::commander> commanders(nullptr, rootLogger.getData());
        //commanders.load("assets/property/co.json");

        //awe::bank<awe::weather> weathers(nullptr, rootLogger.getData());
        //weathers.load("assets/property/weather.json");

        //awe::bank<awe::environment> environments(nullptr, rootLogger.getData());
        //environments.load("assets/property/environment.json");

        //awe::bank<awe::country> countries(nullptr, rootLogger.getData());
        //countries.load("assets/property/country.json");

        //awe::bank<awe::movement_type> movementTypes(nullptr, rootLogger.getData());
        //movementTypes.load("assets/property/movement.json");

        //awe::processOverrides(scripts, commanders);
        //awe::processOverrides(scripts, weathers, commanders);
        //awe::processOverrides(scripts, environments, weathers, commanders);
        //awe::processOverrides(scripts, countries, environments, weathers, commanders);
        //awe::processOverrides(scripts, movementTypes, countries, environments, weathers, commanders);

        //rootLogger.write("{} --- {}", weathers["CLEAR"]->longName(), weathers["CLEAR"]->longName(awe::overrides().commander("JAKE")));
        //rootLogger.write("{} --- {}", weathers["CLEAR"]->shortName(), static_cast<const awe::bank<awe::weather>>(weathers)["CLEAR"]->shortName(awe::overrides().commander("JAKER")));

        //const auto& e = environments;
        //rootLogger.write("{}", e["NORMAL"]->icon(awe::overrides().weather("CLEAR").commander("JAKE")));
        //rootLogger.write("{}", e["NORMAL"]->icon(awe::overrides().weather("CLEARR").commander("JAKE")));
        //rootLogger.write("{}", e["NORMAL"]->icon(awe::overrides().weather("CLEAR").commander("JAKER")));
        //rootLogger.write("{}", e["NORMAL"]->icon(awe::overrides().weather("").commander("")));
        //rootLogger.write("Counter");

        //const auto& c = countries;
        //rootLogger.write("{}", c["ORANGE"]->longName(awe::overrides().environment("NORMAL").weather("CLEAR").commander("JAKE")));
        //rootLogger.write("{}", c["ORANGE"]->longName(awe::overrides().environment("NORMAL").weather("CLEARR").commander("JAKE")));
        //rootLogger.write("{}", c["ORANGE"]->longName(awe::overrides().environment("NORMAL").weather("CLEAR").commander("JAKER")));
        //rootLogger.write("{}", c["ORANGE"]->longName(awe::overrides().environment("NORMAL").weather("CLEARR").commander("JAKER")));
        //rootLogger.write("{}", c["ORANGE"]->longName(awe::overrides().environment("NORMALL").weather("CLEARR").commander("JAKE")));
        //rootLogger.write("{}", c["ORANGE"]->longName(awe::overrides().environment("NORMALL").weather("CLEAR").commander("JAKER")));
        //rootLogger.write("{}", c["ORANGE"]->longName(awe::overrides().environment("NORMALL").weather("CLEAR").commander("JAKE")));
        //rootLogger.write("{}", c["ORANGE"]->longName(awe::overrides().environment("").weather("").commander("")));
        //rootLogger.write("Counter");

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
