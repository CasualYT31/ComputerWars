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

// #include "bank.h"

// #include "game.h"

#include "dialogue.h"
#include "fonts.h"

#include <iostream>

/**
 * The entry point into the program.
 * Some basic game initialisation occurs here: the global sink is opened (which is the file all loggers output to), and the awe::game object is constructed.
 * Much more of the game's initialisation occurs in awe::game's constructor.
 * @return The result of calling awe::game::run(): by this point, the game has been shut down.
 */
int main() {
    // create the sink all loggers output to
    global::sink::Get("Computer Wars", "CasualYouTuber31", "assets/log", false);
    
    sfx::renderer newRenderer;
    newRenderer.load("assets/renderer/renderer.json");
    // example of overriding
    sfx::renderer_settings settings = newRenderer.getSettings();
    settings.style.mouseGrabbed = false;
    newRenderer.setSettings(settings);

    // animated sprite testing
    std::shared_ptr<sfx::animated_spritesheet> sheet = std::make_shared<sfx::animated_spritesheet>();
    sheet->load("./assets/sprites/tile/normal/spritestilenormal.json");
    sfx::animated_sprite sprite(sheet, 0);
    sfx::animated_sprite sprite2(sheet, 0);

    // dialogue box testing
    sfx::fonts fonts;
    fonts.load("assets/fonts/fonts.json");
    awe::dialogue_box box;
    box.setPosition(awe::dialogue_box_position::Bottom);
    box.setTransitionSpeed(200.0f);
    box.setBackgroundColour(sf::Color(150,150,150));
    box.setThemeColour(sf::Color::Red);
    box.setMainText("Testing\nSecond Line");
    box.setNameText("Tank");
    box.setOptions("Option1", "", "Option2");
    try {
        box.setFont(fonts["dialogue"]);
    } catch (std::out_of_range& e) {
        std::cout << "Font error! " << e.what() << std::endl;
    }
    box.setSprite(sheet, 15);

    bool leave = false;
    bool selectCurrentOption = false;
    while (!leave) {
        sf::Event event;
        while (newRenderer.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                leave = true;
            } else if (event.type == sf::Event::KeyReleased) {
                if (event.key.code == sf::Keyboard::Escape) {
                    leave = true;
                } else if (event.key.code == sf::Keyboard::Up) {
                    sprite.setSprite(sprite.getSprite() + 1);
                } else if (event.key.code == sf::Keyboard::Down) {
                    sprite.setSprite(sprite.getSprite() - 1);
                } else if (event.key.code == sf::Keyboard::Left) {
                    sprite--;
                } else if (event.key.code == sf::Keyboard::Right) {
                    sprite++;
                } else if (event.key.code == sf::Keyboard::Z) {
                    selectCurrentOption = true;
                }
            }
        }
        newRenderer.clear(sf::Color::Black);
        newRenderer.animate(sprite);
        newRenderer.animate(sprite2);
        newRenderer.animate(box);
        if (selectCurrentOption) box.selectCurrentOption();
        newRenderer.draw(sprite);
        newRenderer.draw(sprite2, sf::RenderStates(sf::Transform().translate(50.0, 50.0)));
        newRenderer.draw(box);
        newRenderer.display();
        selectCurrentOption = false;
    }

    newRenderer.save();
    newRenderer.close();

    /* // initialise game loop
    awe::game gameLoop;
    // run game loop, then destroy the object once the loop terminates
    return gameLoop.run(); */
}