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
 * Most of the initialisation occurs before handing control over to the sole
 * \c awe::game_engine object. See the documentation on \c main() for more
 * information.
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

#include "awe/engine.h"
// for debugging measure
#include <filesystem>

/**
 * The entry point into the program.
 * A majority of the game initialisation occurs here: the global sink is opened
 * (which is the file all loggers output to), and the \c awe::game_engine object is
 * constructed.
 * @param  file Temporary parameter storing a path to a binary map file to open.
 * @return The result of calling \c awe::game_engine::run(): by this point, the
 *         game has been shut down.
 */
int game(const std::string& file) {
    // initialise the sink all loggers output to
    engine::sink::Get("Computer Wars", "CasualYouTuber31", "assets/log", false,
        true);

    // initialise the language dictionary
    std::shared_ptr<engine::language_dictionary> dictionary =
        std::make_shared<engine::language_dictionary>();
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
    // opening the renderer now will prevent glFlush() SFML errors from plaguing
    // standard output when I load images in the animated_spritesheet objects below
    renderer->openWindow();

    // initialise the user input
    std::shared_ptr<sfx::user_input> userInput =
        std::make_shared<sfx::user_input>();
    userInput->tieWindow(renderer);
    userInput->load("assets/userinput/userinput.json");

    // initialise spritesheets
    std::shared_ptr<awe::spritesheets> spritesheets =
        std::make_shared<awe::spritesheets>();
    spritesheets->CO = std::make_shared<sfx::animated_spritesheet>();
    spritesheets->CO->load("assets/sprites/co/spritesco.json");
    spritesheets->unit = std::make_shared<awe::spritesheets::units>();
    spritesheets->unit->idle = std::make_shared<sfx::animated_spritesheet>();
    spritesheets->unit->idle->load(
        "assets/sprites/unit/idle/spritesunitidle.json");
    spritesheets->tile = std::make_shared<awe::spritesheets::tiles>();
    spritesheets->tile->normal = std::make_shared<sfx::animated_spritesheet>();
    spritesheets->tile->normal->load(
        "assets/sprites/tile/normal/spritestilenormal.json");
    spritesheets->unitPicture = std::make_shared<sfx::animated_spritesheet>();
    spritesheets->unitPicture->load(
        "assets/sprites/unit_picture/spritesunit_picture.json");
    spritesheets->tilePicture =
        std::make_shared<awe::spritesheets::tile_pictures>();
    spritesheets->tilePicture->normal =
        std::make_shared<sfx::animated_spritesheet>();
    spritesheets->tilePicture->normal->load(
        "assets/sprites/tile_picture/normal/spritestile_picturenormal.json");
    spritesheets->icon = std::make_shared<sfx::animated_spritesheet>();
    spritesheets->icon->load("assets/sprites/icon/spritesicon.json");
    spritesheets->GUI = std::make_shared<sfx::animated_spritesheet>();
    spritesheets->GUI->load("assets/sprites/gui/spritesgui.json");

    // initialise the script engine, but let game_engine register the interface and
    // load the script files
    std::shared_ptr<engine::scripts> scripts = std::make_shared<engine::scripts>();

    // initialise the GUI
    // let game_engine perform gui->load()
    // this is because we can't call the SetUp() functions for menus before
    // game_engine has initialised the script interface and loaded the script files
    std::shared_ptr<sfx::gui> gui = std::make_shared<sfx::gui>(scripts);

    // initialise the countries
    std::shared_ptr<awe::bank<awe::country>> countries =
        std::make_shared<awe::bank<awe::country>>();
    countries->load("assets/property/country.json");

    // initialise the weathers
    std::shared_ptr<awe::bank<awe::weather>> weathers =
        std::make_shared<awe::bank<awe::weather>>();
    weathers->load("assets/property/weather.json");

    // initialise the environments
    std::shared_ptr<awe::bank<awe::environment>> environments =
        std::make_shared<awe::bank<awe::environment>>();
    environments->load("assets/property/environment.json");

    // initialise the movements
    std::shared_ptr<awe::bank<awe::movement_type>> movements =
        std::make_shared<awe::bank<awe::movement_type>>();
    movements->load("assets/property/movement.json");

    // initialise the terrains
    std::shared_ptr<awe::bank<awe::terrain>> terrains =
        std::make_shared<awe::bank<awe::terrain>>();
    terrains->load("assets/property/terrain.json");

    // initialise the tiles
    std::shared_ptr<awe::bank<awe::tile_type>> tiles =
        std::make_shared<awe::bank<awe::tile_type>>();
    tiles->load("assets/property/tile.json");

    // initialise the units
    std::shared_ptr<awe::bank<awe::unit_type>> units =
        std::make_shared<awe::bank<awe::unit_type>>();
    units->load("assets/property/unit.json");

    // initialise the COs
    std::shared_ptr<awe::bank<awe::commander>> commanders =
        std::make_shared<awe::bank<awe::commander>>();
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
    gameLoop.setSpritesheets(spritesheets);
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
    gameLoop.initialiseScripts("assets/script");

    // now load gui
    gui->load("assets/gui/gui.json");

    // run game loop, then destroy the object once the loop terminates
    return gameLoop.run(file);
}

/**
 * Calls either \c game() or \c test::test().
 * The second command-line argument contains the path to a binary map file to open.
 * This will be a temporary measure: the command-line arguments will be reworked or
 * ignored in the future.
 */
int main(int argc, char* argv[]) {
    // debugging measure
    std::filesystem::current_path(
        std::filesystem::current_path().parent_path().parent_path().parent_path().
            parent_path()
    );
    return game(((argc < 2) ? ("assets/map/islandx.cwm") : (argv[1])));
}