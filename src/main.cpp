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

/* Tracker
texture.h: changed unsigned ints to SpriteKey and FrameIndex
safejson.h: added applyVector()
script.h & gui.h: moved classes into engine namespace and out of awe namespace

I SHOULD LOOK THROUGH MY NEW CODE TO SEE IF I ALWAYS CHECK FOR NULL POINTERS!
*/

/**@file main.cpp
 * The entry point into the program.
 * Some basic initialisation occurs before handing control over to the sole awe::game object.
 * See the documentation on main() for more information.
 */

#include "language.h"

#include <iostream>

int main() {
    global::sink::Get("Computer Wars", "CasualYouTuber31", "assets/log", false);
    i18n::language_dictionary translate;
    translate.addLanguage("ENG_GB", "./assets/lang/GB_test.json");
    translate.addLanguage("DE_DEU", "./assets/lang/DE_test.json");
    translate.load("./assets/lang/lang.json");
    std::cout << translate("directly print string without translation - current language is \"\"\n");
    translate.setLanguage("ENG_GB");
    std::cout << translate("0");
    std::cout << translate("1", translate("2"));
    std::cout << translate("title");
    translate.setLanguage("test");
    std::cout << translate("0");
    translate.setLanguage("DE_DEU");
    std::cout << translate("0");
    std::cout << translate("1", translate("2"));
    std::cout << translate.getLanguage();
    translate.removeLanguage("DE_DEU");
    translate.setLanguage("ENG_GB");
    translate.removeLanguage("DE_DEU");
    translate.removeLanguage("non");
    translate.addLanguage("testing", "non-existent script path");
    std::cout << translate("invalid key");
    translate.save("./assets/lang/lang.json");
}

/*

#include "bank.h"

#include "game.h"

/**
 * The entry point into the program.
 * Some basic game initialisation occurs here: the global sink is opened (which is the file all loggers output to), and the awe::game object is constructed.
 * Much more of the game's initialisation occurs in awe::game's constructor.
 * @return The result of calling awe::game::run(): by this point, the game has been shut down.
 *
int main() {
    // create the sink all loggers output to
    global::sink::Get("Computer Wars", "CasualYouTuber31", "assets/log", false);
    // initialise game loop
    awe::game gameLoop;
    // run game loop, then destroy the object once the loop terminates
    return gameLoop.run();
}

*/