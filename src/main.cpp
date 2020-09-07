/*Copyright 2020 CasualYouTuber31 <naysar@protonmail.com>

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

/* Project - Overarching Goals
1. I SHOULD LOOK THROUGH MY CODE TO SEE IF I ALWAYS CHECK FOR NULL POINTERS!
2. I should refactor my template solutions to ensure as little redundant duplication as possible.
3. JSON backend has been developed a lot since I started using it: unordered_json support has been added, as well as the ability to serialise/deserialise user-defined types.
   These features must be reviewed to see if they're useful for me, and if they are, I can think about radically changing the way safejson works for the better.
4. I should favour smart pointer objects over raw pointers. I also need to ensure I use the right type of pointer class throughout my code.
*/

/**@file main.cpp
 * The entry point into the program.
 * Most of the initialisation occurs before handing control over to the sole awe::game_engine object.
 * See the documentation on \c main() for more information.
 */

#include "engine.h"
#include <iostream>
#include "file.h"

int main() {
    engine::binary_file file;
    try {
        file.open("test.bin", false);
        file.writeNumber<sf::Int32>(1297564416);
        file.writeBool(true);
        file.writeString("Hello World!");
        file.close();

        file.open("test.bin", true);
        std::cout << file.readBool() << '\n';
        std::cout << file.readNumber<sf::Int32>() << '\n';
        std::cout << file.readString() << '\n';
    } catch (std::exception& e) {
        std::cout << "an error occurred: " << e.what() << '\n';
    }
}

/**
 * The entry point into the program.
 * A majority of the game initialisation occurs here: the global sink is opened (which is the file all loggers output to), and the \c awe::game_engine object is constructed.
 * @return The result of calling \c awe::game_engine::run(): by this point, the game has been shut down.
 */
int main_old() {
    // initialise the sink all loggers output to
    global::sink::Get("Computer Wars", "CasualYouTuber31", "assets/log", false);

    // initialise the language dictionary
    std::shared_ptr<i18n::language_dictionary> dictionary = std::make_shared<i18n::language_dictionary>();
    dictionary->load("assets/lang/lang.json");

    // initialise the fonts
    std::shared_ptr<sfx::fonts> fonts = std::make_shared<sfx::fonts>();
    fonts->load("assets/fonts/fonts.json");

    // initialise the sounds
    std::shared_ptr<sfx::audio> sounds = std::make_shared<sfx::audio>();
    sounds->load("assets/audio/sound/audiosound.json");

    // initialise the BGM
    std::shared_ptr<sfx::audio> music = std::make_shared<sfx::audio>();
    music->load("assets/audio/music/audiomusic.json");

    // initialise the renderer
    std::shared_ptr<sfx::renderer> renderer = std::make_shared<sfx::renderer>();
    renderer->load("assets/renderer/renderer.json");

    // initialise the user input
    std::shared_ptr<sfx::user_input> userInput = std::make_shared<sfx::user_input>(*renderer);
    userInput->load("assets/userinput/userinput.json");

    // initialise the scripts
    std::shared_ptr<engine::scripts> scripts = std::make_shared<engine::scripts>("assets/script/");

    // initialise the GUI
    std::shared_ptr<engine::gui> gui = std::make_shared<engine::gui>(scripts);
    gui->load("assets/gui/gui.json");

    // initialise the countries
    std::shared_ptr<awe::bank<const awe::country>> countries = std::make_shared<awe::bank<const awe::country>>();
    countries->load("assets/property/country.json");

    // initialise the weathers
    std::shared_ptr<awe::bank<const awe::weather>> weathers = std::make_shared<awe::bank<const awe::weather>>();
    weathers->load("assets/property/weather.json");

    // initialise the environments
    std::shared_ptr<awe::bank<const awe::environment>> environments = std::make_shared<awe::bank<const awe::environment>>();
    environments->load("assets/property/environment.json");

    // initialise the movements
    std::shared_ptr<awe::bank<const awe::movement_type>> movements = std::make_shared<awe::bank<const awe::movement_type>>();
    movements->load("assets/property/movement.json");

    // initialise the terrains
    std::shared_ptr<awe::bank<const awe::terrain>> terrains = std::make_shared<awe::bank<const awe::terrain>>();
    terrains->load("assets/property/terrain.json");

    // initialise the tiles
    std::shared_ptr<awe::bank<const awe::tile_type>> tiles = std::make_shared<awe::bank<const awe::tile_type>>();
    tiles->load("assets/property/tile.json");

    // initialise the units
    std::shared_ptr<awe::bank<const awe::unit_type>> units = std::make_shared<awe::bank<const awe::unit_type>>();
    units->load("assets/property/unit.json");

    // initialise the COs
    std::shared_ptr<awe::bank<const awe::commander>> commanders = std::make_shared<awe::bank<const awe::commander>>();
    commanders->load("assets/property/co.json");

    // setup banks
    awe::updateAllTerrains(*tiles, *terrains);
    awe::updateAllMovementsAndLoadedUnits(*units, *movements);

    // initialise game engine
    awe::game_engine gameLoop;
    gameLoop.setDictionary(dictionary);
    gameLoop.setFonts(fonts);
    gameLoop.setSounds(sounds);
    gameLoop.setMusic(music);
    gameLoop.setRenderer(renderer);
    gameLoop.setUserInput(userInput);
    gameLoop.setScripts(scripts);
    gameLoop.setGUI(gui);
    gameLoop.setCountries(countries);
    gameLoop.setWeathers(weathers);
    gameLoop.setEnvironments(environments);
    gameLoop.setMovements(movements);
    gameLoop.setTerrains(terrains);
    gameLoop.setTiles(tiles);
    gameLoop.setUnits(units);
    gameLoop.setCommanders(commanders);

    // open renderer window
    renderer->openWindow();

    // run game loop, then destroy the object once the loop terminates
    return gameLoop.run();
}