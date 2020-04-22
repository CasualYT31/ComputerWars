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
Updating the documentation a little has helped me identify a lot of flaws with the current system:
1. scripts and gui are placed within the awe namespace despite not being
   specific to the engine, they should be put in their own namespace.
2. currently no way to define country-specific unit sprites or pictures:
   only a single sprite key is provided.
3. while tiles have the ability to define different country-based graphics,
   the picture does not.
4. there is no clear vision on how each component should marry together.
   There is a lot of pointer passing with no cohesive standard. For example,
   it appeared before the update to the terrain and unit classes that they
   would have their own draw methods (why else would they accept a spritesheet
   object pointer?), while the GUI class does not.
5. the bank classes have a lot of similiarity: they should really be consolidated
   into a single class somehow. This would help a lot with treating units and
   tiles such as the pipe seam uniformally.
6. I attempted to do this with the property classes but it doesn't work extremely
   well. Many derived classes such as weather don't really need the colour
   property, but country is practically the only one that does. unsigned ints are
   used for IDs which means we have to explicitly check if they are valid all the
   time without doing this implicitly. It would be better to have a type which could
   act as an ID which is bound to a given property object. Some properties that use
   awe::property directly are classes, such as environment, while others are structs,
   such as awe::terrain.
7. there is currently no easy way to hot reload JSON configurations. In addition to
   this, awe::game::game() dictates what scripts to load and doesn't leave this to
   the client: BAD FOR REUSABILITY, even if there shouldn't really be more than one
   game object in any instance of the game application (the hot reloading should be
   very easy to implement, though).
8. some properties such as HP do not make any sense when they are negative: if they
   are negative, they should be set to 0. Since this is the case, it should not be
   treated like an int internally but accept an int in the set() method so that
   the engine does not have to bother about HP being negative and the terrain/unit
   class can fix it. Max fuel and Max ammo being negative could resemble infinite
   fuel and ammo respectively: infinite fuel will definitely become useful for the
   Oozium unit.
9. protip: whenever an unsigned int or int is used, try to replace them with a typedef
   which better describes their purpose. SpriteKey would be better than unsigned int,
   for example, and purhaps we can replace the int HP value with a simple HP class,
   which preserves the unsignedness of the value without the client caring about
   wrapping or other negative side effects.
These are just some of the flaws with what I have right now.
Since changing all of this will create too much to backtrack from in the documentation,
I have decided to leave the documentation update for now to focus on the correct
architecture for the game engine
*/

#include "game.h"

int main() {
    // create the sink all loggers output to
    global::sink::Get("Computer Wars", "CasualYouTuber31", "assets/log", false);
    // initialise game loop
    awe::game gameLoop;
    // run game loop, then destroy the object once the loop terminates
    return gameLoop.run();
}