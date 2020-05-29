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

#include "bank.h"

#include "game.h"

int main() {
    // create the sink all loggers output to
    global::sink::Get("Computer Wars", "CasualYouTuber31", "assets/log", false);
    // initialise game loop
    awe::game gameLoop;
    // run game loop, then destroy the object once the loop terminates
    return gameLoop.run();
}