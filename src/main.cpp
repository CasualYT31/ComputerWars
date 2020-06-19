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
    std::cout << i18n::expand_string::insert("Test with no variables.\n");
    std::cout << i18n::expand_string::insert("Test with the right number of variables: #.\n", "this is the first and only variable");
    std::cout << i18n::expand_string::insert("Test with less variables: #.\n");
    std::cout << i18n::expand_string::insert("Test with more variables: #.\n", 32);
    i18n::expand_string::setVarChar('%');
    std::cout << i18n::expand_string::insert("Test double var chars (only works if variables are given) %% and setVarChar and getVarChar.\n", -7);
    std::cout << i18n::expand_string::getVarChar() << std::endl;
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