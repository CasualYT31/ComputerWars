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

/* Separate idea: create a template type which has a shared_ptr and identifier variable?
This pairing seems like it will be very common and if we have to change the way things are identified we can change it in one place (namely this class),
instead of throughout the code */

/**@file main.cpp
 * The entry point into the program.
 * Some basic initialisation occurs before handing control over to the sole awe::game_engine object.
 * See the documentation on \c main() for more information.
 */

#include "engine.h"

/**
 * The entry point into the program.
 * Some basic game initialisation occurs here: the global sink is opened (which is the file all loggers output to), and the \c awe::game_engine object is constructed.
 * Much more of the game's initialisation occurs in \c awe::game_engine's constructor.
 * @return The result of calling \c awe::game_engine::run(): by this point, the game has been shut down.
 */
int main() {
    // create the sink all loggers output to
    global::sink::Get("Computer Wars", "CasualYouTuber31", "assets/log", false);
    // initialise game loop
    awe::game_engine gameLoop;
    // run game loop, then destroy the object once the loop terminates
    return gameLoop.run();
}